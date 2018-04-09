#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

#include "avr_compiler.h"

//宏定义

//检查RTC忙标志
#define RTC_Busy()               ( RTC.STATUS & RTC_SYNCBUSY_bm )

//返回RTC溢出标志
#define RTC_GetOverflowFlag()    ( RTC.INTFLAGS & RTC_OVFIF_bm )

//返回RTC比较标志
#define RTC_GetCompareFlag()     ( RTC.INTFLAGS & RTC_COMPIF_bm )

//设置新的RTC计数值，需要先检查RTC忙标志
#define RTC_SetCount( _rtccnt )  ( RTC.CNT = (_rtccnt) )

//返回当前计数值
#define RTC_GetCount()           ( RTC.CNT )

//设置RTC新周期值
#define RTC_SetPeriod( _rtcper ) ( RTC.PER = (_rtcper) )

//返回当前周期值
#define RTC_GetPeriod()          ( RTC.PER )

//设置新的RTC比较值，需要先检查RTC忙标志
#define RTC_SetCompareValue( _cmpVal ) ( RTC.COMP = (_cmpVal) )

//返回当前比较值
#define RTC_GetCompareValue()    ( RTC.COMP )


//函数声明

void RTC_Initialize( uint16_t period,uint16_t count,uint16_t compareValue,RTC_PRESCALER_t prescaler);
void RTC_SetOverflowIntLevel( RTC_OVFINTLVL_t intLevel );
void RTC_SetCompareIntLevel( RTC_COMPINTLVL_t intLevel );
void RTC_SetIntLevels( RTC_OVFINTLVL_t ovfIntLevel,RTC_COMPINTLVL_t compIntLevel );
void RTC_SetAlarm( uint16_t alarmTimeout );
void RTC_SetPrescaler( RTC_PRESCALER_t prescaler );

/*! 中断服务程序，拷贝到你的代码里
 *
   ISR(RTC_COMP_vect)
   {
   }

   ISR(RTC_OVF_vect)
   {
   }
 *
 */

#endif
