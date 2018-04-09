#include "rtc_driver.h"


/*! \��ʼ��RTC���ڣ�����ֵ���Ƚ�ֵ����Ƶֵ
 *  ��Ҫ�ȼ��RTCæ��־
 *
 *  \���� period         RTC����   Topֵ = RTC���� - 1.
 *  \���� count          ��ʼ����ֵ
 *  \���� compareValue   �Ƚ�ֵ.
 *  \���� prescaler      ��Ƶֵ
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


/*! \��������жϼ���
 *
 *  \���� intLevel ����жϼ���
 */
void RTC_SetOverflowIntLevel( RTC_OVFINTLVL_t intLevel )
{
	RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_OVFINTLVL_gm ) | intLevel;
}


/*! \���ñȽ��жϼ���
 *
 *  \���� intLevel �Ƚ��жϼ���
 */
void RTC_SetCompareIntLevel( RTC_COMPINTLVL_t intLevel )
{
	RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_COMPINTLVL_gm ) | intLevel;
}


/*! \���ñȽ��жϣ�����жϼ���
 *
 *  \���� ovfIntLevel  ����жϼ���
 *  \���� compIntLevel �Ƚ��жϼ���
 */
void RTC_SetIntLevels( RTC_OVFINTLVL_t ovfIntLevel,
                       RTC_COMPINTLVL_t compIntLevel )
{
	RTC.INTCTRL = ( RTC.INTCTRL &
	              ~( RTC_COMPINTLVL_gm | RTC_OVFINTLVL_gm ) ) |
	              ovfIntLevel |
	              compIntLevel;
}


/*! \���ó�ʱ����
 *
 *  \ע�� ��Ҫ�ȼ��RTCæ��־��ʹ��RTC_Busy()
 *
 *  \ע�� ��ʱֵ����С�ڶ�ʱ������
 *
 *  \���� alarmTimeout Timeout time in RTC clock cycles (scaled).
 */
void RTC_SetAlarm( uint16_t alarmTimeout )
{
	//����Ƚ�ֵ
	uint16_t compareValue = RTC.CNT + alarmTimeout;

	//��������ֵ
	if (compareValue > RTC.PER)
	{
		compareValue -= RTC.PER;
	}
	RTC.COMP = compareValue;
}


/*! \�����µķ�Ƶֵ
 *
 *  \ע�� ��Ҫ�ȼ��RTCæ��־��ʹ��RTC_Busy()
 *
 *  \���� prescaler Clock prescaler setting.
 */
void RTC_SetPrescaler( RTC_PRESCALER_t prescaler )
{
	RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | prescaler;
}
