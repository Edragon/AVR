/*
 * ������:
     DAC
 * ��Ȩ:
     	�й�ʯ�ʹ�ѧǶ��ʽͨ��ʵ����,2011.
 * �汾�޸�:
 	 	2011-03-03 09:03
 * ����:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * ��������:
		AVR Studio 4.18 build 716
 * ����:
 		ʹ��ͨ��0������ݲ�

*/
 		 
.include "ATxmega128A1def.inc";���������ļ�,��������,��Ȼ���ͨ����

.ORG 0
  		RJMP RESET//��λ 
.ORG 0x20
  		RJMP RESET//��λ ������ʱȫ������ʱ��Ī��������жϣ��˴�ʹ����ת��RESET��

.ORG 0X100       ;�����ж���0x00-0x0FF
/*
+------------------------------------------------------------------------------
| Function    : main
+------------------------------------------------------------------------------
*/
RESET:

	ldi R16,DAC_CHSEL_SINGLE_gc
	sts DACB_CTRLB,R16 
	ldi R16,~( DAC_REFSEL_gm | DAC_LEFTADJ_bm )|DAC_REFSEL_INT1V_gc
	sts DACB_CTRLC,R16 
	ldi r16,DAC_CONINTVAL_4CLK_gc|DAC_REFRESH_32CLK_gc
	sts DACB_TIMCTRL,r16
	ldi r16,DAC_CH0EN_bm | DAC_ENABLE_bm
	sts DACB_CTRLA,r16 

	ldi xh,0xff
RESET_1: 

     lds r16,DACB_STATUS
	 SBRS R16,DAC_CH0DRE_bp
	 jmp RESET_1
	 nop
     sts DACB_CH0DATA,xh
	 dec xh  
     jmp RESET_1
