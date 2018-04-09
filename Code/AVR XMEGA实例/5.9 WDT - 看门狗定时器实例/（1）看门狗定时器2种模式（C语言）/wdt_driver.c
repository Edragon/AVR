#include "wdt_driver.h"



/*! \ʹ�ܿ��Ź������ı俴�Ź���ʱ����
 *
 *  ���Ź��Զ���λ
 *
 *  дCTRL�Ĵ���ʱ��Ҫ���CCP�������ȴ����Ź�������ʼ����ͬ�����
 */
void WDT_Enable( void )
{
	uint8_t temp = WDT.CTRL | WDT_ENABLE_bm | WDT_CEN_bm;
	CCP = CCP_IOREG_gc;
	WDT.CTRL = temp;
	while(WDT_IsSyncBusy()){}
}

/*! \ʹ�ܿ��Ź����ı俴�Ź���ʱ����
 *
 *
 *  \����  period  ���Ź���ʱ����
 */
void WDT_EnableAndSetTimeout( WDT_PER_t period )
{
	uint8_t temp = WDT_ENABLE_bm | WDT_CEN_bm | period;
	CCP = CCP_IOREG_gc;
	WDT.CTRL = temp;
	while(WDT_IsSyncBusy()){}
}


/*! \�رտ��Ź������ı俴�Ź���ʱ����
 *
 *  ע�⣬���Ź�ֻ�ܰ�ȫ����1�²��ܹر�
 *
 *  ������Ч
 */
void WDT_Disable( void )
{
	uint8_t temp = (WDT.CTRL & ~WDT_ENABLE_bm) | WDT_CEN_bm;
	CCP = CCP_IOREG_gc;
	WDT.CTRL = temp;
}


/*! \���ش���ģʽʹ��λ��״̬
 *
 *  \retval  true   ����ģʽʹ��
 *  \retval  false  ����ģʽ�ر�
 */
bool WDT_IsWindowModeEnabled( void )
{
	uint8_t wdwm_enabled = (WDT.WINCTRL & WDT_WEN_bm);
	return wdwm_enabled;
}

/*! \ʹ�ܴ���ģʽ�����ı俴�Ź���ʱ����
 *	WD�����ڴ���ģʽ����ǰʹ��
 *  \retval  true   WD�ڴ���ģʽ����ǰʹ��
 *  \retval  false  WD�ڴ���ģʽ����ǰδʹ��
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



/*! \ʹ�ܴ���ģʽ���ı俴�Ź���ʱ����
 * 	WD�����ڴ���ģʽ����ǰʹ��
 *  \retval  true   WD�ڴ���ģʽ����ǰʹ��
 *  \retval  false  WD�ڴ���ģʽ����ǰδʹ��
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


 /*! \�رմ���ģʽ�����ı俴�Ź���ʱ����
 *
 *  ע�⣬����ģʽֻ�ܰ�ȫ����1��2�²��ܹر�
 *
 *  ������Ч
 */
void WDT_DisableWindowMode( void )
{
	uint8_t temp = (WDT.WINCTRL & ~WDT_WEN_bm) | WDT_WCEN_bm;
	CCP = CCP_IOREG_gc;
	WDT.WINCTRL = temp;
}
