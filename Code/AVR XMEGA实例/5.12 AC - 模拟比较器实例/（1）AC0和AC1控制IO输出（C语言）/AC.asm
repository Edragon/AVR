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
		 /*��ʱ��C0��ͨ��A��B��б��ģʽ����������ΪAC����pin0��pin1���������ڲ��ķ�ѹ;
          ACһ�����������ж�һ�����½����ж�;���ж��п���PD.4��PD.5ȡ�����ῴ���ƵĲ�ͣ
		  ��˸

*/

.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����

.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
.ORG 0x88
        RJMP ISR_ACA_AC0_vect
.ORG 0x8A
        RJMP ISR_ACA_AC1_vect
.ORG 0X100       ;�����ж���0x00-0x0FF

/*
+------------------------------------------------------------------------------
| Function    :  
+------------------------------------------------------------------------------
*/ 
.MACRO LED1_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO

/*
+------------------------------------------------------------------------------
| Function    : RESET
+------------------------------------------------------------------------------
| Description : 
+------------------------------------------------------------------------------
*/
RESET:
     
	 ldi r16,0x10
     sts PORTCFG_VPCTRLA,r16//;PORTBӳ�䵽����˿�1��PORTAӳ�䵽����˿�0
	  
	 ldi r16,0x32  
	 sts PORTCFG_VPCTRLB,r16//;PORTCӳ�䵽����˿�2��PORTDӳ�䵽����˿�3
 
     ldi r16,0x00
	 sts VPORT0_DIR,r16 //PORTA��������
	 ldi r16,0xff
     sts VPORT2_DIR,r16 //PORTC�������
	 ldi r16,0xff
     sts VPORT3_DIR,r16 //PORTD�������

	 /* ���ü������� */
	 ldi XH,0X0F
	 ldi XL,0X0a0
	 sts TCC0_PER,XL
	 sts TCC0_PER+1,XH
      
     ldi XH,0X03
	 ldi XL,0X0e8
     sts TCC0_CCABUF,XL
	 sts TCC0_CCABUF+1,XH


	 ldi XH,0X07
	 ldi XL,0X0D0
     sts TCC0_CCBBUF,XL
	 sts TCC0_CCBBUF+1,XH

	/* ����TCΪ��б��ģʽ ʹ��ͨ��A B */
	 LDS R16,TCC0_CTRLB
	 ORI R16,TC_WGMODE_SS_gc|TC0_CCAEN_bm | TC0_CCBEN_bm
	 STS TCC0_CTRLB,R16


	 /* ѡ��ʱ�ӣ�������ʱ�� */
	 LDI R16,TC_CLKSEL_DIV1024_gc
	 STS TCC0_CTRLA,R16

	 
    /*ģ��Ƚ��������ѹ��������*/
	 LDI R16,0
     STS ACA_CTRLB,R16
	 /* ����ģ��Ƚ���0�������� pin 0 and 1. */

     LDI R16,AC_MUXPOS_PIN0_gc|AC_MUXNEG_SCALER_gc
	 STS ACA_AC0MUXCTRL,R16
	 LDI R16,AC_MUXPOS_PIN2_gc|AC_MUXNEG_SCALER_gc
	 STS ACA_AC1MUXCTRL,R16
 
	/*����AC0 AC1�Ĵ���;�ͼ��жϼ���;ʹ��ģ��Ƚ���AC0 AC1 */ 
	 
	 LDI R16,AC_HYSMODE_SMALL_gc|AC_INTMODE_RISING_gc|AC_INTLVL_LO_gc |AC_ENABLE_bm
	 STS ACA_AC0CTRL,R16
	 
	 LDI R16,AC_HYSMODE_SMALL_gc|AC_INTMODE_FALLING_gc|AC_INTLVL_LO_gc |AC_ENABLE_bm
	 STS ACA_AC1CTRL,R16

     LDI R16,PMIC_HILVLEN_bm + PMIC_MEDLVLEN_bm + PMIC_LOLVLEN_bm; 
	 STS PMIC_CTRL,R16//�ɱ�̶���жϿ��ƼĴ����� �� �Ͳ�ʹ�ܣ�ѭ�����ȹرգ��ж�����δ���� Boot section	
  	    
     SEI//ȫ���ж�ʹ����λ

LOOP:	
		NOP
		 
        JMP LOOP


/*
+------------------------------------------------------------------------------
| Function    : �жϺ���
+------------------------------------------------------------------------------
*/ 
ISR_ACA_AC0_vect:
 
  LED1_T 0X10

  RETI
 
ISR_ACA_AC1_vect:
 
  LED1_T 0X20

  RETI
 
