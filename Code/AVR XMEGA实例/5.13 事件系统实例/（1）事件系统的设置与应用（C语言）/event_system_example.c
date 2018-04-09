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
#include "avr_compiler.h"
#include "event_system_driver.c"
#include "TC_driver.c"

void Example1( void );
void Example2( void );
void Example3( void );
void Example4( void );
#define LED1_T()   PORTD_OUTTGL = 0x20
#define LED1_ON()  PORTD_OUTSET = 0x20
#define LED1_OFF() PORTD_OUTSET = 0x20

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main( void )
{

	Example1();
	/*Example2();*/
	/*Example3();*/
	/*Example4();*/

	do {

	}while (1);
}

/*
+------------------------------------------------------------------------------
| Function    : Example1
+------------------------------------------------------------------------------
*/
void Example1( void )
{
	/* PD.0 输入/双沿感知*/
	PORTD.PIN0CTRL |= PORT_ISC_BOTHEDGES_gc;
	PORTD.DIRCLR = 0x01;
	PORTD_DIRSET = 0x20;

	/*选择PD0为channel 0事件输入*/
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_PORTD_PIN0_gc );

	//选择通道0作为TCC0的事件源 并且事件行为是输入捕获
	TCC0.CTRLD = (uint8_t) TC_EVSEL_CH0_gc | TC_EVACT_CAPT_gc;

	/* 使能TCC0比较捕获通道A */
	TCC0.CTRLB |= TC0_CCAEN_bm;

	/*设置TCC0计数周期 */
	TCC0.PER = 0xFFFF;
	//TCC0时钟源
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;

	while (1) {
		if ( TCC0.INTFLAGS & TC0_CCAIF_bm ) {
    /* 当捕获发生时标志位置位，清除标志位*/
			TCC0.INTFLAGS |= TC0_CCAIF_bm;
			LED1_T();
		}
	}
}


/*
+------------------------------------------------------------------------------
| Function    : Example2
+------------------------------------------------------------------------------
*/
void Example2( void )
{
    PORTD_DIRSET = 0x20;
	/*选择TCCO益处作为事件通道0的事件*/
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_TCC0_OVF_gc );

	/*ADC被选择的通道0 1 2 3，事件通道0，1 ，2,3作为所选事件通道；
	EVSEL定义的通道号最小的事件通道将触发SWEEP中定义的ADC通道的一次扫描*/
	ADCA.EVCTRL = (uint8_t) ADC_SWEEP_0123_gc |
	              ADC_EVSEL_0123_gc |
	              ADC_EVACT_SWEEP_gc;

	/* 通道0,1,2,3配置为单端正向输入信号 并且设置0,1,2,3的各个通道的正向输入引脚*/
	ADCA.CH0.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN4_gc |0;
	ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH1.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN5_gc |0;
	ADCA.CH1.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH2.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN6_gc |0;
	ADCA.CH2.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH3.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN7_gc |0;
	ADCA.CH3.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;


	/*  设置ADC时钟预分频为DIV8；精度为12位；ADC自由运行模式
	 *   ADC参考电压为内部VCC / 1.6V；使能ADC
	 */
	ADCA.PRESCALER = ( ADCA.PRESCALER & ~ADC_PRESCALER_gm ) |
	                 ADC_PRESCALER_DIV8_gc;
	ADCA.CTRLB = ( ADCA.CTRLB & ~ADC_RESOLUTION_gm ) |
	             ADC_RESOLUTION_12BIT_gc;
	ADCA.CTRLB = ( ADCA.CTRLB & ~( ADC_CONMODE_bm | ADC_FREERUN_bm ) );
	ADCA.REFCTRL = ( ADCA.REFCTRL & ~ADC_REFSEL_gm ) |
	               ADC_REFSEL_VCC_gc;
	ADCA.CTRLA |= ADC_ENABLE_bm;

	/*设置TCC0计数周期 */
	TCC0.PER = 0x0FFF;

	/* 设置溢出中断为低级别中断 */
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_LO_gc );
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	sei();	
	//TCC0时钟源
	TCC0.CTRLA |= TC_CLKSEL_DIV256_gc;

	while (1) {
	/*当TCC0溢出时ADC通道将不断的被扫描 
	PD5上面的灯会不断的闪烁*/
	           }
}

ISR(TCC0_OVF_vect)
{
 LED1_T();
}
/*
+------------------------------------------------------------------------------
| Function    : Example3
+------------------------------------------------------------------------------
*/
void Example3( void )
{
	/* PD.0 输入/双沿感知*/
	PORTD.PIN0CTRL = PORT_ISC_BOTHEDGES_gc;
	PORTD.DIRCLR = 0x01;

	//TCC0溢出作为通道0的事件源
 
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_TCC0_OVF_gc );

	/*选择PD0为通道1事件输入 */
	EVSYS_SetEventSource( 1, EVSYS_CHMUX_PORTD_PIN0_gc );

	/* 选择通道0作为TCC1的时钟源 TCC0与TCC1级联成了32为计数器*/
	TCC1.CTRLA = TC_CLKSEL_EVCH0_gc;

	/* 设置通道TCC0为捕获模式 */
	TCC0.CTRLD = (uint8_t) TC_EVSEL_CH1_gc | TC_EVACT_CAPT_gc;

	/* 设置通道TCC1为捕获模式并且添加事件的延迟来弥补插入的传播时延 */
	TCC1.CTRLD = (uint8_t) TC_EVSEL_CH1_gc | TC0_EVDLY_bm | TC_EVACT_CAPT_gc;

	/* 使能TCC0,TCC1的A通道 */
	TCC0.CTRLB = TC0_CCAEN_bm;
	TCC1.CTRLB = TC1_CCAEN_bm;

	//TCC0时钟源
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;

	while (1) {
		if ( TCC1.INTFLAGS & TC0_CCAIF_bm ) {
			/* 当捕获发生时标志位置位，清除标志位*/
			 
			TCC0.INTFLAGS |= TC0_CCAIF_bm;
			TCC1.INTFLAGS |= TC1_CCAIF_bm;
		}
	}
}

/*
+------------------------------------------------------------------------------
| Function    : Example4
+------------------------------------------------------------------------------
*/
void Example4( void )
{
	/* PD.0 输入/双沿感知*/
	PORTD.PIN0CTRL = PORT_ISC_RISING_gc;
	PORTD.DIRCLR = 0x01;

	/* PC引脚全部输出*/
	PORTC.DIRSET = 0xFF;

	/*选择PD0为通道0事件输入*/
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_PORTD_PIN0_gc );

	/*  在TCC0通道A上加数字滤波器*/
	  
	EVSYS_SetEventChannelFilter( 0, EVSYS_DIGFILT_8SAMPLES_gc );

	/*设置TCC0计数周期 */ 
	TCC0.PER = 0xFFFF;
    //TCC0时钟源为事件通道0
	TCC0.CTRLA = TC_CLKSEL_EVCH0_gc;


	while (1) {
		/* 输出TCCO计数器中数值的相反*/
		PORTC.OUT = ~TCC0.CNT;
	}
}
