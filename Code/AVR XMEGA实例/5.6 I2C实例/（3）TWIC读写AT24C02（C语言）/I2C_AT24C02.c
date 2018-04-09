/*
 * ������:
     I2C_AT24C02
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-20 10:44
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	������ʹ��TWIF�ʹ���EEPROM AT24C02ͨ�ţ���AT24C02
		��2��д��16���ֽڣ��ٶ���ָ��Ƭ���ֽڵ�ַ�������ݡ�
		24C02�д���Ƭ�ڵ�ַ�Ĵ�����ÿд������һ��������
		�ں󣬸õ�ַ�Ĵ����Զ���1����ʵ�ֶ���һ���洢��Ԫ
		�Ķ�д�������ֽھ��Ե�һ������ʽ��ȡ��Ϊ�����ܵ�
		д��ʱ�䣬һ�β�����д����8���ֽڵ����ݡ�
*/

#include "avr_compiler.h"
#include <util/delay.h>
#include "twi_master_driver.c"

//������ַ 0B0 1010 000
#define DEVICE_ADDRESS    0x50

//�����ֽ���
#define NUM_BYTES        9

// CPU 2MHz
#define CPU_SPEED   2000000

// ������100kHz
#define BAUDRATE	100000

#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

//Ƭ���ֽڵ�ַ
uint8_t WORD_ADDRESS = 0x00;

//�ṹ�����
TWI_Master_t twiMaster;    //TWI ����

//��������
uint8_t sendBuffer[NUM_BYTES] = 
{
0x00,
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07
};

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main(void)
{
	// PORTD ��Ϊ��� LED ��ʾ����
	PORTD.DIRSET = 0xFF;

	// ����ⲿû�н��������裬ʹ������ PF0(TWI-SDA)�Ұ�, PF1(TWI-SCL)��ɫ
	PORTCFG.MPCMASK = 0x03; // һ�����ö������
	PORTF.PIN0CTRL = (PORTF.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	//��ʼ������
	TWI_MasterInit(&twiMaster,&TWIF,TWI_MASTER_INTLVL_LO_gc,TWI_BAUDSETTING);

	//ʹ�ܵͼ����ж�
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	uint8_t BufPos = 0;
	while (1) 
	{
		PORTD.OUT =0;
			
		//�����������ݣ�һ��д8���ֽ�
		sendBuffer[0] = WORD_ADDRESS;
		TWI_MasterWrite(&twiMaster,DEVICE_ADDRESS,&sendBuffer[0],9);
		while (twiMaster.status != TWIM_STATUS_READY)
		{
			//�ȴ��������
		}

		//PORTD.OUT = twiMaster.result;
		_delay_ms(600);
	
		//������ȡ���ݣ���дƬ���ֽڵ�ַ ��һ�ζ�8���ֽ�
		TWI_MasterWriteRead(&twiMaster,DEVICE_ADDRESS,WORD_ADDRESS,1,8);
		while (twiMaster.status != TWIM_STATUS_READY)
		{
			//�ȴ��������
		}
		
		//PORTD.OUT = twiMaster.result;
		//_delay_ms(600);

		for(BufPos=0;BufPos<8;BufPos++)
		{
			//LED ��ʾȡ������
			PORTD.OUT = ~twiMaster.readData[BufPos];
			_delay_ms(600);
		}
	}
}

// TWIF �����жϷ������
ISR(TWIF_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}

