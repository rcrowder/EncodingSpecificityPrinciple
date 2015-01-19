/**********************
 *
 * File:		StAMTone.h
 * Purpose:		This module contains the methods for the AM-tone (amplitude
 *				modulated) generation paradigm.
 * Comments:	09-07-98 LPO: Carrier frequency is now in sine phase and not cos
 *				phase.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		09 Jul 1998
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

#ifndef	_STAMTONE_H
#define _STAMTONE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_AM_NUM_PARS		6
#define	AM_TONE_NUM_CHANNELS		1		/* No. of stimulus channels to
											 * initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	AM_TONE_FREQUENCY,
	AM_TONE_MODULATIONFREQUENCY,
	AM_TONE_MODULATIONDEPTH,
	AM_TONE_INTENSITY,
	AM_TONE_DURATION,
	AM_TONE_SAMPLINGINTERVAL

} AMToneParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	Float	frequency, modulationFrequency;
	Float  modulationDepth, intensity;
	Float	duration, dt;

	/* Private members */
	UniParListPtr	parList;

} AMTone, *AMTonePtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	AMTonePtr	aMTonePtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_PureTone_AM(void);

BOOLN	GenerateSignal_PureTone_AM(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_PureTone_AM(void);

BOOLN	Init_PureTone_AM(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_AM(void);

BOOLN	SetFrequency_PureTone_AM(Float theFrequency);

BOOLN	SetDuration_PureTone_AM(Float theDuration);

BOOLN	SetIntensity_PureTone_AM(Float theIntensity);

BOOLN	SetModulationFrequency_PureTone_AM(Float theModulationFrequency);

BOOLN	InitModule_PureTone_AM(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone_AM(ModulePtr theModule);

BOOLN	SetModulationDepth_PureTone_AM(Float theModulationDepth);

BOOLN	SetSamplingInterval_PureTone_AM(Float theSamplingInterval);

BOOLN	SetUniParList_PureTone_AM(void);

__END_DECLS

#endif
