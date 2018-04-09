#include "eeprom_driver.h"

/*! \使用IO寄存器写一个字节到EEPROM
 *
 *  This function writes one byte to EEPROM using IO-mapped access.
 *  如果内存映射使能，本函数将不起作用
 *  本函数将取消所有正在进行的EEPROM 页缓存加载操作
 *
 *  \参数  pageAddr  EEPROM 页地址,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 *  \参数  byteAddr  EEPROM 字节地址, 0 < byteAddr < EEPROM_PAGESIZE
 *  \参数  value     写到EEPROM的字节数据
 */
void EEPROM_WriteByte( uint8_t pageAddr, uint8_t byteAddr, uint8_t value )
{
	// 清空缓存防止意外数据写入
	EEPROM_FlushBuffer();
	NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;
	//计算地址
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE)
	                            |(byteAddr & (EEPROM_PAGESIZE-1));
	//写地址
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//加载数据触发命令执行
	NVM.DATA0 = value;

	//触发原子操作（擦&写）写签名，执行命令
	NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	NVM_EXEC();
}


/*! \使用IO寄存器读EEPROM一个字节
 *
 *  如果内存映射使能，本函数将不起作用
 *
 *  \参数  pageAddr  EEPROM 页地址,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 *  \参数  byteAddr  EEPROM 字节地址, 0 < byteAddr < EEPROM_PAGESIZE
 *
 *  \return  读取的字节数据
 */
uint8_t EEPROM_ReadByte( uint8_t pageAddr, uint8_t byteAddr )
{
	//等待NVM不忙
	EEPROM_WaitForNVM();
	//计算地址
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE)
	                            |(byteAddr & (EEPROM_PAGESIZE-1));
	//写地址
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//执行读命令
	NVM.CMD = NVM_CMD_READ_EEPROM_gc;
	NVM_EXEC();

	return NVM.DATA0;
}


/*! \等待NVM访问结束，包括EEPROM.
 *
 *  在任何EEPROM 访问时，如果不确定之前的操作是否完成，需要等待
 */
void EEPROM_WaitForNVM( void )
{
	do {} while ((NVM.STATUS & NVM_NVMBUSY_bm) == NVM_NVMBUSY_bm);
}


/*! \清空EEPROM页缓存
 *
 *  将取消所有正在进行的EEPROM 页缓存加载操作，内存映射中也可使用
 */
void EEPROM_FlushBuffer( void )
{
	//等待NVM不忙
	EEPROM_WaitForNVM();

	//清空页缓存
	if ((NVM.STATUS & NVM_EELOAD_bm) != 0) {
		NVM.CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
		NVM_EXEC();
	}
}


/*! \加载单字节到临时页缓存
 *
 *  如果内存映射使能，本函数将不起作用
 *  确保在加载字节之前需要清空缓存
 *  同时, 如果多个字节加载到同一个位置, 他们将进行与(AND)操作
 *
 *  \注意 只有一个页缓存, 因此只有一个页可以加载数据，数据编程到一个页中
 *        如果需要写入不同的页中，加载和写操作需要重复执行
 *
 *  \参数  byteAddr  EEPROM 字节地址, 0 < byteAddr < EEPROM_PAGESIZE
 *  \参数  value     加载到缓存的字节数据
 */
void EEPROM_LoadByte( uint8_t byteAddr, uint8_t value )
{
	//等待NVM不忙，准备NVM命令
	EEPROM_WaitForNVM();
	NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

	//写地址
	NVM.ADDR0 = byteAddr & 0xFF;
	NVM.ADDR1 = 0x00;
	NVM.ADDR2 = 0x00;

	//写数据，执行命令
	NVM.DATA0 = value;
}


/*! \从SRAM缓存加载一整页数据到临时页缓存中
 *  如果内存映射使能，本函数将不起作用
 *  确保在加载字节之前需要清空缓存
 *
 *  \注意 只有地址的低几位用来对缓存寻址因此不需要地址作为参数 *
 *  \参数  values   SRAM 缓存指针
 */
void EEPROM_LoadPage( const uint8_t * values )
{
	//等待NVM不忙
	EEPROM_WaitForNVM();
	NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

	//地址清零，只有低几位使用，再循环内会改变
	NVM.ADDR1 = 0x00;
	NVM.ADDR2 = 0x00;

	//加载多个字节到缓存
	for (uint8_t i = 0; i < EEPROM_PAGESIZE; ++i) {
		NVM.ADDR0 = i;
		NVM.DATA0 = *values;
		++values;
	}
}

/*! \写已经加载好数据的页缓存到EEPROM.
 *
 *  原子操作，在写数据之前会自动擦除，注意页缓存中加载数据的位置才会被写入到EEPROM.
 *  页缓存中没有写入数据的位置，不会对EEPROM数据产生操作（好强大啊）
 *
 *  \参数  pageAddr  EEPROM 页地址,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 */
void EEPROM_AtomicWritePage( uint8_t pageAddr )
{
	//等待NVM不忙
	EEPROM_WaitForNVM();

	//计算页地址
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE);

	//写地址
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//触发EEPROM 原子写操作 (擦&写)命令
	NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	NVM_EXEC();
}


/*! \擦除EEPROM页，所有数据变为0xFF.
 *
 *  \参数  pageAddr  EEPROM 页地址,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 */
void EEPROM_ErasePage( uint8_t pageAddr )
{
	//等待NVM不忙
	EEPROM_WaitForNVM();

	//计算页地址
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE);

	//写地址
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//触发EEPROM擦除命令
	NVM.CMD = NVM_CMD_ERASE_EEPROM_PAGE_gc;
	NVM_EXEC();
}


/*! \写EEPROM页（分离操作，写之前不擦除）
 *
 *  \参数  pageAddr  EEPROM 页地址,  0 < pageAddr < EEPROM_SIZE/EEPROM_PAGESIZE
 */
void EEPROM_SplitWritePage( uint8_t pageAddr )
{
	//等待NVM不忙
	EEPROM_WaitForNVM();

	//计算页地址
	uint16_t address = (uint16_t)(pageAddr*EEPROM_PAGESIZE);

	//写地址
	NVM.ADDR0 = address & 0xFF;
	NVM.ADDR1 = (address >> 8) & 0x1F;
	NVM.ADDR2 = 0x00;

	//触发写EEPROM页命令
	NVM.CMD = NVM_CMD_WRITE_EEPROM_PAGE_gc;
	NVM_EXEC();
}

/*! \擦除整个EEPROM
 *
 */
void EEPROM_EraseAll( void )
{
	//等待NVM不忙
	EEPROM_WaitForNVM();

	//触发擦除整个EEPROM命令
	NVM.CMD = NVM_CMD_ERASE_EEPROM_gc;
	NVM_EXEC();
}

