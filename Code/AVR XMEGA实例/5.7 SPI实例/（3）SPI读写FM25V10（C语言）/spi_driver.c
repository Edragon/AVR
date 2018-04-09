#include "spi_driver.h"

/*! \SPI主机初始化
 *  \参数 spi            SPI主机结构体实例
 *  \参数 module         SPI模块指针
 *  \参数 port           SPI模块所在端口
 *  \参数 lsbFirst       如果设为非零值，数据顺序为LSB
 *  \参数 mode           SPI 模式 (时钟极性和相位).
 *  \参数 intLevel       SPI 中断级别
 *  \参数 clk2x	      	 SPI 倍速
 *  \参数 clockDivision  SPI 时钟分频因子
 */
void SPI_MasterInit(SPI_Master_t *spi,
                    SPI_t *module,
                    PORT_t *port,
                    bool lsbFirst,
                    SPI_MODE_t mode,
                    SPI_INTLVL_t intLevel,
                    bool clk2x,
                    SPI_PRESCALER_t clockDivision)
{
	spi->module         = module;
	spi->port           = port;
	spi->interrupted    = false;
	spi->module->CTRL   = clockDivision |                  //分频因子
	                      (clk2x ? SPI_CLK2X_bm : 0) |     //倍速
	                      SPI_ENABLE_bm |                  //SPI模块使能
	                      (lsbFirst ? SPI_DORD_bm  : 0) |  //数据顺序
	                      SPI_MASTER_bm |                  //SPI主机
	                      mode;                            //SPI 模式
	//中断级别
	spi->module->INTCTRL = intLevel;
	//未指向数据包
	spi->dataPacket = NULL;
 	//MOSI 和 SCK 设为输出
	spi->port->DIRSET  = SPI_MOSI_bm | SPI_SCK_bm;
}


/*! \SPI从机初始化
 *  \参数 spi                  SPI从机结构体实例
 *  \参数 module               SPI模块指针
 *  \参数 port                 SPI模块所在端口
 *  \参数 lsbFirst             如果设为非零值，数据顺序为LSB
 *  \参数 mode                 SPI 模式 (时钟极性和相位).
 *  \参数 intLevel             SPI 中断级别
 */
void SPI_SlaveInit(SPI_Slave_t *spi,
                   SPI_t *module,
                   PORT_t *port,
                   bool lsbFirst,
                   SPI_MODE_t mode,
                   SPI_INTLVL_t intLevel)
{
	//SPI模块
	spi->module       = module;
	spi->port         = port;
	spi->module->CTRL = SPI_ENABLE_bm |                //SPI模块使能
	                    (lsbFirst ? SPI_DORD_bm : 0) | //数据顺序
	                    mode;                          //SPI 模式
	//中断级别
	spi->module->INTCTRL = intLevel;
 	//MISO设为输出
	spi->port->DIRSET = SPI_MISO_bm;
}



/*! \创建数据包
 *
 *  dataPacket, transmitData 和 receiveData 必须在函数外准备好，
    receiveData 和 transmitData 的大小必须和bytesToTransceive一致
 *  \参数 dataPacket         数据包指针
 *  \参数 transmitData       发送数据指针
 *  \参数 receiveData        接收数据指针
 *  \参数 bytesToTransceive  收发数据字节数
 *  \参数 ssPort             SS引脚所在端口
 *  \参数 ssPinMask          SS引脚掩码
 */
void SPI_MasterCreateDataPacket(SPI_DataPacket_t *dataPacket,
                                const uint8_t *transmitData,
                                uint8_t *receiveData,
                                uint8_t bytesToTransceive,
                                PORT_t *ssPort,
                                uint8_t ssPinMask)
{
	dataPacket->ssPort            = ssPort;
	dataPacket->ssPinMask         = ssPinMask;
	dataPacket->transmitData      = transmitData;
	dataPacket->receiveData       = receiveData;
	dataPacket->bytesToTransceive  = bytesToTransceive;
	dataPacket->bytesTransceived   = 0;
	dataPacket->complete          = false;
}


/*! \SPI主机中断服务程序
 *  \参数 spi        SPI主机结构体实例
 */
void SPI_MasterInterruptHandler(SPI_Master_t *spi)
{
	uint8_t data;
	uint8_t bytesTransceived = spi->dataPacket->bytesTransceived;

	// 如果SS引脚中断(SS 使用且被拉低).此时没有数据接收到
	if ( !(spi->module->CTRL & SPI_MASTER_bm) )
	{
		spi->interrupted = true;
	}
	else 
	{   //数据中断
		//存储数据
		data = spi->module->DATA;
		spi->dataPacket->receiveData[bytesTransceived] = data;
		//下一字节
		bytesTransceived++;
		//如果还有数据没传完
		if (bytesTransceived < spi->dataPacket->bytesToTransceive)
		{
			//将数据写入数据寄存器
			data = spi->dataPacket->transmitData[bytesTransceived];
			spi->module->DATA = data;
		}
		else //传输结束
		{
			//释放SS
			uint8_t ssPinMask = spi->dataPacket->ssPinMask;
			SPI_MasterSSHigh(spi->dataPacket->ssPort, ssPinMask);
			spi->dataPacket->complete = true;
		}
	}
	//将bytesTransceived写回数据包
	spi->dataPacket->bytesTransceived = bytesTransceived;
}


/*! \启动传输
 *  数据包必须准备好
 *
 *  \参数 spi                SPI主机结构体实例
 *  \参数 dataPacket         SPI数据包结构体实例

 *  \return                   状态
 *  \retval SPI_OK            传输完成
 *  \retval SPI_BUSY          SPI模块忙
 *  \retval SPI_INTERRUPTED   被其他主机打断
 */
uint8_t SPI_MasterInterruptTransceivePacket(SPI_Master_t *spi,
                                            SPI_DataPacket_t *dataPacket)
{
	uint8_t data;
	bool interrupted = spi->interrupted;

	//如果还没有数据包被发送
	if (spi->dataPacket == NULL)
	{
		spi->dataPacket = dataPacket;
	}
	//如果传输正在进行
	else if (spi->dataPacket->complete == false)
	{
		return (SPI_BUSY);
	}
	//如果被其他主机打断
	else if (interrupted)
	{
		//如果SS被释放
		if (spi->port->OUT & SPI_SS_bm)
		{
			//没有中断
			interrupted = false;
		}
		else {
			return (SPI_INTERRUPTED);
		}
	}
	//如果没有被其他主机打断，启动传输
	spi->dataPacket = dataPacket;
	spi->dataPacket->complete = false;
	spi->interrupted = false;
	//拉低SS
	uint8_t ssPinMask = spi->dataPacket->ssPinMask;
	SPI_MasterSSLow(spi->dataPacket->ssPort, ssPinMask);
	spi->dataPacket->bytesTransceived = 0;
	//开始发送数据
	data = spi->dataPacket->transmitData[0];
	spi->module->DATA = data;
	//发送成功
	return (SPI_OK);
}


/*! \SPI双向传输字节
 *  将DATA 寄存器的数据移位到从机，同时从机数据移位到DATA 寄存器。
 *  本函数没有检查其他主机是否正在传输数据，多主机系统要避免竞争
 *
 *  SS 必须在调用本函数前拉低，结束后释放SS
 *
 *  \注意 最好使用中断方式，不会阻塞
 *
 *  \参数 spi        SPI主机结构体实例
 *  \参数 TXdata     发送到从机的数据
 *
 *  \返回            从机发来的数据
 */
uint8_t SPI_MasterTransceiveByte(SPI_Master_t *spi, uint8_t TXdata)
{
	//发送数据
	spi->module->DATA = TXdata;
	//等待传输完成
	while(!(spi->module->STATUS & SPI_IF_bm)) {}
	//读数据
	uint8_t result = spi->module->DATA;
	return(result);
}



/*! \SPI传输数据包
 *  本函数传输数据包结构体实例内的数个字节，传输过程中
 *  SS保持低电平接收数据存储到数据包内
 *  \参数 spi         SPI主机结构体实例
 *  \参数 dataPacket  SPI数据包结构体实例
 *
 *  \return            执行成功与否
 *  \retval true	   成功
 *  \retval false	   失败
 */
bool SPI_MasterTransceivePacket(SPI_Master_t *spi,
                                SPI_DataPacket_t *dataPacket)
{
	//检查数据包是否被创建
	if(dataPacket == NULL){return false;}
	//将数据包指针赋给SPI模块
	spi->dataPacket = dataPacket;
	uint8_t ssPinMask = spi->dataPacket->ssPinMask;
	//如果使用了SS
	if (spi->dataPacket->ssPort != NULL)
	{
		//SS拉低
		SPI_MasterSSLow(spi->dataPacket->ssPort, ssPinMask);
	}
	//传输字节
	uint8_t bytesTransceived = 0;
	uint8_t bytesToTransceive = dataPacket->bytesToTransceive;
	while (bytesTransceived < bytesToTransceive)
	{
		//发送数据
		uint8_t data = spi->dataPacket->transmitData[bytesTransceived];
		spi->module->DATA = data;
		//等待传输完成
		while(!(spi->module->STATUS & SPI_IF_bm)){}
		//读数据
		data = spi->module->DATA;
		spi->dataPacket->receiveData[bytesTransceived] = data;
		bytesTransceived++;
	}
	//如果使用了SS
	if (spi->dataPacket->ssPort != NULL)
	{
		//SS释放
		SPI_MasterSSHigh(spi->dataPacket->ssPort, ssPinMask);
	}
	//设置变量指示传输成功
	spi->dataPacket->bytesTransceived = bytesTransceived;
	spi->dataPacket->complete = true;
	//执行成功
	return true;
}
