;*************************************************************************************
;*  
;* File Name         : SPI_fm25v0_asm.asm
;* Title             : SPI读写FM25V10
;* Date              : 2010-7-12  21:36
;* Support E-mail    : jack198651@163.com
;* Target MCU        : ATxmega32A4
;*
;* DESCRIPTION       
;* 	   本程序使用SPID向FM25V10读写数据
;*
;*   |---XMEGA32A4---|---------FM25V10---------------|
;*   |---PD4---SS----|---1---/S---Chip Select--------|
;*   |---PD6---MISO--|---2----Q---Serial Data Output-|
;*   |---VCC---------|---3---/W---Write Protect------|
;*   |---GND---------|---4----VSS---------Ground-----|
;*   |---PD5---MOSI--|---5----D---Serial Data Input--|
;*   |---PD7---SCK---|---6----C---Serial Clock-------|
;*   |---VCC---------|---7---/HOLD--------HOLD-------|
;*   |---VCC---------|---8----VDD---Supply Voltage---|
;*   系统时钟选用默认时钟2MHZ
;* WREN  Set Write Enable Latch  0000 0110b 
;* WRDI  Write Disable  0000 0100b 
;* RDSR  Read Status Register  0000 0101b 
;* WRSR  Write Status Register  0000 0001b 
;* READ  Read Memory Data  0000 0011b 
;* FSTRD  Fast Read Memory Data  0000 1011b 
;* WRITE  Write Memory Data   0000 0010b 
;* SLEEP  Enter Sleep Mode  1011 1001b 
;* RDID  Read Device ID  1001 1111b 
;* SNR  Read S/N  1100 0011b
;发送写数据格式 ：F9(写标志)00 00 00（三字节地址） 02（要写数据个数） 01 02（要写的数据）
;发送读数据格式： FA(读标志)00 00 00 （要读的起始地址）02（要读的数据个数）
;*************************************************************************************

.include "ATxmega32A4def.inc"//器件配置文件,决不可少,不然汇编通不过

.ORG 0
  		RJMP RESET//复位 
.ORG 0X20
  		RJMP RESET//复位 （调试的时候全速运行[F5]会莫名进入中断向量表中该地址）

.ORG 0x032
		JMP USARTC0_INT_RXC//跳到串口C0接收完毕中断子程序

.ORG 0X100        //跳过中断区0x00-0x0F4


.MACRO CLKSYS_IsReady
 CLKSYS_IsReady_1:
           LDS R16,OSC_STATUS
		   SBRS R16,@0
		   JMP CLKSYS_IsReady_1//等待外部振荡器准备好
		   NOP
.ENDMACRO 

PLL_XOSC_Initial:
	   LDI R16,0X10
	   STS PORTCFG_VPCTRLA,R16;PORTB映射到虚拟端口1，PORTA映射到虚拟端口0
	   LDI R16,0X32
	   STS PORTCFG_VPCTRLB,R16;PORTC映射到虚拟端口2，PORTD映射到虚拟端口3

	   LDI R16,0X4B
	   STS OSC_XOSCCTRL,R16//设置晶振范围 启动时间 

				   
       LDI R16,OSC_XOSCEN_bm
       STS OSC_CTRL,R16//使能外部振荡器

       CLKSYS_IsReady OSC_XOSCRDY_bp

	   LDI R16,OSC_PLLSRC_XOSC_gc
	   ORI R16,0XC3
       STS OSC_PLLCTRL,R16


	   LDS R16,OSC_CTRL//读取该寄存器的值到R16
	   SBR R16,OSC_PLLEN_bm//对PLLEN这一位置位，使能PLL
	   STS OSC_CTRL,R16

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


SPI_MasterInit: //SPI初始化
		SBI VPORT3_DIR,4 //SS片选引脚设为方向输出
		SBI VPORT3_DIR,5//MOSI引脚设为方向输出
		SBI VPORT3_DIR,7//SCK引脚设为方向输出

		LDI R16,0x50//0,1,0,1,00,00;SPI Clock Double DISABLE,SPI module Enable, Data Order=MSB，Master Select, SPI Mode=0, SPI Clock Prescaler=CLKper/4=0.5MHZ
        STS SPID_CTRL,R16//SPI控制寄存器
        LDI R16,0x00
        STS SPID_INTCTRL,R16//SPI 中断 关闭
		RET

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
	   /* 设置波特率19200*/
        LDI R16,77
	    STS USARTC0_BAUDCTRLA,R16
	    LDI R16,0
	    STS USARTC0_BAUDCTRLB,R16
 
    	//使能接收发送
		LDI R16,0X18
		STS USARTC0_CTRLB,R16//000:Reserved,1:接收使能,1:发送使能,0:波特率不加倍,0:单机模式，0:TXB8不使用
		//6.设置三个中断的中断级别（可以关闭中断）
		LDI R16,0X30
		STS USARTC0_CTRLA,R16//00:Reserved,11:接受完毕中断级别为高层中断,00:发送完毕中断关闭,01: TXBUFFER(DATA)空中断关闭
		//串口C0设置完毕（注意设置过程中全局中断需要关闭）
		RET 
RESET:
        CALL PLL_XOSC_Initial
		CALL SPI_MasterInit
		CALL uart_init
		SBI VPORT3_DIR,0//PORTD0输出
		CBI VPORT3_OUT,0//PORTD0输出低 使能485接收
		//指示灯
		SBI VPORT0_DIR,2//PORTA2输出
		SBI VPORT0_DIR,3//PORTA3输出
		SBI VPORT0_DIR,4//PORTA4输出

TimerC0:
		//2.selecting a clock source
		LDI R16,0X07//计数器时钟源为24MHZ/1024=23437.5Hz
		STS TCC0_CTRLA,R16

        LDI R16,PMIC_HILVLEN_bm + PMIC_MEDLVLEN_bm + PMIC_LOLVLEN_bm; 
		STS PMIC_CTRL,R16//可编程多层中断控制寄存器高 中 低层使能，循环调度关闭，中断向量未移至 Boot section	
  	    
        SEI//全局中断使能置位

LOOP:	
		NOP
		CBI VPORT3_OUT,0//PORTD0输出低 使能485接收
        JMP LOOP

///////////////////////////////////////////////////////////////////////////////////////////		
SPI_READ_DATA://读数据流程
		//发送操作码之前片选产生下降沿
		SBI VPORT3_OUT,4
		NOP
		CBI VPORT3_OUT,4
		//发送读操作码
		LDI R16,0X03 //READ 0X03
        STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//数据移位完成，写3个字节的地址
		CALL SPI_WRITE_ADDRESS
		LD  R17,Y+
LOOP_READ_DATA:
		LDI  R16,0X00
		STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//读入数据移位完成,取出数据
		LDS R16,SPID_DATA
		ST Y+,R16
		//计数减1，读下一个字节的数据
		DEC R17
		BRNE LOOP_READ_DATA
		SBI VPORT3_OUT,4//读操作完成，片选SS拉高
		RET

///////////////////////////////////////////////////////////////////////////////////////////		
SPI_WRITE_DATA://写数据流程
		//发送操作码之前片选产生下降沿
		SBI VPORT3_OUT,4
		NOP
		CBI VPORT3_OUT,4
     	//发送写使能操作码
		LDI R16,0X06 //WREN 0X06
        STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		SBI VPORT3_OUT,4//发送写使能操作码 完成，片选SS拉高

		//发送操作码之前片选产生下降沿
		SBI VPORT3_OUT,4
		NOP
		CBI VPORT3_OUT,4
		//发送写操作码
		LDI R16,0X02 //WRITE 0X02
        STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//数据移位完成，写3个字节的地址
		CALL SPI_WRITE_ADDRESS

		LD  R17,Y+
LOOP_WRITE_DATA:
		LD  R16,Y+
		STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//数据移位完成，计数减1，写下一个字节的数据
		DEC R17
		BRNE LOOP_WRITE_DATA
		SBI VPORT3_OUT,4//写操作完成，片选SS拉高
		RET
///////////////////////////////////////////////////////////////////////////////////////////////////
//读写数据都要等待SPI移位完成标志位置位
SPI_WAITING_IF:
		LDS R16,SPID_STATUS
		SBRS R16,7//Skip if bit 7(IF) in SPID_STATUS set
		RJMP SPI_WAITING_IF//数据移位没有完成继续等待
		RET
///////////////////////////////////////////////////////////////////////////////////////////////////
//读写数据都要写三个字节的地址
SPI_WRITE_ADDRESS:
		LDI R17,0X03
LOOP_ADDRESS:
		LD  R16,X+
		STS SPID_DATA,R16
		CALL SPI_WAITING_IF
		//数据移位完成，计数减1，写下一个字节的地址
		DEC R17
		BRNE LOOP_ADDRESS
		RET
///////////////////////////////////////////////////////////////////////////////////////////////////
USARTC0_INT_RXC:
	    LDI R18,0X00//记录接收字节数
		LDI R31,0x20//串口数据起始储存地址Z=0x2000
		LDI R30,0x00
		SBI VPORT0_OUT,4//接收过程亮灯
STORE_DATA:
		LDS R16,USARTC0_DATA//读Data Register
		ST  Z+,R16
        INC R18//接收字符数加1
		LDI R16,0X00//计数器清零
		STS TCC0_CNT,R16
		STS TCC0_CNT+1,R16

WAITING_RECEIVE:
		LDS R16,TCC0_CNT
		CPI R16,20
        BRSH RECEIVE_END //接收间隔大于20，接收结束
        LDS R16,USARTC0_STATUS
        SBRS R16,7 //有数据来了就存
		JMP WAITING_RECEIVE
	    JMP STORE_DATA
		
RECEIVE_END:
		CBI VPORT0_OUT,4//接收结束灯灭
		//X指针赋地址
		LDI R27,0X20
		LDI R26,0X00
		LD  R16,X+
		CPI R16,0XF9 //自定义SPI写命令
		BREQ SPI_WRITE
		CPI R16,0XFA //自定义SPI读命令
		BREQ SPI_READ

SPI_WRITE:
		/*X指针指向的数据区存储三个字节的地址
		写的数据存在Y指针指向的数据区域，第1个字节表明字节数目，第2个字节及其后是需要写的数据*/
		SBI VPORT0_OUT,3//SPI开始写灯亮
		//X指针赋地址,SPI地址存储开始
		LDI R27,0X20
		LDI R26,0X01
		//Y指针赋地址,SPI存储数据开始地址
		LDI R29,0X20
		LDI R28,0X04
		CALL SPI_WRITE_DATA
		CBI VPORT0_OUT,3//SPI结束写灯灭

		RETI

SPI_READ:
		/*X指针指向的数据区存储三个字节的地址
		读的数据存在Y指针指向的数据区域，第1个字节事先指明读取的字节数目，
		第2个字节之后是存放读出的数据*/
		SBI VPORT0_OUT,2//SPI开始读灯亮
		//X指针赋地址,SPI读取地址
		LDI R27,0X20
		LDI R26,0X01
		//Y指针赋地址,SPI读取数据个数
		LDI R29,0X20
		LDI R28,0X04
		CALL SPI_READ_DATA
		CBI VPORT0_OUT,2//SPI结束读灯灭
		
		//Y指针赋地址,SPI读取数据个数
		LDI R29,0X20
		LDI R28,0X04
		LD  R25,Y+//数据个数
		SBI VPORT3_OUT,0//使能485发送,为数据可以发送到串口助手
		SBI VPORT0_OUT,4//发送过程亮灯
AA_C0:
        LD  R16,Y+
	    STS USARTC0_DATA,R16//串口C0发送 
BB_C0:		      
		LDS		R16,USARTC0_STATUS
		SBRS	R16,5
		RJMP	BB_C0	
		DEC 	R25
		CALL DELAY
		CPI 	R25,0//检查字节数是否全部发送完成
        BRNE AA_C0
		CBI VPORT0_OUT,4//接收结束灯灭
 	    RETI

DELAY:
        
	    LDI R18,0X55
TT:
        LDI R19,0XFF
HH:
        CPI R19,0
		DEC R19
		BRNE HH

	    DEC R18
        CPI R18,0
		BRNE TT
		RET	
 


