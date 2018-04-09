#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include "avr_compiler.h"

//Ӳ������

#define SPI_SS_bm             0x10 //SS ��������
#define SPI_MOSI_bm           0x20 //MOSI ��������
#define SPI_MISO_bm           0x40 //MISO ��������
#define SPI_SCK_bm            0x80 //SCK ��������

//����״̬����

#define SPI_OK              0     //����ɹ�
#define SPI_INTERRUPTED     1     //�����������ж�
#define SPI_BUSY            2     //����æ


//SPI���ݰ��ṹ��
typedef struct SPI_DataPacket
{
	PORT_t *ssPort;                     //SSƬѡ����ָ��
	uint8_t ssPinMask;                  //SSƬѡ��������
	const uint8_t *transmitData;        //Ҫ��������ݵ�ָ��
	volatile uint8_t *receiveData;      //�������ݵĴ洢λ��ָ��
	volatile uint8_t bytesToTransceive; //Ҫ��������ݵ��ֽ���
	volatile uint8_t bytesTransceived;  //Ҫ���յ����ݵ��ֽ���
	volatile bool complete;             //��ɱ�־
} SPI_DataPacket_t;


//SPI�����ṹ��
typedef struct SPI_Master
{
	SPI_t *module;                //ģ��ָ��
	PORT_t *port;                 //ģ��ʹ�õĶ˿�
	bool interrupted;             //���SS�������������͵Ļ�Ϊ��
	SPI_DataPacket_t *dataPacket; //���ݰ�ָ��
} SPI_Master_t;


//SPI�ӻ��ṹ��
typedef struct SPI_Slave
{
	SPI_t *module;      //ģ��ָ��
	PORT_t *port;       //ģ��ʹ�õĶ˿�
} SPI_Slave_t;


//�궨��


/*! ��鴫���Ƿ����
 *  \���� _spi     SPI�����ṹ��ʵ��
 *  \return         ����ĵ�ǰ״̬
 *  \retval true    ����ɹ�
 *  \retval false   �������ڽ���
 */
#define SPI_MasterInterruptTransmissionComplete(_spi) ( (_spi)->dataPacket->complete )


/*! \����SS�Դӻ�Ѱַ
 *  \���� _port        SS�������ڶ˿�
 *  \���� _pinBM       ����������������SS
 */
#define SPI_MasterSSLow(_port, _pinBM) ( (_port)->OUTCLR = (_pinBM) )


/*! \�ͷ�SS
 *  \���� _port         SS�������ڶ˿�
 *  \���� _pinBM        ����������������SS
 */
#define SPI_MasterSSHigh(_port, _pinBM) ( (_port)->OUTSET = (_pinBM) )


/*! \д�����Խӵ�SPI��λ�Ĵ���
 *  \���� _spi        SPI�ӻ��ṹ��ʵ��
 *  \���� _data       ����
 */
#define SPI_SlaveWriteByte(_spi, _data) ( (_spi)->module->DATA = (_data) )


/*! \�ӻ�������
 *  \���� _spi       SPI�ӻ��ṹ��ʵ��
 *  \return           ����
 */
#define SPI_SlaveReadByte(_spi) ( (_spi)->module->DATA )


/*! \����������Ƿ����
 *  \���� _spi       SPI�ӻ��ṹ��ʵ��
 *  \return          ������ݿ��÷�������򷵻ؼ�
 */
#define SPI_SlaveDataAvailable(_spi) ( (_spi)->module->STATUS & SPI_IF_bm )


//��������

void SPI_MasterInit(SPI_Master_t *spi,
                    SPI_t *module,
                    PORT_t *port,
					bool lsbFirst,
                    SPI_MODE_t mode,
                    SPI_INTLVL_t intLevel,
                    bool clk2x,
                    SPI_PRESCALER_t clockDivision);

void SPI_SlaveInit(SPI_Slave_t *spi,
                   SPI_t *module,
                   PORT_t *port,
                   bool lsbFirst,
                   SPI_MODE_t mode,
                   SPI_INTLVL_t intLevel);

void SPI_MasterCreateDataPacket(SPI_DataPacket_t *dataPacket,
                                const uint8_t *transmitData,
                                uint8_t *receiveData,
                                uint8_t bytesToTransceive,
                                PORT_t *ssPort,
                                uint8_t ssPinMask);

void SPI_MasterInterruptHandler(SPI_Master_t *spi);

uint8_t SPI_MasterInterruptTransceivePacket(SPI_Master_t *spi,
                                            SPI_DataPacket_t *dataPacket);

uint8_t SPI_MasterTransceiveByte(SPI_Master_t *spi, uint8_t TXdata);

bool SPI_MasterTransceivePacket(SPI_Master_t *spi,
                                SPI_DataPacket_t *dataPacket);

#endif
