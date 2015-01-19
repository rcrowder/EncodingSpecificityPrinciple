/**********************
 *
 * File:		UtNoiseMod.h
 * Purpose:		Amplitude modulates an input signal with a noise masker.
 * Comments:	Written using ModuleProducer version 1.6.1 (Nov 10 2008).
 * Author:		L. P. O'Mard
 * Created:		17 Nov 2008
 * Updated:
 * Copyright:	(c) 2010 Lowel P. O'Mard
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

#ifndef _UTNOISEMOD_H
#define _UTNOISEMOD_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_AMPMOD_NOISE_MOD_NAME			wxT("UTIL_AMPMOD_NOISE")
#define UTILITY_AMPMOD_NOISE_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_AMPMOD_NOISE_BANDWIDTHMODE,
	UTILITY_AMPMOD_NOISE_BANDWIDTH,
	UTILITY_AMPMOD_NOISE_RANSEED

} NAmpModParSpecifier;

typedef enum {

	UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_HZ,
	UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_ERB,
	UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_NULL

} UtilityBandwidthModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		bandwidthMode;
	Float	bandwidth;
	long	ranSeed;

	/* Private members */
	UniParListPtr	parList;
	int		*kLow, *kUpp;
	Float	*normFactor;
	FFTArrayPtr		fTInv;

} NAmpMod, *NAmpModPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	NAmpModPtr	nAmpModPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

NameSpecifier *	BandwidthModeList_Utility_AmpMod_Noise(int index);

BOOLN	CheckData_Utility_AmpMod_Noise(EarObjectPtr data);

BOOLN	FreeProcessVariables_Utility_AmpMod_Noise(void);

BOOLN	Free_Utility_AmpMod_Noise(void);

UniParListPtr	GetUniParListPtr_Utility_AmpMod_Noise(void);

BOOLN	InitModule_Utility_AmpMod_Noise(ModulePtr theModule);

BOOLN	Init_Utility_AmpMod_Noise(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_AmpMod_Noise(void);

BOOLN	Process_Utility_AmpMod_Noise(EarObjectPtr data);

BOOLN	SetBandwidthMode_Utility_AmpMod_Noise(WChar * theBandwidthMode);

BOOLN	SetBandwidth_Utility_AmpMod_Noise(Float theBandwidth);

BOOLN	SetMode_Utility_AmpMod_Noise(WChar * theMode);

BOOLN	SetParsPointer_Utility_AmpMod_Noise(ModulePtr theModule);

BOOLN	SetRanSeed_Utility_AmpMod_Noise(long theRanSeed);

BOOLN	SetUniParList_Utility_AmpMod_Noise(void);

__END_DECLS

#endif
