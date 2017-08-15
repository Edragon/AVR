#include "RF24L01.H"

void main(void)
{
  uchar sta;
  uchar TX_BUF[32]={'s','u','c','c','e','s','s','f','u','l','!'};
  delayms(22);
  init_NRF24L01();
  while(1)
  {
	nRF24L01_TxPacket(TX_BUF);
	delayms(50);    
  
  }
}