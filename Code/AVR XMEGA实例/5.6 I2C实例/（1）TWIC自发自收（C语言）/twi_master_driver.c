#include "twi_master_driver.h"


/*! \TWI ������ʼ��
 *  ʹ�ܶ�д�ж�
 *  ������Ҫ��ȫ���ж�
 *  \���� twi                      TWI ����ʵ��
 *  \���� module                   Ҫʹ�õ�TWIģ�� TWIC\TWID\TWIE\TWIF��
 *  \���� intLevel                 TWI �����жϼ���
 *  \���� baudRateRegisterSetting  �����ʼĴ�����ֵ
 */
void TWI_MasterInit(TWI_Master_t *twi,
                    TWI_t *module,
                    TWI_MASTER_INTLVL_t intLevel,
                    uint8_t baudRateRegisterSetting)
{
	twi->interface = module;
	twi->interface->MASTER.CTRLA = intLevel |
	                               TWI_MASTER_RIEN_bm |
	                               TWI_MASTER_WIEN_bm |
	                               TWI_MASTER_ENABLE_bm;
	twi->interface->MASTER.BAUD = baudRateRegisterSetting;
	twi->interface->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}


/*! \TWI ����״̬
 *  unknown, idle, owner or busy.
 *  \���� twi    TWI ����ʵ��
 *  \���� TWI_MASTER_BUSSTATE_UNKNOWN_gc ����״̬��δ֪
 *  \���� TWI_MASTER_BUSSTATE_IDLE_gc    ����״̬������
 *  \���� TWI_MASTER_BUSSTATE_OWNER_gc   ����״̬������ռ��
 *  \���� TWI_MASTER_BUSSTATE_BUSY_gc    ����״̬��æ
 */
TWI_MASTER_BUSSTATE_t TWI_MasterState(TWI_Master_t *twi)
{
	TWI_MASTER_BUSSTATE_t twi_status;
	twi_status = (TWI_MASTER_BUSSTATE_t) (twi->interface->MASTER.STATUS &
	                                      TWI_MASTER_BUSSTATE_gm);
	return twi_status;
}


/*! \�������׼���õĻ�������
 *  \���� twi   TWI ����ʵ��
 *  \���� true  ����ɹ�
 *  \���� false ����ʧ��
 */
bool TWI_MasterReady(TWI_Master_t *twi)
{
	bool twi_status = (twi->status & TWIM_STATUS_READY);
	return twi_status;
}


/*! \TWI д
 *  \���� twi	       TWI ����ʵ��
 *  \���� address      �ӻ���ַ.
 *  \���� writeData    Ҫд�����ݵ�ָ��
 *  \���� bytesToWrite Ҫд�����ݵ��ֽ���
 *  \���� true  	   ����ɹ�		
 *  \���� false 	   ����ʧ��
 */
bool TWI_MasterWrite(TWI_Master_t *twi,
                     uint8_t address,
                     uint8_t *writeData,
                     uint8_t bytesToWrite)
{
	bool twi_status = TWI_MasterWriteRead(twi, address, writeData, bytesToWrite, 0);
	return twi_status;
}


/*! \TWI ��
 *  \���� twi            TWI ����ʵ��
 *  \���� address        The �ӻ���ַ.
 *  \���� bytesToRead    Ҫ�������ݵ��ֽ���
 *
 *  \���� true  ����ɹ�	
 *  \���� false ����ʧ��
 */
bool TWI_MasterRead(TWI_Master_t *twi,
                    uint8_t address,
                    uint8_t bytesToRead)
{
	bool twi_status = TWI_MasterWriteRead(twi, address, 0, 0, bytesToRead);
	return twi_status;
}


/*! \TWI���� ����д ����д
 *
 *  \���� twi            TWI ����ʵ��
 *  \���� address        The �ӻ���ַ.
 *  \���� writeData      Ҫд�����ݵ�ָ��
 *  \���� bytesToWrite   Ҫд�����ݵ��ֽ���
 *  \���� bytesToRead    Ҫ�������ݵ��ֽ���
 *
 *  \���� true  ����ɹ�	
 *  \���� false ����ʧ��
 */
bool TWI_MasterWriteRead(TWI_Master_t *twi,
                         uint8_t address,
                         uint8_t *writeData,
                         uint8_t bytesToWrite,
                         uint8_t bytesToRead)
{
	//�������
	if (bytesToWrite > TWIM_WRITE_BUFFER_SIZE) {
		return false;
	}
	if (bytesToRead > TWIM_READ_BUFFER_SIZE) {
		return false;
	}

	//�������׼���ã���ʼ������
	if (twi->status == TWIM_STATUS_READY)
	{

		twi->status = TWIM_STATUS_BUSY;
		twi->result = TWIM_RESULT_UNKNOWN;

		twi->address = address<<1;

		//��仺�棬�������λ��twiʵ���ڵĳ�Ա����ʵû�б�Ҫ��ռ���ڴ�ռ�
		for (uint8_t bufferIndex=0; bufferIndex < bytesToWrite; bufferIndex++) {
			twi->writeData[bufferIndex] = writeData[bufferIndex];
		}

		twi->bytesToWrite = bytesToWrite;
		twi->bytesToRead = bytesToRead;
		twi->bytesWritten = 0;
		twi->bytesRead = 0;

		// ����bytesToWrite�ж��Ƕ�����д, ����START�ź� + 7λ��ַ + R/_W = 0	 
		if (twi->bytesToWrite > 0) {
			uint8_t writeAddress = twi->address & ~0x01;
			twi->interface->MASTER.ADDR = writeAddress;
		}
		// ����bytesToWrite�ж��Ƕ�����д, ����START�ź� + 7λ��ַ + R/_W = 1
		else if (twi->bytesToRead > 0) {
			uint8_t readAddress = twi->address | 0x01;
			twi->interface->MASTER.ADDR = readAddress;
		}
		return true;
	} 
	else 
	{
		return false;
	}
}


/*! \TWI �����жϷ������
 *  ��鵱ǰ״̬������ͬ����
 *  \���� twi  TWI ����ʵ��
 */
void TWI_MasterInterruptHandler(TWI_Master_t *twi)
{
	uint8_t currentStatus = twi->interface->MASTER.STATUS;

	//�ٲö�ʧ�������ߴ���
	if ((currentStatus & TWI_MASTER_ARBLOST_bm) ||
	    (currentStatus & TWI_MASTER_BUSERR_bm)) {

		TWI_MasterArbitrationLostBusErrorHandler(twi);
	}
	//д�ж�
	else if (currentStatus & TWI_MASTER_WIF_bm) {
		TWI_MasterWriteHandler(twi);
	}
	//���ж�
	else if (currentStatus & TWI_MASTER_RIF_bm) {
		TWI_MasterReadHandler(twi);
	}
	//δ֪״̬
	else {
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_FAIL);
	}
}


/*! \ TWI �����ٲö�ʧ�������ߴ���
 *  \���� twi  TWI ����ʵ��
 */
void TWI_MasterArbitrationLostBusErrorHandler(TWI_Master_t *twi)
{
	uint8_t currentStatus = twi->interface->MASTER.STATUS;

	//���ߴ���
	if (currentStatus & TWI_MASTER_BUSERR_bm) {
		twi->result = TWIM_RESULT_BUS_ERROR;
	}
	//�ٲö�ʧ
	else {
		twi->result = TWIM_RESULT_ARBITRATION_LOST;
	}

	//����жϱ�־
	twi->interface->MASTER.STATUS = currentStatus | TWI_MASTER_ARBLOST_bm;

	twi->status = TWIM_STATUS_READY;
}


/*! \ TWI ����д �ж�
 *  ����д��������Ӧ(N)ACK.
 *  \���� twi   TWI ����ʵ��
 */
void TWI_MasterWriteHandler(TWI_Master_t *twi)
{
	uint8_t bytesToWrite  = twi->bytesToWrite;
	uint8_t bytesToRead   = twi->bytesToRead;

	//�ӻ�NACK��ȡ������
	if (twi->interface->MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi->result = TWIM_RESULT_NACK_RECEIVED;
		twi->status = TWIM_STATUS_READY;
	}
	//������
	else if (twi->bytesWritten < bytesToWrite) {
		uint8_t data = twi->writeData[twi->bytesWritten];
		twi->interface->MASTER.DATA = data;
		++twi->bytesWritten;
	}
	// �������Ҫ�����ݣ�д�����ٶ��� ����START�ź� + 7λ��ַ + R/_W = 1
	//ʣ�µĽ��� ���ж�
	else if (twi->bytesRead < bytesToRead) {
		uint8_t readAddress = twi->address | 0x01;
		twi->interface->MASTER.ADDR = readAddress;
	}
	//������ɷ���STOP�źţ�����RESULT OK
	else {
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_OK);
	}
}


/*! \ TWI �������ж�
 *  \���� twi TWI ����ʵ��
 */
void TWI_MasterReadHandler(TWI_Master_t *twi)
{
	//ȡ����
	if (twi->bytesRead < TWIM_READ_BUFFER_SIZE) {
		uint8_t data = twi->interface->MASTER.DATA;
		twi->readData[twi->bytesRead] = data;
		twi->bytesRead++;
	}
	//���ݻ������������RESULT OVERFLOW
	else {
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_BUFFER_OVERFLOW);
	}
	uint8_t bytesToRead = twi->bytesToRead;

	//�����������Ҫ�� ����ACK ��ʼ��һ���ֽ�
	if (twi->bytesRead < bytesToRead) {
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
	}
	//���������� ����NACK �� STOP �ź� 
	else {
		twi->interface->MASTER.CTRLC = TWI_MASTER_ACKACT_bm |
		                               TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_OK);
	}
}


/*! \ TWI ������ϴ���
 *
 *  ����ģ���ڵĳ�Ա��Ϊ�´�����׼��
 *
 *  \���� twi     TWI ����ʵ��
 *  \���� result  �������
 */
void TWI_MasterTransactionFinished(TWI_Master_t *twi, uint8_t result)
{
	twi->result = result;
	twi->status = TWIM_STATUS_READY;
}
