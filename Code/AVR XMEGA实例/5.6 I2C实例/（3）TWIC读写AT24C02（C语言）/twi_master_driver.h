#ifndef TWI_MASTER_DRIVER_H
#define TWI_MASTER_DRIVER_H

#include "avr_compiler.h"

//波特率计算
#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)


//传输状态
#define TWIM_STATUS_READY              0
#define TWIM_STATUS_BUSY               1


//传输结果
typedef enum TWIM_RESULT_enum {
	TWIM_RESULT_UNKNOWN          = (0x00<<0),
	TWIM_RESULT_OK               = (0x01<<0),
	TWIM_RESULT_BUFFER_OVERFLOW  = (0x02<<0),
	TWIM_RESULT_ARBITRATION_LOST = (0x03<<0),
	TWIM_RESULT_BUS_ERROR        = (0x04<<0),
	TWIM_RESULT_NACK_RECEIVED    = (0x05<<0),
	TWIM_RESULT_FAIL             = (0x06<<0),
} TWIM_RESULT_t;

//读写缓存大小
#define TWIM_WRITE_BUFFER_SIZE         9//首字节为写入的片内字节地址
#define TWIM_READ_BUFFER_SIZE          8


/*! \ TWI 主机结构体
 *包含模块，变量的指针，状态
 */
typedef struct TWI_Master {
	TWI_t *interface;                  //TWI接口指针
	register8_t address;                            //从机地址
	register8_t writeData[TWIM_WRITE_BUFFER_SIZE];  //要写的数据
	register8_t readData[TWIM_READ_BUFFER_SIZE];    //读取的数据
	register8_t bytesToWrite;                       //要写的数据字节数
	register8_t bytesToRead;                        //要读的数据字节数
	register8_t bytesWritten;                       //已写的数据字节数
	register8_t bytesRead;                          //已读的数据字节数
	register8_t status;                             //传输状态
	register8_t result;                             //传输结果
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


/*! TWI 主机中断服务程序

 *  拷贝到你的代码里

    ISR(TWIC_TWIM_vect)
    {
      TWI_MasterInterruptHandler(&twiMaster);
    }

 *
 */

#endif /* TWI_MASTER_DRIVER_H */
