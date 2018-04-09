/*
 * 工程名:
     SPI
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-20 12:54
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序使用SPIE 和 SPIF 通信。USARTC0串口打印调试信息
 *    - 连接 PE4 到 PF4 (SS)
 *    - 连接 PE5 到 PF5 (MOSI)
 *    - 连接 PE6 到 PF6 (MISO)
 *    - 连接 PE7 到 PF7 (SCK)

*/

#include "avr_compiler.h"
#include "usart_driver.c"
#include "spi_driver.c"


//测试数据字节数
#define NUM_BYTES     4

//全局变量

//端口E的SPI模块作为主机
SPI_Master_t spiMasterE;

//端口F的SPI模块作为从机
SPI_Slave_t spiSlaveF;

// SPI 数据包
SPI_DataPacket_t dataPacket;

//主机要发送的数据
uint8_t masterSendData[NUM_BYTES] = {0x11, 0x22, 0x33, 0x44};

//从机接收的数据
uint8_t masterReceivedData[NUM_BYTES];

//测试结果
bool success = true;

/*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
void uart_init(void)
{
	/* USARTC0 引脚方向设置*/
  	/* PC3 (TXD0) 输出 */
	PORTC.DIRSET   = PIN3_bm;
	/* PC2 (RXD0) 输入 */
	PORTC.DIRCLR   = PIN2_bm;
	/* USARTC0 模式 - 异步*/
	USART_SetMode(&USARTC0,USART_CMODE_ASYNCHRONOUS_gc);
	/* USARTC0帧结构, 8 位数据位, 无校验, 1停止位 */
	USART_Format_Set(&USARTC0, USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc, false);
	/* 设置波特率 9600*/
	USART_Baudrate_Set(&USARTC0, 12 , 0);
	/* USARTC0 使能发送*/
	USART_Tx_Enable(&USARTC0);
	/* USARTC0 使能接收*/
	USART_Rx_Enable(&USARTC0);
}

/*! \brief Test function.
 *  1: Data is transmitted on byte at a time from the master to the slave.
 *     The slave increments the received data and sends it back. The master reads
 *     the data from the slave and verifies that it equals the data sent + 1.
 *
 *  2: Data is transmitted 4 bytes at a time to the slave. As the master sends
 *     a byte to the slave, the preceding byte is sent back to the master.
 *     When all bytes have been sent, it is verified that the last 3 bytes
 *     received at the master, equal the first 3 bytes sent.
 *
 *  The variable, 'success', will be non-zero when the function reaches the
 *  infinite for-loop if the test was successful.
 */
int main(void)
{
	uart_init(); 
	uart_puts("inside main");uart_putc('\n');

	//SS引脚线与上拉，方向输出
	PORTE.DIRSET = PIN4_bm;
	PORTE.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;

	//SS拉高
	PORTE.OUTSET = PIN4_bm;

	//SS引脚所在端口
	PORT_t *ssPort = &PORTE;

	//初始化端口E上的SPI主机
	SPI_MasterInit(&spiMasterE,
	               &SPIE,
	               &PORTE,
	               false,
	               SPI_MODE_0_gc,
	               SPI_INTLVL_OFF_gc,
	               false,
	               SPI_PRESCALER_DIV4_gc);

	//初始化端口F上的SPI从机
	SPI_SlaveInit(&spiSlaveF,
	              &SPIF,
	              &PORTF,
	              false,
				  SPI_MODE_0_gc,
				  SPI_INTLVL_OFF_gc);

	
	//1.单个字节方式传输
	// 主机: 拉低SS SPI_MasterTransceiveByte()不控制SS
	SPI_MasterSSLow(ssPort, PIN4_bm);
	uint8_t result = 0;

	for(uint8_t i = 0; i < NUM_BYTES; i++)
	{
		//主机: 主机传输数据到从机
		SPI_MasterTransceiveByte(&spiMasterE, masterSendData[i]);
		uart_puts("SPI_MasterTransceiveByte() return result = ");
		uart_putc_hex(result);
		uart_putc('\n');

		//从机: 等待数据可用
		while (SPI_SlaveDataAvailable(&spiSlaveF) == false){}
		uart_puts("SPI_SlaveDataAvailable");
		uart_putc('\n');

		//从机: 取数据
		uint8_t slaveByte = SPI_SlaveReadByte(&spiSlaveF);
		uart_puts("slaveByte = ");
		uart_putc_hex(slaveByte);
		uart_putc('\n');

		//从机: 收到数据加1，发送回去
		slaveByte++;
		SPI_SlaveWriteByte(&spiSlaveF, slaveByte);

		//主机: 发送空字节读数据
		uint8_t masterReceivedByte = SPI_MasterTransceiveByte(&spiMasterE, 0x00);
		uart_puts("masterReceivedByte = ");
		uart_putc_hex(masterReceivedByte);
		uart_putc('\n');

		//主机: 检查数据是否正确
		if (masterReceivedByte != (masterSendData[i] + 1) )
		{
			success = false;
			uart_puts("success = false");
			uart_putc('\n');
		}
		uart_putc('\n');
		uart_putc('\n');
	}
	//主机: 释放SS
	SPI_MasterSSHigh(ssPort, PIN4_bm);
	 

	//2: 传输数据包

	// 创建数据包(SS 在PE4).
	SPI_MasterCreateDataPacket(&dataPacket,
	                           masterSendData,
	                           masterReceivedData,
	                           NUM_BYTES,
	                           &PORTE,
	                           PIN4_bm);

	// 传输数据包
	uint8_t SPI_MTP_FLAG = SPI_MasterTransceivePacket(&spiMasterE, &dataPacket);

	uart_puts("SPI_MTP_FLAG = ");
	uart_putc_hex(SPI_MTP_FLAG);
	uart_putc('\n');

	// 检查接收数据的正确性，假设接收正确
	for (uint8_t i = 0; i < NUM_BYTES - 1; i++)
	{
		if (masterReceivedData[i + 1] != masterSendData[i])
		{
			success = false;
			uart_puts("success = false");
			uart_putc('\n');
		}
		uart_puts("masterReceivedData[");uart_putw_dec(i);uart_puts("+ 1] = ");
		uart_putc_hex(masterReceivedData[i + 1]);uart_putc('\n');
	}
	
	while(true)
	{
		nop();
	}
}
