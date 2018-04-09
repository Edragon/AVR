/*
 * 工程名:
     CLK_OSC
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-09 14:33
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序给出多个XMEGA时钟配置，并通过定时器溢出使LED闪烁，可以观察到时钟的变化
*/
#include <avr/io.h>
#include "avr_compiler.h"
#include "clksys_driver.h"

//---------LED操作--------

#define LED1_T()   PORTD_OUTTGL = 0x20

#define LED2_T()   PORTD_OUTTGL = 0x10

/*
+------------------------------------------------------------------------------
| Function    : Clk_OSC_Setting
+------------------------------------------------------------------------------
| Description : 时钟设置函数
| Parameters  : CLK_SCLKSEL:
|    				 	      CLK_SCLKSEL_RC2M_gc = (0x00<<0), 	 Internal 2MHz RC Oscillator 
|   						  CLK_SCLKSEL_RC32M_gc = (0x01<<0),  Internal 32MHz RC Oscillator 
|   						  CLK_SCLKSEL_RC32K_gc = (0x02<<0),	 Internal 32kHz RC Oscillator 
|   						  CLK_SCLKSEL_XOSC_gc = (0x03<<0), 	 External Crystal Oscillator or Clock 
|   						  CLK_SCLKSEL_PLL_gc = (0x04<<0),  	 Phase Locked Loop
|				OSC_PLLSRC:
|							  OSC_PLLSRC_RC2M_gc = (0x00<<6),  	Internal 2MHz RC Oscillator   测试factor<=25 稳定
|							  OSC_PLLSRC_RC32M_gc = (0x02<<6),  Internal 32MHz RC Oscillator 4分频 测试factor<=6 稳定
|							  OSC_PLLSRC_XOSC_gc = (0x03<<6),   外部时钟8M，若作为输入源最小为0.4MHz 测试factor<=6 稳定
|							  
|				factor:		 倍频因子的范围在1x和31x之间。输出频率不能超过200MHz。PLL最小输出频率为10MHz。
|				CLK_PSADIV:
|							CLK_PSADIV_1_gc = (0x00<<2),    Divide by 1  
|						    CLK_PSADIV_2_gc = (0x01<<2),    Divide by 2  
|						    CLK_PSADIV_4_gc = (0x03<<2),    Divide by 4  
|						    CLK_PSADIV_8_gc = (0x05<<2),    Divide by 8  
|						    CLK_PSADIV_16_gc = (0x07<<2),   Divide by 16  
|						    CLK_PSADIV_32_gc = (0x09<<2),   Divide by 32 
|						    CLK_PSADIV_64_gc = (0x0B<<2),   Divide by 64 
|						    CLK_PSADIV_128_gc = (0x0D<<2),  Divide by 128  
|						    CLK_PSADIV_256_gc = (0x0F<<2),  Divide by 256 
|						    CLK_PSADIV_512_gc = (0x11<<2),  Divide by 512 
|				CLK_PSBCDIV:
|						|	CLK_PSBCDIV_1_1_gc = (0x00<<0),  Divide B by 1 and C by 1 
|						    CLK_PSBCDIV_1_2_gc = (0x01<<0),  Divide B by 1 and C by 2 
|						    CLK_PSBCDIV_4_1_gc = (0x02<<0),  Divide B by 4 and C by 1 
|						    CLK_PSBCDIV_2_2_gc = (0x03<<0),   Divide B by 2 and C by 2 
|
|
+------------------------------------------------------------------------------
*/
void PLL_XOSC_Initial(void)
{
	unsigned char factor = 6;
	CLKSYS_XOSC_Config( OSC_FRQRANGE_2TO9_gc, false,OSC_XOSCSEL_XTAL_16KCLK_gc );//设置晶振范围 启动时间
	CLKSYS_Enable( OSC_XOSCEN_bm  );//使能外部振荡器
	do {} while ( CLKSYS_IsReady( OSC_XOSCRDY_bm  ) == 0 );//等待外部振荡器准备好
	CLKSYS_PLL_Config( OSC_PLLSRC_XOSC_gc, factor );//设置倍频因子并选择外部振荡器为PLL参考时钟
	CLKSYS_Enable( OSC_PLLEN_bm );//使能PLL电路
	do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );//等待PLL准备好
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc);//选择系统时钟源
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );//设置预分频器A,B,C的值	
}

void RC32M_Initial(void)
{
	CLKSYS_Enable( OSC_RC32MEN_bm  );//使能RC32M振荡器
	do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm  ) == 0 );//等待RC32M振荡器准备好
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc);//选择系统时钟源
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );//设置预分频器A,B,C的值	
}

void RC2M_Initial(void)
{
	CLKSYS_Enable( OSC_RC2MEN_bm  );//使能RC2M振荡器
	do {} while ( CLKSYS_IsReady( OSC_RC2MRDY_bm  ) == 0 );//等待RC2M振荡器准备好
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC2M_gc);//选择系统时钟源
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );//设置预分频器A,B,C的值	
}

int main( void ) 
{	
	// PLL，外部晶振8M，输出8M*6=48M
	PLL_XOSC_Initial();
	
	//内部RC32M
	//RC32M_Initial();

	//内部RC2M
	//RC2M_Initial();

	PORTD_DIRSET = 0x30;//PD5，PD4方向设为输出

	/* 设置定时器C0，计数周期65535
	 * 使能溢出中断
	 */
	TCC0.PER = 65535;
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc;
	TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_MED_gc;

	/* 使能低级别中断，打开全局中断*/
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	sei();
	while(1)
	{}
}


/*中断服务程序 LED闪烁*/
ISR(TCC0_OVF_vect)
{
	LED1_T();
	LED2_T();
}
