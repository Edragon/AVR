/*
 * 工程名:
     TC_PWM_Sine
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-12 13:58
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	TCC0通道B占空比按照正弦样本值变化，输出波形通过二阶或多阶低通滤波器
		输出可获得较平滑正弦曲线,频率越低，样点越密，输出波形越平滑
		（2000000/8/255/128=7HZ）
		注意 样本数组定义时使用 PROGMEM 出现异常，即样本值不能存储到程序区
*/
#include "avr_compiler.h"
#include "TC_driver.c"

#define LED1_T()   PORTD_OUTTGL = 0x20

unsigned char SineWaveTable128[128]  = {
128,134,140,147,153,159,165,171,177,182,188,193,199,204,209,213,
218,222,226,230,234,237,240,243,245,248,250,251,253,254,254,255, 
255,255,254,254,253,251,250,248,245,243,240,237,234,230,226,222,
218,213,209,204,199,193,188,182,177,171,165,159,153,147,140,134,
128,122,116,109,103,97,91,85,79,74,68,63,57,52,47,43, 38,34,30,
26,22,19,16,13,11,8,6,5,3,2,2,1, 1,1,2,2,3,5,6,8,11,13,16,19,22
,26,30,34, 38,43,47,52,57,63,68,74,79,85,91,97,103,109,116,122}; // 128点正弦波样本值

uint16_t compareValue = 0x0000;
int main(void)
{
	PORTD_DIR = 0x20;//PD5方向设为输出
	/* PC1输出 */
	PORTC.DIRSET = 0x02;

	/* 设置计数周期 */
	TC_SetPeriod( &TCC0, 255 );

	/* 设置TC为单斜率模式 */
	TC0_ConfigWGM( &TCC0, TC_WGMODE_SS_gc );

	/* 使能通道B */
	TC0_EnableCCChannels( &TCC0, TC0_CCBEN_bm );

	/* 设置溢出中断为低级别中断 */
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_LO_gc );
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	sei();

	/* 选择时钟，启动定时器 */
		
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV64_gc );

	do {
		/*溢出时比较值从CCBBUF[H:L] 传递到CCB[H:L]*/	
	} while (1);

}

ISR(TCC0_OVF_vect)
{
	LED1_T();//溢出指示灯
	/* 新比较值*/
	compareValue ++; 
	if(compareValue>128){compareValue = 0; }
	
	 /*设置到缓冲寄存器*/
	TC_SetCompareB( &TCC0, SineWaveTable128[compareValue] );

	/*溢出时比较值从CCBBUF[H:L] 传递到CCB[H:L]*/	
}
