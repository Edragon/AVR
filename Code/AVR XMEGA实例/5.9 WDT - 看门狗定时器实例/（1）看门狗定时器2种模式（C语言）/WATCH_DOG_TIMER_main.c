/*
 * ������:
     WATCH_DOG_TIMER
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-23 14:03
 * ����:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
 * ����:
     	void wdt_fuse_enable_example( void );
		��˿�趨���Ź���ʱ���ڣ��������ڣ�����������ģʽ

		void wdt_sw_enable_example( void );
		���������Ź���ͨģʽ������ģʽ

		USARTC0��ӡ������Ϣ��
*/
#include "avr_compiler.h"
#include "usart_driver.c"
#include "wdt_driver.c"

//����CPUʱ��
#define F_CPU (2000000UL)

void wdt_fuse_enable_example( void );
void wdt_sw_enable_example( void );

//���Ź���ʱ���ں�����
#define TO_WD     128
//���Ź��ر����ں�����
#define TO_WDW    64

//����ģʽ�¿��Ź����ü��ʱ��
#define WINDOW_MODE_DELAY   ( (TO_WDW) +  (TO_WD / 2) )

//��ͨģʽ�¿��Ź����ü��ʱ��
#define NORMAL_MODE_DELAY   ( TO_WD / 2 )
 /*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : ��ʼ�� USARTC0
+------------------------------------------------------------------------------
*/
void uart_init(void)
{
	/* USARTC0 ���ŷ�������*/
  	/* PC3 (TXD0) ��� */
	PORTC.DIRSET   = PIN3_bm;
	/* PC2 (RXD0) ���� */
	PORTC.DIRCLR   = PIN2_bm;
	/* USARTC0 ģʽ - �첽*/
	USART_SetMode(&USARTC0,USART_CMODE_ASYNCHRONOUS_gc);
	/* USARTC0֡�ṹ, 8 λ����λ, ��У��, 1ֹͣλ */
	USART_Format_Set(&USARTC0, USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc, false);
	/* ���ò����� 9600*/
	USART_Baudrate_Set(&USARTC0, 12 , 0);
	/* USARTC0 ʹ�ܷ���*/
	USART_Tx_Enable(&USARTC0);
	/* USARTC0 ʹ�ܽ���*/
	USART_Rx_Enable(&USARTC0);
}

 /*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main( void )
{
	uart_init(); 
	uart_puts("inside main");uart_putc('\n');
	//wdt_sw_enable_example();
	wdt_fuse_enable_example();

}

/*
+------------------------------------------------------------------------------
| Function    : wdt_sw_enable_example
+------------------------------------------------------------------------------
| Description : �����У���������ģʽ��WD��ʹ�ܣ�ע�⣺WDLOCK��˿λ������λ
+------------------------------------------------------------------------------
*/
void wdt_sw_enable_example( void )
{
	uart_puts("inside void wdt_sw_enable_example( void )");uart_putc('\n');

	//ʹ�ܿ��Ź���ͨģʽ����ʱ����Ϊ32CLK=32 ms
	WDT_EnableAndSetTimeout( WDT_PER_32CLK_gc );
	
	if(WDT.CTRL & WDT_ENABLE_bm)
	{
		uart_puts("WDT Is  Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not Enabled");uart_putc('\n');
	}
	//�����Ҫ�������ÿ��Ź���Ҫ�ȹرտ��Ź�
	WDT_Disable();

	if(WDT.CTRL & WDT_ENABLE_bm)
	{
		uart_puts("WDT Is  Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not Enabled");uart_putc('\n');
	}

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}


	//�����ô���ģʽǰ��Ҫ�����µĿ��Ź���ͨģʽ���ã�
	//��Ϊ����ģʽֻ������ͨģʽʹ��ʱ���ܿ���
	WDT_EnableAndSetTimeout( WDT_PER_128CLK_gc );

	//���ô���ģʽ
	WDT_EnableWindowModeAndSetTimeout( WDT_WPER_64CLK_gc );

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}

	while(true)
	{
		uint16_t repeatCounter;
		//ȷ����Ҫ�������ÿ��Ź�������ᵼ��ϵͳ��λ��������ʱҪ���ڿ��ŵĴ�������
		for (repeatCounter = WINDOW_MODE_DELAY; repeatCounter > 0; --repeatCounter )
		{
			delay_us( 1000 ); // 1ms �ӳ� @ 2MHz
		}
		WDT_Reset();//���ÿ��Ź�
	}
}

/*
+------------------------------------------------------------------------------
| Function    : wdt_fuse_enable_example
+------------------------------------------------------------------------------
| Description : ���Ź�ͨ����˿λ�趨
| 			ע��  ��������ҪWDLOCK��˿λ��λ�����Ź�����Ϊ128CLK����������Ϊ64CLK
+------------------------------------------------------------------------------
*/
void wdt_fuse_enable_example( void )
{

	uart_puts("inside void wdt_fuse_enable_example( void )");uart_putc('\n');
	//�����Ź�ͨ����˿λ�趨����Ҫ���ÿ��Ź�
	WDT_Reset();

	if(WDT.CTRL & WDT_ENABLE_bm)
	{
		uart_puts("WDT Is  Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not Enabled");uart_putc('\n');
	}

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}


	if (true == WDT_IsWindowModeEnabled()){
		while(1){
			//�������ģʽʹ�ܣ�ϵͳ����λ
		}
	}

	//�������ģʽδͨ����˿ʹ�ܣ���Ҫ�ڳ�����ʹ�ܣ����������������趨����Ϊ��˿λ�Ѿ���λ
	WDT_EnableWindowMode();

	// ���Ź����������(TO_WDW * 1.3) ��((TO_WDW + TO_WD)*0.7) ֮��

	delay_us( 90000 );
	WDT_Reset();

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}


	WDT_DisableWindowMode();

	if(WDT_IsWindowModeEnabled())
	{
		uart_puts("WDT Is WindowMode Enabled");uart_putc('\n');
	}
	else
	{
		uart_puts("WDT Is not WindowMode Enabled");uart_putc('\n');
	}


	while(true)
	{
		uint16_t repeatCounter;
		for (repeatCounter = NORMAL_MODE_DELAY; repeatCounter > 0; --repeatCounter ) {
			delay_us( 1000 ); 
		}
		WDT_Reset();
	}
}
