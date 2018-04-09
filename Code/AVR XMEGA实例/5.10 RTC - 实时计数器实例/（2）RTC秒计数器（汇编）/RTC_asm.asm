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
     	������ʵ�������������RTCÿ������ж�,�洦�ж϶�PD.4ȡ�����Ƚ��ж϶�PD.5ȡ��
*/
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����
;.include "usart_driver.inc"

.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
.ORG 0x014         
        RJMP ISR_RTC_OVF_vect
.ORG 0x016         
        RJMP ISR_RTC_COMP_vect
.ORG 0X100       ;�����ж���0x00-0x0FF

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
.MACRO LED
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO
/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
RESET:
	//���ڲ� 32.768 kHz RC��������1Khz 
     ldi r16,OSC_RC32KEN_bm
     sts OSC_CTRL,r16
   //�ȴ�ʱ���ȶ�
RESET_1:
     lds r16,OSC_STATUS
     sbrs r16,OSC_RC32KRDY_bp
     jmp RESET_1

	//�����ڲ�32kHzΪRTCʱ��Դ
	ldi r16,CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm
	sts CLK_RTCCTRL,r16

	//����LED�˿�Ϊ���
	 ldi r16,0xff
	 sts PORTD_DIR,r16

     //���RTCæ��
RESET_2:
	 lds r16,RTC_STATUS
	 sbrc r16,RTC_SYNCBUSY_bp
	 jmp RESET_2
     nop
   

	//����RTC����Ϊ1S
    ldi r16,0xff
	sts RTC_PER,r16
	ldi r16,0x03
	sts RTC_PER+1,r16
	ldi r16,0  
	sts RTC_CNT,r16
	ldi r16,0
	sts RTC_COMP,r16
	ldi r16,RTC_PRESCALER_DIV1_gc
	sts RTC_CTRL,r16
 

	//ʹ������ж�	//ʹ�ܱȽ��ж�
	ldi r16,RTC_OVFINTLVL_LO_gc|RTC_COMPINTLVL_LO_gc
	sts RTC_INTCTRL ,r16
    ldi r16,PMIC_LOLVLEN_bm
	sts PMIC_CTRL,r16  
	sei
RESET_3:
    jmp RESET_3 


/*! \����жϷ������ �ڶ˿�D��LED����ʾ����
 */
ISR_RTC_OVF_vect:
     LED 0x10
	 reti
    
/*! \�Ƚ��жϷ������ �ڶ˿�D��LED����ʾ����
 */
ISR_RTC_COMP_vect:
      LED 0x20
	  reti
  
