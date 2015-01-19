/**********************
 *
 * File:		StBPTone.h
 * Purpose:		This module contains the methods for the multiple pure-tone
 *				signal generation paradigm.
 * Comments:	Written using ModuleProducer version 1.2.0 (Nov  6 1998).
 *				06-11-98 LPO: This version of the module was created to make it
 *				easier to create the universal parameter lists.  This is because
 *				the arrays are assumed to have a length of two, rather than
 *				there being a parameter which defined the lengths.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 1998
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

#ifndef _STBPTONE_H
#define _STBPTONE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PURETONE_Binaural_NUM_PARS			7
#define BINAURAL_PTONE_CHANNELS				2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	PURETONE_Binaural_LEFTFREQUENCY,
	PURETONE_Binaural_RIGHTFREQUENCY,
	PURETONE_Binaural_LEFTINTENSITY,
	PURETONE_Binaural_RIGHTINTENSITY,
	PURETONE_Binaural_PHASEDIFFERENCE,
	PURETONE_Binaural_DURATION,
	PURETONE_Binaural_SAMPLINGINTERVAL

} BPureToneParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	Float	leftFrequency;
	Float	rightFrequency;
	Float	leftIntensity;
	Float	rightIntensity;
	Float	phaseDifference;
	Float	duration;
	Float	dt;

	/* Private members */
	UniParListPtr	parList;

} BPureTone, *BPureTonePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BPureTonePtr	bPureTonePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_PureTone_Binaural(EarObjectPtr data);

BOOLN	Free_PureTone_Binaural(void);

BOOLN	GenerateSignal_PureTone_Binaural(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_PureTone_Binaural(void);

BOOLN	Init_PureTone_Binaural(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_Binaural(void);

BOOLN	SetDuration_PureTone_Binaural(Float theDuration);

BOOLN	SetLeftFrequency_PureTone_Binaural(Float theLeftFrequency);

BOOLN	SetLeftIntensity_PureTone_Binaural(Float theLeftIntensity);

BOOLN	InitModule_PureTone_Binaural(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone_Binaural(ModulePtr theModule);

BOOLN	SetPhaseDifference_PureTone_Binaural(Float thePhaseDifference);

BOOLN	SetRightFrequency_PureTone_Binaural(Float theRightFrequency);

BOOLN	SetRightIntensity_PureTone_Binaural(Float theRightIntensity);

BOOLN	SetSamplingInterval_PureTone_Binaural(Float theSamplingInterval);

BOOLN	SetUniParList_PureTone_Binaural(void);

__END_DECLS

#endif
