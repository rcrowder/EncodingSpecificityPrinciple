/**********************
 *
 * File:		UtStrobe.c
 * Purpose:		This module implements strobe criteria for the AIM stabilised
 *				auditory image (SAI)
 *				See Patterson R. D. and Allerhand M. H. (1995) "Time-domain
 *				modeling of peripheral auditory processing: A modular
 *				Architecture and a software platform", J. Acoust. Soc. Am. 98,
 *				1890-1894.
 * Comments:	Written using ModuleProducer version 1.12 (Oct  9 1997).
 *				The "Threshold" strobe mode algorithm is the same as the
 *				"user" mode at present.  This is because the "User" mode is
 *				interpreted differently by the calling SAI module program.
 *				This method saves having to have a separate set of mode names
 *				in the SAI module.
 *				20-11-97 LPO: Peak strobe criteria now only strobes when the
 *				peak is above the threshold.
 *				21-11-97 LPO: Changed "lagTime" to "delay".
 *				25-11-97 LPO: Implemented the peak shadow "-" mode.
 *				10-12-97 LPO: After a long battle I have finished implementing
 *				the peak shadow "+" mode.  Note that the definition for this
 *				criterion is different from that described in the AIM code
 *				(though even in the AIM code is was not implemented.)
 *				11-12-97 LPO: Added the delayTimeout constraint.  This causes
 *				the most recent strobe peak to be set, regardless of the
 *				delayCount state.
 *				11-09-98 LPO: Memory allocation under Borland C++ 4.5 returns
 *				a null pointer if zero units are requested.  A minimum of 1 must
 *				always be allocated.
 *				06-07-99 LPO: This module now sets the 'staticTimeFlag', as
 *				it sets the 'outputTimeOffset' field,
 * Author:		L. P. O'Mard
 * Created:		10 Oct 1997
 * Updated:		11 Sep 1997
 * Copyright:	(c) 1998, 1999, 2010 Lowel P. O'Mard
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
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "GeCommon.h"
#include "GeNSpecLists.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtStrobe.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

StrobePtr	strobePtr = NULL;

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
Free_Utility_Strobe(void)
{
	/* static const WChar	*funcName = wxT("Free_Utility_Strobe");  */

	if (strobePtr == NULL)
		return(FALSE);
	FreeProcessVariables_Utility_Strobe();
	if (strobePtr->diagnosticModeList)
		free(strobePtr->diagnosticModeList);
	if (strobePtr->parList)
		FreeList_UniParMgr(&strobePtr->parList);
	if (strobePtr->parSpec == GLOBAL) {
		free(strobePtr);
		strobePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitTypeModeList ******************************/

/*
 * This routine intialises the TypeMode list array.
 */

BOOLN
InitTypeModeList_Utility_Strobe(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("USER"), 			STROBE_USER_MODE },
					{ wxT("THRESHOLD"), 	STROBE_THRESHOLD_MODE },
					{ wxT("PEAK"), 			STROBE_PEAK_MODE },
					{ wxT("PEAK_SHADOW-"),	STROBE_PEAK_SHADOW_NEGATIVE_MODE },
					{ wxT("PEAK_SHADOW+"),	STROBE_PEAK_SHADOW_POSITIVE_MODE },
					{ wxT("DELTA_GAMMA"),	STROBE_DELTA_GAMMA_MODE },
					{ NULL,					STROBE_MODE_NULL }
				};
	strobePtr->typeModeList = modeList;
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
Init_Utility_Strobe(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_Strobe");

	if (parSpec == GLOBAL) {
		if (strobePtr != NULL)
			Free_Utility_Strobe();
		if ((strobePtr = (StrobePtr) malloc(sizeof(Strobe))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (strobePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	strobePtr->parSpec = parSpec;
	strobePtr->updateProcessVariablesFlag = TRUE;
	strobePtr->typeMode = STROBE_PEAK_SHADOW_POSITIVE_MODE;
	strobePtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	strobePtr->threshold = 0.0;
	strobePtr->thresholdDecayRate = 5000.0;
	strobePtr->delay = 5e-3;
	strobePtr->delayTimeout = 10e-3;

	InitTypeModeList_Utility_Strobe();
	if ((strobePtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), strobePtr->diagnosticString)) == NULL)
		return(FALSE);
	if (!SetUniParList_Utility_Strobe()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_Strobe();
		return(FALSE);
	}
	strobePtr->numChannels = 0;
	strobePtr->thresholdDecay = 0.0;
	strobePtr->numLastSamples = 0;
	strobePtr->delayTimeoutSamples = 0;
	strobePtr->fp = NULL;
	strobePtr->stateVars = NULL;
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_Utility_Strobe(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_Strobe");
	UniParPtr	pars;

	if ((strobePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  STROBE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = strobePtr->parList->pars;
	SetPar_UniParMgr(&pars[STROBE_TYPE_MODE], wxT("CRITERION"),
	  wxT("Strobe criterion (mode): 'user ', 'threshold' (0), 'peak' (1), ")
	    wxT("'peak_shadow-' (3), or 'peak_shadow+' (4/5)."),
	  UNIPAR_NAME_SPEC,
	  &strobePtr->typeMode, strobePtr->typeModeList,
	  (void * (*)) SetTypeMode_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_DIAGNOSTIC_MODE], wxT("STROBE_DIAGNOSTICS"),
	  wxT("Diagnostic mode ('off', 'screen', 'error' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &strobePtr->diagnosticMode, strobePtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_THRESHOLD], wxT("THRESHOLD"),
	  wxT("Threshold for strobing: 'user', 'threshold' and 'peak' modes only."),
	  UNIPAR_REAL,
	  &strobePtr->threshold, NULL,
	  (void * (*)) SetThreshold_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_THRESHOLD_DECAY_RATE], wxT("THRESHOLD_DECAY"),
	  wxT("Threshold decay rate (%/s)."),
	  UNIPAR_REAL,
	  &strobePtr->thresholdDecayRate, NULL,
	  (void * (*)) SetThresholdDecayRate_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_DELAY], wxT("STROBE_LAG"),
	  wxT("Strobe lag (delay) time (s)."),
	  UNIPAR_REAL,
	  &strobePtr->delay, NULL,
	  (void * (*)) SetDelay_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_DELAY_TIMEOUT], wxT("TIMEOUT"),
	  wxT("Strobe lag (delay) timeout (s)."),
	  UNIPAR_REAL,
	  &strobePtr->delayTimeout, NULL,
	  (void * (*)) SetDelayTimeout_Utility_Strobe);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Utility_Strobe(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_Utility_Strobe");

	if (strobePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (strobePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(strobePtr->parList);

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the module's typeMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTypeMode_Utility_Strobe(WChar *theTypeMode)
{
	static const WChar	*funcName = wxT("SetTypeMode_Utility_Strobe");
	int		specifier;

	if (strobePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTypeMode,
	  strobePtr->typeModeList)) == STROBE_MODE_NULL) {
		NotifyError(wxT("%s: Illegal type mode name (%s)."), funcName,
		  theTypeMode);
		return(FALSE);
	}
	strobePtr->typeMode = specifier;
	strobePtr->updateProcessVariablesFlag = TRUE;
	switch (strobePtr->typeMode) {
	case STROBE_USER_MODE:
	case STROBE_THRESHOLD_MODE:
	case STROBE_PEAK_MODE:
		strobePtr->parList->pars[STROBE_THRESHOLD].enabled = TRUE;
		strobePtr->parList->pars[STROBE_THRESHOLD_DECAY_RATE].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY_TIMEOUT].enabled = FALSE;
		break;
	case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
		strobePtr->parList->pars[STROBE_THRESHOLD].enabled = FALSE;
		strobePtr->parList->pars[STROBE_THRESHOLD_DECAY_RATE].enabled = TRUE;
		strobePtr->parList->pars[STROBE_DELAY].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY_TIMEOUT].enabled = FALSE;
		break;
	case STROBE_PEAK_SHADOW_POSITIVE_MODE:
		strobePtr->parList->pars[STROBE_THRESHOLD].enabled = FALSE;
		strobePtr->parList->pars[STROBE_THRESHOLD_DECAY_RATE].enabled = TRUE;
		strobePtr->parList->pars[STROBE_DELAY].enabled = TRUE;
		strobePtr->parList->pars[STROBE_DELAY_TIMEOUT].enabled = TRUE;
		break;
	default:
		strobePtr->parList->pars[STROBE_THRESHOLD].enabled = FALSE;
		strobePtr->parList->pars[STROBE_THRESHOLD_DECAY_RATE].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY_TIMEOUT].enabled = FALSE;
		;
	}
	strobePtr->parList->updateFlag = TRUE;
	return(TRUE);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_Utility_Strobe(WChar *theDiagnosticMode)
{
	static const WChar	*funcName = wxT("SetDiagnosticMode_Utility_Strobe");

	if (strobePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	strobePtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  strobePtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetThreshold **********************************/

/*
 * This function sets the module's threshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetThreshold_Utility_Strobe(Float theThreshold)
{
	static const WChar	*funcName = wxT("SetThreshold_Utility_Strobe");

	if (strobePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	strobePtr->threshold = theThreshold;
	return(TRUE);

}

/****************************** SetThresholdDecayRate *************************/

/*
 * This function sets the module's thresholdDecayRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetThresholdDecayRate_Utility_Strobe(Float theThresholdDecayRate)
{
	static const WChar	*funcName = wxT("SetThresholdDecayRate_Utility_Strobe");

	if (strobePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	strobePtr->thresholdDecayRate = theThresholdDecayRate;
	strobePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetDelay **************************************/

/*
 * This function sets the module's delay parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDelay_Utility_Strobe(Float theDelay)
{
	static const WChar	*funcName = wxT("SetDelay_Utility_Strobe");

	if (strobePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	strobePtr->delay = theDelay;
	strobePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetDelayTimeout *******************************/

/*
 * This function sets the module's delayTimeout parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDelayTimeout_Utility_Strobe(Float theDelayTimeout)
{
	static const WChar	*funcName = wxT("SetDelayTimeout_Utility_Strobe");

	if (strobePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	strobePtr->delayTimeout = theDelayTimeout;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_Strobe(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_Strobe");

	if (strobePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Strobe Utility Module Parameters:-\n"));
	DPrint(wxT("\tType mode: %s,"), strobePtr->typeModeList[
	  strobePtr->typeMode].name);
	DPrint(wxT("\tDiagnostic mode: %s,\n"), strobePtr->diagnosticModeList[
	  strobePtr->diagnosticMode].name);
	if ((strobePtr->typeMode == STROBE_USER_MODE) ||
	  (strobePtr->typeMode == STROBE_THRESHOLD_MODE))
		DPrint(wxT("\tThreshold = %g units,"), strobePtr->threshold);
	if ((strobePtr->typeMode == STROBE_PEAK_MODE) ||
	  (strobePtr->typeMode == STROBE_PEAK_SHADOW_NEGATIVE_MODE) ||
	  (strobePtr->typeMode == STROBE_PEAK_SHADOW_POSITIVE_MODE))
		DPrint(wxT("\tThreshold decay rate = %g %/s,\n"), strobePtr->
		  thresholdDecayRate);
	else
		DPrint(wxT("\n"));
	if (strobePtr->typeMode == STROBE_PEAK_SHADOW_POSITIVE_MODE) {
		DPrint(wxT("\tDelay = %g ms,"), MSEC(strobePtr->delay));
		DPrint(wxT("\tDelay timeout = "));
		if (strobePtr->delayTimeout < 0.0)
			DPrint(wxT("unlimited.\n"));
		else
			DPrint(wxT("%g ms.\n"), MSEC(strobePtr->delayTimeout));
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_Strobe(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_Strobe");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	strobePtr = (StrobePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Strobe(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_Strobe");

	if (!SetParsPointer_Utility_Strobe(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_Strobe(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = strobePtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_Strobe;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_Strobe;
	theModule->PrintPars = PrintPars_Utility_Strobe;
	theModule->RunProcess = Process_Utility_Strobe;
	theModule->SetParsPointer = SetParsPointer_Utility_Strobe;
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
CheckData_Utility_Strobe(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_Strobe");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((strobePtr->typeMode == STROBE_PEAK_SHADOW_POSITIVE_MODE) &&
	  (_GetDuration_SignalData(_InSig_EarObject(data, 0)) <= strobePtr->delay)) {
		NotifyError(wxT("%s: Strobe delay (%g ms) is not less than signal\n")
		  wxT("duration (%g ms)."), funcName, MSEC(strobePtr->delay),
		  MSEC(_GetDuration_SignalData(_InSig_EarObject(data, 0))));
		return(FALSE);
	}
	return(TRUE);

}

/**************************** FreeStateVariables ******************************/

/*
 * This routine frees the space allocated for the state variables.
 */

void
FreeStateVariables_Utility_Strobe(StrobeStatePtr *p)
{
	if (*p == NULL)
		return;
	if ((*p)->lastInput)
		free((*p)->lastInput);
	free(*p);
	*p = NULL;

}

/**************************** InitStateVariables ******************************/

/*
 * This function allocates the memory for the process state variables.
 * It also carries out the necessary initialisation.
 * These are variables which must be remembered from one run to another in
 * segment processing mode.
 */

StrobeStatePtr
InitStateVariables_Utility_Strobe(ChanLen numLastSamples)
{
	static const WChar *funcName = wxT("InitStateVariables_Utility_Strobe");
	StrobeStatePtr	p;

	if ((p = (StrobeStatePtr) malloc(sizeof(StrobeState))) == NULL) {
		NotifyError(wxT("%s: Out of memory for state variables."), funcName);
		return(NULL);
	}
	if (numLastSamples) {
		if ((p->lastInput = (ChanData *) calloc(numLastSamples, sizeof(
		  ChanData))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'lastInput' array (%u ")
			  wxT("elements)."), funcName, numLastSamples);
			FreeStateVariables_Utility_Strobe(&p);
			return(NULL);
		}
	} else
		p->lastInput = NULL;
	p->gradient = FALSE;
	p->strobeAlreadyPlaced = FALSE;
	p->widthIndex = 0;
	p->delayCount = 0;
	p->prevPeakIndex = 0;
	p->prevPeakHeight = 0.0;
	p->lastSample = 0.0;
	p->threshold = 0.0;
	p->deltaThreshold = 0.0;
	return(p);

}


/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It also carries out any necessary initialisations.
 * This routine adjusts the process timing according to the response delay
 * required.
 */

BOOLN
InitProcessVariables_Utility_Strobe(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Utility_Strobe");
	register ChanData	*lastInput, *outPtr;
	int			i;
	ChanLen		j;
	StrobeStatePtr	statePtr;
	StrobePtr	p = strobePtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Utility_Strobe();
			OpenDiagnostics_NSpecLists(&strobePtr->fp,
			  strobePtr->diagnosticModeList, strobePtr->diagnosticMode);
			if (strobePtr->diagnosticMode)
				DSAM_fprintf(strobePtr->fp, wxT("Time (s)\tThreshold\n"));
			p->numChannels = _OutSig_EarObject(data)->numChannels;
			if ((p->stateVars = (StrobeStatePtr *) calloc(
			  p->numChannels, sizeof(StrobeState))) == NULL) {
		 		NotifyError(wxT("%s: Out of memory for state variables array."),
		 		  funcName);
		 		return(FALSE);
			}
			switch (p->typeMode) {
			case STROBE_PEAK_MODE:
			case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
				p->numLastSamples = 1;
				break;
			case STROBE_PEAK_SHADOW_POSITIVE_MODE:
				p->numLastSamples = (ChanLen) floor(p->delay / _OutSig_EarObject(data)->
				  dt + 0.5);
				p->delayTimeoutSamples = (ChanLen) floor((p->delayTimeout <
				  0.0)? 0: p->delayTimeout / _OutSig_EarObject(data)->dt + 0.5);
				break;
			default:
				p->numLastSamples = 0;
			}
			p->thresholdDecay = p->thresholdDecayRate / 100.0 *
			  _OutSig_EarObject(data)->dt;
			for (i = 0; i < p->numChannels; i++) {
				if ((p->stateVars[i] = InitStateVariables_Utility_Strobe(
				  p->numLastSamples)) == NULL) {
					NotifyError(wxT("%s: Out of memory for state variable ")
					  wxT("(%d)."), funcName, i);
					FreeProcessVariables_Utility_Strobe();
					return(FALSE);
				}
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			statePtr = p->stateVars[i];
			lastInput = statePtr->lastInput;
			outPtr = _OutSig_EarObject(data)->channel[i];
			statePtr->lastSample = _InSig_EarObject(data, 0)->channel[i][0];
			for (j = 0; j < p->numLastSamples; j++) {
				*lastInput++ = _InSig_EarObject(data, 0)->channel[i][0];
				*outPtr++ = 0.0;
			}
			statePtr->gradient = FALSE;
			statePtr->widthIndex = 0;
			statePtr->prevPeakIndex = 0;
			statePtr->prevPeakHeight = _InSig_EarObject(data, 0)->channel[i][0];
			statePtr->delayCount = 0;
			statePtr->delayTimeoutCount = 0;
			statePtr->deltaThreshold = 0.0;
			switch (p->typeMode) {
			case STROBE_PEAK_MODE:
				statePtr->threshold = strobePtr->threshold;
				break;
			case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
			case STROBE_PEAK_SHADOW_POSITIVE_MODE:
				statePtr->threshold = _InSig_EarObject(data, 0)->channel[i][0];
				break;
			default:
				;
			} /* switch */
		}

	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Utility_Strobe(void)
{
	int		i;

	if (strobePtr->stateVars) {
		for (i = 0; i < strobePtr->numChannels; i++)
    		FreeStateVariables_Utility_Strobe(&strobePtr->stateVars[i]);
		free(strobePtr->stateVars);
		strobePtr->stateVars = NULL;
	}
	CloseDiagnostics_NSpecLists(&strobePtr->fp);
	strobePtr->updateProcessVariablesFlag = TRUE;

}

/****************************** ProcessThesholdModes **************************/

/*
 * This routine carries out the threshold strobe processing mode.
 * It is used by both the "threshold" and "user" modes.
 * It assumes that all the correct module intialisation has been performed.
 */

void
ProcessThesholdModes_Utility_Strobe(EarObjectPtr data)
{
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	ChanLen	i;

	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->numChannels;
	  chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = _OutSig_EarObject(data)->channel[chan];
		for (i = 0; i < _InSig_EarObject(data, 0)->length; i++)
			*outPtr++ = (*inPtr++ > strobePtr->threshold)? 1.0: 0.0;
	}

}

/****************************** ProcessPeakChannel ****************************/

/*
 * This routine carries out the peak strobe processing for a range of data.
 * A special provision had to be made for the buffer reading when tests for the
 * end of a peak are made across the end boundary.
 * For reading past the end of the PROCESS_DATA_CHANNEL an copy of the count is
 * used to preserve the count contents, and if the delay reaches zero, and the
 * strobe point is set, then the previous height is set to less than the current
 * input signal height, to ensure no attempt is made to place the strobe point.
 */

#define	_SetDecayStrobeTrigger()  \
			s->threshold = *inPtr; \
			s->deltaThreshold = *inPtr * p->thresholdDecay;

void
ProcessPeakChannel_Utility_Strobe(EarObjectPtr data,
  StrobeChanProcessSpecifier chanProcessSpecifier)
{
	register	ChanData	 *inPtr, *outPtr;
	BOOLN		delayTimeout, gradient, foundPeak;
	int			chan;
	ChanLen		i, length, endProcessCount, endProcessLimitCount;
	ChanData	nextSample, lastSample;
	StrobeStatePtr	s;
	StrobePtr	p = strobePtr;

	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->numChannels;
	  chan++) {
		s = p->stateVars[chan];
		if (chanProcessSpecifier == STROBE_PROCESS_DATA_CHANNEL) {
			inPtr = _InSig_EarObject(data, 0)->channel[chan];
			outPtr = _OutSig_EarObject(data)->channel[chan] + p->numLastSamples;
			length = _InSig_EarObject(data, 0)->length - p->numLastSamples;
		} else {
			inPtr = s->lastInput;
			outPtr = _OutSig_EarObject(data)->channel[chan];
			length = p->numLastSamples;
		}
		for (i = 0; i < length; i++, inPtr++, outPtr++) {
			*outPtr = 0.0;
			if (!s->gradient)
				s->gradient = (*inPtr > s->lastSample);
			if (s->gradient) {
				nextSample = ((chanProcessSpecifier ==
				  STROBE_PROCESS_DATA_CHANNEL) || (i < length - 1))? *(inPtr +
				  1): *(_InSig_EarObject(data, 0)->channel[chan]);
				if (*inPtr > nextSample) {
					if (*inPtr > s->threshold) {
						switch (p->typeMode) {
						case STROBE_PEAK_MODE:
							*(outPtr - s->widthIndex / 2) = STROBE_SPIKE_UNIT;
							break;
						case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
							*(outPtr - s->widthIndex / 2) = STROBE_SPIKE_UNIT;
							_SetDecayStrobeTrigger();
							break;
						case STROBE_PEAK_SHADOW_POSITIVE_MODE:
							if (!s->delayCount || (s->prevPeakHeight <
							  *inPtr)) {
								s->prevPeakHeight = *inPtr;
								s->prevPeakIndex = (chanProcessSpecifier ==
								  STROBE_PROCESS_BUFFER_CHANNEL)? i: i +
								  p->numLastSamples;
								s->delayCount = p->numLastSamples + 1;
								_SetDecayStrobeTrigger();
								if (p->delayTimeoutSamples && !s->
								  delayTimeoutCount)
									s->delayTimeoutCount = p->
									  delayTimeoutSamples;
							}
							break;
						default:
							;
						}
					}
					s->widthIndex = 0;
					s->gradient = FALSE;
				} else if (*inPtr == nextSample) /* - for flat troughs.*/
					s->widthIndex++;
			}
			if (p->diagnosticMode)
				DSAM_fprintf(p->fp, wxT("%g\t%g\n"), (data->timeIndex + i) * data->
				  outSignal->dt, s->threshold);
			s->threshold -= s->deltaThreshold;
			if (p->typeMode == STROBE_PEAK_SHADOW_POSITIVE_MODE) {
				delayTimeout = (p->delayTimeoutSamples &&
				  s->delayTimeoutCount && !--s->delayTimeoutCount);
				if (s->delayCount && (!--s->delayCount || delayTimeout)) {
					if (!s->strobeAlreadyPlaced)
						*(_OutSig_EarObject(data)->channel[chan] + s->prevPeakIndex) =
						  STROBE_SPIKE_UNIT;
					else
						s->strobeAlreadyPlaced = FALSE;
					s->delayTimeoutCount = 0;
				}
			}
			s->lastSample = *inPtr;
		}
		if ((chanProcessSpecifier == STROBE_PROCESS_DATA_CHANNEL) &&
		  s->delayCount) {		/* Look past end of process for peak */
			endProcessLimitCount = s->delayTimeoutCount;
			lastSample = s->lastSample;
			for (endProcessCount = s->delayCount, delayTimeout = foundPeak =
			  gradient = FALSE; endProcessCount && !foundPeak && !delayTimeout;
			  endProcessCount--) {
				if (!gradient)
					gradient = (*inPtr > lastSample);
				if (gradient) {
					if (*inPtr > *(inPtr + 1)) {
						foundPeak = (s->prevPeakHeight < *inPtr);
						gradient = FALSE;
					}
				}
				delayTimeout = (p->delayTimeoutSamples &&
				  endProcessLimitCount && !--endProcessLimitCount);
				lastSample = *inPtr++;
			}
			if (!foundPeak || delayTimeout) {
				*(_OutSig_EarObject(data)->channel[chan] + s->prevPeakIndex) =
				  STROBE_SPIKE_UNIT;
				s->strobeAlreadyPlaced = TRUE;
			}
		}
	}

}

#undef _SetDecayStrobeTrigger

/****************************** ProcessPeakModes ******************************/

/*
 * This routine carries out the peak strobe processing modes.
 * It assumes that all the correct module intialisation has been performed.
 */

void
ProcessPeakModes_Utility_Strobe(EarObjectPtr data)
{
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	ChanLen	i;
	SignalDataPtr	inSignal, outSignal;

	ProcessPeakChannel_Utility_Strobe(data, STROBE_PROCESS_BUFFER_CHANNEL);
	ProcessPeakChannel_Utility_Strobe(data, STROBE_PROCESS_DATA_CHANNEL);
	inSignal =_InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	if (strobePtr->numLastSamples)
		for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
			inPtr = inSignal->channel[chan] + inSignal->length - strobePtr->
			  numLastSamples;
			outPtr = strobePtr->stateVars[chan]->lastInput;
			for (i = 0; i < strobePtr->numLastSamples; i++)
				*outPtr++ = *inPtr++;
		}

}

/****************************** Process ***************************************/

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
 */

BOOLN
Process_Utility_Strobe(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_Strobe");

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_Strobe(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Strobe Utility Module process"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Utility_Strobe(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data),
		  -_OutSig_EarObject(data)->dt * strobePtr->numLastSamples);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	switch (strobePtr->typeMode) {
	case STROBE_USER_MODE:
	case STROBE_THRESHOLD_MODE:
		ProcessThesholdModes_Utility_Strobe(data);
		break;
	case STROBE_PEAK_MODE:
	case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
	case STROBE_PEAK_SHADOW_POSITIVE_MODE:
		ProcessPeakModes_Utility_Strobe(data);
		break;
	default:
		NotifyError(wxT("%s: Unimplemented mode (%d)."), funcName,
		  strobePtr->typeMode);
		return(FALSE);
	} /* switch */
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

