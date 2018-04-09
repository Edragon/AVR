/*
 * ������:
     EBI
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-23 14:03
 * ����:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
 * ����:д��IS61LV6416sram�ڲ�10����ͬ�������ٶ�����ͨ��������ʾ
     	 
*/
#include "avr_compiler.h"
#include "ebi_driver.c"
#include "usart_driver.c"

/* �������� */
#define TESTBYTE   0xA5

/*SRAM��С UL=unsigned long*/
#define SRAM_SIZE 0x10000UL

//����ַ
#define SRAM_ADDR 0x4000
//д�����ݸ���
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
int main( void )
{ 
	/* Configure bus pins as outputs(except for data lines). */
	PORTH.DIR = 0xFF;
	PORTK.DIR = 0xFF;
	PORTJ.DIR = 0x00;
	uart_init();

	/* ��ʼ��EBI */
	EBI_Enable( EBI_SDDATAW_8BIT_gc,
	            EBI_LPCMODE_ALE1_gc,
	            EBI_SRMODE_ALE2_gc,
	            EBI_IFMODE_3PORT_gc );

	/*��ʼ��SRAM*/
	EBI_EnableSRAM( &EBI.CS0,               /* Chip Select 0. */
	                EBI_CS_ASPACE_64KB_gc,  /* 64 KB Address space. */
	                SRAM_ADDR,              /* Base address. */
	                0 );                    /* 0 wait states. */

	/* д����*/
	for (uint32_t i = 0; i < WRITE_NUM; i++) {
		__far_mem_write(i+SRAM_ADDR, TESTBYTE);
	}

	/*������*/
	for (uint32_t i = 0; i < WRITE_NUM; i++) {
			uart_putc_hex(__far_mem_read(i+SRAM_ADDR));
		}
    while(1);
}
