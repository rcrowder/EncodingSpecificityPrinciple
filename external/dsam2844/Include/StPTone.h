/**********************
 *
 * File:		StPTone.h
 * Purpose:		This module contains the methods for the simple pure-tone
 *				signal generation paradigm.
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

#ifndef	_STPTONE_H
#define _STPTONE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_NUM_PARS			4
#define	PURETONE_NUM_CHANNELS		1	/* No. of channels to initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PURETONE_FREQUENCY,
	PURETONE_INTENSITY,
	PURETONE_DURATION,
	PURETONE_SAMPLINGINTERVAL

} PureToneParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	Float	frequency, intensity;
	Float	duration, dt;

	/* Private members */
	UniParListPtr	parList;

} PureTone, *PureTonePtr;

/*********************** External Variables ***********************************/

extern	PureTonePtr	pureTonePtr;

/*********************** Function Prototypes **********************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_PureTone(void);

BOOLN	GenerateSignal_PureTone(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_PureTone(void);

BOOLN	Init_PureTone(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone(void);

BOOLN	SetDuration_PureTone(Float theDuration);

BOOLN	SetFrequency_PureTone(Float theFrequency);

BOOLN	SetIntensity_PureTone(Float theIntensity);

BOOLN	InitModule_PureTone(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone(ModulePtr theModule);

BOOLN	SetSamplingInterval_PureTone(Float theSamplingInterval);

BOOLN	SetUniParList_PureTone(void);

__END_DECLS

#endif
