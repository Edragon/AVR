/*
 * ������:
     LED_Blinking
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-03-15 21:50
 * ����:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
* ����:
     	������ʹ�˿��ϵ�LED�����������˸.
     	ʱ��:�ڲ�ʱ��2MHz���ϵ��ʼʱ�ӣ�
		Ӳ�����ӣ�
				LED1  -->   PD5
				LED2  -->   PD4
*/
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����

.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��
.ORG 0X100       ;�����ж���0x00-0x0F4
RESET:
             LDI R16,0x30
		     STS PORTD_DIR,R16  ;PD5��PD4������Ϊ���

REST_LOOP:
             LDI R16,0x30
             STS PORTD_OUTCLR,R16
	         LDI R17,200         
		     CALL _delay_ms
		     LDI R16,0x30
             STS PORTD_OUTSET,R16
		     LDI R17,200
		     CALL _delay_ms
		     RJMP REST_LOOP
		 


_delay_ms:
L0:            LDI R18,250
L1:            DEC R18 
               BRNE L1
		       DEC R17
		       BRNE L0
		       RET


