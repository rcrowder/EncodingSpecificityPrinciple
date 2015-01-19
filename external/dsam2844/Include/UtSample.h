/**********************
 *
 * File:		UtSample.h
 * Purpose:		This module samples a signal at selected intervals.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 18 1996).
 * Author:		L. P. O'Mard
 * Created:		22 Oct 1996
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

#ifndef _UTSAMPLE_H
#define _UTSAMPLE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_SAMPLE_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_SAMPLE_TIMEOFFSET,
	UTILITY_SAMPLE_SAMPLINGINTERVAL

} SampleParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	Float	timeOffset;
	Float	dt;

	/* Private members */
	UniParListPtr	parList;
	ChanLen	dtIndex, timeOffsetIndex;

} Sample, *SamplePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SamplePtr	samplePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_Sample(EarObjectPtr data);

BOOLN	Free_Utility_Sample(void);

UniParListPtr	GetUniParListPtr_Utility_Sample(void);

BOOLN	Init_Utility_Sample(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_Sample(void);

BOOLN	Process_Utility_Sample(EarObjectPtr data);

BOOLN	InitModule_Utility_Sample(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_Sample(ModulePtr theModule);

BOOLN	SetSamplingInterval_Utility_Sample(Float theSamplingInterval);

BOOLN	SetTimeOffset_Utility_Sample(Float theTimeOffset);

BOOLN	SetUniParList_Utility_Sample(void);

__END_DECLS

#endif
