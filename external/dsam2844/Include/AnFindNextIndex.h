/**********************
 *
 * File:		AnFindNextIndex.h
 * Purpose:		This module finds the next minimum after a specified offset in
 *				each channel of a signal, and returns the respective index for
 *				each channel.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				The agorithm looks for the first minimum after a negative
 *				gradient. A negative gradient is where cPtr(i + 1) - cPtr(i)
 *				< 0.
 * Author:		L. P. O'Mard
 * Created:		6 Jun 1996
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

#ifndef _ANFINDNEXTINDEX_H
#define _ANFINDNEXTINDEX_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_FINDNEXTINDEX_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_FINDNEXTINDEX_MODE,
	ANALYSIS_FINDNEXTINDEX_TIMEOFFSET

} FindIndexParSpecifier;

typedef enum {

	FIND_INDEX_MINIMUM,
	FIND_INDEX_MAXIMUM,
	FIND_INDEX_NULL

} FindIndexModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;
	Float	timeOffset;

	/* Private members */
	NameSpecifier *modeList;
	UniParListPtr	parList;
	BOOLN	findMinimum;
	ChanLen	offsetIndex;

} FindIndex, *FindIndexPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FindIndexPtr	findIndexPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_FindNextIndex(EarObjectPtr data);

BOOLN	CheckData_Analysis_FindNextIndex(EarObjectPtr data);

BOOLN	Free_Analysis_FindNextIndex(void);

UniParListPtr	GetUniParListPtr_Analysis_FindNextIndex(void);

BOOLN	Init_Analysis_FindNextIndex(ParameterSpecifier parSpec);

BOOLN	InitModeList_Analysis_FindNextIndex(void);

BOOLN	PrintPars_Analysis_FindNextIndex(void);

BOOLN	SetMode_Analysis_FindNextIndex(WChar *theMode);

BOOLN	InitModule_Analysis_FindNextIndex(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_FindNextIndex(ModulePtr theModule);

BOOLN	SetTimeOffset_Analysis_FindNextIndex(Float theTimeOffset);

BOOLN	SetUniParList_Analysis_FindNextIndex(void);

__END_DECLS

#endif
