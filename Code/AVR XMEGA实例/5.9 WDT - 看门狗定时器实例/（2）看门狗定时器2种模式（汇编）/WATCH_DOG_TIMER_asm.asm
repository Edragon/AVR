/*
 * ������:
     WATCH_DOG_TIMER
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
     	void wdt_fuse_enable_example( void );
		��˿�趨���Ź���ʱ���ڣ��������ڣ�����������ģʽ

		void wdt_sw_enable_example( void );
		���������Ź���ͨģʽ������ģʽ

		USARTC0��ӡ������Ϣ��
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

//���Ź���ʱ���ں�����
.equ TO_WD=128
//���Ź��ر����ں�����
.equ TO_WDW=64

//����ģʽ�¿��Ź����ü��ʱ��
.equ WINDOW_MODE_DELAY=128

//��ͨģʽ�¿��Ź����ü��ʱ��
.equ NORMAL_MODE_DELAY=64
MAIN: .DB 'I','N','T','O','M','A','I','N',0
sw: .db 'I','N','T','O','s','w',0
fuse: .db 'I','N','T','O','f','u','s','e',0
WDTenabled: .db  'w','d','T','e','n','a','b','l','e','d',0
WDTunenabled: .db  'w','d','T','u','n','e','n','a','b','l','e','d',0
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

	call uart_init 

	LDI R16,ENTER//�س�����
	PUSH R16
    uart_putc

	uart_puts_string  MAIN;uart_putc('\n');

	LDI R16,ENTER
	PUSH R16
	uart_putc

	//wdt_sw_enable_example();
	call wdt_sw_enable_example





/*
+------------------------------------------------------------------------------
| Function    : wdt_sw_enable_example
+------------------------------------------------------------------------------
| Description : �����У���������ģʽ��WD��ʹ�ܣ�ע�⣺WDLOCK��˿λ������λ
+------------------------------------------------------------------------------
*/
wdt_sw_enable_example:
	uart_puts_string  sw 

	LDI R16,ENTER
	PUSH R16
	uart_putc

	//ʹ�ܿ��Ź���ͨģʽ����ʱ����Ϊ32CLK=32 ms
	ldi r16,WDT_ENABLE_bm | WDT_CEN_bm | WDT_PER_32CLK_gc
	ldi R17,CCP_IOREG_gc
	STS CPU_CCP,R17
	STS WDT_CTRL,R16

    
wdt_sw_enable_example_1:
    lds r16,WDT_STATUS 
    sbrc r16,WDT_SYNCBUSY_bp
	jmp wdt_sw_enable_example_1
	nop

	lds r16,WDT_CTRL
	sbrs r16,WDT_ENABLE_bp
	jmp wdt_sw_enable_example_2
	nop
    uart_puts_string  WDTenabled
    jmp wdt_sw_enable_example_3
wdt_sw_enable_example_2:
    uart_puts_string  WDTunenabled
wdt_sw_enable_example_3:
    wdr 
    jmp wdt_sw_enable_example_3
     
