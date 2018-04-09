/*
 * 工程名:
     AC
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-11 1:17
 * 作者:
 		 
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
		 /*定时器C0的通道A与B单斜率模式产生波形作为AC引脚pin0与pin1负引脚是内部的分压;
          AC一个是上升沿中断一个是下降沿中断;在中断中控制PD.4和PD.5取反，会看到灯的不停
		  闪烁

*/

.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过

.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
.ORG 0x88
        RJMP ISR_ACA_AC0_vect
.ORG 0x8A
        RJMP ISR_ACA_AC1_vect
.ORG 0X100       ;跳过中断区0x00-0x0FF

/*
+------------------------------------------------------------------------------
| Function    :  
+------------------------------------------------------------------------------
*/ 
.MACRO LED1_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO

/*
+------------------------------------------------------------------------------
| Function    : RESET
+------------------------------------------------------------------------------
| Description : 
+------------------------------------------------------------------------------
*/
RESET:
     
	 ldi r16,0x10
     sts PORTCFG_VPCTRLA,r16//;PORTB映射到虚拟端口1，PORTA映射到虚拟端口0
	  
	 ldi r16,0x32  
	 sts PORTCFG_VPCTRLB,r16//;PORTC映射到虚拟端口2，PORTD映射到虚拟端口3
 
     ldi r16,0x00
	 sts VPORT0_DIR,r16 //PORTA引脚输入
	 ldi r16,0xff
     sts VPORT2_DIR,r16 //PORTC引脚输出
	 ldi r16,0xff
     sts VPORT3_DIR,r16 //PORTD引脚输出

	 /* 设置计数周期 */
	 ldi XH,0X0F
	 ldi XL,0X0a0
	 sts TCC0_PER,XL
	 sts TCC0_PER+1,XH
      
     ldi XH,0X03
	 ldi XL,0X0e8
     sts TCC0_CCABUF,XL
	 sts TCC0_CCABUF+1,XH


	 ldi XH,0X07
	 ldi XL,0X0D0
     sts TCC0_CCBBUF,XL
	 sts TCC0_CCBBUF+1,XH

	/* 设置TC为单斜率模式 使能通道A B */
	 LDS R16,TCC0_CTRLB
	 ORI R16,TC_WGMODE_SS_gc|TC0_CCAEN_bm | TC0_CCBEN_bm
	 STS TCC0_CTRLB,R16


	 /* 选择时钟，启动定时器 */
	 LDI R16,TC_CLKSEL_DIV1024_gc
	 STS TCC0_CTRLA,R16

	 
    /*模拟比较器输入电压比例因子*/
	 LDI R16,0
     STS ACA_CTRLB,R16
	 /* 设置模拟比较器0的引脚是 pin 0 and 1. */

     LDI R16,AC_MUXPOS_PIN0_gc|AC_MUXNEG_SCALER_gc
	 STS ACA_AC0MUXCTRL,R16
	 LDI R16,AC_MUXPOS_PIN2_gc|AC_MUXNEG_SCALER_gc
	 STS ACA_AC1MUXCTRL,R16
 
	/*设置AC0 AC1的磁滞;低级中断级别;使能模拟比较器AC0 AC1 */ 
	 
	 LDI R16,AC_HYSMODE_SMALL_gc|AC_INTMODE_RISING_gc|AC_INTLVL_LO_gc |AC_ENABLE_bm
	 STS ACA_AC0CTRL,R16
	 
	 LDI R16,AC_HYSMODE_SMALL_gc|AC_INTMODE_FALLING_gc|AC_INTLVL_LO_gc |AC_ENABLE_bm
	 STS ACA_AC1CTRL,R16

     LDI R16,PMIC_HILVLEN_bm + PMIC_MEDLVLEN_bm + PMIC_LOLVLEN_bm; 
	 STS PMIC_CTRL,R16//可编程多层中断控制寄存器高 中 低层使能，循环调度关闭，中断向量未移至 Boot section	
  	    
     SEI//全局中断使能置位

LOOP:	
		NOP
		 
        JMP LOOP


/*
+------------------------------------------------------------------------------
| Function    : 中断函数
+------------------------------------------------------------------------------
*/ 
ISR_ACA_AC0_vect:
 
  LED1_T 0X10

  RETI
 
ISR_ACA_AC1_vect:
 
  LED1_T 0X20

  RETI
 
