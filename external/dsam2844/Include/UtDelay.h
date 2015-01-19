/**********************
 *
 * File:		UtDelay.h
 * Purpose:		This module introduces a delay in a monaural signal or an
 *				interaural time difference (ITD) in a binaural signal.
 * Comments:	Written using ModuleProducer version 1.10 (Jan  3 1997).
 *				For binaural signals if the delay is positive, then the right
 *				(second) channel is delayed relative to the left (first)
 *				channel, and vice versus for negative delays.
 *				Monaural signals always treat delays as positive values.
 * Author:		L. P. O'Mard
 * Created:		08 Apr 1997
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

#ifndef _UTDELAY_H
#define _UTDELAY_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_DELAY_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_DELAY_MODE,
	UTILITY_DELAY_INITIALDELAY,
	UTILITY_DELAY_FINALDELAY

} Delay2ParSpecifier;

typedef enum {

	DELAY_SINGLE_MODE,
	DELAY_LINEAR_MODE,
	DELAY_NULL

} DelayModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;
	Float	initialDelay;
	Float	finalDelay;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;
	Float	delayPerChannel;

} Delay2, *Delay2Ptr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	Delay2Ptr	delay2Ptr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_Delay(EarObjectPtr data);

BOOLN	Free_Utility_Delay(void);

UniParListPtr	GetUniParListPtr_Utility_Delay(void);

BOOLN	Init_Utility_Delay(ParameterSpecifier parSpec);

BOOLN	InitModeList_Utility_Delay(void);

BOOLN	PrintPars_Utility_Delay(void);

BOOLN	Process_Utility_Delay(EarObjectPtr data);

void	SetEnabledPars_Utility_Delay(void);

BOOLN	SetFinalDelay_Utility_Delay(Float theFinalDelay);

BOOLN	SetInitialDelay_Utility_Delay(Float theInitialDelay);

BOOLN	SetMode_Utility_Delay(WChar *theMode);

BOOLN	InitModule_Utility_Delay(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_Delay(ModulePtr theModule);

BOOLN	SetUniParList_Utility_Delay(void);

__END_DECLS

#endif
