/*
 * 工程名:
     DMA
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
 * 描述:DMA单次块传输和重复块传输的代码实现，并将传输以后的数据通过
        串口显示验证正确性
     	 
*/
#include "dma_driver.h"
#include "avr_compiler.h"
#include "usart_driver.c"
/*存储块大小 */
#define MEM_BLOCK_SIZE   (10)
 /*存储块个数*/
#define MEM_BLOCK_COUNT (10)

/*存储总的大小 在demo中 MEM_BLOCK_SIZE * MEM_BLOCK_COUNT不能大于64K*/
#define MEM_BLOCK ( MEM_BLOCK_SIZE * MEM_BLOCK_COUNT )

uint8_t memoryBlockA[MEM_BLOCK];
uint8_t memoryBlockB[MEM_BLOCK];

volatile bool gInterruptDone;
volatile bool gStatus;
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
| Function    : BlockMemCopy
+------------------------------------------------------------------------------
| Description : 单次块传输
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
| Description : 重复块传输
+------------------------------------------------------------------------------
*/
bool MultiBlockMemCopy( const void * src, void * dest, uint16_t blockSize,
                          uint8_t repeatCount, volatile DMA_CH_t * dmaChannel )
{
	uint8_t flags;
    //使能DMA通道
	DMA_EnableChannel( dmaChannel );
    //设置源地址目的地址，块大小，以及地址的重载设置、模式设置，突发传输8个字节，重复传输
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

	//等待出现错误或传输结束
	do {
		flags = DMA_ReturnStatus_non_blocking( dmaChannel );
	} while ( flags == 0);

	dmaChannel->CTRLB |= ( flags );
    //检测错误标志
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
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
void main( void )
{
	uint32_t index;
	uart_init();
	volatile DMA_CH_t * Channel;
	Channel = &DMA.CH0;

	DMA_Enable();
 
    //进行单通道传输
	for ( index = 0; index < MEM_BLOCK; ++index ) 
	{
		memoryBlockA[index] = ( (uint8_t) index & 0xff );
	}
    //重复数据块传输
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
    //数据块单次传输
	if ( gStatus ) 
	    {

	    //使能低级中断（当传输完成或者传输错误）
		DMA_SetIntLevel( Channel, DMA_CH_TRNINTLVL_LO_gc, DMA_CH_ERRINTLVL_LO_gc );
		PMIC.CTRL |= PMIC_LOLVLEN_bm;
		sei();

		//重新填写发送数组
		 uart_putc('\n');
		for ( index = 0; index < MEM_BLOCK; ++index ) 
		{
			memoryBlockA[index] = 0xff - ( (uint8_t) index & 0xff );
		}
		 uart_putc('\n');

		//标志位，在中断函数里面变成true
		gInterruptDone = false;

	    //数据块单次传输
		gStatus = BlockMemCopy( memoryBlockA,
		                        memoryBlockB,
		                        MEM_BLOCK,
		                        Channel);

        //等待传输结束
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
 


//DMA出错中断和传输完成中断
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
