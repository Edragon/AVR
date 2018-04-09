/*
 * 工程名:
     EEPROM_ON_CHIP
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-21 14:15
 * 作者:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
 * 描述:
     	本程序对片内EEPROM使用不同方式进行多种操作。
		USARTC0打印调试信息。
*/

#include "avr_compiler.h"
#include "usart_driver.c"
#include "eeprom_driver.c"

#define TEST_BYTE_1 0x55
#define TEST_BYTE_2 0xAA

#define TEST_BYTE_ADDR_1 0x00
#define TEST_BYTE_ADDR_2 0x08

#define TEST_PAGE_ADDR_1    0  //页地址总是在页的边界
#define TEST_PAGE_ADDR_2    2  //页地址总是在页的边界
#define TEST_PAGE_ADDR_3    5  //页地址总是在页的边界

//写入EEPROM的缓存数据
uint8_t testBuffer[EEPROM_PAGESIZE] = {"Accessing Atmel AVR XMEGA EEPROM"};

 /*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
void uart_init(void)
{
	/* USARTC0 引脚方向设置*/
  	/* PC3 (TXD0) 输出 */
	PORTC.DIRSET   = PIN3_bm;
	/* PC2 (RXD0) 输入 */
	PORTC.DIRCLR   = PIN2_bm;
	/* USARTC0 模式 - 异步*/
	USART_SetMode(&USARTC0,USART_CMODE_ASYNCHRONOUS_gc);
	/* USARTC0帧结构, 8 位数据位, 无校验, 1停止位 */
	USART_Format_Set(&USARTC0, USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc, false);
	/* 设置波特率 9600*/
	USART_Baudrate_Set(&USARTC0, 12 , 0);
	/* USARTC0 使能发送*/
	USART_Tx_Enable(&USARTC0);
	/* USARTC0 使能接收*/
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

	//测试结果
	bool test_ok = true;
	uint8_t temp = 0;

	//清空缓存
	EEPROM_FlushBuffer();

	//1.使用IO寄存器方式写入和读取2个字节
	 
	//关闭EEPROM映射到内存空间
	EEPROM_DisableMapping();

	//写单个字节
	EEPROM_WriteByte(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_1, TEST_BYTE_1);
	EEPROM_WriteByte(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_2, TEST_BYTE_2);

	//读取写入的字节
	temp = EEPROM_ReadByte(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_1);
	if ( temp != TEST_BYTE_1) {
		test_ok = false;
	}
	uart_puts("1 Read TEST_BYTE_1 = ");
	uart_putc_hex(temp);
	uart_putc('\n');


	temp = EEPROM_ReadByte(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_2);
	if ( temp != TEST_BYTE_2) {
		test_ok = false;
	}
	uart_puts("1 Read TEST_BYTE_2 = ");
	uart_putc_hex(temp);
	uart_putc('\n');


	//2.分离操作写一整页

	//加载页缓存，先擦页后写页
	EEPROM_LoadPage(testBuffer);
	EEPROM_ErasePage(TEST_PAGE_ADDR_2);
	EEPROM_SplitWritePage(TEST_PAGE_ADDR_2);

	//读取
	uart_puts("2 Read testBuffer = ");
	uart_putc('\n');
	for (uint8_t i = 0; i < EEPROM_PAGESIZE; ++i)
	{	
		temp = EEPROM_ReadByte(TEST_PAGE_ADDR_2, i );
		if ( temp != testBuffer[i] )
		{
			test_ok = false;
			break;
		}
		uart_putc(temp);
	}
	uart_putc('\n');




	//3.使用内存映射写入并读取两个字节

	EEPROM_EnableMapping();

	//写2个字节
	EEPROM_WaitForNVM();
	EEPROM(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_1) = TEST_BYTE_1;
	//EEPROM_AtomicWritePage(TEST_PAGE_ADDR_1);//这里这一句应当用不着
	EEPROM_WaitForNVM();
	EEPROM(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_2) = TEST_BYTE_2;
	//EEPROM_AtomicWritePage(TEST_PAGE_ADDR_1);//这里这一句应当用不着

	//读取2个字节
	EEPROM_WaitForNVM();
	temp = EEPROM(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_1);
	if ( temp != TEST_BYTE_1)
	{
		test_ok = false;
	}
	uart_puts("3 Read TEST_BYTE_1 = ");
	uart_putc_hex(temp);
	uart_putc('\n');

	temp = EEPROM(TEST_PAGE_ADDR_1, TEST_BYTE_ADDR_2);
	if ( temp != TEST_BYTE_2)
	{
		test_ok = false;
	}
	uart_puts("3 Read TEST_BYTE_2 = ");
	uart_putc_hex(temp);
	uart_putc('\n');


	//4.使用内存映射方式，分离操作，写一页缓存到EEPROM
	EEPROM_EnableMapping();
	//加载缓存
	EEPROM_WaitForNVM();
	for (uint8_t i = 0; i < EEPROM_PAGESIZE; ++i)
	{
		EEPROM(TEST_PAGE_ADDR_3, i) = testBuffer[i];
	}

	//擦除EEPROM页，页缓存并没有被擦除
	EEPROM_ErasePage(TEST_PAGE_ADDR_3);

	//分离写操作，缓存在操作完成后清空
	EEPROM_SplitWritePage(TEST_PAGE_ADDR_3);

	//读取，串口显示
	EEPROM_WaitForNVM();
	uart_puts("4 Read testBuffer = ");
	uart_putc('\n');
	for (uint8_t i = 0; i < EEPROM_PAGESIZE; ++i)
	{
		temp = EEPROM(TEST_PAGE_ADDR_3, i);
		if ( temp!= testBuffer[i] )
		{
			test_ok = false;
			break;
			uart_puts("test_ok = false");
			uart_putc('\n');
		}
		uart_putc(temp);
	}
	uart_putc('\n');

	while(1)
		{
			nop();
		}
}
