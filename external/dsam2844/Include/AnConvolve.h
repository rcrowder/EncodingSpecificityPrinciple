/**********************
 *
 * File:		AnConvolve.h
 * Purpose:		This routine creates convoluted signal from two EarObject's
 *				output signals.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				The output signal will be monaural or binural (with interleaved
 *				channels: LRLRLR) depending upon the format of the two input
 *				signals, which will both be the same.
 *				The convolution is defined as (R*S)[j] = sum(j = 0, m - 1)
 *				{ S[j-k]*R[k]}.
 * Author:		Lowel O'Mard and Enrique Lopez-Poveda.
 * Created:		27 May 1996
 * Updated:	
 * Copyright:	(c) 1998, 2010 Lowel P. O'Mard
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

#ifndef _ANCONVOLVE_H
#define _ANCONVOLVE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_Convolution(EarObjectPtr data);

BOOLN	InitModule_Analysis_Convolution(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_Convolution(ModulePtr theModule);

BOOLN	CheckData_Analysis_Convolution(EarObjectPtr data);

__END_DECLS

#endif
