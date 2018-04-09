char                          Rx_Buff[20],            //接收缓冲 
volatile unsigned char        Rx_Length;              //接收数据长度计数  

#pragma vector = USART_RXC_vect                       // 串口中断  

 
__interrupt void Usart_RFID_Rx_ISR(void)


{
	unsigned char i=0;
	char Rx_Char;
	Rx_Char = UDR;            //接收到一位数据
	Rx_Buff[Rx_Length] = Rx_Char;

	if (Rx_Char == 0x02)         //如果收到的数据是02，证明接收到的是数据头
	{
	   Rx_Length  = 0;       //已经接收了1位数据了
	} 
	Rx_Length ++； 

	if((Rx_Char == 0x03) && (Rx_Length == 13))		// 收13个数据  
		  { 
			 Read_Card_Flag = true;        // 接收完成，卡号已经放Rx_Buff[]里了
		   }  
 }

 
 
 
void Init_Uart (void)    // 初始化串口为9600,8N1
{
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = 0x0E;
 UBRRL = 0x2F; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0xD8; 
Rx_Length  = 0;
}



