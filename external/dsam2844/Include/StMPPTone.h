/**********************
 *
 * File:		StMPPTone.h
 * Purpose:		The module generates a signal which contains multiple puretone
 *				pulses at different frequencies.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		12 Mar 1997
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

#ifndef	_STMPPTONE_H
#define _STMPPTONE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_MULTIPULSE_NUM_PARS		8
#define	PURE_TONE_3_NUM_CHANNELS	1		/* No. of stimulus channels to
											 * initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PURETONE_MULTIPULSE_NUMPULSES,
	PURETONE_MULTIPULSE_FREQUENCIES,
	PURETONE_MULTIPULSE_INTENSITY,
	PURETONE_MULTIPULSE_BEGINPERIODDURATION,
	PURETONE_MULTIPULSE_PULSEDURATION,
	PURETONE_MULTIPULSE_REPETITIONPERIOD,
	PURETONE_MULTIPULSE_DURATION,
	PURETONE_MULTIPULSE_SAMPLINGINTERVAL

} PureTone4ParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	int		numPulses;
	Float	intensity, duration, dt;
	Float	beginPeriodDuration, pulseDuration, repetitionPeriod;
	Float	*frequencies;

	/* Private members */
	UniParListPtr	parList;
	BOOLN	pulseOn;
	int		pulseNumber;
	ChanLen	pulseCount, beginIndex;

} PureTone4, *PureTone4Ptr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	PureTone4Ptr	pureTone4Ptr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	AllocNumPulses_PureTone_MultiPulse(int numPulses);

BOOLN	CheckData_PureTone_MultiPulse(EarObjectPtr data);

BOOLN	Free_PureTone_MultiPulse(void);

BOOLN	GenerateSignal_PureTone_MultiPulse(EarObjectPtr theObject);

Float	GetIndividualFreq_PureTone_MultiPulse(int index);

UniParListPtr	GetUniParListPtr_PureTone_MultiPulse(void);

BOOLN	Init_PureTone_MultiPulse(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_MultiPulse(void);

BOOLN	SetBeginPeriodDuration_PureTone_MultiPulse(
		  Float theBeginPeriodDuration);

BOOLN	SetDefaultNumPulsesArrays_PureTone_MultiPulse(void);

BOOLN	SetDuration_PureTone_MultiPulse(Float theDuration);

BOOLN	SetIntensity_PureTone_MultiPulse(Float theIntensity);

BOOLN	SetIndividualFreq_PureTone_MultiPulse(int theIndex, Float theFreq);

BOOLN	SetFrequencies_PureTone_MultiPulse(Float *theFrequencies);

BOOLN	InitModule_PureTone_MultiPulse(ModulePtr theModule);

BOOLN	SetNumPulses_PureTone_MultiPulse(int theNumPulses);

BOOLN	SetParsPointer_PureTone_MultiPulse(ModulePtr theModule);

BOOLN	SetPulseDuration_PureTone_MultiPulse(Float thePulseDuration);

BOOLN	SetRepetitionPeriod_PureTone_MultiPulse(Float theRepetitionPeriod);

BOOLN	SetSamplingInterval_PureTone_MultiPulse(Float theSamplingInterval);

BOOLN	SetUniParList_PureTone_MultiPulse(void);

__END_DECLS

#endif
