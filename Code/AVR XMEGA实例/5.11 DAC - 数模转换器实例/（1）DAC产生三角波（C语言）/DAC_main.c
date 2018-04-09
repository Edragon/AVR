/*
 * 工程名:
     DAC
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-03-03 09:03
 * 作者:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
 * 描述:
 		使用通道0和通道1产生相位差180°的三角波
 		 
*/
#include "dac_driver.c"

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main( void )
{ 
	uint16_t angle;

	DAC_DualChannel_Enable( &DACB,//端口B
	                        DAC_REFSEL_INT1V_gc,//内部1V
	                        false,//左对齐
	                        DAC_CONINTVAL_4CLK_gc,// 4 CLK / 6 CLK  S/H 
	                        DAC_REFRESH_32CLK_gc );//刷新率

	while (1)
	{
		for ( angle = 0; angle < 0x1000; ++angle )
		{
			while ( DAC_Channel_DataEmpty( &DACB, CH0 ) == false )
			{
			}
			DAC_Channel_Write( &DACB, angle, CH0 );

			while ( DAC_Channel_DataEmpty( &DACB, CH1 ) == false )
			{
			}
			DAC_Channel_Write( &DACB, 0xFFF - angle, CH1 );
		}

		for ( angle = 0; angle < 0x1000; ++angle )
		{
			while ( DAC_Channel_DataEmpty( &DACB, CH0 ) == false )
			{
			}
			DAC_Channel_Write( &DACB, 0xFFF - angle, CH0 );

			while ( DAC_Channel_DataEmpty( &DACB, CH1 ) == false )
			{
			}
			DAC_Channel_Write( &DACB, angle, CH1 );
		}
	}
}
