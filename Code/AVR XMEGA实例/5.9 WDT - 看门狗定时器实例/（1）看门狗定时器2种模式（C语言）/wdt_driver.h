#ifndef WDT_DRIVER_H
#define WDT_DRIVER_H


#include "avr_compiler.h"


//宏定义

//看门狗定时器的使能位置位时同步才发生
#define WDT_IsSyncBusy() ( WDT.STATUS & WDT_SYNCBUSY_bm )

//重置看门狗
#define WDT_Reset()	asm("wdr") //( watchdog_reset( ) )


//函数声明

void WDT_Enable( void );
void WDT_EnableAndSetTimeout( WDT_PER_t period );
void WDT_Disable( void );
bool WDT_IsWindowModeEnabled( void );
bool WDT_EnableWindowMode( void );
bool WDT_EnableWindowModeAndSetTimeout( WDT_WPER_t period );
void WDT_DisableWindowMode( void );

#endif
