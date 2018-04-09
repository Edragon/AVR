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
     	本函数实现了秒计数器，RTC每秒产生中断，使用7个LED灯显示BCD码
		端口D的PIN0-3显示秒数个位，PIN4-6显示秒数十位
		PIN7上的LED间隔0.5秒闪烁
*/
#include "avr_compiler.h"
#include "rtc_driver.c"


#define LED_PORT          PORTD
#define RTC_CYCLES_1S     1024


//bcd码秒计数
typedef struct RTC_BCD_struct{
	uint8_t sec_ones;	//个位
	uint8_t sec_tens;	//十位
} RTC_BCD_t;


/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/

int main(void)
{
	//打开内部 32.768 kHz RC振荡器产生1.024 kHz
	OSC.CTRL |= OSC_RC32KEN_bm;

	do {//等待时钟稳定
	} while ( ( OSC.STATUS & OSC_RC32KRDY_bm ) == 0);

	//设置内部32kHz为RTC时钟源
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;

	//设置LED端口为输出
	LED_PORT.DIR = 0xFF;

	do {//检查RTC忙否
	} while ( RTC_Busy() );

	//配置RTC周期为1S
	RTC_Initialize( RTC_CYCLES_1S, 0, 0, RTC_PRESCALER_DIV1_gc );
	//设置间隔0.5秒PIN7上的LED闪烁
	RTC_SetAlarm(RTC_CYCLES_1S/2);

	//使能溢出中断
	RTC_SetOverflowIntLevel( RTC_OVFINTLVL_LO_gc);
	//使能比较中断
	RTC_SetCompareIntLevel( RTC_COMPINTLVL_LO_gc);

	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	do {
		nop();
	} while (1);
}


/*! \溢出中断服务程序 在端口D的LED上显示秒数
 */
ISR(RTC_OVF_vect)
{
	static RTC_BCD_t rtcTime;

	if ( ++rtcTime.sec_ones > 9 )
	{
		rtcTime.sec_ones = 0;
		rtcTime.sec_tens++;
	}

	if ( rtcTime.sec_tens > 5 )
	{
		rtcTime.sec_tens = 0;
	}

	LED_PORT.OUT = ( ( rtcTime.sec_tens << 4 ) | rtcTime.sec_ones );
}
/*! \比较中断服务程序 在端口D的LED上显示秒数
 */
ISR(RTC_COMP_vect)
   {
      LED_PORT.OUTTGL = 0X80;
	  RTC_SetAlarm(RTC_CYCLES_1S/2);
   }
