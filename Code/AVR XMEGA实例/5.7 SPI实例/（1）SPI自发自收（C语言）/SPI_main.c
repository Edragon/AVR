/*
 * ������:
     SPI
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-20 12:54
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	������ʹ��SPIE �� SPIF ͨ�š�USARTC0���ڴ�ӡ������Ϣ
 *    - ���� PE4 �� PF4 (SS)
 *    - ���� PE5 �� PF5 (MOSI)
 *    - ���� PE6 �� PF6 (MISO)
 *    - ���� PE7 �� PF7 (SCK)

*/

#include "avr_compiler.h"
#include "usart_driver.c"
#include "spi_driver.c"


//���������ֽ���
#define NUM_BYTES     4

//ȫ�ֱ���

//�˿�E��SPIģ����Ϊ����
SPI_Master_t spiMasterE;

//�˿�F��SPIģ����Ϊ�ӻ�
SPI_Slave_t spiSlaveF;

// SPI ���ݰ�
SPI_DataPacket_t dataPacket;

//����Ҫ���͵�����
uint8_t masterSendData[NUM_BYTES] = {0x11, 0x22, 0x33, 0x44};

//�ӻ����յ�����
uint8_t masterReceivedData[NUM_BYTES];

//���Խ��
bool success = true;

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

/*! \brief Test function.
 *  1: Data is transmitted on byte at a time from the master to the slave.
 *     The slave increments the received data and sends it back. The master reads
 *     the data from the slave and verifies that it equals the data sent + 1.
 *
 *  2: Data is transmitted 4 bytes at a time to the slave. As the master sends
 *     a byte to the slave, the preceding byte is sent back to the master.
 *     When all bytes have been sent, it is verified that the last 3 bytes
 *     received at the master, equal the first 3 bytes sent.
 *
 *  The variable, 'success', will be non-zero when the function reaches the
 *  infinite for-loop if the test was successful.
 */
int main(void)
{
	uart_init(); 
	uart_puts("inside main");uart_putc('\n');

	//SS���������������������
	PORTE.DIRSET = PIN4_bm;
	PORTE.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;

	//SS����
	PORTE.OUTSET = PIN4_bm;

	//SS�������ڶ˿�
	PORT_t *ssPort = &PORTE;

	//��ʼ���˿�E�ϵ�SPI����
	SPI_MasterInit(&spiMasterE,
	               &SPIE,
	               &PORTE,
	               false,
	               SPI_MODE_0_gc,
	               SPI_INTLVL_OFF_gc,
	               false,
	               SPI_PRESCALER_DIV4_gc);

	//��ʼ���˿�F�ϵ�SPI�ӻ�
	SPI_SlaveInit(&spiSlaveF,
	              &SPIF,
	              &PORTF,
	              false,
				  SPI_MODE_0_gc,
				  SPI_INTLVL_OFF_gc);

	
	//1.�����ֽڷ�ʽ����
	// ����: ����SS SPI_MasterTransceiveByte()������SS
	SPI_MasterSSLow(ssPort, PIN4_bm);
	uint8_t result = 0;

	for(uint8_t i = 0; i < NUM_BYTES; i++)
	{
		//����: �����������ݵ��ӻ�
		SPI_MasterTransceiveByte(&spiMasterE, masterSendData[i]);
		uart_puts("SPI_MasterTransceiveByte() return result = ");
		uart_putc_hex(result);
		uart_putc('\n');

		//�ӻ�: �ȴ����ݿ���
		while (SPI_SlaveDataAvailable(&spiSlaveF) == false){}
		uart_puts("SPI_SlaveDataAvailable");
		uart_putc('\n');

		//�ӻ�: ȡ����
		uint8_t slaveByte = SPI_SlaveReadByte(&spiSlaveF);
		uart_puts("slaveByte = ");
		uart_putc_hex(slaveByte);
		uart_putc('\n');

		//�ӻ�: �յ����ݼ�1�����ͻ�ȥ
		slaveByte++;
		SPI_SlaveWriteByte(&spiSlaveF, slaveByte);

		//����: ���Ϳ��ֽڶ�����
		uint8_t masterReceivedByte = SPI_MasterTransceiveByte(&spiMasterE, 0x00);
		uart_puts("masterReceivedByte = ");
		uart_putc_hex(masterReceivedByte);
		uart_putc('\n');

		//����: ��������Ƿ���ȷ
		if (masterReceivedByte != (masterSendData[i] + 1) )
		{
			success = false;
			uart_puts("success = false");
			uart_putc('\n');
		}
		uart_putc('\n');
		uart_putc('\n');
	}
	//����: �ͷ�SS
	SPI_MasterSSHigh(ssPort, PIN4_bm);
	 

	//2: �������ݰ�

	// �������ݰ�(SS ��PE4).
	SPI_MasterCreateDataPacket(&dataPacket,
	                           masterSendData,
	                           masterReceivedData,
	                           NUM_BYTES,
	                           &PORTE,
	                           PIN4_bm);

	// �������ݰ�
	uint8_t SPI_MTP_FLAG = SPI_MasterTransceivePacket(&spiMasterE, &dataPacket);

	uart_puts("SPI_MTP_FLAG = ");
	uart_putc_hex(SPI_MTP_FLAG);
	uart_putc('\n');

	// ���������ݵ���ȷ�ԣ����������ȷ
	for (uint8_t i = 0; i < NUM_BYTES - 1; i++)
	{
		if (masterReceivedData[i + 1] != masterSendData[i])
		{
			success = false;
			uart_puts("success = false");
			uart_putc('\n');
		}
		uart_puts("masterReceivedData[");uart_putw_dec(i);uart_puts("+ 1] = ");
		uart_putc_hex(masterReceivedData[i + 1]);uart_putc('\n');
	}
	
	while(true)
	{
		nop();
	}
}
