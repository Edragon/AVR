/*
 * ������:
     LED_KEY
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-02-09 14:33
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	������������ά��������������LED�������������ð�����Ӧ�Ķ˿�Ϊ���룬
		��ʹ���������裻����LED��Ӧ�Ķ˿�Ϊ�������ʼ����LEDΪ����
		�������ж��û��������룬�����
		���м��ȷ����   --  LED1��LED2�˿�ȡ����LED1��LED2���濪�� ��
		�����           --  LED1�� ��
		���Ҽ�           --  LED1�� ��
		���ϼ�           --  LED2�� ��
		���¼�           --  LED2�� ��

     	ʱ��:�ڲ�ʱ��2MHz���ϵ��ʼʱ�ӣ�
		Ӳ�����ӣ�
				LED1  -->   PD5
				LED2  -->   PD4
				LEFT_KEY    --> PE0
				SELECT_KEY  --> PE1
				UP_KEY      --> PE2
				DOWN_KEY    --> PE3
				RIGHT_KEY   --> PE4
*/
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����

.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
.ORG 0X100       ;�����ж���0x00-0x0F4
//--------��������ֵ-------
.EQU No_key=0x00
.EQU SELECT=0x01
.EQU LEFT=0x02
.EQU RIGHT=0x04
.EQU UP=0x08
.EQU DOWN=0x10



/*
+------------------------------------------------------------------------------
| Function    : KEY_initial
+------------------------------------------------------------------------------
| Description : ���ð�������Ϊ����
+------------------------------------------------------------------------------
*/       
KEY_initial:
            LDI R16,0X1F
            STS PORTE_DIRCLR,R16;//���ð�������Ϊ����
            
			/*
	        PORTE_PIN0CTRL = PORT_OPC_PULLUP_gc;
	        PORTE_PIN1CTRL = PORT_OPC_PULLUP_gc;
	        PORTE_PIN2CTRL = PORT_OPC_PULLUP_gc;
	        PORTE_PIN3CTRL = PORT_OPC_PULLUP_gc;
	        PORTE_PIN4CTRL = PORT_OPC_PULLUP_gc;
	        */
	        //���ж�����ŵ�������ͬʱ������ʹ�ö�������������Ĵ���һ�����ö������
            LDI R16,0X1F
	        STS PORTCFG_MPCMASK,R16

			LDI R16,PORT_OPC_PULLUP_gc
            STS PORTE_PIN0CTRL,R16
			RET 
/*
+------------------------------------------------------------------------------
| Function    : Get_Key
+------------------------------------------------------------------------------
| Description : ȷ����ǰ���µļ�
| Parameters  : 
| Returns     : ���ؼ�ֵ�����������������൱��û�м����£�����0��
+------------------------------------------------------------------------------
*/
Get_Key:    
            EOR R18,R18
			EOR R17,R17
            LDS R16,PORTE_IN
			MOV R19,R16  
			ANDI R19,0X01
			SBRS R19,0
            JMP Get_Key_1
Get_Key_11:
			MOV R19,R16
			ANDI R19,0X02
		    SBRS R19,1
            JMP Get_Key_2
Get_Key_22:
			MOV R19,R16
            ANDI R19,0X04
			SBRS R19,2
            JMP Get_Key_3
Get_Key_33:
			MOV R19,R16
            ANDI R19,0X08
			SBRS R19,3
            JMP Get_Key_4
Get_Key_44:
			MOV R19,R16
            ANDI R19,0X10
			SBRS R19,4
            JMP Get_Key_5
			NOP
			JMP Get_Key_6
Get_Key_1:
         LDI R17,LEFT
		 INC R18
		 JMP Get_Key_11

Get_Key_2:
         LDI R17,SELECT
		 INC R18
		 JMP Get_Key_22
Get_Key_3:
         LDI R17,UP
		 INC R18
		 JMP Get_Key_33
Get_Key_4:
         LDI R17,DOWN
		 INC R18
		 JMP Get_Key_44
Get_Key_5:
         LDI R17,RIGHT
		 INC R18     
Get_Key_6:
         CLZ
		 CPI R18,1
		 BREQ Get_Key_END
		 LDI R17,No_key
Get_Key_END:
         RET
/*
+------------------------------------------------------------------------------
| Function    : LED
+------------------------------------------------------------------------------
| Description : ���ص�
| Parameters  : 
| Returns     :
+------------------------------------------------------------------------------
*/
         
.MACRO LED1_ON
           LDI R16,@0
           STS PORTD_OUTSET,R16
.ENDMACRO
.MACRO LED2_ON
           LDI R16,@0
           STS PORTD_OUTSET,R16
.ENDMACRO    
.MACRO LED1_OFF
           LDI R16,@0
           STS PORTD_OUTCLR,R16
.ENDMACRO
.MACRO LED2_OFF
           LDI R16,@0
           STS PORTD_OUTCLR,R16
.ENDMACRO
.MACRO LED1_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO
.MACRO LED2_T
           LDI R16,@0
           STS PORTD_OUTTGL,R16
.ENDMACRO

/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
| Description : ���ݰ�����LED�Ʋ���
+------------------------------------------------------------------------------
*/
RESET:
          LDI R16,0x30
          STS PORTD_DIRSET,R16;//PD5��PD4������Ϊ���
		  LED1_ON 0X20
	      LED2_ON 0X10
	      CALL KEY_initial
RESET_LOOP:	      
          CALL  Get_Key   
          CLZ 
		  CPI R17,0
		  BREQ RESET_END
          CLZ 
		  CPI R17,SELECT
		  BREQ RESET_1
		  CPI R17,LEFT
		  BREQ RESET_2
		  CPI R17,RIGHT
		  BREQ RESET_3
		  CPI R17,UP
		  BREQ RESET_4
		  CPI R17,DOWN 
		  BREQ RESET_5
          LED2_OFF 0X10
		  LDI R17,0
		  JMP RESET_END

RESET_1:  
          LED1_T 0X20
	      LED2_T 0X10
		  LDI R17,0
		  JMP RESET_END
RESET_2:  
          LED1_ON 0X20
		  LDI R17,0
		  JMP RESET_END
RESET_3:  
          LED1_OFF 0X20
		  LDI R17,0
		  JMP RESET_END
RESET_4:  
	      LED2_ON 0X10
		  LDI R17,0
		  JMP RESET_END
RESET_5:  
	      LED2_OFF 0X10
		  LDI R17,0
		  JMP RESET_END             
RESET_END:	
          JMP RESET_LOOP	  
