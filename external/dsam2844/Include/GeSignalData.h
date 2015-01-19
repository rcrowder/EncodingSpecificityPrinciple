/******************
 *
 * File:		SignalData.c
 * Purpose:		This module contains the methods for the SignalData class.
 * Comments:	02-04-95 LPO: changed the data structure so that channels are
 *				allocated dynamically - without the MAX_CHANNELS restriction -
 *				as suggested by Trevor Shackleton.
 *				10-04-97 LPO: Introduced info.cFArray - to only hold CF for
 *				modules which need this information, e.g Carney.
 *				10-11-97 LPO: Introduced the staticTimeFlag parameter for the
 *				SignalDaata structure. When set the _WorldTime_EarObject macro
 *				always sets the time to PROCESS_START_TIME.
 *				30-06-98 LPO: Introduced the numWindowFrames parameter.
 *				This will define the number of windows in the signal.
 *				23-09-98 LPO: Removed the InitCopy_SignalData routine as it
 *				was not used and contained a serious bug.
 * Authors:		L. P. O'Mard
 * Created:		17 Feb 1993
 * Updated:		23 Sep 1998
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

#ifndef	_GESIGNALDATA_H
#define _GESIGNALDATA_H	1

#include <UtNameSpecs.h>

/******************************************************************************/
/*************************** Constant definitions *****************************/
/******************************************************************************/

#define	SIGNALDATA_DEFAULT_INTERLEAVE_LEVEL		1
#define	SIGNALDATA_DEFAULT_NUM_WINDOW_FRAMES	1
#define	SIGNALDATA_MAX_TITLE					80	/* Title for axis labels. */

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

#define	_GetOutputTime_SignalData(SIGNAL, SAMPLE) ((SAMPLE) * (SIGNAL)->dt +\
		  (SIGNAL)->outputTimeOffset)

#define	_GetDuration_SignalData(SIGNAL) ((SIGNAL)->length * (SIGNAL)->dt)

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	SIGNALDATA_LIMIT_MODE_OCTAVE,
	SIGNALDATA_LIMIT_MODE_CHANNEL,
	SIGNALDATA_LIMIT_MODE_NULL

} SignalDataLimitModeSpecifier;

typedef struct {

	Float	*chanLabel;					/* Contains channel labels, e.g. CF */
	Float	*cFArray;					/* Contains only CF information. */
	WChar	channelTitle[SIGNALDATA_MAX_TITLE];	/* Channel axis title */
	WChar	chanDataTitle[SIGNALDATA_MAX_TITLE];/* Channel axis title */
	WChar	sampleTitle[SIGNALDATA_MAX_TITLE];	/* Sample (e.g. dt) axis title*/

} SignalInfo, *SignalInfoPtr;

typedef struct {

	BOOLN	dtFlag;			/* Set flags to guard against errors */
	BOOLN	lengthFlag;		/* and horrible bugs. */
	BOOLN	rampFlag;		/* Checks that signals have been ramped. */
	BOOLN	localInfoFlag;	/* Set if memory was created locally for info. */
	BOOLN	staticTimeFlag;	/* Set to make _WorldTime_EarObject static. */
	BOOLN	externalDataFlag;/* Set when channel memory comes from elsewhere. */
	uShort	numChannels;	/* No. of channels for signal. */
	uShort	origNumChannels;/* Original no. of channels for signal. */
	uShort	offset;			/* Channel offset for threaded processing. */
	uShort	interleaveLevel;/* For post BM binaural processing.*/
	uShort	numWindowFrames;/* For frame processing modues. */
	ChanLen	length;			/* The length of the signal store in datapoint */
	ChanLen	timeIndex;		/* Used for printing output. */
	Float	dt;				/* Sampling interval = 1/(Frame rate) */
	Float	outputTimeOffset;/* t = {sample no.} * dt  - timeOffset. */
	SignalInfo	info;		/* Contains channel lebels, titles etc. */
	ChanData	**channel;	/* Signal data channels */
	ChanData	*block;		/* The signal channels stored in contiguous block. */

} SignalData, *SignalDataPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Add_SignalData(SignalDataPtr a, SignalDataPtr b);

BOOLN	Delay_SignalData(SignalDataPtr signal, Float delay);

BOOLN	Divide_SignalData(SignalDataPtr a, SignalDataPtr b);

BOOLN	CheckInit_SignalData(SignalDataPtr theSignal,
		  const WChar *callingFunction);

BOOLN	CheckPars_SignalData(SignalDataPtr theSignal);

BOOLN	CheckRamp_SignalData(SignalDataPtr theSignal);

BOOLN	CopyInfo_SignalData(SignalDataPtr a, SignalDataPtr b);

int		FindCFIndex_SignalData(SignalDataPtr signal, int minIndex, int maxIndex,
		  Float frequency);

void	Free_SignalData(SignalDataPtr *theData);

void	FreeChannels_SignalData(SignalDataPtr theData);

BOOLN	GaindB_SignalData(SignalDataPtr d, Float gaindB);

BOOLN	GaindBIndividual_SignalData(SignalDataPtr d, Float gaindB[]);

BOOLN	GetChannelLimits_SignalData(SignalDataPtr signal, int *minChan,
		  int *maxChan, Float lowerLimit, Float upperLimit,
		  SignalDataLimitModeSpecifier mode);

Float	GetDuration_SignalData(SignalDataPtr theSignal);

Float	GetOutputTime_SignalData(SignalDataPtr theSignal, ChanLen sample);

void	GetWindowLimits_SignalData(SignalDataPtr signal, int *minChan,
		  int *maxChan, Float frequency, Float lowerLimit, Float upperLimit,
		  SignalDataLimitModeSpecifier mode);

SignalDataPtr	Init_SignalData(const WChar *callingFunctionName);

BOOLN	InitChannels_SignalData(SignalDataPtr theData, uShort numChannels,
		  BOOLN externalDataFlag);

BOOLN	InitInfo_SignalData(SignalInfoPtr info);

NameSpecifier *	LimitModeList_SignalData(int index);

BOOLN	OutputToFile_SignalData(WChar *fileName, SignalDataPtr theData);

BOOLN	ResetInfo_SignalData(SignalDataPtr signal);

BOOLN	Scale_SignalData(SignalDataPtr d, Float multiplier);

BOOLN	SameType_SignalData_NoDiagnostics(SignalDataPtr a, SignalDataPtr b);

BOOLN	SameType_SignalData(SignalDataPtr a, SignalDataPtr b);

void	SetChannelsFromSignal_SignalData(SignalDataPtr theSignal,
		  SignalDataPtr supplier);

BOOLN	SetInfoCF_SignalData(SignalDataPtr theData, int index, Float cF);

void	SetInfoCFArray_SignalData(SignalDataPtr theData, Float *cFs);

void	SetInfoChannelLabels_SignalData(SignalDataPtr signal, Float *labels);

BOOLN	SetInfoChannelLabel_SignalData(SignalDataPtr theData, int index,
		  Float label);

void	SetInfoChanDataTitle_SignalData(SignalDataPtr theData, WChar *title);

void	SetInfoChannelTitle_SignalData(SignalDataPtr theData, WChar *title);

void	SetInfoSampleTitle_SignalData(SignalDataPtr theData, WChar *title);

DSAM_API void	SetInterleaveLevel_SignalData(SignalDataPtr theData,
		  uShort theInterleaveLevel);

void	SetLength_SignalData(SignalDataPtr theData, ChanLen theLength);

DSAM_API void	SetLocalInfoFlag_SignalData(SignalDataPtr theData, BOOLN flag);

void	SetNumWindowFrames_SignalData(SignalDataPtr theData,
		  uShort theNumWindowFrames);

void	SetOutputTimeOffset_SignalData(SignalDataPtr theData,
		  Float theOutputTimeOffset);

void	SetSamplingInterval_SignalData(SignalDataPtr theData,
		  Float theSamplingInterval);

DSAM_API void	SetStaticTimeFlag_SignalData(SignalDataPtr signal, BOOLN flag);

void	SetTimeIndex_SignalData(SignalDataPtr theData, ChanLen theTimeIndex);

__END_DECLS

#endif
