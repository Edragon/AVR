/*
 * ������:
     CLK_OSC
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-03-19 14:33
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	���������XMEGAʱ�����÷���
*/

.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����

.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
.ORG 0x01C
        RJMP ISR
.ORG 0X100       ;�����ж���0x00-0x0F4

/*
+------------------------------------------------------------------------------
| Function    : LED
+------------------------------------------------------------------------------
| Description : ���ص�
| Parameters  : 
| Returns     :
+------------------------------------------------------------------------------
*/
.MACRO LED_T1
           LDI R16,@0
           STS PORTD_OUTTGL,R16
           
.ENDMACRO
.MACRO LED_T2
           LDI R16,@0
           STS PORTD_OUTTGL,R16
           
.ENDMACRO



.MACRO CLKSYS_IsReady
 CLKSYS_IsReady_1:
           LDS R16,OSC_STATUS
		   SBRS R16,@0
		   JMP CLKSYS_IsReady_1//�ȴ��ⲿ����׼����
		   NOP
.ENDMACRO 
/*
+------------------------------------------------------------------------------
| Function    : Clk_OSC_Setting
+------------------------------------------------------------------------------
| Description : ʱ�����ú���
| Parameters  : CLK_SCLKSEL:
|    				 	      CLK_SCLKSEL_RC2M_gc = (0x00<<0), 	 Internal 2MHz RC Oscillator 
|   						  CLK_SCLKSEL_RC32M_gc = (0x01<<0),  Internal 32MHz RC Oscillator 
|   						  CLK_SCLKSEL_RC32K_gc = (0x02<<0),	 Internal 32kHz RC Oscillator 
|   						  CLK_SCLKSEL_XOSC_gc = (0x03<<0), 	 External Crystal Oscillator or Clock 
|   						  CLK_SCLKSEL_PLL_gc = (0x04<<0),  	 Phase Locked Loop
|				OSC_PLLSRC:
|							  OSC_PLLSRC_RC2M_gc = (0x00<<6),  	Internal 2MHz RC Oscillator   ����factor<=25 �ȶ�
|							  0����ʹ�����໷
|							  OSC_PLLSRC_RC32M_gc = (0x02<<6),  Internal 32MHz RC Oscillator 4��Ƶ ����factor<=6 �ȶ�
|							  OSC_PLLSRC_XOSC_gc = (0x03<<6),   �ⲿʱ�ӣ�����Ϊ����Դ��СΪ0.4MHz
|							  
|				factor:		 ��Ƶ���ӵķ�Χ��1x��31x֮�䡣���Ƶ�ʲ��ܳ���200MHz��PLL��С���Ƶ��Ϊ10MHz��
|				CLK_PSADIV:
|							CLK_PSADIV_1_gc = (0x00<<2),    Divide by 1  
|						    CLK_PSADIV_2_gc = (0x01<<2),    Divide by 2  
|						    CLK_PSADIV_4_gc = (0x03<<2),    Divide by 4  
|						    CLK_PSADIV_8_gc = (0x05<<2),    Divide by 8  
|						    CLK_PSADIV_16_gc = (0x07<<2),   Divide by 16  
|						    CLK_PSADIV_32_gc = (0x09<<2),   Divide by 32 
|						    CLK_PSADIV_64_gc = (0x0B<<2),   Divide by 64 
|						    CLK_PSADIV_128_gc = (0x0D<<2),  Divide by 128  
|						    CLK_PSADIV_256_gc = (0x0F<<2),  Divide by 256 
|						    CLK_PSADIV_512_gc = (0x11<<2),  Divide by 512 
|				CLK_PSBCDIV:
|						|	CLK_PSBCDIV_1_1_gc = (0x00<<0),  Divide B by 1 and C by 1 
|						    CLK_PSBCDIV_1_2_gc = (0x01<<0),  Divide B by 1 and C by 2 
|						    CLK_PSBCDIV_4_1_gc = (0x02<<0),  Divide B by 4 and C by 1 
|						    CLK_PSBCDIV_2_2_gc = (0x03<<0),   Divide B by 2 and C by 2 
|
|
+------------------------------------------------------------------------------
*/
PLL_XOSC_Initial:
                  LDI R16,0X4B
				  STS OSC_XOSCCTRL,R16//���þ���Χ ����ʱ�� 

				   
                  LDI R16,OSC_XOSCEN_bm
                  STS OSC_CTRL,R16//ʹ���ⲿ����
/*CLKSYS_IsReady_1:
                  LDS R16,OSC_STATUS
		          SBRS R16,OSC_XOSCRDY_bp
		          JMP CLKSYS_IsReady_1//�ȴ��ⲿ����׼����
				  NOP*/
                  CLKSYS_IsReady OSC_XOSCRDY_bp

				  LDI R16,OSC_PLLSRC_XOSC_gc
				  ORI R16,0XC6
                  STS OSC_PLLCTRL,R16


				  LDS R16,OSC_CTRL//��ȡ�üĴ�����ֵ��R16
		          SBR R16,OSC_PLLEN_bm//��PLLEN��һλ��λ��ʹ��PLL
		          STS OSC_CTRL,R16

/*CLKSYS_IsReady_2:
                  LDS R16,OSC_STATUS
		          SBRS R16,OSC_PLLRDY_bp
		          JMP CLKSYS_IsReady_2//�ȴ��ⲿ����׼����
				  NOP*/
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

RC32M_Initial:    
                   
                  LDI R16,OSC_RC32MEN_bm
                  STS OSC_CTRL,R16//ʹ��RC32M����

	              CLKSYS_IsReady OSC_RC32MRDY_bm//�ȴ�RC32M����׼����
                  
				  LDI R16, CLK_SCLKSEL_RC32M_gc
				  LDI R17,0XD8//��Կ
	           	  STS CPU_CCP,R17//����
				  STS CLK_CTRL,R16//ѡ��ϵͳʱ��Դ
                   
				  LDI R16,CLK_PSADIV_1_gc
				  ORI R16,CLK_PSBCDIV_1_1_gc
				  LDI R17,0XD8//��Կ
		          STS CPU_CCP,R17//����
				  STS CLK_PSCTRL,R16//����Ԥ��Ƶ��A,B,C��ֵ

				  RET
RC2M_Initial:     
                  
                  LDI R16,OSC_RC2MEN_bm 
                  STS OSC_CTRL,R16// ʹ��RC2M����
                  
                  CLKSYS_IsReady OSC_RC2MRDY_bm//�ȴ�RC2M����׼����
                  
				  LDI R17,0XD8//��Կ
	              STS CPU_CCP,R17//����
				  LDI R16, CLK_SCLKSEL_RC2M_gc
				  STS CLK_CTRL,R16//ѡ��ϵͳʱ��Դ
 
	              LDI R17,0XD8//��Կ
		          STS CPU_CCP,R17//����
                  LDI R16,CLK_PSADIV_1_gc
				  ORI R16,CLK_PSBCDIV_1_1_gc
				  STS CLK_PSCTRL,R16//����Ԥ��Ƶ��A,B,C��ֵ

				  RET
	
RESET:            
                  
                  // PLL���ⲿ����8M�����8M*6=48M
	              CALL PLL_XOSC_Initial
	  
	              //�ڲ�RC32M
	              //RC32M_Initial();

	              //�ڲ�RC2M
	              //RC2M_Initial();
                  LDI R16,0X30
	              STS PORTD_DIRSET,R16;//PD5��PD4������Ϊ���

                /* ���ö�ʱ��C0����������65535
	              * ʹ������ж�
	              */
				  LDI R16,0X0FF
	              STS TCC0_PER,R16
				  STS TCC0_PER+1,R16
				  LDI R16,TC_CLKSEL_DIV64_gc
	              STS TCC0_CTRLA ,R16
                  LDI R16,TC_OVFINTLVL_MED_gc
				  STS TCC0_INTCTRLA,R16

               	  /* ʹ�ܸ߼����жϣ���ȫ���ж�*/
				  LDI R16,PMIC_HILVLEN_bp
	              STS PMIC_CTRL,R16
	              SEI

RESET_LOOP: 
                  RJMP RESET_LOOP

ISR:              LED_T1 0X20
                  LED_T2 0X10
                  
                  RETI
                   
