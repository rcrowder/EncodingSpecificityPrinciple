/**********************
 *
 * File:		StPulseTrain.h
 * Purpose:		The pulse train stimulus generation module.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				21-11-97 LPO: Amended routine so that the first pulse occurrs
 *				in the first sample of the output.
 * Author:		L. P. O'Mard
 * Created:		04 Mar 1996
 * Updated:		21 Nov 1997
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

#ifndef _STPULSETRAIN_H
#define _STPULSETRAIN_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PULSETRAIN_NUM_PARS			5

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	PULSETRAIN_PULSERATE,
	PULSETRAIN_PULSEDURATION,
	PULSETRAIN_AMPLITUDE,
	PULSETRAIN_DURATION,
	PULSETRAIN_SAMPLINGINTERVAL

} PulseTrainParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	Float	pulseRate;
	Float	pulseDuration;
	Float	amplitude;
	Float	duration;
	Float	dt;

	/* Private members */
	UniParListPtr	parList;
	Float	nextPulseTime, remainingPulseTime;

} PulseTrain, *PulseTrainPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	PulseTrainPtr	pulseTrainPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_PulseTrain(EarObjectPtr data);

BOOLN	Free_PulseTrain(void);

BOOLN	GenerateSignal_PulseTrain(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_PulseTrain(void);

BOOLN	Init_PulseTrain(ParameterSpecifier parSpec);

BOOLN	PrintPars_PulseTrain(void);

BOOLN	SetAmplitude_PulseTrain(Float theAmplitude);

BOOLN	SetDuration_PulseTrain(Float theDuration);

BOOLN	InitModule_PulseTrain(ModulePtr theModule);

BOOLN	SetParsPointer_PulseTrain(ModulePtr theModule);

BOOLN	SetPulseDuration_PulseTrain(Float thePulseDuration);

BOOLN	SetPulseRate_PulseTrain(Float thePulseRate);

BOOLN	SetSamplingInterval_PulseTrain(Float theSamplingInterval);

BOOLN	SetUniParList_PulseTrain(void);

__END_DECLS

#endif
