#include "wdt_driver.h"



/*! \使能看门狗，不改变看门狗超时周期
 *
 *  看门狗自动复位
 *
 *  写CTRL寄存器时需要解除CCP保护，等待看门狗和其他始终与同步完成
 */
void WDT_Enable( void )
{
	uint8_t temp = WDT.CTRL | WDT_ENABLE_bm | WDT_CEN_bm;
	CCP = CCP_IOREG_gc;
	WDT.CTRL = temp;
	while(WDT_IsSyncBusy()){}
}

/*! \使能看门狗，改变看门狗超时周期
 *
 *
 *  \参数  period  看门狗超时周期
 */
void WDT_EnableAndSetTimeout( WDT_PER_t period )
{
	uint8_t temp = WDT_ENABLE_bm | WDT_CEN_bm | period;
	CCP = CCP_IOREG_gc;
	WDT.CTRL = temp;
	while(WDT_IsSyncBusy()){}
}


/*! \关闭看门狗，不改变看门狗超时周期
 *
 *  注意，看门狗只能安全级别1下才能关闭
 *
 *  立即生效
 */
void WDT_Disable( void )
{
	uint8_t temp = (WDT.CTRL & ~WDT_ENABLE_bm) | WDT_CEN_bm;
	CCP = CCP_IOREG_gc;
	WDT.CTRL = temp;
}


/*! \返回窗口模式使能位的状态
 *
 *  \retval  true   窗口模式使能
 *  \retval  false  窗口模式关闭
 */
bool WDT_IsWindowModeEnabled( void )
{
	uint8_t wdwm_enabled = (WDT.WINCTRL & WDT_WEN_bm);
	return wdwm_enabled;
}

/*! \使能窗口模式，不改变看门狗超时周期
 *	WD必须在窗口模式开启前使能
 *  \retval  true   WD在窗口模式开启前使能
 *  \retval  false  WD在窗口模式开启前未使能
 */
bool WDT_EnableWindowMode( void )
{
	uint8_t wd_enable = WDT.CTRL & WDT_ENABLE_bm;
	uint8_t temp = WDT.WINCTRL | WDT_WEN_bm | WDT_WCEN_bm;
	CCP = CCP_IOREG_gc;
	WDT.WINCTRL = temp;
	while(WDT_IsSyncBusy()){}
        
	return wd_enable;
}



/*! \使能窗口模式，改变看门狗超时周期
 * 	WD必须在窗口模式开启前使能
 *  \retval  true   WD在窗口模式开启前使能
 *  \retval  false  WD在窗口模式开启前未使能
 */
bool WDT_EnableWindowModeAndSetTimeout( WDT_WPER_t period )
{
	uint8_t wd_enable = WDT.CTRL & WDT_ENABLE_bm;
	uint8_t temp = WDT_WEN_bm | WDT_WCEN_bm | period;
	CCP = CCP_IOREG_gc;
	WDT.WINCTRL = temp;
	while(WDT_IsSyncBusy()){}
        
	return wd_enable;
}


 /*! \关闭窗口模式，不改变看门狗超时周期
 *
 *  注意，窗口模式只能安全级别1，2下才能关闭
 *
 *  立即生效
 */
void WDT_DisableWindowMode( void )
{
	uint8_t temp = (WDT.WINCTRL & ~WDT_WEN_bm) | WDT_WCEN_bm;
	CCP = CCP_IOREG_gc;
	WDT.WINCTRL = temp;
}
