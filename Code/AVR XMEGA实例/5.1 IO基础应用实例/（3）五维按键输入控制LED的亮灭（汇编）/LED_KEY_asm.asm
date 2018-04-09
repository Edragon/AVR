/*
 * 工程名:
     LED_KEY
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-02-09 14:33
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序利用五维按键输入来控制LED的亮灭，首先设置按键对应的端口为输入，
		并使能上拉电阻；配置LED对应的端口为输出，初始两个LED为开；
		主程序判断用户按键输入，如果：
		按中间的确定键   --  LED1、LED2端口取反，LED1、LED2交替开关 ；
		按左键           --  LED1开 ；
		按右键           --  LED1关 ；
		按上键           --  LED2开 ；
		按下键           --  LED2关 ；

     	时钟:内部时钟2MHz（上电初始时钟）
		硬件连接：
				LED1  -->   PD5
				LED2  -->   PD4
				LEFT_KEY    --> PE0
				SELECT_KEY  --> PE1
				UP_KEY      --> PE2
				DOWN_KEY    --> PE3
				RIGHT_KEY   --> PE4
*/
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过

.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
.ORG 0X100       ;跳过中断区0x00-0x0F4
//--------按键返回值-------
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
| Description : 设置按键引脚为上拉
+------------------------------------------------------------------------------
*/       
KEY_initial:
            LDI R16,0X1F
            STS PORTE_DIRCLR,R16;//设置按键引脚为输入
            
			/*
	        PORTE_PIN0CTRL = PORT_OPC_PULLUP_gc;
	        PORTE_PIN1CTRL = PORT_OPC_PULLUP_gc;
	        PORTE_PIN2CTRL = PORT_OPC_PULLUP_gc;
	        PORTE_PIN3CTRL = PORT_OPC_PULLUP_gc;
	        PORTE_PIN4CTRL = PORT_OPC_PULLUP_gc;
	        */
	        //当有多个引脚的配置相同时，可以使用多引脚配置掩码寄存器一次配置多个引脚
            LDI R16,0X1F
	        STS PORTCFG_MPCMASK,R16

			LDI R16,PORT_OPC_PULLUP_gc
            STS PORTE_PIN0CTRL,R16
			RET 
/*
+------------------------------------------------------------------------------
| Function    : Get_Key
+------------------------------------------------------------------------------
| Description : 确定当前按下的键
| Parameters  : 
| Returns     : 返回键值（如果多个按键按下相当于没有键按下，返回0）
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
| Description : 开关灯
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
| Description : 根据按键对LED灯操作
+------------------------------------------------------------------------------
*/
RESET:
          LDI R16,0x30
          STS PORTD_DIRSET,R16;//PD5，PD4方向设为输出
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
