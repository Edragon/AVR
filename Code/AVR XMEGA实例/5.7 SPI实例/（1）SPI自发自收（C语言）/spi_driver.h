#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include "avr_compiler.h"

//硬件定义

#define SPI_SS_bm             0x10 //SS 引脚掩码
#define SPI_MOSI_bm           0x20 //MOSI 引脚掩码
#define SPI_MISO_bm           0x40 //MISO 引脚掩码
#define SPI_SCK_bm            0x80 //SCK 引脚掩码

//主机状态定义

#define SPI_OK              0     //传输成功
#define SPI_INTERRUPTED     1     //被其他主机中断
#define SPI_BUSY            2     //主机忙


//SPI数据包结构体
typedef struct SPI_DataPacket
{
	PORT_t *ssPort;                     //SS片选引脚指针
	uint8_t ssPinMask;                  //SS片选引脚掩码
	const uint8_t *transmitData;        //要传输的数据的指针
	volatile uint8_t *receiveData;      //接收数据的存储位置指针
	volatile uint8_t bytesToTransceive; //要传输的数据的字节数
	volatile uint8_t bytesTransceived;  //要接收的数据的字节数
	volatile bool complete;             //完成标志
} SPI_DataPacket_t;


//SPI主机结构体
typedef struct SPI_Master
{
	SPI_t *module;                //模块指针
	PORT_t *port;                 //模块使用的端口
	bool interrupted;             //如果SS被其他主机拉低的话为真
	SPI_DataPacket_t *dataPacket; //数据包指针
} SPI_Master_t;


//SPI从机结构体
typedef struct SPI_Slave
{
	SPI_t *module;      //模块指针
	PORT_t *port;       //模块使用的端口
} SPI_Slave_t;


//宏定义


/*! 检查传输是否完成
 *  \参数 _spi     SPI主机结构体实例
 *  \return         传输的当前状态
 *  \retval true    传输成功
 *  \retval false   传输正在进行
 */
#define SPI_MasterInterruptTransmissionComplete(_spi) ( (_spi)->dataPacket->complete )


/*! \拉低SS对从机寻址
 *  \参数 _port        SS引脚所在端口
 *  \参数 _pinBM       引脚掩码用来拉低SS
 */
#define SPI_MasterSSLow(_port, _pinBM) ( (_port)->OUTCLR = (_pinBM) )


/*! \释放SS
 *  \参数 _port         SS引脚所在端口
 *  \参数 _pinBM        引脚掩码用来拉低SS
 */
#define SPI_MasterSSHigh(_port, _pinBM) ( (_port)->OUTSET = (_pinBM) )


/*! \写数据自接到SPI移位寄存器
 *  \参数 _spi        SPI从机结构体实例
 *  \参数 _data       数据
 */
#define SPI_SlaveWriteByte(_spi, _data) ( (_spi)->module->DATA = (_data) )


/*! \从机读数据
 *  \参数 _spi       SPI从机结构体实例
 *  \return           数据
 */
#define SPI_SlaveReadByte(_spi) ( (_spi)->module->DATA )


/*! \检查新数据是否可用
 *  \参数 _spi       SPI从机结构体实例
 *  \return          如果数据可用返回真否则返回假
 */
#define SPI_SlaveDataAvailable(_spi) ( (_spi)->module->STATUS & SPI_IF_bm )


//函数声明

void SPI_MasterInit(SPI_Master_t *spi,
                    SPI_t *module,
                    PORT_t *port,
					bool lsbFirst,
                    SPI_MODE_t mode,
                    SPI_INTLVL_t intLevel,
                    bool clk2x,
                    SPI_PRESCALER_t clockDivision);

void SPI_SlaveInit(SPI_Slave_t *spi,
                   SPI_t *module,
                   PORT_t *port,
                   bool lsbFirst,
                   SPI_MODE_t mode,
                   SPI_INTLVL_t intLevel);

void SPI_MasterCreateDataPacket(SPI_DataPacket_t *dataPacket,
                                const uint8_t *transmitData,
                                uint8_t *receiveData,
                                uint8_t bytesToTransceive,
                                PORT_t *ssPort,
                                uint8_t ssPinMask);

void SPI_MasterInterruptHandler(SPI_Master_t *spi);

uint8_t SPI_MasterInterruptTransceivePacket(SPI_Master_t *spi,
                                            SPI_DataPacket_t *dataPacket);

uint8_t SPI_MasterTransceiveByte(SPI_Master_t *spi, uint8_t TXdata);

bool SPI_MasterTransceivePacket(SPI_Master_t *spi,
                                SPI_DataPacket_t *dataPacket);

#endif
