/**********************
 *
 * File:		UtStrobe.h
 * Purpose:		This module implements strobe criteria for the AIM stabilised
 *				auditory image (SAI)
 *				See Patterson R. D. and Allerhand M. H. (1995) "Time-domain
 *				modeling of peripheral auditory processing: A modular
 *				Architecture and a software platform", J. Acoust. Soc. Am. 98,
 *				1890-1894.
 * Comments:	Written using ModuleProducer version 1.12 (Oct  9 1997).
 *				The "Threshold" strobe mode algorithm is the same as the
 *				"user" mode at present.  This is because the "User" mode is
 *				interpreted differently by the calling SAI module program.
 *				This method saves having to have a separate set of mode names
 *				in the SAI module.
 *				20-11-97 LPO: Peak strobe criteria now only strobes when the
 *				peak is above the threshold.
 *				21-11-97 LPO: Changed "lagTime" to "delay".
 *				25-11-97 LPO: Implemented the peak shadow "-" mode.
 *				10-12-97 LPO: After a long battle I have finished implementing
 *				the peak shadow "+" mode.  Note that the definition for this
 *				criterion is different from that described in the AIM code
 *				(though even in the AIM code is was not implemented.)
 *				11-12-97 LPO: Added the delayTimeout constraint.  This causes
 *				the most recent strobe peak to be set, regardless of the
 *				delayCount state.
 *				06-07-99 LPO: This module now sets the 'staticTimeFlag', as
 *				it sets the 'outputTimeOffset' field,
 * Author:		L. P. O'Mard
 * Created:		10 Oct 1997
 * Updated:		11 Sep 1997
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

#ifndef _UTSTROBE_H
#define _UTSTROBE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define STROBE_NUM_PARS			6
#define STROBE_SPIKE_UNIT		1.0

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	STROBE_TYPE_MODE,
	STROBE_DIAGNOSTIC_MODE,
	STROBE_THRESHOLD,
	STROBE_THRESHOLD_DECAY_RATE,
	STROBE_DELAY,
	STROBE_DELAY_TIMEOUT

} StrobeParSpecifier;

typedef	enum {

	STROBE_USER_MODE,
	STROBE_THRESHOLD_MODE,
	STROBE_PEAK_MODE,
	STROBE_PEAK_SHADOW_NEGATIVE_MODE,
	STROBE_PEAK_SHADOW_POSITIVE_MODE,
	STROBE_DELTA_GAMMA_MODE,
	STROBE_MODE_NULL

} StrobeModeSpecifier;

typedef	enum {

	STROBE_PROCESS_BUFFER_CHANNEL,
	STROBE_PROCESS_DATA_CHANNEL

} StrobeChanProcessSpecifier;

typedef struct {

	BOOLN		gradient;
	BOOLN		strobeAlreadyPlaced;
	Float		threshold;
	Float		deltaThreshold;
	ChanLen		widthIndex;
	ChanLen		delayCount;
	ChanLen		delayTimeoutCount;
	ChanLen		prevPeakIndex;
	ChanData	prevPeakHeight;
	ChanData	lastSample;
	ChanData	*lastInput;

} StrobeState, *StrobeStatePtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		typeMode;
	int		diagnosticMode;
	Float	threshold;
	Float	thresholdDecayRate;
	Float	delay;
	Float	delayTimeout;

	/* Private members */
	NameSpecifier	*diagnosticModeList;
	NameSpecifier	*typeModeList;
	UniParListPtr	parList;
	WChar			diagnosticString[MAX_FILE_PATH];
	int				numChannels;
	Float			thresholdDecay;
	FILE			*fp;
	ChanLen			numLastSamples;
	ChanLen			delayTimeoutSamples;
	StrobeStatePtr	*stateVars;

} Strobe, *StrobePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	StrobePtr	strobePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_Strobe(EarObjectPtr data);

BOOLN	Free_Utility_Strobe(void);

void	FreeProcessVariables_Utility_Strobe(void);

void	FreeStateVariables_Utility_Strobe(StrobeStatePtr *p);

UniParListPtr	GetUniParListPtr_Utility_Strobe(void);

BOOLN	Init_Utility_Strobe(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Utility_Strobe(EarObjectPtr data);

StrobeStatePtr	InitStateVariables_Utility_Strobe(ChanLen numLastSamples);

BOOLN	InitTypeModeList_Utility_Strobe(void);

BOOLN	PrintPars_Utility_Strobe(void);

BOOLN	Process_Utility_Strobe(EarObjectPtr data);

void	ProcessPeakChannel_Utility_Strobe(EarObjectPtr data,
		  StrobeChanProcessSpecifier chanProcessSpecifier);

void	ProcessPeakModes_Utility_Strobe(EarObjectPtr data);

void	ProcessThesholdModes_Utility_Strobe(EarObjectPtr data);

BOOLN	SetDiagnosticMode_Utility_Strobe(WChar *theDiagnosticMode);

BOOLN	SetDelay_Utility_Strobe(Float theDelay);

BOOLN	SetDelayTimeout_Utility_Strobe(Float theDelayTimeout);

BOOLN	SetTypeMode_Utility_Strobe(WChar *theTypeMode);

BOOLN	InitModule_Utility_Strobe(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_Strobe(ModulePtr theModule);

BOOLN	SetThresholdDecayRate_Utility_Strobe(
		  Float theThresholdDecayRate);

BOOLN	SetThreshold_Utility_Strobe(Float theThreshold);

BOOLN	SetUniParList_Utility_Strobe(void);

__END_DECLS

#endif
