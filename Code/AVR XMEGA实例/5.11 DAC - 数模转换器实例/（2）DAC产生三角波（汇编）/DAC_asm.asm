/*
 * 工程名:
     DAC
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-03-03 09:03
 * 作者:
 		jack198651@qq.com
 * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
 * 描述:
 		使用通道0产生锯齿波

*/
 		 
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过

.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）

.ORG 0X100       ;跳过中断区0x00-0x0FF
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
