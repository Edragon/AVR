/*
 * 工程名:
     USART
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-03-22 21:35
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序给出USARTC0串行收发示例
*/
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过

.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
.ORG 0x032        //USARTC0数据接收完毕中断入口
        RJMP ISR
 
.ORG 0X100       ;跳过中断区0x00-0x0F4
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
			  
			
			 /*USARTC0 接收低级断级别*/
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
| Description : USARTC0接收中断函数 收到的数据发送回去USARTC0
+------------------------------------------------------------------------------
*/

ISR:       
              LDS R17,USARTC0_DATA
              USART_IsTXDataRegisterEmpty USART_DREIF_bp
              STS USARTC0_DATA,R17
			  RETI
 
              
            
