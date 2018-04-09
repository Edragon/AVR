/*
 * 工程名:
     EBI_asm
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
/* 测试数据 */
.EQU TESTBYTE=0xA6

//SRAM大小是64K

//基地址
.EQU SRAM_ADDR=0x4000
//写的数据个数
.EQU WRITE_NUM=0x0A

/*  Hardware setup for 3-port SRAM interface:
 *
 *  PORTK[7:0] - A[7:0]/A[15:8]/A[23:16] (BYTE 2 and 3 connected through ALE1 and ALE2)
 *
 *  PORTJ[7:0] - D[7:0]
 *
 *  PORTH[7:0] - {CS3,CS2,CS1,CS0,ALE2,ALE1,RE,WE} (CS0 used for SRAM)*/
 
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
	  /* 设置总线引脚 */
	    LDI R16,0XFF
	    STS PORTH_DIR,R16
	    STS PORTK_DIR,R16
		LDI R16,0X00
	    STS PORTJ_DIR,R16
	    CALL uart_init

	   /* 初始化EBI */
	    LDI R16,EBI_SDDATAW_8BIT_gc|EBI_LPCMODE_ALE1_gc|EBI_SRMODE_ALE2_gc|EBI_IFMODE_3PORT_gc
        STS EBI_CTRL,R16


	   /*初始化SRAM*/
	    LDI R16,EBI_CS_SRWS_gm
	    STS EBI_CS0_CTRLB,R16
    
	    LDI R16,0x00
        STS EBI_CS0_BASEADDR,R16 //基地址必须是外接地址空间大小的倍数
        STS EBI_CS0_BASEADDR+1,R16

	    LDI R16,EBI_CS_MODE_SRAM_gc|EBI_CS_ASPACE_64KB_gc
	    STS EBI_CS0_CTRLA,R16            

	    /* 写数据*/
	    LDI R30,0X00
	    LDI R31,0X40
	    in R17,CPU_RAMPZ

		LDI R16,0X00
	    OUT CPU_RAMPZ,R16
RESET_1:
	   
	    LDI R16,TESTBYTE
	    ST Z,R16
		INC R30
        CLZ
    	CPI R30,10
    	BRNE RESET_1
	    OUT CPU_RAMPZ,R17
	 
	    /*读数据*/
	    LDI R30,0X00
	    LDI R31,0X40
	    in R17, CPU_RAMPZ

		LDI R16,0X00
	    OUT CPU_RAMPZ,R16
RESET_2:
        
	    LD  XH,Z+
		uart_putc_hex
        CLZ
	    CPI R30,10
        in R16,CPU_SREG
	    SBRS R16,1
		JMP RESET_2
		NOP
	    OUT CPU_RAMPZ,R17
RESET_3:JMP  RESET_3
