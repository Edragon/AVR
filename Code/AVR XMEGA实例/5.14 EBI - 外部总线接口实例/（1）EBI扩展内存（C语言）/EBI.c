/*
 * 工程名:
     EBI
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-23 14:03
 * 作者:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
 * 描述:写入IS61LV6416sram内部10个相同的数据再读出来通过串口显示
     	 
*/
#include "avr_compiler.h"
#include "ebi_driver.c"
#include "usart_driver.c"

/* 测试数据 */
#define TESTBYTE   0xA5

/*SRAM大小 UL=unsigned long*/
#define SRAM_SIZE 0x10000UL

//基地址
#define SRAM_ADDR 0x4000
//写的数据个数
#define WRITE_NUM 0x0A

/*  Hardware setup for 3-port SRAM interface:
 *
 *  PORTK[7:0] - A[7:0]/A[15:8]/A[23:16] (BYTE 2 and 3 connected through ALE1 and ALE2)
 *
 *  PORTJ[7:0] - D[7:0]
 *
 *  PORTH[7:0] - {CS3,CS2,CS1,CS0,ALE2,ALE1,RE,WE} (CS0 used for SRAM)*/
 
/* +---------------------------------------------------------------------------
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
int main( void )
{ 
	/* Configure bus pins as outputs(except for data lines). */
	PORTH.DIR = 0xFF;
	PORTK.DIR = 0xFF;
	PORTJ.DIR = 0x00;
	uart_init();

	/* 初始化EBI */
	EBI_Enable( EBI_SDDATAW_8BIT_gc,
	            EBI_LPCMODE_ALE1_gc,
	            EBI_SRMODE_ALE2_gc,
	            EBI_IFMODE_3PORT_gc );

	/*初始化SRAM*/
	EBI_EnableSRAM( &EBI.CS0,               /* Chip Select 0. */
	                EBI_CS_ASPACE_64KB_gc,  /* 64 KB Address space. */
	                SRAM_ADDR,              /* Base address. */
	                0 );                    /* 0 wait states. */

	/* 写数据*/
	for (uint32_t i = 0; i < WRITE_NUM; i++) {
		__far_mem_write(i+SRAM_ADDR, TESTBYTE);
	}

	/*读数据*/
	for (uint32_t i = 0; i < WRITE_NUM; i++) {
			uart_putc_hex(__far_mem_read(i+SRAM_ADDR));
		}
    while(1);
}
