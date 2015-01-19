/**********************
 *
 * File:		FlLowPass.c
 * Purpose:		This is an implementation of a 1st order Butterworth low-pass
 *				filter.  The filter is used to approximate the effects of
 *				dendrite filtering.
 * Comments:	It uses the filter IIR filter from the UtFilter module.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		03 Dec 1996
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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
#include "FlLowPass.h"

/******************************************************************************/
/**************************** Global subroutines ******************************/
/******************************************************************************/

LowPassFPtr	lowPassFPtr = NULL;

/******************************************************************************/
/**************************** Subroutines and functions ***********************/
/******************************************************************************/

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Filter_LowPass(void)
{
	static NameSpecifier	list[] = {

					{ wxT("NORMAL"),	FILTER_LOW_PASS_MODE_NORMAL },
					{ wxT("SCALED"),	FILTER_LOW_PASS_MODE_SCALED },
					{ NULL, 			FILTER_LOW_PASS_MODE_NULL }
				};
	lowPassFPtr->modeList = list;
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
Init_Filter_LowPass(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_Filter_LowPass");

	if (parSpec == GLOBAL) {
		if (lowPassFPtr != NULL)
			Free_Filter_LowPass();
		if ((lowPassFPtr = (LowPassFPtr) malloc(sizeof(LowPassF))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (lowPassFPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	lowPassFPtr->parSpec = parSpec;
	lowPassFPtr->updateProcessVariablesFlag = TRUE;
	lowPassFPtr->mode = FILTER_LOW_PASS_MODE_NORMAL;
	lowPassFPtr->cutOffFrequency = 5000.0;
	lowPassFPtr->signalMultiplier = 1.0;
	lowPassFPtr->numChannels = 0;
	lowPassFPtr->coefficients = NULL;

	InitModeList_Filter_LowPass();
	if (!SetUniParList_Filter_LowPass()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Filter_LowPass();
		return(FALSE);
	}
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
Free_Filter_LowPass(void)
{
	if (lowPassFPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Filter_LowPass();
	if (lowPassFPtr->parList)
		FreeList_UniParMgr(&lowPassFPtr->parList);
	if (lowPassFPtr->parSpec == GLOBAL) {
		free(lowPassFPtr);
		lowPassFPtr = NULL;
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
SetUniParList_Filter_LowPass(void)
{
	static const WChar *funcName = wxT("SetUniParList_Filter_LowPass");
	UniParPtr	pars;

	if ((lowPassFPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  FILTER_LOW_PASS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = lowPassFPtr->parList->pars;
	SetPar_UniParMgr(&pars[FILTER_LOW_PASS_MODE], wxT("MODE"),
	  wxT("Output mode for model ('normal' or 'scaled')."),
	  UNIPAR_NAME_SPEC,
	  &lowPassFPtr->mode, lowPassFPtr->modeList,
	  (void * (*)) SetMode_Filter_LowPass);
	SetPar_UniParMgr(&pars[FILTER_LOW_PASS_CUTOFFFREQUENCY], wxT(
	  "CUT_OFF_FREQ"),
	  wxT("Cut-off frequency (3 dB down point - Hz)."),
	  UNIPAR_REAL,
	  &lowPassFPtr->cutOffFrequency, NULL,
	  (void * (*)) SetCutOffFrequency_Filter_LowPass);
	SetPar_UniParMgr(&pars[FILTER_LOW_PASS_SIGNALMULTIPLIER], wxT("SCALE"),
	  wxT("Signal multiplier scale ('scaled' mode only)."),
	  UNIPAR_REAL,
	  &lowPassFPtr->signalMultiplier, NULL,
	  (void * (*)) SetSignalMultiplier_Filter_LowPass);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Filter_LowPass(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Filter_LowPass");

	if (lowPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lowPassFPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(lowPassFPtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Filter_LowPass(WChar *theMode)
{
	static const WChar	*funcName = wxT("SetMode_Filter_LowPass");
	int		specifier;

	if (lowPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, lowPassFPtr->modeList)) ==
	  FILTER_LOW_PASS_MODE_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	lowPassFPtr->mode = specifier;
	lowPassFPtr->parList->pars[FILTER_LOW_PASS_SIGNALMULTIPLIER].enabled =
	  (specifier == FILTER_LOW_PASS_MODE_SCALED);
	return(TRUE);

}

/********************************* SetCutOffFrequency *************************/

/*
 * This function sets the module's lower, 3dB down cut off frequency parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCutOffFrequency_Filter_LowPass(Float theCutOffFrequency)
{
	static const WChar	 *funcName = wxT("SetCutOffFrequency_Filter_LowPass");

	if (lowPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	lowPassFPtr->cutOffFrequency = theCutOffFrequency;
	lowPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetSignalMultiplier ************************/

/*
 * This function sets the module's signal multiplying factor.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSignalMultiplier_Filter_LowPass(Float theSignalMultiplier)
{
	static const WChar	 *funcName = wxT("SetSignalMultiplier_Filter_LowPass");

	if (lowPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	lowPassFPtr->signalMultiplier = theSignalMultiplier;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_Filter_LowPass(void)
{
	DPrint(wxT("Low-Pass Filter Module Parameters:-\n"));
	DPrint(wxT("\tMode = %s,"),
	  lowPassFPtr->modeList[lowPassFPtr->mode].name);
	DPrint(wxT("\tCut-off Frequency = %g Hz\n"), lowPassFPtr->cutOffFrequency);
	if (lowPassFPtr->mode == FILTER_LOW_PASS_MODE_SCALED)
		DPrint(wxT("\tSignal multiplying factor = %g (?).\n"),
		  lowPassFPtr->signalMultiplier);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Filter_LowPass(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Filter_LowPass");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	lowPassFPtr = (LowPassFPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Filter_LowPass(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Filter_LowPass");

	if (!SetParsPointer_Filter_LowPass(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Filter_LowPass(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = lowPassFPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Filter_LowPass;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_LowPass;
	theModule->PrintPars = PrintPars_Filter_LowPass;
	theModule->RunProcess = RunProcess_Filter_LowPass;
	theModule->SetParsPointer = SetParsPointer_Filter_LowPass;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Filter_LowPass(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Filter_LowPass");
	int		i, j;
	Float	*statePtr;
	LowPassFPtr	p = lowPassFPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_LowPass();
		if ((p->coefficients = (ContButt1CoeffsPtr *) calloc(_OutSig_EarObject(
		  data)->numChannels, sizeof(ContButt1CoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory."), funcName);
		 	return(FALSE);
		}
		p->numChannels = _OutSig_EarObject(data)->numChannels;
	 	for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++)
			if ((p->coefficients[i] = InitIIR1ContCoeffs_Filters(
			  p->cutOffFrequency, _InSig_EarObject(data, 0)->dt, LOWPASS)) ==
			  NULL) {
				NotifyError(wxT("%s: Could not allocate filter coefficients."),
				  funcName);
				return(FALSE);
			}
		p->updateProcessVariablesFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			statePtr = p->coefficients[i]->state;
			for (j = 0; j < FILTERS_NUM_CONTBUTT1_STATE_VARS; j++)
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
FreeProcessVariables_Filter_LowPass(void)
{
	int		i;

	if (lowPassFPtr->coefficients == NULL)
		return;
	for (i = 0; i < lowPassFPtr->numChannels; i++)
		free(lowPassFPtr->coefficients[i]);
	free(lowPassFPtr->coefficients);
	lowPassFPtr->coefficients = NULL;
	lowPassFPtr->updateProcessVariablesFlag = TRUE;

}

/**************************** RunModel ****************************************/

/*
 * This routine filters the input signal and puts the result into the output
 * signal.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * This routine adjusts the signal inputs for the sampling interval, i.e.
 * each sample is multiplied by, pulse duration / dt.
 */

BOOLN
RunProcess_Filter_LowPass(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunProcess_Filter_LowPass");
	int			chan;
	ChanLen		i;
	register ChanData	*inPtr, *outPtr;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Low-pass filter process"));
		if (!CheckInSignal_EarObject(data, funcName))
			return(FALSE);
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Couldn't initialse output signal."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Filter_LowPass(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		switch (lowPassFPtr->mode) {
		case FILTER_LOW_PASS_MODE_NORMAL:
			for (i = 0; i < _InSig_EarObject(data, 0)->length; i++)
				*outPtr++ = *inPtr++;
			break;
		case FILTER_LOW_PASS_MODE_SCALED:
			for (i = 0; i < _InSig_EarObject(data, 0)->length; i++)
				*outPtr++ = *inPtr++ * lowPassFPtr->signalMultiplier;
			break;
		} /* switch */

	}

	/* Filter signal */

	IIR1Cont_Filters(outSignal, lowPassFPtr->coefficients);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
