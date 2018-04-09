/************************************************************************************** 
 * ������:
    SPI_fm25v0_asm.asm
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-11 1:17
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
		 
*/
 /*
;* File Name         : SPI_fm25v0_asm.asm
;* Title             : SPI��дFM25V10
;* Date              : 2010-7-12  21:36
;* Support E-mail    : jack198651@163.com
;* Target MCU        : ATxmega32A4
;*
;* DESCRIPTION       
;* 	   ������ʹ��SPID��FM25V10��д����
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
;*   ϵͳʱ��ѡ��Ĭ��ʱ��2MHZ
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
;����д���ݸ�ʽ ��F9(д��־)00 00 00�����ֽڵ�ַ�� 02��Ҫд���ݸ����� 01 02��Ҫд�����ݣ�
;���Ͷ����ݸ�ʽ�� FA(����־)00 00 00 ��Ҫ������ʼ��ַ��02��Ҫ�������ݸ�����
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
	unsigned char factor =3;
	CLKSYS_XOSC_Config( OSC_FRQRANGE_2TO9_gc, false,OSC_XOSCSEL_XTAL_16KCLK_gc );//���þ���Χ ����ʱ��
	CLKSYS_Enable( OSC_XOSCEN_bm  );//ʹ���ⲿ����
	do {} while ( CLKSYS_IsReady( OSC_XOSCRDY_bm  ) == 0 );//�ȴ��ⲿ����׼����
	CLKSYS_PLL_Config( OSC_PLLSRC_XOSC_gc, factor );//���ñ�Ƶ���Ӳ�ѡ���ⲿ����ΪPLL�ο�ʱ��
	CLKSYS_Enable( OSC_PLLEN_bm );//ʹ��PLL��·
	do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );//�ȴ�PLL׼����
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc);//ѡ��ϵͳʱ��Դ
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );//����Ԥ��Ƶ��A,B,C��ֵ	
}

/*
+------------------------------------------------------------------------------
| Function    : SPI_MasterInit
+------------------------------------------------------------------------------
| Description : ��ʼ�� USARTC0
+------------------------------------------------------------------------------
*/
void SPI_MasterInit(void)
 {    
 
	    PORTCFG.VPCTRLA=0x10;//;PORTBӳ�䵽����˿�1��PORTAӳ�䵽����˿�0
	    
	    PORTCFG.VPCTRLB=0x32;//;PORTCӳ�䵽����˿�2��PORTDӳ�䵽����˿�3
        //SPI��ʼ��
		VPORT3_DIR=0x10; //SSƬѡ������Ϊ�������
		VPORT3_DIR|=0x20;//MOSI������Ϊ�������
		VPORT3_DIR|=0x80;//SCK������Ϊ�������
        VPORT3_DIR|=0x01;//485��������
        //0,1,0,1,00,00;SPI Clock Double DISABLE,SPI module Enable, Data Order=MSB��Master Select, SPI Mode=0, SPI Clock Prescaler=CLKper/4=0.5MHZ
        SPID_CTRL=0x50;//SPI���ƼĴ���
        SPID_INTCTRL=0x00;//SPI �ж� �ر�
	 
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
	USART_Baudrate_Set(&USARTC0, 77 , 0);
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
void main(void)
{	   
       
        
        PLL_XOSC_Initial(); 
		SPI_MasterInit();
	    uart_init();

		//ָʾ��
		VPORT0_DIR=0x04;//PORTA2���
		VPORT0_DIR|=0x08;//PORTA3���
		VPORT0_DIR|=0x10;//PORTA4���
		 
	   //������ʱ��ԴΪ24MHZ/1024=23437.5Hz
	    /* Set period/TOP value. */
	    TC_SetPeriod( &TCC0, 0x1000 );
        /* Select clock source. */
	    TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV1024_gc);
	 
		USARTC0_CTRLA=0X38;
        PMIC.CTRL |=PMIC_MEDLVLEN_bm+PMIC_LOLVLEN_bm+PMIC_HILVLEN_bm; //Enable Low_Level interrupts
    	sei();
		VPORT3_OUT&=0xfe;//PORTD0���� ʹ��485����
        while(1)
		VPORT3_OUT&=0xFE;//PORTD0����� ʹ��485����
	 
	    
                
}
 
/*
+------------------------------------------------------------------------------
| Function    : ����������
+------------------------------------------------------------------------------
*/
void SPI_READ_DATA(void) //����������
{       
        VPORT0_OUT=0x08;//���չ�������
		//���Ͳ�����֮ǰƬѡ�����½���
	    VPORT3_OUT|=0x10;
		for(uint8_t i=0;i<=5;i++);
		VPORT3_OUT&=0xef;
		//���Ͷ�������
	    //READ 0X03
        SPID_DATA=0x03;
		while((SPID_STATUS&0x80)!=0x80);
		//������λ��ɣ�д3���ֽڵĵ�ַ
	    SPI_WRITE_ADDRESS();
		for(uint8_t i=0;i<receivedata[4];i++)
         {
		  SPID_DATA=0x00;
		 //��д���ݶ�Ҫ�ȴ�SPI��λ��ɱ�־λ��λ
         while((SPID_STATUS&0x80)!=0x80);
		 writedata[i]=SPID_DATA; 
		 }
         VPORT3_OUT|=0x01;//PORTD0����� ʹ��485����
		 for(uint8_t j=0;j<receivedata[4];j++)
		
		  {
	 	   while(!(USARTC0.STATUS & USART_DREIF_bm));
		 
	        USART_PutChar(&USARTC0,writedata[j]);

            }
		 while(!(USARTC0.STATUS & USART_TXCIF_bm));
		 VPORT3_OUT|=0x10;//��������ɣ�ƬѡSS����
	 	 VPORT0_OUT&=0xF7;//����
}
/*
+------------------------------------------------------------------------------
| Function    : д��������
+------------------------------------------------------------------------------
*/
 
void SPI_WRITE_DATA(void)//д��������
{
		//���Ͳ�����֮ǰƬѡ�����½���
		VPORT0_OUT=0x04;//���չ�������
		VPORT3_OUT|=0x10;
		for(uint8_t i=0;i<=5;i++);
		VPORT3_OUT&=0xef;
     	//����дʹ�ܲ�����//WREN 0X06
		 
        SPID_DATA=0X06;
	    while((SPID_STATUS&0x80)!=0x80);
		VPORT3_OUT|=0x10;//����дʹ�ܲ����� ��ɣ�ƬѡSS����

		//���Ͳ�����֮ǰƬѡ�����½���
		VPORT3_OUT|=0x10;
		for(uint8_t i=0;i<=5;i++);
		VPORT3_OUT&=0xef;
		//����д������
	  //WRITE 0X02
        SPID_DATA=0X02;
		while((SPID_STATUS&0x80)!=0x80);
		//������λ��ɣ�д3���ֽڵĵ�ַ
	    SPI_WRITE_ADDRESS();

	    for(uint8_t i=0;i<receivedata[4];i++)
         {
		  
		 SPID_DATA=receivedata[i+5];
		 while((SPID_STATUS&0x80)!=0x80);
		//������λ��ɣ�������1��д��һ���ֽڵ�����
		  }
		 VPORT3_OUT|=0x10;//д������ɣ�ƬѡSS����
		 VPORT0_OUT&=0xFB;//����
}

/*
+------------------------------------------------------------------------------
| Function    : ��д���ݶ�Ҫд�����ֽڵĵ�ַ 
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
| Description : USARTC0�����жϺ��� �յ������ݷ��ͻ�ȥUSARTC0
+------------------------------------------------------------------------------
*/

ISR(USARTC0_RXC_vect) 
{    
      
       int count_num=0;
	   VPORT0_OUT|=0x10;//���չ�������
	   receivedata[count_num]=USARTC0_DATA;//��Data Register

	   TCC0_CNT=0;

	   //���ռ������20�����ս���
       while(TCC0_CNT<=20)
	  {
		if((USARTC0.STATUS&0x80)==0x80)
	  	{
		count_num++;
        receivedata[count_num]=USARTC0_DATA;//��Data Register
	    TCC0_CNT=0;
		 
		} 

		}
        //VPORT3_OUT|=0x01;//PORTD0����� ʹ��485����
	 
	    //while(!(USARTC0.STATUS & USART_DREIF_bm));
		 
	    //USART_PutChar(&USARTC0,receivedata[2]);

		//while(!(USARTC0.STATUS & USART_TXCIF_bm));
        //VPORT3_OUT&=0xFE;//PORTD0����� ʹ��485����

		VPORT0_OUT&=0xef;//���ս�������

	    if(receivedata[0]==0xf9)//�Զ���SPIд����
        SPI_WRITE_DATA();
        if(receivedata[0]==0xfa)//�Զ���SPI������ 
		SPI_READ_DATA();
	 
		
	     
} 

ISR(USARTC0_TXC_vect) 
{

}
