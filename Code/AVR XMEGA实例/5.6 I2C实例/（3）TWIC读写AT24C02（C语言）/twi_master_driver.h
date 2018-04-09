#ifndef TWI_MASTER_DRIVER_H
#define TWI_MASTER_DRIVER_H

#include "avr_compiler.h"

//�����ʼ���
#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)


//����״̬
#define TWIM_STATUS_READY              0
#define TWIM_STATUS_BUSY               1


//������
typedef enum TWIM_RESULT_enum {
	TWIM_RESULT_UNKNOWN          = (0x00<<0),
	TWIM_RESULT_OK               = (0x01<<0),
	TWIM_RESULT_BUFFER_OVERFLOW  = (0x02<<0),
	TWIM_RESULT_ARBITRATION_LOST = (0x03<<0),
	TWIM_RESULT_BUS_ERROR        = (0x04<<0),
	TWIM_RESULT_NACK_RECEIVED    = (0x05<<0),
	TWIM_RESULT_FAIL             = (0x06<<0),
} TWIM_RESULT_t;

//��д�����С
#define TWIM_WRITE_BUFFER_SIZE         9//���ֽ�Ϊд���Ƭ���ֽڵ�ַ
#define TWIM_READ_BUFFER_SIZE          8


/*! \ TWI �����ṹ��
 *����ģ�飬������ָ�룬״̬
 */
typedef struct TWI_Master {
	TWI_t *interface;                  //TWI�ӿ�ָ��
	register8_t address;                            //�ӻ���ַ
	register8_t writeData[TWIM_WRITE_BUFFER_SIZE];  //Ҫд������
	register8_t readData[TWIM_READ_BUFFER_SIZE];    //��ȡ������
	register8_t bytesToWrite;                       //Ҫд�������ֽ���
	register8_t bytesToRead;                        //Ҫ���������ֽ���
	register8_t bytesWritten;                       //��д�������ֽ���
	register8_t bytesRead;                          //�Ѷ��������ֽ���
	register8_t status;                             //����״̬
	register8_t result;                             //������
}TWI_Master_t;



void TWI_MasterInit(TWI_Master_t *twi,
                    TWI_t *module,
                    TWI_MASTER_INTLVL_t intLevel,
                    uint8_t baudRateRegisterSetting);
TWI_MASTER_BUSSTATE_t TWI_MasterState(TWI_Master_t *twi);
bool TWI_MasterReady(TWI_Master_t *twi);
bool TWI_MasterWrite(TWI_Master_t *twi,
                     uint8_t address,
                     uint8_t * writeData,
                     uint8_t bytesToWrite);
bool TWI_MasterRead(TWI_Master_t *twi,
                    uint8_t address,
                    uint8_t bytesToRead);
bool TWI_MasterWriteRead(TWI_Master_t *twi,
                         uint8_t address,
                         uint8_t *writeData,
                         uint8_t bytesToWrite,
                         uint8_t bytesToRead);
void TWI_MasterInterruptHandler(TWI_Master_t *twi);
void TWI_MasterArbitrationLostBusErrorHandler(TWI_Master_t *twi);
void TWI_MasterWriteHandler(TWI_Master_t *twi);
void TWI_MasterReadHandler(TWI_Master_t *twi);
void TWI_MasterTransactionFinished(TWI_Master_t *twi, uint8_t result);


/*! TWI �����жϷ������

 *  ��������Ĵ�����

    ISR(TWIC_TWIM_vect)
    {
      TWI_MasterInterruptHandler(&twiMaster);
    }

 *
 */

#endif /* TWI_MASTER_DRIVER_H */
