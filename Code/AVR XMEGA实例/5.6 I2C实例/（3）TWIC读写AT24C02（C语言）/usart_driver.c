#include <avr/io.h>
#include "avr_compiler.h"
#define USART USARTC0
/*
+------------------------------------------------------------------------------
| Function    : uart_putc
+------------------------------------------------------------------------------
| Description : send the data 
|				
| Parameters  :  DATA to send  (HEX)
| Returns     : 
|
+------------------------------------------------------------------------------
*/
void uart_putc(unsigned char c)
{
    if(c == '\n')
        uart_putc('\r');

    /* wait until transmit buffer is empty */
    while(!(USART.STATUS & USART_DREIF_bm));

    /* send next byte */
    USART.DATA = c;
}

/*
+------------------------------------------------------------------------------
| Function    : uart_putc_hex
+------------------------------------------------------------------------------
| Description : convert the data to ASCII(2 Bytes) and send it 
|				0xf1 -->> 'F'+'1'
| Parameters  :  DATA to send  (8 bits HEX)
| Returns     : 
|
+------------------------------------------------------------------------------
*/
void uart_putc_hex(unsigned char b)
{
    /* upper nibble */
    if((b >> 4) < 0x0a)
        uart_putc((b >> 4) + '0');
    else
        uart_putc((b >> 4) - 0x0a + 'a');

    /* lower nibble */
    if((b & 0x0f) < 0x0a)
        uart_putc((b & 0x0f) + '0');
    else
        uart_putc((b & 0x0f) - 0x0a + 'a');
}

/*
+------------------------------------------------------------------------------
| Function    : uart_putw_hex
+------------------------------------------------------------------------------
| Description : convert the data to ASCII(4 Bytes) and send it 
|				0xf1e0 -->> 'f'+'1'+'e'+'0'
| Parameters  :  DATA to send  (16 bits HEX)
| Returns     : 
|
+------------------------------------------------------------------------------
*/
void uart_putw_hex(unsigned int w)
{
    uart_putc_hex((unsigned char) (w >> 8));
    uart_putc_hex((unsigned char) (w & 0xff));
}

/*
+------------------------------------------------------------------------------
| Function    : uart_putdw_hex
+------------------------------------------------------------------------------
| Description : convert the data to ASCII(2 Bytes) and send it 
|				0xf1e01234 -->> 'f'+'1'+'e'+'0'+'1'+'2'+'3'+'4'
| Parameters  :  DATA to send  (32 bits HEX)
| Returns     : 
|
+------------------------------------------------------------------------------
*/
void uart_putdw_hex(unsigned long dw)
{
    uart_putw_hex((unsigned int) (dw >> 16));
    uart_putw_hex((unsigned int) (dw & 0xffff));
}

/*
+------------------------------------------------------------------------------
| Function    : uart_putw_dec
+------------------------------------------------------------------------------
| Description : convert the data to decimal numbers and send it 
|				0x123 -->> '2'+'9'+'1'
| Parameters  :  DATA to send  (16 bits HEX)
| Returns     : 
|
+------------------------------------------------------------------------------
*/
void uart_putw_dec(unsigned int w)
{
    unsigned int num = 10000;
    unsigned char started = 0;

    while(num > 0)
    {
        unsigned char b = w / num;
        if(b > 0 || started || num == 1)
        {
            uart_putc('0' + b);
            started = 1;
        }
        w -= b * num;

        num /= 10;
    }
}

/*
+------------------------------------------------------------------------------
| Function    : uart_putdw_dec
+------------------------------------------------------------------------------
| Description : convert the data to decimal numbers and send it 
|				0x123 -->> '2'+'9'+'1'
| Parameters  :  DATA to send  (32 bits HEX)
| Returns     : 
|
+------------------------------------------------------------------------------
*/
void uart_putdw_dec(unsigned long dw)
{
    unsigned long num = 1000000000;
    unsigned char started = 0;

    while(num > 0)
    {
        unsigned char b = dw / num;
        if(b > 0 || started || num == 1)
        {
            uart_putc('0' + b);
            started = 1;
        }
        dw -= b * num;

        num /= 10;
    }
}

/*
+------------------------------------------------------------------------------
| Function    : uart_puts
+------------------------------------------------------------------------------
| Description : send string
|				
| Parameters  : string to send
| Returns     : 
|
+------------------------------------------------------------------------------
*/
void uart_puts(const char* str)
{
    while(*str)
        uart_putc(*str++);
}

/*
+------------------------------------------------------------------------------
| Function    : uart_getc
+------------------------------------------------------------------------------
| Description : receive one byte 
|				
| Parameters  : 
| Returns     : data
|
+------------------------------------------------------------------------------
*/
unsigned char uart_getc(void)
{
    /* wait until receive buffer is full */
    while(!(USART.STATUS & USART_RXCIF_bm));

    unsigned char b = USART.DATA;
    if(b == '\r')
        b = '\n';
		
    return b;
}
//+------------------------------------------------------------------------------

 /*Macros. */

/*! \brief Macro that sets the USART frame format.
 *
 *  Sets the frame format, Frame Size, parity mode and number of stop bits.
 *
 *  \param _usart        Pointer to the USART module
 *  \param _charSize     The character size. Use USART_CHSIZE_t type.
 *  \param _parityMode   The parity Mode. Use USART_PMODE_t type.
 *  \param _twoStopBits  Enable two stop bit mode. Use bool type.
 */
#define USART_Format_Set(_usart, _charSize, _parityMode, _twoStopBits)         \
	(_usart)->CTRLC = (uint8_t) _charSize | _parityMode |                      \
	                  (_twoStopBits ? USART_SBMODE_bm : 0)


/*! \brief Set USART baud rate.
 *
 *  Sets the USART's baud rate register.
 *
 *  UBRR_Value   : Value written to UBRR
 *  ScaleFactor  : Time Base Generator Scale Factor
 *
 *  Equation for calculation of BSEL value in asynchronous normal speed mode:
 *  	If ScaleFactor >= 0
 *  		BSEL = ((I/O clock frequency)/(2^(ScaleFactor)*16*Baudrate))-1
 *  	If ScaleFactor < 0
 *  		BSEL = (1/(2^(ScaleFactor)*16))*(((I/O clock frequency)/Baudrate)-1)
 *
 *	\note See XMEGA manual for equations for calculation of BSEL value in other
 *        modes.
 *
 *  \param _usart          Pointer to the USART module.
 *  \param _bselValue      Value to write to BSEL part of Baud control register.
 *                         Use uint16_t type.
 *  \param _bScaleFactor   USART baud rate scale factor.
 *                         Use uint8_t type
 */
#define USART_Baudrate_Set(_usart, _bselValue, _bScaleFactor)                  \
	(_usart)->BAUDCTRLA =(uint8_t)_bselValue;                                           \
	(_usart)->BAUDCTRLB =(_bScaleFactor << USART_BSCALE0_bp)|(_bselValue >> 8)


/*! \brief Enable USART receiver.
 *
 *  \param _usart    Pointer to the USART module
 */
#define USART_Rx_Enable(_usart) ((_usart)->CTRLB |= USART_RXEN_bm)


/*! \brief Disable USART receiver.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Rx_Disable(_usart) ((_usart)->CTRLB &= ~USART_RXEN_bm)


/*! \brief Enable USART transmitter.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Tx_Enable(_usart)	((_usart)->CTRLB |= USART_TXEN_bm)


/*! \brief Disable USART transmitter.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Tx_Disable(_usart) ((_usart)->CTRLB &= ~USART_TXEN_bm)


/*! \brief Set USART RXD interrupt level.
 *
 *  Sets the interrupt level on RX Complete interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _rxdIntLevel  Interrupt level of the RXD interrupt.
 *                       Use USART_RXCINTLVL_t type.
 */
#define USART_RxdInterruptLevel_Set(_usart, _rxdIntLevel)                      \
	((_usart)->CTRLA = ((_usart)->CTRLA & ~USART_RXCINTLVL_gm) | _rxdIntLevel)


/*! \brief Set USART TXD interrupt level.
 *
 *  Sets the interrupt level on TX Complete interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _txdIntLevel  Interrupt level of the TXD interrupt.
 *                       Use USART_TXCINTLVL_t type.
 */
#define USART_TxdInterruptLevel_Set(_usart, _txdIntLevel)                      \
	(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_TXCINTLVL_gm) | _txdIntLevel



/*! \brief Set USART DRE interrupt level.
 *
 *  Sets the interrupt level on Data Register interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _dreIntLevel  Interrupt level of the DRE interrupt.
 *                       Use USART_DREINTLVL_t type.
 */
#define USART_DreInterruptLevel_Set(_usart, _dreIntLevel)                      \
	(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_DREINTLVL_gm) | _dreIntLevel


/*! \brief Set the mode the USART run in.
 *
 * Set the mode the USART run in. The default mode is asynchronous mode.
 *
 *  \param  _usart       Pointer to the USART module register section.
 *  \param  _usartMode   Selects the USART mode. Use  USART_CMODE_t type.
 *
 *  USART modes:
 *  - 0x0        : Asynchronous mode.
 *  - 0x1        : Synchronous mode.
 *  - 0x2        : IrDA mode.
 *  - 0x3        : Master SPI mode.
 */
#define USART_SetMode(_usart, _usartMode)                                      \
	((_usart)->CTRLC = ((_usart)->CTRLC & (~USART_CMODE_gm)) | _usartMode)



/*! \brief Check if data register empty flag is set.
 *
 *  \param _usart      The USART module.
 */
#define USART_IsTXDataRegisterEmpty(_usart) (((_usart)->STATUS & USART_DREIF_bm) != 0)



/*! \brief Put data (5-8 bit character).
 *
 *  Use the macro USART_IsTXDataRegisterEmpty before using this function to
 *  put data to the TX register.
 *
 *  \param _usart      The USART module.
 *  \param _data       The data to send.
 */
#define USART_PutChar(_usart, _data) ((_usart)->DATA = _data)



/*! \brief Checks if the RX complete interrupt flag is set.
 *
 *   Checks if the RX complete interrupt flag is set.
 *
 *  \param _usart     The USART module.
 */
#define USART_IsRXComplete(_usart) (((_usart)->STATUS & USART_RXCIF_bm) != 0)




/*! \brief Get received data (5-8 bit character).
 *
 *  This macro reads out the RX register.
 *  Use the macro USART_RX_Complete to check if anything is received.
 *
 *  \param _usart     The USART module.
 *
 *  \retval           Received data.
 */
#define USART_GetChar(_usart)  ((_usart)->DATA)
