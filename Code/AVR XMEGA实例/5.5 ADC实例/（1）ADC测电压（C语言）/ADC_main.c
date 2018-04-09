/*
 * 工程名:
     ADC
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
		void ADCA_CH0_1_2_3_Sweep_Interrupt(void)
		ADCA 4通道扫描，自由运行模式，CH2低级别中断，中断子程序将
		四个通道的转换结果记录到数组adcSamples[4][10]中，USARTC0打印
		输出调试信息

		void ADCA_CH0_1_2_3_Poll(void)
		ADCA 4通道扫描，自由运行模式，查询中断标志读取转换结果
*/

#include "avr_compiler.h"
#include "usart_driver.c"
#include "adc_driver.c"

/*! How many samples for each ADC channel.*/
#define SAMPLE_COUNT 10

/*! Sample storage (all four channels).*/
int16_t adcSamples[4][SAMPLE_COUNT];

uint16_t interrupt_count = 0;
int8_t offset;
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

void ADCA_CH0_1_2_3_Sweep_Interrupt(void)
{	
	uart_puts("inside ADCA_CH0_1_2_3_Sweep_Interrupt");
	uart_putc('\n');
	// 加载校准值
	ADC_CalibrationValues_Load(&ADCA);

	// 设置 ADC A 有符号模式 12 位分辨率
  	ADC_ConvMode_and_Resolution_Config(&ADCA, ADC_ConvMode_Signed, ADC_RESOLUTION_12BIT_gc);

	// 设置ADC分频 
	ADC_Prescaler_Config(&ADCA, ADC_PRESCALER_DIV32_gc);

	// 设置参考电压 VCC/1.6 V
	ADC_Reference_Config(&ADCA, ADC_REFSEL_INT1V_gc);

   	//  ADC A，差分输入，无增益
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,ADC_CH_INPUTMODE_DIFF_gc,ADC_DRIVER_CH_GAIN_NONE);

   	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN0_gc, ADC_CH_MUXNEG_PIN0_gc);

	ADC_Enable(&ADCA);
	/* Wait until common mode voltage is stable. Default clk is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	ADC_Wait_8MHz(&ADCA);

	//补偿值
 	offset = ADC_Offset_Get_Signed(&ADCA, &ADCA.CH0, false);

	uart_puts(" ADC offset = ");
	uart_putc_hex(offset);
	uart_putc('\n');
    ADC_Disable(&ADCA);
    
	/* Setup channel 0, 1, 2 and 3 to have single ended input. */
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH1,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH2,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH3,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

    
	/* Set input to the channels in ADC A to be PIN 0, 2, 4 and 6. */
	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN0_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH1, ADC_CH_MUXPOS_PIN2_gc, 0);
	
	ADC_Ch_InputMux_Config(&ADCA.CH2, ADC_CH_MUXPOS_PIN4_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH3, ADC_CH_MUXPOS_PIN6_gc, 0);

	 /*Setup sweep of all four virtual channels.*/
	ADC_SweepChannels_Config(&ADCA, ADC_SWEEP_0123_gc);

	/* Enable low level interrupts on ADCA channel 2, on conversion complete. */
	ADC_Ch_Interrupts_Config(&ADCA.CH2, ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_LO_gc);

	/* Enable PMIC interrupt level low. */
	PMIC.CTRL |= PMIC_LOLVLEX_bm;

	/* Enable global interrupts. */
	sei();

	/* Enable ADC A with free running mode, VCC reference and signed conversion.*/
	ADC_Enable(&ADCA);

	/* Wait until common mode voltage is stable. Default clock is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	ADC_Wait_8MHz(&ADCA);

	/* Enable free running mode. */
	ADC_FreeRunning_Enable(&ADCA);
}


void ADCA_CH0_1_2_3_Poll(void)
{
	uart_puts("inside ADCA_CH0_1_2_3_Poll");
	uart_putc('\n');
	/* Move stored calibration values to ADC A. */
	ADC_CalibrationValues_Load(&ADCA);

	/* Set up ADC A to have signed conversion mode and 12 bit resolution. */
  	ADC_ConvMode_and_Resolution_Config(&ADCA, ADC_ConvMode_Signed, ADC_RESOLUTION_12BIT_gc);

	/* Set sample rate. */
	ADC_Prescaler_Config(&ADCA, ADC_PRESCALER_DIV32_gc);

	/* Set reference voltage on ADC A to be VCC/1.6 V.*/
	ADC_Reference_Config(&ADCA, ADC_REFSEL_VCC_gc); 

	/* Setup channel 0, 1, 2 and 3 with different inputs. */
	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH0,
	                                 ADC_CH_INPUTMODE_DIFF_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH1,
	                                 ADC_CH_INPUTMODE_INTERNAL_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH2,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
                                     ADC_DRIVER_CH_GAIN_NONE);

	ADC_Ch_InputMode_and_Gain_Config(&ADCA.CH3,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_DRIVER_CH_GAIN_NONE);

   	/* Get offset value for ADC A. */
   	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN0_gc, ADC_CH_MUXNEG_PIN0_gc);

	ADC_Enable(&ADCA);
	/* Wait until common mode voltage is stable. Default clk is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	ADC_Wait_8MHz(&ADCA);
 	offset = ADC_Offset_Get_Signed(&ADCA, &ADCA.CH0, false);
	
	uart_puts(" ADC offset = ");
	uart_putc_hex(offset);
	uart_putc('\n');

    ADC_Disable(&ADCA);
    
	/* Set input to the channels in ADC A */
	ADC_Ch_InputMux_Config(&ADCA.CH0, ADC_CH_MUXPOS_PIN0_gc, ADC_CH_MUXNEG_PIN2_gc);
	ADC_Ch_InputMux_Config(&ADCA.CH1, ADC_CH_MUXINT_SCALEDVCC_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH2, ADC_CH_MUXPOS_PIN0_gc, 0);
	ADC_Ch_InputMux_Config(&ADCA.CH3, ADC_CH_MUXPOS_PIN2_gc, 0);

	/* Setup sweep of all four virtual channels. */
	ADC_SweepChannels_Config(&ADCA, ADC_SWEEP_0123_gc);

	/* Enable ADC A .*/
	ADC_Enable(&ADCA);

	/* Wait until common mode voltage is stable. Default clk is 2MHz and
	 * therefore below the maximum frequency to use this function. */
	ADC_Wait_8MHz(&ADCA);

	/* Enable free running mode. */
	ADC_FreeRunning_Enable(&ADCA);

	/* Store samples in table.*/
	for (uint16_t i = 0; i < SAMPLE_COUNT; ++i) {

	  	do{
			/* If the conversion on the ADCA channel 0 never is
			 * complete this will be a deadlock. */
		}while(!ADC_Ch_Conversion_Complete(&ADCA.CH0));
		adcSamples[0][i] = ADC_ResultCh_GetWord_Signed(&ADCA.CH0, offset);

		do{
			/* If the conversion on the ADCA channel 1 never is
			 * complete this will be a deadlock.*/
		}while(!ADC_Ch_Conversion_Complete(&ADCA.CH1));
		adcSamples[1][i] = ADC_ResultCh_GetWord_Signed(&ADCA.CH1, offset);

		do{
			/* If the conversion on the ADCA channel 2 never is
			 * complete this will be a deadlock.*/
		}while(!ADC_Ch_Conversion_Complete(&ADCA.CH2));
		adcSamples[2][i] = ADC_ResultCh_GetWord_Signed(&ADCA.CH2, offset);

		do{
		  /* If the conversion on the ADCA channel 3 never is
		   * complete this will be a deadlock.*/
		}while(!ADC_Ch_Conversion_Complete(&ADCA.CH3));
		adcSamples[3][i] = ADC_ResultCh_GetWord_Signed(&ADCA.CH3, offset);
	}


	/* Turn off free running and disable ADC module.*/
	ADC_FreeRunning_Disable(&ADCA);
	ADC_Disable(&ADCA);

	
		uart_puts(" ADCA.CH0 = ");uart_putc('\n');
	for (uint16_t i = 0; i < 10; ++i) {
		uart_putw_dec(adcSamples[0][i]);uart_puts(",");
		}uart_putc('\n');
		uart_puts(" ADCA.CH1 = ");uart_putc('\n');
	for (uint16_t i = 0; i < 10; ++i) {
		uart_putw_dec(adcSamples[1][i]);uart_puts(",");
		}uart_putc('\n');
		uart_puts(" ADCA.CH2 = ");uart_putc('\n');
	for (uint16_t i = 0; i < 10; ++i) {
		uart_putw_dec(adcSamples[2][i]);uart_puts(",");
		}uart_putc('\n');
		uart_puts(" ADCA.CH3 = ");uart_putc('\n');
	for (uint16_t i = 0; i < 10; ++i) {
		uart_putw_dec(adcSamples[3][i]);uart_puts(",");
		}uart_putc('\n');
}


int main(void)
{
	uart_init();
	ADCA_CH0_1_2_3_Sweep_Interrupt();
	//ADCA_CH0_1_2_3_Poll();
	while(true){
	}
}


/*! Interrupt routine that reads out the result form the conversion on all
 *  channels to a global array. If the number of conversions carried out is less
 *  than the number given in the define SAMPLE_COUNT a new conversion on all the
 *  channels is started. If not the interrupt on ADC A channel 3 is disabled.
 *
 */

ISR(ADCA_CH2_vect)
{
	if(interrupt_count == 0)
	{
		uart_puts("inside Interrupt routine interrupt_count = 0");
		uart_putc('\n');
	}
	/*  Read samples and clear interrupt flags. */
  	adcSamples[0][interrupt_count] = ADC_ResultCh_GetWord_Signed(&ADCA.CH0, offset);
	adcSamples[1][interrupt_count] = ADC_ResultCh_GetWord_Signed(&ADCA.CH1, offset);
	adcSamples[2][interrupt_count] = ADC_ResultCh_GetWord_Signed(&ADCA.CH2, offset);
	adcSamples[3][interrupt_count] = ADC_ResultCh_GetWord_Signed(&ADCA.CH3, offset);

  	if(interrupt_count == SAMPLE_COUNT-1)
	{
		ADC_Ch_Interrupts_Config(&ADCA.CH3, ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_OFF_gc);
		ADC_FreeRunning_Disable(&ADCA);
		ADC_Disable(&ADCA);

		uart_puts("inside Interrupt routine interrupt_count = 99 ");uart_putc('\n');

		uart_puts(" ADCA.CH0 = ");uart_putc('\n');
		for (uint16_t i = 0; i < 10; ++i) {
			uart_putw_dec(adcSamples[0][i]);uart_puts(",");
			}uart_putc('\n');
			uart_puts(" ADCA.CH1 = ");uart_putc('\n');
		for (uint16_t i = 0; i < 10; ++i) {
			uart_putw_dec(adcSamples[1][i]);uart_puts(",");
			}uart_putc('\n');
			uart_puts(" ADCA.CH2 = ");uart_putc('\n');
		for (uint16_t i = 0; i < 10; ++i) {
			uart_putw_dec(adcSamples[2][i]);uart_puts(",");
			}uart_putc('\n');
			uart_puts(" ADCA.CH3 = ");uart_putc('\n');
		for (uint16_t i = 0; i < 10; ++i) {
			uart_putw_dec(adcSamples[3][i]);uart_puts(",");
			}uart_putc('\n');
	}
	interrupt_count++;
}
