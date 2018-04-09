/*
 * 工程名:
     WATCH_DOG_TIMER
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-23 14:03
 * 作者:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
 * 描述:
     	void wdt_fuse_enable_example( void );
		熔丝设定看门狗超时周期，窗口周期，程序开启窗口模式

		void wdt_sw_enable_example( void );
		程序开启看门狗普通模式，窗口模式

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

//看门狗超时周期毫秒数
.equ TO_WD=128
//看门狗关闭周期毫秒数
.equ TO_WDW=64

//窗口模式下看门狗重置间隔时间
.equ WINDOW_MODE_DELAY=128

//普通模式下看门狗重置间隔时间
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

	//wdt_sw_enable_example();
	call wdt_sw_enable_example





/*
+------------------------------------------------------------------------------
| Function    : wdt_sw_enable_example
+------------------------------------------------------------------------------
| Description : 程序中，开启窗口模式（WD已使能）注意：WDLOCK熔丝位不能置位
+------------------------------------------------------------------------------
*/
wdt_sw_enable_example:
	uart_puts_string  sw 

	LDI R16,ENTER
	PUSH R16
	uart_putc

	//使能看门狗普通模式，超时周期为32CLK=32 ms
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
     
