#include"RF24L01.H"


void main()
{
  uchar sta,i;
  uchar RX_BUF[32];
  init_NRF24L01();
  LcdIni();
  
  while(1)
   {
     SetRX_Mode();
	 if(nRF24L01_RxPacket(RX_BUF))
	  {
	    for(i=0;i<11;i++)  
	     {
	       write_com(0x80+i);
	       write_data(RX_BUF[i]);
	     }
	  }
     delayms(50);
   }
}