
#ifndef DAC_DRIVER_H
#define DAC_DRIVER_H

#include "avr_compiler.h"

//DACͨ��
typedef enum DAC_CH_enum {
	CH0 = 0x00,
	CH1 = 0x01,
} DAC_CH_t;


//DAC ������
typedef enum DAC_TRIG_enum {
	DAC_TRIG_0_0 = 0x00,                                //û�д���ͨ��
	DAC_TRIG_0_1 = DAC_CH0TRIG_bm,                      //����ͨ��0
	DAC_TRIG_1_0 = DAC_CH1TRIG_bm,                      //����ͨ��1
	DAC_TRIG_1_1 = ( DAC_CH1TRIG_bm | DAC_CH0TRIG_bm ),  //����ͨ��0��1
} DAC_TRIG_t;


//�궨��

/*! \�ر�ָ����DACģ��
 *
 *  \����  _dac  DACģ�飬 DACA �� DACB
 */
#define DAC_Disable( _dac )                ( (_dac)->CTRLA &= ~DAC_ENABLE_bm )


/*! \ʹ��������ڲ�ADC
 *
 *  ������У��DAC
 *
 *  \����  _dac   DACģ��,  DACA �� DACB.
 */
#define DAC_InternalOutput_Enable( _dac )  ( (_dac)->CTRLA |= DAC_IDOEN_bm )


/*! \�ر�������ڲ�ADC
 *
 *  \����  _dac   DACģ��,  DACA �� DACB.
 */
#define DAC_InternalOutput_Disable( _dac ) ( (_dac)->CTRLA &= ~DAC_IDOEN_bm )


//��������

void DAC_SingleChannel_Enable( volatile DAC_t * dac,
                               DAC_REFSEL_t convRef,
                               bool leftAdjust );
void DAC_DualChannel_Enable( volatile DAC_t * dac,
                             DAC_REFSEL_t convRef,
                             bool leftAdjust,
                             DAC_CONINTVAL_t sampleInterval,
                             DAC_REFRESH_t refreshInterval );
void DAC_Channel_Write( volatile DAC_t * dac, uint16_t data, DAC_CH_t channel );
bool DAC_Channel_DataEmpty( volatile DAC_t * dac, DAC_CH_t channel );
void DAC_EventAction_Set( volatile DAC_t * dac,
                          DAC_TRIG_t trigChannel,
                          uint8_t eventLine );

#endif

