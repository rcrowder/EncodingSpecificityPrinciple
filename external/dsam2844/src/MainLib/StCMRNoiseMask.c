/**********************
 *
 * File:		StCMRNoiseMask.c
 * Purpose:		CMR stimulus - a signal with noise flankers.
 * Comments:	Written using ModuleProducer version 1.6.1 (Sep 24 2008).
 * 				Revised from the NeuroSound software code: Stim_CMN.
 *				This stimulus module has had additional controls added in order
 *				to reproduce the Pierzycki and Seeber BSA 2008 stimulus
 * 				The ERB spacing and bandwidth mode has been added, in addition
 * 				to the re-introduction of the modulation frequency, which was
 * 				not previously being used and a signal gating mode.
 *				The comodulation condition is achieved by using the same noise
 * 				carrier for the flanking bands.  For the codeviant condition
 * 				new noise is created for each flanking band.
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeNSpecLists.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtString.h"
#include "UtFFT.h"
#include "UtBandwidth.h"
#include "UtRandom.h"
#include "FiParFile.h"
#include "StCMRNoiseMask.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CMRNoiseMPtr	cMRNoiseMPtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_CMR_NoiseMasker(void)
{
	/* static const WChar	*funcName = wxT("Free_CMR_NoiseMasker"); */

	if (cMRNoiseMPtr == NULL)
		return(FALSE);
	if (cMRNoiseMPtr->parList)
		FreeList_UniParMgr(&cMRNoiseMPtr->parList);
	if (cMRNoiseMPtr->parSpec == GLOBAL) {
		free(cMRNoiseMPtr);
		cMRNoiseMPtr = NULL;
	}
	return(TRUE);

}

/****************************** BandwidthModeList *****************************/

/*
 * This function initialises the 'bandwidthMode' list array
 */

NameSpecifier *
BandwidthModeList_CMR_NoiseMasker(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("HERTZ"),	CMR_NOISEMASKER_BANDWIDTHMODE_HZ },
			{ wxT("ERB"),	CMR_NOISEMASKER_BANDWIDTHMODE_ERB },
			{ 0,			CMR_NOISEMASKER_BANDWIDTHMODE_NULL },
		};
	return(&modeList[index]);

}

/****************************** ConditionList *********************************/

/*
 * This function initialises the 'p->condition' list array
 */

NameSpecifier *
ConditionList_CMR_NoiseMasker(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("CD"),	CMR_NOISEMASKER_CONDITION_CD },
			{ wxT("CM"),	CMR_NOISEMASKER_CONDITION_CM },
			{ wxT("DV"),	CMR_NOISEMASKER_CONDITION_DV },
			{ wxT("RF"),	CMR_NOISEMASKER_CONDITION_RF },
			{ wxT("SO"),	CMR_NOISEMASKER_CONDITION_SO },
			{ 0,			CMR_NOISEMASKER_CONDITION_NULL },
		};
	return(&modeList[index]);

}

/****************************** SigGateModeList ********************************/

/*
 * This function initialises the 'sigGateMode' list array
 */

NameSpecifier *
SigGateModeList_CMR_NoiseMasker(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("COS"),		CMR_NOISEMASKER_SIGGATEMODE_COS },
			{ wxT("GAUSSIAN"),	CMR_NOISEMASKER_SIGGATEMODE_GAUSSIAN },
			{ 0,				CMR_NOISEMASKER_SIGGATEMODE_NULL },
		};
	return(&modeList[index]);

}

/****************************** Init ******************************************/

/*
 * This function initialises the module by setting module's parameter
 * pointer structure.
 * The GLOBAL option is for hard programming - it sets the module's
 * pointer to the global parameter structure and initialises the
 * parameters. The LOCAL option is for generic programming - it
 * initialises the parameter structure currently pointed to by the
 * module's parameter pointer.
 */

BOOLN
Init_CMR_NoiseMasker(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_CMR_NoiseMasker");

	if (parSpec == GLOBAL) {
		if (cMRNoiseMPtr != NULL)
			Free_CMR_NoiseMasker();
		if ((cMRNoiseMPtr = (CMRNoiseMPtr) malloc(sizeof(CMRNoiseM))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (cMRNoiseMPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	cMRNoiseMPtr->parSpec = parSpec;
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->binauralMode = GENERAL_BOOLEAN_OFF;
	cMRNoiseMPtr->spacingType = GENERAL_SPACINGMODE_LINEAR;
	cMRNoiseMPtr->condition = CMR_NOISEMASKER_CONDITION_RF;
	cMRNoiseMPtr->onFreqEar = GENERAL_EAR_LEFT;
	cMRNoiseMPtr->sigEar = GENERAL_EAR_LEFT;
	DSAM_strcpy(cMRNoiseMPtr->flankEar, wxT("L"));
	cMRNoiseMPtr->nlow = 3;
	cMRNoiseMPtr->nupp = 3;
	cMRNoiseMPtr->nGapLow = 1;
	cMRNoiseMPtr->nGapUpp = 1;
	cMRNoiseMPtr->ranSeed = 0;
	cMRNoiseMPtr->lowFBLevel = 0.0;
	cMRNoiseMPtr->uppFBLevel = 0.0;
	cMRNoiseMPtr->oFMLevel = 60.0;
	cMRNoiseMPtr->fBModPhase = 90.0;
	cMRNoiseMPtr->spacing = 100.0;
	cMRNoiseMPtr->bandwidthMode = CMR_NOISEMASKER_BANDWIDTHMODE_HZ;
	cMRNoiseMPtr->bandwidth = 20.0;
	cMRNoiseMPtr->mskModFreq = 0.0;
	cMRNoiseMPtr->mskModPhase = -90.0;
	cMRNoiseMPtr->sigLevel = 0.0;
	cMRNoiseMPtr->sigFreq = 700.0;
	cMRNoiseMPtr->sigGateMode = CMR_NOISEMASKER_SIGGATEMODE_COS;
	cMRNoiseMPtr->gateTime = 10.0e-3;
	cMRNoiseMPtr->duration = 0.5;
	cMRNoiseMPtr->dt = DEFAULT_DT;

	if (!SetUniParList_CMR_NoiseMasker()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_CMR_NoiseMasker();
		return(FALSE);
	}
	cMRNoiseMPtr->fTInv = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_CMR_NoiseMasker(void)
{
	static const WChar	*funcName = wxT("SetUniParList_CMR_NoiseMasker");
	UniParPtr	pars;

	if ((cMRNoiseMPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  CMR_NOISEMASKER_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = cMRNoiseMPtr->parList->pars;
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_BINAURALMODE], wxT("BINAURAL_MODE"),
	  wxT("Binaural mode ('on' or 'off ')."),
	  UNIPAR_BOOL,
	  &cMRNoiseMPtr->binauralMode, NULL,
	  (void * (*)) SetBinauralMode_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_CONDITION], wxT("CONDITION"),
	  wxT("Stimulus condition mode: ('cd', 'cm', 'dv', 'rf' or 'so'."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->condition, ConditionList_CMR_NoiseMasker(0),
	  (void * (*)) SetCondition_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_ONFREQEAR], wxT("ON_FREQ_EAR"),
	  wxT("On-frequency ear ('left' or 'right')."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->onFreqEar, EarModeList_NSpecLists(0),
	  (void * (*)) SetOnFreqEar_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SIGEAR], wxT("SIGNAL_EAR"),
	  wxT("Signal presentation ear ('left' or 'right')."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->sigEar, EarModeList_NSpecLists(0),
	  (void * (*)) SetSigEar_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_FLANKEAR], wxT("FLANK_EAR"),
	  wxT("Flanker ear string ??"),
	  UNIPAR_STRING,
	  &cMRNoiseMPtr->flankEar, NULL,
	  (void * (*)) SetFlankEar_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_NLOW], wxT("NUM_LOW_BANDS"),
	  wxT("No. lower frequency bands."),
	  UNIPAR_INT,
	  &cMRNoiseMPtr->nlow, NULL,
	  (void * (*)) SetNlow_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_NUPP], wxT("NUM_UPPER_BANDS"),
	  wxT("No. upper frequency bands."),
	  UNIPAR_INT,
	  &cMRNoiseMPtr->nupp, NULL,
	  (void * (*)) SetNupp_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_NGAPLOW], wxT("NUM_LOWER_GAP"),
	  wxT("No. of lower Gaps."),
	  UNIPAR_INT,
	  &cMRNoiseMPtr->nGapLow, NULL,
	  (void * (*)) SetNGapLow_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_NGAPUPP], wxT("NUM_UPPER_GAP"),
	  wxT("No. of upper Gaps."),
	  UNIPAR_INT,
	  &cMRNoiseMPtr->nGapUpp, NULL,
	  (void * (*)) SetNGapUpp_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_RANSEED], wxT("RAN_SEED"),
	  wxT("Random Number Seed."),
	  UNIPAR_LONG,
	  &cMRNoiseMPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_LOWFBLEVEL], wxT("LOWER_FB_LEVEL"),
	  wxT("Lower FB level (dB re OFM)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->lowFBLevel, NULL,
	  (void * (*)) SetLowFBLevel_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_UPPFBLEVEL], wxT("UPPER_FB_LEVEL"),
	  wxT("Upper FB level (dB re OFM)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->uppFBLevel, NULL,
	  (void * (*)) SetUppFBLevel_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_BATTEN], wxT("OFM_LEVEL"),
	  wxT("On-frequency masker level (dB SPL)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->oFMLevel, NULL,
	  (void * (*)) SetOFMLevel_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_FBMODPHASE], wxT("FB_MOD_PHASE"),
	  wxT("Flanker band modulation phase - used only in 'CD' condition."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->fBModPhase, NULL,
	  (void * (*)) SetFBModPhase_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SPACINGTYPE], wxT("SPACING_TYPE"),
	  wxT("Spacing type ('linear', 'octave' or 'ERB')."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->spacingType, SpacingModeList_NSpecLists(0),
	  (void * (*)) SetSpacingType_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SPACING], wxT("SPACING_VAR"),
	  wxT("Spacing variable (Hz, octaves or ERBs)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->spacing, NULL,
	  (void * (*)) SetSpacing_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_BANDWIDTHMODE], wxT("BW_MODE"),
	  wxT("Noise bandwidth mode ('Hz', or 'ERB')."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->bandwidthMode, BandwidthModeList_CMR_NoiseMasker(0),
	  (void * (*)) SetBandwidthMode_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_BANDWIDTH], wxT("BANDWIDTH"),
	  wxT("Bandwidth ('Hz' or 'ERB density')."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->bandwidth, NULL,
	  (void * (*)) SetBandwidth_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_MSKMODFREQ], wxT("MASKER_MOD_FREQ"),
	  wxT("Masker modulation frequency or 0 for none (Hz)"),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->mskModFreq, NULL,
	  (void * (*)) SetMskModFreq_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_MSKMODPHASE], wxT("MASKER_MOD_PHASE"),
	  wxT("Masker modulation phase (degrees)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->mskModPhase, NULL,
	  (void * (*)) SetMskModPhase_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SIGLEVEL], wxT("SIGNAL_LEVEL"),
	  wxT("Signal level (dB SPL)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->sigLevel, NULL,
	  (void * (*)) SetSigLevel_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SIGFREQ], wxT("SIGNAL_FREQ"),
	  wxT("Signal frequency (Hz)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->sigFreq, NULL,
	  (void * (*)) SetSigFreq_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SIGGATEMODE], wxT("SIGNAL_GATE_MODE"),
	  wxT("Signal gate mode ('cos' or 'gaussian')"),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->sigGateMode, SigGateModeList_CMR_NoiseMasker(0),
	  (void * (*)) SetSigGateMode_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_GATETIME], wxT("SIGNAL_GATE_TIME"),
	  wxT("Signal gate time (s)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->gateTime, NULL,
	  (void * (*)) SetGateTime_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->duration, NULL,
	  (void * (*)) SetDuration_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_CMR_NoiseMasker);

	SetEnabledPars_CMR_NoiseMasker();
	return(TRUE);

}

/********************************* SetDefaulEnabledPars ***********************/

/*
 * This routine sets the parameter list so that the correct parameters
 * are enabled/disabled.
 */

BOOLN
SetEnabledPars_CMR_NoiseMasker(void)
{
	static const WChar *funcName = wxT("SetEnabledPars_CMR_NoiseMasker");
	CMRNoiseMPtr	p = cMRNoiseMPtr;

	if (p == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	p->parList->pars[CMR_NOISEMASKER_FLANKEAR].enabled = TRUE;
	p->parList->pars[CMR_NOISEMASKER_NUPP].enabled = TRUE;
	p->parList->pars[CMR_NOISEMASKER_NLOW].enabled = TRUE;
	p->parList->pars[CMR_NOISEMASKER_NGAPLOW].enabled = TRUE;
	p->parList->pars[CMR_NOISEMASKER_NGAPUPP].enabled = TRUE;
	p->parList->pars[CMR_NOISEMASKER_LOWFBLEVEL].enabled = TRUE;
	p->parList->pars[CMR_NOISEMASKER_UPPFBLEVEL].enabled = TRUE;
	p->parList->pars[CMR_NOISEMASKER_SPACING].enabled = TRUE;
	p->parList->pars[CMR_NOISEMASKER_FBMODPHASE].enabled = FALSE;
	switch (p->condition) {
	case CMR_NOISEMASKER_CONDITION_CD:
		p->parList->pars[CMR_NOISEMASKER_FBMODPHASE].enabled = TRUE;
		break;
	case CMR_NOISEMASKER_CONDITION_RF:
	case CMR_NOISEMASKER_CONDITION_SO:
		p->parList->pars[CMR_NOISEMASKER_FLANKEAR].enabled = FALSE;
		p->parList->pars[CMR_NOISEMASKER_NUPP].enabled = FALSE;
		p->parList->pars[CMR_NOISEMASKER_NLOW].enabled = FALSE;
		p->parList->pars[CMR_NOISEMASKER_NGAPLOW].enabled = FALSE;
		p->parList->pars[CMR_NOISEMASKER_NGAPUPP].enabled = FALSE;
		p->parList->pars[CMR_NOISEMASKER_LOWFBLEVEL].enabled = FALSE;
		p->parList->pars[CMR_NOISEMASKER_UPPFBLEVEL].enabled = FALSE;
		p->parList->pars[CMR_NOISEMASKER_SPACING].enabled = FALSE;
		break;
		break;
	default:
		;
	}
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_CMR_NoiseMasker(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (cMRNoiseMPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(cMRNoiseMPtr->parList);

}

/****************************** SetBinauralMode *******************************/

/*
 * This function sets the module's binauralMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinauralMode_CMR_NoiseMasker(WChar * theBinauralMode)
{
	static const WChar	*funcName = wxT("SetBinauralMode_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theBinauralMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName, theBinauralMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->binauralMode = specifier;
	return(TRUE);

}

/****************************** SetSpacingType ********************************/

/*
 * This function sets the module's p->spacingType parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpacingType_CMR_NoiseMasker(WChar * theSpacingType)
{
	static const WChar	*funcName = wxT("SetSpacingType_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSpacingType,
	  SpacingModeList_NSpecLists(0))) == CFLIST_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSpacingType);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->spacingType = specifier;
	return(TRUE);

}

/****************************** SetCondition **********************************/

/*
 * This function sets the module's p->condition parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCondition_CMR_NoiseMasker(WChar * theCondition)
{
	static const WChar	*funcName = wxT("SetCondition_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theCondition,
			ConditionList_CMR_NoiseMasker(0))) == CMR_NOISEMASKER_CONDITION_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theCondition);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->condition = specifier;
	SetEnabledPars_CMR_NoiseMasker();
	return(TRUE);

}

/****************************** SetOnFreqEar **********************************/

/*
 * This function sets the module's onFreqEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOnFreqEar_CMR_NoiseMasker(WChar * theOnFreqEar)
{
	static const WChar	*funcName = wxT("SetOnFreqEar_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOnFreqEar,
	  EarModeList_NSpecLists(0))) == GENERAL_EAR_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOnFreqEar);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->onFreqEar = specifier;
	return(TRUE);

}

/****************************** SetSigEar *************************************/

/*
 * This function sets the module's sigEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigEar_CMR_NoiseMasker(WChar * theSigEar)
{
	static const WChar	*funcName = wxT("SetSigEar_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSigEar,
			EarModeList_NSpecLists(0))) == GENERAL_EAR_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSigEar);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->sigEar = specifier;
	return(TRUE);

}

/****************************** SetSigGateMode ********************************/

/*
 * This function sets the module's sigGateMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigGateMode_CMR_NoiseMasker(WChar * theSigGateMode)
{
	static const WChar	*funcName = wxT("SetSigGateMode_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSigGateMode,
	  SigGateModeList_CMR_NoiseMasker(0))) == CMR_NOISEMASKER_SIGGATEMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSigGateMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->sigGateMode = specifier;
	return(TRUE);

}

/****************************** SetFlankEar ***********************************/

/*
 * This function sets the module's flankEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFlankEar_CMR_NoiseMasker(WChar *theFlankEar)
{
	static const WChar	*funcName = wxT("SetFlankEar_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	DSAM_strcpy(cMRNoiseMPtr->flankEar, theFlankEar);
	return(TRUE);

}

/****************************** SetNlow ***************************************/

/*
 * This function sets the module's p->nlow parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNlow_CMR_NoiseMasker(int theNlow)
{
	static const WChar	*funcName = wxT("SetNlow_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->nlow = theNlow;
	return(TRUE);

}

/****************************** SetNupp ***************************************/

/*
 * This function sets the module's p->nupp parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNupp_CMR_NoiseMasker(int theNupp)
{
	static const WChar	*funcName = wxT("SetNupp_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->nupp = theNupp;
	return(TRUE);

}

/****************************** SetNGapLow ************************************/

/*
 * This function sets the module's nGapLow parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNGapLow_CMR_NoiseMasker(int theNGapLow)
{
	static const WChar	*funcName = wxT("SetNGapLow_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->nGapLow = theNGapLow;
	return(TRUE);

}

/****************************** SetNGapUpp ************************************/

/*
 * This function sets the module's nGapUpp parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNGapUpp_CMR_NoiseMasker(int theNGapUpp)
{
	static const WChar	*funcName = wxT("SetNGapUpp_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->nGapUpp = theNGapUpp;
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_CMR_NoiseMasker(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetLowFBLevel ***********************************/

/*
 * This function sets the module's lowFBLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowFBLevel_CMR_NoiseMasker(Float theLowFBLevel)
{
	static const WChar	*funcName = wxT("SetLowFBLevel_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->lowFBLevel = theLowFBLevel;
	return(TRUE);

}

/****************************** SetUppFBLevel ***********************************/

/*
 * This function sets the module's uppFBLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUppFBLevel_CMR_NoiseMasker(Float theUppFBLevel)
{
	static const WChar	*funcName = wxT("SetUppFBLevel_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->uppFBLevel = theUppFBLevel;
	return(TRUE);

}

/****************************** SetOFMLevel *************************************/

/*
 * This function sets the module's p->oFMLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOFMLevel_CMR_NoiseMasker(Float theOFMLevel)
{
	static const WChar	*funcName = wxT("SetOFMLevel_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->oFMLevel = theOFMLevel;
	return(TRUE);

}

/****************************** SetFBModPhase *********************************/

/*
 * This function sets the module's fBModPhase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFBModPhase_CMR_NoiseMasker(Float theFBModPhase)
{
	static const WChar	*funcName = wxT("SetFBModPhase_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->fBModPhase = theFBModPhase;
	return(TRUE);

}

/****************************** SetSpacing ************************************/

/*
 * This function sets the module's p->spacing parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpacing_CMR_NoiseMasker(Float theSpacing)
{
	static const WChar	*funcName = wxT("SetSpacing_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->spacing = theSpacing;
	return(TRUE);

}

/****************************** SetBandwidthMode ******************************/

/*
 * This function sets the module's bandwidthMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBandwidthMode_CMR_NoiseMasker(WChar * theBandwidthMode)
{
	static const WChar	*funcName = wxT("SetBandwidthMode_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theBandwidthMode,
		BandwidthModeList_CMR_NoiseMasker(0))) == CMR_NOISEMASKER_BANDWIDTHMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theBandwidthMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->bandwidthMode = specifier;
	return(TRUE);

}

/****************************** SetBandwidth ******************************/

/*
 * This function sets the module's bandwidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBandwidth_CMR_NoiseMasker(Float theBandwidth)
{
	static const WChar	*funcName = wxT("SetBandwidth_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->bandwidth = theBandwidth;
	return(TRUE);

}

/****************************** SetMskModFreq *********************************/

/*
 * This function sets the module's mskModFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMskModFreq_CMR_NoiseMasker(Float theMskModFreq)
{
	static const WChar	*funcName = wxT("SetMskModFreq_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->mskModFreq = theMskModFreq;
	return(TRUE);

}

/****************************** SetMskModPhase ********************************/

/*
 * This function sets the module's mskModPhase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMskModPhase_CMR_NoiseMasker(Float theMskModPhase)
{
	static const WChar	*funcName = wxT("SetMskModPhase_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->mskModPhase = theMskModPhase;
	return(TRUE);

}

/****************************** SetSigLevel ***********************************/

/*
 * This function sets the module's sigLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigLevel_CMR_NoiseMasker(Float theSigLevel)
{
	static const WChar	*funcName = wxT("SetSigLevel_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->sigLevel = theSigLevel;
	return(TRUE);

}

/****************************** SetSigFreq ************************************/

/*
 * This function sets the module's sigFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigFreq_CMR_NoiseMasker(Float theSigFreq)
{
	static const WChar	*funcName = wxT("SetSigFreq_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->sigFreq = theSigFreq;
	return(TRUE);

}

/****************************** SetGateTime ***********************************/

/*
 * This function sets the module's gateTime parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGateTime_CMR_NoiseMasker(Float theGateTime)
{
	static const WChar	*funcName = wxT("SetGateTime_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->gateTime = theGateTime;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_CMR_NoiseMasker(Float theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_CMR_NoiseMasker(Float theSamplingInterval)
{
	static const WChar	*funcName = wxT("SetSamplingInterval_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_CMR_NoiseMasker(void)
{
	/*static const WChar	*funcName = wxT("PrintPars_CMR_NoiseMasker");*/

	DPrint(wxT("?? CMR Module Parameters:-\n"));
	DPrint(wxT("\tbinauralMode = %s\n"), BooleanList_NSpecLists(cMRNoiseMPtr->
	  binauralMode)->name);
	DPrint(wxT("\tpacingType = %s \n"), SpacingModeList_NSpecLists(
	  cMRNoiseMPtr->spacingType)->name);
	DPrint(wxT("\tcondition = %s \n"), ConditionList_CMR_NoiseMasker(
	  cMRNoiseMPtr->condition)->name);
	DPrint(wxT("\tonFreqEar = %s \n"), EarModeList_NSpecLists(
	  cMRNoiseMPtr->onFreqEar)->name);
	DPrint(wxT("\tsigEar = %s \n"), EarModeList_NSpecLists(cMRNoiseMPtr->
	  sigEar)->name);
	DPrint(wxT("\tflankEar = %s ??\n"), cMRNoiseMPtr->flankEar);
	DPrint(wxT("\tnlow = %d ??\n"), cMRNoiseMPtr->nlow);
	DPrint(wxT("\tnupp = %d ??\n"), cMRNoiseMPtr->nupp);
	DPrint(wxT("\tnGapLow = %d ??\n"), cMRNoiseMPtr->nGapLow);
	DPrint(wxT("\tnGapUpp = %d ??\n"), cMRNoiseMPtr->nGapUpp);
	DPrint(wxT("\tranSeed = %ld ??\n"), cMRNoiseMPtr->ranSeed);
	DPrint(wxT("\tlowFBLevel = %g ??\n"), cMRNoiseMPtr->lowFBLevel);
	DPrint(wxT("\tuppFBLevel = %g ??\n"), cMRNoiseMPtr->uppFBLevel);
	DPrint(wxT("\toFMLevel = %g ??\n"), cMRNoiseMPtr->oFMLevel);
	DPrint(wxT("\tfBModPhase = %g ??\n"), cMRNoiseMPtr->fBModPhase);
	DPrint(wxT("\tspacing = %g ??\n"), cMRNoiseMPtr->spacing);
	DPrint(wxT("\tbandwidthMode = %s \n"), BandwidthModeList_CMR_NoiseMasker(
	  cMRNoiseMPtr->bandwidthMode)->name);
	DPrint(wxT("\tbandwidth = %g ??\n"), cMRNoiseMPtr->bandwidth);
	DPrint(wxT("\tmskModFreq = %g ??\n"), cMRNoiseMPtr->mskModFreq);
	DPrint(wxT("\tmskModPhase = %g ??\n"), cMRNoiseMPtr->mskModPhase);
	DPrint(wxT("\tsigLevel = %g ??\n"), cMRNoiseMPtr->sigLevel);
	DPrint(wxT("\tsigFreq = %g ??\n"), cMRNoiseMPtr->sigFreq);
	DPrint(wxT("\tsigGateMode = %s \n"), SigGateModeList_CMR_NoiseMasker(
	  cMRNoiseMPtr->sigGateMode)->name);
	DPrint(wxT("\tgateTime = %g ??\n"), cMRNoiseMPtr->gateTime);
	DPrint(wxT("\tduration = %g ??\n"), cMRNoiseMPtr->duration);
	DPrint(wxT("\tsamplingInterval = %g ??\n"), cMRNoiseMPtr->dt);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_CMR_NoiseMasker(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_CMR_NoiseMasker");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	cMRNoiseMPtr = (CMRNoiseMPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_CMR_NoiseMasker(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_CMR_NoiseMasker");

	if (!SetParsPointer_CMR_NoiseMasker(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_CMR_NoiseMasker(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = cMRNoiseMPtr;
	theModule->Free = Free_CMR_NoiseMasker;
	theModule->GetUniParListPtr = GetUniParListPtr_CMR_NoiseMasker;
	theModule->PrintPars = PrintPars_CMR_NoiseMasker;
	theModule->RunProcess = GenerateSignal_CMR_NoiseMasker;
	theModule->SetParsPointer = SetParsPointer_CMR_NoiseMasker;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

BOOLN
CheckData_CMR_NoiseMasker(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_CMR_NoiseMasker");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_CMR_NoiseMasker(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_CMR_NoiseMasker");
	SignalDataPtr	outSignal = _OutSig_EarObject(data);
	CMRNoiseMPtr	p = cMRNoiseMPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_CMR_NoiseMasker();
		if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
			return(FALSE);
		if ((p->fTInv = InitArray_FFT(outSignal->length, TRUE, 1)) == NULL) {
			NotifyError(wxT("%s: Out of memory for fT fft structure."), funcName);
			return(FALSE);
		}
		p->fTInv->plan[0] = DSAM_FFTW_NAME(plan_dft_c2r_1d)(p->fTInv->fftLen,
		  (Complx *) p->fTInv->data, p->fTInv->data, FFTW_ESTIMATE);
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		/*** Put reset (to zero ?) code here ***/
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_CMR_NoiseMasker(void)
{
	/* static const WChar	*funcName = wxT("FreeProcessVariables_CMR_NoiseMasker");*/

	FreeArray_FFT(&cMRNoiseMPtr->fTInv);
	return(TRUE);

}

/****************************** GenerateSignal ********************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if
 * required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used.
 * The "batten / 20.0" division is required to maintain the amplitude
 * equivalence with the NeuroSound code with the dB SPL conversion used.
 */

BOOLN
GenerateSignal_CMR_NoiseMasker(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("GenerateSignal_CMR_NoiseMasker");
	register ChanData	 *outPtr, *upPtr, *downPtr;
	int		i, k, chan, nLow, nUpp, kUpp[CMR_NOISEMASKER_MAXNTOTAL];
	int		nTotal, nSignal, kLow;
	ChanLen	sample, silence, gatesample, mskModPhaseSamples;
	Float	value, sigAtten, lowAtten, uppAtten, t, mskModPhase;
	Float	batten, srate, freqcomp = 0, gatefactor, minERBRate, modFactor;
	Float	flankModPhase = 0.75 * PIx2, normFactor[CMR_NOISEMASKER_MAXNTOTAL];
	Float	freqcomps[CMR_NOISEMASKER_MAXNTOTAL], sigPhase = 0.0;
	SignalDataPtr	outSignal;
	CMRNoiseMPtr	p = cMRNoiseMPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_CMR_NoiseMasker(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("CMR stimulus with noise maskers."));
		if (!InitOutSignal_EarObject(data, (p->binauralMode)? 2: 1,
		  (ChanLen) floor(p->duration / p->dt + 0.5), p->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), (uShort) _OutSig_EarObject(
		  data)->numChannels);
		if (!InitProcessVariables_CMR_NoiseMasker(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	outSignal = _OutSig_EarObject(data);
	batten = (p->condition == CMR_NOISEMASKER_CONDITION_SO)? 0.0: RMS_AMP(p->oFMLevel); //SO -> signal only
	sigAtten = RMS_AMP(p->sigLevel);
	lowAtten = RMS_AMP(p->lowFBLevel) * batten / 20.0;
	uppAtten = RMS_AMP(p->uppFBLevel) * batten / 20.0;

	/* SET PARAMETERS FOR MASKER AND SIGNAL */
	/* ------------------ other useful parameters -----------------------------
	// ------------------ masker parameters from command window --------------
	 */
	srate = 1.0 / p->dt;
	mskModPhase = DEGREES_TO_RADS(p->mskModPhase);
	if (p->condition == CMR_NOISEMASKER_CONDITION_CD)	//codeviant -> out of phase
		flankModPhase = mskModPhase + DEGREES_TO_RADS(p->fBModPhase);

	// ------------------ signal parameters -----------------------------------
	if (p->sigGateMode != CMR_NOISEMASKER_SIGGATEMODE_COS) {
		NotifyError(wxT("%s: gate mode not yet implemented."), funcName);
		return(FALSE);
	}
	gatesample = (ChanLen) (srate * p->gateTime);
	if (p->mskModFreq > 0) {
		mskModPhaseSamples = (int) ((mskModPhase / PIx2 + 0.25) / p->mskModFreq *
		  srate);
		silence = (ChanLen) (CMR_NOISEMASKER_SIG_MOD_OFFSET_CYCLES / p->mskModFreq *
		  srate);
	} else {
		mskModPhaseSamples = 0;
		silence = outSignal->length / 4;
	}

	// -------- precalculations for the masker ---------------------------------
	if (p->condition == CMR_NOISEMASKER_CONDITION_RF)	{ // no flanking bands for ref -> overwrite p->nlow,p->nupp
		nLow = 0;
		nUpp = 0;
	} else {
		nLow = p->nlow;
		nUpp = p->nupp;
	}
	// now calculate center freq of flanking components
	nTotal = p->nupp + nLow;
	nSignal = nTotal;	/* put at the end of flanker components for convenience. */
	//special case. Only one R,L,or D given -> set all flanking band s to that ear
	if (DSAM_strlen(p->flankEar) == 1)	{
			for (k=1; k < CMR_NOISEMASKER_MAXNTOTAL; k++)	{
				p->flankEar[k] = p->flankEar[0];
			}
			p->flankEar[k] = '\0';
	}
	if (DSAM_strlen(p->flankEar) < (size_t) nTotal){
		NotifyError(wxT("%s: Not all flanking band know which ear they have to go too!"), funcName);
		return (FALSE);
	}

	switch (p->spacingType) {
	case GENERAL_SPACINGMODE_LINEAR:
		for (i=0;i<=(nLow-1);i++)	{ // lower components
				freqcomp = p->sigFreq-p->spacing*(nLow - i +p->nGapLow);
				freqcomps[i] = freqcomp;
			}
		for (i=1;i<=p->nupp;i++)	{     // upper components
				freqcomp = p->sigFreq+p->spacing*(i+p->nGapUpp);
				freqcomps[i + nLow -1] = freqcomp;
			}
		break;
	case GENERAL_SPACINGMODE_OCTAVE:
		for (i = 0 ; i <= (nLow - 1); i++)	{ // lower components
			freqcomp = p->sigFreq / pow(2.0,(nLow - i + p->nGapLow) * p->spacing);
			freqcomps[i] = freqcomp;
		}
		for (i = 1; i <= p->nupp; i++)	{	  // upper components
			freqcomp=p->sigFreq * pow(2.0, (i + p->nGapUpp) * p->spacing);
			freqcomps[i + nLow -1] = freqcomp;
		}
		break;
	case GENERAL_SPACINGMODE_ERB:	/* 'spacing' is the ERB density in this mode. */
		minERBRate = ERBRateFromF_Bandwidth(p->sigFreq) - (nLow) / p->spacing;
		for (i = 0 ; i <= (nLow - 1); i++)	{ // lower components
			freqcomp = FFromERBRate_Bandwidth(minERBRate + (i - p->nGapLow) /
			  p->spacing);
			freqcomps[i] = freqcomp;
		}
		for (i = 1; i <= p->nupp; i++)	{	  // upper components
			freqcomp = FFromERBRate_Bandwidth(minERBRate + (i + nLow +
			  p->nGapUpp) / p->spacing);
			freqcomps[i + nLow -1] = freqcomp;
		}
		break;
	default:
		NotifyError(wxT("%s: The '%s' spacing mode has not yet been implemented."),
		  funcName, SpacingModeList_NSpecLists(p->spacingType)->name);
		return(FALSE);
	}
	freqcomps[nSignal] = p->sigFreq;
	for (i = 0; i < nTotal + 1; i++) {
		kUpp[i] = (int) (0.5 * ((p->bandwidthMode == CMR_NOISEMASKER_BANDWIDTHMODE_HZ)?
		  p->bandwidth: ERBFromF_Bandwidth(freqcomps[i])) / srate * outSignal->length);
		normFactor[i] = 1.0 / sqrt((Float) kUpp[i]); // after amultiplying with normFactor each noiseband has 0dB output level
	}
	/* needs some error checks (at the moment still missing!)
	 specials for the multiplied noise */
	kLow = 1; //we dont want a DC

	/*////////////////////////////////////////////////////////////////////////
	// now calculate the low-pass noise for the on-frequency band on one
	// side (it depends on the p->condition if we use the same or a different
	// one in the other ear
	////////////////////////////////////////////////////////////////////////
	 */
	CreateNoiseBand_FFT(p->fTInv, 0, data->randPars, kLow, kUpp[nSignal]);

	for (chan = 0; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		/* STIMULUS GENERATION */
		if (((chan == 0) && (p->sigEar != GENERAL_EAR_RIGHT )) ||
		  ((chan == 1) && (p->sigEar != GENERAL_EAR_LEFT))) {		// signal L(eft) or D(iotic)
			for (sample=0; sample<outSignal->length; sample++)
				outPtr[sample] = sin(sigPhase + (sample / srate) * p->sigFreq * PIx2) *
				 sigAtten * SQRT_2;
			for (sample=0; sample < silence - mskModPhaseSamples; sample++) // silence before
				outPtr[sample] = 0.0;
			for (sample=0; sample < (silence + mskModPhaseSamples); sample++)  // silence after signal
				outPtr[outSignal->length - sample - 1] = 0.0;
			upPtr = outPtr + silence - mskModPhaseSamples;
			downPtr = outPtr + outSignal->length - silence - mskModPhaseSamples - 1;
			for (sample = 0; sample < gatesample; sample++) {// apply gate windows
				switch (p->sigGateMode) {
				case CMR_NOISEMASKER_SIGGATEMODE_COS:
					gatefactor = (1.0 - cos((sample / (Float) gatesample) * PI)) / 2.0;
					break;
				default:
					NotifyError(wxT("%s: Gating mode '%d' not implmented"), funcName,
					  p->sigGateMode);
					return(FALSE);
				}
				*upPtr++ *= gatefactor;
				*downPtr-- *= gatefactor;
			}
		}
		for (sample=0; sample<outSignal->length; sample++) {
			t = sample / srate;
			modFactor = (p->mskModFreq > 0)? (1.0 + sin(mskModPhase + t *
			  p->mskModFreq * PIx2)): 1.0;
			if (((chan == 0) && (p->onFreqEar != GENERAL_EAR_RIGHT)) ||
			  ((chan == 1) && (p->onFreqEar != GENERAL_EAR_LEFT)))	{	// on frequency band  L(eft) or D(iotic)
				value = sin(sigPhase + t * p->sigFreq * PIx2) * p->fTInv->data[sample] *
				  normFactor[nSignal] * modFactor * batten;
				outPtr[sample] = outPtr[sample] + value;
			}
			modFactor = (p->mskModFreq > 0)? (1.0 + sin(flankModPhase + t *
			  p->mskModFreq * PIx2)): 1.0;
			if ((p->condition == CMR_NOISEMASKER_CONDITION_CM) || (p->condition ==
			  CMR_NOISEMASKER_CONDITION_CD))	{
				for (i=0;i<nLow;i++)	{//lower bands
					if (p->flankEar[i] != 'R')	{ //flankingband i L(eft) or D(iotic)
						value = sin(sigPhase + t * freqcomps[i] * PIx2) * p->fTInv->data[sample] *
						  normFactor[i] * modFactor * lowAtten;
						outPtr[sample] += value;
					}
				}
				for (i=nLow;i<nTotal;i++)	{//upper bands
					if (p->flankEar[i] != 'R')	{ //flankingband i L(eft) or D(iotic)
						value = sin(sigPhase + t * freqcomps[i] * PIx2) * p->fTInv->data[sample] *
						  normFactor[i] * modFactor * uppAtten;
						outPtr[sample] += value;
					}
				}
			}
		}
		if (p->condition == CMR_NOISEMASKER_CONDITION_DV)	{
			for (i=0;i<nLow;i++)	{//lower bands
				if (((chan == 0) && (p->flankEar[i] != 'R')) ||
				  ((chan == 1) && (p->flankEar[i] != 'L'))	)	{ //flankingband i  L(eft) or D(iotic)
					////////////////////////////////////////////////////////////
					// deviant means for each band a new lp-noise
					////////////////////////////////////////////////////////////
					CreateNoiseBand_FFT(p->fTInv, 0, data->randPars, kLow, kUpp[i]);
					for (sample=0; sample<outSignal->length; sample++) {
						t = sample / srate;
						modFactor = (p->mskModFreq > 0)? (1.0 + sin(flankModPhase + t *
						  p->mskModFreq * PIx2)): 1.0;
						value = sin(sigPhase + t * freqcomps[i] * PIx2) *
						  p->fTInv->data[sample] * normFactor[i] * modFactor * lowAtten;
						outPtr[sample] += value;
					}
				}
			}
			for (i=nLow;i<nTotal;i++)	{//upper bands
				if (((chan == 0) && (p->flankEar[i] != 'R')) ||
				  ((chan == 1) && (p->flankEar[i] != 'L')))	{ //flankingband i  L(eft) or D(iotic)
					/* deviant means for each band a new lp-noise */
					////////////////////////////////////////////////////////////
					CreateNoiseBand_FFT(p->fTInv, 0, data->randPars, kLow, kUpp[i]);
					for (sample=0; sample<outSignal->length; sample++) {
						t = sample / srate;
						modFactor = (p->mskModFreq > 0)? (1.0 + sin(flankModPhase + t *
						  p->mskModFreq * PIx2)): 1.0;
						value = sin(sigPhase + t * freqcomps[i] * PIx2) *
						  p->fTInv->data[sample] * normFactor[i] * modFactor * uppAtten;
						outPtr[sample] += value;
					}
				}
			}
		}
	}


	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
