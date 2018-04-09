/*
 * ������:
     DMA
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
 * ����:DMA���ο鴫����ظ��鴫��Ĵ���ʵ�֣����������Ժ������ͨ��
        ������ʾ��֤��ȷ��
     	 
*/
#include "dma_driver.h"
#include "avr_compiler.h"
#include "usart_driver.c"
/*�洢���С */
#define MEM_BLOCK_SIZE   (10)
 /*�洢�����*/
#define MEM_BLOCK_COUNT (10)

/*�洢�ܵĴ�С ��demo�� MEM_BLOCK_SIZE * MEM_BLOCK_COUNT���ܴ���64K*/
#define MEM_BLOCK ( MEM_BLOCK_SIZE * MEM_BLOCK_COUNT )

uint8_t memoryBlockA[MEM_BLOCK];
uint8_t memoryBlockB[MEM_BLOCK];

volatile bool gInterruptDone;
volatile bool gStatus;
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
| Function    : BlockMemCopy
+------------------------------------------------------------------------------
| Description : ���ο鴫��
+------------------------------------------------------------------------------
*/
bool BlockMemCopy( const void * src,
                   void * dest,
                   uint16_t blockSize,
                   volatile DMA_CH_t * dmaChannel )
{
	DMA_EnableChannel( dmaChannel );

	DMA_SetupBlock( dmaChannel,
	                src,
	                DMA_CH_SRCRELOAD_NONE_gc,
	                DMA_CH_SRCDIR_INC_gc,
	                dest,
	                DMA_CH_DESTRELOAD_NONE_gc,
	                DMA_CH_DESTDIR_INC_gc,
	                blockSize,
	                DMA_CH_BURSTLEN_8BYTE_gc,
	                0,
	                false );

	DMA_StartTransfer( dmaChannel );

	return true;
}
/*
+------------------------------------------------------------------------------
| Function    : MultiBlockMemCopy
+------------------------------------------------------------------------------
| Description : �ظ��鴫��
+------------------------------------------------------------------------------
*/
bool MultiBlockMemCopy( const void * src, void * dest, uint16_t blockSize,
                          uint8_t repeatCount, volatile DMA_CH_t * dmaChannel )
{
	uint8_t flags;
    //ʹ��DMAͨ��
	DMA_EnableChannel( dmaChannel );
    //����Դ��ַĿ�ĵ�ַ�����С���Լ���ַ���������á�ģʽ���ã�ͻ������8���ֽڣ��ظ�����
	DMA_SetupBlock( dmaChannel,
	                src,
	                DMA_CH_SRCRELOAD_NONE_gc,
	                DMA_CH_SRCDIR_INC_gc,
	                dest,
	                DMA_CH_DESTRELOAD_NONE_gc,
	                DMA_CH_DESTDIR_INC_gc,
	                blockSize,
	                DMA_CH_BURSTLEN_8BYTE_gc,
	                repeatCount,
	                true );

	DMA_StartTransfer( dmaChannel );

	//�ȴ����ִ���������
	do {
		flags = DMA_ReturnStatus_non_blocking( dmaChannel );
	} while ( flags == 0);

	dmaChannel->CTRLB |= ( flags );
    //�������־
	if ( ( flags & DMA_CH_ERRIF_bm ) != 0x00 ) {
		return false;
	} else {
		return true;
	}
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
	uint32_t index;
	uart_init();
	volatile DMA_CH_t * Channel;
	Channel = &DMA.CH0;

	DMA_Enable();
 
    //���е�ͨ������
	for ( index = 0; index < MEM_BLOCK; ++index ) 
	{
		memoryBlockA[index] = ( (uint8_t) index & 0xff );
	}
    //�ظ����ݿ鴫��
	gStatus = MultiBlockMemCopy( memoryBlockA,
	                             memoryBlockB,
	                             MEM_BLOCK_SIZE,
	                             MEM_BLOCK_COUNT,
	                             Channel );
	if ( gStatus )
	   {
		       for ( index = 0; index < MEM_BLOCK; ++index) 
		           uart_putdw_dec(memoryBlockB[index]);
			  
		}
    //���ݿ鵥�δ���
	if ( gStatus ) 
	    {

	    //ʹ�ܵͼ��жϣ���������ɻ��ߴ������
		DMA_SetIntLevel( Channel, DMA_CH_TRNINTLVL_LO_gc, DMA_CH_ERRINTLVL_LO_gc );
		PMIC.CTRL |= PMIC_LOLVLEN_bm;
		sei();

		//������д��������
		 uart_putc('\n');
		for ( index = 0; index < MEM_BLOCK; ++index ) 
		{
			memoryBlockA[index] = 0xff - ( (uint8_t) index & 0xff );
		}
		 uart_putc('\n');

		//��־λ�����жϺ���������true
		gInterruptDone = false;

	    //���ݿ鵥�δ���
		gStatus = BlockMemCopy( memoryBlockA,
		                        memoryBlockB,
		                        MEM_BLOCK,
		                        Channel);

        //�ȴ��������
		do {} while ( gInterruptDone == false );
		 uart_putc('\n');
		if ( gStatus ) 
		        {
			       for ( index = 0; index < MEM_BLOCK; ++index ) 
				   uart_putdw_dec(memoryBlockB[index]);
			     }
		 uart_putc('\n');
		}
		while(1);
	}
 


//DMA�����жϺʹ�������ж�
ISR(DMA_CH0_vect)
{
	if (DMA.CH0.CTRLB & DMA_CH_ERRIF_bm) {
		DMA.CH0.CTRLB |= DMA_CH_ERRIF_bm;
		gStatus = false;
	} else {
		DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm;
		gStatus = true;
	}
	gInterruptDone = true;
}
