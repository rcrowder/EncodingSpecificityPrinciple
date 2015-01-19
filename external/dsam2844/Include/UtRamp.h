/**********************
 * File:		UtRamp.h
 * Purpose:		This is the signal ramp module. The ramps gradually raise or
 * 				lower the signal intensity to full strength or from full strength
 * 				to zero at the beginning or end of the signal respectively.
 * Comments:	Valid ramp functions are defined as a function which varies from
 * 				0 to 1 as a function of the ramp interval.
 *				The functions used must be defined in the format given below (see
 *				The Sine_Ramp and Ramp_linear) and are passed to the
 *				RampUpOutSignal_Ramp and RampDownOutSignal_Ramp functions as
 *				the arguments.
 *				Impulse signals, such as clicks, should not be ramped: if a
 *				warning message is given, in this case ignore it.
 * 				This module allows the processing of a simulation to be
 *				interrupted, producing a specified message.
 * Author:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:
 * Copyright:	(c) 1993, 2010 Lowel P. O'Mard
 *
 *********************
 *
 *  This file is part of DSAM.
 *
 *  DSAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DSAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 *********************/

#ifndef	_UTRAMP_H
#define _UTRAMP_H	1

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	RampUpOutSignal_Ramp(EarObjectPtr data,
		  Float (* RampFunction)(ChanLen, ChanLen), Float timeInterval);

void	RampDownOutSignal_Ramp(EarObjectPtr data,
		   Float (* RampFunction)(ChanLen, ChanLen), Float timeInterval);

BOOLN	CheckPars_Ramp(SignalData *theSignal, Float timeInterval);

Float	Sine_Ramp(ChanLen step, ChanLen intervalIndex);

Float	Linear_Ramp(ChanLen step, ChanLen intervalIndex);

__END_DECLS

#endif
