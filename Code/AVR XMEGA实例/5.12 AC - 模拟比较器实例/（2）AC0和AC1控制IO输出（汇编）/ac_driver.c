/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Analog Comparator driver source file.
 *
 *      This file contains the function implementations for the XMEGA Analog Comparator driver.
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
 *      Several functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is discouraged,
 *      in some occasions the operator makes it possible to write pretty clean and
 *      neat code. In this driver, the construct is used to set or not set a
 *      configuration bit based on a boolean input parameter, such as
 *      the "some_parameter" in the example above.
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
#include "ac_driver.h"

/*! \brief Enable a comparator.
 *
 *  This function enables one of the two comparator submodules.
 *
 *  \param  ac         Pointer to Analog Comparator module.
 *  \param  comp       Which submodule, 0 or 1.
 *  \param  highSpeed  Set to false for low-power, true for high-speed.
 */
void AC_Enable(AC_t * ac, AC_COMP_t comp, bool highSpeed)
{
	/* Access correct registers based on which comparator submodule indicated. */
	if(comp == 0){
		ac->AC0CTRL = (ac->AC0CTRL & ~AC_HSMODE_bm)|
 		              (AC_ENABLE_bm | (highSpeed ? AC_HSMODE_bm : 0));
	}else{
		ac->AC1CTRL = (ac->AC1CTRL & ~AC_HSMODE_bm)|
			          (AC_ENABLE_bm | (highSpeed ? AC_HSMODE_bm : 0));
	}
}

/*! \brief Disable a comparator.
 *
 *  This function disables one of the two comparator submodules.
 *
 *  \param  ac    Pointer to Analog Comparator module.
 *  \param  comp  Which submodule, 0 or 1.
 */
void AC_Disable(AC_t * ac, AC_COMP_t comp)
{
	/* Access correct registers based on which comparator submodule indicated. */
	if(comp == 0){
		ac->AC0CTRL &= ~AC_ENABLE_bm;
	}else{
		ac->AC1CTRL &= ~AC_ENABLE_bm;
	}
}


/*! \brief Configure interrupt mode and level for a comparator.
 *
 *  This function configures interrupt settings for one of the two comparator submodules.
 *
 *  \param  ac              Pointer to Analog Comparator module.
 *  \param  comp            Which submodule, 0 or 1.
 *  \param  interruptMode   Interrupt mode setting (falling, rising, toggle).
 *  \param  interruptLevel  Interrupt level setting.
 */
void AC_ConfigInterrupt(AC_t * ac, AC_COMP_t comp, AC_INTMODE_t interruptMode,
						AC_INTLVL_t interruptLevel)
{
	/* Access correct registers based on which comparator submodule indicated. */
	if(comp == 0){
		ac->AC0CTRL = (ac->AC0CTRL & ~(AC_INTMODE_gm | AC_INTLVL_gm))|
		              ((uint8_t) interruptMode | interruptLevel);
	}else{
		ac->AC1CTRL = (ac->AC1CTRL & ~(AC_INTMODE_gm | AC_INTLVL_gm)) |
			          ((uint8_t) interruptMode | interruptLevel);
	}
}


/*! \brief Configure hysteresis settings for a comparator.
 *
 *   This function configures hysteresis settings for one of the two comparator submodules.
 *
 *  \param  ac              Pointer to Analog Comparator module.
 *  \param  comp            Which submodule, 0 or 1.
 *  \param  hysteresisMode  Hysteresis setting (none, small, large).
 */
void AC_ConfigHysteresis(AC_t * ac, AC_COMP_t comp, AC_HYSMODE_t hysteresisMode)
{
	/* Access correct registers based on which comparator submodule indicated. */
	if(comp == 0){
		ac->AC0CTRL = (ac->AC0CTRL & ~AC_HYSMODE_gm) | hysteresisMode;
	}else{
		ac->AC1CTRL = (ac->AC1CTRL & ~AC_HYSMODE_gm) | hysteresisMode;
	}
}

/*! \brief Configure MUX input settings for a comparator.
 *
 *  This function configures MUX settings for one of the two comparator submodules.
 *
 *  \param  ac    Pointer to Analog Comparator module.
 *  \param  comp  Which submodule, 0 or 1.
 *  \param  pos   Positive MUX input.
 *  \param  neg   Negative MUX input.
 */
void AC_ConfigMUX(AC_t * ac, AC_COMP_t comp, AC_MUXPOS_t pos, AC_MUXNEG_t neg)
{
	/* Access correct registers based on which comparator submodule indicated. */
	if(comp == 0){
		ac->AC0MUXCTRL = (ac->AC0MUXCTRL & ~(AC_MUXPOS_gm | AC_MUXNEG_gm)) |
		                 ((uint8_t) pos | neg);
	}else{
		ac->AC1MUXCTRL = (ac->AC1MUXCTRL & ~(AC_MUXPOS_gm | AC_MUXNEG_gm)) |
			             ((uint8_t) pos | neg);
	}
}


/*! \brief Configure the voltage scaler.
 *
 *   This function configures the internal voltage scaler. The voltage scaler
 *   input is VCC and it can scale the voltage in 1/64 steps. The output from
 *   the voltage scaler can be used as input.
 *
 *  \param  ac              Pointer to Analog Comparator module.
 *  \param  scaleFactor     Scale factor in number of 1/64ths.
 */
void AC_ConfigVoltageScaler(AC_t * ac, uint8_t scaleFactor)
{
	/* Scale factor gets truncated to bit field size. */
	ac->CTRLB = (ac->CTRLB & ~AC_SCALEFAC_gm) |
	            ((scaleFactor << AC_SCALEFAC_gp) & AC_SCALEFAC_gm);
}


/*! \brief Enable and configure window mode and associated interrupt settings.
 *
 *   This function enables and configures the window mode and the window
 *   interrupt level.
 *
 *  \param  ac              Pointer to Analog Comparator module.
 *  \param  interruptMode   Interrupt mode setting (above, below, inside, outside).
 *  \param  interruptLevel  Interrupt level setting.
 *
 *  \note Both submodules in the Analog Comparator must be enabled to make the
 *        window mode work. This is done by using the AC_enable function.
 */
void AC_EnableWindowMode(AC_t * ac, AC_WINTMODE_t interruptMode,
                         AC_WINTLVL_t interruptLevel)
{
	ac->WINCTRL = (ac->WINCTRL & ~(AC_WINTMODE_gm | AC_WINTLVL_gm)) |
	              (AC_WEN_bm | interruptMode | interruptLevel);
}


/*! \brief  This function disables the window mode.
 *
 *  \param  ac  Pointer to Analog Comparator module.
 */
void AC_DisableWindowMode(AC_t * ac)
{
	ac->WINCTRL = ~AC_WEN_bm;
}


/*! \brief  This function enables routing of comparator 0 output to pin 0.
 *
 *  \param  ac  Pointer to Analog Comparator module.
 */
void AC_EnableComparator0_Output(AC_t * ac)
{
	ac->CTRLA |= AC_AC0OUT_bm;
}


/*! \brief  This function disables routing of comparator 0 output to pin 0.
 *
 *  \param  ac  Pointer to Analog Comparator module.
 */
void AC_DisableComparator0_Output(AC_t * ac)
{
	ac->CTRLA &= ~AC_AC0OUT_bm;
}


/*! \brief Get current state of one comparator.
 *
 *  This function returns the current state of one of the two comparator submodule.
 *
 *  \param  ac    Pointer to Analog Comparator module.
 *  \param  comp  Which submodule, 0 or 1.
 *
 *  \return  0 if positive input is below negative, non-zero otherwise.
 */
uint8_t AC_GetComparatorState(AC_t * ac, AC_COMP_t comp)
{
	uint8_t state = 0;

	/* Access correct bits based on which comparator submodule indicated. */
	if(comp == 0){
		state = ac->STATUS & AC_AC0STATE_bm;
	}else if(comp == 1){
		state = ac->STATUS & AC_AC1STATE_bm;
	}else{

	}

	return state;
}


/*! \brief Get current state for window mode.
 *
 *  This function returns the current window state.
 *  The return value from this function can be compared with the
 *  enumerator values from AC_WSTATE_t.
 *
 *  \param  ac  Pointer to Analog Comparator module.
 *
 *  \return  Window state.
 */
uint8_t AC_GetWindowState(AC_t * ac)
{
	return (ac->STATUS & AC_WSTATE_gm);
}


/*! \brief Wait for event from a comparator.
 *
 *  This function waits for a comparator event, clears the flag and returns.
 *  Which event to wait for is decided by the interrupt mode setting.
 *
 *  \param  ac    Pointer to Analog Comparator module.
 *  \param  comp  Which submodule, 0 or 1.
 */
void AC_WaitForComparator_Blocking(AC_t * ac, AC_COMP_t comp)
{
	/* Access correct bits based on which comparator submodule indicated.*/
	if(comp == 0){
		/* Wait for interrupt flag to be set and clear it. */
		do {} while ((ac->STATUS & AC_AC0IF_bm) == 0);
		ac->STATUS = AC_AC0IF_bm;
	}else{
		/* Wait for interrupt flag to be set and clear it. */
		do {} while ((ac->STATUS & AC_AC1IF_bm) == 0);
		ac->STATUS = AC_AC1IF_bm;
	}
}


/*! \brief Wait for window mode event.
 *
 *  This function waits for a window mode event, clears the flag and returns.
 *  Which event to wait for is decided by the window interrupt mode setting.
 *
 *  \param  ac    Pointer to Analog Comparator module.
 */
void AC_WaitForWindow_Blocking(AC_t * ac)
{
	/* Wait for interrupt flag to be set and clear it. */
	do {} while ((ac->STATUS & AC_WIF_bm) == 0);
	ac->STATUS = AC_WIF_bm;
}

