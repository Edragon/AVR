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
/* 定义 CPU 时钟和分频值 */
#define F_CPU           2000000UL
#define CPU_PRESCALER   1

#include "avr_compiler.h"
#include "usart_driver.c"
#include "TC_driver.c"

void Example1( void );
void Example2( void );
void Example3( void );
void Example4( void );
void Example5( void );
void Example6( void );


#define LED1_ON()  PORTD_OUTCLR = 0x20
#define LED1_OFF() PORTD_OUTSET = 0x20
#define LED1_T()   PORTD_OUTTGL = 0x20

#define LED2_ON()  PORTD_OUTCLR = 0x10
#define LED2_OFF() PORTD_OUTSET = 0x10
#define LED2_T()   PORTD_OUTTGL = 0x10

/*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
void uart_init(void)
{
	/* USARTC0 引脚方向设置*/
  	/* PC3 (TXD0) 输出 */
	PORTC.DIRSET   = PIN3_bm;
	/* PC2 (RXD0) 输入 */
	PORTC.DIRCLR   = PIN2_bm;
	/* USARTC0 模式 - 异步*/
	USART_SetMode(&USARTC0,USART_CMODE_ASYNCHRONOUS_gc);
	/* USARTC0帧结构, 8 位数据位, 无校验, 1停止位 */
	USART_Format_Set(&USARTC0, USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc, false);
	/* 设置波特率 9600*/
	USART_Baudrate_Set(&USARTC0, 12 , 0);
	/* USARTC0 使能发送*/
	USART_Tx_Enable(&USARTC0);
	/* USARTC0 使能接收*/
	USART_Rx_Enable(&USARTC0);
}
/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main( void )
{
	uart_init(); 
	//Example1();
	//Example2();
	//Example3();
	Example4();
	//Example5();
	//Example6();
}

/*
+------------------------------------------------------------------------------
| Function    : Example1
+------------------------------------------------------------------------------
| Description : TCC0基本计数
+------------------------------------------------------------------------------
*/
void Example1( void )
{
	/* Set period/TOP value. */
	TC_SetPeriod( &TCC0, 0x1000 );

	/* Select clock source. */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1_gc );

	do {
		/* Wait while the timer counts. */
	} while (1);
}


/*
+------------------------------------------------------------------------------
| Function    : Example2
+------------------------------------------------------------------------------
| Description : TCC0通道A输入捕获 按下五维导航键的DOWN健，产生下降沿，CCAIF置位
|				后，在Port D上输出捕获值，可接8个LED灯显示			
+------------------------------------------------------------------------------
*/
void Example2( void )
{
	uint16_t inputCaptureTime;

	/* PE3设为输入，下降沿触发 输入上拉 当I/O引脚作为事件的捕获源，该引脚必须配置为边沿检测。*/
	PORTE.PIN3CTRL = PORT_ISC_FALLING_gc + PORT_OPC_PULLUP_gc;
	PORTE.DIRCLR = 0x08;

	/* Port D设为输出 */
	PORTD.DIRSET = 0xFF;

	/* Pe0作为事件通道2的输入. */
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTE_PIN3_gc;

	/* 设置 TCC0 输入捕获使用事件通道2 */
	TC0_ConfigInputCapture( &TCC0, TC_EVSEL_CH2_gc );

	/* 使能通道A */
	TC0_EnableCCChannels( &TCC0, TC0_CCAEN_bm );

	/* Set period/TOP value. */
	TC_SetPeriod( &TCC0, 255 );

	/* 选择时钟，启动定时器 */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1_gc );

	 do {
		do {} while ( TC_GetCCAFlag( &TCC0 ) == 0 );
		/*定时器把事件发生时计数寄存器的当前计数值拷贝到CCA寄存器*/
		inputCaptureTime = TC_GetCaptureA( &TCC0 );
		PORTD.OUT = (uint8_t) (inputCaptureTime);
	} while (1);
}


/*
+------------------------------------------------------------------------------
| Function    : Example3
+------------------------------------------------------------------------------
| Description : TCC0频率测量和PC0输出			
+------------------------------------------------------------------------------
*/
void Example3( void )
{
	/* Port D设为输出 LED指示*/
	PORTD.DIRSET = 0xFF;
	LED1_OFF();
	LED2_ON();

	/* PC0设为输入，双沿触发 */
	PORTC.PIN0CTRL = PORT_ISC_BOTHEDGES_gc;
	PORTC.DIRCLR = 0x01;

	/* PC0作为事件通道0的输入. */
	EVSYS.CH0MUX = EVSYS_CHMUX_PORTC_PIN0_gc;

	/* 设置 TCC0 输入捕获使用事件通道2 */
	TC0_ConfigInputCapture( &TCC0, TC_EVSEL_CH0_gc );

	/* 使能通道A */
	TC0_EnableCCChannels( &TCC0, TC0_CCAEN_bm );

	/* 如果设置的周期寄存器的值比0x8000小，捕获完成后将
	把I/O引脚的电平变化存储在捕获寄存器的最高位（MSB）。
	Clear MSB of PER[H:L] to allow for propagation of edge polarity. */
	TC_SetPeriod( &TCC0, 0x7FFF );

	/* 选择时钟，启动定时器 */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1_gc );

	/* 使能通道A 低级别中断 */
	TC0_SetCCAIntLevel( &TCC0, TC_CCAINTLVL_LO_gc );
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	sei();

	do {
		/* Wait while interrupt measure Frequency and Duty cycle. */
	} while (1);
}


ISR(TCC0_CCA_vect)
{
	LED1_T();
	LED2_T();
	static uint32_t frequency;
	static uint32_t dutyCycle;
	static uint16_t totalPeriod;
	static uint16_t highPeriod;

	uint16_t thisCapture = TC_GetCaptureA( &TCC0 );

	/*  按上升沿来保存总周期值 */
	if ( thisCapture & 0x8000 ) {//MSB=1,代表高电平，上升沿
		totalPeriod = thisCapture & 0x7FFF;
		TC_Restart( &TCC0 );
	}
	 /* 下降沿保存高电平的周期 */
	else {
		highPeriod = thisCapture;
	}

	dutyCycle = ( ( ( highPeriod * 100 ) / totalPeriod ) + dutyCycle ) / 2;
	frequency = ( ( ( F_CPU / CPU_PRESCALER ) / totalPeriod ) + frequency ) / 2;
	 /* 串口打印占空比和频率 */
	uart_puts("dutyCycle = ");
	uart_putdw_dec(dutyCycle);
	uart_putc('\n');
	uart_puts("frequency = ");
	uart_putdw_dec(frequency);
	uart_putc('\n');
}

/*
+------------------------------------------------------------------------------
| Function    : Example4
+------------------------------------------------------------------------------
| Description : TCC0通道B占空比变化的脉宽调制，PC1输出	可观察LED亮灭占空比的变化		
+------------------------------------------------------------------------------
*/

void Example4( void )
{	
	uint16_t compareValue = 0x0000;
	/* PC1输出 */
	PORTC.DIRSET = 0x02;

	/* 设置计数周期 */
	TC_SetPeriod( &TCC0, 512 );

	/* 设置TC为单斜率模式 */
	TC0_ConfigWGM( &TCC0, TC_WGMODE_SS_gc );

	/* 使能通道B */
	TC0_EnableCCChannels( &TCC0, TC0_CCBEN_bm );


	/* 选择时钟，启动定时器 */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV64_gc );	


	do {
		/* 新比较值*/
		compareValue += 31; 
		if(compareValue>512)compareValue = 31; 

		 /*设置到缓冲寄存器*/
		TC_SetCompareB( &TCC0, compareValue );

		/*溢出时比较值从CCBBUF[H:L] 传递到CCB[H:L]*/	
		do {} while( TC_GetOverflowFlag( &TCC0 ) == 0 );

		/* 清除溢出标志 */
		TC_ClearOverflowFlag( &TCC0 );

	} while (1);
}


/*
+------------------------------------------------------------------------------
| Function    : Example5
+------------------------------------------------------------------------------
| Description : TCC0对事件信号（PE3下降沿）计数，溢出中断对PD4取反	
+------------------------------------------------------------------------------
*/
void Example5( void )
{
	/* PE3设为输入， 输入上拉，下降沿感知，DOWN键按下计数*/
	PORTE.PIN3CTRL = PORT_ISC_FALLING_gc + PORT_OPC_PULLUP_gc;
	PORTE.DIRCLR = 0x08;

	/* PD4设为输出*/
	PORTD.DIRSET = 0x30;

	/* 选择PE3为事件通道0的输入, 使能数字滤波*/
	EVSYS.CH0MUX = EVSYS_CHMUX_PORTE_PIN3_gc;
	EVSYS.CH0CTRL = EVSYS_DIGFILT_8SAMPLES_gc;

	/* 设置计数周期值-TOP */
	TC_SetPeriod( &TCC0, 4 );

	/* 设置溢出中断为低级别中断 */
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_LO_gc );
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	sei();

	/* 启动定时器 */
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_EVCH0_gc );

	do {} while (1);
}


ISR(TCC0_OVF_vect)
{
	/* 取反PD4 */
	PORTD.OUTTGL = 0x10;

	PORTD.OUTCLR = 0x20;
}


/*
+------------------------------------------------------------------------------
| Function    : Example6
+------------------------------------------------------------------------------
| Description : 32位计数,溢出对	PD4取反，LED间隔1S闪烁
+------------------------------------------------------------------------------
*/
void Example6( void )
{
	/* PD4设为输出 */
	PORTD.DIRSET = 0x10;
	/* TCC0溢出作为事件通道0的输入 */
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;

	/* 使能TCC1传播时延 */
	TC_EnableEventDelay( &TCC1 );
	/* 设置计数周期 */
	TC_SetPeriod( &TCC0, 1250 );
	TC_SetPeriod( &TCC1, 200 );
	/* 使用通道0作为TCC1时钟源 */
	TC1_ConfigClockSource( &TCC1, TC_CLKSEL_EVCH0_gc );

	/* 使用外设时钟8分频作为TCC0时钟源 启动定时器*/
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV8_gc );

	do {
		
		do {} while( TC_GetOverflowFlag( &TCC1 ) == 0 );
		/* 取反PD4 */
		PORTD.OUTTGL = 0x10;
		
		/* 清除溢出标志 */
		TC_ClearOverflowFlag( &TCC1 );
		
	} while (1);

}
