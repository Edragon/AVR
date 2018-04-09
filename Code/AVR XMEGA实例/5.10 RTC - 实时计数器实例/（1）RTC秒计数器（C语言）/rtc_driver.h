#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

#include "avr_compiler.h"

//�궨��

//���RTCæ��־
#define RTC_Busy()               ( RTC.STATUS & RTC_SYNCBUSY_bm )

//����RTC�����־
#define RTC_GetOverflowFlag()    ( RTC.INTFLAGS & RTC_OVFIF_bm )

//����RTC�Ƚϱ�־
#define RTC_GetCompareFlag()     ( RTC.INTFLAGS & RTC_COMPIF_bm )

//�����µ�RTC����ֵ����Ҫ�ȼ��RTCæ��־
#define RTC_SetCount( _rtccnt )  ( RTC.CNT = (_rtccnt) )

//���ص�ǰ����ֵ
#define RTC_GetCount()           ( RTC.CNT )

//����RTC������ֵ
#define RTC_SetPeriod( _rtcper ) ( RTC.PER = (_rtcper) )

//���ص�ǰ����ֵ
#define RTC_GetPeriod()          ( RTC.PER )

//�����µ�RTC�Ƚ�ֵ����Ҫ�ȼ��RTCæ��־
#define RTC_SetCompareValue( _cmpVal ) ( RTC.COMP = (_cmpVal) )

//���ص�ǰ�Ƚ�ֵ
#define RTC_GetCompareValue()    ( RTC.COMP )


//��������

void RTC_Initialize( uint16_t period,uint16_t count,uint16_t compareValue,RTC_PRESCALER_t prescaler);
void RTC_SetOverflowIntLevel( RTC_OVFINTLVL_t intLevel );
void RTC_SetCompareIntLevel( RTC_COMPINTLVL_t intLevel );
void RTC_SetIntLevels( RTC_OVFINTLVL_t ovfIntLevel,RTC_COMPINTLVL_t compIntLevel );
void RTC_SetAlarm( uint16_t alarmTimeout );
void RTC_SetPrescaler( RTC_PRESCALER_t prescaler );

/*! �жϷ�����򣬿�������Ĵ�����
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
