/*
 * 工程名:
     I2C
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-19 15:47
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序使用TWIC自发自收，同时使能主机和从机，
		主程序中对发送缓存sendBuffer中的数据显示，
		发送，再对接收数据显示。
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
.ORG  0X018
        RJMP ISR_TWIC_TWIS_vect
.ORG 0X100       ;跳过中断区0x00-0x0FF
.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'
//从机地址
.equ SLAVE_ADDRESS=0x55
// CPU 2MHz
// 波特率400kHz
.equ TWI_BAUDSETTING=0
//测试数据
sendBuffer: .DB 0x01,0x02,0x03,0x04,0xbb,0xaa,0xdd,0xee,0x00
MAIN:       .DB 'I','N','T','O','M','A','I','N',0
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

		LDI R16,ENTER
	    PUSH R16
	    uart_putc


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


	//初始化从机
       
		LDI R16,TWI_SLAVE_INTLVL_LO_gc |TWI_SLAVE_ENABLE_bm|TWI_SLAVE_DIEN_bm |TWI_SLAVE_APIEN_bm;|TWI_SLAVE_PIEN_bm
                                
	    STS TWIC_SLAVE_CTRLA,R16

       
	    LDI R16,SLAVE_ADDRESS<<1

	 
	    STS TWIC_SLAVE_ADDR,R16

	                               
  
	//使能低级别中断
	    LDI R16,PMIC_LOLVLEN_bm
	    STS PMIC_CTRL,R16
	    SEI
        //主机发送数据
		// 根据bytesToWrite判断是写, 发送START信号 + 7位地址 +  R/_W = 0
        
	

        LDI R16,SLAVE_ADDRESS<<1&0x0fe
		STS TWIC_MASTER_ADDR,R16
        ldi xh,0x21
		ldi xl,0x00 
		ldi r16,0x00
        st x+,r16
		st x,r16//计数
	 
RESET_2:		
		JMP RESET_2
/*! TWIC Master Interrupt vector. */


ISRT_TWIC_TWIM_vect:
         
          LDS  R16,TWIC_MASTER_STATUS
          SBRS R16,TWI_MASTER_WIF_bp
	      JMP ISRT_TWIC_TWIM_vect_2//读中断
          nop
		  sbrs r16,4
          jmp ISRT_TWIC_TWIM_vect_0
		  nop
		  // 根据bytesToWrite判断是读,以后交给读中断 发送START信号 + 7位地址 +  R/_W = 1
	      LDI R16,SLAVE_ADDRESS<<1|0X01 
	      STS TWIC_MASTER_ADDR,R16 
		  jmp ISRT_TWIC_TWIM_vect_1
		  nop

ISRT_TWIC_TWIM_vect_0://写中断
         LDI ZH,HIGH(sendBuffer<<1)
	     LDI ZL,LOW(sendBuffer<<1)
		 ldi xh,0x21
		 ldi xl,0x01 
		 ld r19,x
		 add zl,r19
         lpm r16,z+
		 inc r19
		 st x,r19
		 clz 
		 cpi r16,0x00
		 breq ISRT_TWIC_TWIM_vect_1
	     STS TWIC_MASTER_DATA,R16
		 jmp ISRT_TWIC_TWIM_vect_3
ISRT_TWIC_TWIM_vect_1:
         STS TWIC_MASTER_DATA,R16
         lds r16,TWIC_MASTER_STATUS
         ori R16,TWI_MASTER_ARBLOST_bm|TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm
         sts TWIC_MASTER_STATUS,R16
		 JMP ISRT_TWIC_TWIM_vect_3


ISRT_TWIC_TWIM_vect_2:
          
          LDS xh,TWIC_MASTER_DATA
          uart_putc_hex
		  ldi xh,0x21
		  ldi xl,0x01 
		  ld r16,x
		  clz
		  cpi r16,0x00
		  breq  ISRT_TWIC_TWIM_vect_4
          LDI R16,TWI_MASTER_CMD_RECVTRANS_gc
	      STS TWIC_MASTER_CTRLC,R16

		  lds r16,TWIC_MASTER_STATUS
          ori R16,TWI_MASTER_ARBLOST_bm|TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm
          sts TWIC_MASTER_STATUS,R16

		  jmp ISRT_TWIC_TWIM_vect_3
ISRT_TWIC_TWIM_vect_4:
          LDI R16,TWI_MASTER_ACKACT_bm|TWI_MASTER_CMD_STOP_gc
		                               
	      STS TWIC_MASTER_CTRLC,R16 

         
ISRT_TWIC_TWIM_vect_3:
		   reti


/*! TWIC Slave Interrupt vector. */
ISR_TWIC_TWIS_vect:

        LDS  R16,TWIC_SLAVE_STATUS
		sbrs r16,6
		jmp end_1
		nop
		LDI  R16,TWI_SLAVE_CMD_RESPONSE_gc
        STS  TWIC_SLAVE_CTRLB,R16
		jmp  end

end_1:  

        LDS  R16,TWIC_SLAVE_STATUS
        sbrs r16,7
		jmp  end
		nop
		sbrs r16,1
		jmp  end_2
		nop 
		jmp  end_5
		nop
end_2:  
		LDS R16,TWIC_SLAVE_DATA
       	clz
		cpi r16,0x00
		brne end_3
		nop
		jmp end_4
end_3:  
        ldi yh,0x22
		eor yl,yl
		ldi xh,0x21
		ldi xl,0x01 
		ld r17,x
		add yl,r17
		st y,r16
		mov xh,r16
		uart_putc_hex
		LDI  R16,TWI_SLAVE_CMD_RESPONSE_gc
        STS  TWIC_SLAVE_CTRLB,R16
		jmp end
end_4:   
         
        ldi yh,0x22
		eor yl,yl
		ldi xh,0x21
		ldi xl,0x01 
		ld r17,x
		add yl,r17
		st y,r16
		mov xh,r16
		uart_putc_hex
        LDI  R16,TWI_SLAVE_ACKACT_bm|TWI_SLAVE_CMD_COMPTRANS_gc
		                              
        STS  TWIC_SLAVE_CTRLB,R16
		jmp end

end_5:  
        sbrc r16,4
		jmp end_6
		nop
        ldi yh,0x22
		eor yl,yl
		ldi xh,0x21
		ldi xl,0x01 
		ld r17,x
        add yl,r17
		dec r17
	    st x,r17
        ld r16,y
		STS TWIC_SLAVE_DATA,R16
		jmp end
end_6:
        LDI  R16,TWI_SLAVE_ACKACT_bm|TWI_SLAVE_CMD_COMPTRANS_gc
		                              
        STS  TWIC_SLAVE_CTRLB,R16
end:
	 	LDI R16,TWI_SLAVE_APIF_bm|TWI_SLAVE_DIF_bm
	    STS TWIC_SLAVE_STATUS,r16
        RETI


