/**********************
 *
 * File:		StEGatedTone.h
 * Purpose:		This module generates exponential damped or ramped sinusoids.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 * Author:		L. P. O'Mard
 * Created:		14 Dec 1997
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

#ifndef _STEGATEDTONE_H
#define _STEGATEDTONE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define EGATED_TONE_NUM_PARS		11
#define	EGATED_TONE_NUM_CHANNELS	1		/* No. of stimulus channels. */

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	EGATED_TONE_TYPEMODE,
	EGATED_TONE_FLOORMODE,
	EGATED_TONE_CARRIERFREQUENCY,
	EGATED_TONE_AMPLITUDE,
	EGATED_TONE_PHASE,
	EGATED_TONE_BEGINPERIODDURATION,
	EGATED_TONE_REPETITIONRATE,
	EGATED_TONE_HALFLIFE,
	EGATED_TONE_FLOOR,
	EGATED_TONE_DURATION,
	EGATED_TONE_SAMPLINGINTERVAL

} EGatedToneParSpecifier;

typedef enum {

	EGATED_TONE_RAMPED_MODE,
	EGATED_TONE_DAMPED_MODE,
	EGATED_TONE_MODE_NULL

} TypeModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		typeMode;
	int		floorMode;
	Float	carrierFrequency;
	Float	amplitude;
	Float	phase;
	Float	beginPeriodDuration;
	Float	repetitionRate;
	Float	halfLife;
	Float	floor;
	Float	duration;
	Float	dt;

	/* Private members */
	NameSpecifier	*typeModeList;
	UniParListPtr	parList;
	Float	cycleTimer;
	Float	nextCycle;

} EGatedTone, *EGatedTonePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	EGatedTonePtr	eGatedTonePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_Stimulus_ExpGatedTone(void);

BOOLN	GenerateSignal_Stimulus_ExpGatedTone(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_Stimulus_ExpGatedTone(void);

BOOLN	Init_Stimulus_ExpGatedTone(ParameterSpecifier parSpec);

BOOLN	InitTypeModeList_Stimulus_ExpGatedTone(void);

BOOLN	PrintPars_Stimulus_ExpGatedTone(void);

BOOLN	SetAmplitude_Stimulus_ExpGatedTone(Float theAmplitude);

BOOLN	SetBeginPeriodDuration_Stimulus_ExpGatedTone(Float theBeginPeriodDuration);

BOOLN	SetFloorMode_Stimulus_ExpGatedTone(WChar *theFloorMode);

BOOLN	SetDuration_Stimulus_ExpGatedTone(Float theDuration);

BOOLN	SetFloor_Stimulus_ExpGatedTone(Float theFloor);

BOOLN	SetCarrierFrequency_Stimulus_ExpGatedTone(Float theCarrierFrequency);

BOOLN	SetHalfLife_Stimulus_ExpGatedTone(Float theHalfLife);

BOOLN	InitModule_Stimulus_ExpGatedTone(ModulePtr theModule);

BOOLN	SetParsPointer_Stimulus_ExpGatedTone(ModulePtr theModule);

BOOLN	SetPhase_Stimulus_ExpGatedTone(Float thePhase);

BOOLN	SetRepetitionRate_Stimulus_ExpGatedTone(Float theRepetitionRate);

BOOLN	SetSamplingInterval_Stimulus_ExpGatedTone(Float theSamplingInterval);

BOOLN	SetTypeMode_Stimulus_ExpGatedTone(WChar *theTypeMode);

BOOLN	SetUniParList_Stimulus_ExpGatedTone(void);

__END_DECLS

#endif
