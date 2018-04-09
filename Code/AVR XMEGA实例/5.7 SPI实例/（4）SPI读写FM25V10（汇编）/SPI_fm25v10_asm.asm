;*************************************************************************************
;*  
;* File Name         : SPI_fm25v0_asm.asm
;* Title             : SPI��дFM25V10
;* Date              : 2010-7-12  21:36
;* Support E-mail    : jack198651@163.com
;* Target MCU        : ATxmega32A4
;*
;* DESCRIPTION       
;* 	   ������ʹ��SPID��FM25V10��д����
;*
;*   |---XMEGA32A4---|---------FM25V10---------------|
;*   |---PD4---SS----|---1---/S---Chip Select--------|
;*   |---PD6---MISO--|---2----Q---Serial Data Output-|
;*   |---VCC---------|---3---/W---Write Protect------|
;*   |---GND---------|---4----VSS---------Ground-----|
;*   |---PD5---MOSI--|---5----D---Serial Data Input--|
;*   |---PD7---SCK---|---6----C---Serial Clock-------|
;*   |---VCC---------|---7---/HOLD--------HOLD-------|
;*   |---VCC---------|---8----VDD---Supply Voltage---|
;*   ϵͳʱ��ѡ��Ĭ��ʱ��2MHZ
;* WREN  Set Write Enable Latch  0000 0110b 
;* WRDI  Write Disable  0000 0100b 
;* RDSR  Read Status Register  0000 0101b 
;* WRSR  Write Status Register  0000 0001b 
;* READ  Read Memory Data  0000 0011b 
;* FSTRD  Fast Read Memory Data  0000 1011b 
;* WRITE  Write Memory Data   0000 0010b 
;* SLEEP  Enter Sleep Mode  1011 1001b 
;* RDID  Read Device ID  1001 1111b 
;* SNR  Read S/N  1100 0011b
;����д���ݸ�ʽ ��F9(д��־)00 00 00�����ֽڵ�ַ�� 02��Ҫд���ݸ����� 01 02��Ҫд�����ݣ�
;���Ͷ����ݸ�ʽ�� FA(����־)00 00 00 ��Ҫ������ʼ��ַ��02��Ҫ�������ݸ�����
;*************************************************************************************

.include "ATxmega32A4def.inc"//���������ļ�,��������,��Ȼ���ͨ����

.ORG 0
  		RJMP RESET//��λ 
.ORG 0X20
  		RJMP RESET//��λ �����Ե�ʱ��ȫ������[F5]��Ī�������ж��������иõ�ַ��

.ORG 0x032
		JMP USARTC0_INT_RXC//��������C0��������ж��ӳ���

.ORG 0X100        //�����ж���0x00-0x0F4


.MACRO CLKSYS_IsReady
 CLKSYS_IsReady_1:
           LDS R16,OSC_STATUS
		   SBRS R16,@0
		   JMP CLKSYS_IsReady_1//�ȴ��ⲿ����׼����
		   NOP
.ENDMACRO 

PLL_XOSC_Initial:
	   LDI R16,0X10
	   STS PORTCFG_VPCTRLA,R16;PORTBӳ�䵽����˿�1��PORTAӳ�䵽����˿�0
	   LDI R16,0X32
	   STS PORTCFG_VPCTRLB,R16;PORTCӳ�䵽����˿�2��PORTDӳ�䵽����˿�3

	   LDI R16,0X4B
	   STS OSC_XOSCCTRL,R16//���þ���Χ ����ʱ�� 

				   
       LDI R16,OSC_XOSCEN_bm
       STS OSC_CTRL,R16//ʹ���ⲿ����

       CLKSYS_IsReady OSC_XOSCRDY_bp

	   LDI R16,OSC_PLLSRC_XOSC_gc
	   ORI R16,0XC3
       STS OSC_PLLCTRL,R16


	   LDS R16,OSC_CTRL//��ȡ�üĴ�����ֵ��R16
	   SBR R16,OSC_PLLEN_bm//��PLLEN��һλ��λ��ʹ��PLL
	   STS OSC_CTRL,R16

       CLKSYS_IsReady OSC_PLLRDY_bp

	   LDI R16,CLK_SCLKSEL_PLL_gc
	   LDI R17,0XD8//��Կ
	   STS CPU_CCP,R17//����
	   STS CLK_CTRL,R16//ѡ��ϵͳʱ��Դ

	   LDI R16,CLK_PSADIV_1_gc
	   ORI R16,CLK_PSBCDIV_1_1_gc
       LDI R17,0XD8//��Կ
	   STS CPU_CCP,R17//����
	   STS CLK_PSCTRL,R16//����Ԥ��Ƶ��A,B,C��ֵ

	
       RET


SPI_MasterInit: //SPI��ʼ��
		SBI VPORT3_DIR,4 //SSƬѡ������Ϊ�������
		SBI VPORT3_DIR,5//MOSI������Ϊ�������
		SBI VPORT3_DIR,7//SCK������Ϊ�������

		LDI R16,0x50//0,1,0,1,00,00;SPI Clock Double DISABLE,SPI module Enable, Data Order=MSB��Master Select, SPI Mode=0, SPI Clock Prescaler=CLKper/4=0.5MHZ
        STS SPID_CTRL,R16//SPI���ƼĴ���
        LDI R16,0x00
        STS SPID_INTCTRL,R16//SPI �ж� �ر�
		RET

uart_init:

	   /* USARTC0 ���ŷ�������*/
  	   /* PC3 (TXD0) ��� */
        LDI R16,0X08
	    STS PORTC_DIRSET,R16
	   /* PC2 (RXD0) ���� */
	    LDI R16,0X04
	    STS PORTC_DIRCLR,R16
	   /* USARTC0 ģʽ - �첽*/	/* USARTC0֡�ṹ, 8 λ����λ, ��У��, 1ֹͣλ */
	    LDI R16,USART_CMODE_ASYNCHRONOUS_gc|USART_CHSIZE_8BIT_gc|USART_PMODE_DISABLED_gc
	    STS USARTC0_CTRLC,R16
	   /* ���ò�����19200*/
        LDI R16,77
	    STS USARTC0_BAUDCTRLA,R16
	    LDI R16,0
	    STS USARTC0_BAUDCTRLB,R16
 
    	//ʹ�ܽ��շ���
		LDI R16,0X18
		STS USARTC0_CTRLB,R16//000:Reserved,1:����ʹ��,1:����ʹ��,0:�����ʲ��ӱ�,0:����ģʽ��0:TXB8��ʹ��
		//6.���������жϵ��жϼ��𣨿��Թر��жϣ�
		LDI R16,0X30
		STS USARTC0_CTRLA,R16//00:Reserved,11:��������жϼ���Ϊ�߲��ж�,00:��������жϹر�,01: TXBUFFER(DATA)���жϹر�
		//����C0������ϣ�ע�����ù�����ȫ���ж���Ҫ�رգ�
		RET 
RESET:
        CALL PLL_XOSC_Initial
		CALL SPI_MasterInit
		CALL uart_init
		SBI VPORT3_DIR,0//PORTD0���
		CBI VPORT3_OUT,0//PORTD0����� ʹ��485����
		//ָʾ��
		SBI VPORT0_DIR,2//PORTA2���
		SBI VPORT0_DIR,3//PORTA3���
		SBI VPORT0_DIR,4//PORTA4���

TimerC0:
		//2.selecting a clock source
		LDI R16,0X07//������ʱ��ԴΪ24MHZ/1024=23437.5Hz
		STS TCC0_CTRLA,R16

        LDI R16,PMIC_HILVLEN_bm + PMIC_MEDLVLEN_bm + PMIC_LOLVLEN_bm; 
		STS PMIC_CTRL,R16//�ɱ�̶���жϿ��ƼĴ����� �� �Ͳ�ʹ�ܣ�ѭ�����ȹرգ��ж�����δ���� Boot section	
  	    
        SEI//ȫ���ж�ʹ����λ

LOOP:	
		NOP
		CBI VPORT3_OUT,0//PORTD0����� ʹ��485����
        JMP LOOP

///////////////////////////////////////////////////////////////////////////////////////////		
SPI_READ_DATA://����������
		//���Ͳ�����֮ǰƬѡ�����½���
		SBI VPORT3_OUT,4
		NOP
		CBI VPORT3_OUT,4
		//���Ͷ�������
		LDI R16,0X03 //READ 0X03
        STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//������λ��ɣ�д3���ֽڵĵ�ַ
		CALL SPI_WRITE_ADDRESS
		LD  R17,Y+
LOOP_READ_DATA:
		LDI  R16,0X00
		STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//����������λ���,ȡ������
		LDS R16,SPID_DATA
		ST Y+,R16
		//������1������һ���ֽڵ�����
		DEC R17
		BRNE LOOP_READ_DATA
		SBI VPORT3_OUT,4//��������ɣ�ƬѡSS����
		RET

///////////////////////////////////////////////////////////////////////////////////////////		
SPI_WRITE_DATA://д��������
		//���Ͳ�����֮ǰƬѡ�����½���
		SBI VPORT3_OUT,4
		NOP
		CBI VPORT3_OUT,4
     	//����дʹ�ܲ�����
		LDI R16,0X06 //WREN 0X06
        STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		SBI VPORT3_OUT,4//����дʹ�ܲ����� ��ɣ�ƬѡSS����

		//���Ͳ�����֮ǰƬѡ�����½���
		SBI VPORT3_OUT,4
		NOP
		CBI VPORT3_OUT,4
		//����д������
		LDI R16,0X02 //WRITE 0X02
        STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//������λ��ɣ�д3���ֽڵĵ�ַ
		CALL SPI_WRITE_ADDRESS

		LD  R17,Y+
LOOP_WRITE_DATA:
		LD  R16,Y+
		STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//������λ��ɣ�������1��д��һ���ֽڵ�����
		DEC R17
		BRNE LOOP_WRITE_DATA
		SBI VPORT3_OUT,4//д������ɣ�ƬѡSS����
		RET
///////////////////////////////////////////////////////////////////////////////////////////////////
//��д���ݶ�Ҫ�ȴ�SPI��λ��ɱ�־λ��λ
SPI_WAITING_IF:
		LDS R16,SPID_STATUS
		SBRS R16,7//Skip if bit 7(IF) in SPID_STATUS set
		RJMP SPI_WAITING_IF//������λû����ɼ����ȴ�
		RET
///////////////////////////////////////////////////////////////////////////////////////////////////
//��д���ݶ�Ҫд�����ֽڵĵ�ַ
SPI_WRITE_ADDRESS:
		LDI R17,0X03
LOOP_ADDRESS:
		LD  R16,X+
		STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//������λ��ɣ�������1��д��һ���ֽڵĵ�ַ
		DEC R17
		BRNE LOOP_ADDRESS
		RET
///////////////////////////////////////////////////////////////////////////////////////////////////
USARTC0_INT_RXC:
	    LDI R18,0X00//��¼�����ֽ���
		LDI R31,0x20//����������ʼ�����ַZ=0x2000
		LDI R30,0x00
		SBI VPORT0_OUT,4//���չ�������
STORE_DATA:
		LDS R16,USARTC0_DATA//��Data Register
		ST  Z+,R16
        INC R18//�����ַ�����1
		LDI R16,0X00//����������
		STS TCC0_CNT,R16
		STS TCC0_CNT+1,R16

WAITING_RECEIVE:
		LDS R16,TCC0_CNT
		CPI R16,20
        BRSH RECEIVE_END //���ռ������20�����ս���
        LDS R16,USARTC0_STATUS
        SBRS R16,7 //���������˾ʹ�
		JMP WAITING_RECEIVE
	    JMP STORE_DATA
		
RECEIVE_END:
		CBI VPORT0_OUT,4//���ս�������
		//Xָ�븳��ַ
		LDI R27,0X20
		LDI R26,0X00
		LD  R16,X+
		CPI R16,0XF9 //�Զ���SPIд����
		BREQ SPI_WRITE
		CPI R16,0XFA //�Զ���SPI������
		BREQ SPI_READ

SPI_WRITE:
		/*Xָ��ָ����������洢�����ֽڵĵ�ַ
		д�����ݴ���Yָ��ָ����������򣬵�1���ֽڱ����ֽ���Ŀ����2���ֽڼ��������Ҫд������*/
		SBI VPORT0_OUT,3//SPI��ʼд����
		//Xָ�븳��ַ,SPI��ַ�洢��ʼ
		LDI R27,0X20
		LDI R26,0X01
		//Yָ�븳��ַ,SPI�洢���ݿ�ʼ��ַ
		LDI R29,0X20
		LDI R28,0X04
		CALL SPI_WRITE_DATA
		CBI VPORT0_OUT,3//SPI����д����

		RETI

SPI_READ:
		/*Xָ��ָ����������洢�����ֽڵĵ�ַ
		�������ݴ���Yָ��ָ����������򣬵�1���ֽ�����ָ����ȡ���ֽ���Ŀ��
		��2���ֽ�֮���Ǵ�Ŷ���������*/
		SBI VPORT0_OUT,2//SPI��ʼ������
		//Xָ�븳��ַ,SPI��ȡ��ַ
		LDI R27,0X20
		LDI R26,0X01
		//Yָ�븳��ַ,SPI��ȡ���ݸ���
		LDI R29,0X20
		LDI R28,0X04
		CALL SPI_READ_DATA
		CBI VPORT0_OUT,2//SPI����������
		
		//Yָ�븳��ַ,SPI��ȡ���ݸ���
		LDI R29,0X20
		LDI R28,0X04
		LD  R25,Y+//���ݸ���
		SBI VPORT3_OUT,0//ʹ��485����,Ϊ���ݿ��Է��͵���������
		SBI VPORT0_OUT,4//���͹�������
AA_C0:
        LD  R16,Y+
	    STS USARTC0_DATA,R16//����C0���� 
BB_C0:		      
		LDS		R16,USARTC0_STATUS
		SBRS	R16,5
		RJMP	BB_C0	
		DEC 	R25
		CALL DELAY
		CPI 	R25,0//����ֽ����Ƿ�ȫ���������
        BRNE AA_C0
		CBI VPORT0_OUT,4//���ս�������
 	    RETI

DELAY:
        
	    LDI R18,0X55
TT:
        LDI R19,0XFF
HH:
        CPI R19,0
		DEC R19
		BRNE HH

	    DEC R18
        CPI R18,0
		BRNE TT
		RET	
 


