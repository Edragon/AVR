#include "twi_master_driver.h"


/*! \TWI 主机初始化
 *  使能读写中断
 *  主程序要打开全局中断
 *  \参数 twi                      TWI 主机实例
 *  \参数 module                   要使用的TWI模块 TWIC\TWID\TWIE\TWIF等
 *  \参数 intLevel                 TWI 主机中断级别
 *  \参数 baudRateRegisterSetting  波特率寄存器的值
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


/*! \TWI 总线状态
 *  unknown, idle, owner or busy.
 *  \参数 twi    TWI 主机实例
 *  \返回 TWI_MASTER_BUSSTATE_UNKNOWN_gc 总线状态：未知
 *  \返回 TWI_MASTER_BUSSTATE_IDLE_gc    总线状态：空闲
 *  \返回 TWI_MASTER_BUSSTATE_OWNER_gc   总线状态：主机占用
 *  \返回 TWI_MASTER_BUSSTATE_BUSY_gc    总线状态：忙
 */
TWI_MASTER_BUSSTATE_t TWI_MasterState(TWI_Master_t *twi)
{
	TWI_MASTER_BUSSTATE_t twi_status;
	twi_status = (TWI_MASTER_BUSSTATE_t) (twi->interface->MASTER.STATUS &
	                                      TWI_MASTER_BUSSTATE_gm);
	return twi_status;
}


/*! \如果传输准备好的话返回真
 *  \参数 twi   TWI 主机实例
 *  \返回 true  传输成功
 *  \返回 false 传输失败
 */
bool TWI_MasterReady(TWI_Master_t *twi)
{
	bool twi_status = (twi->status & TWIM_STATUS_READY);
	return twi_status;
}


/*! \TWI 写
 *  \参数 twi	       TWI 主机实例
 *  \参数 address      从机地址.
 *  \参数 writeData    要写的数据的指针
 *  \参数 bytesToWrite 要写的数据的字节数
 *  \返回 true  	   传输成功		
 *  \返回 false 	   传输失败
 */
bool TWI_MasterWrite(TWI_Master_t *twi,
                     uint8_t address,
                     uint8_t *writeData,
                     uint8_t bytesToWrite)
{
	bool twi_status = TWI_MasterWriteRead(twi, address, writeData, bytesToWrite, 0);
	return twi_status;
}


/*! \TWI 读
 *  \参数 twi            TWI 主机实例
 *  \参数 address        The 从机地址.
 *  \参数 bytesToRead    要读的数据的字节数
 *
 *  \返回 true  传输成功	
 *  \返回 false 传输失败
 */
bool TWI_MasterRead(TWI_Master_t *twi,
                    uint8_t address,
                    uint8_t bytesToRead)
{
	bool twi_status = TWI_MasterWriteRead(twi, address, 0, 0, bytesToRead);
	return twi_status;
}


/*! \TWI主机 读和写 读或写
 *
 *  \参数 twi            TWI 主机实例
 *  \参数 address        The 从机地址.
 *  \参数 writeData      要写的数据的指针
 *  \参数 bytesToWrite   要写的数据的字节数
 *  \参数 bytesToRead    要读的数据的字节数
 *
 *  \返回 true  传输成功	
 *  \返回 false 传输失败
 */
bool TWI_MasterWriteRead(TWI_Master_t *twi,
                         uint8_t address,
                         uint8_t *writeData,
                         uint8_t bytesToWrite,
                         uint8_t bytesToRead)
{
	//参数检查
	if (bytesToWrite > TWIM_WRITE_BUFFER_SIZE) {
		return false;
	}
	if (bytesToRead > TWIM_READ_BUFFER_SIZE) {
		return false;
	}

	//如果总线准备好，初始化传输
	if (twi->status == TWIM_STATUS_READY)
	{

		twi->status = TWIM_STATUS_BUSY;
		twi->result = TWIM_RESULT_UNKNOWN;

		twi->address = address<<1;

		//填充缓存，这个缓存位于twi实例内的成员，其实没有必要，占用内存空间
		for (uint8_t bufferIndex=0; bufferIndex < bytesToWrite; bufferIndex++) {
			twi->writeData[bufferIndex] = writeData[bufferIndex];
		}

		twi->bytesToWrite = bytesToWrite;
		twi->bytesToRead = bytesToRead;
		twi->bytesWritten = 0;
		twi->bytesRead = 0;

		// 根据bytesToWrite判断是读还是写, 发送START信号 + 7位地址 + R/_W = 0	 
		if (twi->bytesToWrite > 0) {
			uint8_t writeAddress = twi->address & ~0x01;
			twi->interface->MASTER.ADDR = writeAddress;
		}
		// 根据bytesToWrite判断是读还是写, 发送START信号 + 7位地址 + R/_W = 1
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


/*! \TWI 主机中断服务程序
 *  检查当前状态，做不同处理
 *  \参数 twi  TWI 主机实例
 */
void TWI_MasterInterruptHandler(TWI_Master_t *twi)
{
	uint8_t currentStatus = twi->interface->MASTER.STATUS;

	//仲裁丢失或者总线错误
	if ((currentStatus & TWI_MASTER_ARBLOST_bm) ||
	    (currentStatus & TWI_MASTER_BUSERR_bm)) {

		TWI_MasterArbitrationLostBusErrorHandler(twi);
	}
	//写中断
	else if (currentStatus & TWI_MASTER_WIF_bm) {
		TWI_MasterWriteHandler(twi);
	}
	//读中断
	else if (currentStatus & TWI_MASTER_RIF_bm) {
		TWI_MasterReadHandler(twi);
	}
	//未知状态
	else {
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_FAIL);
	}
}


/*! \ TWI 主机仲裁丢失或者总线错误
 *  \参数 twi  TWI 主机实例
 */
void TWI_MasterArbitrationLostBusErrorHandler(TWI_Master_t *twi)
{
	uint8_t currentStatus = twi->interface->MASTER.STATUS;

	//总线错误
	if (currentStatus & TWI_MASTER_BUSERR_bm) {
		twi->result = TWIM_RESULT_BUS_ERROR;
	}
	//仲裁丢失
	else {
		twi->result = TWIM_RESULT_ARBITRATION_LOST;
	}

	//清除中断标志
	twi->interface->MASTER.STATUS = currentStatus | TWI_MASTER_ARBLOST_bm;

	twi->status = TWIM_STATUS_READY;
}


/*! \ TWI 主机写 中断
 *  处理写操作，回应(N)ACK.
 *  \参数 twi   TWI 主机实例
 */
void TWI_MasterWriteHandler(TWI_Master_t *twi)
{
	uint8_t bytesToWrite  = twi->bytesToWrite;
	uint8_t bytesToRead   = twi->bytesToRead;

	//从机NACK，取消传输
	if (twi->interface->MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi->result = TWIM_RESULT_NACK_RECEIVED;
		twi->status = TWIM_STATUS_READY;
	}
	//传数据
	else if (twi->bytesWritten < bytesToWrite) {
		uint8_t data = twi->writeData[twi->bytesWritten];
		twi->interface->MASTER.DATA = data;
		++twi->bytesWritten;
	}
	// 如果还需要读数据，写完了再读， 发送START信号 + 7位地址 + R/_W = 1
	//剩下的交给 读中断
	else if (twi->bytesRead < bytesToRead) {
		uint8_t readAddress = twi->address | 0x01;
		twi->interface->MASTER.ADDR = readAddress;
	}
	//传输完成发送STOP信号，设置RESULT OK
	else {
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_OK);
	}
}


/*! \ TWI 主机读中断
 *  \参数 twi TWI 主机实例
 */
void TWI_MasterReadHandler(TWI_Master_t *twi)
{
	//取数据
	if (twi->bytesRead < TWIM_READ_BUFFER_SIZE) {
		uint8_t data = twi->interface->MASTER.DATA;
		twi->readData[twi->bytesRead] = data;
		twi->bytesRead++;
	}
	//数据缓冲溢出，设置RESULT OVERFLOW
	else {
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_BUFFER_OVERFLOW);
	}
	uint8_t bytesToRead = twi->bytesToRead;

	//如果还有数据要读 发送ACK 开始读一个字节
	if (twi->bytesRead < bytesToRead) {
		twi->interface->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
	}
	//如果传输完毕 发送NACK 和 STOP 信号 
	else {
		twi->interface->MASTER.CTRLC = TWI_MASTER_ACKACT_bm |
		                               TWI_MASTER_CMD_STOP_gc;
		TWI_MasterTransactionFinished(twi, TWIM_RESULT_OK);
	}
}


/*! \ TWI 传输完毕处理
 *
 *  设置模块内的成员，为新传输做准备
 *
 *  \参数 twi     TWI 主机实例
 *  \参数 result  操作结果
 */
void TWI_MasterTransactionFinished(TWI_Master_t *twi, uint8_t result)
{
	twi->result = result;
	twi->status = TWIM_STATUS_READY;
}
