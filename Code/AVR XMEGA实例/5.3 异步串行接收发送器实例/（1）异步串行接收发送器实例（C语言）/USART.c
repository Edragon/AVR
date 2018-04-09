/*
 * 工程名:
     USART
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-10 21:35
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序给出USARTC0串行收发示例
*/
#include <avr/io.h>
#include "avr_compiler.h"
#include <avr/interrupt.h>
#include "usart_driver.c"
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

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main(void)
{   
	uart_init(); 
	uart_putc('\n');
	uart_putw_hex(0xFFFF);
	uart_putc('=');
	uart_putdw_dec(0xFFFF);
	uart_putc('\n');
	uart_putdw_hex(0xAABBCCDD);
	uart_putc('=');
	uart_putdw_dec(0xAABBCCDD);
	uart_putc('\n');
	uart_puts("www.upc.edu.cn");
	uart_putc('\n');

	/* USARTC0 接收中断级别*/
	USART_RxdInterruptLevel_Set(&USARTC0,USART_RXCINTLVL_LO_gc);
	/* Enable interrupts. */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	while(1);
	return 0;
}

/*
+------------------------------------------------------------------------------
| Function    : ISR(USARTE1_RXC_vect)
+------------------------------------------------------------------------------
| Description : USARTC0接收中断函数 收到的数据发送回去USARTC0
+------------------------------------------------------------------------------
*/
ISR(USARTC0_RXC_vect)  
{
	unsigned char buffer;
	//LED1_T();
	buffer = USART_GetChar(&USARTC0);
 	do{
		/* Wait until it is possible to put data into TX data register.
		 * NOTE: If TXDataRegister never becomes empty this will be a DEADLOCK. */
		}while(!USART_IsTXDataRegisterEmpty(&USARTC0));
	USART_PutChar(&USARTC0,buffer);
}













