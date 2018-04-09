/*
 * 工程名:
     SPI
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-20 12:54
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序使用SPIE 和 SPIF 通信。USARTC0串口打印调试信息
 *    - 连接 PE4 到 PF4 (SS)
 *    - 连接 PE5 到 PF5 (MOSI)
 *    - 连接 PE6 到 PF6 (MISO)
 *    - 连接 PE7 到 PF7 (SCK)

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

MAIN: .DB 'I','N','T','O','M','A','I','N',0
//端口E的SPI模块作为主机

//主机要发送的数据
masterSendData: .db 0x11, 0x22, 0x33, 0x44

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

	//SS引脚线与上拉，方向输出
	LDI R16,0x10
	STS PORTE_DIRSET,R16

	 LDI R16,PORT_OPC_WIREDANDPULL_gc
	 STS PORTE_PIN4CTRL,R16   

	 //SS拉高
     LDI R16,0x10
	 STS PORTE_OUTSET,R16 

	//初始化端口E上的SPI主机

	 LDI R16,SPI_PRESCALER_DIV4_gc|SPI_ENABLE_bm |SPI_MASTER_bm|SPI_MODE_0_gc    
	 STS SPIE_CTRL,R16
	    
	 LDI R16,SPI_INTLVL_OFF_gc
	 STS SPIE_INTCTRL,R16
     
	 LDI R16,0XB0
	 STS PORTE_DIRSET,R16

 
	 
	                                   
	 

	//初始化端口F上的SPI从机
     LDI R16,SPI_ENABLE_bm|SPI_MODE_0_gc
	 STS SPIF_CTRL,R16
	 //中断级别                    
	 LDI R16,SPI_INTLVL_OFF_gc
	 STS SPIF_INTCTRL,R16                
	 LDI R16,0X40
	 STS PORTF_DIRSET,R16

	
	//1.单个字节方式传输
	// 主机: 拉低SS SPI_MasterTransceiveByte()不控制SS
	 LDI R16,0X10
	 STS PORTE_OUTCLR,R16

		//主机: 主机传输数据到从机
			//发送数据0XAA
     LDI R16,0XAA
	 STS SPIE_DATA,R16
	//等待传输完成
RESET_1:
	 LDS R16,SPIE_STATUS
	 SBRS R16,SPI_IF_bp
	 JMP RESET_1
     nop

		//从机: 等待数据可用
RESET_2:
     LDS R16,SPIF_STATUS
	 SBRS R16,SPI_IF_bp
	 jmp RESET_2
	 nop
	 

		//从机: 取数据

     LDI R16,ENTER //回车换行
	 PUSH R16
	 uart_putc


     LDS XH,SPIF_DATA 
	 uart_putc_hex

 	 LDI R16,ENTER //回车换行
	 PUSH R16
	 uart_putc
//发送数据0XBB	 
	 LDI R16,0XBB
	 STS SPIF_DATA,R16

	 LDI R16,0X00
	 STS SPIE_DATA,R16

RESET_3:
	 LDS R16,SPIE_STATUS
	 SBRS R16,SPI_IF_bp
	 JMP RESET_3
     nop

     LDI R16,ENTER //回车换行
	 PUSH R16
	 uart_putc


     LDS XH,SPIE_DATA 
	 uart_putc_hex

	 LDI R16,ENTER //回车换行
	 PUSH R16
	 uart_putc
RESET_4:
     JMP RESET_4
	 RET
	
	
