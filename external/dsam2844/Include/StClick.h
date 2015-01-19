/**********************
 *
 * File:		StClick.h
 * Purpose:		This module contains the methods for the Click stimulus
 *				generation paradigm.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997, 2010 Lowel P. O'Mard
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

#ifndef	_STCLICK_H
#define _STCLICK_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define CLICK_NUM_PARS			4

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	CLICK_CLICKTIME,
	CLICK_AMPLITUDE,
	CLICK_DURATION,
	CLICK_SAMPLINGINTERVAL

} ClickParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	Float	clickTime, amplitude;
	Float	duration, dt;

	/* Private members */
	UniParListPtr	parList;

} Click, *ClickPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	ClickPtr	clickPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_Click(void);

BOOLN	GenerateSignal_Click(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_Click(void);

BOOLN	Init_Click(ParameterSpecifier parSpec);

BOOLN	PrintPars_Click(void);

BOOLN	SetClickTime_Click(Float theClickTime);

BOOLN	SetDuration_Click(Float theDuration);

BOOLN	SetAmplitude_Click(Float theAmplitude);

BOOLN	InitModule_Click(ModulePtr theModule);

BOOLN	SetParsPointer_Click(ModulePtr theModule);

BOOLN	SetSamplingInterval_Click(Float theSamplingInterval);

BOOLN	SetUniParList_Click(void);

__END_DECLS

#endif
