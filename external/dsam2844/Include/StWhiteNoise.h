/**********************
 *
 * File:		StWhiteNoise.h
 * Purpose:		This module contains the methods for the simple white-noise
 *				signal generation paradigm.
 * Comments:
 * Author:		E. A. Lopez-Poveda
 * Created:		17 Jan 1995
 * Updated:		12 Mar 1997
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

#ifndef	_STWHITENOISE_H
#define _STWHITENOISE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define WHITENOISE_NUM_PARS			7

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	WHITENOISE_NUMCHANNELS,
	WHITENOISE_CORRELATIONDEGREE,
	WHITENOISE_RANDOMIZATIONINDEX,
	WHITENOISE_RANSEED,
	WHITENOISE_INTENSITY,
	WHITENOISE_DURATION,
	WHITENOISE_SAMPLINGINTERVAL

} WhiteNoiseParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	int		numChannels;
	Float	intensity;
	Float	duration;
	Float	dt;
	int		correlationDegree;
	int		randomizationIndex;
	long	ranSeed;

	/* Private members */
	UniParListPtr	parList;

} WhiteNoise, *WhiteNoisePtr;

/********************************* External Variables *************************/

extern	WhiteNoisePtr	whiteNoisePtr;

/********************************* Function Prototypes ************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_WhiteNoise(void);

BOOLN	GenerateSignal_WhiteNoise(EarObjectPtr theObject);

BOOLN	Init_WhiteNoise(ParameterSpecifier parSpec);

UniParListPtr	GetUniParListPtr_WhiteNoise(void);

BOOLN	PrintPars_WhiteNoise(void);

BOOLN	InitModule_WhiteNoise(ModulePtr theModule);

BOOLN	SetNumChannels_WhiteNoise(int theNumChannels);

BOOLN	SetDuration_WhiteNoise(Float theDuration);

BOOLN	SetIntensity_WhiteNoise(Float theIntensity);

BOOLN	SetParsPointer_WhiteNoise(ModulePtr theModule);

BOOLN	SetSamplingInterval_WhiteNoise(Float theSamplingInterval);

BOOLN	SetCorrelationDegree_WhiteNoise(int theCorrelationDegree);

BOOLN	SetRandomizationIndex_WhiteNoise(int theRandomizationIndex);

BOOLN	SetRanSeed_WhiteNoise(long theRanSeed);

BOOLN	SetUniParList_WhiteNoise(void);

__END_DECLS

#endif
