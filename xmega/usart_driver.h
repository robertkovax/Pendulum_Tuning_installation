/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA USART driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA USART driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA ADC module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * \par Application note:
 *      AVR1307: Using the XMEGA USART
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1694 $
 * $Date: 2008-07-29 14:21:58 +0200 (ti, 29 jul 2008) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifndef USART_DRIVER_H
#define USART_DRIVER_H

#include "avr_compiler.h"

/** @brief  UART Baudrate Expression
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define USART_BAUD_SELECT(baudRate,xtalCpu)					((xtalCpu)/((baudRate)*16l)-1)
#define USART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu)	(((xtalCpu)/((baudRate)*8l)-1)|0x8000)


/* Enable if you would use the extended library version. */
#define EXTENDED	1

/* Enable if you would use the XON XOFF Flowcontrol. */
#define FLOWCTRL 1												// todo 

/* USART buffer defines. */
#define USART_RX_BUFFER_SIZE 256								// brief  Receive buffer size: 2,4,8,16,32,64,128 or 256 bytes. 
#define USART_TX_BUFFER_SIZE 256								// Transmit buffer size: 2,4,8,16,32,64,128 or 256 bytes 
#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1 )		// Receive buffer mask. 
#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1 )		// Transmit buffer mask. 


#if ( USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
	#endif
#if ( USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK )
	#error TX buffer size is not a power of 2
#endif

/* test if the size of the circular buffers fits into SRAM */
#if ( (USART_RX_BUFFER_SIZE+USART_TX_BUFFER_SIZE) >= (RAMEND-0x60 ) )
	#error "size of USART_RX_BUFFER_SIZE + USART_TX_BUFFER_SIZE larger than size of SRAM"
#endif


/* \brief USART transmit and receive ring buffer. */
typedef struct USART_Buffer
{
	/* \brief Receive buffer. */
	volatile uint8_t RX[USART_RX_BUFFER_SIZE];
	/* \brief Transmit buffer. */
	volatile uint8_t TX[USART_TX_BUFFER_SIZE];
	/* \brief Receive buffer head. */
	volatile uint8_t RX_Head;
	/* \brief Receive buffer tail. */
	volatile uint8_t RX_Tail;
	/* \brief Transmit buffer head. */
	volatile uint8_t TX_Head;
	/* \brief Transmit buffer tail. */
	volatile uint8_t TX_Tail;
} USART_Buffer_t;


/* 
** high byte error return code of uart_getc()					// todo error integration
*/
#define USART_FRAME_ERROR      0x0800							// Framing Error by UART       
#define USART_OVERRUN_ERROR    0x0400            				// Overrun condition by UART   
#define USART_BUFFER_OVERFLOW  0x0200            				// receive ringbuffer overflow 
#define USART_NO_DATA          0x0100            				// no receive data available   


/*! \brief Struct used when interrupt driven driver is used.
*
*  Struct containing pointer to a usart, a buffer and a location to store Data
*  register interrupt level temporary.
*/
typedef struct Usart_and_buffer
{
	/* \brief Pointer to USART module to use. */
	USART_t * usart;
	/* \brief Data register empty interrupt level. */
	USART_DREINTLVL_t dreIntLevel;
	/* \brief Data buffer. */
	USART_Buffer_t buffer;
} USART_data_t;

USART_data_t				USART_C0_data;				// USART data structUSART data struct used in example. 


/* Macros. */

/*! \brief Macro that sets the USART frame format.
 *
 *  Sets the frame format, Frame Size, parity mode and number of stop bits.
 *
 *  \param _usart        Pointer to the USART module
 *  \param _charSize     The character size. Use USART_CHSIZE_t type.
 *  \param _parityMode   The parity Mode. Use USART_PMODE_t type.
 *  \param _twoStopBits  Enable two stop bit mode. Use bool type.
 */
#define USART_Format_Set(_usart, _charSize, _parityMode, _twoStopBits)     (_usart)->CTRLC = (uint8_t) _charSize | _parityMode |(_twoStopBits ? USART_SBMODE_bm : 0)


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
#define USART_Baudrate_Set(_usart, _bselValue, _bScaleFactor)		(_usart)->BAUDCTRLA =(uint8_t)_bselValue; (_usart)->BAUDCTRLB =(_bScaleFactor << USART_BSCALE0_bp)|(_bselValue >> 8)


/*! \brief Enable USART receiver.
 *
 *  \param _usart    Pointer to the USART module
 */
#define USART_Rx_Enable(_usart) ((_usart)->CTRLB |= USART_RXEN_bm)


/*! \brief Disable USART receiver.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Rx_Disable(_usart)		((_usart)->CTRLB &= ~USART_RXEN_bm)


/*! \brief Enable USART transmitter.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Tx_Enable(_usart)			((_usart)->CTRLB |= USART_TXEN_bm)


/*! \brief Disable USART transmitter.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Tx_Disable(_usart)		((_usart)->CTRLB &= ~USART_TXEN_bm)


#define USART_CLK2X_Enable(_usart)		((_usart)->CTRLB |= USART_CLK2X_bm)		// Enable Double Speed Modus


/*! \brief Set USART RXD interrupt level.
 *
 *  Sets the interrupt level on RX Complete interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _rxdIntLevel  Interrupt level of the RXD interrupt.
 *                       Use USART_RXCINTLVL_t type.
 */
#define USART_RxdInterruptLevel_Set(_usart, _rxdIntLevel)   ((_usart)->CTRLA = ((_usart)->CTRLA & ~USART_RXCINTLVL_gm) | _rxdIntLevel)


/*! \brief Set USART TXD interrupt level.
 *
 *  Sets the interrupt level on TX Complete interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _txdIntLevel  Interrupt level of the TXD interrupt.
 *                       Use USART_TXCINTLVL_t type.
 */
#define USART_TxdInterruptLevel_Set(_usart, _txdIntLevel)   (_usart)->CTRLA = ((_usart)->CTRLA & ~USART_TXCINTLVL_gm) | _txdIntLevel



/*! \brief Set USART DRE interrupt level.
 *
 *  Sets the interrupt level on Data Register interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _dreIntLevel  Interrupt level of the DRE interrupt.
 *                       Use USART_DREINTLVL_t type.
 */
#define USART_DreInterruptLevel_Set(_usart, _dreIntLevel)   (_usart)->CTRLA = ((_usart)->CTRLA & ~USART_DREINTLVL_gm) | _dreIntLevel


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
#define USART_SetMode(_usart, _usartMode)  ((_usart)->CTRLC = ((_usart)->CTRLC & (~USART_CMODE_gm)) | _usartMode)



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


/**
 *  @brief   Put string to ringbuffer for transmitting via UART
 *
 *  The string is buffered by the uart library in a circular buffer
 *  and one character at a time is transmitted to the UART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 * 
 *  @param   s string to be transmitted
 *  @return  none
 */
#define usart_puts_P(_usart, __s)      (usart_puts_p((_usart),PSTR(__s)))	// Macro to automatically put a string constant into program memory


/* Functions for interrupt driven driver. */
void USART_InterruptDriver_Initialize(USART_data_t * usart_data, USART_t * usart, USART_DREINTLVL_t dreIntLevel );
void USART_InterruptDriver_DreInterruptLevel_Set(USART_data_t * usart_data, USART_DREINTLVL_t dreIntLevel);

bool USART_TXBuffer_FreeSpace(USART_data_t * usart_data);
bool USART_TXBuffer_PutByte(USART_data_t * usart_data, uint8_t data);
bool USART_RXBufferData_Available(USART_data_t * usart_data);
uint8_t USART_RXBuffer_GetByte(USART_data_t * usart_data);
bool USART_RXComplete(USART_data_t * usart_data);
void USART_DataRegEmpty(USART_data_t * usart_data);

/* Functions for polled driver. */
void USART_NineBits_PutChar(USART_t * usart, uint16_t data);
uint16_t USART_NineBits_GetChar(USART_t * usart);



/* Add-on */
void usart_init(USART_data_t * usart_data, USART_t * usart, unsigned int baudrate, USART_DREINTLVL_t dreIntLevel );
bool usart_putc(USART_data_t * usart_data, uint8_t data);
bool usart_puts(USART_data_t * usart_data, const char *str );
bool usart_puts_p(USART_data_t * usart_data, const char *progmem_str );
uint8_t usart_getc(USART_data_t * usart_data);

extern void printFloat(USART_data_t * usart_data,double n);
extern void printInteger(USART_data_t * usart_data,long n);
extern void printIntegerInBase(USART_data_t * usart_data,unsigned long n, unsigned long base);

extern void usart_putc_hex(USART_data_t * usart_data,uint8_t b);
extern void usart_putw_hex(USART_data_t * usart_data,uint16_t w);
extern void usart_putdw_hex(USART_data_t * usart_data,uint32_t dw);

extern void usart_putw_dec(USART_data_t * usart_data,uint16_t w);
extern void usart_putdw_dec(USART_data_t * usart_data,uint32_t dw);

#endif