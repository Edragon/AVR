/************************************************************************************** 
 * 工程名:
    SPI_fm25v0_asm.asm
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-11 1:17
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
		 
*/
 /*
;* File Name         : SPI_fm25v0_asm.asm
;* Title             : SPI读写FM25V10
;* Date              : 2010-7-12  21:36
;* Support E-mail    : jack198651@163.com
;* Target MCU        : ATxmega32A4
;*
;* DESCRIPTION       
;* 	   本程序使用SPID向FM25V10读写数据
;*
;*   |---XMEGA32A4---|---------FM25V10---------------|
;*   |---PD4---SS----|---1---/S---Chip Select--------|
;*   |---PD6---MISO--|---2----Q---Serial Data Output-|
;*   |---VCC---------|---3---/W---Write Protect------|
;*   |---GND---------|---4----VSS---------Ground-----|
;*   |---PD5---MOSI--|---5----D---Serial Data Input--|
;*   |---PD7---SCK---|---6----C---Serial Clock-------|
;*   |---VCC---------|---7---/HOLD--------HOLD-------|
;*   |---VCC---------|---8----VDD---Supply Voltage---|
;*   系统时钟选用默认时钟2MHZ
;* WREN  Set Write Enable Latch  0000 0110b 
;* WRDI  Write Disable  0000 0100b 
;* RDSR  Read Status Register  0000 0101b 
;* WRSR  Write Status Register  0000 0001b 
;* READ  Read Memory Data  0000 0011b 
;* FSTRD  Fast Read Memory Data  0000 1011b 
;* WRITE  Write Memory Data   0000 0010b 
;* SLEEP  Enter Sleep Mode  1011 1001b 
;* RDID  Read Device ID  1001 1111b 
;* SNR  Read S/N  1100 0011b
;发送写数据格式 ：F9(写标志)00 00 00（三字节地址） 02（要写数据个数） 01 02（要写的数据）
;发送读数据格式： FA(读标志)00 00 00 （要读的起始地址）02（要读的数据个数）
;**************************************************************************************/

#include <avr/io.h>
#include "avr_compiler.h"
#include "clksys_driver.c"
#include "usart_driver.c"
#include "TC_driver.c"

uint8_t receivedata[100];
uint8_t writedata[100];
 

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
	unsigned char factor =3;
	CLKSYS_XOSC_Config( OSC_FRQRANGE_2TO9_gc, false,OSC_XOSCSEL_XTAL_16KCLK_gc );//设置晶振范围 启动时间
	CLKSYS_Enable( OSC_XOSCEN_bm  );//使能外部振荡器
	do {} while ( CLKSYS_IsReady( OSC_XOSCRDY_bm  ) == 0 );//等待外部振荡器准备好
	CLKSYS_PLL_Config( OSC_PLLSRC_XOSC_gc, factor );//设置倍频因子并选择外部振荡器为PLL参考时钟
	CLKSYS_Enable( OSC_PLLEN_bm );//使能PLL电路
	do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );//等待PLL准备好
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc);//选择系统时钟源
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );//设置预分频器A,B,C的值	
}

/*
+------------------------------------------------------------------------------
| Function    : SPI_MasterInit
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
void SPI_MasterInit(void)
 {    
 
	    PORTCFG.VPCTRLA=0x10;//;PORTB映射到虚拟端口1，PORTA映射到虚拟端口0
	    
	    PORTCFG.VPCTRLB=0x32;//;PORTC映射到虚拟端口2，PORTD映射到虚拟端口3
        //SPI初始化
		VPORT3_DIR=0x10; //SS片选引脚设为方向输出
		VPORT3_DIR|=0x20;//MOSI引脚设为方向输出
		VPORT3_DIR|=0x80;//SCK引脚设为方向输出
        VPORT3_DIR|=0x01;//485控制引脚
        //0,1,0,1,00,00;SPI Clock Double DISABLE,SPI module Enable, Data Order=MSB，Master Select, SPI Mode=0, SPI Clock Prescaler=CLKper/4=0.5MHZ
        SPID_CTRL=0x50;//SPI控制寄存器
        SPID_INTCTRL=0x00;//SPI 中断 关闭
	 
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
	USART_Baudrate_Set(&USARTC0, 77 , 0);
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
void main(void)
{	   
       
        
        PLL_XOSC_Initial(); 
		SPI_MasterInit();
	    uart_init();

		//指示灯
		VPORT0_DIR=0x04;//PORTA2输出
		VPORT0_DIR|=0x08;//PORTA3输出
		VPORT0_DIR|=0x10;//PORTA4输出
		 
	   //计数器时钟源为24MHZ/1024=23437.5Hz
	    /* Set period/TOP value. */
	    TC_SetPeriod( &TCC0, 0x1000 );
        /* Select clock source. */
	    TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1024_gc);
	 
		USARTC0_CTRLA=0X38;
        PMIC.CTRL |=PMIC_MEDLVLEN_bm+PMIC_LOLVLEN_bm+PMIC_HILVLEN_bm; //Enable Low_Level interrupts
    	sei();
		VPORT3_OUT&=0xfe;//PORTD0出低 使能485接收
        while(1)
		VPORT3_OUT&=0xFE;//PORTD0输出低 使能485接收
	 
	    
                
}
 
/*
+------------------------------------------------------------------------------
| Function    : 读数据流程
+------------------------------------------------------------------------------
*/
void SPI_READ_DATA(void) //读数据流程
{       
        VPORT0_OUT=0x08;//接收过程亮灯
		//发送操作码之前片选产生下降沿
	    VPORT3_OUT|=0x10;
		for(uint8_t i=0;i<=5;i++);
		VPORT3_OUT&=0xef;
		//发送读操作码
	    //READ 0X03
        SPID_DATA=0x03;
		while((SPID_STATUS&0x80)!=0x80);
		//数据移位完成，写3个字节的地址
	    SPI_WRITE_ADDRESS();
		for(uint8_t i=0;i<receivedata[4];i++)
         {
		  SPID_DATA=0x00;
		 //读写数据都要等待SPI移位完成标志位置位
         while((SPID_STATUS&0x80)!=0x80);
		 writedata[i]=SPID_DATA; 
		 }
         VPORT3_OUT|=0x01;//PORTD0输出高 使能485发送
		 for(uint8_t j=0;j<receivedata[4];j++)
		
		  {
	 	   while(!(USARTC0.STATUS & USART_DREIF_bm));
		 
	        USART_PutChar(&USARTC0,writedata[j]);

            }
		 while(!(USARTC0.STATUS & USART_TXCIF_bm));
		 VPORT3_OUT|=0x10;//读操作完成，片选SS拉高
	 	 VPORT0_OUT&=0xF7;//灯灭
}
/*
+------------------------------------------------------------------------------
| Function    : 写数据流程
+------------------------------------------------------------------------------
*/
 
void SPI_WRITE_DATA(void)//写数据流程
{
		//发送操作码之前片选产生下降沿
		VPORT0_OUT=0x04;//接收过程亮灯
		VPORT3_OUT|=0x10;
		for(uint8_t i=0;i<=5;i++);
		VPORT3_OUT&=0xef;
     	//发送写使能操作码//WREN 0X06
		 
        SPID_DATA=0X06;
	    while((SPID_STATUS&0x80)!=0x80);
		VPORT3_OUT|=0x10;//发送写使能操作码 完成，片选SS拉高

		//发送操作码之前片选产生下降沿
		VPORT3_OUT|=0x10;
		for(uint8_t i=0;i<=5;i++);
		VPORT3_OUT&=0xef;
		//发送写操作码
	  //WRITE 0X02
        SPID_DATA=0X02;
		while((SPID_STATUS&0x80)!=0x80);
		//数据移位完成，写3个字节的地址
	    SPI_WRITE_ADDRESS();

	    for(uint8_t i=0;i<receivedata[4];i++)
         {
		  
		 SPID_DATA=receivedata[i+5];
		 while((SPID_STATUS&0x80)!=0x80);
		//数据移位完成，计数减1，写下一个字节的数据
		  }
		 VPORT3_OUT|=0x10;//写操作完成，片选SS拉高
		 VPORT0_OUT&=0xFB;//灯灭
}

/*
+------------------------------------------------------------------------------
| Function    : 读写数据都要写三个字节的地址 
+------------------------------------------------------------------------------
*/
void SPI_WRITE_ADDRESS(void)
{
        for(uint8_t i=0;i<3;i++)
		{
		SPID_DATA=receivedata[i+1];
		while((SPID_STATUS&0x80)!=0x80);
		
		}
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
      
       int count_num=0;
	   VPORT0_OUT|=0x10;//接收过程亮灯
	   receivedata[count_num]=USARTC0_DATA;//读Data Register

	   TCC0_CNT=0;

	   //接收间隔大于20，接收结束
       while(TCC0_CNT<=20)
	  {
		if((USARTC0.STATUS&0x80)==0x80)
	  	{
		count_num++;
        receivedata[count_num]=USARTC0_DATA;//读Data Register
	    TCC0_CNT=0;
		 
		} 

		}
        //VPORT3_OUT|=0x01;//PORTD0输出高 使能485发送
	 
	    //while(!(USARTC0.STATUS & USART_DREIF_bm));
		 
	    //USART_PutChar(&USARTC0,receivedata[2]);

		//while(!(USARTC0.STATUS & USART_TXCIF_bm));
        //VPORT3_OUT&=0xFE;//PORTD0输出低 使能485接收

		VPORT0_OUT&=0xef;//接收结束灯灭

	    if(receivedata[0]==0xf9)//自定义SPI写命令
        SPI_WRITE_DATA();
        if(receivedata[0]==0xfa)//自定义SPI读命令 
		SPI_READ_DATA();
	 
		
	     
} 

ISR(USARTC0_TXC_vect) 
{

}
