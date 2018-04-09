/*
 * ������:
     I2C_AT24C02
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-20 10:44
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	������ʹ��TWIF�ʹ���EEPROM AT24C02ͨ�ţ���AT24C02
		��2��д��16���ֽڣ��ٶ���ָ��Ƭ���ֽڵ�ַ�������ݡ�
		24C02�д���Ƭ�ڵ�ַ�Ĵ�����ÿд������һ��������
		�ں󣬸õ�ַ�Ĵ����Զ���1����ʵ�ֶ���һ���洢��Ԫ
		�Ķ�д�������ֽھ��Ե�һ������ʽ��ȡ��Ϊ�����ܵ�
		д��ʱ�䣬һ�β�����д����8���ֽڵ����ݡ�

*/

.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����
.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
;.ORG 0x036        //USARTC0���ݽ�������ж����
        ;RJMP ISR
.ORG  0X01A
		RJMP ISRT_TWIC_TWIM_vect

.ORG 0X100       ;�����ж���0x00-0x0FF

.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'
//������ַ 0B0 1010 000
.equ DEVICE_ADDRESS=0x50

//�����ֽ���
.equ NUM_BYTES=9

// CPU 2MHz
// ������100kHz
.equ TWI_BAUDSETTING=5
//Ƭ���ֽڵ�ַ
.equ WORD_ADDRESS=0x00

//��������
sendBuffer: .DB 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x80//0x00дʱ�ֽڵ�ַ 0x80�ǽ�����־
MAIN:       .DB 'I','N','T','O','M','A','I','N',0
writedata:  .DB 'w','r','i','t','e','d','a','t','a',0
readdata:   .DB 'r','e','a','d','d','a','t','a',0
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
/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
RESET:

// PORTD ��Ϊ���
	    CALL uart_init

	    LDI R16,ENTER
	    PUSH R16
	    uart_putc
 
	    uart_puts_string MAIN


	    LDI R16,0X03
	    STS PORTD_DIRSET,R16

	// ����ⲿû�н��������裬ʹ������ PC0(TWI-SDA), PC1(TWI-SCL)
	    LDI R16,0X03
	    STS PORTCFG_MPCMASK,R16 // һ�����ö������
	    LDI R16,PORT_OPC_PULLUP_gc
	    STS PORTC_PIN0CTRL,R16
      
	    
		
		LDI R16,TWI_BAUDSETTING
	 
	    STS TWIC_MASTER_BAUD,R16

	    LDI R16,TWI_MASTER_INTLVL_LO_gc|TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm |TWI_MASTER_ENABLE_bm
	
	    STS TWIC_MASTER_CTRLA,R16

		ldi r16,TWI_MASTER_BUSSTATE_IDLE_gc
	    sts twic_MASTER_STATUS,r16

	 

     //ʹ�ܵͼ����ж�
	    LDI R16,PMIC_LOLVLEN_bm
	    STS PMIC_CTRL,R16
	    SEI

RESET_1:
	 
	    LDI R16,ENTER
	    PUSH R16
	    uart_putc
 
	    uart_puts_string writedata

		LDI R16,ENTER
	    PUSH R16
	    uart_putc

		//������������
		// ����bytesToWrite�ж���д, ����START�ź� + 7λ��ַ +  R/_W = 0
		LDI R16,DEVICE_ADDRESS<<1&0x0fe
		STS TWIC_MASTER_ADDR,R16
		ldi r16,0x00
		sts  GPIO_GPIOR0,r16//��־λ
		sts  GPIO_GPIOR1,r16
		sts  GPIO_GPIOR2,r16//����
	
	    
RESET_2:
        lds r16,GPIO_GPIOR0		
        cpi r16,0x01
		brne RESET_2

//�����и��ӳ� ��ΪоƬ���յ�stop�ź�ʱ�ſ�ʼд��֮ǰ�������İ˸��ֽ�
		LDI R16,ENTER
	    PUSH R16
	    uart_putc
 
	    uart_puts_string readdata

		LDI R16,ENTER
	    PUSH R16
	    uart_putc
	 
		//����αдָ��Ŀ����ȷ��Ҫ��ȡ���ݵĵ�ַ
        LDI R16,DEVICE_ADDRESS<<1&0x0fe//αдָ�� Ŀ����Ϊ��ȷ����ȡ�ĵ�ַ������������ַ �ֽڵ�ַ0x00 ��ȷ����ȡ���ݵĵ�ַ��
		STS TWIC_MASTER_ADDR,R16
        ldi r16,0x00
		sts  GPIO_GPIOR0,r16
		ldi r16,0x01
		sts  GPIO_GPIOR1,r16//��־λ
	 
RESET_4:
        lds r16,GPIO_GPIOR1		
        cpi r16,0x00
		brne RESET_4
        
		 
		LDI R16,DEVICE_ADDRESS<<1|0x01
		STS TWIC_MASTER_ADDR,R16

		ldi r16,0x00
		sts  GPIO_GPIOR0,r16

RESET_5:
        lds r16,GPIO_GPIOR0		
        cpi r16,0x01
		brne RESET_5

RESET_6:  
        jmp RESET_6
	 
		
	 

// TWIF �����жϷ������
ISRT_TWIC_TWIM_vect:
         
	      LDS  R16,TWIC_MASTER_STATUS
          SBRS R16,TWI_MASTER_WIF_bp
	      JMP ISRT_TWIC_TWIM_vect_2//���ж�
		  nop
		  sbrs r16,4
          jmp ISRT_TWIC_TWIM_vect_0
		  NOP
          jmp ISRT_TWIC_TWIM_vect_5


ISRT_TWIC_TWIM_vect_0://д�ж�
          lds r16,GPIO_GPIOR1
          sbrc r16,0
		  jmp ISRT_TWIC_TWIM_vect_1
          LDI ZH,HIGH(sendBuffer<<1)
	      LDI ZL,LOW(sendBuffer<<1)
		  lds r19,GPIO_GPIOR2	
		  add zl,r19
          lpm r16,z+
		  inc r19
		  sts GPIO_GPIOR2,r19
		  sbrc r16,7
		  jmp ISRT_TWIC_TWIM_vect_4
		  mov xh,r16
          uart_putc_hex
		  nop
	      STS TWIC_MASTER_DATA,R16
		  lds r16,TWIC_MASTER_STATUS
          ori R16,TWI_MASTER_ARBLOST_bm|TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm
          sts TWIC_MASTER_STATUS,R16
		  jmp ISRT_TWIC_TWIM_vect_3
ISRT_TWIC_TWIM_vect_1:
          
         lds r16,GPIO_GPIOR0		
         sbrc r16,0
		 jmp ISRT_TWIC_TWIM_vect_4
		 nop
         ldi r16,0x00
         STS TWIC_MASTER_DATA,R16
         lds r16,TWIC_MASTER_STATUS
         ori R16,TWI_MASTER_ARBLOST_bm|TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm
         sts TWIC_MASTER_STATUS,R16
         ldi r16,0x01
		 sts  GPIO_GPIOR0,r16
		 
		 JMP ISRT_TWIC_TWIM_vect_3 


ISRT_TWIC_TWIM_vect_2:
           
          LDS xh,TWIC_MASTER_DATA
		  uart_putc_hex
		  lds r19,GPIO_GPIOR2
          dec r19
          clz
		  cpi r19,0x02
		  breq  ISRT_TWIC_TWIM_vect_4
		  sts GPIO_GPIOR2,r19
          LDI R16,TWI_MASTER_CMD_RECVTRANS_gc
	      STS TWIC_MASTER_CTRLC,R16
ISRT_TWIC_TWIM_vect_5:
		  lds r16,TWIC_MASTER_STATUS
          ori R16,TWI_MASTER_ARBLOST_bm|TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm
          sts TWIC_MASTER_STATUS,R16
		  jmp ISRT_TWIC_TWIM_vect_3
ISRT_TWIC_TWIM_vect_4:

          LDI R16,TWI_MASTER_ACKACT_bm|TWI_MASTER_CMD_STOP_gc
		  STS TWIC_MASTER_CTRLC,R16 
		  lds r16,TWIC_MASTER_STATUS
          ori R16,TWI_MASTER_ARBLOST_bm|TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm
          sts TWIC_MASTER_STATUS,R16
		  ldi r16,0x01
		  sts GPIO_GPIOR0,r16
		  ldi r16,0x00
		  sts GPIO_GPIOR1,r16

ISRT_TWIC_TWIM_vect_3:
           reti

_delay_ms:
L0:      LDI R18,250
L1:      DEC R18 
         BRNE L1
		 DEC R17
		 BRNE L0
		 RET
