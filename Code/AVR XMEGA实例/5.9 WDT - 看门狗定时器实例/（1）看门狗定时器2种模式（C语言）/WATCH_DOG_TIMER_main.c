/*
 * 工程名:
     WATCH_DOG_TIMER
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
     	void wdt_fuse_enable_example( void );
		熔丝设定看门狗超时周期，窗口周期，程序开启窗口模式

		void wdt_sw_enable_example( void );
		程序开启看门狗普通模式，窗口模式

		USARTC0打印调试信息。
*/
#include "avr_compiler.h"
#include "usart_driver.c"
#include "wdt_driver.c"

//定义CPU时钟
#define F_CPU (2000000UL)

void wdt_fuse_enable_example( void );
void wdt_sw_enable_example( void );

//看门狗超时周期毫秒数
#define TO_WD     128
//看门狗关闭周期毫秒数
#define TO_WDW    64

//窗口模式下看门狗重置间隔时间
#define WINDOW_MODE_DELAY   ( (TO_WDW) +  (TO_WD / 2) )

//普通模式下看门狗重置间隔时间
#define NORMAL_MODE_DELAY   ( TO_WD / 2 )
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
	uart_puts("inside main");uart_putc('\n');
	//wdt_sw_enable_example();
	wdt_fuse_enable_example();

}

/*
+------------------------------------------------------------------------------
| Function    : wdt_sw_enable_example
+------------------------------------------------------------------------------
| Description : 程序中，开启窗口模式（WD已使能）注意：WDLOCK熔丝位不能置位
+------------------------------------------------------------------------------
*/
void wdt_sw_enable_example( void )
{
	uart_puts("inside void wdt_sw_enable_example( void )");uart_putc('\n');

	//使能看门狗普通模式，超时周期为32CLK=32 ms
	WDT_EnableAndSetTimeout( WDT_PER_32CLK_gc );
	
	if(WDT.CTRL & WDT_ENABLE_bm)
	{
		uart_puts("WDT Is  Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not Enabled");uart_putc('\n');
	}
	//如果需要重新配置看门狗需要先关闭看门狗
	WDT_Disable();

	if(WDT.CTRL & WDT_ENABLE_bm)
	{
		uart_puts("WDT Is  Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not Enabled");uart_putc('\n');
	}

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}


	//在配置窗口模式前需要进行新的看门狗普通模式配置，
	//因为窗口模式只有在普通模式使能时才能开启
	WDT_EnableAndSetTimeout( WDT_PER_128CLK_gc );

	//配置窗口模式
	WDT_EnableWindowModeAndSetTimeout( WDT_WPER_64CLK_gc );

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}

	while(true)
	{
		uint16_t repeatCounter;
		//确保不要过早重置看门狗，否则会导致系统复位，插入延时要大于开放的窗口周期
		for (repeatCounter = WINDOW_MODE_DELAY; repeatCounter > 0; --repeatCounter )
		{
			delay_us( 1000 ); // 1ms 延迟 @ 2MHz
		}
		WDT_Reset();//重置看门狗
	}
}

/*
+------------------------------------------------------------------------------
| Function    : wdt_fuse_enable_example
+------------------------------------------------------------------------------
| Description : 看门狗通过熔丝位设定
| 			注意  本函数需要WDLOCK熔丝位置位，看门狗周期为128CLK，窗口周期为64CLK
+------------------------------------------------------------------------------
*/
void wdt_fuse_enable_example( void )
{

	uart_puts("inside void wdt_fuse_enable_example( void )");uart_putc('\n');
	//当看门狗通过熔丝位设定后，需要重置看门狗
	WDT_Reset();

	if(WDT.CTRL & WDT_ENABLE_bm)
	{
		uart_puts("WDT Is  Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not Enabled");uart_putc('\n');
	}

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}


	if (true == WDT_IsWindowModeEnabled()){
		while(1){
			//如果窗口模式使能，系统将复位
		}
	}

	//如果窗口模式未通过熔丝使能，需要在程序中使能，但是周期数不能设定，因为熔丝位已经置位
	WDT_EnableWindowMode();

	// 看门狗重置最好在(TO_WDW * 1.3) 和((TO_WDW + TO_WD)*0.7) 之间

	delay_us( 90000 );
	WDT_Reset();

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}


	WDT_DisableWindowMode();

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}


	while(true)
	{
		uint16_t repeatCounter;
		for (repeatCounter = NORMAL_MODE_DELAY; repeatCounter > 0; --repeatCounter ) {
			delay_us( 1000 ); 
		}
		WDT_Reset();
	}
}
