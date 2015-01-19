/**********************
 *
 * File:		AnAverages.h
 * Purpose:		This routine calculates the average value for each channel.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				This module assumes that a negative timeRange assumes a
 *				period to the end of the signal.
 * Author:		L. P. O'Mard
 * Created:		21 May 1996
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

#ifndef _ANAVERAGES_H
#define _ANAVERAGES_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_AVERAGES_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_AVERAGES_MODE,
	ANALYSIS_AVERAGES_TIMEOFFSET,
	ANALYSIS_AVERAGES_TIMERANGE

} AveragesParSpecifier;

typedef	enum {

	AVERAGES_FULL,
	AVERAGES_NEGATIVE_WAVE,
	AVERAGES_POSITIVE_WAVE,
	AVERAGES_NULL

} AveragesModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;
	Float	timeOffset;
	Float	timeRange;

	/* Private members */
	NameSpecifier *modeList;
	UniParListPtr	parList;
	ChanLen	timeOffsetIndex, timeRangeIndex;

} Averages, *AveragesPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	AveragesPtr	averagesPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_Averages(EarObjectPtr data);

BOOLN	CheckData_Analysis_Averages(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_Analysis_Averages(void);

BOOLN	Free_Analysis_Averages(void);

BOOLN	Init_Analysis_Averages(ParameterSpecifier parSpec);

BOOLN	InitModeList_Analysis_Averages(void);

BOOLN	PrintPars_Analysis_Averages(void);

BOOLN	SetMode_Analysis_Averages(WChar *theMode);

BOOLN	InitModule_Analysis_Averages(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_Averages(ModulePtr theModule);

BOOLN	SetTimeRange_Analysis_Averages(Float theTimeRange);

BOOLN	SetTimeOffset_Analysis_Averages(Float theTimeOffset);

BOOLN	SetUniParList_Analysis_Averages(void);

__END_DECLS

#endif
