/**********************
 *
 * File:		FlBandPass.c
 * Purpose:		This is an implementation of a pre-emphasis band pass filter
 *				using the filters.c module.
 *				Beauchamp K. and Yuen C. (1979) "Digital Methods for signal
 *				analysis", George Allen & Unwin, London. P 256-257.
 * Comments:	The band-pass filter is used to model the effects of the
 *				outer- and middle-ear on an input signal.
 *				LPO (10-09-95): A pre-attenuation parameter has been added to
 *				the module definition.
 *				20-07-98 LPO: Name changed from MoPreEmph.[ch]
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		20 Jul 1998
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtFilters.h"
#include "UtString.h"
#include "FlBandPass.h"

/******************************************************************************/
/**************************** Global subroutines ******************************/
/******************************************************************************/

BandPassFPtr	bandPassFPtr = NULL;

/******************************************************************************/
/**************************** Subroutines and functions ***********************/
/******************************************************************************/

/********************************* Init ***************************************/

/*
 * This function initialises the module by setting module's parameter pointer
 * structure.
 * The GLOBAL option is for hard programming - it sets the module's pointer to
 * the global parameter structure and initialises the parameters.
 * The LOCAL option is for generic programming - it initialises the parameter
 * structure currently pointed to by the module's parameter pointer.
 */

BOOLN
Init_Filter_BandPass(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_Filter_BandPass");

	if (parSpec == GLOBAL) {
		if (bandPassFPtr != NULL)
			Free_Filter_BandPass();
		if ((bandPassFPtr = (BandPassFPtr) malloc(sizeof(BandPassF))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bandPassFPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bandPassFPtr->parSpec = parSpec;
	bandPassFPtr->updateProcessVariablesFlag = TRUE;
	bandPassFPtr->cascade = 2;
	bandPassFPtr->upperCutOffFreq = 8500.0;
	bandPassFPtr->lowerCutOffFreq = 450.0;
	bandPassFPtr->preAttenuation = 0.0;

	if (!SetUniParList_Filter_BandPass()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Filter_BandPass();
		return(FALSE);
	}
	bandPassFPtr->coefficients = NULL;
	bandPassFPtr->numChannels = 0;
	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface requires that a non-void value be returned.
 */

BOOLN
Free_Filter_BandPass(void)
{
	if (bandPassFPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Filter_BandPass();
	if (bandPassFPtr->coefficients)
		free(bandPassFPtr->coefficients);
	if (bandPassFPtr->parList)
		FreeList_UniParMgr(&bandPassFPtr->parList);
	if (bandPassFPtr->parSpec == GLOBAL) {
		free(bandPassFPtr);
		bandPassFPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_Filter_BandPass(void)
{
	static const WChar *funcName = wxT("SetUniParList_Filter_BandPass");
	UniParPtr	pars;

	if ((bandPassFPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BANDPASS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bandPassFPtr->parList->pars;
	SetPar_UniParMgr(&pars[BANDPASS_FILTERCASCADE], wxT("CASCADE"),
	  wxT("Filter cascade."),
	  UNIPAR_INT,
	  &bandPassFPtr->cascade, NULL,
	  (void * (*)) SetCascade_Filter_BandPass);
	SetPar_UniParMgr(&pars[BANDPASS_PREATTENUATION], wxT("GAIN"),
	  wxT("Pre-attenuation for filter (dB)."),
	  UNIPAR_REAL,
	  &bandPassFPtr->preAttenuation, NULL,
	  (void * (*)) SetPreAttenuation_Filter_BandPass);
	SetPar_UniParMgr(&pars[BANDPASS_LOWERCUTOFFFREQ], wxT("LOWER_FREQ"),
	  wxT("Lower, 3 dB down cut-off frequency (Hz)."),
	  UNIPAR_REAL,
	  &bandPassFPtr->lowerCutOffFreq, NULL,
	  (void * (*)) SetLowerCutOffFreq_Filter_BandPass);
	SetPar_UniParMgr(&pars[BANDPASS_UPPERCUTOFFFREQ], wxT("UPPER_FREQ"),
	  wxT("Upper, 3 dB down cut-off frequency (Hz)."),
	  UNIPAR_REAL,
	  &bandPassFPtr->upperCutOffFreq, NULL,
	  (void * (*)) SetUpperCutOffFreq_Filter_BandPass);

	SetAltAbbreviation_UniParMgr(&pars[BANDPASS_PREATTENUATION], wxT(
	  "ATTENUATION"));
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Filter_BandPass(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_Filter_BandPass");

	if (bandPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bandPassFPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bandPassFPtr->parList);

}

/********************************* SetCascade *********************************/

/*
 * This function sets the module's lower, 3dB down cut off frequency parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCascade_Filter_BandPass(int theCascade)
{
	static const WChar *funcName = wxT("SetCascade_Filter_BandPass");

	if (bandPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theCascade < 1) {
		NotifyError(wxT("%s: This value must be greater than 0 (%d).\n"),
		  funcName, theCascade);
		return(FALSE);
	}
	bandPassFPtr->cascade = theCascade;
	return(TRUE);

}

/********************************* SetLowerCutOffFreq *************************/

/*
 * This function sets the module's lower, 3dB down cut off frequency parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLowerCutOffFreq_Filter_BandPass(Float theLowerCutOffFreq)
{
	static const WChar	*funcName = wxT("SetLowerCutOffFreq_Filter_BandPass");

	if (bandPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bandPassFPtr->lowerCutOffFreq = theLowerCutOffFreq;
	bandPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetUpperCutOffFreq *************************/

/*
 * This function sets the module's upper, 3dB down cut off frequency parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetUpperCutOffFreq_Filter_BandPass(Float theUpperCutOffFreq)
{
	static const WChar	*funcName = wxT("SetUpperCutOffFreq_Filter_BandPass");

	if (bandPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bandPassFPtr->upperCutOffFreq = theUpperCutOffFreq;
	bandPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************** SetPreAttenuation *************************************/

/*
 * This function sets the module's total filter attenuation parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPreAttenuation_Filter_BandPass(Float thePreAttenuation)
{
	static const WChar	*funcName = wxT("SetPreAttenuation_Filter_BandPass");

	if (bandPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bandPassFPtr->preAttenuation = thePreAttenuation;
	bandPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_Filter_BandPass(void)
{
	DPrint(wxT("Band Pass Filter Module Parameters:-\n"));
	DPrint(wxT("\tFilter cascade = %d,\tPre-attenuation = %g dB\n"),
	  bandPassFPtr->cascade, bandPassFPtr->preAttenuation);
	DPrint(wxT("\tLower/Upper cut-off Frequencies = %g / %g Hz.\n"),
	  bandPassFPtr->lowerCutOffFreq, bandPassFPtr->upperCutOffFreq);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Filter_BandPass(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Filter_BandPass");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bandPassFPtr = (BandPassFPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Filter_BandPass(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Filter_BandPass");

	if (!SetParsPointer_Filter_BandPass(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Filter_BandPass(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = bandPassFPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Filter_BandPass;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_BandPass;
	theModule->PrintPars = PrintPars_Filter_BandPass;
	theModule->RunProcess = RunModel_Filter_BandPass;
	theModule->SetParsPointer = SetParsPointer_Filter_BandPass;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Filter_BandPass(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Filter_BandPass");
	BOOLN	ok = TRUE;
	int		i, j;
	Float	*statePtr;
	BandPassFPtr	p = bandPassFPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_BandPass();
		if ((p->coefficients = (BandPassCoeffsPtr *) calloc(_OutSig_EarObject(data)->
		  numChannels, sizeof(BandPassCoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory."), funcName);
		 	return(FALSE);
		}
		p->numChannels = _OutSig_EarObject(data)->numChannels;
	 	for (i = 0; (i < _OutSig_EarObject(data)->numChannels) && ok; i++)
			if ((p->coefficients[i] = InitBandPassCoeffs_Filters(p->cascade,
			  p->lowerCutOffFreq, p->upperCutOffFreq, _InSig_EarObject(data,
			  0)->dt)) == NULL) {
				NotifyError(wxT("%s: Failed initialised filter channel %d."),
				  funcName, i);
				ok = FALSE;
			}
		if (!ok) {
			FreeProcessVariables_Filter_BandPass();
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			statePtr = p->coefficients[i]->state;
			for (j = 0; j < p->cascade * FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
				*statePtr++ = 0.0;
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
FreeProcessVariables_Filter_BandPass(void)
{
	int		i;

	if (bandPassFPtr->coefficients == NULL)
		return;
	for (i = 0; i < bandPassFPtr->numChannels; i++)
		FreeBandPassCoeffs_Filters(&bandPassFPtr->coefficients[i]);
	free(bandPassFPtr->coefficients);
	bandPassFPtr->coefficients = NULL;

}

/**************************** RunModel ****************************************/

/*
 * This routine filters the input signal and puts the result into the output
 * signal.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * The appropriate coefficients are calculated at the beginning, then the
 * memory used is released before the program returns.
 */

BOOLN
RunModel_Filter_BandPass(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_Filter_BandPass");
	BandPassFPtr	p = bandPassFPtr;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Band pass filter module process"));
		if (!CheckInSignal_EarObject(data, funcName))
			return(FALSE);
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
		if (!InitOutTypeFromInSignal_EarObject(data, 0)) {
			NotifyError(wxT("%s: Could not initialise the process output ")
			  wxT("signal."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Filter_BandPass(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	InitOutDataFromInSignal_EarObject(data);

	/* Filter signal */

	if (fabs(p->preAttenuation) > DSAM_EPSILON)
		GaindB_SignalData(_OutSig_EarObject(data), p->preAttenuation);
	BandPass_Filters(_OutSig_EarObject(data), p->coefficients);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
