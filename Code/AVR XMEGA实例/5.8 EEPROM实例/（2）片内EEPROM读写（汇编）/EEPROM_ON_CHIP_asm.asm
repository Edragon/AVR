/*
 * ������:
     EEPROM_ON_CHIP
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-21 14:15
 * ����:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
 * ����:
     	�������Ƭ��EEPROMʹ�ò�ͬ��ʽ���ж��ֲ�����
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



.equ TEST_BYTE_1=0x55
.equ TEST_BYTE_2=0xAA

.equ TEST_BYTE_ADDR_1=0x00
.equ TEST_BYTE_ADDR_2=0x08

.equ TEST_PAGE_ADDR_1=0  //ҳ��ַ������ҳ�ı߽�
.equ TEST_PAGE_ADDR_2=2  //ҳ��ַ������ҳ�ı߽�
.equ TEST_PAGE_ADDR_3=5  //ҳ��ַ������ҳ�ı߽�

//д��EEPROM�Ļ�������

MAIN: .DB 'I','N','T','O','M','A','I','N',0
testBuffer: .db 'A','c','c','e','s','s','i','n','A','c','c','e','s','s','i','n','A','c','c','e','s','s','i','n','A','c','c','e','s','s','i',0

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
| Function    : 
+------------------------------------------------------------------------------
*/


NVM_EXEC:
    push r30 
	push r31
    push r16
    push r18
	ldi r30, 0xCB
	ldi r31, 0x01
	ldi r16, 0xD8
	ldi r18, 0x01
	out 0x34, r16
	st Z, r18
    pop r18
	pop r16
	pop r31
	pop r30
	ret


EEPROM_FlushBuffer:

    call EEPROM_WaitForNVM
	nop
EEPROM_FlushBuffer_1:
    LDS r16,NVM_STATUS
    SBRC r16,NVM_EELOAD_bp
	jmp EEPROM_FlushBuffer_1
    nop
	ldi r16,NVM_CMD_ERASE_EEPROM_BUFFER_gc
	sts NVM_CMD,r16
	call NVM_EXEC
	ret

EEPROM_WaitForNVM:

EEPROM_WaitForNVM_1:
    lds r16,NVM_STATUS
	sbrc r16,NVM_NVMBUSY_bp
	JMP EEPROM_WaitForNVM_1
	nop
	ret

.MACRO EEPROM_ReadByte
	call EEPROM_WaitForNVM
	pop r16
	pop r17
	pop r18
	//д��ַ
	sts NVM_ADDR0,r16
	sts NVM_ADDR1,r17
	sts NVM_ADDR2,r18

	//ִ�ж�����
	ldi r16,NVM_CMD_READ_EEPROM_gc
	sts NVM_CMD,r16
	call NVM_EXEC 
    lds xh,NVM_DATA0
	PUSH XH
	uart_putc 
.ENDMACRO

.MACRO EEPROM_WriteByte
    call EEPROM_FlushBuffer 
    pop r16
	pop r17
	pop r18
	pop r19
	LDI r20,NVM_CMD_LOAD_EEPROM_BUFFER_gc
	sts NVM_CMD,r20
	//д��ַ
	sts NVM_ADDR0,r17
	sts NVM_ADDR1,r18
	sts NVM_ADDR2,r19
	 

	//�������ݴ�������ִ��
	 sts NVM_DATA0,r16

	//����ԭ�Ӳ�������&д��дǩ����ִ������
	 ldi r16,NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc
	 sts NVM_CMD,r16
	 call NVM_EXEC 
.ENDMACRO

.MACRO EEPROM_ErasePage
	//�ȴ�NVM��æ
	call EEPROM_WaitForNVM
    pop r16
	pop r17
	pop r18
	
	 
	sts NVM_ADDR0,r16
	sts NVM_ADDR2,r18
	sts NVM_ADDR1,r17

    ldi r16,NVM_CMD_ERASE_EEPROM_PAGE_gc
	sts NVM_CMD,r16
	call NVM_EXEC
.ENDMACRO


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



	//1 ��ջ���
    call EEPROM_FlushBuffer
//�ر�EEPROMӳ�䵽�ڴ�ռ�
    ldi r16,~NVM_EEMAPEN_bm
	sts NVM_CTRLB,r16

	ldi r16,0x00
	push r16
	push r16
	push r16
	ldi r16,0x61
	push r16
   EEPROM_WriteByte ;0x00 0x00 0x00 0x55
    ldi r16,0x00
	push r16
	push r16
	ldi r16,0x08
	push r16
	ldi r16,0x62
	push r16
   EEPROM_WriteByte ;0x00 0x00 0x08 0xaa*/
		//��ȡд����ֽ�  
    ldi r16,0x00
	push r16
	push r16
	push r16
    EEPROM_ReadByte 
    ldi r16,0x00
	push r16
	push r16
	ldi r16,0x08
	push r16
   EEPROM_ReadByte 

 	LDI R16,ENTER//�س�����
	PUSH R16
    uart_putc



	//2.�������дһ��ҳ

	//����ҳ���棬�Ȳ�ҳ��дҳ
	call EEPROM_WaitForNVM
	ldi r16,NVM_CMD_LOAD_EEPROM_BUFFER_gc
	sts NVM_CMD,r16

	//��ַ���㣬ֻ�еͼ�λʹ�ã���ѭ���ڻ�ı�
	LDI r16,0x00
	STS NVM_ADDR1,r16
	LDI r16,0x00
	sts NVM_ADDR2,r16

	//���ض���ֽڵ�����
	eor r20,r20
	LDI ZH,HIGH(testBuffer<<1)
	LDI ZL,LOW(testBuffer<<1)

RESET_1:
   
	sts NVM_ADDR0,r20
	inc r20
	LPM R16,Z+
    
    sts NVM_DATA0,r16
	clz 
	cpi r16,0
	brne RESET_1
	
	ldi r16,0x00
	push r16
	ldi r16,0x02
	push r16
	ldi r16,0x00
	push r16 

   EEPROM_ErasePage

    	//�ȴ�NVM��æ
	call EEPROM_WaitForNVM

	//д��ַ
	ldi r16,0x00
	sts NVM_ADDR0,r16
	sts NVM_ADDR2,r16
	ldi r16,0x02
    sts NVM_ADDR1,r16

	//����дEEPROMҳ����
	ldi r16,NVM_CMD_WRITE_EEPROM_PAGE_gc
	sts NVM_CMD,r16 
	call NVM_EXEC

	eor r20,r20
RESET_4:
    ldi r16,0x00
	push r16
	ldi r16,0x02
	push r16
	mov r16,r20
	INC R20
	push r16
    
	EEPROM_ReadByte

	CPI XH,0
	BRNE RESET_4

	LDI R16,ENTER//�س�����
	PUSH R16
    uart_putc



//3 EEPROMӳ�䵽�ڴ�ռ�
	ldi r16,NVM_EEMAPEN_bm
	sts NVM_CTRLB,r16
	

	//д2���ֽ�
	call EEPROM_WaitForNVM 
	ldi zh,0x10
	ldi zl,0x00
    ldi r16,0xdc
	st z+,r16
	call EEPROM_WaitForNVM

	ldi r16,0x00
	sts NVM_ADDR0,r16
	sts NVM_ADDR2,r16
	ldi r16,0x10
    sts NVM_ADDR1,r16
    
	ldi r16,NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc
	sts NVM_CMD,r16
	call NVM_EXEC

    
	call EEPROM_WaitForNVM 
    ldi r16,0xaa
    st z,r16
	
	ldi r16,0x01
	sts NVM_ADDR0,r16
	ldi r16,0x00
	sts NVM_ADDR2,r16
	ldi r16,0x10
    sts NVM_ADDR1,r16
    
	ldi r16,NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc
	sts NVM_CMD,r16
	call NVM_EXEC
	//��ȡ2���ֽ�
	call EEPROM_WaitForNVM

    ldi zh,0x10
	ldi zl,0x00
    ld xh,z+
	ld xl,z
    uart_putw_hex

	LDI R16,ENTER//�س�����
	PUSH R16
    uart_putc


  
    

	//4.ʹ���ڴ�ӳ�䷽ʽ�����������дһҳ���浽EEPROM
//��ַ���㣬ֻ�еͼ�λʹ�ã���ѭ���ڻ�ı�
	ldi r16,NVM_EEMAPEN_bm
	sts NVM_CTRLB,r16

	ldi r16,NVM_CMD_LOAD_EEPROM_BUFFER_gc
	sts NVM_CMD,r16
	

    call EEPROM_WaitForNVM

	//���ض���ֽڵ�����
	LDI ZH,HIGH(testBuffer<<1)
	LDI ZL,LOW(testBuffer<<1)
	ldi xh,0x13
	ldi xl,0x00

RESET_2: 
	LPM R16,Z+
	st x+,r16
	clz 
	cpi r16,0
	brne RESET_2
	
	ldi r16,0x00
	push r16
	ldi r16,0x13
	push r16
	ldi r16,0x00
	push r16 

    EEPROM_ErasePage

    	//�ȴ�NVM��æ
	call EEPROM_WaitForNVM

	//д��ַ
	ldi r16,0x00
	sts NVM_ADDR0,r16
	ldi r16,0x00
	sts NVM_ADDR2,r16
	ldi r16,0x13
    sts NVM_ADDR1,r16

	//����дEEPROMҳ����
	ldi r16,NVM_CMD_WRITE_EEPROM_PAGE_gc
	sts NVM_CMD,r16 
	call NVM_EXEC
    
	call EEPROM_WaitForNVM
	eor r20,r20
	ldi zh,0x13
	ldi zl,0x00
RESET_5:
   
    ld xh,z+
	PUSH XH
	uart_putc 

	CPI XH,0
	BRNE RESET_5

RESET_3:

    JMP RESET_3  
