/*
 * ������:
     LED_KEY
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
     	������������ά��������������LED�������������ð�����Ӧ�Ķ˿�Ϊ���룬
		��ʹ���������裻����LED��Ӧ�Ķ˿�Ϊ�������ʼ����LEDΪ����
		�������ж��û��������룬�����
		���м��ȷ����   --  LED1��LED2�˿�ȡ����LED1��LED2���濪�� ��
		�����           --  LED1�� ��
		���Ҽ�           --  LED1�� ��
		���ϼ�           --  LED2�� ��
		���¼�           --  LED2�� ��

     	ʱ��:�ڲ�ʱ��2MHz���ϵ��ʼʱ�ӣ�
		Ӳ�����ӣ�
				LED1  -->   PD5
				LED2  -->   PD4
				LEFT_KEY    --> PE0
				SELECT_KEY  --> PE1
				UP_KEY      --> PE2
				DOWN_KEY    --> PE3
				RIGHT_KEY   --> PE4
*/
#include <avr/io.h>

//---------LED����--------
#define LED1_ON()  PORTD_OUTCLR = 0x20
#define LED1_OFF() PORTD_OUTSET = 0x20
#define LED1_T()   PORTD_OUTTGL = 0x20

#define LED2_ON()  PORTD_OUTCLR = 0x10
#define LED2_OFF() PORTD_OUTSET = 0x10
#define LED2_T()   PORTD_OUTTGL = 0x10


//--------��������ֵ-------
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
| Description : ���ð�������Ϊ����
+------------------------------------------------------------------------------
*/
void KEY_initial(void)
{
	PORTE_DIRCLR = 0x1F;//���ð�������Ϊ����
	/*
	PORTE_PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN1CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN2CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN3CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN4CTRL = PORT_OPC_PULLUP_gc;
	*/
	//���ж�����ŵ�������ͬʱ������ʹ�ö�������������Ĵ���һ�����ö������
	PORTCFG_MPCMASK = 0X1F;
	PORTE_PIN0CTRL = PORT_OPC_PULLUP_gc;
}

/*
+------------------------------------------------------------------------------
| Function    : Get_Key
+------------------------------------------------------------------------------
| Description : ȷ����ǰ���µļ�
| Parameters  : 
| Returns     : ���ؼ�ֵ�����������������൱��û�м����£�����0��
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
| Description : ���ݰ�����LED�Ʋ���
+------------------------------------------------------------------------------
*/
int main(void)
{	
	unsigned char Key_return = 0;
	PORTD_DIRSET = 0x30;//PD5��PD4������Ϊ���
	LED1_ON(); 
	LED2_ON(); 
	KEY_initial();//��ʼ����������
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






