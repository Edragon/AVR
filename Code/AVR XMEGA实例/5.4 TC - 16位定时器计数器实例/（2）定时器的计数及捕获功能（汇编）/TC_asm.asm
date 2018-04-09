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
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����
.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//��λ 
;.ORG 0x20
  	;	RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
;.ORG 0x036        //USARTC0���ݽ�������ж����
        ;RJMP ISR
.ORG 0X01C
        RJMP ISR_OVFIF
.ORG 0X20
        RJMP ISR_CCA_vect
 
.ORG 0X100       ;�����ж���0x00-0x0FF

.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'


/*
+------------------------------------------------------------------------------
| Function    : LED
+------------------------------------------------------------------------------
| Description : ���ص�
| Parameters  : 
| Returns     :
+------------------------------------------------------------------------------
*/
         
.MACRO LED1_ON
           LDI R16,@0
           STS PORTD_OUTSET,R16
.ENDMACRO
.MACRO LED2_ON
           LDI R16,@0
           STS PORTD_OUTSET,R16
.ENDMACRO    
.MACRO LED1_OFF
           LDI R16,@0
           STS PORTD_OUTCLR,R16
.ENDMACRO
.MACRO LED2_OFF
           LDI R16,@0
           STS PORTD_OUTCLR,R16
.ENDMACRO
.MACRO LED1_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO
.MACRO LED2_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO
/*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : ��ʼ�� USARTC0
+------------------------------------------------------------------------------
*/
uart_init:

	       /* USARTC0 ���ŷ�������*/
  	       /* PC3 (TXD0) ��� */
           LDI R16,0X08
	       STS PORTC_DIRSET,R16
	       /* PC2 (RXD0) ���� */
	       LDI R16,0X04
	       STS PORTC_DIRCLR,R16
	        /* USARTC0 ģʽ - �첽*/	/* USARTC0֡�ṹ, 8 λ����λ, ��У��, 1ֹͣλ */
	       LDI R16,USART_CMODE_ASYNCHRONOUS_gc|USART_CHSIZE_8BIT_gc|USART_PMODE_DISABLED_gc
	       STS USARTC0_CTRLC,R16
	        /* ���ò����� 9600*/
           LDI R16,12
	       STS USARTC0_BAUDCTRLA,R16
	       LDI R16,0
	       STS USARTC0_BAUDCTRLB,R16
	        /* USARTC0 ʹ�ܷ���*//* USARTC0 ʹ�ܽ���*/
	       LDI R16,USART_TXEN_bm|USART_RXEN_bm
      	   STS USARTC0_CTRLB,R16
           RET
/*
+------------------------------------------------------------------------------
| Function    : Example1
+------------------------------------------------------------------------------
| Description : TCC0��������
+------------------------------------------------------------------------------
*/
Example1:  
	/* Set period/TOP value. */
	      LDI XH,0X10
	      EOR XL,XL
          STS TCC0_PER,XH
	      STS TCC0_PER+1,XL

          LDI R16,TC_CLKSEL_DIV1_gc
          STS TCC0_CTRLA,R16
Example1_0:
          JMP Example1_0
	      RET



/*
+------------------------------------------------------------------------------
| Function    : Example2
+------------------------------------------------------------------------------
| Description : TCC0ͨ��A���벶�� ������ά��������DOWN���������½��أ�CCAIF��λ
|				����Port D���������ֵ���ɽ�8��LED����ʾ			
+------------------------------------------------------------------------------
*/
Example2:

	/* PE3��Ϊ���룬�½��ش��� �������� ��I/O������Ϊ�¼��Ĳ���Դ�������ű�������Ϊ���ؼ�⡣*/
	LDI R16,PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc
    STS PORTE_PIN3CTRL,R16
	 
	LDI R16,0X08
	STS PORTE_DIRCLR,R16

	/* Port D��Ϊ��� */
	LDI R16,0X0FF
	STS PORTD_DIRSET,R16

	/* PE3��Ϊ�¼�ͨ��2������. */
	LDI R16,EVSYS_CHMUX_PORTE_PIN3_gc
	STS EVSYS_CH2MUX,R16

	/* ���� TCC0 ���벶��ʹ���¼�ͨ��2 */
	LDI R16,TC_EVSEL_CH2_gc |TC_EVACT_CAPT_gc
	STS TCC0_CTRLD,R16
	 
	/* ʹ��ͨ��A */
    LDI R16,TC0_CCAEN_bm &( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm )
	STS TCC0_CTRLB,R16
	 

	/* Set period/TOP value. */
	EOR XH,XH
	LDI XL,0X0FF
    STS  TCC0_PER,XL
	STS  TCC0_PER+1,XH

	/* ѡ��ʱ�ӣ�������ʱ�� */
	LDI R16,TC_CLKSEL_DIV1_gc
    STS TCC0_CTRLA,R16

Example2_1:    
    LDS R16,TCC0_INTFLAGS
	SBRS R16,TC0_CCAIF_bp
	JMP Example2_1
	NOP
	  
	/*��ʱ�����¼�����ʱ�����Ĵ����ĵ�ǰ����ֵ������CCA�Ĵ���*/
	LDS R16,TCC0_CCA
	STS PORTD_OUT,R16
    JMP  Example2_1
	RET



/*
+------------------------------------------------------------------------------
| Function    : Example3
+------------------------------------------------------------------------------
| Description : TCC0����ߵ͵�ƽ pc0��Ϊ���루���ô˺�������Ƶ�ʺ�ռ�ձȣ�		
+------------------------------------------------------------------------------
*/
Example3:
 
	/* Port D��Ϊ��� LEDָʾ*/
	LDI R16,0X0FF
	STS PORTD_DIRSET,R16
	LED1_OFF 0X20
	LED2_ON  0X10

	/* PC0��Ϊ���룬˫�ش��� */
	LDI R16,PORT_ISC_BOTHEDGES_gc
	STS PORTC_PIN0CTRL,R16
	LDI R16,0X01
	STS PORTC_DIRCLR,R16

	/* PC0��Ϊ�¼�ͨ��0������. */
	LDI R16,EVSYS_CHMUX_PORTC_PIN0_gc
	STS EVSYS_CH0MUX,R16

	/* ���� TCC0 ���벶��ʹ���¼�ͨ��0 */
	LDI R16,TC_EVSEL_CH0_gc |TC_EVACT_CAPT_gc
	STS TCC0_CTRLD,R16
	 

	/* ʹ��ͨ��A */
	LDI R16,TC0_CCAEN_bm &( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm )
	STS TCC0_CTRLB,R16
	 

	/* ������õ����ڼĴ�����ֵ��0x8000С��������ɺ�
	��I/O���ŵĵ�ƽ�仯�洢�ڲ���Ĵ��������λ��MSB����
	Clear MSB of PER[H:L] to allow for propagation of edge polarity. */
	LDI XH,0X7F
	LDI XL,0X0FF
	STS  TCC0_PER,XL
	STS  TCC0_PER+1,XH

	/* ѡ��ʱ�ӣ�������ʱ�� */
	LDI R16,TC_CLKSEL_DIV1_gc
	STS TCC0_CTRLA,R16

	/* ʹ��ͨ��A �ͼ����ж� */
	LDI R16,TC_CCAINTLVL_LO_gc
    STS TCC0_INTCTRLB,R16
	 
    LDI R16,PMIC_LOLVLEN_bm
	STS PMIC_CTRL,R16

	SEI
Example3_0:    
	JMP Example3_0
	RET

 


ISR_CCA_vect:
 
	LED1_T 0X20
	LED2_T 0X10
    /*��ʱ�����¼�����ʱ�����Ĵ����ĵ�ǰ����ֵ������CCA�Ĵ���*/
	LDS R17,TCC0_CCA
	LDS R16,TCC0_CCA+1
	/*  ��������������������ֵ */
    
	SBRS R16,7//MSB=1,����ߵ�ƽ��������
	JMP Example3_1
	NOP
	ANDI R16,0X80
	;ST TCC0_CTRLFSET,TC_CMD_RESTART_gc
	MOVW XH:XL,R16:R17
    uart_putw_dec

	JMP Example3_END
Example3_1:	
    
    MOVW XH:XL,R16:R17
    uart_putw_dec
 

Example3_END:
    
	RETI   

/*
+------------------------------------------------------------------------------
| Function    : Example4
+------------------------------------------------------------------------------
| Description : TCC0ͨ��Bռ�ձȱ仯��������ƣ�PC1���	�ɹ۲�LED����ռ�ձȵı仯		
+------------------------------------------------------------------------------
*/

Example4:
	/* PC1��� */
    LDI R16,0X02
	STS PORTC_DIRSET,R16

	/* ���ü������� */
	LDI XH,0X02
	EOR XL,XL
    STS TCC0_PER,XL
	STS TCC0_PER+1,XH
	/* ����TCΪ��б��ģʽ */
	LDS R16,TCC0_CTRLB
	ORI R16,TC_WGMODE_SS_gc
	STS TCC0_CTRLB,R16

	 /* ʹ��ͨ��B */
	LDI R16,TC0_CCBEN_bm &( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm )
	STS TCC0_CTRLB,R16
	 
	/* ѡ��ʱ�ӣ�������ʱ�� */
	LDI R16,TC_CLKSEL_DIV64_gc
	STS TCC0_CTRLA,R16
 
	 
		/* �±Ƚ�ֵ*/
    EOR XH,XH
	EOR XL,XL
	EOR YH,YH
	EOR YL,YL
	LDI YH,0X02
Example4_0:
    ADIW XH:XL,31
    CLZ
    CP XL,YL
    CPC XH,YH
    BRLO Example4_1
	EOR XH,XH
	EOR XL,XL
	ADIW XH:XL,31

Example4_1:		
   /*���õ�����Ĵ���*/  
    STS TCC0_CCBBUF,XL
	STS TCC0_CCBBUF+1,XH

  /*���ʱ�Ƚ�ֵ��CCBBUF[H:L] ���ݵ�CCB[H:L]*/
 Example4_2:
    LDS R16,TCC0_INTFLAGS
    SBRS R16,TC0_OVFIF_bp
	JMP Example4_2	
    NOP

  /* ��������־ */
    LDI R16,TC0_OVFIF_bm
	STS TCC0_INTFLAGS,R16
    JMP Example4_0
	RET


/*
+------------------------------------------------------------------------------
| Function    : Example5
+------------------------------------------------------------------------------
| Description : TCC0���¼��źţ�PE3�½��أ�����������ж϶�PD4ȡ��	
+------------------------------------------------------------------------------
*/
Example5:

	/* PE3��Ϊ���룬 �����������½��ظ�֪��DOWN�����¼���*/
	LDI R16,PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc
	STS PORTE_PIN3CTRL,R16
	LDI R16,0X08
	STS PORTE_DIRCLR,R16

	/* PD4��Ϊ���*/
	LDI R16,0X30
	STS PORTD_DIRSET,R16

	/* ѡ��PE3Ϊ�¼�ͨ��0������, ʹ�������˲�*/
	LDI R16,EVSYS_CHMUX_PORTE_PIN3_gc
	STS EVSYS_CH0MUX,R16
	LDI R16,EVSYS_DIGFILT_8SAMPLES_gc
	STS EVSYS_CH0CTRL,R16

	/* ���ü�������ֵ-TOP */
	LDI XL,0X04
	EOR XH,XH
    STS TCC0_PER,XL
	STS TCC0_PER+1,XH

	         /* ��������ж�Ϊ�ͼ����ж� */
			 LDI R16,USART_RXCINTLVL_LO_gc
             STS USARTC0_CTRLA,R16
	         /* Enable interrupts.*/
			 LDI R16,PMIC_LOLVLEN_bm
	         STS PMIC_CTRL,R16
             SEI   
  
	        /* ������ʱ�� */
			LDI R16,TC_CLKSEL_EVCH0_gc
            STS TCC0_CTRLA,R16
Example5_0:	        
            JMP Example5_0
			RET


ISR_OVFIF:
 
	       LED1_T 0X20
           LED2_T 0X10
		   RETI
	 
    

/*
+------------------------------------------------------------------------------
| Function    : Example6
+------------------------------------------------------------------------------
| Description : 32λ����,�����	PD4ȡ����LED�����˸
+------------------------------------------------------------------------------
*/
Example6:

	/* PD4��Ϊ��� */
	LDI R16,0x10
	STS PORTD_DIRSET,R16
	/* TCC0�����Ϊ�¼�ͨ��0������ */
	LDI R16,EVSYS_CHMUX_TCC0_OVF_gc
	STS EVSYS_CH0MUX,R16

	/* ʹ��TCC1����ʱ�� */
	LDS R16,TCC0_CTRLD
	ORI R16,TC0_EVDLY_bm
	STS TCC0_CTRLD,R16
	/* ���ü������� */
	LDI XL,0X0E2
	LDI XH,0X04
    STS TCC0_PER,XL
	STS TCC0_PER+1,XH
	LDI XL,0X0C8
	EOR XH,XH
    STS TCC1_PER,XL
	STS TCC1_PER+1,XH
	/* ʹ��ͨ��0��ΪTCC1ʱ��Դ */
	LDI R16,TC_CLKSEL_EVCH0_gc
	STS TCC1_CTRLA,R16
	/* ʹ������ʱ��8��Ƶ��ΪTCC0ʱ��Դ ������ʱ��*/
	LDI R16,TC_CLKSEL_DIV8_gc
	STS TCC0_CTRLA,R16

Example6_1:
        LDS R16,TCC1_INTFLAGS
        SBRS R16,TC1_OVFIF_bp
	    JMP Example6_1	
		NOP
		/* ȡ��PD4 */
		LED1_T 0X10
	    /* ��������־ */
		LDI R16,TC1_OVFIF_bm
		STS TCC1_INTFLAGS,R16
		RET

        
/*
+------------------------------------------------------------------------------
| Function    : RESET
+------------------------------------------------------------------------------
*/
RESET: 
	 CALL uart_init 
	 //CALL Example1
	 //CALL Example2 
	 //CALL Example3
	 CALL Example6
	 //CALL Example5 
	 //CALL Example6 


