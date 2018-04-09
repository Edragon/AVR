/*
 * 工程名:
     LED_Blinking
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-03-15 21:50
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序使端口上的LED发光二极管闪烁.
     	时钟:内部时钟2MHz（上电初始时钟）
		硬件连接：
				LED1  -->   PD5
				LED2  -->   PD4
*/
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过

.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
.ORG 0X100       ;跳过中断区0x00-0x0F4
RESET:
             LDI R16,0x30
		     STS PORTD_DIR,R16  ;PD5，PD4方向设为输出

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


