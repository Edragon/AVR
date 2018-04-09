#ifndef WDT_DRIVER_H
#define WDT_DRIVER_H


#include "avr_compiler.h"


//�궨��

//���Ź���ʱ����ʹ��λ��λʱͬ���ŷ���
#define WDT_IsSyncBusy() ( WDT.STATUS & WDT_SYNCBUSY_bm )

//���ÿ��Ź�
#define WDT_Reset()	asm("wdr") //( watchdog_reset( ) )


//��������

void WDT_Enable( void );
void WDT_EnableAndSetTimeout( WDT_PER_t period );
void WDT_Disable( void );
bool WDT_IsWindowModeEnabled( void );
bool WDT_EnableWindowMode( void );
bool WDT_EnableWindowModeAndSetTimeout( WDT_WPER_t period );
void WDT_DisableWindowMode( void );

#endif
