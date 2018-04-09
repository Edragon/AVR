
#include "dac_driver.h"


/*! \使能单个通道，通道0
 *
 *  \参数  dac         DAC模块寄存器区指针
 *  \参数  convRef     选择所有转换的参考电压
 *  \参数  leftAdjust  为真时，左对齐
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


/*! \使能双通道
 *
 *  \参数  dac              DAC模块寄存器区指针
 *  \参数  convRef          选择所有转换的参考电压
 *  \参数  leftAdjust       为真时，左对齐
 *  \参数  sampleInterval   DAC转换间隔，2次转换的最小间隔
 *  \参数  refreshInterval  双通道模式下刷新间隔
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


/*! \对选择的通道写数据
 *
 *  \参数  dac     DAC模块寄存器区指针
 *  \参数  data    要转换的数据
 *  \参数  channel CH0 或 CH1
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


/*! \检查通道数据寄存器是否为空
 *
 *  在写新数据时，要检查数据寄存器是否为空
 *
 *  \参数  dac     DAC模块寄存器区指针
 *  \参数  channel CH0 或 CH1.
 *
 *  \retval dac状态 真-空
 *  \retval dac状态 假-非空
 */
bool DAC_Channel_DataEmpty( volatile DAC_t * dac, DAC_CH_t channel )
{
	bool dacStatus = ( dac->STATUS &
	                 ( channel ? DAC_CH1DRE_bm : DAC_CH0DRE_bm ));
	return dacStatus;
}


/*! \事件行为配置
 *
 *  \注意  没有检查事件通道是否有效
 *
 *  \参数  dac         DAC模块寄存器区指针
 *  \参数  trigChannel 触发的通道 DAC_TRIG_0_0, DAC_TRIG_0_1, DAC_TRIG_1_0 或 DAC_TRIG_0_0.
 *  \参数  eventLine   事件通道(0..7)
 */
void DAC_EventAction_Set( volatile DAC_t * dac,
                          DAC_TRIG_t trigChannel,
                          uint8_t eventLine )
{
	dac->CTRLB = ( dac->CTRLB & ~DAC_TRIG_1_1 ) | trigChannel;
	dac->EVCTRL = eventLine & DAC_EVSEL_gm;
}
