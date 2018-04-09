/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA EBI driver SRAM example source.
 *
 *      This file contains an example application that demonstrates the EBI
 *      driver. It shows how to configure the EBI for 3-port SRAM operation.
 *      The example fills a data pattern in the memory, copies back
 *      and compares the copied block.
 *
 * \par Application note:
 *      AVR1312: Using the XMEGA External Bus Interface
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 595 $
 * $Date: 2007-03-22 14:43:03 +0100 (to, 22 mar 2007) $  \n
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
#include "avr_compiler.h"
#include "ebi_driver.h"

/* \brief Define the testbyte to be written to the RAM. */
#define TESTBYTE   0xA5

/*! \brief Size of SDRAM to be accessed. */
#define SRAM_SIZE 0x20000UL

/*! \brief Memory block to use in the memory map.
 *
 *   This depends on the size of the RAM. It can be up to (16MB/RAM_SIZE - 1).
 *   In this example the RAM size is 128KB, so the block can maximum 127.
 */
#define MEMORY_BLOCK 1

/*! \brief Address where we want SDRAM to be accessed. */
#define SRAM_ADDR (SRAM_SIZE * MEMORY_BLOCK)


/*! \brief Test function for EBI with SRAM.
 *
 *  Hardware setup for 3-port SRAM interface:
 *
 *  PORTK[7:0] - A[7:0]/A[15:8]/A[23:16] (BYTE 2 and 3 connected through ALE1 and ALE2)
 *
 *  PORTJ[7:0] - D[7:0]
 *
 *  PORTH[7:0] - {CS3,CS2,CS1,CS0,ALE2,ALE1,RE,WE} (CS0 used for SRAM)
 *
 *  In this example a 128KB SRAM is used, which does not require any wait states
 *  at 2 MHz. Pull-up resistors should be connected to the Chip Select line to
 *  prevent garbage from being written to the SRAM during start-up, while the
 *  control lines are still in an unknown state.
 *
 *  This example assume the use of two address latch enable (ALE) signals to
 *  multiplex the address bytes to the SRAM.
 *
 *  The setup is tested by writing a set of data to the SRAM. The data is then
 *  read back and verified. At the end, the program will be stuck in one of
 *  two infinite loops, dependent on whether the test passed or not.
 */
int main( void )
{
	/* Counter indicating correct data transfer to and from SDRAM */
	uint32_t SRAM_ERR = 0;

	/* Configure bus pins as outputs(except for data lines). */
	PORTH.DIR = 0xFF;
	PORTK.DIR = 0xFF;
	PORTJ.DIR = 0x00;

	/* Initialize EBI. */
	EBI_Enable( EBI_SDDATAW_8BIT_gc,
	            EBI_LPCMODE_ALE1_gc,
	            EBI_SRMODE_ALE12_gc,
	            EBI_IFMODE_3PORT_gc );

	/* Initialize SRAM */
	EBI_EnableSRAM( &EBI.CS0,               /* Chip Select 0. */
	                EBI_CS_ASPACE_128KB_gc, /* 128 KB Address space. */
	                SRAM_ADDR,              /* Base address. */
	                0 );                    /* 0 wait states. */

	/* Fill SRAM with data. */
	for (uint32_t i = 0; i < SRAM_SIZE; i++) {
		__far_mem_write(i+SRAM_ADDR, TESTBYTE);
	}

	/* Read back from SRAM and verify. */
	for (uint32_t i = 0; i < SRAM_SIZE; i++) {
		if (__far_mem_read(i+SRAM_ADDR) != TESTBYTE){
			SRAM_ERR++;
		}
	}

	/* Report success or failure. */
	if (SRAM_ERR == 0) {
		while(true) {
		/* Breakpoint for success. */
			nop();
		}
	}
	else {
		while(true) {
		/* Breakpoint for failure. */
			nop();
		}
	}
}
