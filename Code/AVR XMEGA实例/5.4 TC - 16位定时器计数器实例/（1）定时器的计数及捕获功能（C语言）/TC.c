/*
 * ������:
     TC
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-11 1:17
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	Example1 TCC0��������
		Example2 TCC0ͨ��A���벶��
		Example3 TCC0Ƶ�ʲ���PC0���ź�Ƶ��
		Example4 TCC0ͨ��Bռ�ձȱ仯������������
		Example5 TCC0���¼��źż���
		Example6 32λ����
*/
/* ���� CPU ʱ�Ӻͷ�Ƶֵ */
#define F_CPU           2000000UL
#define CPU_PRESCALER   1

#include "avr_compiler.h"
#include "usart_driver.c"
#include "TC_driver.c"

void Example1( void );
void Example2( void );
void Example3( void );
void Example4( void );
void Example5( void );
void Example6( void );


#define LED1_ON()  PORTD_OUTCLR = 0x20
#define LED1_OFF() PORTD_OUTSET = 0x20
#define LED1_T()   PORTD_OUTTGL = 0x20

#define LED2_ON()  PORTD_OUTCLR = 0x10
#define LED2_OFF() PORTD_OUTSET = 0x10
#define LED2_T()   PORTD_OUTTGL = 0x10

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
	//Example1();
	//Example2();
	//Example3();
	Example4();
	//Example5();
	//Example6();
}

/*
+------------------------------------------------------------------------------
| Function    : Example1
+------------------------------------------------------------------------------
| Description : TCC0��������
+------------------------------------------------------------------------------
*/
void Example1( void )
{
	/* Set period/TOP value. */
	TC_SetPeriod( &TCC0, 0x1000 );

	/* Select clock source. */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1_gc );

	do {
		/* Wait while the timer counts. */
	} while (1);
}


/*
+------------------------------------------------------------------------------
| Function    : Example2
+------------------------------------------------------------------------------
| Description : TCC0ͨ��A���벶�� ������ά��������DOWN���������½��أ�CCAIF��λ
|				����Port D���������ֵ���ɽ�8��LED����ʾ			
+------------------------------------------------------------------------------
*/
void Example2( void )
{
	uint16_t inputCaptureTime;

	/* PE3��Ϊ���룬�½��ش��� �������� ��I/O������Ϊ�¼��Ĳ���Դ�������ű�������Ϊ���ؼ�⡣*/
	PORTE.PIN3CTRL = PORT_ISC_FALLING_gc + PORT_OPC_PULLUP_gc;
	PORTE.DIRCLR = 0x08;

	/* Port D��Ϊ��� */
	PORTD.DIRSET = 0xFF;

	/* Pe0��Ϊ�¼�ͨ��2������. */
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTE_PIN3_gc;

	/* ���� TCC0 ���벶��ʹ���¼�ͨ��2 */
	TC0_ConfigInputCapture( &TCC0, TC_EVSEL_CH2_gc );

	/* ʹ��ͨ��A */
	TC0_EnableCCChannels( &TCC0, TC0_CCAEN_bm );

	/* Set period/TOP value. */
	TC_SetPeriod( &TCC0, 255 );

	/* ѡ��ʱ�ӣ�������ʱ�� */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1_gc );

	 do {
		do {} while ( TC_GetCCAFlag( &TCC0 ) == 0 );
		/*��ʱ�����¼�����ʱ�����Ĵ����ĵ�ǰ����ֵ������CCA�Ĵ���*/
		inputCaptureTime = TC_GetCaptureA( &TCC0 );
		PORTD.OUT = (uint8_t) (inputCaptureTime);
	} while (1);
}


/*
+------------------------------------------------------------------------------
| Function    : Example3
+------------------------------------------------------------------------------
| Description : TCC0Ƶ�ʲ�����PC0���			
+------------------------------------------------------------------------------
*/
void Example3( void )
{
	/* Port D��Ϊ��� LEDָʾ*/
	PORTD.DIRSET = 0xFF;
	LED1_OFF();
	LED2_ON();

	/* PC0��Ϊ���룬˫�ش��� */
	PORTC.PIN0CTRL = PORT_ISC_BOTHEDGES_gc;
	PORTC.DIRCLR = 0x01;

	/* PC0��Ϊ�¼�ͨ��0������. */
	EVSYS.CH0MUX = EVSYS_CHMUX_PORTC_PIN0_gc;

	/* ���� TCC0 ���벶��ʹ���¼�ͨ��2 */
	TC0_ConfigInputCapture( &TCC0, TC_EVSEL_CH0_gc );

	/* ʹ��ͨ��A */
	TC0_EnableCCChannels( &TCC0, TC0_CCAEN_bm );

	/* ������õ����ڼĴ�����ֵ��0x8000С��������ɺ�
	��I/O���ŵĵ�ƽ�仯�洢�ڲ���Ĵ��������λ��MSB����
	Clear MSB of PER[H:L] to allow for propagation of edge polarity. */
	TC_SetPeriod( &TCC0, 0x7FFF );

	/* ѡ��ʱ�ӣ�������ʱ�� */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1_gc );

	/* ʹ��ͨ��A �ͼ����ж� */
	TC0_SetCCAIntLevel( &TCC0, TC_CCAINTLVL_LO_gc );
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	sei();

	do {
		/* Wait while interrupt measure Frequency and Duty cycle. */
	} while (1);
}


ISR(TCC0_CCA_vect)
{
	LED1_T();
	LED2_T();
	static uint32_t frequency;
	static uint32_t dutyCycle;
	static uint16_t totalPeriod;
	static uint16_t highPeriod;

	uint16_t thisCapture = TC_GetCaptureA( &TCC0 );

	/*  ��������������������ֵ */
	if ( thisCapture & 0x8000 ) {//MSB=1,����ߵ�ƽ��������
		totalPeriod = thisCapture & 0x7FFF;
		TC_Restart( &TCC0 );
	}
	 /* �½��ر���ߵ�ƽ������ */
	else {
		highPeriod = thisCapture;
	}

	dutyCycle = ( ( ( highPeriod * 100 ) / totalPeriod ) + dutyCycle ) / 2;
	frequency = ( ( ( F_CPU / CPU_PRESCALER ) / totalPeriod ) + frequency ) / 2;
	 /* ���ڴ�ӡռ�ձȺ�Ƶ�� */
	uart_puts("dutyCycle = ");
	uart_putdw_dec(dutyCycle);
	uart_putc('\n');
	uart_puts("frequency = ");
	uart_putdw_dec(frequency);
	uart_putc('\n');
}

/*
+------------------------------------------------------------------------------
| Function    : Example4
+------------------------------------------------------------------------------
| Description : TCC0ͨ��Bռ�ձȱ仯��������ƣ�PC1���	�ɹ۲�LED����ռ�ձȵı仯		
+------------------------------------------------------------------------------
*/

void Example4( void )
{	
	uint16_t compareValue = 0x0000;
	/* PC1��� */
	PORTC.DIRSET = 0x02;

	/* ���ü������� */
	TC_SetPeriod( &TCC0, 512 );

	/* ����TCΪ��б��ģʽ */
	TC0_ConfigWGM( &TCC0, TC_WGMODE_SS_gc );

	/* ʹ��ͨ��B */
	TC0_EnableCCChannels( &TCC0, TC0_CCBEN_bm );


	/* ѡ��ʱ�ӣ�������ʱ�� */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV64_gc );	


	do {
		/* �±Ƚ�ֵ*/
		compareValue += 31; 
		if(compareValue>512)compareValue = 31; 

		 /*���õ�����Ĵ���*/
		TC_SetCompareB( &TCC0, compareValue );

		/*���ʱ�Ƚ�ֵ��CCBBUF[H:L] ���ݵ�CCB[H:L]*/	
		do {} while( TC_GetOverflowFlag( &TCC0 ) == 0 );

		/* ��������־ */
		TC_ClearOverflowFlag( &TCC0 );

	} while (1);
}


/*
+------------------------------------------------------------------------------
| Function    : Example5
+------------------------------------------------------------------------------
| Description : TCC0���¼��źţ�PE3�½��أ�����������ж϶�PD4ȡ��	
+------------------------------------------------------------------------------
*/
void Example5( void )
{
	/* PE3��Ϊ���룬 �����������½��ظ�֪��DOWN�����¼���*/
	PORTE.PIN3CTRL = PORT_ISC_FALLING_gc + PORT_OPC_PULLUP_gc;
	PORTE.DIRCLR = 0x08;

	/* PD4��Ϊ���*/
	PORTD.DIRSET = 0x30;

	/* ѡ��PE3Ϊ�¼�ͨ��0������, ʹ�������˲�*/
	EVSYS.CH0MUX = EVSYS_CHMUX_PORTE_PIN3_gc;
	EVSYS.CH0CTRL = EVSYS_DIGFILT_8SAMPLES_gc;

	/* ���ü�������ֵ-TOP */
	TC_SetPeriod( &TCC0, 4 );

	/* ��������ж�Ϊ�ͼ����ж� */
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_LO_gc );
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	sei();

	/* ������ʱ�� */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_EVCH0_gc );

	do {} while (1);
}


ISR(TCC0_OVF_vect)
{
	/* ȡ��PD4 */
	PORTD.OUTTGL = 0x10;

	PORTD.OUTCLR = 0x20;
}


/*
+------------------------------------------------------------------------------
| Function    : Example6
+------------------------------------------------------------------------------
| Description : 32λ����,�����	PD4ȡ����LED���1S��˸
+------------------------------------------------------------------------------
*/
void Example6( void )
{
	/* PD4��Ϊ��� */
	PORTD.DIRSET = 0x10;
	/* TCC0�����Ϊ�¼�ͨ��0������ */
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;

	/* ʹ��TCC1����ʱ�� */
	TC_EnableEventDelay( &TCC1 );
	/* ���ü������� */
	TC_SetPeriod( &TCC0, 1250 );
	TC_SetPeriod( &TCC1, 200 );
	/* ʹ��ͨ��0��ΪTCC1ʱ��Դ */
	TC1_ConfigClockSource( &TCC1, TC_CLKSEL_EVCH0_gc );

	/* ʹ������ʱ��8��Ƶ��ΪTCC0ʱ��Դ ������ʱ��*/
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV8_gc );

	do {
		
		do {} while( TC_GetOverflowFlag( &TCC1 ) == 0 );
		/* ȡ��PD4 */
		PORTD.OUTTGL = 0x10;
		
		/* ��������־ */
		TC_ClearOverflowFlag( &TCC1 );
		
	} while (1);

}
