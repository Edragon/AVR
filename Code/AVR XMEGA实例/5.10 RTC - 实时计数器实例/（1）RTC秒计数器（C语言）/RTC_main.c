/*
 * ������:
     RTC
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
 * ����:
     	������ʵ�������������RTCÿ������жϣ�ʹ��7��LED����ʾBCD��
		�˿�D��PIN0-3��ʾ������λ��PIN4-6��ʾ����ʮλ
		PIN7�ϵ�LED���0.5����˸
*/
#include "avr_compiler.h"
#include "rtc_driver.c"


#define LED_PORT          PORTD
#define RTC_CYCLES_1S     1024


//bcd�������
typedef struct RTC_BCD_struct{
	uint8_t sec_ones;	//��λ
	uint8_t sec_tens;	//ʮλ
} RTC_BCD_t;


/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/

int main(void)
{
	//���ڲ� 32.768 kHz RC��������1.024 kHz
	OSC.CTRL |= OSC_RC32KEN_bm;

	do {//�ȴ�ʱ���ȶ�
	} while ( ( OSC.STATUS & OSC_RC32KRDY_bm ) == 0);

	//�����ڲ�32kHzΪRTCʱ��Դ
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;

	//����LED�˿�Ϊ���
	LED_PORT.DIR = 0xFF;

	do {//���RTCæ��
	} while ( RTC_Busy() );

	//����RTC����Ϊ1S
	RTC_Initialize( RTC_CYCLES_1S, 0, 0, RTC_PRESCALER_DIV1_gc );
	//���ü��0.5��PIN7�ϵ�LED��˸
	RTC_SetAlarm(RTC_CYCLES_1S/2);

	//ʹ������ж�
	RTC_SetOverflowIntLevel( RTC_OVFINTLVL_LO_gc);
	//ʹ�ܱȽ��ж�
	RTC_SetCompareIntLevel( RTC_COMPINTLVL_LO_gc);

	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	do {
		nop();
	} while (1);
}


/*! \����жϷ������ �ڶ˿�D��LED����ʾ����
 */
ISR(RTC_OVF_vect)
{
	static RTC_BCD_t rtcTime;

	if ( ++rtcTime.sec_ones > 9 )
	{
		rtcTime.sec_ones = 0;
		rtcTime.sec_tens++;
	}

	if ( rtcTime.sec_tens > 5 )
	{
		rtcTime.sec_tens = 0;
	}

	LED_PORT.OUT = ( ( rtcTime.sec_tens << 4 ) | rtcTime.sec_ones );
}
/*! \�Ƚ��жϷ������ �ڶ˿�D��LED����ʾ����
 */
ISR(RTC_COMP_vect)
   {
      LED_PORT.OUTTGL = 0X80;
	  RTC_SetAlarm(RTC_CYCLES_1S/2);
   }
