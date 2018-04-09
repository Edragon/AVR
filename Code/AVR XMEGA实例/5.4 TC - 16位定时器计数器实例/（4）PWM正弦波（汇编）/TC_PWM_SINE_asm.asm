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
     	TCC0ͨ��Bռ�ձȰ�����������ֵ�仯��PC1����������ͨ�����׻��׵�
		ͨ�˲�������ɻ�ý�ƽ����������,Ƶ��Խ�ͣ�����Խ�ܣ��������Խƽ��
		��2000000/8/255/128=7HZ��
		ע�� �������鶨��ʱʹ�� PROGMEM �����쳣��������ֵ���ܴ洢��������
*/
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����
.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
;.ORG 0x036        //USARTC0���ݽ�������ж����
        ;RJMP ISR
.ORG 0X01C
        RJMP ISR_TCC0_OVF_vect

.ORG 0X100       ;�����ж���0x00-0x0FF
.MACRO LED1_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO
SineWaveTable128: .DB  128,134,140,147,153,159,165,171,177,182,188,193,199,204,209,213,218,222,226,230,234,237,240,243,245,248,250,251,253,254,254,255, 255,255,254,254,253,251,250,248,245,243,240,237,234,230,226,222,218,213,209,204,199,193,188,182,177,171,165,159,153,147,140,134,128,122,116,109,103,97,91,85,79,74,68,63,57,52,47,43, 38,34,30,26,22,19,16,13,11,8,6,5,3,2,2,1, 1,1,2,2,3,5,6,8,11,13,16,19,22,26,30,34, 38,43,47,52,57,63,68,74,79,85,91,97,103,109,116,122 // 128�����Ҳ�����ֵ

/*
+------------------------------------------------------------------------------
| Function    : RESET
+------------------------------------------------------------------------------
*/
RESET:

    LDI R16,0X20
	STS PORTD_DIR,R16;PD5������Ϊ���
	/* PC1��� */
	LDI R16,0X02
	STS PORTC_DIRSET,R16

	/* ���ü������� */
	LDI XL,0X0FF
	EOR XH,XH
    STS TCC0_PER,XL
	STS TCC0_PER+1,XH

	/* ����TCΪ��б��ģʽ */
	LDS R16,TCC0_CTRLB
	ORI R16,TC_WGMODE_SS_gc
	STS TCC0_CTRLB,R16

	/* ʹ��ͨ��B */
	LDI R16,TC0_CCBEN_bm &( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm )
	STS TCC0_CTRLB,R16

	/* ��������ж�Ϊ�ͼ����ж� */
	LDI R16,USART_RXCINTLVL_LO_gc
    STS USARTC0_CTRLA,R16
	/* Enable interrupts.*/
    LDI R16,PMIC_LOLVLEN_bm
	STS PMIC_CTRL,R16
    SEI   


	/* ѡ��ʱ�ӣ�������ʱ�� ���ʱ�Ƚ�ֵ��CCBBUF[H:L] ���ݵ�CCB[H:L]*/
	LDI R16,TC_CLKSEL_DIV64_gc
	STS TCC0_CTRLA,R16


	LDI ZH,HIGH(SineWaveTable128<<1)
	LDI ZL,LOW(SineWaveTable128<<1)

	EOR XH,XH;������	
RESET_0:
    JMP  RESET_0 



ISR_TCC0_OVF_vect:
 
	LED1_T 0X20
	/* �±Ƚ�ֵ*/
	INC XH 
	SBRS XH,7
	JMP ISR_TCC0_OVF_vect_0
	NOP 
	EOR XH,XH

ISR_TCC0_OVF_vect_0:	
	 /*���õ�����Ĵ��� ���ʱ�Ƚ�ֵ��CCBBUF[H:L] ���ݵ�CCB[H:L]*/
	 LPM R16,Z+
	 EOR R17,R17
	 STS TCC0_CCBBUF,R16
	 STS TCC0_CCBBUF+1,R17
	 RETI

	 
		



