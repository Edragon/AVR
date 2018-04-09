/*
 * ������:
     bootloader
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
 * ����: �Ӵ��ڷ������ݣ�д���ַΪ0x2800����Ϊflash�ǰ���Ϊ��λ�ģ���Ӧ�ó�������Ȼ���ٶ��������͵�����
����ʱҪѡ��project->configuration options->memory set����add nameΪ.text ��ַΪ0x10000flash �����ַ��
boot����ʼ��ַ     	 
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
| Description : ��ʼ�� USARTC0
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
| Function    :write_one_page
+------------------------------------------------------------------------------
| Description : ��ʼ�� USARTC0
+------------------------------------------------------------------------------
*/
 //����һ��Flashҳ
void write_one_page(unsigned char *buf)
{
    FlashAddr=0x5000;
    //��������Flash����ҳ
	SP_LoadFlashPage(buf);
	//������ҳ����д��һ��Flashҳ
	SP_EraseWriteApplicationPage(FlashAddr);
	/* Wait for NVM to finish. */
	SP_WaitForSPM();//�ȴ�ҳ������                      
}

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
| Description : ��ʼ�� USARTC0
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
	/*ʹ���ж�*/
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
| Description : USARTC0�����жϺ��� �յ������ݷ��ͻ�ȥUSARTC0
+------------------------------------------------------------------------------
*/
ISR(USARTC0_RXC_vect) 
{    
       
       uint8_t count_num=0;
	   bootapp=false;
	   TCC0_CNT=0;
	   receivedata[count_num]= USART_GetChar(&USARTC0);//��Data Register
      

	   //���ռ������20�����ս���
       while(TCC0_CNT<=20)
	  {
		if((USARTC0.STATUS&0x80)==0x80)
	  	{
		count_num++;
        receivedata[count_num]= USART_GetChar(&USARTC0);//��Data Register
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


