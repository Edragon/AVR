/*
 * ������:
     EEPROM_ON_CHIP
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-21 14:15
 * ����:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
 * ����:
     	�������Ƭ��EEPROMʹ�ò�ͬ��ʽ���ж��ֲ�����
		USARTC0��ӡ������Ϣ��
*/

#include "avr_compiler.h"
#include "usart_driver.c"
#include "eeprom_driver.c"

#define TEST_BYTE_1 0x55
#define TEST_BYTE_2 0xAA

#define TEST_BYTE_ADDR_1 0x00
#define TEST_BYTE_ADDR_2 0x08

#define TEST_PAGE_ADDR_1    0  //ҳ��ַ������ҳ�ı߽�
#define TEST_PAGE_ADDR_2    2  //ҳ��ַ������ҳ�ı߽�
#define TEST_PAGE_ADDR_3    5  //ҳ��ַ������ҳ�ı߽�

//д��EEPROM�Ļ�������
uint8_t testBuffer[EEPROM_PAGESIZE] = {"Accessing Atmel AVR XMEGA EEPROM"};

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
int main( void )
{
	uart_init(); 
	uart_puts("inside main");uart_putc('\n');

	//���Խ��
	bool test_ok = true;
	uint8_t temp = 0;

	//��ջ���
	EEPROM_FlushBuffer();

	//1.ʹ��IO�Ĵ�����ʽд��Ͷ�ȡ2���ֽ�
	 
	//�ر�EEPROMӳ�䵽�ڴ�ռ�
	EEPROM_DisableMapping();

	//д�����ֽ�
	EEPROM_WriteByte(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_1, TEST_BYTE_1);
	EEPROM_WriteByte(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_2, TEST_BYTE_2);

	//��ȡд����ֽ�
	temp = EEPROM_ReadByte(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_1);
	if ( temp != TEST_BYTE_1) {
		test_ok = false;
	}
	uart_puts("1 Read TEST_BYTE_1 = ");
	uart_putc_hex(temp);
	uart_putc('\n');


	temp = EEPROM_ReadByte(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_2);
	if ( temp != TEST_BYTE_2) {
		test_ok = false;
	}
	uart_puts("1 Read TEST_BYTE_2 = ");
	uart_putc_hex(temp);
	uart_putc('\n');


	//2.�������дһ��ҳ

	//����ҳ���棬�Ȳ�ҳ��дҳ
	EEPROM_LoadPage(testBuffer);
	EEPROM_ErasePage(TEST_PAGE_ADDR_2);
	EEPROM_SplitWritePage(TEST_PAGE_ADDR_2);

	//��ȡ
	uart_puts("2 Read testBuffer = ");
	uart_putc('\n');
	for (uint8_t i = 0; i < EEPROM_PAGESIZE; ++i)
	{	
		temp = EEPROM_ReadByte(TEST_PAGE_ADDR_2, i );
		if ( temp != testBuffer[i] )
		{
			test_ok = false;
			break;
		}
		uart_putc(temp);
	}
	uart_putc('\n');




	//3.ʹ���ڴ�ӳ��д�벢��ȡ�����ֽ�

	EEPROM_EnableMapping();

	//д2���ֽ�
	EEPROM_WaitForNVM();
	EEPROM(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_1) = TEST_BYTE_1;
	//EEPROM_AtomicWritePage(TEST_PAGE_ADDR_1);//������һ��Ӧ���ò���
	EEPROM_WaitForNVM();
	EEPROM(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_2) = TEST_BYTE_2;
	//EEPROM_AtomicWritePage(TEST_PAGE_ADDR_1);//������һ��Ӧ���ò���

	//��ȡ2���ֽ�
	EEPROM_WaitForNVM();
	temp = EEPROM(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_1);
	if ( temp != TEST_BYTE_1)
	{
		test_ok = false;
	}
	uart_puts("3 Read TEST_BYTE_1 = ");
	uart_putc_hex(temp);
	uart_putc('\n');

	temp = EEPROM(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_2);
	if ( temp != TEST_BYTE_2)
	{
		test_ok = false;
	}
	uart_puts("3 Read TEST_BYTE_2 = ");
	uart_putc_hex(temp);
	uart_putc('\n');


	//4.ʹ���ڴ�ӳ�䷽ʽ�����������дһҳ���浽EEPROM
	EEPROM_EnableMapping();
	//���ػ���
	EEPROM_WaitForNVM();
	for (uint8_t i = 0; i < EEPROM_PAGESIZE; ++i)
	{
		EEPROM(TEST_PAGE_ADDR_3, i) = testBuffer[i];
	}

	//����EEPROMҳ��ҳ���沢û�б�����
	EEPROM_ErasePage(TEST_PAGE_ADDR_3);

	//����д�����������ڲ�����ɺ����
	EEPROM_SplitWritePage(TEST_PAGE_ADDR_3);

	//��ȡ��������ʾ
	EEPROM_WaitForNVM();
	uart_puts("4 Read testBuffer = ");
	uart_putc('\n');
	for (uint8_t i = 0; i < EEPROM_PAGESIZE; ++i)
	{
		temp = EEPROM(TEST_PAGE_ADDR_3, i);
		if ( temp!= testBuffer[i] )
		{
			test_ok = false;
			break;
			uart_puts("test_ok = false");
			uart_putc('\n');
		}
		uart_putc(temp);
	}
	uart_putc('\n');

	while(1)
		{
			nop();
		}
}
