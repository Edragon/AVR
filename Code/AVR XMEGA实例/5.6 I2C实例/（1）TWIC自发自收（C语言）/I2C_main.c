/*
 * ������:
     I2C
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-19 15:47
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	������ʹ��TWIC�Է����գ�ͬʱʹ�������ʹӻ���
		�������жԷ��ͻ���sendBuffer�е�������ʾ1s��
		���ͣ��ٶԽ���������ʾ1s��
*/

#include "avr_compiler.h"
#include <util/delay.h>
#include "twi_master_driver.c"
#include "twi_slave_driver.c"

//�ӻ���ַ
#define SLAVE_ADDRESS    0x55

//�����ֽ���
#define NUM_BYTES        6

// CPU 2MHz
#define CPU_SPEED   2000000

// ������100kHz
#define BAUDRATE	100000

#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)


//�ṹ�����
TWI_Master_t twiMaster;    //TWI ����
TWI_Slave_t twiSlave;      //TWI �ӻ�


//��������
uint8_t sendBuffer[NUM_BYTES] = {0x01,0x02,0x03,0x04,0x05,0x0F};

/*
+------------------------------------------------------------------------------
| Function    : TWIC_SlaveProcessData
+------------------------------------------------------------------------------
| Description : �ӻ����ݴ����� �Խ�������ȡ��
+------------------------------------------------------------------------------
*/
void TWIC_SlaveProcessData(void)
{
	uint8_t bufIndex = twiSlave.bytesReceived;
	twiSlave.sendData[bufIndex] = (~twiSlave.receivedData[bufIndex]);
}

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
int main(void)
{
	// PORTD ��Ϊ��� LED ��ʾ����
	PORTD.DIRSET = 0xFF;

	// ����ⲿû�н��������裬ʹ������ PC0(TWI-SDA), PC1(TWI-SCL)
	PORTCFG.MPCMASK = 0x03; // һ�����ö������
	PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	//��ʼ������
	TWI_MasterInit(&twiMaster,
	               &TWIC,
	               TWI_MASTER_INTLVL_LO_gc,
	               TWI_BAUDSETTING);

	//��ʼ���ӻ�
	TWI_SlaveInitializeDriver(&twiSlave, &TWIC, TWIC_SlaveProcessData);
	TWI_SlaveInitializeModule(&twiSlave,
	                          SLAVE_ADDRESS,
	                          TWI_SLAVE_INTLVL_LO_gc);

	//ʹ�ܵͼ����ж�
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	uint8_t BufPos = 0;
	while (1) 
	{
		PORTD.OUT =0;
		for(BufPos=0;BufPos<6;BufPos++)
		{
			//LED ��ʾ����
			PORTD.OUT = sendBuffer[BufPos];
			_delay_ms(1000);		
			//������������
			TWI_MasterWriteRead(&twiMaster,SLAVE_ADDRESS,&sendBuffer[BufPos],1,1);
			while (twiMaster.status != TWIM_STATUS_READY)
			{
				//�ȴ��������
			}
			//LED ��ʾ����(�Ѿ�ȡ��)
			PORTD.OUT = (twiMaster.readData[0]);
			_delay_ms(1000);	
		}
	}
}

/*! TWIC Master Interrupt vector. */
ISR(TWIC_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
}

/*! TWIC Slave Interrupt vector. */
ISR(TWIC_TWIS_vect)
{
	TWI_SlaveInterruptHandler(&twiSlave);
}
