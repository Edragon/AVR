/*
 * ������:
     USART
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-10 21:35
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	���������USARTC0�����շ�ʾ��
*/
#include <avr/io.h>
#include "avr_compiler.h"
#include <avr/interrupt.h>
#include "usart_driver.c"
/*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : ��ʼ�� USARTC0
+------------------------------------------------------------------------------
*/
void uart_init(void)
{
	/* USARTC0 ���ŷ�������*/
  	/* PC3 (TXD0) ��� */
	PORTC.DIRSET   = PIN3_bm;
	/* PC2 (RXD0) ���� */
	PORTC.DIRCLR   = PIN2_bm;
	/* USARTC0 ģʽ - �첽*/
	USART_SetMode(&USARTC0,USART_CMODE_ASYNCHRONOUS_gc);
	/* USARTC0֡�ṹ, 8 λ����λ, ��У��, 1ֹͣλ */
	USART_Format_Set(&USARTC0, USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc, false);
	/* ���ò����� 9600*/
	USART_Baudrate_Set(&USARTC0, 12 , 0);
	/* USARTC0 ʹ�ܷ���*/
	USART_Tx_Enable(&USARTC0);
	/* USARTC0 ʹ�ܽ���*/
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

	/* USARTC0 �����жϼ���*/
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
| Description : USARTC0�����жϺ��� �յ������ݷ��ͻ�ȥUSARTC0
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













