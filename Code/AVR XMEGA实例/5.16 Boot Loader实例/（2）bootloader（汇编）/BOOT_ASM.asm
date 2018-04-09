/*
 * ������:
      BOOT_ASM
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-03-22 21:35
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	�Ӵ��ڷ������ݣ�д���ַΪ0x2800����Ϊflash�ǰ���Ϊ��λ�ģ���Ӧ�ó�������Ȼ���ٶ��������͵�����
		����һ���жϺ�������������ִ�д��룬���������Ϊ������ٴ�ִ��boot������
		���ô�boot������
*/
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����

.include "usart_driver.inc"
.ORG 0X10000       //���ô�boot������
  		JMP RESET  //��λ 
.ORG 0x10032        //USARTC0���ݽ�������ж����
        JMP ISR
 
.ORG 0X10100       ;�����ж���0x00-0x0F4

.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'



/*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : ��ʼ�� USARTC0
+------------------------------------------------------------------------------
*/
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
	        /* ���ò����� 9600*/
           LDI R16,12
	       STS USARTC0_BAUDCTRLA,R16
	       LDI R16,0
	       STS USARTC0_BAUDCTRLB,R16
	        /* USARTC0 ʹ�ܷ���*//* USARTC0 ʹ�ܽ���*/
	       LDI R16,USART_TXEN_bm|USART_RXEN_bm
      	   STS USARTC0_CTRLB,R16
           RET

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/

RESET:       CALL uart_init 
             LDI R16,'M'
			 PUSH R16
			 uart_putc
			 LDI R16,'A'
			 PUSH R16
			 uart_putc
			 LDI R16,'I'
			 PUSH R16
			 uart_putc
			 LDI R16,'N'
			 PUSH R16
			 uart_putc
			 LDI R16,ENTER
			 PUSH R16
			 uart_putc //���ﲻ�ܵ���uart_puts_string��Ϊ�����������ʹ����LPMָ��

             LDI XH,0X10
	         EOR XL,XL
             STS TCC0_PER,XH
	         STS TCC0_PER+1,XL

             LDI R16,TC_CLKSEL_DIV1024_gc//���ڳ���һ��ʱ����Ϊ�Ѿ��������
             STS TCC0_CTRLA,R16

			 /*USARTC0 ���յͼ��ϼ���*/
	         LDI R16,USART_RXCINTLVL_LO_gc
	         STS USARTC0_CTRLA,R16
	         /* Enable interrupts.*/
			 LDI R17,CCP_IOREG_gc
	         LDI R16,PMIC_LOLVLEN_bm|PMIC_IVSEL_bm
			 STS CPU_CCP,R17
	         STS PMIC_CTRL,R16//Enable Low_Level interrupts
 	         SEI
			 LDI R16,0X00
			 STS GPIO_GPIOR0,R16//������1����ת��������

 RESET_LOOP:
             LDS R16,GPIO_GPIOR0
			 CLZ
             CPI R16,1
	         BRNE  RESET_LOOP
			 NOP
			 LDI R16,0X00
			 STS GPIO_GPIOR0,R16//������1����ת��������	 
             JMP QUIT_BOOT
			 JMP RESET_LOOP//��ԶҲ���ᵽ��һ��

/*
+------------------------------------------------------------------------------
| Function    : ISR(USARTE1_RXC_vect)
+------------------------------------------------------------------------------
| Description : USARTC0�����жϺ��� �յ������ݷ��ͻ�ȥUSARTC0
+------------------------------------------------------------------------------
*/
RECEIVE_DATA: 
              LDI R18,0X00//��¼�����ֽ���,�����ط�����ʹ�������ƻ�����
		      LDI R31,0x20//����������ʼ�����ַZ=0x2000
		      LDI R30,0x00
STORE_DATA:
		      LDS R16,USARTC0_DATA//�����ݼĴ���
		      ST  Z+,R16
              INC R18//�����ַ�����1
		      LDI R16,0X00//����������
		      STS TCC0_CNT,R16
		      STS TCC0_CNT+1,R16

WAITING_RECEIVE:
		      LDS R16,TCC0_CNT
		      CPI R16,5
              BRSH RECEIVE_END //���ռ������5�����ս���
              LDS R16,USARTC0_STATUS
              SBRS R16,7 //���������˾ʹ�
		      JMP WAITING_RECEIVE
	          JMP STORE_DATA
RECEIVE_END:
			  RET
SP_LoadFlashPage:
              IN R21,CPU_RAMPZ
			  LDI R16,0X00
              OUT   CPU_RAMPZ,R16 
	          CLR	ZL               
	          CLR	ZH              
                 
	          LDI   XH,0X20
			  LDI   XL,0X00        
              LDI 	r20, NVM_CMD_LOAD_FLASH_BUFFER_gc   
	          STS	NVM_CMD, r20                     
	          LDI	r19, CCP_SPM_gc  
			  MOV   R17,R18//Ҫд������ݸ���                  
SP_LoadFlashPage_1:
	          LD	R0, X+        
	          MOV   R1,R0     
	          STS	CPU_CCP, r19       
	          SPM                    
	          ADIW	ZL, 2          
              DEC R17
			  CLZ
			  CPI R17,0
              BRNE SP_LoadFlashPage_1   
			  OUT CPU_RAMPZ,R21
              CLR R1
	          RET
 
SP_EraseWriteApplicationPage:
	          IN	r21, CPU_RAMPZ
			  LDI R16,0X00  
			  OUT	CPU_RAMPZ,R16
	          LDI   ZH,0X50
			  LDI   ZL,0X00//FLASH��ַ 
			  OUT	CPU_RAMPZ,ZL
			  
	          LDI	r20, NVM_CMD_ERASE_WRITE_APP_PAGE_gc   
	          STS	NVM_CMD, r20    
			  LDI	r19, CCP_SPM_gc  
			  STS	CPU_CCP, r19
			  SPM
			  CLR R1
	          OUT	CPU_RAMPZ, r21             
	          RET

SP_WaitForSPM:
	          LDS	r16, NVM_STATUS     
	          SBRC	r16, NVM_NVMBUSY_bp  
	          RJMP	SP_WaitForSPM        
	          CLR	r16
	          STS	NVM_CMD, r16        
	          RET
SP_ReadWord:
	          IN  r21, CPU_RAMPZ 
			  LDI R16,0X00
			  OUT CPU_RAMPZ,R16
			  LDI ZH,0X50
			  LDI ZL,0X00//��flash�ĵ�ַ
			  LDI XH,0X30
			  LDI XL,0X00//SDRAM�д�Ŵ�flash���������ݵĵ�ַ   
			  LDI R19,0X00//����             
SP_ReadWord_1:         
	          ELPM	R24, Z 
			  ST X+,R24   
			  ADIW ZL,2    
			  INC R19
			  CLZ
			  CP R18,R19  
			  BRNE  SP_ReadWord_1       
	          OUT	CPU_RAMPZ, r21       
	          RET              
SEND_DATA:	
              LDI XH,0X30
			  LDI XL,0X00//SDRAM�д�Ŵ�flash���������ݵĵ�ַ
			  LDI R17,0X00//����		  
SEND_DATA_1:
			  LD R16,X+
              USART_IsTXDataRegisterEmpty USART_DREIF_bp
              STS USARTC0_DATA,R16
			  INC R17
              CLZ
			  CP R17,R18
			  BRNE SEND_DATA_1
			  RET
QUIT_BOOT:     
              LDI R17,CCP_IOREG_gc
	          LDI R16,0X00
			  STS CPU_CCP,R17
	          STS PMIC_CTRL,R16//���ж�������ᵽ������
              JMP 0X0000
			 
              
ISR:             
              CALL RECEIVE_DATA//���ڽ�������
			  CALL SP_LoadFlashPage//дflash������
			  CALL SP_EraseWriteApplicationPage//������дһҳ
			  CALL SP_WaitForSPM//�ȴ�д��
			  CALL SP_ReadWord//������
			  CALL SEND_DATA//���ڷ��ض�������
              LDI R16,0X01
			  STS GPIO_GPIOR0,R16//������1����ת��������
			  RETI
              
