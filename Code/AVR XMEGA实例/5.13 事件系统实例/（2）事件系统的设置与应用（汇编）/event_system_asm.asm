/*
 * 工程名:
     event_system_example
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
     	 
*/
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过
.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）

.ORG 0X100       ;跳过中断区0x00-0x0FF


/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/

RESET:

	call Example1 
	/*call Example2 
	call Example3 
	call Example4 */
RESET_1:
    JMP RESET_1

/*
+------------------------------------------------------------------------------
| Function    : Example1
+------------------------------------------------------------------------------
*/
Example1:

	/* PD.0 输入/双沿感知*/
	ldi r16,PORT_ISC_BOTHEDGES_gc
	sts PORTD_PIN0CTRL,r16
    ldi r16,0x01
	sts PORTD_DIRCLR,r16

	/*选择PD0为channel 0事件输入*/
	LDI R16,EVSYS_CHMUX_PORTD_PIN0_gc
	STS EVSYS_CH0MUX,R16

	//选择通道0作为TCC0的事件源 并且事件行为是输入捕获
	LDI R16,TC_EVSEL_CH0_gc |TC_EVACT_CAPT_gc
	STS TCC0_CTRLD,R16

	/* 使能TCC0比较捕获通道A */
    LDI R16,TC0_CCAEN_bm
	STS TCC0_CTRLB,R16


	/*设置TCC0计数周期 */
	LDI XL,0X0FF
    STS  TCC0_PER,XL
	STS  TCC0_PER+1,XL

	//TCC0时钟源
	LDI R16,TC_CLKSEL_DIV1_gc
    STS TCC0_CTRLA,R16
Example1_1:/* 当捕获发生时标志位置位，清除标志位*/
 
	LDS R16,TCC0_INTFLAGS 
	SBRS R16,TC0_CCAIF_bp
	JMP  Example1_1
         
    LDI R16,TC0_CCAIF_bm
	STS TCC0_INTFLAGS,R16
	JMP Example1_1
    RET

/*
+------------------------------------------------------------------------------
| Function    : Example2
+------------------------------------------------------------------------------
*/
Example2:
	/*选择TCCO益处作为事件通道0的事件*/
	LDI R16,EVSYS_CHMUX_TCC0_OVF_gc
	STS EVSYS_CH0MUX,R16

	/*ADC被选择的通道0 1 2 3，事件通道0，1 ，2,3作为所选事件通道；
	EVSEL定义的通道号最小的事件通道将触发SWEEP中定义的ADC通道的一次扫描*/
	LDI R16,ADC_SWEEP_0123_gc |ADC_EVSEL_0123_gc |ADC_EVACT_SWEEP_gc
	STS ADCA_EVCTRL,R16

	/* 通道0,1,2,3配置为单端正向输入信号 并且设置0,1,2,3的各个通道的正向输入引脚*/
	LDI R16,ADC_CH_MUXPOS_PIN4_gc |0
	STS ADCA_CH0_MUXCTRL,R16 
	LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc
	STS ADCA_CH0_CTRL,R16 
	LDI R16,ADC_CH_MUXPOS_PIN5_gc |0 
	STS ADCA_CH1_MUXCTRL,R16 
	LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc
	STS ADCA_CH1_CTRL,R16 
	LDI R16,ADC_CH_MUXPOS_PIN6_gc |0 
	STS ADCA_CH2_MUXCTRL,R16 
	LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc
	STS ADCA_CH2_CTRL,R16
	LDI R16,ADC_CH_MUXPOS_PIN7_gc |0
	STS ADCA_CH3_MUXCTRL,R16 
	LDI R16,ADC_CH_INPUTMODE_SINGLEENDED_gc
	STS ADCA_CH3_CTRL,R16 


	/*  设置ADC时钟预分频为DIV8；精度为12位；ADC自由运行模式
	 *   ADC参考电压为内部VCC / 1.6V；使能ADC
	 */
	LDI R16,ADC_PRESCALER_DIV8_gc
	STS ADCA_PRESCALER,R16 
	     
    LDI R16,ADC_RESOLUTION_12BIT_gc|ADC_FREERUN_bm            
	STS ADCA_CTRLB,R16
	              
    LDI R16,ADC_REFSEL_VCC_gc
	STS ADCA_REFCTRL,R16
	LDI R16,ADC_ENABLE_bm                
	STS ADCA_CTRLA,R16 

	/*设置TCC0计数周期 */
	LDI XL,0X0FF
    STS  TCC0_PER,XL
	STS  TCC0_PER+1,XL
	//TCC0时钟源
	LDI R16,TC_CLKSEL_DIV1_gc
    STS TCC0_CTRLA,R16

Example2_1:
		/*当TCC0溢出时ADC通道将不断的被扫描 */
 
    JMP Example2_1
	RET
/*
+------------------------------------------------------------------------------
| Function    : Example3
+------------------------------------------------------------------------------
*/
Example3:
 
	/* PD.0 输入/双沿感知*/
	ldi r16,PORT_ISC_BOTHEDGES_gc
	sts PORTD_PIN0CTRL,r16
    ldi r16,0x01
	sts PORTD_DIRCLR,r16

	//TCC0溢出作为通道0的事件源
    LDI R16,EVSYS_CHMUX_TCC0_OVF_gc
	STS EVSYS_CH0MUX,R16
 

	/*选择PD0为通道1事件输入 */
	LDI R16,EVSYS_CHMUX_PORTD_PIN0_gc
	STS EVSYS_CH1MUX,R16
 

	/* 选择通道0作为TCC1的时钟源 TCC0与TCC1级联成了32为计数器*/ 
	LDI R16,TC_CLKSEL_EVCH0_gc
	STS TCC1_CTRLA,R16

	/* 设置通道TCC0为捕获模式 */
	LDI R16,TC_EVSEL_CH1_gc | TC_EVACT_CAPT_gc 
	STS TCC0_CTRLD,R16

	/* 设置通道TCC1为捕获模式并且添加事件的延迟来弥补插入的传播时延 */
	LDI R16,TC_EVSEL_CH1_gc|TC_EVACT_CAPT_gc| TC0_EVDLY_bm
	STS TCC1_CTRLD,R16

	/* 使能TCC0,TCC1的A通道 */
	LDI R16,TC0_CCAEN_bm
	STS TCC0_CTRLB,R16
	LDI R16,TC1_CCAEN_bm
	STS TCC1_CTRLB,R16

	//TCC0时钟源
	LDI R16,TC_CLKSEL_DIV1_gc
	STS TCC0_CTRLA,R16

Example3_1:/* 当捕获发生时标志位置位，清除标志位*/
 
	LDS R16,TCC0_INTFLAGS 
	SBRS R16,TC0_CCAIF_bp
	JMP  Example3_1
         
    LDI R16,TC0_CCAIF_bm
	STS TCC0_INTFLAGS,R16
	LDI R16,TC1_CCAIF_bm
	STS TCC1_INTFLAGS,R16
	JMP Example3_1
    RET

/*
+------------------------------------------------------------------------------
| Function    : Example4
+------------------------------------------------------------------------------
*/
Example4:
	/* PD.0 输入/双沿感知*/
	ldi r16,PORT_ISC_BOTHEDGES_gc
	sts PORTD_PIN0CTRL,r16
    ldi r16,0x01
	sts PORTD_DIRCLR,r16

	/* PC引脚全部输出*/
    LDI R16,0x0ff
	STS PORTC_DIRSET,R16

	/*选择PD0为通道0事件输入*/
	LDI R16,EVSYS_CHMUX_PORTD_PIN0_gc
	STS EVSYS_CH0MUX,R16

	/* 在TCC0通道A上加数字滤波器*/
	LDI R16,EVSYS_DIGFILT_8SAMPLES_gc
	STS EVSYS_CH0CTRL,R16 
 
    /*设置TCC0计数周期 */
	LDI r16,0x0ff
    STS  TCC0_PER,r16
	STS  TCC0_PER+1,r16
	//TCC0时钟源
	LDI R16,TC_CLKSEL_EVCH0_gc
    STS TCC0_CTRLA,R16
 

Example4_1:
	/* 输出TCCO计数器中数值*/
	LDS R16,TCC0_CNT
 	STS PORTC_OUT,R16
    JMP Example4_1
	RET
 
