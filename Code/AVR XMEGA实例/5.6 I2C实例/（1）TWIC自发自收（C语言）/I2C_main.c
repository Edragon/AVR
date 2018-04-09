/*
 * 工程名:
     I2C
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-19 15:47
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序使用TWIC自发自收，同时使能主机和从机，
		主程序中对发送缓存sendBuffer中的数据显示1s，
		发送，再对接收数据显示1s。
*/

#include "avr_compiler.h"
#include <util/delay.h>
#include "twi_master_driver.c"
#include "twi_slave_driver.c"

//从机地址
#define SLAVE_ADDRESS    0x55

//缓冲字节数
#define NUM_BYTES        6

// CPU 2MHz
#define CPU_SPEED   2000000

// 波特率100kHz
#define BAUDRATE	100000

#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)


//结构体变量
TWI_Master_t twiMaster;    //TWI 主机
TWI_Slave_t twiSlave;      //TWI 从机


//测试数据
uint8_t sendBuffer[NUM_BYTES] = {0x01,0x02,0x03,0x04,0x05,0x0F};

/*
+------------------------------------------------------------------------------
| Function    : TWIC_SlaveProcessData
+------------------------------------------------------------------------------
| Description : 从机数据处理函数 对接收数据取反
+------------------------------------------------------------------------------
*/
void TWIC_SlaveProcessData(void)
{
	uint8_t bufIndex = twiSlave.bytesReceived;
	twiSlave.sendData[bufIndex] = (~twiSlave.receivedData[bufIndex]);
}

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main(void)
{
	// PORTD 设为输出 LED 显示数据
	PORTD.DIRSET = 0xFF;

	// 如果外部没有接上拉电阻，使能上拉 PC0(TWI-SDA), PC1(TWI-SCL)
	PORTCFG.MPCMASK = 0x03; // 一次配置多个引脚
	PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	//初始化主机
	TWI_MasterInit(&twiMaster,
	               &TWIC,
	               TWI_MASTER_INTLVL_LO_gc,
	               TWI_BAUDSETTING);

	//初始化从机
	TWI_SlaveInitializeDriver(&twiSlave, &TWIC, TWIC_SlaveProcessData);
	TWI_SlaveInitializeModule(&twiSlave,
	                          SLAVE_ADDRESS,
	                          TWI_SLAVE_INTLVL_LO_gc);

	//使能低级别中断
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	uint8_t BufPos = 0;
	while (1) 
	{
		PORTD.OUT =0;
		for(BufPos=0;BufPos<6;BufPos++)
		{
			//LED 显示数据
			PORTD.OUT = sendBuffer[BufPos];
			_delay_ms(1000);		
			//主机发送数据
			TWI_MasterWriteRead(&twiMaster,SLAVE_ADDRESS,&sendBuffer[BufPos],1,1);
			while (twiMaster.status != TWIM_STATUS_READY)
			{
				//等待传输完成
			}
			//LED 显示数据(已经取反)
			PORTD.OUT = (twiMaster.readData[0]);
			_delay_ms(1000);	
		}
	}
}

/*! TWIC Master Interrupt vector. */
ISR(TWIC_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}

/*! TWIC Slave Interrupt vector. */
ISR(TWIC_TWIS_vect)
{
	TWI_SlaveInterruptHandler(&twiSlave);
}
