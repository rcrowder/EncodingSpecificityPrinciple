/**********************
 *
 * File:		UtGcFilters.h
 * Purpose:		This module contains the functions that initialise, generate,
 *				control various filters.
 * Comments:	This module is based on subroutine of Package of GCFBv1.04a.
 * Author:		Masashi Unoki
 * Created:		21 Nov 2000
 * Updated:		07 Dec 2000
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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

#ifndef	_UTGCFILTERS_H
#define _UTGCFILTERS_H	1

#include "UtFFT.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef PI
# define PI 3.1415926538
#endif
#ifndef TwoPI
# define TwoPI (2* PI)
#endif
#ifndef winSizeERB
# define winSizeERB	3
#endif

#define GCFILTERS_NUM_CASCADE_ACF_FILTER			4 	/* cascadeAC */
#define GCFILTERS_NUM_CASCADE_ERBGT_FILTER			4 	/* cascadeAC */
#define GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER		2 	/* - per cascaded filter*/
#define GCFILTERS_NUM_LI_STATE_VARS_PER_FILTER		1 	/* - per cascaded filter*/
#define GCFILTERS_NUM_CNTL_STATE_VARS_PER_FILTER	1 /* - per cascaded filter*/
#define GCFILTERS_NUM_FRQ_RSL						1024 /* Frequency Resolution */

#define GCFILTERS_REF_FREQ				1000.0	/* Reference frequency */
#define	GCFILTERS_ACF_P0				2.0;
#define	GCFILTERS_ACF_P4				1.0724;
#define	GCFILTERS_ACF_NUM_COEFFS		3
#define	GCFILTERS_ACF_LEN_STATE_VECTOR	3

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#define MAX(x,y) ((x)<(y)? (y):(x))
#define MIN(x,y) ((x)<(y)? (x):(y))

#define GCFILTERS_P1(B, C)	(1.7818 * (1 - 0.0791 * (B)) * (1 - 0.1655 * fabs(C)))
#define GCFILTERS_P2(B, C)	(0.5689 * (1 - 0.1620 * (B)) * (1 - 0.0857 * fabs(C)))
#define GCFILTERS_P3(B, C)	(0.2523 * (1 - 0.0244 * (B)) * (1 + 0.0574 * fabs(C)))

/* The following macro estimates fr from fpeak: Revised from matlab Fr2Fpeak.m */
#define GCFILTERS_FR2FPEAK(N, B, C, FR, ERBW) ((FR) + (C) * (ERBW) * (B) / (N))

/******************************************************************************/
/*************************** enum Definitions ********************************/
/******************************************************************************/

enum {

	GCFILTERS_PGC_FORWARD_PLAN,
	GCFILTERS_PGC_BACKWARD_PLAN

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	GCFILTER_CARRIER_MODE_COMPLEX,
	GCFILTER_CARRIER_MODE_COS,
	GCFILTER_CARRIER_MODE_ENVELOPE,
	GCFILTER_CARRIER_MODE_SIN,
	GCFILTER_CARRIER_MODE_NULL

} GCCarrierModeSpecifier;

typedef struct {

	int	cascade;				/* The cascade of the filter */
	Float	*numCoeffs;			/* feed feedforward coeffs */
	Float	*denomCoeffs;		/* feed feedback coeffs    */
	Float	gainLossFactor;
	Float	*stateFVector;
	Float	*stateBVector;

} AsymCmpCoeffs, *AsymCmpCoeffsPtr;

typedef struct {

	int		numFilt;			/* The cascade of the filter */
	Float	fs;					/* Sampling rate */
	Float	p0, p1, p2, p3, p4;
	Float	b, c;
	Float	*bz;				/* MA coefficients  (NumCh*3*NumFilt) */
	Float	*ap;				/* AR coefficients  (NumCh*3*NumFilt) */
	Float	*sigInPrev;			/* Input state vector */
	Float	*sigOutPrev;		/* Output state vector */
	Float	*y;					/* Points to result output */
	BandwidthModePtr	bMode;

} AsymCmpCoeffs2, *AsymCmpCoeffs2Ptr;

typedef struct {

	int	cascade;
	Float	*a0, *a1, *a2;		/* feed feedforward coeffs */
	Float	*b1, *b2;			/* feed feedback coeffs    */
	Float	gainLossFactor;
	Float	*stateVector;

} ERBGammaToneCoeffs, *ERBGammaToneCoeffsPtr;

typedef struct {

	int	cascade;
	Float	a0, a1;			/* feed feedforward coeffs */
	Float	b1;				/* feed feedback coeffs    */
	Float	gainLossFactor;
	Float	*stateVector;

} OnePoleCoeffs, *OnePoleCoeffsPtr;

typedef struct {

	Float 	outSignalLI;
	Float	aEst;			/* Estimated a */
	Float	cEst;			/* Estimated c */
	Float	psEst;			/* Estimated Ps */

} CntlGammaC, *CntlGammaCPtr;

#if HAVE_FFTW3
	typedef struct {

		FFTArrayPtr	pGC;
		FFTArrayPtr	pGCOut;

	} GammaChirpCoeffs, *GammaChirpCoeffsPtr;
#endif /* HAVE_FFTW3 */

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	Float	Filters_AsymCmpCoef0[];        	/* ACF coefficents */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

/*************************** HAVE_FFTW3 Prototypes ****************************/

#if HAVE_FFTW3
	void	FreePGammaChirpCoeffs_GCFilters(GammaChirpCoeffsPtr *p);

	GammaChirpCoeffsPtr	InitPGammaChirpCoeffs_GCFilters(Float cF, Float bw,
						  Float sR, Float orderG, Float coefERBw, Float coefC,
						  Float phase, int swCarr, int swNorm,
						  SignalDataPtr inSignal);

	void	PassiveGCFilter_GCFilters(EarObjectPtr data, GammaChirpCoeffsPtr *pGCoeffs);

#endif /* HAVE_FFTW3 */

void	ACFilterBank_GCFilters(AsymCmpCoeffs2Ptr *aCFCoeffs, EarObjectPtr data,
		  int chanOffset, int numChannels, ChanLen sample);

void	AsymCmp_GCFilters(SignalDataPtr theSignal, ChanLen nsmpl,
						AsymCmpCoeffsPtr p[]);

void	ASympCmpFreqResp_GCFilters(Float *asymFunc, Float frs, Float fs, Float b,
		  Float c, BandwidthModePtr bMode);

NameSpecifier *	PGCCarrierList_GCFilters(int index);

BOOLN	CheckCntlInit_GCFilters(CntlGammaCPtr *cntlGammaC);

void	ERBGammaTone_GCFilters(SignalDataPtr theSignal,
						ERBGammaToneCoeffsPtr p[]);

void	FreeAsymCmpCoeffs_GCFilters(AsymCmpCoeffsPtr *p);

void	FreeAsymCmpCoeffs2_GCFilters(AsymCmpCoeffs2Ptr *p);

void	FreeERBGammaToneCoeffs_GCFilters(ERBGammaToneCoeffsPtr *p);

void	FreeOwoPoleCoeffs_GCFilters(OnePoleCoeffsPtr *p);

void	FreeCntlGammaChirp_GCFilters(CntlGammaCPtr *p);

void	FreeHammingWindow(Float *p);

void	FreeERBWindow_GCFilters(Float *p);

void	FreeLeakyIntCoeffs_GCFilters(OnePoleCoeffsPtr *p);

void	GammaChirpAmpFreqResp_GCFilters(Float *ampFrsp, Float frs, Float eRBw,
		  Float sR, Float orderG, Float coefERBw, Float coefC, Float phase);

AsymCmpCoeffs2Ptr	InitAsymCmpCoeffs2_GCFilters(int cascade, Float fs,
					  Float b, Float c, BandwidthModePtr bMode);


void	LeakyInt_GCFilters(CntlGammaCPtr p[], OnePoleCoeffsPtr q[],
		  int numChannels);

void	ResetAsymCmpCoeffs2State_GCFilters(AsymCmpCoeffs2Ptr p);

void	SetAsymCmpCoeffs2_GCFilters(AsymCmpCoeffs2Ptr p, Float frs);

void	SetpsEst_GCFilters(CntlGammaCPtr cntlGammaC[], int numChannels,
		  Float *winPsEst, Float coefPsEst);

void	SetcEst_GCFilters(CntlGammaCPtr cntlGammaC[], int numChannels,
		  Float cCoeff0, Float cCoeff1, Float cLowerLim, Float cUpperLim);

void 	SetaEst_GCFilters(CntlGammaCPtr cntlGammaC[], int numChannels,
		  Float cmprs);

void	CntlGammaChirp_GCFilters(SignalDataPtr theSignal, ChanLen nsmpl,
		  CntlGammaCPtr cntlGammaC[], Float cCoeff0, Float cCoeff1,
		  Float cLowerLim, Float cUpperLim, Float *winPsEst,
		  Float coefPsEst, Float cmprs, OnePoleCoeffsPtr coefficientsLI[]);

void	CalcAsymCmpCoeffs_GCFilters(AsymCmpCoeffsPtr p, Float centreFreq,
		  Float bWidth3dB, Float bCoeff, Float cCoeff, int cascade,
		  Float sampleClk);

AsymCmpCoeffsPtr	InitAsymCmpCoeffs_GCFilters(void);

OnePoleCoeffsPtr 	InitLeakyIntCoeffs_GCFilters(Float Tcnst, Float sampleClk);

ERBGammaToneCoeffsPtr	InitERBGammaToneCoeffs_GCFilters(Float centreFreq,
						  Float bWidth3dB, Float bCoeff, int cascade,
						  Float sampleClk);

CntlGammaCPtr	InitCntlGammaChirp_GCFilters(void);

Float*	InitERBWindow_GCFilters(Float eRBDensity, int numChannels);

BOOLN	CheckInit_CntlGammaC(CntlGammaCPtr cntlGammaC[]);

__END_DECLS

#endif
