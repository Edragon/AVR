/*
 * 工程名:
     RTC
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
     	本函数实现了秒计数器，RTC每秒产生中断,益处中断对PD.4取反，比较中断对PD.5取反
*/
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过
;.include "usart_driver.inc"

.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
.ORG 0x014         
        RJMP ISR_RTC_OVF_vect
.ORG 0x016         
        RJMP ISR_RTC_COMP_vect
.ORG 0X100       ;跳过中断区0x00-0x0FF

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
.MACRO LED
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO
/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
RESET:
	//打开内部 32.768 kHz RC振荡器产生1Khz 
     ldi r16,OSC_RC32KEN_bm
     sts OSC_CTRL,r16
   //等待时钟稳定
RESET_1:
     lds r16,OSC_STATUS
     sbrs r16,OSC_RC32KRDY_bp
     jmp RESET_1

	//设置内部32kHz为RTC时钟源
	ldi r16,CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm
	sts CLK_RTCCTRL,r16

	//设置LED端口为输出
	 ldi r16,0xff
	 sts PORTD_DIR,r16

     //检查RTC忙否
RESET_2:
	 lds r16,RTC_STATUS
	 sbrc r16,RTC_SYNCBUSY_bp
	 jmp RESET_2
     nop
   

	//配置RTC周期为1S
    ldi r16,0xff
	sts RTC_PER,r16
	ldi r16,0x03
	sts RTC_PER+1,r16
	ldi r16,0  
	sts RTC_CNT,r16
	ldi r16,0
	sts RTC_COMP,r16
	ldi r16,RTC_PRESCALER_DIV1_gc
	sts RTC_CTRL,r16
 

	//使能溢出中断	//使能比较中断
	ldi r16,RTC_OVFINTLVL_LO_gc|RTC_COMPINTLVL_LO_gc
	sts RTC_INTCTRL ,r16
    ldi r16,PMIC_LOLVLEN_bm
	sts PMIC_CTRL,r16  
	sei
RESET_3:
    jmp RESET_3 


/*! \溢出中断服务程序 在端口D的LED上显示秒数
 */
ISR_RTC_OVF_vect:
     LED 0x10
	 reti
    
/*! \比较中断服务程序 在端口D的LED上显示秒数
 */
ISR_RTC_COMP_vect:
      LED 0x20
	  reti
  
