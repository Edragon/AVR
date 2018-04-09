/*
 * ������:
     CLK_OSC
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-09 14:33
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	������������XMEGAʱ�����ã���ͨ����ʱ�����ʹLED��˸�����Թ۲쵽ʱ�ӵı仯
*/
#include <avr/io.h>
#include "avr_compiler.h"
#include "clksys_driver.h"

//---------LED����--------

#define LED1_T()   PORTD_OUTTGL = 0x20

#define LED2_T()   PORTD_OUTTGL = 0x10

/*
+------------------------------------------------------------------------------
| Function    : Clk_OSC_Setting
+------------------------------------------------------------------------------
| Description : ʱ�����ú���
| Parameters  : CLK_SCLKSEL:
|    				 	      CLK_SCLKSEL_RC2M_gc = (0x00<<0), 	 Internal 2MHz RC Oscillator 
|   						  CLK_SCLKSEL_RC32M_gc = (0x01<<0),  Internal 32MHz RC Oscillator 
|   						  CLK_SCLKSEL_RC32K_gc = (0x02<<0),	 Internal 32kHz RC Oscillator 
|   						  CLK_SCLKSEL_XOSC_gc = (0x03<<0), 	 External Crystal Oscillator or Clock 
|   						  CLK_SCLKSEL_PLL_gc = (0x04<<0),  	 Phase Locked Loop
|				OSC_PLLSRC:
|							  OSC_PLLSRC_RC2M_gc = (0x00<<6),  	Internal 2MHz RC Oscillator   ����factor<=25 �ȶ�
|							  OSC_PLLSRC_RC32M_gc = (0x02<<6),  Internal 32MHz RC Oscillator 4��Ƶ ����factor<=6 �ȶ�
|							  OSC_PLLSRC_XOSC_gc = (0x03<<6),   �ⲿʱ��8M������Ϊ����Դ��СΪ0.4MHz ����factor<=6 �ȶ�
|							  
|				factor:		 ��Ƶ���ӵķ�Χ��1x��31x֮�䡣���Ƶ�ʲ��ܳ���200MHz��PLL��С���Ƶ��Ϊ10MHz��
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
	CLKSYS_XOSC_Config( OSC_FRQRANGE_2TO9_gc, false,OSC_XOSCSEL_XTAL_16KCLK_gc );//���þ���Χ ����ʱ��
	CLKSYS_Enable( OSC_XOSCEN_bm  );//ʹ���ⲿ����
	do {} while ( CLKSYS_IsReady( OSC_XOSCRDY_bm  ) == 0 );//�ȴ��ⲿ����׼����
	CLKSYS_PLL_Config( OSC_PLLSRC_XOSC_gc, factor );//���ñ�Ƶ���Ӳ�ѡ���ⲿ����ΪPLL�ο�ʱ��
	CLKSYS_Enable( OSC_PLLEN_bm );//ʹ��PLL��·
	do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );//�ȴ�PLL׼����
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc);//ѡ��ϵͳʱ��Դ
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );//����Ԥ��Ƶ��A,B,C��ֵ	
}

void RC32M_Initial(void)
{
	CLKSYS_Enable( OSC_RC32MEN_bm  );//ʹ��RC32M����
	do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm  ) == 0 );//�ȴ�RC32M����׼����
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc);//ѡ��ϵͳʱ��Դ
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );//����Ԥ��Ƶ��A,B,C��ֵ	
}

void RC2M_Initial(void)
{
	CLKSYS_Enable( OSC_RC2MEN_bm  );//ʹ��RC2M����
	do {} while ( CLKSYS_IsReady( OSC_RC2MRDY_bm  ) == 0 );//�ȴ�RC2M����׼����
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC2M_gc);//ѡ��ϵͳʱ��Դ
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );//����Ԥ��Ƶ��A,B,C��ֵ	
}

int main( void ) 
{	
	// PLL���ⲿ����8M�����8M*6=48M
	PLL_XOSC_Initial();
	
	//�ڲ�RC32M
	//RC32M_Initial();

	//�ڲ�RC2M
	//RC2M_Initial();

	PORTD_DIRSET = 0x30;//PD5��PD4������Ϊ���

	/* ���ö�ʱ��C0����������65535
	 * ʹ������ж�
	 */
	TCC0.PER = 65535;
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc;
	TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_MED_gc;

	/* ʹ�ܵͼ����жϣ���ȫ���ж�*/
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	sei();
	while(1)
	{}
}


/*�жϷ������ LED��˸*/
ISR(TCC0_OVF_vect)
{
	LED1_T();
	LED2_T();
}
