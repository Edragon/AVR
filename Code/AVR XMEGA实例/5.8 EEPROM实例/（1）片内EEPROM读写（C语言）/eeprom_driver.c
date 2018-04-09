#include "eeprom_driver.h"

/*! \ʹ��IO�Ĵ���дһ���ֽڵ�EEPROM
 *
 *  This function writes one byte to EEPROM using IO-mapped access.
 *  ����ڴ�ӳ��ʹ�ܣ�����������������
 *  ��������ȡ���������ڽ��е�EEPROM ҳ������ز���
 *
 *  \����  pageAddr  EEPROM ҳ��ַ,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 *  \����  byteAddr  EEPROM �ֽڵ�ַ, 0 < byteAddr < EEPROM_PAGESIZE
 *  \����  value     д��EEPROM���ֽ�����
 */
void EEPROM_WriteByte( uint8_t pageAddr, uint8_t byteAddr, uint8_t value )
{
	// ��ջ����ֹ��������д��
	EEPROM_FlushBuffer();
	NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;
	//�����ַ
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE)
	                            |(byteAddr & (EEPROM_PAGESIZE-1));
	//д��ַ
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//�������ݴ�������ִ��
	NVM.DATA0 = value;

	//����ԭ�Ӳ�������&д��дǩ����ִ������
	NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	NVM_EXEC();
}


/*! \ʹ��IO�Ĵ�����EEPROMһ���ֽ�
 *
 *  ����ڴ�ӳ��ʹ�ܣ�����������������
 *
 *  \����  pageAddr  EEPROM ҳ��ַ,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 *  \����  byteAddr  EEPROM �ֽڵ�ַ, 0 < byteAddr < EEPROM_PAGESIZE
 *
 *  \return  ��ȡ���ֽ�����
 */
uint8_t EEPROM_ReadByte( uint8_t pageAddr, uint8_t byteAddr )
{
	//�ȴ�NVM��æ
	EEPROM_WaitForNVM();
	//�����ַ
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE)
	                            |(byteAddr & (EEPROM_PAGESIZE-1));
	//д��ַ
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//ִ�ж�����
	NVM.CMD = NVM_CMD_READ_EEPROM_gc;
	NVM_EXEC();

	return NVM.DATA0;
}


/*! \�ȴ�NVM���ʽ���������EEPROM.
 *
 *  ���κ�EEPROM ����ʱ�������ȷ��֮ǰ�Ĳ����Ƿ���ɣ���Ҫ�ȴ�
 */
void EEPROM_WaitForNVM( void )
{
	do {} while ((NVM.STATUS & NVM_NVMBUSY_bm) == NVM_NVMBUSY_bm);
}


/*! \���EEPROMҳ����
 *
 *  ��ȡ���������ڽ��е�EEPROM ҳ������ز������ڴ�ӳ����Ҳ��ʹ��
 */
void EEPROM_FlushBuffer( void )
{
	//�ȴ�NVM��æ
	EEPROM_WaitForNVM();

	//���ҳ����
	if ((NVM.STATUS & NVM_EELOAD_bm) != 0) {
		NVM.CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
		NVM_EXEC();
	}
}


/*! \���ص��ֽڵ���ʱҳ����
 *
 *  ����ڴ�ӳ��ʹ�ܣ�����������������
 *  ȷ���ڼ����ֽ�֮ǰ��Ҫ��ջ���
 *  ͬʱ, �������ֽڼ��ص�ͬһ��λ��, ���ǽ�������(AND)����
 *
 *  \ע�� ֻ��һ��ҳ����, ���ֻ��һ��ҳ���Լ������ݣ����ݱ�̵�һ��ҳ��
 *        �����Ҫд�벻ͬ��ҳ�У����غ�д������Ҫ�ظ�ִ��
 *
 *  \����  byteAddr  EEPROM �ֽڵ�ַ, 0 < byteAddr < EEPROM_PAGESIZE
 *  \����  value     ���ص�������ֽ�����
 */
void EEPROM_LoadByte( uint8_t byteAddr, uint8_t value )
{
	//�ȴ�NVM��æ��׼��NVM����
	EEPROM_WaitForNVM();
	NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

	//д��ַ
	NVM.ADDR0 = byteAddr & 0xFF;
	NVM.ADDR1 = 0x00;
	NVM.ADDR2 = 0x00;

	//д���ݣ�ִ������
	NVM.DATA0 = value;
}


/*! \��SRAM�������һ��ҳ���ݵ���ʱҳ������
 *  ����ڴ�ӳ��ʹ�ܣ�����������������
 *  ȷ���ڼ����ֽ�֮ǰ��Ҫ��ջ���
 *
 *  \ע�� ֻ�е�ַ�ĵͼ�λ�����Ի���Ѱַ��˲���Ҫ��ַ��Ϊ���� *
 *  \����  values   SRAM ����ָ��
 */
void EEPROM_LoadPage( const uint8_t * values )
{
	//�ȴ�NVM��æ
	EEPROM_WaitForNVM();
	NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

	//��ַ���㣬ֻ�еͼ�λʹ�ã���ѭ���ڻ�ı�
	NVM.ADDR1 = 0x00;
	NVM.ADDR2 = 0x00;

	//���ض���ֽڵ�����
	for (uint8_t i = 0; i < EEPROM_PAGESIZE; ++i) {
		NVM.ADDR0 = i;
		NVM.DATA0 = *values;
		++values;
	}
}

/*! \д�Ѿ����غ����ݵ�ҳ���浽EEPROM.
 *
 *  ԭ�Ӳ�������д����֮ǰ���Զ�������ע��ҳ�����м������ݵ�λ�òŻᱻд�뵽EEPROM.
 *  ҳ������û��д�����ݵ�λ�ã������EEPROM���ݲ�����������ǿ�󰡣�
 *
 *  \����  pageAddr  EEPROM ҳ��ַ,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 */
void EEPROM_AtomicWritePage( uint8_t pageAddr )
{
	//�ȴ�NVM��æ
	EEPROM_WaitForNVM();

	//����ҳ��ַ
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE);

	//д��ַ
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//����EEPROM ԭ��д���� (��&д)����
	NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	NVM_EXEC();
}


/*! \����EEPROMҳ���������ݱ�Ϊ0xFF.
 *
 *  \����  pageAddr  EEPROM ҳ��ַ,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 */
void EEPROM_ErasePage( uint8_t pageAddr )
{
	//�ȴ�NVM��æ
	EEPROM_WaitForNVM();

	//����ҳ��ַ
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE);

	//д��ַ
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//����EEPROM��������
	NVM.CMD = NVM_CMD_ERASE_EEPROM_PAGE_gc;
	NVM_EXEC();
}


/*! \дEEPROMҳ�����������д֮ǰ��������
 *
 *  \����  pageAddr  EEPROM ҳ��ַ,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 */
void EEPROM_SplitWritePage( uint8_t pageAddr )
{
	//�ȴ�NVM��æ
	EEPROM_WaitForNVM();

	//����ҳ��ַ
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE);

	//д��ַ
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//����дEEPROMҳ����
	NVM.CMD = NVM_CMD_WRITE_EEPROM_PAGE_gc;
	NVM_EXEC();
}

/*! \��������EEPROM
 *
 */
void EEPROM_EraseAll( void )
{
	//�ȴ�NVM��æ
	EEPROM_WaitForNVM();

	//������������EEPROM����
	NVM.CMD = NVM_CMD_ERASE_EEPROM_gc;
	NVM_EXEC();
}

