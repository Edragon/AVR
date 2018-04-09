/*
 * ������:
     DAC
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-03-03 09:03
 * ����:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
 * ����:
 		ʹ��ͨ��0��ͨ��1������λ��180������ǲ�
 		 
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

	DAC_DualChannel_Enable( &DACB,//�˿�B
	                        DAC_REFSEL_INT1V_gc,//�ڲ�1V
	                        false,//�����
	                        DAC_CONINTVAL_4CLK_gc,// 4 CLK / 6 CLK  S/H 
	                        DAC_REFRESH_32CLK_gc );//ˢ����

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
