/**********************
 *
 * File:		StCMRHarmMask.h
 * Purpose:		Comodulation Masking Release stimulus with harmonic noise flankers.
 * Comments:	Written using ModuleProducer version 1.6.1 (Oct 23 2008).
 * 				Revised from the NeuroSound software code: Stim_CMR.
 * Author:		L. P. O'Mard
 * Created:		03 Nov 2008
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

#ifndef _STCMRHARMMASK_H
#define _STCMRHARMMASK_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define CMR_HARMONICMASKER_MOD_NAME			wxT("STIM_CMR_HARMONICMASKER")
#define CMR_HARMONICMASKER_NUM_PARS			21
#define CMR_HARMONICMASKER_MAXNTOTAL		9
#define CMR_HARMONICMASKER_PHASE_FACTOR		0.745

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	CMR_HARMONICMASKER_BINAURALMODE,
	CMR_HARMONICMASKER_SPACINGTYPE,
	CMR_HARMONICMASKER_CONDITION,
	CMR_HARMONICMASKER_ONFREQEAR,
	CMR_HARMONICMASKER_SIGEAR,
	CMR_HARMONICMASKER_SIGMODE,
	CMR_HARMONICMASKER_FLANKEAR,
	CMR_HARMONICMASKER_NLOW,
	CMR_HARMONICMASKER_NUPP,
	CMR_HARMONICMASKER_NGAPLOW,
	CMR_HARMONICMASKER_NGAPUPP,
	CMR_HARMONICMASKER_LOWFBLEVEL,
	CMR_HARMONICMASKER_UPPFBLEVEL,
	CMR_HARMONICMASKER_OFMLEVEL,
	CMR_HARMONICMASKER_SPACING,
	CMR_HARMONICMASKER_MSKMODFREQ,
	CMR_HARMONICMASKER_SIGLEVEL,
	CMR_HARMONICMASKER_SIGFREQ,
	CMR_HARMONICMASKER_GATETIME,
	CMR_HARMONICMASKER_DURATION,
	CMR_HARMONICMASKER_SAMPLINGINTERVAL

} CMRHarmMParSpecifier;

typedef enum {

	CMR_HARMONICMASKER_CONDITION_CD,
	CMR_HARMONICMASKER_CONDITION_CM,
	CMR_HARMONICMASKER_CONDITION_RF,
	CMR_HARMONICMASKER_CONDITION_SO,
	CMR_HARMONICMASKER_CONDITION_UM,
	CMR_HARMONICMASKER_CONDITION_NULL

} CMRHarmMConditionSpecifier;

typedef enum {

	CMR_HARMONICMASKER_SIGMODE_CONT,
	CMR_HARMONICMASKER_SIGMODE_PIPS,
	CMR_HARMONICMASKER_SIGMODE_NULL

} CMRSigModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		binauralMode;
	int		spacingType;
	int		condition;
	int		onFreqEar;
	int		sigEar;
	int		sigMode;
	WChar	flankEar[MAXLINE];
	int		nlow;
	int		nupp;
	int		nGapLow;
	int		nGapUpp;
	Float	lowFBLevel;
	Float	uppFBLevel;
	Float	oFMLevel;
	Float	spacing;
	Float	mskmodfreq;
	Float	sigLevel;
	Float	sigFreq;
	Float	gateTime;
	Float	duration, dt;

	/* Private members */
	NameSpecifier	*conditionList;
	NameSpecifier	*sigModeList;
	UniParListPtr	parList;

} CMRHarmM, *CMRHarmMPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CMRHarmMPtr	cMRHarmMPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_CMR_HarmonicMasker(EarObjectPtr data);

BOOLN	Free_CMR_HarmonicMasker(void);

BOOLN	GenerateSignal_CMR_HarmonicMasker(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_CMR_HarmonicMasker(void);

BOOLN	InitConditionList_CMR_HarmonicMasker(void);

BOOLN	InitModule_CMR_HarmonicMasker(ModulePtr theModule);

BOOLN	InitSigModeList_CMR_HarmonicMasker(void);

BOOLN	Init_CMR_HarmonicMasker(ParameterSpecifier parSpec);

BOOLN	PrintPars_CMR_HarmonicMasker(void);

BOOLN	SetBinauralMode_CMR_HarmonicMasker(WChar * theBinauralMode);

BOOLN	SetCondition_CMR_HarmonicMasker(WChar * theCondition);

BOOLN	SetDuration_CMR_HarmonicMasker(Float theDuration);

BOOLN	SetEnabledPars_CMR_HarmonicMasker(void);

BOOLN	SetFlankEar_CMR_HarmonicMasker(WChar *theFlankEar);

BOOLN	SetGateTime_CMR_HarmonicMasker(Float theGateTime);

BOOLN	SetLowFBLevel_CMR_HarmonicMasker(Float theLowFBLevel);

BOOLN	SetMskmodfreq_CMR_HarmonicMasker(Float theMskmodfreq);

BOOLN	SetNGapLow_CMR_HarmonicMasker(int theNGapLow);

BOOLN	SetNGapUpp_CMR_HarmonicMasker(int theNGapUpp);

BOOLN	SetNlow_CMR_HarmonicMasker(int theNlow);

BOOLN	SetNupp_CMR_HarmonicMasker(int theNupp);

BOOLN	SetOFMLevel_CMR_HarmonicMasker(Float theOFMLevel);

BOOLN	SetOnFreqEar_CMR_HarmonicMasker(WChar * theOnFreqEar);

BOOLN	SetParsPointer_CMR_HarmonicMasker(ModulePtr theModule);

BOOLN	SetSamplingInterval_CMR_HarmonicMasker(Float theSamplingInterval);

BOOLN	SetSigEar_CMR_HarmonicMasker(WChar * theSigEar);

BOOLN	SetSigFreq_CMR_HarmonicMasker(Float theSigFreq);

BOOLN	SetSigLevel_CMR_HarmonicMasker(Float theSigLevel);

BOOLN	SetSigMode_CMR_HarmonicMasker(WChar * theSigMode);

BOOLN	SetSpacingType_CMR_HarmonicMasker(WChar * theSpacingType);

BOOLN	SetSpacing_CMR_HarmonicMasker(Float theSpacing);

BOOLN	SetUniParList_CMR_HarmonicMasker(void);

BOOLN	SetUppFBLevel_CMR_HarmonicMasker(Float theUppFBLevel);

__END_DECLS

#endif
