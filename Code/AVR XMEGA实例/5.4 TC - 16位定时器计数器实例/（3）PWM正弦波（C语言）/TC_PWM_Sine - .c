/*
 * ������:
     TC_PWM_Sine
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-12 13:58
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	TCC0ͨ��Bռ�ձȰ�����������ֵ�仯���������ͨ�����׻��׵�ͨ�˲���
		����ɻ�ý�ƽ����������,Ƶ��Խ�ͣ�����Խ�ܣ��������Խƽ��
		��2000000/8/255/128=7HZ��
		ע�� �������鶨��ʱʹ�� PROGMEM �����쳣��������ֵ���ܴ洢��������
*/
#include "avr_compiler.h"
#include "TC_driver.c"

#define LED1_T()   PORTD_OUTTGL = 0x20

unsigned char SineWaveTable128[128]  = {
128,134,140,147,153,159,165,171,177,182,188,193,199,204,209,213,
218,222,226,230,234,237,240,243,245,248,250,251,253,254,254,255, 
255,255,254,254,253,251,250,248,245,243,240,237,234,230,226,222,
218,213,209,204,199,193,188,182,177,171,165,159,153,147,140,134,
128,122,116,109,103,97,91,85,79,74,68,63,57,52,47,43, 38,34,30,
26,22,19,16,13,11,8,6,5,3,2,2,1, 1,1,2,2,3,5,6,8,11,13,16,19,22
,26,30,34, 38,43,47,52,57,63,68,74,79,85,91,97,103,109,116,122}; // 128�����Ҳ�����ֵ

uint16_t compareValue = 0x0000;
int main(void)
{
	PORTD_DIR = 0x20;//PD5������Ϊ���
	/* PC1��� */
	PORTC.DIRSET = 0x02;

	/* ���ü������� */
	TC_SetPeriod( &TCC0, 255 );

	/* ����TCΪ��б��ģʽ */
	TC0_ConfigWGM( &TCC0, TC_WGMODE_SS_gc );

	/* ʹ��ͨ��B */
	TC0_EnableCCChannels( &TCC0, TC0_CCBEN_bm );

	/* ��������ж�Ϊ�ͼ����ж� */
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_LO_gc );
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	sei();

	/* ѡ��ʱ�ӣ�������ʱ�� */
		
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV64_gc );

	do {
		/*���ʱ�Ƚ�ֵ��CCBBUF[H:L] ���ݵ�CCB[H:L]*/	
	} while (1);

}

ISR(TCC0_OVF_vect)
{
	LED1_T();//���ָʾ��
	/* �±Ƚ�ֵ*/
	compareValue ++; 
	if(compareValue>128){compareValue = 0; }
	
	 /*���õ�����Ĵ���*/
	TC_SetCompareB( &TCC0, SineWaveTable128[compareValue] );

	/*���ʱ�Ƚ�ֵ��CCBBUF[H:L] ���ݵ�CCB[H:L]*/	
}
