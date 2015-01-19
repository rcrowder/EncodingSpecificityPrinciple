/**********************
 *
 * File:		MoBMGammaT.c
 * Purpose:		This is an implementation of a Gamma Tone filter using the
 *				UtFilters.c module.
 * Comments:	The Gamma Tone filter is used to model the response of the
 *				Basilar Membrane.
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "GeNSpecLists.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtFilters.h"
#include "UtString.h"
#include "MoBMGammaT.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

BMGammaTPtr	bMGammaTPtr = NULL;

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface requires that a non-void value be returned.
 */

BOOLN
Free_BasilarM_GammaT(void)
{
	if (bMGammaTPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_GammaT();
	Free_CFList(&bMGammaTPtr->theCFs);
	if (bMGammaTPtr->parList)
		FreeList_UniParMgr(&bMGammaTPtr->parList);
	if (bMGammaTPtr->parSpec == GLOBAL) {
		free(bMGammaTPtr);
		bMGammaTPtr = NULL;
	}
	return(TRUE);

}

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
Init_BasilarM_GammaT(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_BasilarM_GammaT");

	if (parSpec == GLOBAL) {
		if (bMGammaTPtr != NULL)
			Free_BasilarM_GammaT();
		if ((bMGammaTPtr = (BMGammaTPtr) malloc(sizeof(BMGammaT))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMGammaTPtr == NULL) {
			NotifyError(wxT("%s: 'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bMGammaTPtr->parSpec = parSpec;
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	bMGammaTPtr->interleaveMode = GENERAL_BOOLEAN_ON;
	bMGammaTPtr->cascade = 4;
	if ((bMGammaTPtr->theCFs = GenerateDefault_CFList(
	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS,
	  CFLIST_DEFAULT_LOW_FREQ, CFLIST_DEFAULT_HIGH_FREQ,
	  CFLIST_DEFAULT_BW_MODE_NAME, CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {
		NotifyError(wxT("%s: could not set default CFList."), funcName);
		return(FALSE);
	}

	if (!SetUniParList_BasilarM_GammaT()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_GammaT();
		return(FALSE);
	}
	bMGammaTPtr->numChannels = 0;
	bMGammaTPtr->coefficients = NULL;
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_BasilarM_GammaT(void)
{
	static const WChar *funcName = wxT("SetUniParList_BasilarM_GammaT");
	UniParPtr	pars;

	if ((bMGammaTPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BM_GAMMAT_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bMGammaTPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_GAMMAT_INTERLEAVEMODE], wxT("INTERLEAVE_MODE"),
	  wxT("Interleaving of output channels relative to input channels ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &bMGammaTPtr->interleaveMode, NULL,
	  (void * (*)) SetInterleaveMode_BasilarM_GammaT);
	SetPar_UniParMgr(&pars[BM_GAMMAT_CASCADE], wxT("CASCADE"),
	  wxT("Filter cascade."),
	  UNIPAR_INT,
	  &bMGammaTPtr->cascade, NULL,
	  (void * (*)) SetCascade_BasilarM_GammaT);
	SetPar_UniParMgr(&pars[BM_GAMMAT_THE_CFS], wxT("CFLIST"),
	  wxT("Centre frequency specification"),
	  UNIPAR_CFLIST,
	  &bMGammaTPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_GammaT);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_GammaT(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_BasilarM_GammaT");

	if (bMGammaTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMGammaTPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bMGammaTPtr->parList);

}

/****************************** SetInterleaveMode *****************************/

/*
 * This function sets the module's interleaveMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInterleaveMode_BasilarM_GammaT(WChar * theInterleaveMode)
{
	static const WChar	*funcName = wxT("SetInterleaveMode_BasilarM_GammaT");
	int		specifier;

	if (bMGammaTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theInterleaveMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName, theInterleaveMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	bMGammaTPtr->interleaveMode = specifier;
	return(TRUE);

}

/********************************* SetCascade *********************************/

/*
 * This function sets the module's cascade parameter.
 * It first checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCascade_BasilarM_GammaT(int theCascade)
{
	static const WChar	 *funcName = wxT("SetCascade_BasilarM_GammaT");

	if (bMGammaTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMGammaTPtr->cascade = theCascade;
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetCFList **********************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_GammaT(CFListPtr theCFList)
{
	static const WChar *funcName = wxT("SetCFList_BasilarM_GammaT");

	if (bMGammaTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not correctly set."),
		  funcName);
		return(FALSE);
	}
	if (bMGammaTPtr->theCFs != NULL)
		Free_CFList(&bMGammaTPtr->theCFs);
	bMGammaTPtr->theCFs = theCFList;
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetBandwidths ******************************/

/*
 * This function sets the band width mode for the gamma tone filters.
 * The band width mode defines the function for calculating the gamma tone
 * filter band width 3 dB down.
 * No checks are made on the correct length for the bandwidth array.
 */

BOOLN
SetBandwidths_BasilarM_GammaT(WChar *theBandwidthMode, Float *theBandwidths)
{
	static const WChar *funcName = wxT("SetBandwidths_BasilarM_GammaT");

	if (bMGammaTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(bMGammaTPtr->theCFs, theBandwidthMode,
	  theBandwidths)) {
		NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
		return(FALSE);
	}
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 *
 */

CFListPtr
GetCFListPtr_BasilarM_GammaT(void)
{
	static const WChar *funcName = wxT("GetCFListPtr_BasilarM_GammaT");

	if (bMGammaTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMGammaTPtr->theCFs == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly ")
		  wxT("set. NULL returned."), funcName);
		return(NULL);
	}
	return(bMGammaTPtr->theCFs);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_BasilarM_GammaT(void)
{
	DPrint(wxT("Gamma Tone Basilar Membrane Filter  Module Parameters:-\n"));
	PrintPars_CFList(bMGammaTPtr->theCFs);
	DPrint(wxT("\tInterleave mode = %s,\n"),
	  BooleanList_NSpecLists(bMGammaTPtr->interleaveMode)->name);
	DPrint(wxT("\tFilter cascade = %d.\n"), bMGammaTPtr->cascade);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_GammaT(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_BasilarM_GammaT");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bMGammaTPtr = (BMGammaTPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_GammaT(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_GammaT");

	if (!SetParsPointer_BasilarM_GammaT(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_GammaT(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMGammaTPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_BasilarM_GammaT;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_GammaT;
	theModule->PrintPars = PrintPars_BasilarM_GammaT;
	theModule->RunProcess = RunModel_BasilarM_GammaT;
	theModule->SetParsPointer = SetParsPointer_BasilarM_GammaT;
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_BasilarM_GammaT(void)
{
	int		i;

	if (bMGammaTPtr->coefficients == NULL)
		return;
	for (i = 0; i < bMGammaTPtr->numChannels; i++)
    	FreeGammaToneCoeffs_Filters(&bMGammaTPtr->coefficients[i]);
	free(bMGammaTPtr->coefficients);
	bMGammaTPtr->coefficients = NULL;
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;

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
CheckData_BasilarM_GammaT(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_BasilarM_GammaT");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
		NotifyError(wxT("%s: Input signal not correctly initialised."),
		  funcName);
		return(FALSE);
	}
	if (!bMGammaTPtr->interleaveMode && (_InSig_EarObject(data, 0)->
	  numChannels != bMGammaTPtr->theCFs->numChannels)) {
		NotifyError(wxT("%s: No. of input channels (%d) must be the same as ")
		  wxT("the no. of CFs (%d) when not in interleave mode."),
		  funcName, _InSig_EarObject(data, 0)->numChannels, bMGammaTPtr->
		  theCFs->numChannels);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_BasilarM_GammaT(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_BasilarM_GammaT");
	int		i, j, cFIndex, stateVectorLength;
	Float	sampleRate, *ptr;
	BMGammaTPtr	p = bMGammaTPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || p->theCFs->
	  updateFlag) {
		FreeProcessVariables_BasilarM_GammaT();
		if ((p->coefficients = (GammaToneCoeffsPtr *) calloc(_OutSig_EarObject(
		  data)->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory."), funcName);
		 	return(FALSE);
		}
		sampleRate = 1.0 / _InSig_EarObject(data, 0)->dt;
		p->numChannels = _OutSig_EarObject(data)->numChannels;
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			cFIndex = i / _InSig_EarObject(data, 0)->interleaveLevel;
			if ((p->coefficients[i] = InitGammaToneCoeffs_Filters(p->theCFs->
			  frequency[cFIndex], p->theCFs->bandwidth[cFIndex], p->cascade,
			  sampleRate)) == NULL) {
				NotifyError(wxT("%s: Could not initialise coefficients for ")
				  wxT("channel %d."), funcName, i);
				return(FALSE);
			}
		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data),
		  wxT("Frequency (Hz)"));
		SetInfoChannelLabels_SignalData(_OutSig_EarObject(data), p->theCFs->
		  frequency);
		SetInfoCFArray_SignalData(_OutSig_EarObject(data), p->theCFs->
		  frequency);
		p->updateProcessVariablesFlag = FALSE;
		p->theCFs->updateFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		stateVectorLength = p->cascade *
		  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER;
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			ptr = p->coefficients[i]->stateVector;
			for (j = 0; j < stateVectorLength; j++)
				*ptr++ = 0.0;
		}

	}
	return(TRUE);

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
RunModel_BasilarM_GammaT(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_BasilarM_GammaT");
	uShort	totalChannels;
	BMGammaTPtr p = bMGammaTPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_BasilarM_GammaT(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Gamma tone basilar membrane ")
		  wxT("filtering"));
		totalChannels = (p->interleaveMode)? p->theCFs->numChannels *
		  _InSig_EarObject(data, 0)->numChannels: p->theCFs->numChannels;
		if (!InitOutTypeFromInSignal_EarObject(data, totalChannels)) {
			NotifyError(wxT("%s: Cannot initialise output channel."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_BasilarM_GammaT(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	InitOutDataFromInSignal_EarObject(data);
	GammaTone_Filters(_OutSig_EarObject(data), bMGammaTPtr->coefficients);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
