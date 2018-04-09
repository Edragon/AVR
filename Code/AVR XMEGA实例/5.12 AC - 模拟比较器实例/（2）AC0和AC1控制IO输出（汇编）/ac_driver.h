/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Analog Comparator driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA Analog Comparator driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA Analog Comparator.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * \par Application note:
 *      AVR1302: Using the XMEGA Analog Comparator
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1569 $
 * $Date: 2008-04-22 13:03:43 +0200 (ti, 22 apr 2008) $  \n
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
#ifndef AC_DRIVER_H
#define AC_DRIVER_H

#include "avr_compiler.h"


typedef enum AC_COMP_ENUM{
	ANALOG_COMPARATOR0 = 0,
	ANALOG_COMPARATOR1 = 1,
}AC_COMP_t;


void AC_Enable(AC_t * ac, AC_COMP_t comp, bool highSpeed);
void AC_Disable(AC_t * ac, AC_COMP_t comp);
void AC_ConfigInterrupt(AC_t * ac, AC_COMP_t comp, AC_INTMODE_t interruptMode,
                        AC_INTLVL_t interruptLevel);
void AC_ConfigHysteresis(AC_t * ac, AC_COMP_t comp, AC_HYSMODE_t hysteresisMode);
void AC_ConfigMUX(AC_t * ac, AC_COMP_t comp, AC_MUXPOS_t pos, AC_MUXNEG_t neg);
void AC_ConfigVoltageScaler(AC_t * ac,  uint8_t scaleFactor);
void AC_EnableWindowMode(AC_t * ac, AC_WINTMODE_t interruptMode, AC_WINTLVL_t interruptLevel);
void AC_DisableWindowMode(AC_t * ac);
void AC_EnableComparator0_Output(AC_t * ac);
void AC_DisableComparator0_Output(AC_t * ac);
uint8_t AC_GetComparatorState(AC_t * ac, AC_COMP_t comp);
uint8_t AC_GetWindowState(AC_t * ac);
void AC_WaitForComparator_Blocking(AC_t * ac, AC_COMP_t comp);
void AC_WaitForWindow_Blocking(AC_t * ac);


#endif // AC_DRIVER_H
