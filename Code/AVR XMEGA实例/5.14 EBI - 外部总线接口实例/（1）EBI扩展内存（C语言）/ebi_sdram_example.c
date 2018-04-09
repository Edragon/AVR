/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA EBI driver SDRAM example source.
 *
 *      This file contains an example application that demonstrates the EBI
 *      driver. It shows how to configure the EBI for 3-port SDRAM operation.
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
#define SDRAM_SIZE 0x800000UL

/*! \brief Memory block to use in the memory map.
 *
 *   This depends on the size of the RAM. It can be up to (16MB/RAM_SIZE - 1).
 *   In this example the RAM size is 8MB, so the block can be either 0 or 1.
 */
#define MEMORY_BLOCK 1

/*! \brief Address where we want SDRAM to be accessed. */
#define SDRAM_ADDR (SDRAM_SIZE * MEMORY_BLOCK)


/*! \brief Test function for EBI with SDRAM.
 *
 *  Hardware setup for 3-port SDRAM interface:
 *
 *  PORTK[7:0] - A[7:0]
 *
 *  PORTJ[7:0] - {A[11:8],D[3:0]}
 *
 *  PORTH[7:0] - {WE,CAS,RAS,DQM,BA0,BA1,CKE,CLK}
 *
 *  Since the EBI in 3-port mode does not have any spare pins for Chip Select,
 *  this should be controlled by a General Purpose IO pin or be pulled low by
 *  externally to enable the RAM.
 *
 *  The EBI SDRAM settings need to be set according to the characteristics of
 *  the SDRAM in use. The settings for used with the SDRAM in this example is
 *  commented in the EBI_EnableSDRAM function call.
 *
 *  The setup is tested by writing a set of data to the SDRAM. The data is then
 *  read back and verified. At the end, the program will be stuck in one of
 *  two infinite loops, dependent on whether the test passed or not.
 */
int main( void )
{
	/* Counter indicating correct data transfer to and from SDRAM */
	uint32_t SDRAM_ERR = 0;

	/* Configure bus pins as outputs(except for data lines). */
	PORTH.DIR = 0xFF;
	PORTK.DIR = 0xFF;
	PORTJ.DIR = 0xF0;

	/* Initialize EBI. */
	EBI_Enable( EBI_SDDATAW_4BIT_gc,
	            EBI_LPCMODE_ALE1_gc,
	            EBI_SRMODE_ALE12_gc,
	            EBI_IFMODE_3PORT_gc );

	/* Initialize SDRAM. (PER2X clock is 2MHz giving a 500ns clock cycle.) */
	EBI_EnableSDRAM( EBI_CS_ASPACE_8MB_gc,   /* 8 MB address space. */
	                 SDRAM_ADDR,             /* Base address. */
	                 false,                  /* 2 cycle CAS Latency. */
	                 true,                   /* 12 Row bits. */
	                 EBI_SDCOL_10BIT_gc,     /* 10 Column bits. */
	                 EBI_MRDLY_2CLK_gc,      /* 2 cycle Mode Register Delay. (min 2CLK) */
	                 EBI_ROWCYCDLY_1CLK_gc,  /* 1 cycle Row Cycle Delay. */
	                 EBI_RPDLY_1CLK_gc,      /* 1 cycle Row to Pre-charge Delay. (min 37ns) */
	                 EBI_WRDLY_2CLK_gc,      /* 2 cycle Write Recovery Delay. (1CLK + 7ns)*/
	                 EBI_ESRDLY_1CLK_gc,     /* 1 cycle Exit Self Refresh to Active Delay. (min 67ns) */
	                 EBI_ROWCOLDLY_1CLK_gc,  /* 1 cycle Row to Column Delay. (min 15ns) */
	                 0x001F,                 /* 31 cycle Refresh Period (max 15.625us). */
	                 0x00C8 );               /* 200 cycle Initialization Delay (min 100us). */

	/* Fill SDRAM with data. */
	for (uint32_t i = 0; i < 0x40000; i++) {
		__far_mem_write(i+SDRAM_ADDR, TESTBYTE);
	}

	/* Read back from SDRAM and verify. */
	for (uint32_t i = 0; i < 0x40000; i++) {
		if (__far_mem_read(i+SDRAM_ADDR) != TESTBYTE){
			SDRAM_ERR++;
		}
	}

	/* Report success or failure. */

	if (SDRAM_ERR == 0) {
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
