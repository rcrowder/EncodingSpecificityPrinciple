/**********************
 *
 * File:		StPTone2.h
 * Purpose:		The module generates a pure-tone signal preceded and ended by
 *				periods of silence.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
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

#ifndef	_STPTONE2_H
#define _STPTONE2_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_2_NUM_PARS			6
#define	PURE_TONE_2_NUM_CHANNELS	1		/* No. of stimulus channels to
											 * initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PURETONE_2_FREQUENCY,
	PURETONE_2_INTENSITY,
	PURETONE_2_DURATION,
	PURETONE_2_SAMPLINGINTERVAL,
	PURETONE_2_BEGINPERIODDURATION,
	PURETONE_2_ENDPERIODDURATION

} PureTone2ParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	Float	frequency, intensity;
	Float	duration, dt;
	Float	beginPeriodDuration, endPeriodDuration;

	/* Private members */
	UniParListPtr	parList;

} PureTone2, *PureTone2Ptr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	PureTone2Ptr	pureTone2Ptr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_PureTone_2(void);

BOOLN	GenerateSignal_PureTone_2(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_PureTone_2(void);

BOOLN	Init_PureTone_2(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_2(void);

BOOLN	SetBeginPeriodDuration_PureTone_2(Float theBeginPeriodDuration);

BOOLN	SetDuration_PureTone_2(Float theDuration);

BOOLN	SetEndPeriodDuration_PureTone_2(Float theEndPeriodDuration);

BOOLN	SetFrequency_PureTone_2(Float theFrequency);

BOOLN	SetIntensity_PureTone_2(Float theIntensity);

BOOLN	InitModule_PureTone_2(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone_2(ModulePtr theModule);

BOOLN	SetSamplingInterval_PureTone_2(Float theSamplingInterval);

BOOLN	SetUniParList_PureTone_2(void);

__END_DECLS

#endif
