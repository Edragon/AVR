#include "spi_driver.h"

/*! \SPI������ʼ��
 *  \���� spi            SPI�����ṹ��ʵ��
 *  \���� module         SPIģ��ָ��
 *  \���� port           SPIģ�����ڶ˿�
 *  \���� lsbFirst       �����Ϊ����ֵ������˳��ΪLSB
 *  \���� mode           SPI ģʽ (ʱ�Ӽ��Ժ���λ).
 *  \���� intLevel       SPI �жϼ���
 *  \���� clk2x	      	 SPI ����
 *  \���� clockDivision  SPI ʱ�ӷ�Ƶ����
 */
void SPI_MasterInit(SPI_Master_t *spi,
                    SPI_t *module,
                    PORT_t *port,
                    bool lsbFirst,
                    SPI_MODE_t mode,
                    SPI_INTLVL_t intLevel,
                    bool clk2x,
                    SPI_PRESCALER_t clockDivision)
{
	spi->module         = module;
	spi->port           = port;
	spi->interrupted    = false;
	spi->module->CTRL   = clockDivision |                  //��Ƶ����
	                      (clk2x ? SPI_CLK2X_bm : 0) |     //����
	                      SPI_ENABLE_bm |                  //SPIģ��ʹ��
	                      (lsbFirst ? SPI_DORD_bm  : 0) |  //����˳��
	                      SPI_MASTER_bm |                  //SPI����
	                      mode;                            //SPI ģʽ
	//�жϼ���
	spi->module->INTCTRL = intLevel;
	//δָ�����ݰ�
	spi->dataPacket = NULL;
 	//MOSI �� SCK ��Ϊ���
	spi->port->DIRSET  = SPI_MOSI_bm | SPI_SCK_bm;
}


/*! \SPI�ӻ���ʼ��
 *  \���� spi                  SPI�ӻ��ṹ��ʵ��
 *  \���� module               SPIģ��ָ��
 *  \���� port                 SPIģ�����ڶ˿�
 *  \���� lsbFirst             �����Ϊ����ֵ������˳��ΪLSB
 *  \���� mode                 SPI ģʽ (ʱ�Ӽ��Ժ���λ).
 *  \���� intLevel             SPI �жϼ���
 */
void SPI_SlaveInit(SPI_Slave_t *spi,
                   SPI_t *module,
                   PORT_t *port,
                   bool lsbFirst,
                   SPI_MODE_t mode,
                   SPI_INTLVL_t intLevel)
{
	//SPIģ��
	spi->module       = module;
	spi->port         = port;
	spi->module->CTRL = SPI_ENABLE_bm |                //SPIģ��ʹ��
	                    (lsbFirst ? SPI_DORD_bm : 0) | //����˳��
	                    mode;                          //SPI ģʽ
	//�жϼ���
	spi->module->INTCTRL = intLevel;
 	//MISO��Ϊ���
	spi->port->DIRSET = SPI_MISO_bm;
}



/*! \�������ݰ�
 *
 *  dataPacket, transmitData �� receiveData �����ں�����׼���ã�
    receiveData �� transmitData �Ĵ�С�����bytesToTransceiveһ��
 *  \���� dataPacket         ���ݰ�ָ��
 *  \���� transmitData       ��������ָ��
 *  \���� receiveData        ��������ָ��
 *  \���� bytesToTransceive  �շ������ֽ���
 *  \���� ssPort             SS�������ڶ˿�
 *  \���� ssPinMask          SS��������
 */
void SPI_MasterCreateDataPacket(SPI_DataPacket_t *dataPacket,
                                const uint8_t *transmitData,
                                uint8_t *receiveData,
                                uint8_t bytesToTransceive,
                                PORT_t *ssPort,
                                uint8_t ssPinMask)
{
	dataPacket->ssPort            = ssPort;
	dataPacket->ssPinMask         = ssPinMask;
	dataPacket->transmitData      = transmitData;
	dataPacket->receiveData       = receiveData;
	dataPacket->bytesToTransceive  = bytesToTransceive;
	dataPacket->bytesTransceived   = 0;
	dataPacket->complete          = false;
}


/*! \SPI�����жϷ������
 *  \���� spi        SPI�����ṹ��ʵ��
 */
void SPI_MasterInterruptHandler(SPI_Master_t *spi)
{
	uint8_t data;
	uint8_t bytesTransceived = spi->dataPacket->bytesTransceived;

	// ���SS�����ж�(SS ʹ���ұ�����).��ʱû�����ݽ��յ�
	if ( !(spi->module->CTRL & SPI_MASTER_bm) )
	{
		spi->interrupted = true;
	}
	else 
	{   //�����ж�
		//�洢����
		data = spi->module->DATA;
		spi->dataPacket->receiveData[bytesTransceived] = data;
		//��һ�ֽ�
		bytesTransceived++;
		//�����������û����
		if (bytesTransceived < spi->dataPacket->bytesToTransceive)
		{
			//������д�����ݼĴ���
			data = spi->dataPacket->transmitData[bytesTransceived];
			spi->module->DATA = data;
		}
		else //�������
		{
			//�ͷ�SS
			uint8_t ssPinMask = spi->dataPacket->ssPinMask;
			SPI_MasterSSHigh(spi->dataPacket->ssPort, ssPinMask);
			spi->dataPacket->complete = true;
		}
	}
	//��bytesTransceivedд�����ݰ�
	spi->dataPacket->bytesTransceived = bytesTransceived;
}


/*! \��������
 *  ���ݰ�����׼����
 *
 *  \���� spi                SPI�����ṹ��ʵ��
 *  \���� dataPacket         SPI���ݰ��ṹ��ʵ��

 *  \return                   ״̬
 *  \retval SPI_OK            �������
 *  \retval SPI_BUSY          SPIģ��æ
 *  \retval SPI_INTERRUPTED   �������������
 */
uint8_t SPI_MasterInterruptTransceivePacket(SPI_Master_t *spi,
                                            SPI_DataPacket_t *dataPacket)
{
	uint8_t data;
	bool interrupted = spi->interrupted;

	//�����û�����ݰ�������
	if (spi->dataPacket == NULL)
	{
		spi->dataPacket = dataPacket;
	}
	//����������ڽ���
	else if (spi->dataPacket->complete == false)
	{
		return (SPI_BUSY);
	}
	//����������������
	else if (interrupted)
	{
		//���SS���ͷ�
		if (spi->port->OUT & SPI_SS_bm)
		{
			//û���ж�
			interrupted = false;
		}
		else {
			return (SPI_INTERRUPTED);
		}
	}
	//���û�б�����������ϣ���������
	spi->dataPacket = dataPacket;
	spi->dataPacket->complete = false;
	spi->interrupted = false;
	//����SS
	uint8_t ssPinMask = spi->dataPacket->ssPinMask;
	SPI_MasterSSLow(spi->dataPacket->ssPort, ssPinMask);
	spi->dataPacket->bytesTransceived = 0;
	//��ʼ��������
	data = spi->dataPacket->transmitData[0];
	spi->module->DATA = data;
	//���ͳɹ�
	return (SPI_OK);
}


/*! \SPI˫�����ֽ�
 *  ��DATA �Ĵ�����������λ���ӻ���ͬʱ�ӻ�������λ��DATA �Ĵ�����
 *  ������û�м�����������Ƿ����ڴ������ݣ�������ϵͳҪ���⾺��
 *
 *  SS �����ڵ��ñ�����ǰ���ͣ��������ͷ�SS
 *
 *  \ע�� ���ʹ���жϷ�ʽ����������
 *
 *  \���� spi        SPI�����ṹ��ʵ��
 *  \���� TXdata     ���͵��ӻ�������
 *
 *  \����            �ӻ�����������
 */
uint8_t SPI_MasterTransceiveByte(SPI_Master_t *spi, uint8_t TXdata)
{
	//��������
	spi->module->DATA = TXdata;
	//�ȴ��������
	while(!(spi->module->STATUS & SPI_IF_bm)) {}
	//������
	uint8_t result = spi->module->DATA;
	return(result);
}



/*! \SPI�������ݰ�
 *  �������������ݰ��ṹ��ʵ���ڵ������ֽڣ����������
 *  SS���ֵ͵�ƽ�������ݴ洢�����ݰ���
 *  \���� spi         SPI�����ṹ��ʵ��
 *  \���� dataPacket  SPI���ݰ��ṹ��ʵ��
 *
 *  \return            ִ�гɹ����
 *  \retval true	   �ɹ�
 *  \retval false	   ʧ��
 */
bool SPI_MasterTransceivePacket(SPI_Master_t *spi,
                                SPI_DataPacket_t *dataPacket)
{
	//������ݰ��Ƿ񱻴���
	if(dataPacket == NULL){return false;}
	//�����ݰ�ָ�븳��SPIģ��
	spi->dataPacket = dataPacket;
	uint8_t ssPinMask = spi->dataPacket->ssPinMask;
	//���ʹ����SS
	if (spi->dataPacket->ssPort != NULL)
	{
		//SS����
		SPI_MasterSSLow(spi->dataPacket->ssPort, ssPinMask);
	}
	//�����ֽ�
	uint8_t bytesTransceived = 0;
	uint8_t bytesToTransceive = dataPacket->bytesToTransceive;
	while (bytesTransceived < bytesToTransceive)
	{
		//��������
		uint8_t data = spi->dataPacket->transmitData[bytesTransceived];
		spi->module->DATA = data;
		//�ȴ��������
		while(!(spi->module->STATUS & SPI_IF_bm)){}
		//������
		data = spi->module->DATA;
		spi->dataPacket->receiveData[bytesTransceived] = data;
		bytesTransceived++;
	}
	//���ʹ����SS
	if (spi->dataPacket->ssPort != NULL)
	{
		//SS�ͷ�
		SPI_MasterSSHigh(spi->dataPacket->ssPort, ssPinMask);
	}
	//���ñ���ָʾ����ɹ�
	spi->dataPacket->bytesTransceived = bytesTransceived;
	spi->dataPacket->complete = true;
	//ִ�гɹ�
	return true;
}
