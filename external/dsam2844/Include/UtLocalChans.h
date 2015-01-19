/**********************
 *
 * File:		UtLocalChans.h
 * Purpose:		It calculates measures of a multi-channel input, grouping
 *				the channels according to a specified algorithm.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 * Author:		L. P. O'Mard
 * Created:		17 Jan 2002
 * Updated:
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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

#ifndef _UTLOCALCHANS_H
#define _UTLOCALCHANS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_LOCALCHANS_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_LOCALCHANS_MODE,
	UTILITY_LOCALCHANS_LIMITMODE,
	UTILITY_LOCALCHANS_LOWERAVELIMIT,
	UTILITY_LOCALCHANS_UPPERAVELIMIT

} LocalChansParSpecifier;

typedef enum {

	UTILITY_LOCALCHANS_MODE_SUM,
	UTILITY_LOCALCHANS_MODE_AVERAGE,
	UTILITY_LOCALCHANS_MODE_NULL

} UtilityLocalChansModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;
	int		limitMode;
	Float	lowerLimit;
	Float	upperLimit;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;
	int		minChan;

} LocalChans, *LocalChansPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	LocalChansPtr	localChansPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	Calc_Utility_LocalChans(EarObjectPtr data);

BOOLN	CheckData_Utility_LocalChans(EarObjectPtr data);

BOOLN	Free_Utility_LocalChans(void);

UniParListPtr	GetUniParListPtr_Utility_LocalChans(void);

void	GetWindowLimits_Utility_LocalChans(SignalDataPtr signal, int chan,
		  int *minChan, int *maxChan);

BOOLN	InitModeList_Utility_LocalChans(void);

BOOLN	InitModule_Utility_LocalChans(ModulePtr theModule);

BOOLN	Init_Utility_LocalChans(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_LocalChans(void);

void	ResetProcess_Utility_LocalChans(EarObjectPtr data);

BOOLN	SetLowerLimit_Utility_LocalChans(Float theLowerLimit);

BOOLN	SetLimitMode_Utility_LocalChans(WChar * theLimitMode);

BOOLN	SetMode_Utility_LocalChans(WChar * theMode);

BOOLN	SetParsPointer_Utility_LocalChans(ModulePtr theModule);

BOOLN	SetUniParList_Utility_LocalChans(void);

BOOLN	SetUpperLimit_Utility_LocalChans(Float theUpperLimit);

__END_DECLS

#endif
