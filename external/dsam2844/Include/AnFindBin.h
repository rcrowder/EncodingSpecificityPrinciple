/**********************
 *
 * File:		AnFindBin.h
 * Purpose:		This module finds the maximum/minimum bin values for a signal
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				The search is implemented using a bin "window" of "binWidth".
 * Author:		L. P. O'Mard
 * Created:		8 Mar 1996
 * Updated:		9 Jun 1996
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

#ifndef _ANFINDBIN_H
#define _ANFINDBIN_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_FINDBIN_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_FINDBIN_MODE,
	ANALYSIS_FINDBIN_BINWIDTH,
	ANALYSIS_FINDBIN_TIMEOFFSET,
	ANALYSIS_FINDBIN_TIMEWIDTH

} FindBinParSpecifier;

typedef enum {

	FIND_BIN_MIN_VALUE_MODE,
	FIND_BIN_MIN_INDEX_MODE,
	FIND_BIN_MAX_VALUE_MODE,
	FIND_BIN_MAX_INDEX_MODE,
	FIND_BIN_NULL

} FindBinModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;
	Float	binWidth;
	Float	timeOffset;
	Float	timeWidth;

	/* Private members */
	NameSpecifier *modeList;
	UniParListPtr	parList;
	BOOLN	findMinimum;
	ChanLen	binWidthIndex, timeWidthIndex, timeOffsetIndex;

} FindBin, *FindBinPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FindBinPtr	findBinPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_FindBin(EarObjectPtr data);

BOOLN	CheckData_Analysis_FindBin(EarObjectPtr data);

BOOLN	Free_Analysis_FindBin(void);

UniParListPtr	GetUniParListPtr_Analysis_FindBin(void);

BOOLN	Init_Analysis_FindBin(ParameterSpecifier parSpec);

BOOLN	InitModeList_Analysis_FindBin(void);

BOOLN	PrintPars_Analysis_FindBin(void);

BOOLN	SetBinWidth_Analysis_FindBin(Float theBinWidth);

BOOLN	SetMode_Analysis_FindBin(WChar *theMode);

BOOLN	InitModule_Analysis_FindBin(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_FindBin(ModulePtr theModule);

BOOLN	SetTimeOffset_Analysis_FindBin(Float theTimeOffset);

BOOLN	SetTimeWidth_Analysis_FindBin(Float theTimeWidth);

BOOLN	SetUniParList_Analysis_FindBin(void);

__END_DECLS

#endif
