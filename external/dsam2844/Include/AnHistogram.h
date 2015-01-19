/**********************
 *
 * File:		AnHistogram.h
 * Purpose:		This program calculates period or post-stimulus time histograms.
 * Comments:	Written using ModuleProducer version 1.3.
 *				A negative binwidth will instruct instruct the process to use
 *				the previous signal's dt value.
 *				As for most of the analysis routines, this routine is not
 *				segmented mode friendly.
 *				13-02-98 LPO: Corrected time offset checking.
 *				04-03-99 LPO: Corrected the double precsision error using SGI's
 *				by introducing the 'DBL_GREATER macro'.
 * Author:		L. P. O'Mard
 * Created:		17 Nov 1995
 * Updated:		04 Mar 1999
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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

#ifndef _ANHISTOGRAM_H
#define _ANHISTOGRAM_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_HISTOGRAM_NUM_PARS				7
#define ANALYSIS_HISTOGRAM_NUM_SUB_PROCESSES	1

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_HISTOGRAM_DATABUFFER

} AnHistogramSubProcessSpecifier;

typedef enum {

	ANALYSIS_HISTOGRAM_DETECTIONMODE,
	ANALYSIS_HISTOGRAM_OUTPUT_MODE,
	ANALYSIS_HISTOGRAM_TYPEMODE,
	ANALYSIS_HISTOGRAM_EVENTTHRESHOLD,
	ANALYSIS_HISTOGRAM_BINWIDTH,
	ANALYSIS_HISTOGRAM_PERIOD,
	ANALYSIS_HISTOGRAM_TIMEOFFSET

} HistogramParSpecifier;

typedef enum {

	HISTOGRAM_DETECT_SPIKES,
	HISTOGRAM_CONTINUOUS,
	HISTOGRAM_DETECT_NULL

} HistogramDetectionSpecifier;

typedef enum {

	HISTOGRAM_OUTPUT_BIN_COUNTS,
	HISTOGRAM_OUTPUT_SPIKE_RATE,
	HISTOGRAM_OUTPUT_NULL

} HistogramOutputSpecifier;

typedef enum {

	HISTOGRAM_PSTH,
	HISTOGRAM_PH,
	HISTOGRAM_TYPE_NULL

} HistogramTypeSpecifier;

typedef struct {

	ChanLen	numPeriods;
	ChanLen	offsetIndex;
	ChanLen	extraSample;
	ChanLen	bufferSamples;

} HistogramState, *HistogramStatePtr;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		detectionMode;
	int		outputMode;
	int		typeMode;
	Float	eventThreshold;
	Float	binWidth;
	Float	period;
	Float	timeOffset;

	/* Private members */
	NameSpecifier	*detectionModeList;
	NameSpecifier	*outputModeList;
	NameSpecifier	*typeModeList;
	UniParListPtr	parList;
	BOOLN			*riseDetected;
	Float			wBinWidth, wPeriod, dt;
	int				numInitialisedThreads;
	HistogramState	*hState;
	EarObjectPtr	dataBuffer;

} Histogram, *HistogramPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	HistogramPtr	histogramPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_Histogram(EarObjectPtr data);

BOOLN	CheckData_Analysis_Histogram(EarObjectPtr data);

BOOLN	Free_Analysis_Histogram(void);

BOOLN	FreeProcessVariables_Analysis_Histogram(void);

UniParListPtr	GetUniParListPtr_Analysis_Histogram(void);

BOOLN	Init_Analysis_Histogram(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Analysis_Histogram(EarObjectPtr data);

BOOLN	InitDetectionModeList_Analysis_Histogram(void);

BOOLN	InitOutputModeList_Analysis_Histogram(void);

BOOLN	InitTypeModeList_Analysis_Histogram(void);

BOOLN	PrintPars_Analysis_Histogram(void);

void	PushDataBuffer_Analysis_Histogram(EarObjectPtr data,
		  ChanLen lastSamples);

void	ResetProcess_Analysis_Histogram(EarObjectPtr data);

BOOLN	SetBinWidth_Analysis_Histogram(Float theBinWidth);

BOOLN	SetDetectionMode_Analysis_Histogram(WChar *theDetectionMode);

BOOLN	SetEventThreshold_Analysis_Histogram(Float theEventThreshold);

BOOLN	SetOutputMode_Analysis_Histogram(WChar *theOutputMode);

BOOLN	SetTimeOffset_Analysis_Histogram(Float theTimeOffset);

BOOLN	InitModule_Analysis_Histogram(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_Histogram(ModulePtr theModule);

BOOLN	SetPeriod_Analysis_Histogram(Float thePeriod);

BOOLN	SetTypeMode_Analysis_Histogram(WChar *theTypeMode);

BOOLN	SetUniParList_Analysis_Histogram(void);

__END_DECLS

#endif
