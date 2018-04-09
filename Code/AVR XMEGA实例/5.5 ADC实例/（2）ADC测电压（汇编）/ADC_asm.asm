/*
 * ������:
     ADC
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
		void ADCA_CH0_1_2_3_Sweep_Interrupt(void)
		ADCA 4ͨ��ɨ�裬��������ģʽ��CH3�ͼ����жϣ��ж��ӳ���
		�ĸ�ͨ����ת��ͨ��USARTC0��ӡ�����������Ϣ

*/
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����
.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
;.ORG 0x036        //USARTC0���ݽ�������ж����
        ;RJMP ISR
.ORG 0X094
        RJMP ISR_ADCA_CH3_vect

.ORG 0X100       ;�����ж���0x00-0x0FF
.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'
STRING: .DB 'A','D','C','A','_','O','F','F','S','E','T','=',0
STRING0: .DB 'C','H','A','N','E','L','_','0','=',0
STRING1: .DB 'C','H','A','N','E','L','_','1','=',0
STRING2: .DB 'C','H','A','N','E','L','_','2','=',0
STRING3: .DB 'C','H','A','N','E','L','_','3','=',0


 
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
	       LDI R16,USART_TXEN_bm
      	   STS USARTC0_CTRLB,R16
           RET

ADCA_CH0_1_2_3_Sweep_Interrupt:
 
	// ����У׼ֵ
           LDI ZL,PROD_SIGNATURES_START + NVM_PROD_SIGNATURES_ADCACAL0_offset
	       EOR ZH,ZH 
		   LDI R16,NVM_CMD_READ_CALIB_ROW_gc
		   STS NVM_CMD,R16

	       LPM R17,Z+
		   LPM R18,Z

		   LDI R16,NVM_CMD_NO_OPERATION_gc
		   STS NVM_CMD,R16
	       STS ADCA_CAL,R17 
	       STS ADCA_CAL+1,R18  
 

	// ���� ADC A �з���ģʽ 12 λ�ֱ���
           LDI R16,ADC_CONMODE_bm|ADC_RESOLUTION_12BIT_gc
	       STS ADCA_CTRLB,R16

	// ����ADC��Ƶ 
	       LDI R16,ADC_PRESCALER_DIV32_gc
           STS ADCA_PRESCALER,R16
    

	// ���òο���ѹ VCC/1.6 V
	       LDI R16,ADC_REFSEL_INT1V_gc
		   STS ADCA_REFCTRL,R16

   	//  ADC A��������룬������
	       LDI R16,ADC_CH_INPUTMODE_DIFF_gc|ADC_CH_GAIN_1X_gc
           STS ADCA_CH0_CTRL,R16
           
		   LDI R16,ADC_CH_MUXPOS_PIN0_gc|ADC_CH_MUXNEG_PIN0_gc
		   STS ADCA_CH0_MUXCTRL,R16
    //����ADC  ת��ʹ��     
           LDI R16,ADC_ENABLE_bm|ADC_CH0START_bm
		   STS ADCA_CTRLA,R16 

	/* Wait until common mode voltage is stable. Default clk is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	       CALL _delay_ms
           uart_puts_string STRING
 
		   LDS R16,ADCA_CH0_INTFLAGS
ADCA_CH0_1_2_3_Sweep_Interrupt_0:
		   SBRS R16,ADC_CH_CHIF_bp
           JMP ADCA_CH0_1_2_3_Sweep_Interrupt_0
		   NOP
		   LDI R16,ADC_CH_CHIF_bm
           STS ADCA_CH0_INTFLAGS,R16

	/* Return result register contents*/
	       LDS XL,ADCA_CH0RES
		   LDS XH,ADCA_CH0RES+1
           uart_putw_dec
   //�ر�ADC          
           LDI R16,0X00
		   STS ADCA_CTRLA,R16 
 
	/* Setup channel 0, 1, 2 and 3 to have single ended input. */
           LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc|ADC_CH_GAIN_1X_gc
           STS ADCA_CH0_CTRL,R16
	       LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc|ADC_CH_GAIN_1X_gc
           STS ADCA_CH1_CTRL,R16
		   LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc|ADC_CH_GAIN_1X_gc
           STS ADCA_CH2_CTRL,R16
		   LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc|ADC_CH_GAIN_1X_gc
           STS ADCA_CH3_CTRL,R16
    
	/* Set input to the channels in ADC A to be PIN 0, 2, 4 and 6. */
	       LDI R16,ADC_CH_MUXPOS_PIN0_gc
		   STS ADCA_CH0_MUXCTRL,R16
		   LDI R16,ADC_CH_MUXPOS_PIN2_gc
		   STS ADCA_CH1_MUXCTRL,R16
		   LDI R16,ADC_CH_MUXPOS_PIN4_gc
		   STS ADCA_CH2_MUXCTRL,R16
		   LDI R16,ADC_CH_MUXPOS_PIN6_gc
		   STS ADCA_CH3_MUXCTRL,R16

	 /*Setup sweep of all four virtual channels.*/
		   LDI R16,ADC_SWEEP_0123_gc
		   STS ADCA_EVCTRL,R16
	/* Enable low level interrupts on ADCA channel 3, on conversion complete. */
		   LDI R16,ADC_CH_INTMODE_COMPLETE_gc|ADC_CH_INTLVL_LO_gc
		   STS ADCA_CH3_INTCTRL,R16
	      

	/* Enable PMIC interrupt level low. */
            LDI R16,PMIC_LOLVLEX_bm
	        STS PMIC_CTRL,R16
            SEI
	/* Enable ADCA with free running mode, VCC reference and signed conversion.*/
	        ;LDS R16,ADCA_CTRLA
            LDI R16,ADC_ENABLE_bm
		    STS ADCA_CTRLA,R16 

	/* Wait until common mode voltage is stable. Default clock is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	         CALL _delay_ms     
	/* Enable free running mode. */

	         LDI R16,ADC_FREERUN_bm
	         STS ADCA_CTRLB,R16
    /* Enable global interrupts. */
             RET

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
RESET:
    CLR  R25
	CALL uart_init 
	CALL ADCA_CH0_1_2_3_Sweep_Interrupt 
RESET_0:
	JMP RESET_0


/*! Interrupt routine that reads out the result form the conversion on all
 *  channels to a global array. If the number of conversions carried out is less
 *  than the number given in the define SAMPLE_COUNT a new conversion on all the
 *  channels is started. If not the interrupt on ADC A channel 3 is disabled.
 *
 */
/*
+------------------------------------------------------------------------------
| Function    : ISR_ADCA_CH2_vect
+------------------------------------------------------------------------------
| Description : 
+------------------------------------------------------------------------------
*/
ISR_ADCA_CH3_vect:
 
	/*  Read samples and clear interrupt flags. */
	INC R25
    CPI R25,5
	BRNE ISR_ADCA_CH3_vect_1
	LDI R16,0X00
	STS PMIC_CTRL,R16
    LDI R16,0X00
	STS ADCA_CTRLB,R16
ISR_ADCA_CH3_vect_1:
    LDI R16,ADC_CH_CHIF_bm
	STS ADCA_CH0_INTFLAGS,R16
	STS ADCA_CH1_INTFLAGS,R16
	STS ADCA_CH2_INTFLAGS,R16

    LDI R16,ENTER
	PUSH R16
	uart_putc
	uart_puts_string STRING0
	LDS XL,ADCA_CH0RES
    LDS XH,ADCA_CH0RES+1
    uart_putw_dec
    
    LDI R16,ENTER
	PUSH R16
	uart_putc

	uart_puts_string STRING1
    LDS XL,ADCA_CH1RES
	LDS XH,ADCA_CH1RES+1
    uart_putw_dec

	LDI R16,ENTER
	PUSH R16
	uart_putc

	uart_puts_string STRING2

	LDS XL,ADCA_CH2RES
	LDS XH,ADCA_CH2RES+1
    uart_putw_dec

	LDI R16,ENTER
	PUSH R16
	uart_putc

	uart_puts_string STRING3

	LDS XL,ADCA_CH3RES
	LDS XH,ADCA_CH3RES+1
    uart_putw_dec

	LDI R16,ENTER
	PUSH R16
	uart_putc
    RETI

_delay_ms:
         LDI R17,0xff
L0:      LDI R18,0xff
L1:      DEC R18 
         BRNE L1
		 DEC R17
		 BRNE L0
		 RET

