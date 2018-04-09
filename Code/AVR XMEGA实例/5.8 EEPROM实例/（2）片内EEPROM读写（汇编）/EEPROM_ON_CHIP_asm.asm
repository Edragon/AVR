/*
 * 工程名:
     EEPROM_ON_CHIP
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-21 14:15
 * 作者:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
 * 描述:
     	本程序对片内EEPROM使用不同方式进行多种操作。
		USARTC0打印调试信息。
*/

.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过
.include "usart_driver.inc"

.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
;.ORG 0x036        //USARTC0数据接收完毕中断入口
        ;RJMP ISR
.ORG 0X100       ;跳过中断区0x00-0x0FF


.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'



.equ TEST_BYTE_1=0x55
.equ TEST_BYTE_2=0xAA

.equ TEST_BYTE_ADDR_1=0x00
.equ TEST_BYTE_ADDR_2=0x08

.equ TEST_PAGE_ADDR_1=0  //页地址总是在页的边界
.equ TEST_PAGE_ADDR_2=2  //页地址总是在页的边界
.equ TEST_PAGE_ADDR_3=5  //页地址总是在页的边界

//写入EEPROM的缓存数据

MAIN: .DB 'I','N','T','O','M','A','I','N',0
testBuffer: .db 'A','c','c','e','s','s','i','n','A','c','c','e','s','s','i','n','A','c','c','e','s','s','i','n','A','c','c','e','s','s','i',0

 /*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
uart_init:

	   /* USARTC0 引脚方向设置*/
  	   /* PC3 (TXD0) 输出 */
         LDI R16,0X08
	     STS PORTC_DIRSET,R16
	   /* PC2 (RXD0) 输入 */
	     LDI R16,0X04
	     STS PORTC_DIRCLR,R16
	   /* USARTC0 模式 - 异步*/	/* USARTC0帧结构, 8 位数据位, 无校验, 1停止位 */
	     LDI R16,USART_CMODE_ASYNCHRONOUS_gc|USART_CHSIZE_8BIT_gc|USART_PMODE_DISABLED_gc
	     STS USARTC0_CTRLC,R16
	   /* 设置波特率 9600*/
         LDI R16,12
	     STS USARTC0_BAUDCTRLA,R16
	     LDI R16,0
	     STS USARTC0_BAUDCTRLB,R16
	   /* USARTC0 使能发送*//* USARTC0 使能接收*/
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
	//写地址
	sts NVM_ADDR0,r16
	sts NVM_ADDR1,r17
	sts NVM_ADDR2,r18

	//执行读命令
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
	//写地址
	sts NVM_ADDR0,r17
	sts NVM_ADDR1,r18
	sts NVM_ADDR2,r19
	 

	//加载数据触发命令执行
	 sts NVM_DATA0,r16

	//触发原子操作（擦&写）写签名，执行命令
	 ldi r16,NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc
	 sts NVM_CMD,r16
	 call NVM_EXEC 
.ENDMACRO

.MACRO EEPROM_ErasePage
	//等待NVM不忙
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

	LDI R16,ENTER//回车换行
	PUSH R16
    uart_putc

	uart_puts_string  MAIN;uart_putc('\n');

	LDI R16,ENTER
	PUSH R16
	uart_putc



	//1 清空缓存
    call EEPROM_FlushBuffer
//关闭EEPROM映射到内存空间
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
		//读取写入的字节  
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

 	LDI R16,ENTER//回车换行
	PUSH R16
    uart_putc



	//2.分离操作写一整页

	//加载页缓存，先擦页后写页
	call EEPROM_WaitForNVM
	ldi r16,NVM_CMD_LOAD_EEPROM_BUFFER_gc
	sts NVM_CMD,r16

	//地址清零，只有低几位使用，再循环内会改变
	LDI r16,0x00
	STS NVM_ADDR1,r16
	LDI r16,0x00
	sts NVM_ADDR2,r16

	//加载多个字节到缓存
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

    	//等待NVM不忙
	call EEPROM_WaitForNVM

	//写地址
	ldi r16,0x00
	sts NVM_ADDR0,r16
	sts NVM_ADDR2,r16
	ldi r16,0x02
    sts NVM_ADDR1,r16

	//触发写EEPROM页命令
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

	LDI R16,ENTER//回车换行
	PUSH R16
    uart_putc



//3 EEPROM映射到内存空间
	ldi r16,NVM_EEMAPEN_bm
	sts NVM_CTRLB,r16
	

	//写2个字节
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
	//读取2个字节
	call EEPROM_WaitForNVM

    ldi zh,0x10
	ldi zl,0x00
    ld xh,z+
	ld xl,z
    uart_putw_hex

	LDI R16,ENTER//回车换行
	PUSH R16
    uart_putc


  
    

	//4.使用内存映射方式，分离操作，写一页缓存到EEPROM
//地址清零，只有低几位使用，再循环内会改变
	ldi r16,NVM_EEMAPEN_bm
	sts NVM_CTRLB,r16

	ldi r16,NVM_CMD_LOAD_EEPROM_BUFFER_gc
	sts NVM_CMD,r16
	

    call EEPROM_WaitForNVM

	//加载多个字节到缓存
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

    	//等待NVM不忙
	call EEPROM_WaitForNVM

	//写地址
	ldi r16,0x00
	sts NVM_ADDR0,r16
	ldi r16,0x00
	sts NVM_ADDR2,r16
	ldi r16,0x13
    sts NVM_ADDR1,r16

	//触发写EEPROM页命令
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
