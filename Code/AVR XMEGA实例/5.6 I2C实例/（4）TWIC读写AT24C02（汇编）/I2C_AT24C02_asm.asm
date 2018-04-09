/*
 * 工程名:
     I2C_AT24C02
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-20 10:44
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序使用TWIF和串行EEPROM AT24C02通信，向AT24C02
		分2次写入16个字节，再读出指定片内字节地址处的数据。
		24C02中带有片内地址寄存器。每写入或读出一个数据字
		节后，该地址寄存器自动加1，以实现对下一个存储单元
		的读写。所有字节均以单一操作方式读取。为降低总的
		写入时间，一次操作可写入多达8个字节的数据。

*/

.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过
.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
;.ORG 0x036        //USARTC0数据接收完毕中断入口
        ;RJMP ISR
.ORG  0X01A
		RJMP ISRT_TWIC_TWIM_vect

.ORG 0X100       ;跳过中断区0x00-0x0FF

.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'
//器件地址 0B0 1010 000
.equ DEVICE_ADDRESS=0x50

//缓冲字节数
.equ NUM_BYTES=9

// CPU 2MHz
// 波特率100kHz
.equ TWI_BAUDSETTING=5
//片内字节地址
.equ WORD_ADDRESS=0x00

//测试数据
sendBuffer: .DB 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x80//0x00写时字节地址 0x80是结束标志
MAIN:       .DB 'I','N','T','O','M','A','I','N',0
writedata:  .DB 'w','r','i','t','e','d','a','t','a',0
readdata:   .DB 'r','e','a','d','d','a','t','a',0
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

// PORTD 设为输出
	    CALL uart_init

	    LDI R16,ENTER
	    PUSH R16
	    uart_putc
 
	    uart_puts_string MAIN


	    LDI R16,0X03
	    STS PORTD_DIRSET,R16

	// 如果外部没有接上拉电阻，使能上拉 PC0(TWI-SDA), PC1(TWI-SCL)
	    LDI R16,0X03
	    STS PORTCFG_MPCMASK,R16 // 一次配置多个引脚
	    LDI R16,PORT_OPC_PULLUP_gc
	    STS PORTC_PIN0CTRL,R16
      
	    
		
		LDI R16,TWI_BAUDSETTING
	 
	    STS TWIC_MASTER_BAUD,R16

	    LDI R16,TWI_MASTER_INTLVL_LO_gc|TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm |TWI_MASTER_ENABLE_bm
	
	    STS TWIC_MASTER_CTRLA,R16

		ldi r16,TWI_MASTER_BUSSTATE_IDLE_gc
	    sts twic_MASTER_STATUS,r16

	 

     //使能低级别中断
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

		//主机发送数据
		// 根据bytesToWrite判断是写, 发送START信号 + 7位地址 +  R/_W = 0
		LDI R16,DEVICE_ADDRESS<<1&0x0fe
		STS TWIC_MASTER_ADDR,R16
		ldi r16,0x00
		sts  GPIO_GPIOR0,r16//标志位
		sts  GPIO_GPIOR1,r16
		sts  GPIO_GPIOR2,r16//计数
	
	    
RESET_2:
        lds r16,GPIO_GPIOR0		
        cpi r16,0x01
		brne RESET_2

//必须有个延迟 因为芯片在收到stop信号时才开始写入之前发给他的八个字节
		LDI R16,ENTER
	    PUSH R16
	    uart_putc
 
	    uart_puts_string readdata

		LDI R16,ENTER
	    PUSH R16
	    uart_putc
	 
		//主机伪写指令目的是确定要读取数据的地址
        LDI R16,DEVICE_ADDRESS<<1&0x0fe//伪写指令 目的是为了确定读取的地址（发送器件地址 字节地址0x00 以确定读取数据的地址）
		STS TWIC_MASTER_ADDR,R16
        ldi r16,0x00
		sts  GPIO_GPIOR0,r16
		ldi r16,0x01
		sts  GPIO_GPIOR1,r16//标志位
	 
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
	 
		
	 

// TWIF 主机中断服务程序
ISRT_TWIC_TWIM_vect:
         
	      LDS  R16,TWIC_MASTER_STATUS
          SBRS R16,TWI_MASTER_WIF_bp
	      JMP ISRT_TWIC_TWIM_vect_2//读中断
		  nop
		  sbrs r16,4
          jmp ISRT_TWIC_TWIM_vect_0
		  NOP
          jmp ISRT_TWIC_TWIM_vect_5


ISRT_TWIC_TWIM_vect_0://写中断
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
