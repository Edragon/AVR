/*
 * ������:
     LED_Blinking
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-09 12:50
 * ����:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
 * ����:
     	������ʹ�˿��ϵ�LED�����������˸.
     	ʱ��:�ڲ�ʱ��2MHz���ϵ��ʼʱ�ӣ�
		Ӳ�����ӣ�
				LED1  -->   PD5
				LED2  -->   PD4
*/
#include <avr/io.h>
#include <util/delay.h>

#define LED1_ON()  PORTD_OUTCLR = 0x20
#define LED1_OFF() PORTD_OUTSET = 0x20
#define LED1_T()   PORTD_OUTTGL = 0x20

#define LED2_ON()  PORTD_OUTCLR = 0x10
#define LED2_OFF() PORTD_OUTSET = 0x10
#define LED2_T()   PORTD_OUTTGL = 0x10

int main()
{
	PORTD_DIR = 0x30;//PD5��PD4������Ϊ���

	while(1)
	{
		LED1_ON();
		LED2_ON();
		_delay_ms(500);
		LED1_OFF();
		LED2_OFF();
		_delay_ms(500);
	}
 
}
