/*
 * 工程名:
     TC
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-11 1:17
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	Example1 TCC0基本计数
		Example2 TCC0通道A输入捕获
		Example3 TCC0频率测量PC0口信号频率
		Example4 TCC0通道B占空比变化的脉宽调制输出
		Example5 TCC0对事件信号计数
		Example6 32位计数
*/
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过
.include "usart_driver.inc"
.ORG 0
  		RJMP RESET//复位 
;.ORG 0x20
  	;	RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
;.ORG 0x036        //USARTC0数据接收完毕中断入口
        ;RJMP ISR
.ORG 0X01C
        RJMP ISR_OVFIF
.ORG 0X20
        RJMP ISR_CCA_vect
 
.ORG 0X100       ;跳过中断区0x00-0x0FF

.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'


/*
+------------------------------------------------------------------------------
| Function    : LED
+------------------------------------------------------------------------------
| Description : 开关灯
| Parameters  : 
| Returns     :
+------------------------------------------------------------------------------
*/
         
.MACRO LED1_ON
           LDI R16,@0
           STS PORTD_OUTSET,R16
.ENDMACRO
.MACRO LED2_ON
           LDI R16,@0
           STS PORTD_OUTSET,R16
.ENDMACRO    
.MACRO LED1_OFF
           LDI R16,@0
           STS PORTD_OUTCLR,R16
.ENDMACRO
.MACRO LED2_OFF
           LDI R16,@0
           STS PORTD_OUTCLR,R16
.ENDMACRO
.MACRO LED1_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO
.MACRO LED2_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO
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
| Function    : Example1
+------------------------------------------------------------------------------
| Description : TCC0基本计数
+------------------------------------------------------------------------------
*/
Example1:  
	/* Set period/TOP value. */
	      LDI XH,0X10
	      EOR XL,XL
          STS TCC0_PER,XH
	      STS TCC0_PER+1,XL

          LDI R16,TC_CLKSEL_DIV1_gc
          STS TCC0_CTRLA,R16
Example1_0:
          JMP Example1_0
	      RET



/*
+------------------------------------------------------------------------------
| Function    : Example2
+------------------------------------------------------------------------------
| Description : TCC0通道A输入捕获 按下五维导航键的DOWN健，产生下降沿，CCAIF置位
|				后，在Port D上输出捕获值，可接8个LED灯显示			
+------------------------------------------------------------------------------
*/
Example2:

	/* PE3设为输入，下降沿触发 输入上拉 当I/O引脚作为事件的捕获源，该引脚必须配置为边沿检测。*/
	LDI R16,PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc
    STS PORTE_PIN3CTRL,R16
	 
	LDI R16,0X08
	STS PORTE_DIRCLR,R16

	/* Port D设为输出 */
	LDI R16,0X0FF
	STS PORTD_DIRSET,R16

	/* PE3作为事件通道2的输入. */
	LDI R16,EVSYS_CHMUX_PORTE_PIN3_gc
	STS EVSYS_CH2MUX,R16

	/* 设置 TCC0 输入捕获使用事件通道2 */
	LDI R16,TC_EVSEL_CH2_gc |TC_EVACT_CAPT_gc
	STS TCC0_CTRLD,R16
	 
	/* 使能通道A */
    LDI R16,TC0_CCAEN_bm &( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm )
	STS TCC0_CTRLB,R16
	 

	/* Set period/TOP value. */
	EOR XH,XH
	LDI XL,0X0FF
    STS  TCC0_PER,XL
	STS  TCC0_PER+1,XH

	/* 选择时钟，启动定时器 */
	LDI R16,TC_CLKSEL_DIV1_gc
    STS TCC0_CTRLA,R16

Example2_1:    
    LDS R16,TCC0_INTFLAGS
	SBRS R16,TC0_CCAIF_bp
	JMP Example2_1
	NOP
	  
	/*定时器把事件发生时计数寄存器的当前计数值拷贝到CCA寄存器*/
	LDS R16,TCC0_CCA
	STS PORTD_OUT,R16
    JMP  Example2_1
	RET



/*
+------------------------------------------------------------------------------
| Function    : Example3
+------------------------------------------------------------------------------
| Description : TCC0捕获高低电平 pc0作为输入（可用此函数计算频率和占空比）		
+------------------------------------------------------------------------------
*/
Example3:
 
	/* Port D设为输出 LED指示*/
	LDI R16,0X0FF
	STS PORTD_DIRSET,R16
	LED1_OFF 0X20
	LED2_ON  0X10

	/* PC0设为输入，双沿触发 */
	LDI R16,PORT_ISC_BOTHEDGES_gc
	STS PORTC_PIN0CTRL,R16
	LDI R16,0X01
	STS PORTC_DIRCLR,R16

	/* PC0作为事件通道0的输入. */
	LDI R16,EVSYS_CHMUX_PORTC_PIN0_gc
	STS EVSYS_CH0MUX,R16

	/* 设置 TCC0 输入捕获使用事件通道0 */
	LDI R16,TC_EVSEL_CH0_gc |TC_EVACT_CAPT_gc
	STS TCC0_CTRLD,R16
	 

	/* 使能通道A */
	LDI R16,TC0_CCAEN_bm &( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm )
	STS TCC0_CTRLB,R16
	 

	/* 如果设置的周期寄存器的值比0x8000小，捕获完成后将
	把I/O引脚的电平变化存储在捕获寄存器的最高位（MSB）。
	Clear MSB of PER[H:L] to allow for propagation of edge polarity. */
	LDI XH,0X7F
	LDI XL,0X0FF
	STS  TCC0_PER,XL
	STS  TCC0_PER+1,XH

	/* 选择时钟，启动定时器 */
	LDI R16,TC_CLKSEL_DIV1_gc
	STS TCC0_CTRLA,R16

	/* 使能通道A 低级别中断 */
	LDI R16,TC_CCAINTLVL_LO_gc
    STS TCC0_INTCTRLB,R16
	 
    LDI R16,PMIC_LOLVLEN_bm
	STS PMIC_CTRL,R16

	SEI
Example3_0:    
	JMP Example3_0
	RET

 


ISR_CCA_vect:
 
	LED1_T 0X20
	LED2_T 0X10
    /*定时器把事件发生时计数寄存器的当前计数值拷贝到CCA寄存器*/
	LDS R17,TCC0_CCA
	LDS R16,TCC0_CCA+1
	/*  按上升沿来保存总周期值 */
    
	SBRS R16,7//MSB=1,代表高电平，上升沿
	JMP Example3_1
	NOP
	ANDI R16,0X80
	;ST TCC0_CTRLFSET,TC_CMD_RESTART_gc
	MOVW XH:XL,R16:R17
    uart_putw_dec

	JMP Example3_END
Example3_1:	
    
    MOVW XH:XL,R16:R17
    uart_putw_dec
 

Example3_END:
    
	RETI   

/*
+------------------------------------------------------------------------------
| Function    : Example4
+------------------------------------------------------------------------------
| Description : TCC0通道B占空比变化的脉宽调制，PC1输出	可观察LED亮灭占空比的变化		
+------------------------------------------------------------------------------
*/

Example4:
	/* PC1输出 */
    LDI R16,0X02
	STS PORTC_DIRSET,R16

	/* 设置计数周期 */
	LDI XH,0X02
	EOR XL,XL
    STS TCC0_PER,XL
	STS TCC0_PER+1,XH
	/* 设置TC为单斜率模式 */
	LDS R16,TCC0_CTRLB
	ORI R16,TC_WGMODE_SS_gc
	STS TCC0_CTRLB,R16

	 /* 使能通道B */
	LDI R16,TC0_CCBEN_bm &( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm )
	STS TCC0_CTRLB,R16
	 
	/* 选择时钟，启动定时器 */
	LDI R16,TC_CLKSEL_DIV64_gc
	STS TCC0_CTRLA,R16
 
	 
		/* 新比较值*/
    EOR XH,XH
	EOR XL,XL
	EOR YH,YH
	EOR YL,YL
	LDI YH,0X02
Example4_0:
    ADIW XH:XL,31
    CLZ
    CP XL,YL
    CPC XH,YH
    BRLO Example4_1
	EOR XH,XH
	EOR XL,XL
	ADIW XH:XL,31

Example4_1:		
   /*设置到缓冲寄存器*/  
    STS TCC0_CCBBUF,XL
	STS TCC0_CCBBUF+1,XH

  /*溢出时比较值从CCBBUF[H:L] 传递到CCB[H:L]*/
 Example4_2:
    LDS R16,TCC0_INTFLAGS
    SBRS R16,TC0_OVFIF_bp
	JMP Example4_2	
    NOP

  /* 清除溢出标志 */
    LDI R16,TC0_OVFIF_bm
	STS TCC0_INTFLAGS,R16
    JMP Example4_0
	RET


/*
+------------------------------------------------------------------------------
| Function    : Example5
+------------------------------------------------------------------------------
| Description : TCC0对事件信号（PE3下降沿）计数，溢出中断对PD4取反	
+------------------------------------------------------------------------------
*/
Example5:

	/* PE3设为输入， 输入上拉，下降沿感知，DOWN键按下计数*/
	LDI R16,PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc
	STS PORTE_PIN3CTRL,R16
	LDI R16,0X08
	STS PORTE_DIRCLR,R16

	/* PD4设为输出*/
	LDI R16,0X30
	STS PORTD_DIRSET,R16

	/* 选择PE3为事件通道0的输入, 使能数字滤波*/
	LDI R16,EVSYS_CHMUX_PORTE_PIN3_gc
	STS EVSYS_CH0MUX,R16
	LDI R16,EVSYS_DIGFILT_8SAMPLES_gc
	STS EVSYS_CH0CTRL,R16

	/* 设置计数周期值-TOP */
	LDI XL,0X04
	EOR XH,XH
    STS TCC0_PER,XL
	STS TCC0_PER+1,XH

	         /* 设置溢出中断为低级别中断 */
			 LDI R16,USART_RXCINTLVL_LO_gc
             STS USARTC0_CTRLA,R16
	         /* Enable interrupts.*/
			 LDI R16,PMIC_LOLVLEN_bm
	         STS PMIC_CTRL,R16
             SEI   
  
	        /* 启动定时器 */
			LDI R16,TC_CLKSEL_EVCH0_gc
            STS TCC0_CTRLA,R16
Example5_0:	        
            JMP Example5_0
			RET


ISR_OVFIF:
 
	       LED1_T 0X20
           LED2_T 0X10
		   RETI
	 
    

/*
+------------------------------------------------------------------------------
| Function    : Example6
+------------------------------------------------------------------------------
| Description : 32位计数,溢出对	PD4取反，LED间隔闪烁
+------------------------------------------------------------------------------
*/
Example6:

	/* PD4设为输出 */
	LDI R16,0x10
	STS PORTD_DIRSET,R16
	/* TCC0溢出作为事件通道0的输入 */
	LDI R16,EVSYS_CHMUX_TCC0_OVF_gc
	STS EVSYS_CH0MUX,R16

	/* 使能TCC1传播时延 */
	LDS R16,TCC0_CTRLD
	ORI R16,TC0_EVDLY_bm
	STS TCC0_CTRLD,R16
	/* 设置计数周期 */
	LDI XL,0X0E2
	LDI XH,0X04
    STS TCC0_PER,XL
	STS TCC0_PER+1,XH
	LDI XL,0X0C8
	EOR XH,XH
    STS TCC1_PER,XL
	STS TCC1_PER+1,XH
	/* 使用通道0作为TCC1时钟源 */
	LDI R16,TC_CLKSEL_EVCH0_gc
	STS TCC1_CTRLA,R16
	/* 使用外设时钟8分频作为TCC0时钟源 启动定时器*/
	LDI R16,TC_CLKSEL_DIV8_gc
	STS TCC0_CTRLA,R16

Example6_1:
        LDS R16,TCC1_INTFLAGS
        SBRS R16,TC1_OVFIF_bp
	    JMP Example6_1	
		NOP
		/* 取反PD4 */
		LED1_T 0X10
	    /* 清除溢出标志 */
		LDI R16,TC1_OVFIF_bm
		STS TCC1_INTFLAGS,R16
		RET

        
/*
+------------------------------------------------------------------------------
| Function    : RESET
+------------------------------------------------------------------------------
*/
RESET: 
	 CALL uart_init 
	 //CALL Example1
	 //CALL Example2 
	 //CALL Example3
	 CALL Example6
	 //CALL Example5 
	 //CALL Example6 


