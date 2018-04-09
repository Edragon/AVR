/*
 * 工程名:
     LED_Blinking
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-09 12:50
 * 作者:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
 * 描述:
     	本程序使端口上的LED发光二极管闪烁.
     	时钟:内部时钟2MHz（上电初始时钟）
		硬件连接：
				LED1  -->   PD5
				LED2  -->   PD4
*/
#include <avr/io.h>
#include <util/delay.h>

#define LED1_ON()  PORTD_OUTCLR = 0x20
#define LED1_OFF() PORTD_OUTSET = 0x20
#define LED1_T()   PORTD_OUTTGL = 0x20

#define LED2_ON()  PORTD_OUTCLR = 0x10
#define LED2_OFF() PORTD_OUTSET = 0x10
#define LED2_T()   PORTD_OUTTGL = 0x10

int main()
{
	PORTD_DIR = 0x30;//PD5，PD4方向设为输出

	while(1)
	{
		LED1_ON();
		LED2_ON();
		_delay_ms(500);
		LED1_OFF();
		LED2_OFF();
		_delay_ms(500);
	}
 
}
