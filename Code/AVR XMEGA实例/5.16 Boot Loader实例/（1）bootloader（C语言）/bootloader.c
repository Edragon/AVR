/*
 * 工程名:
     bootloader
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-23 14:03
 * 作者:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
 * 描述: 从串口发送数据，写入地址为0x2800（因为flash是按字为单位的）的应用程序区，然后再读出来会送到串口
下载时要选择project->configuration options->memory set里面add name为.text 地址为0x10000flash 这个地址是
boot的起始地址     	 
*/
#include "avr_compiler.h"
#include "usart_driver.c"
#include "TC_driver.c"
#include "sp_driver.h"
#define PROG_START  PROGMEM_START
uint16_t receivedata[255];
uint16_t readdata[255];
bool  volatile bootapp;
unsigned long int FlashAddr;
/*
+------------------------------------------------------------------------------
| Function    : quit
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/

void quit (void)
{

  CPU_CCP=CCP_IOREG_gc;
  PMIC.CTRL = 0X00;
  EIND=0X00;
  (*((void(*)(void))PROG_START))();
}
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
| Function    :write_one_page
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
 //更新一个Flash页
void write_one_page(unsigned char *buf)
{
    FlashAddr=0x5000;
    //数据填入Flash缓冲页
	SP_LoadFlashPage(buf);
	//将缓冲页数据写入一个Flash页
	SP_EraseWriteApplicationPage(FlashAddr);
	/* Wait for NVM to finish. */
	SP_WaitForSPM();//等待页编程完成                      
}

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
void main( void )
{   
    uart_init();
    uart_puts((void *)"enter into main");
	uart_putc('\n');
	bootapp=true;
    TC_SetPeriod( &TCC0, 0x1000 );
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1024_gc );
    USART_RxdInterruptLevel_Set(&USARTC0,USART_RXCINTLVL_LO_gc);
	/*使能中断*/
	CPU_CCP=CCP_IOREG_gc;
	PMIC.CTRL = PMIC_LOLVLEN_bm|PMIC_IVSEL_bm; //Enable Low_Level interrupts
	sei();	
	while(bootapp);
	cli();
	quit();
	 
}
/*
+------------------------------------------------------------------------------
| Function    : ISR(USARTE1_RXC_vect)
+------------------------------------------------------------------------------
| Description : USARTC0接收中断函数 收到的数据发送回去USARTC0
+------------------------------------------------------------------------------
*/
ISR(USARTC0_RXC_vect) 
{    
       
       uint8_t count_num=0;
	   bootapp=false;
	   TCC0_CNT=0;
	   receivedata[count_num]= USART_GetChar(&USARTC0);//读Data Register
      

	   //接收间隔大于20，接收结束
       while(TCC0_CNT<=20)
	  {
		if((USARTC0.STATUS&0x80)==0x80)
	  	{
		count_num++;
        receivedata[count_num]= USART_GetChar(&USARTC0);//读Data Register
	    TCC0_CNT=0;
		 
		} 

	  }		  
	  write_one_page(receivedata); 
	  FlashAddr=0x5000;
	  for(uint8_t i=0;i<=count_num;i++)
	  {
	  readdata[i]=SP_ReadWord(FlashAddr);
      FlashAddr++;
	  FlashAddr++;
	  }
	  
      for(uint8_t i=0;i<=count_num;i++)
	  {
        while(!USART_IsTXDataRegisterEmpty(&USARTC0));
        USART_PutChar(&USARTC0,readdata[i]);
      } 	  
}


