
#include "dac_driver.h"


/*! \ʹ�ܵ���ͨ����ͨ��0
 *
 *  \����  dac         DACģ��Ĵ�����ָ��
 *  \����  convRef     ѡ������ת���Ĳο���ѹ
 *  \����  leftAdjust  Ϊ��ʱ�������
 */
void DAC_SingleChannel_Enable( volatile DAC_t * dac,
                              DAC_REFSEL_t convRef,
                              bool leftAdjust )
{
	dac->CTRLB = ( dac->CTRLB & ~DAC_CHSEL_gm ) | DAC_CHSEL_SINGLE_gc;
	dac->CTRLC = ( dac->CTRLC & ~(DAC_REFSEL_gm | DAC_LEFTADJ_bm) ) |
	             convRef | ( leftAdjust ? DAC_LEFTADJ_bm : 0x00 );
	dac->CTRLA = ( dac->CTRLA & ~DAC_CH1EN_bm ) |
	             DAC_CH0EN_bm | DAC_ENABLE_bm;
}


/*! \ʹ��˫ͨ��
 *
 *  \����  dac              DACģ��Ĵ�����ָ��
 *  \����  convRef          ѡ������ת���Ĳο���ѹ
 *  \����  leftAdjust       Ϊ��ʱ�������
 *  \����  sampleInterval   DACת�������2��ת������С���
 *  \����  refreshInterval  ˫ͨ��ģʽ��ˢ�¼��
 */
void DAC_DualChannel_Enable( volatile DAC_t * dac,
                            DAC_REFSEL_t convRef,
                            bool leftAdjust,
                            DAC_CONINTVAL_t sampleInterval,
                            DAC_REFRESH_t refreshInterval )
{
	dac->CTRLB = ( dac->CTRLB & ~DAC_CHSEL_gm ) | DAC_CHSEL_DUAL_gc;
	dac->CTRLC = ( dac->CTRLC & ~( DAC_REFSEL_gm | DAC_LEFTADJ_bm ) ) |
	             convRef |
	             ( leftAdjust ? DAC_LEFTADJ_bm : 0x00 );
	dac->TIMCTRL = (uint8_t) sampleInterval | refreshInterval;
	dac->CTRLA |= DAC_CH1EN_bm | DAC_CH0EN_bm | DAC_ENABLE_bm;
}


/*! \��ѡ���ͨ��д����
 *
 *  \����  dac     DACģ��Ĵ�����ָ��
 *  \����  data    Ҫת��������
 *  \����  channel CH0 �� CH1
 */
void DAC_Channel_Write( volatile DAC_t * dac, uint16_t data, DAC_CH_t channel )
{
	if ( channel == CH0 )
	{
		dac->CH0DATA = data;
	}
	else
	{
		dac->CH1DATA = data;
	}
}


/*! \���ͨ�����ݼĴ����Ƿ�Ϊ��
 *
 *  ��д������ʱ��Ҫ������ݼĴ����Ƿ�Ϊ��
 *
 *  \����  dac     DACģ��Ĵ�����ָ��
 *  \����  channel CH0 �� CH1.
 *
 *  \retval dac״̬ ��-��
 *  \retval dac״̬ ��-�ǿ�
 */
bool DAC_Channel_DataEmpty( volatile DAC_t * dac, DAC_CH_t channel )
{
	bool dacStatus = ( dac->STATUS &
	                 ( channel ? DAC_CH1DRE_bm : DAC_CH0DRE_bm ));
	return dacStatus;
}


/*! \�¼���Ϊ����
 *
 *  \ע��  û�м���¼�ͨ���Ƿ���Ч
 *
 *  \����  dac         DACģ��Ĵ�����ָ��
 *  \����  trigChannel ������ͨ�� DAC_TRIG_0_0, DAC_TRIG_0_1, DAC_TRIG_1_0 �� DAC_TRIG_0_0.
 *  \����  eventLine   �¼�ͨ��(0..7)
 */
void DAC_EventAction_Set( volatile DAC_t * dac,
                          DAC_TRIG_t trigChannel,
                          uint8_t eventLine )
{
	dac->CTRLB = ( dac->CTRLB & ~DAC_TRIG_1_1 ) | trigChannel;
	dac->EVCTRL = eventLine & DAC_EVSEL_gm;
}
