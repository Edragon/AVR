/*
 * 工程名:
     DMA_asm
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
 * 描述:通过EBI写入10个相同的数据，然后再读出数据，通过串口显示
     	 
*/
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过
.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）

.ORG 0X100       ;跳过中断区0x00-0x0FF

.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'
//块大小
.EQU MEM_BLOCK_SIZE=10
 /*存储块个数*/
.EQU MEM_BLOCK_COUNT=10
//数据个数
.EQU MEM_BLOCK=100
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
        CALL uart_init
		LDI R16,DMA_ENABLE_bm
		STS DMA_CTRL,R16
		LDS R16,DMA_CH0_CTRLA
		ORI R16,DMA_CH_ENABLE_bm
        STS DMA_CH0_CTRLA,R16

		LDI XH,0X20
        LDI XL,0X00
		LDI ZH,0X00
RESET_1:
        ST X+,ZH
		INC ZH
		CLZ
		CPI XL,100
		BRNE RESET_1
       //重复数据块传输
	    LDI R16,0X00
	    STS DMA_CH0_SRCADDR0,R16
		LDI R16,0X20
	    STS DMA_CH0_SRCADDR1,R16
		LDI R16,0X00
	    STS DMA_CH0_SRCADDR2,R16  
        
		LDI R16,0X00
	    STS DMA_CH0_DESTADDR0,R16
		LDI R16,0X30
	    STS DMA_CH0_DESTADDR1,R16
		LDI R16,0X00
 	    STS DMA_CH0_DESTADDR2,R16
		
        
		LDI R16,DMA_CH_SRCRELOAD_NONE_gc|DMA_CH_SRCDIR_INC_gc| DMA_CH_DESTRELOAD_NONE_gc|DMA_CH_DESTDIR_INC_gc
		STS DMA_CH0_ADDRCTRL,R16

		LDI R16,MEM_BLOCK_SIZE
	    STS DMA_CH0_TRFCNT,R16
		LDI R16,0X00
	    STS DMA_CH0_TRFCNT+1,R16

		LDS R16,DMA_CH0_CTRLA
		ORI R16,DMA_CH_BURSTLEN_8BYTE_gc|DMA_CH_REPEAT_bm
	    STS DMA_CH0_CTRLA,R16

        LDI R16,MEM_BLOCK_COUNT
		STS DMA_CH0_REPCNT,R16
		LDI R16,0X00
	    STS DMA_CH0_REPCNT+1,R16
    	//请求传输
		LDS R16,DMA_CH0_CTRLA
		ORI R16,DMA_CH_TRFREQ_bm
	    STS DMA_CH0_CTRLA,R16
RESET_2:        
		LDS R16,DMA_CH0_CTRLB
        ANDI R16,DMA_CH_ERRIF_bm|DMA_CH_TRNIF_bm
		CPI R16,0
		BREQ RESET_2
		LDS R16,DMA_CH0_CTRLB
        ORI R16,DMA_CH_ERRIF_bm|DMA_CH_TRNIF_bm
		STS DMA_CH0_CTRLB,R16
        //读出数据
		LDI ZH,0X30
        LDI ZL,0X00
RESET_3:
        LD XH,Z+
	    uart_putc_hex
		CLZ
		CPI ZL,100
		in R16,CPU_SREG
	    SBRS R16,1
		JMP RESET_3
        NOP
        
    	LDI R16,DMA_CH_ENABLE_bm
        STS DMA_CH0_CTRLA,R16
        //数据块单次传输
        LDI R16,0X00
	    STS DMA_CH0_SRCADDR0,R16
		LDI R16,0X20
	    STS DMA_CH0_SRCADDR1,R16
		LDI R16,0X00
	    STS DMA_CH0_SRCADDR2,R16  
        
		LDI R16,0X00
	    STS DMA_CH0_DESTADDR0,R16
		LDI R16,0X35
	    STS DMA_CH0_DESTADDR1,R16
		LDI R16,0X00
 	    STS DMA_CH0_DESTADDR2,R16
        
		LDI R16,DMA_CH_SRCRELOAD_NONE_gc|DMA_CH_SRCDIR_INC_gc|DMA_CH_DESTRELOAD_NONE_gc|DMA_CH_DESTDIR_INC_gc
		STS DMA_CH0_ADDRCTRL,R16

		LDI R16,MEM_BLOCK
	    STS DMA_CH0_TRFCNT,R16
		LDI R16,0X00
	    STS DMA_CH0_TRFCNT+1,R16

		LDS R16,DMA_CH0_CTRLA
		ORI R16,DMA_CH_BURSTLEN_8BYTE_gc
	    STS DMA_CH0_CTRLA,R16
  
    	//请求传输
		LDS R16,DMA_CH0_CTRLA
		ORI R16,DMA_CH_TRFREQ_bm
	    STS DMA_CH0_CTRLA,R16
	  
RESET_4:        
		LDS R16,DMA_CH0_CTRLB
        ANDI R16,DMA_CH_ERRIF_bm|DMA_CH_TRNIF_bm
		CPI R16,0
		BREQ RESET_4
        //读出数据
		LDI ZH,0X35
        LDI ZL,0X00
RESET_5:
        LD XH,Z+
        uart_putc_hex
		CLZ
		CPI ZL,100
		in R16,CPU_SREG
	    SBRS R16,1
		JMP RESET_5
        NOP 
RESET_6:	 
        JMP RESET_6
 
 
 