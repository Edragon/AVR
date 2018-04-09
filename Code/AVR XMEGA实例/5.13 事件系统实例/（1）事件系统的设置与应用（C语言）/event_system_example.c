/*
 * ������:
     event_system_example
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
     	 
*/
#include "avr_compiler.h"
#include "event_system_driver.c"
#include "TC_driver.c"

void Example1( void );
void Example2( void );
void Example3( void );
void Example4( void );
#define LED1_T()   PORTD_OUTTGL = 0x20
#define LED1_ON()  PORTD_OUTSET = 0x20
#define LED1_OFF() PORTD_OUTSET = 0x20

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main( void )
{

	Example1();
	/*Example2();*/
	/*Example3();*/
	/*Example4();*/

	do {

	}while (1);
}

/*
+------------------------------------------------------------------------------
| Function    : Example1
+------------------------------------------------------------------------------
*/
void Example1( void )
{
	/* PD.0 ����/˫�ظ�֪*/
	PORTD.PIN0CTRL |= PORT_ISC_BOTHEDGES_gc;
	PORTD.DIRCLR = 0x01;
	PORTD_DIRSET = 0x20;

	/*ѡ��PD0Ϊchannel 0�¼�����*/
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_PORTD_PIN0_gc );

	//ѡ��ͨ��0��ΪTCC0���¼�Դ �����¼���Ϊ�����벶��
	TCC0.CTRLD = (uint8_t) TC_EVSEL_CH0_gc | TC_EVACT_CAPT_gc;

	/* ʹ��TCC0�Ƚϲ���ͨ��A */
	TCC0.CTRLB |= TC0_CCAEN_bm;

	/*����TCC0�������� */
	TCC0.PER = 0xFFFF;
	//TCC0ʱ��Դ
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;

	while (1) {
		if ( TCC0.INTFLAGS & TC0_CCAIF_bm ) {
    /* ��������ʱ��־λ��λ�������־λ*/
			TCC0.INTFLAGS |= TC0_CCAIF_bm;
			LED1_T();
		}
	}
}


/*
+------------------------------------------------------------------------------
| Function    : Example2
+------------------------------------------------------------------------------
*/
void Example2( void )
{
    PORTD_DIRSET = 0x20;
	/*ѡ��TCCO�洦��Ϊ�¼�ͨ��0���¼�*/
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_TCC0_OVF_gc );

	/*ADC��ѡ���ͨ��0 1 2 3���¼�ͨ��0��1 ��2,3��Ϊ��ѡ�¼�ͨ����
	EVSEL�����ͨ������С���¼�ͨ��������SWEEP�ж����ADCͨ����һ��ɨ��*/
	ADCA.EVCTRL = (uint8_t) ADC_SWEEP_0123_gc |
	              ADC_EVSEL_0123_gc |
	              ADC_EVACT_SWEEP_gc;

	/* ͨ��0,1,2,3����Ϊ�������������ź� ��������0,1,2,3�ĸ���ͨ����������������*/
	ADCA.CH0.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN4_gc |0;
	ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH1.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN5_gc |0;
	ADCA.CH1.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH2.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN6_gc |0;
	ADCA.CH2.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH3.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN7_gc |0;
	ADCA.CH3.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;


	/*  ����ADCʱ��Ԥ��ƵΪDIV8������Ϊ12λ��ADC��������ģʽ
	 *   ADC�ο���ѹΪ�ڲ�VCC / 1.6V��ʹ��ADC
	 */
	ADCA.PRESCALER = ( ADCA.PRESCALER & ~ADC_PRESCALER_gm ) |
	                 ADC_PRESCALER_DIV8_gc;
	ADCA.CTRLB = ( ADCA.CTRLB & ~ADC_RESOLUTION_gm ) |
	             ADC_RESOLUTION_12BIT_gc;
	ADCA.CTRLB = ( ADCA.CTRLB & ~( ADC_CONMODE_bm | ADC_FREERUN_bm ) );
	ADCA.REFCTRL = ( ADCA.REFCTRL & ~ADC_REFSEL_gm ) |
	               ADC_REFSEL_VCC_gc;
	ADCA.CTRLA |= ADC_ENABLE_bm;

	/*����TCC0�������� */
	TCC0.PER = 0x0FFF;

	/* ��������ж�Ϊ�ͼ����ж� */
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_LO_gc );
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	sei();	
	//TCC0ʱ��Դ
	TCC0.CTRLA |= TC_CLKSEL_DIV256_gc;

	while (1) {
	/*��TCC0���ʱADCͨ�������ϵı�ɨ�� 
	PD5����ĵƻ᲻�ϵ���˸*/
	           }
}

ISR(TCC0_OVF_vect)
{
 LED1_T();
}
/*
+------------------------------------------------------------------------------
| Function    : Example3
+------------------------------------------------------------------------------
*/
void Example3( void )
{
	/* PD.0 ����/˫�ظ�֪*/
	PORTD.PIN0CTRL = PORT_ISC_BOTHEDGES_gc;
	PORTD.DIRCLR = 0x01;

	//TCC0�����Ϊͨ��0���¼�Դ
 
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_TCC0_OVF_gc );

	/*ѡ��PD0Ϊͨ��1�¼����� */
	EVSYS_SetEventSource( 1, EVSYS_CHMUX_PORTD_PIN0_gc );

	/* ѡ��ͨ��0��ΪTCC1��ʱ��Դ TCC0��TCC1��������32Ϊ������*/
	TCC1.CTRLA = TC_CLKSEL_EVCH0_gc;

	/* ����ͨ��TCC0Ϊ����ģʽ */
	TCC0.CTRLD = (uint8_t) TC_EVSEL_CH1_gc | TC_EVACT_CAPT_gc;

	/* ����ͨ��TCC1Ϊ����ģʽ��������¼����ӳ����ֲ�����Ĵ���ʱ�� */
	TCC1.CTRLD = (uint8_t) TC_EVSEL_CH1_gc | TC0_EVDLY_bm | TC_EVACT_CAPT_gc;

	/* ʹ��TCC0,TCC1��Aͨ�� */
	TCC0.CTRLB = TC0_CCAEN_bm;
	TCC1.CTRLB = TC1_CCAEN_bm;

	//TCC0ʱ��Դ
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;

	while (1) {
		if ( TCC1.INTFLAGS & TC0_CCAIF_bm ) {
			/* ��������ʱ��־λ��λ�������־λ*/
			 
			TCC0.INTFLAGS |= TC0_CCAIF_bm;
			TCC1.INTFLAGS |= TC1_CCAIF_bm;
		}
	}
}

/*
+------------------------------------------------------------------------------
| Function    : Example4
+------------------------------------------------------------------------------
*/
void Example4( void )
{
	/* PD.0 ����/˫�ظ�֪*/
	PORTD.PIN0CTRL = PORT_ISC_RISING_gc;
	PORTD.DIRCLR = 0x01;

	/* PC����ȫ�����*/
	PORTC.DIRSET = 0xFF;

	/*ѡ��PD0Ϊͨ��0�¼�����*/
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_PORTD_PIN0_gc );

	/*  ��TCC0ͨ��A�ϼ������˲���*/
	  
	EVSYS_SetEventChannelFilter( 0, EVSYS_DIGFILT_8SAMPLES_gc );

	/*����TCC0�������� */ 
	TCC0.PER = 0xFFFF;
    //TCC0ʱ��ԴΪ�¼�ͨ��0
	TCC0.CTRLA = TC_CLKSEL_EVCH0_gc;


	while (1) {
		/* ���TCCO����������ֵ���෴*/
		PORTC.OUT = ~TCC0.CNT;
	}
}
