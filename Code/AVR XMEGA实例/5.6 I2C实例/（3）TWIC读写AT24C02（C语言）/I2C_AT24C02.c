/*
 * 工程名:
     I2C_AT24C02
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-20 10:44
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序使用TWIF和串行EEPROM AT24C02通信，向AT24C02
		分2次写入16个字节，再读出指定片内字节地址处的数据。
		24C02中带有片内地址寄存器。每写入或读出一个数据字
		节后，该地址寄存器自动加1，以实现对下一个存储单元
		的读写。所有字节均以单一操作方式读取。为降低总的
		写入时间，一次操作可写入多达8个字节的数据。
*/

#include "avr_compiler.h"
#include <util/delay.h>
#include "twi_master_driver.c"

//器件地址 0B0 1010 000
#define DEVICE_ADDRESS    0x50

//缓冲字节数
#define NUM_BYTES        9

// CPU 2MHz
#define CPU_SPEED   2000000

// 波特率100kHz
#define BAUDRATE	100000

#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

//片内字节地址
uint8_t WORD_ADDRESS = 0x00;

//结构体变量
TWI_Master_t twiMaster;    //TWI 主机

//测试数据
uint8_t sendBuffer[NUM_BYTES] = 
{
0x00,
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07
};

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main(void)
{
	// PORTD 设为输出 LED 显示数据
	PORTD.DIRSET = 0xFF;

	// 如果外部没有接上拉电阻，使能上拉 PF0(TWI-SDA)灰白, PF1(TWI-SCL)棕色
	PORTCFG.MPCMASK = 0x03; // 一次配置多个引脚
	PORTF.PIN0CTRL = (PORTF.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	//初始化主机
	TWI_MasterInit(&twiMaster,&TWIF,TWI_MASTER_INTLVL_LO_gc,TWI_BAUDSETTING);

	//使能低级别中断
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	uint8_t BufPos = 0;
	while (1) 
	{
		PORTD.OUT =0;
			
		//主机发送数据，一次写8个字节
		sendBuffer[0] = WORD_ADDRESS;
		TWI_MasterWrite(&twiMaster,DEVICE_ADDRESS,&sendBuffer[0],9);
		while (twiMaster.status != TWIM_STATUS_READY)
		{
			//等待传输完成
		}

		//PORTD.OUT = twiMaster.result;
		_delay_ms(600);
	
		//主机读取数据，先写片内字节地址 再一次读8个字节
		TWI_MasterWriteRead(&twiMaster,DEVICE_ADDRESS,WORD_ADDRESS,1,8);
		while (twiMaster.status != TWIM_STATUS_READY)
		{
			//等待传输完成
		}
		
		//PORTD.OUT = twiMaster.result;
		//_delay_ms(600);

		for(BufPos=0;BufPos<8;BufPos++)
		{
			//LED 显示取反数据
			PORTD.OUT = ~twiMaster.readData[BufPos];
			_delay_ms(600);
		}
	}
}

// TWIF 主机中断服务程序
ISR(TWIF_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}

