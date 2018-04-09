/*
 * ������:
     AC
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-11 1:17
 * ����:
 		 
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
		 /*��ʱ��C0��ͨ��A��B��б��ģʽ����������ΪAC����pin0��pin1���������ڲ��ķ�ѹ
          ACһ�����������ж�һ�����½����ж����ж��п���PD.4��PD.5ȡ�����ῴ���ƵĲ�ͣ
		  ��˸

*/

#include "ac_driver.c"
#include "TC_driver.c"

/* The Analog comparator used in the example.*/
#define AC ACA
#define LED1_T()   PORTD_OUTTGL = 0x20
#define LED2_T()   PORTD_OUTTGL = 0x10
/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main(void)
{  
 
     PORTCFG.VPCTRLA=0x10;//;PORTBӳ�䵽����˿�1��PORTAӳ�䵽����˿�0
	    
	 PORTCFG.VPCTRLB=0x32;//;PORTCӳ�䵽����˿�2��PORTDӳ�䵽����˿�3
 
	 VPORT0_DIR=0x00; //PORTA��������
     VPORT2_DIR=0xFF; //PORTC�������
     VPORT3_DIR=0xFF; //PORTD�������

	 /* ���ü������� */
	 TC_SetPeriod( &TCC0,4000 );
     TC_SetCompareA( &TCC0,1000);
	 TC_SetCompareB( &TCC0,2000);

	/* ����TCΪ��б��ģʽ */
	 TC0_ConfigWGM( &TCC0, TC_WGMODE_SS_gc );

	/* ʹ��ͨ��A B */
	 TC0_EnableCCChannels( &TCC0, TC0_CCAEN_bm );
	 TC0_EnableCCChannels( &TCC0, TC0_CCBEN_bm );


	 /* ѡ��ʱ�ӣ�������ʱ�� */
	 TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1024_gc );
	 

	//ʹ��ģ��Ƚ���AC0 AC1
	 AC_Enable(&AC, ANALOG_COMPARATOR0, false);
	 AC_Enable(&AC, ANALOG_COMPARATOR1, false);

	 //ģ��Ƚ��������ѹ��������
     AC_ConfigVoltageScaler(&AC,0);
	/* ����ģ��Ƚ���0�������� pin 0 and 1. */
	 AC_ConfigMUX(&AC, ANALOG_COMPARATOR0, AC_MUXPOS_PIN0_gc, AC_MUXNEG_SCALER_gc);

	 AC_ConfigMUX(&AC, ANALOG_COMPARATOR1, AC_MUXPOS_PIN2_gc, AC_MUXNEG_SCALER_gc);
 

	/*����AC0 AC1�Ĵ��� */ 
	 AC_ConfigHysteresis(&AC, ANALOG_COMPARATOR0, AC_HYSMODE_SMALL_gc);
	 AC_ConfigHysteresis(&AC, ANALOG_COMPARATOR1, AC_HYSMODE_SMALL_gc);

	 AC_ConfigInterrupt(&AC, ANALOG_COMPARATOR0,AC_INTMODE_RISING_gc,AC_INTLVL_LO_gc);
	 AC_ConfigInterrupt(&AC, ANALOG_COMPARATOR1,AC_INTMODE_FALLING_gc,AC_INTLVL_LO_gc);
 
     PMIC.CTRL |=PMIC_MEDLVLEN_bm+PMIC_LOLVLEN_bm+PMIC_HILVLEN_bm; //Enable Low_Level interrupts
     sei();
     while (1);
}

/*
+------------------------------------------------------------------------------
| Function    : �жϺ���
+------------------------------------------------------------------------------
*/ ISR(ACA_AC0_vect)
{
  LED2_T();
}
ISR(ACA_AC1_vect)
{
  LED1_T();
}
