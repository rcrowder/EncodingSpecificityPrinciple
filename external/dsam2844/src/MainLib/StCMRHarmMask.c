/**********************
 *
 * File:		StCMRHarmMask.c
 * Purpose:		Comodulation Masking Release stimulus with harmonic noise
 * 				flankers.
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "UtString.h"
#include "FiParFile.h"
#include "StCMRHarmMask.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CMRHarmMPtr	cMRHarmMPtr = NULL;

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
Free_CMR_HarmonicMasker(void)
{
	/* static const WChar	*funcName = wxT("Free_CMR_HarmonicMasker"); */

	if (cMRHarmMPtr == NULL)
		return(FALSE);
	if (cMRHarmMPtr->parList)
		FreeList_UniParMgr(&cMRHarmMPtr->parList);
	if (cMRHarmMPtr->parSpec == GLOBAL) {
		free(cMRHarmMPtr);
		cMRHarmMPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitConditionList *****************************/

/*
 * This function initialises the 'condition' list array
 */

BOOLN
InitConditionList_CMR_HarmonicMasker(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("CD"),	CMR_HARMONICMASKER_CONDITION_CD },
			{ wxT("CM"),	CMR_HARMONICMASKER_CONDITION_CM },
			{ wxT("RF"),	CMR_HARMONICMASKER_CONDITION_RF },
			{ wxT("SO"),	CMR_HARMONICMASKER_CONDITION_SO },
			{ wxT("UM"),	CMR_HARMONICMASKER_CONDITION_UM },
			{ 0,			CMR_HARMONICMASKER_CONDITION_NULL },
		};
	cMRHarmMPtr->conditionList = modeList;
	return(TRUE);

}

/****************************** InitSigModeList *******************************/

/*
 * This function initialises the 'sigMode' list array
 */

BOOLN
InitSigModeList_CMR_HarmonicMasker(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("CONT"),	CMR_HARMONICMASKER_SIGMODE_CONT },
			{ wxT("PIPS"),	CMR_HARMONICMASKER_SIGMODE_PIPS },
			{ 0,			CMR_HARMONICMASKER_SIGMODE_NULL },
		};
	cMRHarmMPtr->sigModeList = modeList;
	return(TRUE);

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
Init_CMR_HarmonicMasker(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_CMR_HarmonicMasker");

	if (parSpec == GLOBAL) {
		if (cMRHarmMPtr != NULL)
			Free_CMR_HarmonicMasker();
		if ((cMRHarmMPtr = (CMRHarmMPtr) malloc(sizeof(CMRHarmM))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (cMRHarmMPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	cMRHarmMPtr->parSpec = parSpec;
	cMRHarmMPtr->binauralMode = GENERAL_BOOLEAN_OFF;
	cMRHarmMPtr->spacingType = GENERAL_SPACINGMODE_LINEAR;
	cMRHarmMPtr->condition = CMR_HARMONICMASKER_CONDITION_RF;
	cMRHarmMPtr->onFreqEar = GENERAL_EAR_LEFT;
	cMRHarmMPtr->sigEar = GENERAL_EAR_LEFT;
	cMRHarmMPtr->sigMode = CMR_HARMONICMASKER_SIGMODE_PIPS;
	DSAM_strcpy(cMRHarmMPtr->flankEar, wxT("L"));
	cMRHarmMPtr->nlow = 3;
	cMRHarmMPtr->nupp = 3;
	cMRHarmMPtr->nGapLow = 1;
	cMRHarmMPtr->nGapUpp = 1;
	cMRHarmMPtr->lowFBLevel = 0.0;
	cMRHarmMPtr->uppFBLevel = 0.0;
	cMRHarmMPtr->oFMLevel = 0.0;
	cMRHarmMPtr->spacing = 100.0;
	cMRHarmMPtr->mskmodfreq = 10.0;
	cMRHarmMPtr->sigLevel = 0.0;
	cMRHarmMPtr->sigFreq = 700.0;
	cMRHarmMPtr->gateTime = 20.0e-3;
	cMRHarmMPtr->duration = 0.5;
	cMRHarmMPtr->dt = DEFAULT_DT;

	InitConditionList_CMR_HarmonicMasker();
	InitSigModeList_CMR_HarmonicMasker();
	if (!SetUniParList_CMR_HarmonicMasker()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_CMR_HarmonicMasker();
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_CMR_HarmonicMasker(void)
{
	static const WChar	*funcName = wxT("SetUniParList_CMR_HarmonicMasker");
	UniParPtr	pars;

	if ((cMRHarmMPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  CMR_HARMONICMASKER_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = cMRHarmMPtr->parList->pars;
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_BINAURALMODE], wxT("BINAURAL_MODE"),
	  wxT("Binaural mode ('on' or 'off ')."),
	  UNIPAR_BOOL,
	  &cMRHarmMPtr->binauralMode, NULL,
	  (void * (*)) SetBinauralMode_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_SPACINGTYPE], wxT("SPACING_TYPE"),
	  wxT("Spacing type ('linear' or 'log')."),
	  UNIPAR_NAME_SPEC,
	  &cMRHarmMPtr->spacingType, SpacingModeList_NSpecLists(0),
	  (void * (*)) SetSpacingType_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_CONDITION], wxT("CONDITION"),
	  wxT("Stimulus condition mode: ('cd' - co-deviant, 'cm' - comodulation, 'rf' - ")
	    wxT("reference, 'so' - signal only or 'um' - unmodulated."),
	  UNIPAR_NAME_SPEC,
	  &cMRHarmMPtr->condition, cMRHarmMPtr->conditionList,
	  (void * (*)) SetCondition_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_ONFREQEAR], wxT("ON_FREQ_EAR"),
	  wxT("On-frequency ear ('left' or 'right')."),
	  UNIPAR_NAME_SPEC,
	  &cMRHarmMPtr->onFreqEar, EarModeList_NSpecLists(0),
	  (void * (*)) SetOnFreqEar_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_SIGEAR], wxT("SIGNAL_EAR"),
	  wxT("Signal presentation ear ('left' or 'right')."),
	  UNIPAR_NAME_SPEC,
	  &cMRHarmMPtr->sigEar, EarModeList_NSpecLists(0),
	  (void * (*)) SetSigEar_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_SIGMODE], wxT("SIGNAL_MODE"),
	  wxT("Signal"),
	  UNIPAR_NAME_SPEC,
	  &cMRHarmMPtr->sigMode, cMRHarmMPtr->sigModeList,
	  (void * (*)) SetSigMode_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_FLANKEAR], wxT("FLANK_EAR"),
	  wxT("Flanker ear string ??"),
	  UNIPAR_STRING,
	  &cMRHarmMPtr->flankEar, NULL,
	  (void * (*)) SetFlankEar_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_NLOW], wxT("NUM_LOW_BANDS"),
	  wxT("No. lower frequency bands"),
	  UNIPAR_INT,
	  &cMRHarmMPtr->nlow, NULL,
	  (void * (*)) SetNlow_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_NUPP], wxT("NUM_UPPER_BANDS"),
	  wxT("No. upper frequency bands."),
	  UNIPAR_INT,
	  &cMRHarmMPtr->nupp, NULL,
	  (void * (*)) SetNupp_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_NGAPLOW], wxT("NUM_LOWER_GAP"),
	  wxT("No. of lower Gaps"),
	  UNIPAR_INT,
	  &cMRHarmMPtr->nGapLow, NULL,
	  (void * (*)) SetNGapLow_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_NGAPUPP], wxT("NUM_UPPER_GAP"),
	  wxT("No. of upper Gaps."),
	  UNIPAR_INT,
	  &cMRHarmMPtr->nGapUpp, NULL,
	  (void * (*)) SetNGapUpp_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_LOWFBLEVEL], wxT("LOWER_FB_LEVEL"),
	  wxT("Lower FB level (dB re OFM)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->lowFBLevel, NULL,
	  (void * (*)) SetLowFBLevel_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_UPPFBLEVEL], wxT("UPPER_FB_LEVEL"),
	  wxT("Upper FB level (dB re OFM)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->uppFBLevel, NULL,
	  (void * (*)) SetUppFBLevel_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_OFMLEVEL], wxT("OFM_LEVEL"),
	  wxT("On-frequency masker level (dB SPL)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->oFMLevel, NULL,
	  (void * (*)) SetOFMLevel_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_SPACING], wxT("SPACING_VAR"),
	  wxT("Spacing (Hz or octaves)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->spacing, NULL,
	  (void * (*)) SetSpacing_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_MSKMODFREQ], wxT("MASKER_MOD_FREQ"),
	  wxT("Flanking bands modulation frequency (Hz)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->mskmodfreq, NULL,
	  (void * (*)) SetMskmodfreq_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_SIGLEVEL], wxT("SIGNAL_LEVEL"),
	  wxT("Signal level (dB SPL)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->sigLevel, NULL,
	  (void * (*)) SetSigLevel_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_SIGFREQ], wxT("SIGNAL_FREQ"),
	  wxT("Signal frequency (Hz)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->sigFreq, NULL,
	  (void * (*)) SetSigFreq_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_GATETIME], wxT("SIGNAL_GATE_TIME"),
	  wxT("Signal gate time (s)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->gateTime, NULL,
	  (void * (*)) SetGateTime_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->duration, NULL,
	  (void * (*)) SetDuration_CMR_HarmonicMasker);
	SetPar_UniParMgr(&pars[CMR_HARMONICMASKER_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &cMRHarmMPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_CMR_HarmonicMasker);

	SetEnabledPars_CMR_HarmonicMasker();
	return(TRUE);

}

/********************************* SetDefaulEnabledPars ***********************/

/*
 * This routine sets the parameter list so that the correct parameters
 * are enabled/disabled.
 */

BOOLN
SetEnabledPars_CMR_HarmonicMasker(void)
{
	static const WChar *funcName = wxT("SetEnabledPars_CMR_HarmonicMasker");
	CMRHarmMPtr	p = cMRHarmMPtr;

	if (p == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	p->parList->pars[CMR_HARMONICMASKER_FLANKEAR].enabled = TRUE;
	p->parList->pars[CMR_HARMONICMASKER_NUPP].enabled = TRUE;
	p->parList->pars[CMR_HARMONICMASKER_NLOW].enabled = TRUE;
	p->parList->pars[CMR_HARMONICMASKER_NGAPLOW].enabled = TRUE;
	p->parList->pars[CMR_HARMONICMASKER_NGAPUPP].enabled = TRUE;
	p->parList->pars[CMR_HARMONICMASKER_LOWFBLEVEL].enabled = TRUE;
	p->parList->pars[CMR_HARMONICMASKER_UPPFBLEVEL].enabled = TRUE;
	p->parList->pars[CMR_HARMONICMASKER_SPACING].enabled = TRUE;
	switch (p->condition) {
	case CMR_HARMONICMASKER_CONDITION_RF:
	case CMR_HARMONICMASKER_CONDITION_SO:
		p->parList->pars[CMR_HARMONICMASKER_FLANKEAR].enabled = FALSE;
		p->parList->pars[CMR_HARMONICMASKER_NUPP].enabled = FALSE;
		p->parList->pars[CMR_HARMONICMASKER_NLOW].enabled = FALSE;
		p->parList->pars[CMR_HARMONICMASKER_NGAPLOW].enabled = FALSE;
		p->parList->pars[CMR_HARMONICMASKER_NGAPUPP].enabled = FALSE;
		p->parList->pars[CMR_HARMONICMASKER_LOWFBLEVEL].enabled = FALSE;
		p->parList->pars[CMR_HARMONICMASKER_UPPFBLEVEL].enabled = FALSE;
		p->parList->pars[CMR_HARMONICMASKER_SPACING].enabled = FALSE;
		break;
	case CMR_HARMONICMASKER_CONDITION_UM:
		p->parList->pars[CMR_HARMONICMASKER_MSKMODFREQ].enabled = FALSE;
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
GetUniParListPtr_CMR_HarmonicMasker(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (cMRHarmMPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(cMRHarmMPtr->parList);

}

/****************************** SetBinauralMode *******************************/

/*
 * This function sets the module's binauralMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinauralMode_CMR_HarmonicMasker(WChar * theBinauralMode)
{
	static const WChar	*funcName = wxT("SetBinauralMode_CMR_HarmonicMasker");
	int		specifier;

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theBinauralMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName, theBinauralMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->binauralMode = specifier;
	return(TRUE);

}

/****************************** SetSpacingType ********************************/

/*
 * This function sets the module's spacingType parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpacingType_CMR_HarmonicMasker(WChar * theSpacingType)
{
	static const WChar	*funcName = wxT("SetSpacingType_CMR_HarmonicMasker");
	int		specifier;

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSpacingType, SpacingModeList_NSpecLists(
	  0))) == CFLIST_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSpacingType);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->spacingType = specifier;
	return(TRUE);

}

/****************************** SetCondition **********************************/

/*
 * This function sets the module's condition parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCondition_CMR_HarmonicMasker(WChar * theCondition)
{
	static const WChar	*funcName = wxT("SetCondition_CMR_HarmonicMasker");
	int		specifier;

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theCondition,
		cMRHarmMPtr->conditionList)) == CMR_HARMONICMASKER_CONDITION_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theCondition);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->condition = specifier;
	SetEnabledPars_CMR_HarmonicMasker();
	return(TRUE);

}

/****************************** SetOnFreqEar **********************************/

/*
 * This function sets the module's onFreqEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOnFreqEar_CMR_HarmonicMasker(WChar * theOnFreqEar)
{
	static const WChar	*funcName = wxT("SetOnFreqEar_CMR_HarmonicMasker");
	int		specifier;

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOnFreqEar,
	  EarModeList_NSpecLists(0))) == GENERAL_EAR_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOnFreqEar);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->onFreqEar = specifier;
	return(TRUE);

}

/****************************** SetSigEar *************************************/

/*
 * This function sets the module's sigEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigEar_CMR_HarmonicMasker(WChar * theSigEar)
{
	static const WChar	*funcName = wxT("SetSigEar_CMR_HarmonicMasker");
	int		specifier;

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSigEar,
	  EarModeList_NSpecLists(0))) == GENERAL_EAR_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSigEar);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->sigEar = specifier;
	return(TRUE);

}

/****************************** SetSigMode ************************************/

/*
 * This function sets the module's sigMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigMode_CMR_HarmonicMasker(WChar * theSigMode)
{
	static const WChar	*funcName = wxT("SetSigMode_CMR_HarmonicMasker");
	int		specifier;

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSigMode,
		cMRHarmMPtr->sigModeList)) == CMR_HARMONICMASKER_SIGMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSigMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->sigMode = specifier;
	return(TRUE);

}

/****************************** SetFlankEar ***********************************/

/*
 * This function sets the module's flankEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFlankEar_CMR_HarmonicMasker(WChar *theFlankEar)
{
	static const WChar	*funcName = wxT("SetFlankEar_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	DSAM_strcpy(cMRHarmMPtr->flankEar, theFlankEar);
	return(TRUE);

}

/****************************** SetNlow ***************************************/

/*
 * This function sets the module's nlow parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNlow_CMR_HarmonicMasker(int theNlow)
{
	static const WChar	*funcName = wxT("SetNlow_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->nlow = theNlow;
	return(TRUE);

}

/****************************** SetNupp ***************************************/

/*
 * This function sets the module's nupp parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNupp_CMR_HarmonicMasker(int theNupp)
{
	static const WChar	*funcName = wxT("SetNupp_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->nupp = theNupp;
	return(TRUE);

}

/****************************** SetNGapLow ************************************/

/*
 * This function sets the module's p->nGapLow parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNGapLow_CMR_HarmonicMasker(int theNGapLow)
{
	static const WChar	*funcName = wxT("SetNGapLow_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->nGapLow = theNGapLow;
	return(TRUE);

}

/****************************** SetNGapUpp ************************************/

/*
 * This function sets the module's NGapUpp parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNGapUpp_CMR_HarmonicMasker(int theNGapUpp)
{
	static const WChar	*funcName = wxT("SetNGapUpp_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->nGapUpp = theNGapUpp;
	return(TRUE);

}

/****************************** SetLowFBLevel *********************************/

/*
 * This function sets the module's lowFBLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowFBLevel_CMR_HarmonicMasker(Float theLowFBLevel)
{
	static const WChar	*funcName = wxT("SetLowFBLevel_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->lowFBLevel = theLowFBLevel;
	return(TRUE);

}

/****************************** SetUppFBLevel *********************************/

/*
 * This function sets the module's uppFBLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUppFBLevel_CMR_HarmonicMasker(Float theUppFBLevel)
{
	static const WChar	*funcName = wxT("SetUppFBLevel_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->uppFBLevel = theUppFBLevel;
	return(TRUE);

}

/****************************** SetOFMLevel ***********************************/

/*
 * This function sets the module's oFMLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOFMLevel_CMR_HarmonicMasker(Float theOFMLevel)
{
	static const WChar	*funcName = wxT("SetOFMLevel_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->oFMLevel = theOFMLevel;
	return(TRUE);

}

/****************************** SetSpacing ************************************/

/*
 * This function sets the module's spacing parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpacing_CMR_HarmonicMasker(Float theSpacing)
{
	static const WChar	*funcName = wxT("SetSpacing_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->spacing = theSpacing;
	return(TRUE);

}

/****************************** SetMskmodfreq *********************************/

/*
 * This function sets the module's mskmodfreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMskmodfreq_CMR_HarmonicMasker(Float theMskmodfreq)
{
	static const WChar	*funcName = wxT("SetMskmodfreq_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->mskmodfreq = theMskmodfreq;
	return(TRUE);

}

/****************************** SetSigLevel ***********************************/

/*
 * This function sets the module's sigLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigLevel_CMR_HarmonicMasker(Float theSigLevel)
{
	static const WChar	*funcName = wxT("SetSigLevel_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->sigLevel = theSigLevel;
	return(TRUE);

}

/****************************** SetSigFreq ************************************/

/*
 * This function sets the module's sigFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigFreq_CMR_HarmonicMasker(Float theSigFreq)
{
	static const WChar	*funcName = wxT("SetSigFreq_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->sigFreq = theSigFreq;
	return(TRUE);

}

/****************************** SetGateTime ***********************************/

/*
 * This function sets the module's gateTime parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGateTime_CMR_HarmonicMasker(Float theGateTime)
{
	static const WChar	*funcName = wxT("SetGateTime_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->gateTime = theGateTime;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_CMR_HarmonicMasker(Float theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_CMR_HarmonicMasker(Float theSamplingInterval)
{
	static const WChar	*funcName = wxT(
	  "SetSamplingInterval_CMR_HarmonicMasker");

	if (cMRHarmMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRHarmMPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_CMR_HarmonicMasker(void)
{

	DPrint(wxT("?? CMR Module Parameters:-\n"));
	DPrint(wxT("\tbinauralMode = %s\n"), BooleanList_NSpecLists(cMRHarmMPtr->
	  binauralMode)->name);
	DPrint(wxT("\tspacingType = %s \n"), SpacingModeList_NSpecLists(
	  cMRHarmMPtr->spacingType)->name);
	DPrint(wxT("\tcondition = %s \n"),
	  cMRHarmMPtr->conditionList[cMRHarmMPtr->condition].name);
	DPrint(wxT("\tonFreqEar = %s \n"),
			EarModeList_NSpecLists(cMRHarmMPtr->onFreqEar)->name);
	DPrint(wxT("\tsigEar = %s \n"), EarModeList_NSpecLists(cMRHarmMPtr->
	  sigEar)->name);
	DPrint(wxT("\tsigMode = %s \n"), cMRHarmMPtr->sigModeList[cMRHarmMPtr->
	  sigMode].name);
	DPrint(wxT("\tflankEar = %s ??\n"), cMRHarmMPtr->flankEar);
	DPrint(wxT("\tnlow = %d ??\n"), cMRHarmMPtr->nlow);
	DPrint(wxT("\tnupp = %d ??\n"), cMRHarmMPtr->nupp);
	DPrint(wxT("\tp->nGapLow = %d ??\n"), cMRHarmMPtr->nGapLow);
	DPrint(wxT("\tNGapUpp = %d ??\n"), cMRHarmMPtr->nGapUpp);
	DPrint(wxT("\tlowFBLevel = %g ??\n"), cMRHarmMPtr->lowFBLevel);
	DPrint(wxT("\tuppFBLevel = %g ??\n"), cMRHarmMPtr->uppFBLevel);
	DPrint(wxT("\toFMLevel = %g ??\n"), cMRHarmMPtr->oFMLevel);
	DPrint(wxT("\tspacing = %g ??\n"), cMRHarmMPtr->spacing);
	DPrint(wxT("\tmskmodfreq = %g ??\n"), cMRHarmMPtr->mskmodfreq);
	DPrint(wxT("\tsigLevel = %g ??\n"), cMRHarmMPtr->sigLevel);
	DPrint(wxT("\tsigFreq = %g ??\n"), cMRHarmMPtr->sigFreq);
	DPrint(wxT("\tgateTime = %g ??\n"), cMRHarmMPtr->gateTime);
	DPrint(wxT("\tduration = %g ??\n"), cMRHarmMPtr->duration);
	DPrint(wxT("\tsamplingInterval = %g ??\n"), cMRHarmMPtr->dt);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_CMR_HarmonicMasker(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_CMR_HarmonicMasker");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	cMRHarmMPtr = (CMRHarmMPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_CMR_HarmonicMasker(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_CMR_HarmonicMasker");

	if (!SetParsPointer_CMR_HarmonicMasker(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_CMR_HarmonicMasker(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = cMRHarmMPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_CMR_HarmonicMasker;
	theModule->GetUniParListPtr = GetUniParListPtr_CMR_HarmonicMasker;
	theModule->PrintPars = PrintPars_CMR_HarmonicMasker;
	theModule->RunProcess = GenerateSignal_CMR_HarmonicMasker;
	theModule->SetParsPointer = SetParsPointer_CMR_HarmonicMasker;
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
CheckData_CMR_HarmonicMasker(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_CMR_HarmonicMasker");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
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
GenerateSignal_CMR_HarmonicMasker(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("GenerateSignal_CMR_HarmonicMasker");
	register ChanData	*outPtr;
	int		i, k, n, chan, gatesample, nLow, nUpp, nTotal;
	int		nmax, samplebeg;
	ChanLen	sample, silence;
	Float	batten, srate, onfreqmodphase, flankmodphase, sigphase, t;
	Float	flankmoddepth, sigAtten, lowAtten, uppAtten, gatefactor;
	Float	freqcomp, freqcomps[CMR_HARMONICMASKER_MAXNTOTAL], value;
	SignalDataPtr	outSignal;
	CMRHarmMPtr	p = cMRHarmMPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_CMR_HarmonicMasker(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("CMR stimulus with harmonic maskers."));
		if (!InitOutSignal_EarObject(data, (p->binauralMode)? 2: 1,
		  (ChanLen) floor(p->duration / p->dt + 0.5), p->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), (uShort) _OutSig_EarObject(
		  data)->numChannels);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	srate = 1.0 / p->dt;
	batten = (p->condition == CMR_HARMONICMASKER_CONDITION_SO)? 0.0:
	  RMS_AMP(p->oFMLevel);
	sigAtten = RMS_AMP(p->sigLevel);
	lowAtten = RMS_AMP(p->lowFBLevel) * batten / 20.0;
	uppAtten = RMS_AMP(p->uppFBLevel) * batten / 20.0;

	onfreqmodphase = CMR_HARMONICMASKER_PHASE_FACTOR * PIx2;
	flankmodphase = (p->condition == CMR_HARMONICMASKER_CONDITION_CD)? 0.25 * PIx2:
	  CMR_HARMONICMASKER_PHASE_FACTOR * PIx2;
	sigphase = 0.0;
	gatesample = (int) floor(srate * p->gateTime + 0.5); //only important for continuous
	silence = (int) (outSignal->length / 4) ;

	// -------- precalculations for the masker ---------------------------------
	if (p->condition == CMR_HARMONICMASKER_CONDITION_RF)	{ // no flanking bands for ref -> overwrite p->nlow,p->nupp
		nLow = 0;
		nUpp = 0;
	} else {
		nLow = p->nlow;
		nUpp = p->nupp;
	}
	flankmoddepth = (p->condition == CMR_HARMONICMASKER_CONDITION_UM)? 0.0: 1.0;
	// now calculate center freq of flanking components
	nTotal = nUpp + nLow;
	//special case. Only one R,L,or D given -> set all flanking band s to that ear
	if (DSAM_strlen(p->flankEar) == 1)	{
			for (k=1; k < CMR_HARMONICMASKER_MAXNTOTAL; k++)	{
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
		for (i = 0; i <= (nLow - 1);i++)	{
				freqcomp = p->sigFreq-p->spacing * (Float)(nLow - i +
				  p->nGapLow);
				freqcomps[i] = freqcomp;
			}
			for (i = 1; i <= nUpp; i++)	{
				freqcomp = p->sigFreq + p->spacing * (Float)(i + p->nGapUpp);
				freqcomps[i + nLow -1] = freqcomp;
			}
		break;
	case GENERAL_SPACINGMODE_OCTAVE:
		for (i=0;i<=(nLow-1);i++)	{
			freqcomp = p->sigFreq/(Float)pow(2.0,((nLow - i + p->nGapLow) *
			  (Float)p->spacing));
			freqcomps[i] = freqcomp;
		}
		for (i=1;i<=nUpp;i++)	{
			freqcomp=p->sigFreq*(Float)pow(2.0,(i+p->nGapUpp)*
			  (Float)p->spacing);
			freqcomps[i + nLow -1] = freqcomp;
		}
		break;
	default:
		NotifyError(wxT("%s: The '%s' spacing mode has not yet been implemented."),
		  funcName, SpacingModeList_NSpecLists(p->spacingType)->name);
		return(FALSE);
	}

	for (chan = 0; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		////////////////////////////////////////////////////////////////////////////
		// STIMULUS GENERATION
		////////////////////////////////////////////////////////////////////////////
		// --------------- generate stimulus LEFT ear -----------------------------
		for (sample=0; sample<outSignal->length; sample++)  //first set all to zero
			outPtr[sample] = 0;
		if (((chan == 0) && (p->sigEar != GENERAL_EAR_RIGHT )) ||
		  ((chan == 1) && (p->sigEar != GENERAL_EAR_LEFT))) {		// signal L(eft) or D(iotic)
			for (sample=0; sample<outSignal->length; sample++) {
				outPtr[sample] =  sin(sigphase + (sample/srate) * p->sigFreq *
				  PIx2)*sigAtten * SQRT_2;
			}
			if (p->sigMode == CMR_HARMONICMASKER_SIGMODE_CONT) {
				for (sample=0; sample<silence; sample++) {
					outPtr[sample] = 0;
					outPtr[outSignal->length-1-sample]= 0;
				}
				for (sample=silence; sample<(silence+gatesample); sample++) {
					gatefactor = (1.0 - cos(((sample-silence) /
					  (Float) gatesample) * PI))/2.0;
					outPtr[sample] *= gatefactor;
					outPtr[outSignal->length-sample-1] *= gatefactor;
				}
			} else {
				ChanLen mskmodsample = (int) (srate / p->mskmodfreq);
				// set all mskperiod to zero for the first 1/4 of stimulus (=first dips without signal)
				silence = (int) (((int) (outSignal->length * 0.25 / mskmodsample) + 0.5) * mskmodsample);
				for (sample=0; sample<silence; sample++) {
					outPtr[sample] = 0;
				}
				samplebeg = 0;
				nmax = (int)((outSignal->length * p->mskmodfreq) / srate);
				for (n=1; n<nmax ;n++)	{ // now we apply the gating windows
					// it will not have an effect for the first 1/4 outSignal->length since values are already 0
					samplebeg = (int) ((n - 0.5) * (Float) mskmodsample);
					for (sample=0; sample< (mskmodsample * 0.25); sample++) {
						outPtr[sample+samplebeg] = 0;
						gatefactor = (1.0 - cos(((sample) / (Float) (mskmodsample*0.25)) * PI)); ///2.0;
						outPtr[sample+samplebeg+(int)(mskmodsample * 0.25)] *= gatefactor;  // thats the falling part
						outPtr[(mskmodsample+samplebeg-1-sample-(int)(mskmodsample * 0.25))] *= gatefactor; //thats the falling part
						outPtr[(mskmodsample+samplebeg-1-sample)]= 0; //fill the rest with zeros
					}
				}
				n = nmax; //fill the rest with 0...
				samplebeg = (int) ((n - 0.5) * (Float) mskmodsample);
				if ((outSignal->length - samplebeg) > (mskmodsample-(int)(mskmodsample * 0.25)) ) { //...unless it is > 3/4 a period, then there is enough room for another tone pip
					for (sample=0; sample< (mskmodsample * 0.25); sample++) {
						outPtr[sample+samplebeg] = 0;
						gatefactor = (1.0 - cos(((sample) / (Float) (mskmodsample*0.25)) * PI));///2.0;
						outPtr[(sample+samplebeg+(int)(mskmodsample * 0.25))] *= gatefactor;
						outPtr[(mskmodsample+samplebeg-1-sample-(int)(mskmodsample * 0.25))] *= gatefactor;
					}
					samplebeg = (mskmodsample+samplebeg-(int)(mskmodsample * 0.25));
				}
				for (sample=samplebeg; sample<outSignal->length; sample++) {
					outPtr[sample] = 0;
				}
			}
		}
		for (sample=0; sample<outSignal->length; sample++) {
			t = sample / srate;
			if (((chan == 0) && (p->onFreqEar != GENERAL_EAR_RIGHT)) ||
			  ((chan == 1) && (p->onFreqEar != GENERAL_EAR_LEFT)))	{	// on frequency band  L(eft) or D(iotic)
				value = sin(sigphase + t * p->sigFreq * PIx2) * (1 +
				  sin(onfreqmodphase + t * p->mskmodfreq * PIx2)) * batten *
				  SQRT_2;
				outPtr[sample] = outPtr[sample] + value;
			}
			for (i=0;i<=(nLow-1);i++)	{//lower bands
				if (((chan == 0) && (p->flankEar[i] != 'R')) ||
				  ((chan == 1) && (p->flankEar[i] != 'L')))	{ //flankingband i  R(ight) or D(iotic)
					value = sin(sigphase + t * freqcomps[i] * PIx2) * (1 +
					  flankmoddepth * sin(flankmodphase + t * p->mskmodfreq *
					  PIx2)) * lowAtten * SQRT_2;
					outPtr[sample] = outPtr[sample] + value;
				}
			}
			for (i=nLow;i<nTotal;i++)	{//upper bands
				if (((chan == 0) && (p->flankEar[i] != 'R')) ||
				  ((chan == 1) && (p->flankEar[i] != 'L')))	{ //flankingband i  R(ight) or D(iotic)
					value = sin(sigphase + t * freqcomps[i] * PIx2)* (1.0 +
					  flankmoddepth * sin(flankmodphase + t * p->mskmodfreq *
					  PIx2)) *  uppAtten * SQRT_2;
					outPtr[sample] = outPtr[sample] + value;
				}
			}
		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
