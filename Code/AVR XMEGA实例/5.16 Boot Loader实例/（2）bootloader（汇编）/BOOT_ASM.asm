/*
 * 工程名:
      BOOT_ASM
 * 版权:
     	中国石油大学嵌入式通信实验室,2011.
 * 版本修改:
 	 	2011-03-22 21:35
 * 作者:
 		jack198651@qq.com
  * MCU:
 		ATXmega128A1
 * 开发环境:
		AVR Studio 4.18 build 716
* 描述:
     	从串口发送数据，写入地址为0x2800（因为flash是按字为单位的）的应用程序区，然后再读出来会送到串口
		进入一次中断后，跳到程序区，执行代码，如果程序区为空则会再次执行boot区程序
		设置从boot区启动
*/
.include "ATxmega128A1def.inc";器件配置文件,决不可少,不然汇编通不过

.include "usart_driver.inc"
.ORG 0X10000       //设置从boot区启动
  		JMP RESET  //复位 
.ORG 0x10032        //USARTC0数据接收完毕中断入口
        JMP ISR
 
.ORG 0X10100       ;跳过中断区0x00-0x0F4

.EQU ENTER  ='\n'
.EQU NEWLINE='\r'
.EQU EQUE   ='='
.EQU ZERO   ='0'
.EQU A_ASSCI='A'



/*
+------------------------------------------------------------------------------
| Function    : uart_init
+------------------------------------------------------------------------------
| Description : 初始化 USARTC0
+------------------------------------------------------------------------------
*/
uart_init:

	       /* USARTC0 引脚方向设置*/
  	       /* PC3 (TXD0) 输出 */
           LDI R16,0X08
	       STS PORTC_DIRSET,R16
	       /* PC2 (RXD0) 输入 */
	       LDI R16,0X04
	       STS PORTC_DIRCLR,R16
	        /* USARTC0 模式 - 异步*/	/* USARTC0帧结构, 8 位数据位, 无校验, 1停止位 */
	       LDI R16,USART_CMODE_ASYNCHRONOUS_gc|USART_CHSIZE_8BIT_gc|USART_PMODE_DISABLED_gc
	       STS USARTC0_CTRLC,R16
	        /* 设置波特率 9600*/
           LDI R16,12
	       STS USARTC0_BAUDCTRLA,R16
	       LDI R16,0
	       STS USARTC0_BAUDCTRLB,R16
	        /* USARTC0 使能发送*//* USARTC0 使能接收*/
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
			 uart_putc //这里不能调用uart_puts_string因为这个过程里面使用了LPM指令

             LDI XH,0X10
	         EOR XL,XL
             STS TCC0_PER,XH
	         STS TCC0_PER+1,XL

             LDI R16,TC_CLKSEL_DIV1024_gc//串口超过一定时间认为已经接收完毕
             STS TCC0_CTRLA,R16

			 /*USARTC0 接收低级断级别*/
	         LDI R16,USART_RXCINTLVL_LO_gc
	         STS USARTC0_CTRLA,R16
	         /* Enable interrupts.*/
			 LDI R17,CCP_IOREG_gc
	         LDI R16,PMIC_LOLVLEN_bm|PMIC_IVSEL_bm
			 STS CPU_CCP,R17
	         STS PMIC_CTRL,R16//Enable Low_Level interrupts
 	         SEI
			 LDI R16,0X00
			 STS GPIO_GPIOR0,R16//如果变成1则跳转到程序区

 RESET_LOOP:
             LDS R16,GPIO_GPIOR0
			 CLZ
             CPI R16,1
	         BRNE  RESET_LOOP
			 NOP
			 LDI R16,0X00
			 STS GPIO_GPIOR0,R16//如果变成1则跳转到程序区	 
             JMP QUIT_BOOT
			 JMP RESET_LOOP//永远也不会到这一步

/*
+------------------------------------------------------------------------------
| Function    : ISR(USARTE1_RXC_vect)
+------------------------------------------------------------------------------
| Description : USARTC0接收中断函数 收到的数据发送回去USARTC0
+------------------------------------------------------------------------------
*/
RECEIVE_DATA: 
              LDI R18,0X00//记录接收字节数,其他地方不能使用以免破坏数据
		      LDI R31,0x20//串口数据起始储存地址Z=0x2000
		      LDI R30,0x00
STORE_DATA:
		      LDS R16,USARTC0_DATA//读数据寄存器
		      ST  Z+,R16
              INC R18//接收字符数加1
		      LDI R16,0X00//计数器清零
		      STS TCC0_CNT,R16
		      STS TCC0_CNT+1,R16

WAITING_RECEIVE:
		      LDS R16,TCC0_CNT
		      CPI R16,5
              BRSH RECEIVE_END //接收间隔大于5，接收结束
              LDS R16,USARTC0_STATUS
              SBRS R16,7 //有数据来了就存
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
			  MOV   R17,R18//要写入的数据个数                  
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
			  LDI   ZL,0X00//FLASH地址 
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
			  LDI ZL,0X00//读flash的地址
			  LDI XH,0X30
			  LDI XL,0X00//SDRAM中存放从flash读出来数据的地址   
			  LDI R19,0X00//计数             
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
			  LDI XL,0X00//SDRAM中存放从flash读出来数据的地址
			  LDI R17,0X00//计数		  
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
	          STS PMIC_CTRL,R16//将中断向量表搬到程序区
              JMP 0X0000
			 
              
ISR:             
              CALL RECEIVE_DATA//串口接收数据
			  CALL SP_LoadFlashPage//写flash缓冲区
			  CALL SP_EraseWriteApplicationPage//擦除并写一页
			  CALL SP_WaitForSPM//等待写完
			  CALL SP_ReadWord//读数据
			  CALL SEND_DATA//串口返回读的数据
              LDI R16,0X01
			  STS GPIO_GPIOR0,R16//如果变成1则跳转到程序区
			  RETI
              
