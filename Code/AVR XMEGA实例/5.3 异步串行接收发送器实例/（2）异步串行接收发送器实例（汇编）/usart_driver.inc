
/*
+------------------------------------------------------------------------------
| Function    : USART_IsTXDataRegisterEmpty
+------------------------------------------------------------------------------
| Description : check bit 
|				
| Parameters  :  0—7中的一位
| Returns     : 
|
+------------------------------------------------------------------------------
*/
.MACRO USART_IsTXDataRegisterEmpty
          push r16
USART_IsTXDataRegisterEmpty_1:
          LDS  R16,USARTC0_STATUS
          SBRS R16,@0
		  JMP  USART_IsTXDataRegisterEmpty_1
		  NOP
		  pop r16
.ENDMACRO
/*
+------------------------------------------------------------------------------
| Function    : uart_putc
+------------------------------------------------------------------------------
| Description : 输出字符
|				
| Parameters  :  字符的ascii码入栈 
| Returns     : 
|
+------------------------------------------------------------------------------
*/
.MACRO uart_putc
           POP R17
		   CLZ
		   CPI R17,ENTER
		   BRNE uart_putc_1
		   USART_IsTXDataRegisterEmpty USART_DREIF_bp
		   STS USARTC0_DATA,R17
		   USART_IsTXDataRegisterEmpty USART_DREIF_bp
           LDI R17,NEWLINE
		   STS USARTC0_DATA,R17
uart_putc_1:
		   USART_IsTXDataRegisterEmpty USART_DREIF_bp
           STS USARTC0_DATA,R17
.ENDMACRO   

/*
+------------------------------------------------------------------------------
| Function    : uart_putc_hex
+------------------------------------------------------------------------------
| Description : 一个字节二进制转换成十六进制表示
|				
| Parameters  :  一个字节 传入XH中
| Returns     : 
|
+------------------------------------------------------------------------------
*/

.MACRO uart_putc_hex
 /* upper nibble */
          push r16
          push r17
		  push r18
		  push r19
		  push xh

          MOV R19,XH
		  SWAP R19
		  MOV  R17,R19
		  ANDI R17,0X0F
		  CLZ 
		  CPI  R17,0X0a
          BRSH uart_putc_hex_1
		  LDI R18,ZERO
          ADD R17,R18
          PUSH R17
		  uart_putc
          JMP uart_putc_hex_3
uart_putc_hex_1:		    
          SUBI R17,0X0a
		  LDI R18,A_ASSCI
		  ADD R17,R18
		  PUSH R17
		  uart_putc 
 /* lower nibble */
uart_putc_hex_3:
		  SWAP R19
		  MOV  R17,R19
		  ANDI R17,0X0F
		  CLZ 
		  CPI  R17,0X0a
          BRSH uart_putc_hex_2
		  LDI R18,ZERO
          ADD R17,R18
		  PUSH R17
		  uart_putc
		  JMP uart_putc_hex_end
uart_putc_hex_2:
		  SUBI R17,0X0a
		  LDI R18,A_ASSCI
		  ADD R17,R18
		  PUSH R17
		  uart_putc
uart_putc_hex_end:
          pop xh
		  pop r19
		  pop r18
		  pop r17
		  pop r16
.ENDMACRO 
/*
+------------------------------------------------------------------------------
| Function    : uart_putw_hex
+------------------------------------------------------------------------------
| Description : 一个字二进制转换成十六进制表示
|				
| Parameters  :  一个字 传入XH:XL中
| Returns     : 
|
+------------------------------------------------------------------------------
*/
.MACRO uart_putw_hex  
          push xh
		  push xl
          uart_putc_hex 
		  MOV XH,XL
		  uart_putc_hex 
		  pop xl
		  pop xh
.ENDMACRO   

/*
+------------------------------------------------------------------------------
| Function    : uart_putw_dec
+------------------------------------------------------------------------------
| Description : 一个字节或一个字二进制转换成十进制表示
|				
| Parameters  :  一个字节或一个字 放在XH:XL中传入
| Returns     : 
|
+------------------------------------------------------------------------------
*/
.MACRO     uart_putw_dec 
           push xh
		   push xl
		   push r16
		   push r17
		   push r18
		   push r19
		   push r20
		   push yh
		   push yl
		   push zh
		   push zl

           LDI YL,0X0F//整除10 ZH：ZL里面是商
           push yl
		   EOR YH,YH
		   EOR YL,YL
		   EOR ZH,ZH
		   EOR ZL,ZL
		   EOR R17,R17
		   EOR R19,R19


           
           ADIW YH:YL,10
DIVISION_1:		  
           CLZ
		   CP XL,YL
		   CPC XH,YH
		   BRLO DIVISION_2
           ORI R17,1
           SBIW XH:XL,10
		   ADIW ZH:ZL,1
	       JMP DIVISION_1 
DIVISION_2: 
           push xl
		   CLZ
		   CPI R17,1
		   BRNE DIVISION_3    
		   EOR XH,XH
		   EOR XL,XL
		   MOVW XH:XL,ZH:ZL
		   EOR ZH,ZH
		   EOR ZL,ZL
		   EOR R17,R17
		   JMP DIVISION_1 
		   
DIVISION_3:  
		   pop r16
		   CLZ
		   CPI R16,0X0F
		   BREQ DIVISION_4
		   LDI R18,ZERO
           ADD R16,R18
		   PUSH R16
		   uart_putc
		   JMP DIVISION_3


DIVISION_4:
           pop zl
		   pop zh
		   pop yl
		   pop yh
           pop r20 
		   pop r19
		   pop r18 
		   pop r17
		   pop r16 
		   pop xl
		   pop xh   
.ENDMACRO        
/*
+------------------------------------------------------------------------------
| Function    : uart_puts_string
+------------------------------------------------------------------------------
| Description : 输出表示符为STRING后面的内容直到遇到ascii码为0停止输出
|				
| Parameters  : 传入参数(标示符)
| Returns     : 
|
+------------------------------------------------------------------------------
*/
.MACRO   uart_puts_string
         push zh
		 push zl
		 push r16

         LDI ZH,HIGH(@0<<1)
	     LDI ZL,LOW(@0<<1)
uart_puts_string_2:
		 LPM R16,Z
		 CLZ 
		 CPI R16,0
		 BREQ uart_puts_string_1
		 PUSH R16
		 uart_putc
		 ADIW ZH:ZL,1	
		 JMP uart_puts_string_2    
uart_puts_string_1:       
         pop r16
		 pop zl
		 pop zh    
.ENDMACRO  
