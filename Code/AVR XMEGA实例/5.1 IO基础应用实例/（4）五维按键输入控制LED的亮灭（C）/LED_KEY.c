/*
 * 工程名:
     LED_KEY
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
     	本程序利用五维按键输入来控制LED的亮灭，首先设置按键对应的端口为输入，
		并使能上拉电阻；配置LED对应的端口为输出，初始两个LED为开；
		主程序判断用户按键输入，如果：
		按中间的确定键   --  LED1、LED2端口取反，LED1、LED2交替开关 ；
		按左键           --  LED1开 ；
		按右键           --  LED1关 ；
		按上键           --  LED2开 ；
		按下键           --  LED2关 ；

     	时钟:内部时钟2MHz（上电初始时钟）
		硬件连接：
				LED1  -->   PD5
				LED2  -->   PD4
				LEFT_KEY    --> PE0
				SELECT_KEY  --> PE1
				UP_KEY      --> PE2
				DOWN_KEY    --> PE3
				RIGHT_KEY   --> PE4
*/
#include <avr/io.h>

//---------LED操作--------
#define LED1_ON()  PORTD_OUTCLR = 0x20
#define LED1_OFF() PORTD_OUTSET = 0x20
#define LED1_T()   PORTD_OUTTGL = 0x20

#define LED2_ON()  PORTD_OUTCLR = 0x10
#define LED2_OFF() PORTD_OUTSET = 0x10
#define LED2_T()   PORTD_OUTTGL = 0x10


//--------按键返回值-------
#define No_key   0x00
#define SELECT   0x01
#define LEFT     0x02
#define RIGHT    0x04
#define UP       0x08
#define DOWN     0x10

/*
+------------------------------------------------------------------------------
| Function    : KEY_initial
+------------------------------------------------------------------------------
| Description : 设置按键引脚为上拉
+------------------------------------------------------------------------------
*/
void KEY_initial(void)
{
	PORTE_DIRCLR = 0x1F;//设置按键引脚为输入
	/*
	PORTE_PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN1CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN2CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN3CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN4CTRL = PORT_OPC_PULLUP_gc;
	*/
	//当有多个引脚的配置相同时，可以使用多引脚配置掩码寄存器一次配置多个引脚
	PORTCFG_MPCMASK = 0X1F;
	PORTE_PIN0CTRL = PORT_OPC_PULLUP_gc;
}

/*
+------------------------------------------------------------------------------
| Function    : Get_Key
+------------------------------------------------------------------------------
| Description : 确定当前按下的键
| Parameters  : 
| Returns     : 返回键值（如果多个按键按下相当于没有键按下，返回0）
+------------------------------------------------------------------------------
*/
unsigned char Get_Key(void)
{
	unsigned char Key=0,num_keypress = 0;
	if((PORTE_IN&(1<<1))==0)
		{
			Key|=SELECT;
			num_keypress++;
			}
	if((PORTE_IN&(1<<0))==0)
		{
			Key|=LEFT;
			num_keypress++;
			}
	if((PORTE_IN&(1<<4))==0)
		{
			Key|=RIGHT;
			num_keypress++;
			}
	if((PORTE_IN&(1<<2))==0)
		{
			Key|=UP;
			num_keypress++;
			}
	if((PORTE_IN&(1<<3))==0)
		{
			Key|=DOWN;
			num_keypress++;
			}
	if(num_keypress>1)
		Key=No_key;
	return Key;
}

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
| Description : 根据按键对LED灯操作
+------------------------------------------------------------------------------
*/
int main(void)
{	
	unsigned char Key_return = 0;
	PORTD_DIRSET = 0x30;//PD5，PD4方向设为输出
	LED1_ON(); 
	LED2_ON(); 
	KEY_initial();//初始化按键引脚
	while(1)
	{
		Key_return = Get_Key(); 
		if(Key_return)
		{
			switch(Key_return)
			{
				case SELECT: 
					LED1_T();
					LED2_T();
					break;  
				case LEFT  : 
					LED1_ON();
					break;
				case RIGHT : 
					LED1_OFF();
					break;
				case UP    : 
					LED2_ON();
					break;
				case DOWN  : 
					LED2_OFF();
					break;
				default :break;
			}
			Key_return=0;
		}
	}
	return 0;
}






