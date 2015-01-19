/**********************
 *
 * File:		UtFilters.h
 * Purpose:		This module contains the functions that initialise and
 *				generate various filters., e.g. the  2-pole GammaTone filters,
 *				the 2-pole Butterworth low pass filter, the Bessel low pass
 *				filter and the non-linear compresion filter. (They were
 *				revised from M.A. Stone's HUMAN.c 8-10-92 (Exp. Psych.
 *				Cambridge, England).
 * Comments:	Some IIR filters are used, which performs z plane filters for
 *				two-ploe coefficients. A digital band pass filter has been
 *				added.
 *				29-08-95 E.A. Lopez-Poveda: According to the theory, DSP,
 *				William D. Stanley, Reston press, 1st. Ed., p. 172, tan(theta)
 *				must always be positive, ie, the argument of the tangent must
 *				be always smaller than PI / 2. This is being checked here in
 *				all relevant routines.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		26 Jan 1997
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

#ifndef	_UTFILTERS_H
#define _UTFILTERS_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef PI
# define PI 3.1415926538
#endif
#ifndef Pi
# define Pi PI
#endif
#define Piby2 (PI/2.)
#define TwoPi (2.* Pi)
#define LOWPASS 1
#define HIGHPASS 0
#define DELAY_SF		(Float) (NGAMMA - 1)	/* Delay scale factor to
												 * time align FB output */
#define FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER	2 /* - per cascaded filter*/
#define FILTERS_NUM_IIR2_STATE_VARS			2
#define FILTERS_NUM_BP_STATE_VARS			4
#define FILTERS_NUM_IIR2CONT_STATE_VARS		4
#define FILTERS_NUM_CONTBUTT1_STATE_VARS	2
#define FILTERS_NUM_CONTBUTT2_STATE_VARS	4

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef struct {

	int		cascade;				/* The cascade of the filter */
	Float	a0, a1;
	Float	b1, b2;
	Float	*stateVector;

} GammaToneCoeffs, *GammaToneCoeffsPtr;

typedef struct {

	int		cascade;
	Float	b1, b2;
	Float	a0, a1, a2;
	Float	gainLossFactor;
	Float	*state;

} TwoPoleCoeffs, *TwoPoleCoeffsPtr;

typedef struct {

	int		cascade;
	Float	j;
	Float	k;
	Float	l;
	Float	gainLossFactor;
	Float	*state;

} BandPassCoeffs, *BandPassCoeffsPtr;

/*
 * Second order segmented Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 */

typedef struct {

	int		cascade;
	Float	cC;
	Float	twoC;
	Float	dD;
	Float	eE;
	Float	gainLossFactor;
	Float	*state;

} ContButtCoeffs, *ContButtCoeffsPtr;

/*
 * First order segmented Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 */

typedef struct {

	Float	gG;
	Float	gGG;
	Float	hH;
	Float	gainLossFactor;
	Float	state[FILTERS_NUM_CONTBUTT1_STATE_VARS];

} ContButt1Coeffs, *ContButt1CoeffsPtr;


/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	Float	Filters_bess2Poly[];         	/* Bessel coefficents */

extern	Float	Filters_butt2Poly[];			/* Butterworth */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	BandPass_Filters(SignalDataPtr theSignal, BandPassCoeffsPtr p[]);

Float	BandPassFD_Filters(Float freq, Float lowerCutOffFreq,
		  Float upperCutOffFreq, int order);

BOOLN	BrokenStick1Compression_Filters(SignalDataPtr theSignal, Float aA,
		  Float bB, Float cC);

BOOLN	BrokenStick1Compression2_Filters(SignalDataPtr theSignal, Float *aA,
		  Float *bB, Float cC);

BOOLN	Compression_Filters(SignalDataPtr theSignal, Float nrwthr,
		  Float nrwcr);

void	FreeBandPassCoeffs_Filters(BandPassCoeffsPtr *p);

void	FreeGammaToneCoeffs_Filters(GammaToneCoeffsPtr *p);

void	FreeIIR2ContCoeffs_Filters(ContButtCoeffsPtr *p);

void	FreeTwoPoleCoeffs_Filters(TwoPoleCoeffsPtr *p);

BOOLN	GammaTone_Filters(SignalDataPtr theSignal, GammaToneCoeffsPtr p[]);

BOOLN	IIR2_Filters(SignalDataPtr theSignal, TwoPoleCoeffsPtr p[]);

BOOLN	IIR1Cont_Filters(SignalDataPtr theSignal, ContButt1CoeffsPtr p[]);

BOOLN	IIR2Cont_Filters(SignalDataPtr theSignal,
		  ContButtCoeffsPtr pArray[]);

BOOLN	IIR1ContSingle_Filters(SignalDataPtr theSignal, ContButt1CoeffsPtr p);

BandPassCoeffsPtr	InitBandPassCoeffs_Filters(int cascade,
					  Float lowerCutOffFreq, Float upperCutOffFreq,
					  Float dt);

ContButtCoeffsPtr	InitIIR2ContCoeffs_Filters(int cascade,
					  Float cutOffFrequency, Float samplingInterval,
					  int highOrLowPass);

ContButt1CoeffsPtr	InitIIR1ContCoeffs_Filters(Float cutOffFrequency,
					  Float samplingInterval, int highOrLowPass);

GammaToneCoeffsPtr	InitGammaToneCoeffs_Filters(Float centreFreq,
					  Float bWidth3dB, int cascade, Float sampleClk);

TwoPoleCoeffsPtr 	InitIIR2Coeffs_Filters(Float *splane, int cascade,
					  Float f3dB, Float fs, int low_or_high);

		/* latter generates z plane coefficients from 2 pole blocks, f3dB is
		 * the corner frequency, fs sampling freq, coeffs points to an array
		 * the corner the corner splane contains the coefficients of the
		 * denominator polynomial. */

BOOLN	InversePowerCompression_Filters(SignalDataPtr theSignal, Float shift,
		  Float slope);

BOOLN	UptonBStick1Compression_Filters(SignalDataPtr theSignal, Float aA,
			Float bB, Float cC, Float dD);

void	ZeroArray_Filters(Float *p, int length);

__END_DECLS

#endif
