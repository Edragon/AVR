/*
 * 工程名:
     AC
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-11 1:17
 * 作者:
 		 
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
		 /*定时器C0的通道A与B单斜率模式产生波形作为AC引脚pin0与pin1负引脚是内部的分压
          AC一个是上升沿中断一个是下降沿中断在中断中控制PD.4和PD.5取反，会看到灯的不停
		  闪烁

*/

#include "ac_driver.c"
#include "TC_driver.c"

/* The Analog comparator used in the example.*/
#define AC ACA
#define LED1_T()   PORTD_OUTTGL = 0x20
#define LED2_T()   PORTD_OUTTGL = 0x10
/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main(void)
{  
 
     PORTCFG.VPCTRLA=0x10;//;PORTB映射到虚拟端口1，PORTA映射到虚拟端口0
	    
	 PORTCFG.VPCTRLB=0x32;//;PORTC映射到虚拟端口2，PORTD映射到虚拟端口3
 
	 VPORT0_DIR=0x00; //PORTA引脚输入
     VPORT2_DIR=0xFF; //PORTC引脚输出
     VPORT3_DIR=0xFF; //PORTD引脚输出

	 /* 设置计数周期 */
	 TC_SetPeriod( &TCC0,4000 );
     TC_SetCompareA( &TCC0,1000);
	 TC_SetCompareB( &TCC0,2000);

	/* 设置TC为单斜率模式 */
	 TC0_ConfigWGM( &TCC0, TC_WGMODE_SS_gc );

	/* 使能通道A B */
	 TC0_EnableCCChannels( &TCC0, TC0_CCAEN_bm );
	 TC0_EnableCCChannels( &TCC0, TC0_CCBEN_bm );


	 /* 选择时钟，启动定时器 */
	 TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1024_gc );
	 

	//使能模拟比较器AC0 AC1
	 AC_Enable(&AC, ANALOG_COMPARATOR0, false);
	 AC_Enable(&AC, ANALOG_COMPARATOR1, false);

	 //模拟比较器输入电压比例因子
     AC_ConfigVoltageScaler(&AC,0);
	/* 设置模拟比较器0的引脚是 pin 0 and 1. */
	 AC_ConfigMUX(&AC, ANALOG_COMPARATOR0, AC_MUXPOS_PIN0_gc, AC_MUXNEG_SCALER_gc);

	 AC_ConfigMUX(&AC, ANALOG_COMPARATOR1, AC_MUXPOS_PIN2_gc, AC_MUXNEG_SCALER_gc);
 

	/*设置AC0 AC1的磁滞 */ 
	 AC_ConfigHysteresis(&AC, ANALOG_COMPARATOR0, AC_HYSMODE_SMALL_gc);
	 AC_ConfigHysteresis(&AC, ANALOG_COMPARATOR1, AC_HYSMODE_SMALL_gc);

	 AC_ConfigInterrupt(&AC, ANALOG_COMPARATOR0,AC_INTMODE_RISING_gc,AC_INTLVL_LO_gc);
	 AC_ConfigInterrupt(&AC, ANALOG_COMPARATOR1,AC_INTMODE_FALLING_gc,AC_INTLVL_LO_gc);
 
     PMIC.CTRL |=PMIC_MEDLVLEN_bm+PMIC_LOLVLEN_bm+PMIC_HILVLEN_bm; //Enable Low_Level interrupts
     sei();
     while (1);
}

/*
+------------------------------------------------------------------------------
| Function    : 中断函数
+------------------------------------------------------------------------------
*/ ISR(ACA_AC0_vect)
{
  LED2_T();
}
ISR(ACA_AC1_vect)
{
  LED1_T();
}
