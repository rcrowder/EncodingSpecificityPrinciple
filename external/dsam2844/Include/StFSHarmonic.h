/**********************
 *
 * File:		StFSHarmonic.h
 * Purpose:		Stimulus which consists of a harmonic series of untapered
 * 				linearly swept tones.
 * Comments:	Written using ModuleProducer version 1.5.0 (Feb 22 2007).
 * 				The stimulus formulation is taken from Aldridge, David F.,
 * 				“Mathematics of Linear Sweeps”, Canadian Journal of
 * 				Exploration Geophysics, (1992) Vol 28,  No. 1, p.62-69.
 * Author:		L. P. O'Mard
 * Created:		12 Mar 2007
 * Updated:
 * Copyright:	(c) 2007, 2010 Lowel P. O'Mard
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

#ifndef _STFSHARMONIC_H
#define _STFSHARMONIC_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define HARMONIC_FREQSWEPT_MOD_NAME			wxT("STIM_HARMONIC_FREQSWEPT")
#define HARMONIC_FREQSWEPT_NUM_PARS			9
#define HARMONIC_FREQSWEPT_NUM_CHANNELS		1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	HARMONIC_FREQSWEPT_LOWESTHARMONIC,
	HARMONIC_FREQSWEPT_HIGHESTHARMONIC,
	HARMONIC_FREQSWEPT_PHASEMODE,
	HARMONIC_FREQSWEPT_PHASEVARIABLE,
	HARMONIC_FREQSWEPT_INITIALFREQ,
	HARMONIC_FREQSWEPT_FINALFREQ,
	HARMONIC_FREQSWEPT_INTENSITY,
	HARMONIC_FREQSWEPT_DURATION,
	HARMONIC_FREQSWEPT_SAMPLINGINTERVAL

} FSHarmParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		lowestHarmonic;
	int		highestHarmonic;
	int		phaseMode;
	Float	phaseVariable;
	Float	initialFreq;
	Float	finalFreq;
	Float	intensity;
	Float	duration, dt;

	/* Private members */
	NameSpecifier	*phaseModeList;
	UniParListPtr	parList;
	long	ranSeed;
	Float	*phase;
	Float	*harmonicFrequency;
	Float	*sweepRate;

} FSHarm, *FSHarmPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FSHarmPtr	fSHarmPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Harmonic_FreqSwept(EarObjectPtr data);

BOOLN	FreeProcessVariables_Harmonic_FreqSwept(void);

BOOLN	Free_Harmonic_FreqSwept(void);

BOOLN	GenerateSignal_Harmonic_FreqSwept(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_Harmonic_FreqSwept(void);

BOOLN	InitModule_Harmonic_FreqSwept(ModulePtr theModule);

BOOLN	InitProcessVariables_Harmonic_FreqSwept(EarObjectPtr data);

BOOLN	Init_Harmonic_FreqSwept(ParameterSpecifier parSpec);

BOOLN	PrintPars_Harmonic_FreqSwept(void);

BOOLN	SetDuration_Harmonic_FreqSwept(Float theDuration);

BOOLN	SetFinalFreq_Harmonic_FreqSwept(Float theFinalFreq);

BOOLN	SetHighestHarmonic_Harmonic_FreqSwept(int theHighestHarmonic);

BOOLN	SetInitialFreq_Harmonic_FreqSwept(Float theInitialFreq);

BOOLN	SetIntensity_Harmonic_FreqSwept(Float theIntensity);

BOOLN	SetLowestHarmonic_Harmonic_FreqSwept(int theLowestHarmonic);

BOOLN	SetParsPointer_Harmonic_FreqSwept(ModulePtr theModule);

BOOLN	SetPhaseMode_Harmonic_FreqSwept(WChar * thePhaseMode);

BOOLN	SetPhaseVariable_Harmonic_FreqSwept(Float thePhaseVariable);

BOOLN	SetSamplingInterval_Harmonic_FreqSwept(Float theSamplingInterval);

BOOLN	SetUniParList_Harmonic_FreqSwept(void);

__END_DECLS

#endif
