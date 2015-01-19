/**********************
 *
 * File:		FlDistLowPass.c
 * Purpose:		This is an implementation of a 1st order Butterworth low-pass
 *				filter.  The filter is used to approximate the effects of
 *				dendrite filtering.
 *				This version has been revised from FlLowPass and produces a
 *				different filter for each channel.
 * Comments:	It uses the filter IIR filter from the UtFilter module.
 * Author:		L. P. O'Mard
 * Created:		26 Nov 2009
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
#include "FlDistLowPass.h"

/******************************************************************************/
/**************************** Global subroutines ******************************/
/******************************************************************************/

DLowPassFPtr	dLowPassFPtr = NULL;

/******************************************************************************/
/**************************** Subroutines and functions ***********************/
/******************************************************************************/

/****************************** ModeList **************************************/

NameSpecifier *
ModeList_Filter_DLowPass(int index)
{
	static NameSpecifier	list[] = {

			{ wxT("NORMAL"),	FILTER_DIST_LOW_PASS_MODE_NORMAL },
			{ wxT("SCALED"),	FILTER_DIST_LOW_PASS_MODE_SCALED },
			{ NULL, 			FILTER_DIST_LOW_PASS_MODE_NULL }
		};
	return(&list[index]);

}

/****************************** FreqDistrList *****************************/

NameSpecifier *
FreqDistrList_Filter_DLowPass(int index)
{
	static NameSpecifier	list[] = {

			{ wxT("STANDARD"),	FILTER_DIST_LOW_PASS_FREQ_DISTR_STANDARD },
			{ wxT("USER"),		FILTER_DIST_LOW_PASS_FREQ_DISTR_USER },
			{ 0, 				FILTER_DIST_LOW_PASS_FREQ_DISTR_NULL }
		};
	return(&list[index]);

}

/****************************** GetNumFreqDistrPars ************************/

/*
 * This function returns the number of parameters for the respective
 * freqDistr parameter array structure. Using it helps maintain the
 * correspondence between the mode names.
 */

int
GetNumFreqDistrPars_Filter_LowPassDist(int mode)
{
	static const WChar	*funcName = wxT("GetNumFreqDistrPars_Filter_LowPassDist");
	switch (mode) {
	case FILTER_DIST_LOW_PASS_FREQ_DISTR_STANDARD:
		return(1);
	case FILTER_DIST_LOW_PASS_FREQ_DISTR_USER:
		return(0);
	default:
		NotifyError(wxT("%s: Mode not listed (%d), returning zero."), funcName,
		  mode);
		return(0);
	}
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
Init_Filter_DLowPass(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_Filter_DLowPass");

	if (parSpec == GLOBAL) {
		if (dLowPassFPtr != NULL)
			Free_Filter_DLowPass();
		if ((dLowPassFPtr = (DLowPassFPtr) malloc(sizeof(DLowPassF))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (dLowPassFPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	dLowPassFPtr->parSpec = parSpec;
	dLowPassFPtr->updateProcessVariablesFlag = TRUE;
	dLowPassFPtr->mode = FILTER_DIST_LOW_PASS_MODE_NORMAL;
	dLowPassFPtr->signalMultiplier = 1.0;
	dLowPassFPtr->numChannels = 0;
	dLowPassFPtr->coefficients = NULL;
	if ((dLowPassFPtr->freqDistr = Init_ParArray(wxT("freq_Distr"),
	  FreqDistrList_Filter_DLowPass(0), GetNumFreqDistrPars_Filter_LowPassDist,
	  NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise freqDistr parArray structure"),
		  funcName);
		Free_Filter_DLowPass();
		return(FALSE);
	}
	SetDefaultFreqDistr_Filter_DLowPass(dLowPassFPtr->freqDistr);

	if (!SetUniParList_Filter_DLowPass()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Filter_DLowPass();
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
Free_Filter_DLowPass(void)
{
	if (dLowPassFPtr == NULL)
		return(TRUE);
	Free_ParArray(&dLowPassFPtr->freqDistr);
	FreeProcessVariables_Filter_DLowPass();
	if (dLowPassFPtr->parList)
		FreeList_UniParMgr(&dLowPassFPtr->parList);
	if (dLowPassFPtr->parSpec == GLOBAL) {
		free(dLowPassFPtr);
		dLowPassFPtr = NULL;
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
SetUniParList_Filter_DLowPass(void)
{
	static const WChar *funcName = wxT("SetUniParList_Filter_DLowPass");
	UniParPtr	pars;

	if ((dLowPassFPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  FILTER_DIST_LOW_PASS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = dLowPassFPtr->parList->pars;
	SetPar_UniParMgr(&pars[FILTER_DIST_LOW_PASS_MODE], wxT("MODE"),
	  wxT("Output mode for model ('normal' or 'scaled')."),
	  UNIPAR_NAME_SPEC,
	  &dLowPassFPtr->mode, ModeList_Filter_DLowPass(0),
	  (void * (*)) SetMode_Filter_DLowPass);
	SetPar_UniParMgr(&pars[FILTER_DIST_LOW_PASS_SIGNALMULTIPLIER], wxT("SCALE"),
	  wxT("Signal multiplier scale ('scaled' mode only)."),
	  UNIPAR_REAL,
	  &dLowPassFPtr->signalMultiplier, NULL,
	  (void * (*)) SetSignalMultiplier_Filter_DLowPass);
	SetPar_UniParMgr(&pars[FILTER_DISTLOWPASS_FREQDISTR], wxT("FREQ_DISTR"),
	  wxT("Filter frequency distribution mode ('standard' or 'user'.)"),
	  UNIPAR_PARARRAY,
	  &dLowPassFPtr->freqDistr, NULL,
	  (void * (*)) SetFreqDistr_Filter_DLowPass);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Filter_DLowPass(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Filter_DLowPass");

	if (dLowPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (dLowPassFPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(dLowPassFPtr->parList);

}

/****************************** SetDefaultFreqDistr ************************/

/*
 * This routine sets the default values for the frequency distribution
 * array parameters.
 */

void
SetDefaultFreqDistr_Filter_DLowPass(ParArrayPtr freqDistr)
{
	SetMode_ParArray(freqDistr, wxT("standard"));
	freqDistr->params[0] = 5000.0;

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Filter_DLowPass(WChar *theMode)
{
	static const WChar	*funcName = wxT("SetMode_Filter_DLowPass");
	int		specifier;

	if (dLowPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, ModeList_Filter_DLowPass(0))) ==
	  FILTER_DIST_LOW_PASS_MODE_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	dLowPassFPtr->mode = specifier;
	dLowPassFPtr->parList->pars[FILTER_DIST_LOW_PASS_SIGNALMULTIPLIER].enabled =
	  (specifier == FILTER_DIST_LOW_PASS_MODE_SCALED);
	return(TRUE);

}

/********************************* SetSignalMultiplier ************************/

/*
 * This function sets the module's signal multiplying factor.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSignalMultiplier_Filter_DLowPass(Float theSignalMultiplier)
{
	static const WChar	 *funcName = wxT("SetSignalMultiplier_Filter_DLowPass");

	if (dLowPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	dLowPassFPtr->signalMultiplier = theSignalMultiplier;
	return(TRUE);

}

/****************************** SetFreqDistr **********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFreqDistr_Filter_DLowPass(ParArrayPtr theFreqDistr)
{
	static const WChar	*funcName = wxT("SetFreqDistr_Filter_DLowPass");

	if (dLowPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theFreqDistr, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."),  funcName);
		return(FALSE);
	}
	dLowPassFPtr->freqDistr = theFreqDistr;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_Filter_DLowPass(void)
{
	DPrint(wxT("Low-Pass Filter Module Parameters:-\n"));
	DPrint(wxT("\tMode = %s,"),
			ModeList_Filter_DLowPass(dLowPassFPtr->mode)->name);
	if (dLowPassFPtr->mode == FILTER_DIST_LOW_PASS_MODE_SCALED)
		DPrint(wxT("\tSignal multiplying factor = %g (?).\n"),
		  dLowPassFPtr->signalMultiplier);
	PrintPars_ParArray(dLowPassFPtr->freqDistr);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Filter_DLowPass(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Filter_DLowPass");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	dLowPassFPtr = (DLowPassFPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Filter_DLowPass(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Filter_DLowPass");

	if (!SetParsPointer_Filter_DLowPass(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Filter_DLowPass(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = dLowPassFPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Filter_DLowPass;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_DLowPass;
	theModule->PrintPars = PrintPars_Filter_DLowPass;
	theModule->RunProcess = RunProcess_Filter_DLowPass;
	theModule->SetParsPointer = SetParsPointer_Filter_DLowPass;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Filter_DLowPass(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Filter_DLowPass");
	int		i, j;
	Float	*statePtr, cutOffFrequency;
	DLowPassFPtr	p = dLowPassFPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_DLowPass();
		if ((p->coefficients = (ContButt1CoeffsPtr *) calloc(_OutSig_EarObject(
		  data)->numChannels, sizeof(ContButt1CoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory."), funcName);
		 	return(FALSE);
		}
		p->numChannels = _OutSig_EarObject(data)->numChannels;
	 	for (i = 0; i < p->numChannels; i++) {
	 		switch (p->freqDistr->mode) {
	 		case FILTER_DIST_LOW_PASS_FREQ_DISTR_STANDARD:
	 			cutOffFrequency = p->freqDistr->params[0];
	 			break;
	 		case FILTER_DIST_LOW_PASS_FREQ_DISTR_USER:
	 			if (p->freqDistr->varNumParams < p->numChannels) {
	 				NotifyError(wxT("%s: The are %d channels, but only %d have settings."),
	 				  funcName, p->numChannels, p->freqDistr->varNumParams);
	 				return(FALSE);
	 			}
	 			cutOffFrequency = p->freqDistr->params[i];
	 			break;
	 		default:
	 			NotifyError(wxT("%s: Mode (%d) not listed, returning zero."), funcName,
	 			  p->freqDistr->mode);
	 			return(FALSE);
	 		}
			if (cutOffFrequency > 0.0) {
				if ((p->coefficients[i] = InitIIR1ContCoeffs_Filters(
				  cutOffFrequency, _InSig_EarObject(data, 0)->dt, LOWPASS)) ==
				  NULL) {
					NotifyError(wxT("%s: Could not allocate filter coefficients."),
					  funcName);
					return(FALSE);
				}
			} else
				p->coefficients[i] = NULL;
	 	}
		p->updateProcessVariablesFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < p->numChannels; i++)
			if (p->coefficients[i]){
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
FreeProcessVariables_Filter_DLowPass(void)
{
	int		i;

	if (dLowPassFPtr->coefficients == NULL)
		return;
	for (i = 0; i < dLowPassFPtr->numChannels; i++)
		if (dLowPassFPtr->coefficients[i])
			free(dLowPassFPtr->coefficients[i]);
	free(dLowPassFPtr->coefficients);
	dLowPassFPtr->coefficients = NULL;
	dLowPassFPtr->updateProcessVariablesFlag = TRUE;

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
RunProcess_Filter_DLowPass(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunProcess_Filter_DLowPass");
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
		if (!InitProcessVariables_Filter_DLowPass(data)) {
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
		switch (dLowPassFPtr->mode) {
		case FILTER_DIST_LOW_PASS_MODE_NORMAL:
			for (i = 0; i < _InSig_EarObject(data, 0)->length; i++)
				*outPtr++ = *inPtr++;
			break;
		case FILTER_DIST_LOW_PASS_MODE_SCALED:
			for (i = 0; i < _InSig_EarObject(data, 0)->length; i++)
				*outPtr++ = *inPtr++ * dLowPassFPtr->signalMultiplier;
			break;
		} /* switch */

	}

	/* Filter signal */

	IIR1Cont_Filters(outSignal, dLowPassFPtr->coefficients);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
