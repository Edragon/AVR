
#include "dma_driver.h"
//DMA软件复位
void DMA_Reset( void )                 
{	                            
	DMA.CTRL &= ~DMA_ENABLE_bm;
	DMA.CTRL |= DMA_RESET_bm;   
	while (DMA.CTRL & DMA_RESET_bm);	// Wait until reset is completed
}
//DMA设置双缓冲
void DMA_ConfigDoubleBuffering( DMA_DBUFMODE_t dbufMode )
{
	DMA.CTRL = ( DMA.CTRL & ~DMA_DBUFMODE_gm ) | dbufMode;
}
//DMA通道设置优先级
void DMA_SetPriority( DMA_PRIMODE_t priMode )
{
	DMA.CTRL = ( DMA.CTRL & ~DMA_PRIMODE_gm ) | priMode;
}
//查看通道是否繁忙
uint8_t DMA_CH_IsOngoing( volatile DMA_CH_t * channel )
{
	uint8_t flagMask;
	flagMask = channel->CTRLB & DMA_CH_CHBUSY_bm;
	return flagMask;
}
//查看DMA状态寄存器通道繁忙位
uint8_t DMA_IsOngoing( void )
{
	uint8_t flagMask;
	flagMask = DMA.STATUS & 0xF0;
	return flagMask;
}

//查看通道挂起位
uint8_t DMA_CH_IsPending( volatile DMA_CH_t * channel )
{
	uint8_t flagMask;
	flagMask = channel->CTRLB & DMA_CH_CHPEND_bm;
	return flagMask;
}
//查看那些通道有块传输被挂起
uint8_t DMA_IsPending( void )
{
	uint8_t flagMask;
	flagMask = DMA.STATUS & 0x0F;
	return flagMask;
}
//查看通道的错误中断标志位和传输结束中断标志位
uint8_t DMA_ReturnStatus_non_blocking( volatile DMA_CH_t * channel )
{
	uint8_t relevantFlags;
	relevantFlags = channel->CTRLB & (DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm);
	return relevantFlags;
}

//查看错误中断标志位和传输结束中断标志位，直到其中任一位置位结束循环返回标志位
uint8_t DMA_ReturnStatus_blocking( volatile DMA_CH_t * channel )
{
	uint8_t flagMask;
	uint8_t relevantFlags;

	flagMask = DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm;

	do {
		relevantFlags = channel->CTRLB & flagMask;
	} while (relevantFlags == 0x00);

	channel->CTRLB = flagMask;
	return relevantFlags;
}
//DMA通道使能位
void DMA_EnableChannel( volatile DMA_CH_t * channel )
{
	channel->CTRLA |= DMA_CH_ENABLE_bm;
}

//DMA通道关闭
void DMA_DisableChannel( volatile DMA_CH_t * channel )
{
	channel->CTRLA &= ~DMA_CH_ENABLE_bm;
}

//DMA通道复位，所有寄存器回复初始值，如果通道打开在复位之前一定要关闭
void DMA_ResetChannel( volatile DMA_CH_t * channel )
{
	channel->CTRLA &= ~DMA_CH_ENABLE_bm;
	channel->CTRLA |= DMA_CH_RESET_bm;
	channel->CTRLA &= ~DMA_CH_RESET_bm;
}

//为DMA一通道设置中断优先级
void DMA_SetIntLevel( volatile DMA_CH_t * channel,
                      DMA_CH_TRNINTLVL_t transferInt,
                      DMA_CH_ERRINTLVL_t errorInt )
{
	channel->CTRLB = (channel->CTRLB & ~(DMA_CH_ERRINTLVL_gm | DMA_CH_TRNINTLVL_gm)) |
			 transferInt | errorInt;
}


//位块传输设置必要的寄存器
void DMA_SetupBlock( volatile DMA_CH_t * channel,
                     const void * srcAddr,
                     DMA_CH_SRCRELOAD_t srcReload,
                     DMA_CH_SRCDIR_t srcDirection,
                     void * destAddr,
                     DMA_CH_DESTRELOAD_t destReload,
                     DMA_CH_DESTDIR_t destDirection,
                     uint16_t blockSize,
                     DMA_CH_BURSTLEN_t burstMode,
                     uint8_t repeatCount,
                     bool useRepeat )
{
	channel->SRCADDR0 = (( (uint32_t) srcAddr) >> 0*8 ) & 0xFF;
	channel->SRCADDR1 = (( (uint32_t) srcAddr) >> 1*8 ) & 0xFF;
	channel->SRCADDR2 = (( (uint32_t) srcAddr) >> 2*8 ) & 0xFF;

	channel->DESTADDR0 = (( (uint32_t) destAddr) >> 0*8 ) & 0xFF;
	channel->DESTADDR1 = (( (uint32_t) destAddr) >> 1*8 ) & 0xFF;
	channel->DESTADDR2 = (( (uint32_t) destAddr) >> 2*8 ) & 0xFF;

	channel->ADDRCTRL = (uint8_t) srcReload | srcDirection |
	                              destReload | destDirection;
	channel->TRFCNT = blockSize;
	channel->CTRLA = ( channel->CTRLA & ~( DMA_CH_BURSTLEN_gm | DMA_CH_REPEAT_bm ) ) |
	                  burstMode | ( useRepeat ? DMA_CH_REPEAT_bm : 0);

	if ( useRepeat ) {
		channel->REPCNT = repeatCount;
	}
}


//设置DMA通道单次传输
void DMA_EnableSingleShot( volatile DMA_CH_t * channel )
{
	channel->CTRLA |= DMA_CH_SINGLE_bm;
}

//关闭单次通道传输
void DMA_DisableSingleShot( volatile DMA_CH_t * channel )
{
	channel->CTRLA &= ~DMA_CH_SINGLE_bm;
}

//设置通道触发源
void DMA_SetTriggerSource( volatile DMA_CH_t * channel, uint8_t trigger )
{
	channel->TRIGSRC = trigger;
}
 //发出通道传输请求
void DMA_StartTransfer( volatile DMA_CH_t * channel )
{
	channel->CTRLA |= DMA_CH_TRFREQ_bm;
}
