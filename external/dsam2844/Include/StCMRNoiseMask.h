/**********************
 *
 * File:		StCMRNoiseMask.h
 * Purpose:		CMR stimulus - a signal with noise flankers.
 * Comments:	Written using ModuleProducer version 1.6.1 (Sep 24 2008).
 * 				Revised from the NeuroSound software code: Stim_CMN.
 *				This stimulus module has had additional controls added in order
 *				to reproduce the Pierzycki and Seeber BSA 2008 stimulus
 * 				The ERB spacing and bandwidth mode has been added, in addition
 * 				to the re-introduction of the modulation frequency, which was
 * 				not previously being used and a signal gating mode.
 * Author:		L. P. O'Mard
 * Created:		30 Sep 2008
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

#ifndef _STCMRNOISEMASK_H
#define _STCMRNOISEMASK_H 1

#include "UtCmplxM.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define CMR_NOISEMASKER_MOD_NAME			wxT("STIM_CMR_NOISEMASKER")
#define CMR_NOISEMASKER_NUM_PARS			26
#define CMR_NOISEMASKER_MAXNTOTAL			9	/* Maximum number of flanking bands */
#define CMR_NOISEMASKER_SIG_MOD_OFFSET_CYCLES	3.0

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	CMR_NOISEMASKER_BINAURALMODE,
	CMR_NOISEMASKER_CONDITION,
	CMR_NOISEMASKER_ONFREQEAR,
	CMR_NOISEMASKER_SIGEAR,
	CMR_NOISEMASKER_FLANKEAR,
	CMR_NOISEMASKER_NLOW,
	CMR_NOISEMASKER_NUPP,
	CMR_NOISEMASKER_NGAPLOW,
	CMR_NOISEMASKER_NGAPUPP,
	CMR_NOISEMASKER_RANSEED,
	CMR_NOISEMASKER_LOWFBLEVEL,
	CMR_NOISEMASKER_UPPFBLEVEL,
	CMR_NOISEMASKER_BATTEN,
	CMR_NOISEMASKER_FBMODPHASE,
	CMR_NOISEMASKER_SPACINGTYPE,
	CMR_NOISEMASKER_SPACING,
	CMR_NOISEMASKER_BANDWIDTHMODE,
	CMR_NOISEMASKER_BANDWIDTH,
	CMR_NOISEMASKER_MSKMODFREQ,
	CMR_NOISEMASKER_MSKMODPHASE,
	CMR_NOISEMASKER_SIGLEVEL,
	CMR_NOISEMASKER_SIGFREQ,
	CMR_NOISEMASKER_SIGGATEMODE,
	CMR_NOISEMASKER_GATETIME,
	CMR_NOISEMASKER_DURATION,
	CMR_NOISEMASKER_SAMPLINGINTERVAL

} CMRNoiseMParSpecifier;

typedef enum {

	CMR_NOISEMASKER_BANDWIDTHMODE_HZ,
	CMR_NOISEMASKER_BANDWIDTHMODE_ERB,
	CMR_NOISEMASKER_BANDWIDTHMODE_NULL

} CMRBandwidthModeSpecifier;

typedef enum {

	CMR_NOISEMASKER_CONDITION_CD,
	CMR_NOISEMASKER_CONDITION_CM,
	CMR_NOISEMASKER_CONDITION_DV,
	CMR_NOISEMASKER_CONDITION_RF,
	CMR_NOISEMASKER_CONDITION_SO,
	CMR_NOISEMASKER_CONDITION_NULL

} CMRNoiseMConditionSpecifier;

typedef enum {

	CMR_NOISEMASKER_SIGGATEMODE_COS,
	CMR_NOISEMASKER_SIGGATEMODE_GAUSSIAN,
	CMR_NOISEMASKER_SIGGATEMODE_NULL

} CMRSigGateModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		binauralMode;
	int		spacingType;
	int		condition;
	int		onFreqEar;
	int		sigEar;
	WChar	flankEar[MAXLINE];
	int		nlow;
	int		nupp;
	int		nGapLow;
	int		nGapUpp;
	long	ranSeed;
	Float	lowFBLevel;
	Float	uppFBLevel;
	Float	oFMLevel;
	Float	fBModPhase;
	Float	spacing;
	int		bandwidthMode;
	Float	bandwidth;
	Float	mskModFreq;
	Float	mskModPhase;
	Float	sigLevel;
	Float	sigFreq;
	int		sigGateMode;
	Float	gateTime;
	Float	duration, dt;

	/* Private members */
	UniParListPtr	parList;
	FFTArrayPtr		fTInv;

} CMRNoiseM, *CMRNoiseMPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CMRNoiseMPtr	cMRNoiseMPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

NameSpecifier *	BandwidthModeList_CMR_NoiseMasker(int index);

BOOLN	CheckData_CMR_NoiseMasker(EarObjectPtr data);

NameSpecifier *	ConditionList_CMR_NoiseMasker(int index);

BOOLN	FreeProcessVariables_CMR_NoiseMasker(void);

BOOLN	Free_CMR_NoiseMasker(void);

BOOLN	GenerateSignal_CMR_NoiseMasker(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_CMR_NoiseMasker(void);

BOOLN	InitModule_CMR_NoiseMasker(ModulePtr theModule);

BOOLN	InitProcessVariables_CMR_NoiseMasker(EarObjectPtr data);

BOOLN	Init_CMR_NoiseMasker(ParameterSpecifier parSpec);

BOOLN	PrintPars_CMR_NoiseMasker(void);

BOOLN	SetOFMLevel_CMR_NoiseMasker(Float theOFMLevel);

BOOLN	SetBandwidthMode_CMR_NoiseMasker(WChar * theBandwidthMode);

BOOLN	SetBinauralMode_CMR_NoiseMasker(WChar * theBinauralMode);

BOOLN	SetCondition_CMR_NoiseMasker(WChar * theCondition);

BOOLN	SetDuration_CMR_NoiseMasker(Float theDuration);

BOOLN	SetEnabledPars_CMR_NoiseMasker(void);

BOOLN	SetFBModPhase_CMR_NoiseMasker(Float theFBModPhase);

BOOLN	SetFlankEar_CMR_NoiseMasker(WChar *theFlankEar);

BOOLN	SetGateTime_CMR_NoiseMasker(Float theGateTime);

BOOLN	SetBandwidth_CMR_NoiseMasker(Float theBandwidth);

BOOLN	SetLowFBLevel_CMR_NoiseMasker(Float theLowFBLevel);

BOOLN	SetMskModFreq_CMR_NoiseMasker(Float theMskModFreq);

BOOLN	SetMskModPhase_CMR_NoiseMasker(Float theMskModPhase);

BOOLN	SetNGapLow_CMR_NoiseMasker(int theNGapLow);

BOOLN	SetNGapUpp_CMR_NoiseMasker(int theNGapUpp);

BOOLN	SetNlow_CMR_NoiseMasker(int theNlow);

BOOLN	SetNupp_CMR_NoiseMasker(int theNupp);

BOOLN	SetOnFreqEar_CMR_NoiseMasker(WChar * theOnFreqEar);

BOOLN	SetParsPointer_CMR_NoiseMasker(ModulePtr theModule);

BOOLN	SetRanSeed_CMR_NoiseMasker(long theRanSeed);

BOOLN	SetSamplingInterval_CMR_NoiseMasker(Float theSamplingInterval);

BOOLN	SetSigLevel_CMR_NoiseMasker(Float theSigLevel);

BOOLN	SetSigEar_CMR_NoiseMasker(WChar * theSigEar);

BOOLN	SetSigFreq_CMR_NoiseMasker(Float theSigFreq);

BOOLN	SetSigGateMode_CMR_NoiseMasker(WChar * theSigGateMode);

BOOLN	SetSpacingType_CMR_NoiseMasker(WChar * theSpacingType);

BOOLN	SetSpacing_CMR_NoiseMasker(Float theSpacing);

BOOLN	SetUniParList_CMR_NoiseMasker(void);

BOOLN	SetUppFBLevel_CMR_NoiseMasker(Float theUppFBLevel);

NameSpecifier *	SigGateModeList_CMR_NoiseMasker(int index);

__END_DECLS

#endif
