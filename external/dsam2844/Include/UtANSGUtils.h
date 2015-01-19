/**********************
 *
 * File:		UtANSGUtils.h
 * Purpose:		This file contains the auditory nerve spike distribution
 * 				management and other utility routines.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jun 2009
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

#ifndef UTANSGUTILS_H_
#define UTANSGUTILS_H_ 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	ANSGUTILS_DISTRIBUTION_ALPHA_WAVE_LIMIT_COEFF	8.25

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define ANSGUTILS_GAUSSIAN(V, X, M)	(exp(-SQR((X) - (M)) / (2.0 * (V))) / \
		  sqrt((V) * PIx2))

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSGUTILS_DISTRIBUTION_DBL_GAUSSIAN_MODE,
	ANSGUTILS_DISTRIBUTION_GAUSSIAN_MODE,
	ANSGUTILS_DISTRIBUTION_STANDARD_MODE,
	ANSGUTILS_DISTRIBUTION_USER_MODE,
	ANSGUTILS_DISTRIBUTION_NULL

} ANSGDistSpecifier;

typedef enum {

	ANSGUTILS_DISTRIBUTION_OUTPUTMODE_SQUARE_PULSE,
	ANSGUTILS_DISTRIBUTION_OUTPUTMODE_ALPHA_WAVE,
	ANSGUTILS_DISTRIBUTION_OUTPUTMODE_NULL

} ANSGDistOutputModeSpecifier;

typedef enum {

	ANSGUTILS_GAUSS_NUM_FIBRES,
	ANSGUTILS_GAUSS_MEAN,
	ANSGUTILS_GAUSS_VAR1,
	ANSGUTILS_GAUSS_VAR2

} ANSGDistGaussianSpecifier;

typedef struct {

	int		numChannels;
	int		*numFibres;

} ANSGDist, *ANSGDistPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	AddRemainingPulse_ANSGUtils(ChanData *outPtr, Float *pulse,
		  ChanLen *pulseIndexPtr, ChanLen pulseDurationIndex);

Float	CalcPulseDuration_ANSGUtils(int outputMode, Float durationCoeff);

ChanLen	CalcPulseDurationIndex_ANSGUtils(int outputMode, Float duration,
		  Float dt);

BOOLN	CheckFuncPars_ANSGUtils(ParArrayPtr p, SignalDataPtr signal);

BOOLN	Init_ANSGUtils(ANSGDistPtr *p, int numChannels);

void	Free_ANSGUtils(ANSGDistPtr *p);

Float *	GeneratePulse_ANSGUtils(int outputMode, int durationIndex,
		  Float timeToPeak, Float magnitude, Float dt);

int		GetDistFuncValue_ANSGUtils(ParArrayPtr p, int numChannels, int chan);

int		GetNumDistributionPars_ANSGUtils(int mode);

NameSpecifier *	ModeList_ANSGUtils(int index);

NameSpecifier *	OutputModeList_ANSGUtils(int index);

void	PrintFibres_ANSGUtils(FILE *fp, const WChar *prefix, int *fibres,
		  Float *frequencies, int numChannels);

void	SetDefaultDistribution_ANSGUtils(ParArrayPtr distribution);

BOOLN	SetFibres_ANSGUtils(ANSGDistPtr *aNDist, ParArrayPtr p, Float *frequencies,
		  int numChannels);

void	SetStandardNumFibres_ANSGUtils(ParArrayPtr distribution, int numFibres);

__END_DECLS

#endif /* UTANSGUTILS_H_ */
