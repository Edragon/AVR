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
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����
.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��

.ORG 0X100       ;�����ж���0x00-0x0FF


/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/

RESET:

	call Example1 
	/*call Example2 
	call Example3 
	call Example4 */
RESET_1:
    JMP RESET_1

/*
+------------------------------------------------------------------------------
| Function    : Example1
+------------------------------------------------------------------------------
*/
Example1:

	/* PD.0 ����/˫�ظ�֪*/
	ldi r16,PORT_ISC_BOTHEDGES_gc
	sts PORTD_PIN0CTRL,r16
    ldi r16,0x01
	sts PORTD_DIRCLR,r16

	/*ѡ��PD0Ϊchannel 0�¼�����*/
	LDI R16,EVSYS_CHMUX_PORTD_PIN0_gc
	STS EVSYS_CH0MUX,R16

	//ѡ��ͨ��0��ΪTCC0���¼�Դ �����¼���Ϊ�����벶��
	LDI R16,TC_EVSEL_CH0_gc |TC_EVACT_CAPT_gc
	STS TCC0_CTRLD,R16

	/* ʹ��TCC0�Ƚϲ���ͨ��A */
    LDI R16,TC0_CCAEN_bm
	STS TCC0_CTRLB,R16


	/*����TCC0�������� */
	LDI XL,0X0FF
    STS  TCC0_PER,XL
	STS  TCC0_PER+1,XL

	//TCC0ʱ��Դ
	LDI R16,TC_CLKSEL_DIV1_gc
    STS TCC0_CTRLA,R16
Example1_1:/* ��������ʱ��־λ��λ�������־λ*/
 
	LDS R16,TCC0_INTFLAGS 
	SBRS R16,TC0_CCAIF_bp
	JMP  Example1_1
         
    LDI R16,TC0_CCAIF_bm
	STS TCC0_INTFLAGS,R16
	JMP Example1_1
    RET

/*
+------------------------------------------------------------------------------
| Function    : Example2
+------------------------------------------------------------------------------
*/
Example2:
	/*ѡ��TCCO�洦��Ϊ�¼�ͨ��0���¼�*/
	LDI R16,EVSYS_CHMUX_TCC0_OVF_gc
	STS EVSYS_CH0MUX,R16

	/*ADC��ѡ���ͨ��0 1 2 3���¼�ͨ��0��1 ��2,3��Ϊ��ѡ�¼�ͨ����
	EVSEL�����ͨ������С���¼�ͨ��������SWEEP�ж����ADCͨ����һ��ɨ��*/
	LDI R16,ADC_SWEEP_0123_gc |ADC_EVSEL_0123_gc |ADC_EVACT_SWEEP_gc
	STS ADCA_EVCTRL,R16

	/* ͨ��0,1,2,3����Ϊ�������������ź� ��������0,1,2,3�ĸ���ͨ����������������*/
	LDI R16,ADC_CH_MUXPOS_PIN4_gc |0
	STS ADCA_CH0_MUXCTRL,R16 
	LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc
	STS ADCA_CH0_CTRL,R16 
	LDI R16,ADC_CH_MUXPOS_PIN5_gc |0 
	STS ADCA_CH1_MUXCTRL,R16 
	LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc
	STS ADCA_CH1_CTRL,R16 
	LDI R16,ADC_CH_MUXPOS_PIN6_gc |0 
	STS ADCA_CH2_MUXCTRL,R16 
	LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc
	STS ADCA_CH2_CTRL,R16
	LDI R16,ADC_CH_MUXPOS_PIN7_gc |0
	STS ADCA_CH3_MUXCTRL,R16 
	LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc
	STS ADCA_CH3_CTRL,R16 


	/*  ����ADCʱ��Ԥ��ƵΪDIV8������Ϊ12λ��ADC��������ģʽ
	 *   ADC�ο���ѹΪ�ڲ�VCC / 1.6V��ʹ��ADC
	 */
	LDI R16,ADC_PRESCALER_DIV8_gc
	STS ADCA_PRESCALER,R16 
	     
    LDI R16,ADC_RESOLUTION_12BIT_gc|ADC_FREERUN_bm            
	STS ADCA_CTRLB,R16
	              
    LDI R16,ADC_REFSEL_VCC_gc
	STS ADCA_REFCTRL,R16
	LDI R16,ADC_ENABLE_bm                
	STS ADCA_CTRLA,R16 

	/*����TCC0�������� */
	LDI XL,0X0FF
    STS  TCC0_PER,XL
	STS  TCC0_PER+1,XL
	//TCC0ʱ��Դ
	LDI R16,TC_CLKSEL_DIV1_gc
    STS TCC0_CTRLA,R16

Example2_1:
		/*��TCC0���ʱADCͨ�������ϵı�ɨ�� */
 
    JMP Example2_1
	RET
/*
+------------------------------------------------------------------------------
| Function    : Example3
+------------------------------------------------------------------------------
*/
Example3:
 
	/* PD.0 ����/˫�ظ�֪*/
	ldi r16,PORT_ISC_BOTHEDGES_gc
	sts PORTD_PIN0CTRL,r16
    ldi r16,0x01
	sts PORTD_DIRCLR,r16

	//TCC0�����Ϊͨ��0���¼�Դ
    LDI R16,EVSYS_CHMUX_TCC0_OVF_gc
	STS EVSYS_CH0MUX,R16
 

	/*ѡ��PD0Ϊͨ��1�¼����� */
	LDI R16,EVSYS_CHMUX_PORTD_PIN0_gc
	STS EVSYS_CH1MUX,R16
 

	/* ѡ��ͨ��0��ΪTCC1��ʱ��Դ TCC0��TCC1��������32Ϊ������*/ 
	LDI R16,TC_CLKSEL_EVCH0_gc
	STS TCC1_CTRLA,R16

	/* ����ͨ��TCC0Ϊ����ģʽ */
	LDI R16,TC_EVSEL_CH1_gc | TC_EVACT_CAPT_gc 
	STS TCC0_CTRLD,R16

	/* ����ͨ��TCC1Ϊ����ģʽ��������¼����ӳ����ֲ�����Ĵ���ʱ�� */
	LDI R16,TC_EVSEL_CH1_gc|TC_EVACT_CAPT_gc| TC0_EVDLY_bm
	STS TCC1_CTRLD,R16

	/* ʹ��TCC0,TCC1��Aͨ�� */
	LDI R16,TC0_CCAEN_bm
	STS TCC0_CTRLB,R16
	LDI R16,TC1_CCAEN_bm
	STS TCC1_CTRLB,R16

	//TCC0ʱ��Դ
	LDI R16,TC_CLKSEL_DIV1_gc
	STS TCC0_CTRLA,R16

Example3_1:/* ��������ʱ��־λ��λ�������־λ*/
 
	LDS R16,TCC0_INTFLAGS 
	SBRS R16,TC0_CCAIF_bp
	JMP  Example3_1
         
    LDI R16,TC0_CCAIF_bm
	STS TCC0_INTFLAGS,R16
	LDI R16,TC1_CCAIF_bm
	STS TCC1_INTFLAGS,R16
	JMP Example3_1
    RET

/*
+------------------------------------------------------------------------------
| Function    : Example4
+------------------------------------------------------------------------------
*/
Example4:
	/* PD.0 ����/˫�ظ�֪*/
	ldi r16,PORT_ISC_BOTHEDGES_gc
	sts PORTD_PIN0CTRL,r16
    ldi r16,0x01
	sts PORTD_DIRCLR,r16

	/* PC����ȫ�����*/
    LDI R16,0x0ff
	STS PORTC_DIRSET,R16

	/*ѡ��PD0Ϊͨ��0�¼�����*/
	LDI R16,EVSYS_CHMUX_PORTD_PIN0_gc
	STS EVSYS_CH0MUX,R16

	/* ��TCC0ͨ��A�ϼ������˲���*/
	LDI R16,EVSYS_DIGFILT_8SAMPLES_gc
	STS EVSYS_CH0CTRL,R16 
 
    /*����TCC0�������� */
	LDI r16,0x0ff
    STS  TCC0_PER,r16
	STS  TCC0_PER+1,r16
	//TCC0ʱ��Դ
	LDI R16,TC_CLKSEL_EVCH0_gc
    STS TCC0_CTRLA,R16
 

Example4_1:
	/* ���TCCO����������ֵ*/
	LDS R16,TCC0_CNT
 	STS PORTC_OUT,R16
    JMP Example4_1
	RET
 
