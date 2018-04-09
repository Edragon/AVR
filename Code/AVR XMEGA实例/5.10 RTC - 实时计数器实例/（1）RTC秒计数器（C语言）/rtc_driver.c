#include "rtc_driver.h"


/*! \初始化RTC周期，计数值，比较值，分频值
 *  需要先检查RTC忙标志
 *
 *  \参数 period         RTC周期   Top值 = RTC周期 - 1.
 *  \参数 count          初始计数值
 *  \参数 compareValue   比较值.
 *  \参数 prescaler      分频值
 */
void RTC_Initialize( uint16_t period,
                     uint16_t count,
                     uint16_t compareValue,
                     RTC_PRESCALER_t prescaler )
{
	RTC.PER = period - 1;
	RTC.CNT = count;
	RTC.COMP = compareValue;
	RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | prescaler;
}


/*! \设置溢出中断级别
 *
 *  \参数 intLevel 溢出中断级别
 */
void RTC_SetOverflowIntLevel( RTC_OVFINTLVL_t intLevel )
{
	RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_OVFINTLVL_gm ) | intLevel;
}


/*! \设置比较中断级别
 *
 *  \参数 intLevel 比较中断级别
 */
void RTC_SetCompareIntLevel( RTC_COMPINTLVL_t intLevel )
{
	RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_COMPINTLVL_gm ) | intLevel;
}


/*! \设置比较中断，溢出中断级别
 *
 *  \参数 ovfIntLevel  溢出中断级别
 *  \参数 compIntLevel 比较中断级别
 */
void RTC_SetIntLevels( RTC_OVFINTLVL_t ovfIntLevel,
                       RTC_COMPINTLVL_t compIntLevel )
{
	RTC.INTCTRL = ( RTC.INTCTRL &
	              ~( RTC_COMPINTLVL_gm | RTC_OVFINTLVL_gm ) ) |
	              ovfIntLevel |
	              compIntLevel;
}


/*! \设置超时警报
 *
 *  \注意 需要先检查RTC忙标志，使用RTC_Busy()
 *
 *  \注意 超时值必须小于定时器周期
 *
 *  \参数 alarmTimeout Timeout time in RTC clock cycles (scaled).
 */
void RTC_SetAlarm( uint16_t alarmTimeout )
{
	//计算比较值
	uint16_t compareValue = RTC.CNT + alarmTimeout;

	//超过周期值
	if (compareValue > RTC.PER)
	{
		compareValue -= RTC.PER;
	}
	RTC.COMP = compareValue;
}


/*! \设置新的分频值
 *
 *  \注意 需要先检查RTC忙标志，使用RTC_Busy()
 *
 *  \参数 prescaler Clock prescaler setting.
 */
void RTC_SetPrescaler( RTC_PRESCALER_t prescaler )
{
	RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | prescaler;
}
