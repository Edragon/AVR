/*
 * 工程名:
     CLK_OSC
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-03-19 14:33
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	本程序给出XMEGA时钟配置方案
*/

.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过

.ORG 0
  		RJMP RESET//复位 
.ORG 0x20
  		RJMP RESET//复位 （调试时全速运行时会莫名进入该中断，此处使其跳转到RESET）
.ORG 0x01C
        RJMP ISR
.ORG 0X100       ;跳过中断区0x00-0x0F4

/*
+------------------------------------------------------------------------------
| Function    : LED
+------------------------------------------------------------------------------
| Description : 开关灯
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
		   JMP CLKSYS_IsReady_1//等待外部振荡器准备好
		   NOP
.ENDMACRO 
/*
+------------------------------------------------------------------------------
| Function    : Clk_OSC_Setting
+------------------------------------------------------------------------------
| Description : 时钟设置函数
| Parameters  : CLK_SCLKSEL:
|    				 	      CLK_SCLKSEL_RC2M_gc = (0x00<<0), 	 Internal 2MHz RC Oscillator 
|   						  CLK_SCLKSEL_RC32M_gc = (0x01<<0),  Internal 32MHz RC Oscillator 
|   						  CLK_SCLKSEL_RC32K_gc = (0x02<<0),	 Internal 32kHz RC Oscillator 
|   						  CLK_SCLKSEL_XOSC_gc = (0x03<<0), 	 External Crystal Oscillator or Clock 
|   						  CLK_SCLKSEL_PLL_gc = (0x04<<0),  	 Phase Locked Loop
|				OSC_PLLSRC:
|							  OSC_PLLSRC_RC2M_gc = (0x00<<6),  	Internal 2MHz RC Oscillator   测试factor<=25 稳定
|							  0代表不使用锁相环
|							  OSC_PLLSRC_RC32M_gc = (0x02<<6),  Internal 32MHz RC Oscillator 4分频 测试factor<=6 稳定
|							  OSC_PLLSRC_XOSC_gc = (0x03<<6),   外部时钟，若作为输入源最小为0.4MHz
|							  
|				factor:		 倍频因子的范围在1x和31x之间。输出频率不能超过200MHz。PLL最小输出频率为10MHz。
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
				  STS OSC_XOSCCTRL,R16//设置晶振范围 启动时间 

				   
                  LDI R16,OSC_XOSCEN_bm
                  STS OSC_CTRL,R16//使能外部振荡器
/*CLKSYS_IsReady_1:
                  LDS R16,OSC_STATUS
		          SBRS R16,OSC_XOSCRDY_bp
		          JMP CLKSYS_IsReady_1//等待外部振荡器准备好
				  NOP*/
                  CLKSYS_IsReady OSC_XOSCRDY_bp

				  LDI R16,OSC_PLLSRC_XOSC_gc
				  ORI R16,0XC6
                  STS OSC_PLLCTRL,R16


				  LDS R16,OSC_CTRL//读取该寄存器的值到R16
		          SBR R16,OSC_PLLEN_bm//对PLLEN这一位置位，使能PLL
		          STS OSC_CTRL,R16

/*CLKSYS_IsReady_2:
                  LDS R16,OSC_STATUS
		          SBRS R16,OSC_PLLRDY_bp
		          JMP CLKSYS_IsReady_2//等待外部振荡器准备好
				  NOP*/
                  CLKSYS_IsReady OSC_PLLRDY_bp

				  LDI R16,CLK_SCLKSEL_PLL_gc
				  LDI R17,0XD8//密钥
		          STS CPU_CCP,R17//解锁
				  STS CLK_CTRL,R16//选择系统时钟源

				  LDI R16,CLK_PSADIV_1_gc
				  ORI R16,CLK_PSBCDIV_1_1_gc
                  LDI R17,0XD8//密钥
		          STS CPU_CCP,R17//解锁
				  STS CLK_PSCTRL,R16//设置预分频器A,B,C的值


				  RET

RC32M_Initial:    
                   
                  LDI R16,OSC_RC32MEN_bm
                  STS OSC_CTRL,R16//使能RC32M振荡器

	              CLKSYS_IsReady OSC_RC32MRDY_bm//等待RC32M振荡器准备好
                  
				  LDI R16, CLK_SCLKSEL_RC32M_gc
				  LDI R17,0XD8//密钥
	           	  STS CPU_CCP,R17//解锁
				  STS CLK_CTRL,R16//选择系统时钟源
                   
				  LDI R16,CLK_PSADIV_1_gc
				  ORI R16,CLK_PSBCDIV_1_1_gc
				  LDI R17,0XD8//密钥
		          STS CPU_CCP,R17//解锁
				  STS CLK_PSCTRL,R16//设置预分频器A,B,C的值

				  RET
RC2M_Initial:     
                  
                  LDI R16,OSC_RC2MEN_bm 
                  STS OSC_CTRL,R16// 使能RC2M振荡器
                  
                  CLKSYS_IsReady OSC_RC2MRDY_bm//等待RC2M振荡器准备好
                  
				  LDI R17,0XD8//密钥
	              STS CPU_CCP,R17//解锁
				  LDI R16, CLK_SCLKSEL_RC2M_gc
				  STS CLK_CTRL,R16//选择系统时钟源
 
	              LDI R17,0XD8//密钥
		          STS CPU_CCP,R17//解锁
                  LDI R16,CLK_PSADIV_1_gc
				  ORI R16,CLK_PSBCDIV_1_1_gc
				  STS CLK_PSCTRL,R16//设置预分频器A,B,C的值

				  RET
	
RESET:            
                  
                  // PLL，外部晶振8M，输出8M*6=48M
	              CALL PLL_XOSC_Initial
	  
	              //内部RC32M
	              //RC32M_Initial();

	              //内部RC2M
	              //RC2M_Initial();
                  LDI R16,0X30
	              STS PORTD_DIRSET,R16;//PD5，PD4方向设为输出

                /* 设置定时器C0，计数周期65535
	              * 使能溢出中断
	              */
				  LDI R16,0X0FF
	              STS TCC0_PER,R16
				  STS TCC0_PER+1,R16
				  LDI R16,TC_CLKSEL_DIV64_gc
	              STS TCC0_CTRLA ,R16
                  LDI R16,TC_OVFINTLVL_MED_gc
				  STS TCC0_INTCTRLA,R16

               	  /* 使能高级别中断，打开全局中断*/
				  LDI R16,PMIC_HILVLEN_bp
	              STS PMIC_CTRL,R16
	              SEI

RESET_LOOP: 
                  RJMP RESET_LOOP

ISR:              LED_T1 0X20
                  LED_T2 0X10
                  
                  RETI
                   
