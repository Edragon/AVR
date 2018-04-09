/*
 * ������:
     SPI
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-20 12:54
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	������ʹ��SPIE �� SPIF ͨ�š�USARTC0���ڴ�ӡ������Ϣ
 *    - ���� PE4 �� PF4 (SS)
 *    - ���� PE5 �� PF5 (MOSI)
 *    - ���� PE6 �� PF6 (MISO)
 *    - ���� PE7 �� PF7 (SCK)

*/

.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����
.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
;.ORG 0x036        //USARTC0���ݽ�������ж����
        ;RJMP ISR
.ORG 0X100       ;�����ж���0x00-0x0FF
.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'

MAIN: .DB 'I','N','T','O','M','A','I','N',0
//�˿�E��SPIģ����Ϊ����

//����Ҫ���͵�����
masterSendData: .db 0x11, 0x22, 0x33, 0x44

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

/*! \brief Test function.
 *  1: Data is transmitted on byte at a time from the master to the slave.
 *     The slave increments the received data and sends it back. The master reads
 *     the data from the slave and verifies that it equals the data sent + 1.
 *
 *  2: Data is transmitted 4 bytes at a time to the slave. As the master sends
 *     a byte to the slave, the preceding byte is sent back to the master.
 *     When all bytes have been sent, it is verified that the last 3 bytes
 *     received at the master, equal the first 3 bytes sent.
 *
 *  The variable, 'success', will be non-zero when the function reaches the
 *  infinite for-loop if the test was successful.
 */
/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
RESET:
	call uart_init
	uart_puts_string  MAIN;uart_putc('\n');

	//SS���������������������
	LDI R16,0x10
	STS PORTE_DIRSET,R16

	 LDI R16,PORT_OPC_WIREDANDPULL_gc
	 STS PORTE_PIN4CTRL,R16   

	 //SS����
     LDI R16,0x10
	 STS PORTE_OUTSET,R16 

	//��ʼ���˿�E�ϵ�SPI����

	 LDI R16,SPI_PRESCALER_DIV4_gc|SPI_ENABLE_bm |SPI_MASTER_bm|SPI_MODE_0_gc    
	 STS SPIE_CTRL,R16
	    
	 LDI R16,SPI_INTLVL_OFF_gc
	 STS SPIE_INTCTRL,R16
     
	 LDI R16,0XB0
	 STS PORTE_DIRSET,R16

 
	 
	                                   
	 

	//��ʼ���˿�F�ϵ�SPI�ӻ�
     LDI R16,SPI_ENABLE_bm|SPI_MODE_0_gc
	 STS SPIF_CTRL,R16
	 //�жϼ���                    
	 LDI R16,SPI_INTLVL_OFF_gc
	 STS SPIF_INTCTRL,R16                
	 LDI R16,0X40
	 STS PORTF_DIRSET,R16

	
	//1.�����ֽڷ�ʽ����
	// ����: ����SS SPI_MasterTransceiveByte()������SS
	 LDI R16,0X10
	 STS PORTE_OUTCLR,R16

		//����: �����������ݵ��ӻ�
			//��������0XAA
     LDI R16,0XAA
	 STS SPIE_DATA,R16
	//�ȴ��������
RESET_1:
	 LDS R16,SPIE_STATUS
	 SBRS R16,SPI_IF_bp
	 JMP RESET_1
     nop

		//�ӻ�: �ȴ����ݿ���
RESET_2:
     LDS R16,SPIF_STATUS
	 SBRS R16,SPI_IF_bp
	 jmp RESET_2
	 nop
	 

		//�ӻ�: ȡ����

     LDI R16,ENTER //�س�����
	 PUSH R16
	 uart_putc


     LDS XH,SPIF_DATA 
	 uart_putc_hex

 	 LDI R16,ENTER //�س�����
	 PUSH R16
	 uart_putc
//��������0XBB	 
	 LDI R16,0XBB
	 STS SPIF_DATA,R16

	 LDI R16,0X00
	 STS SPIE_DATA,R16

RESET_3:
	 LDS R16,SPIE_STATUS
	 SBRS R16,SPI_IF_bp
	 JMP RESET_3
     nop

     LDI R16,ENTER //�س�����
	 PUSH R16
	 uart_putc


     LDS XH,SPIE_DATA 
	 uart_putc_hex

	 LDI R16,ENTER //�س�����
	 PUSH R16
	 uart_putc
RESET_4:
     JMP RESET_4
	 RET
	
	
