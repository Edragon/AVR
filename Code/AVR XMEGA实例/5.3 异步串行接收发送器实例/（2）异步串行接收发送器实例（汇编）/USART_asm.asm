/*
 * ������:
     USART
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-03-22 21:35
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	���������USARTC0�����շ�ʾ��
*/
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����

.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
.ORG 0x032        //USARTC0���ݽ�������ж����
        RJMP ISR
 
.ORG 0X100       ;�����ж���0x00-0x0F4
.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'
STRING: .DB 'W','W','W','.','U','P','C','.','E','D','U','.','C','N',0

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
| Function    : main
+------------------------------------------------------------------------------
*/

RESET:

	         CALL uart_init 

		     LDI R16,ENTER
			 PUSH R16
	         uart_putc

			 EOR XH,XH
			 EOR XL,XL
			 COM XH
			 COM XL
			 uart_putw_hex

			 LDI R16,EQUE
			 PUSH R16
			 uart_putc

             EOR XH,XH
			 EOR XL,XL
			 COM XH
			 COM XL
			 uart_putw_dec
		    
			 LDI R16,ENTER
			 PUSH R16
	         uart_putc
	      
			 uart_puts_string STRING
 
	         LDI R16,ENTER
			 PUSH R16
	         uart_putc
			  
			
			 /*USARTC0 ���յͼ��ϼ���*/
	         LDI R16,USART_RXCINTLVL_LO_gc
	         STS USARTC0_CTRLA,R16
	         /* Enable interrupts.*/
	         LDI R16,PMIC_LOLVLEN_bm
	         STS PMIC_CTRL,R16//Enable Low_Level interrupts
 	         SEI
 RESET_LOOP:
             NOP
           	 NOP
	         JMP RESET_LOOP

/*
+------------------------------------------------------------------------------
| Function    : ISR(USARTE1_RXC_vect)
+------------------------------------------------------------------------------
| Description : USARTC0�����жϺ��� �յ������ݷ��ͻ�ȥUSARTC0
+------------------------------------------------------------------------------
*/

ISR:       
              LDS R17,USARTC0_DATA
              USART_IsTXDataRegisterEmpty USART_DREIF_bp
              STS USARTC0_DATA,R17
			  RETI
 
              
            
