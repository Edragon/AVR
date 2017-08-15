#include <iom16v.h>
#include <macros.h>
#define uchar unsigned char
#define uint unsigned int
#define LcdBus PORTC
	
const uchar rs=3;
const uchar rw=4;
const uchar en=5;
const uchar psb=6;
const uchar rst=7;


/*------------------us delay function-----------------------------*/ 
void delayus(uint us) 
{  uint i; 
   us=us*5/4;
   for(i=0;i<us;i++); 
} 	


/*------------------ms delay function-----------------------------*/ 
void delayms(uint ms) 
{  uint i,j; 
   for(i=0;i<ms;i++) 
      for(j=0;j<1141;j++); 
} 


/*------------------Busy Check--------------------------*/ 
void ChkBusy(void)
{
	DDRC=0X00;		//Set as input
	PORTC=0X00;
	PORTD &=~BIT(rs);
	PORTD |=BIT(rw);
	PORTD |=BIT(en);
	while(PINC & 0X80);
	PORTD &=~BIT(en);
	DDRC=0XFF;		//Set as output
}

/*------------------Clean Screen--------------------------*/   
void clrscreen(void) 
{ 
    write_com(0x01); 
    delayms(2); 
} 


/*------------------Initialization LCD--------------------------*/    
void LcdIni(void)
{
	DDRC=0XFF;
	PORTC=0XFF;
	DDRD=0XFF;
	PORTD=0XFF;
	
	PORTD &= ~BIT(rst);
	NOP();
	PORTD |=BIT(rst);		     //Reset
	
	delayms(5);
	write_com(0x30);             //Select the basic commands list, select the 8 bits data stream
	delayms(5);
	write_com(0x01);             //Clear screen, and set the address pointer to 00H
	delayms(5);
	write_com(0x06);             //Set moving direction and moving bits of the screen marker when reading and writing
	delayms(5);	
	write_com(0x0c);             //Open display (without screen market, no reversed white)


/*------------------Write commands--------------------------*/ 
void write_com(uchar com)
{
	ChkBusy();
	PORTD &=~BIT(rs);
	PORTD &=~BIT(rw);
	PORTD &=~BIT(en);
	PORTC =com;
	PORTD |=BIT(en);
    delayus(10);
	PORTD &=~BIT(en);
}


/*------------------write data--------------------------*/ 
void write_data(uchar dat)
{
	ChkBusy();
	PORTD |=BIT(rs);
	PORTD &=~BIT(rw);
	PORTD &=~BIT(en);
	PORTC=dat;
	PORTD |=BIT(en);
	delayus(10);
	PORTD &=~BIT(en);
}