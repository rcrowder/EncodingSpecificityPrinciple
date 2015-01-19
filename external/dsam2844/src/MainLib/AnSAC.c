/**********************
 *
 * File:		AnSAC.c
 * Purpose:		This module calculates a shuffled autocorrelogram from the
 * 				input signal channels: Dries H. G. Louage, Marcel van der
 * 				Heijden, and Philip X. Joris, (2004) "Temporal Properties of
 * 				Responses to Broadband Noise in the Auditory Nerve",
 * 				J. Neurophysiol. 91:2051-2065.
 * Comments:	Written using ModuleProducer version 1.5.0 (Jan 17 2007).
 * 				This module does not run in threaded mode.
 * Author:		L. P. O'Mard
 * Created:		17 Jan 2007
 * Updated:
 * Copyright:	(c) 2007, 2010 Lowel P. O'Mard
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
#include "FiParFile.h"
#include "AnSAC.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SACPtr	sACPtr = NULL;

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
Free_Analysis_SAC(void)
{
	/* static const WChar	*funcName = wxT("Free_Analysis_SAC"); */

	if (sACPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_SAC();
	if (sACPtr->parList)
		FreeList_UniParMgr(&sACPtr->parList);
	if (sACPtr->parSpec == GLOBAL) {
		free(sACPtr);
		sACPtr = NULL;
	}
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
Init_Analysis_SAC(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_SAC");

	if (parSpec == GLOBAL) {
		if (sACPtr != NULL)
			Free_Analysis_SAC();
		if ((sACPtr = (SACPtr) malloc(sizeof(SAC))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (sACPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	sACPtr->parSpec = parSpec;
	sACPtr->updateProcessVariablesFlag = TRUE;
	sACPtr->normalisation = GENERAL_BOOLEAN_ON;
	sACPtr->order = -1;
	sACPtr->eventThreshold = 0.5;
	sACPtr->maxInterval = -1.0;
	sACPtr->binWidth = 0.05e-3;

	if (!SetUniParList_Analysis_SAC()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_SAC();
		return(FALSE);
	}
	sACPtr->lastNormalisationFactor = 1.0;
	sACPtr->spikeListSpec = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_SAC(void)
{
	static const WChar	*funcName = wxT("SetUniParList_Analysis_SAC");
	UniParPtr	pars;

	if ((sACPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_SAC_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = sACPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_SAC_NORMALISATION], wxT("NORMALISATION"),
	  wxT("Normalisation switch ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &sACPtr->normalisation, NULL,
	  (void * (*)) SetNormalisation_Analysis_SAC);
	SetPar_UniParMgr(&pars[ANALYSIS_SAC_ORDER], wxT("ORDER"),
	  wxT("Order of spike interactions (1 = 1st, 2 = 2nd, -1 = all order)."),
	  UNIPAR_INT,
	  &sACPtr->order, NULL,
	  (void * (*)) SetOrder_Analysis_SAC);
	SetPar_UniParMgr(&pars[ANALYSIS_SAC_EVENTTHRESHOLD], wxT("THRESHOLD"),
	  wxT("Event threshold (arbitrary units)."),
	  UNIPAR_REAL,
	  &sACPtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Analysis_SAC);
	SetPar_UniParMgr(&pars[ANALYSIS_SAC_MAXINTERVAL], wxT("MAX_INTERVAL"),
	  wxT("Max. interval for histogram: -ve assumes end of signal (s)."),
	  UNIPAR_REAL,
	  &sACPtr->maxInterval, NULL,
	  (void * (*)) SetMaxInterval_Analysis_SAC);
	SetPar_UniParMgr(&pars[ANALYSIS_SAC_BINWIDTH], wxT("BIN_WIDTH"),
	  wxT("Analysis histogram bin width (s)."),
	  UNIPAR_REAL,
	  &sACPtr->binWidth, NULL,
	  (void * (*)) SetBinWidth_Analysis_SAC);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_SAC(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_SAC");

	if (sACPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (sACPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(sACPtr->parList);

}

/****************************** SetNormalisation ******************************/

/*
 * This function sets the module's normalisation parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNormalisation_Analysis_SAC(WChar * theNormalisation)
{
	static const WChar	*funcName = wxT("SetNormalisation_Analysis_SAC");
	int		specifier;

	if (sACPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}

	if ((specifier = Identify_NameSpecifier(theNormalisation,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  theNormalisation);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sACPtr->updateProcessVariablesFlag = TRUE;
	sACPtr->normalisation = specifier;
	return(TRUE);

}

/****************************** SetOrder **************************************/

/*
 * This function sets the module's order parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOrder_Analysis_SAC(int theOrder)
{
	static const WChar	*funcName = wxT("SetOrder_Analysis_SAC");

	if (sACPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sACPtr->updateProcessVariablesFlag = TRUE;
	sACPtr->order = theOrder;
	return(TRUE);

}

/****************************** SetEventThreshold *****************************/

/*
 * This function sets the module's eventThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEventThreshold_Analysis_SAC(Float theEventThreshold)
{
	static const WChar	*funcName = wxT("SetEventThreshold_Analysis_SAC");

	if (sACPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sACPtr->updateProcessVariablesFlag = TRUE;
	sACPtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetMaxInterval ********************************/

/*
 * This function sets the module's maxInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxInterval_Analysis_SAC(Float theMaxInterval)
{
	static const WChar	*funcName = wxT("SetMaxInterval_Analysis_SAC");

	if (sACPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sACPtr->updateProcessVariablesFlag = TRUE;
	sACPtr->maxInterval = theMaxInterval;
	return(TRUE);

}

/****************************** SetBinWidth ***********************************/

/*
 * This function sets the module's binWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinWidth_Analysis_SAC(Float theBinWidth)
{
	static const WChar	*funcName = wxT("SetBinWidth_Analysis_SAC");

	if (sACPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sACPtr->updateProcessVariablesFlag = TRUE;
	sACPtr->binWidth = theBinWidth;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_SAC(void)
{
	DPrint(wxT("Shuffled Autocorrelogram Analysis Module Parameters:-\n"));
	DPrint(wxT("\tNormalisation switch = %s,"), BooleanList_NSpecLists(sACPtr->
	  normalisation)->name);
	DPrint(wxT("\tOrder = "));
	if (sACPtr->order > 0)
		DPrint(wxT("%d,"), sACPtr->order);
	else
		DPrint(wxT("unlimited,"));
	DPrint(wxT("\tEvent threshold = %g units\n"), sACPtr->eventThreshold);
	DPrint(wxT("\tMax. interval = "));
	if (sACPtr->maxInterval > 0.0)
		DPrint(wxT("%g ms.\n"), MSEC(sACPtr->maxInterval));
	else
		DPrint(wxT("end of signal.\n"));
	DPrint(wxT("\tAnalysis histogram bin width = %g (ms)\n"), MSEC(sACPtr->binWidth));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_SAC(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_SAC");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	sACPtr = (SACPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Analysis_SAC(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_SAC");

	if (!SetParsPointer_Analysis_SAC(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_SAC(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = sACPtr;
	theModule->Free = Free_Analysis_SAC;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_SAC;
	theModule->PrintPars = PrintPars_Analysis_SAC;
	theModule->ResetProcess = ResetProcess_Analysis_SAC;
	theModule->RunProcess = Calc_Analysis_SAC;
	theModule->SetParsPointer = SetParsPointer_Analysis_SAC;
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
CheckData_Analysis_SAC(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_SAC");
	Float	signalDuration;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if (sACPtr->maxInterval > signalDuration) {
		NotifyError(wxT("%s: Maximum interval is longer than signal (value ")
		  wxT("must\nbe <= %g ms)."), funcName, signalDuration);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 * During the reset process the input signal will have the same
 * offset and numChannels as the output signal.
 */

void
ResetProcess_Analysis_SAC(EarObjectPtr data)
{
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	ResetOutSignal_EarObject(data);
	ResetListSpec_SpikeList(sACPtr->spikeListSpec, outSignal->offset,
	  outSignal->numChannels);
	sACPtr->lastNormalisationFactor = 1.0;
}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_Analysis_SAC(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_Analysis_SAC");
	SACPtr	p = sACPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_SAC();
			if ((p->spikeListSpec = InitListSpec_SpikeList(
			  _InSig_EarObject(data, 0)->numChannels)) == NULL) {
				NotifyError(wxT("%s: Out of memory for spikeListSpec."),
				  funcName);
				return(FALSE);
			}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetProcess_Analysis_SAC(data);
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Analysis_SAC(void)
{
	FreeListSpec_SpikeList(&sACPtr->spikeListSpec);
	return(TRUE);

}

/****************************** Calc ******************************************/

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
 * The binScale variable is calculated from the sample lengths to avoid precision
 * errors under 32-bit systems which otherwise produce spike indexes the same length
 * as the output signal length.
 */

BOOLN
Calc_Analysis_SAC(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_SAC");
	register	ChanData	 *outPtr;
	register	Float binScale, normalisationFactor, spikeCount;
	int		chan, sChan;
	Float	maxInterval;
	ChanLen	i, spikeIntervalIndex;
	SpikeSpecPtr	p1, p2;
	SpikeListSpecPtr	sL;
	SignalDataPtr	inSignal, outSignal;
	SACPtr	p = sACPtr;

	inSignal = _InSig_EarObject(data, 0);
	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_SAC(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Shuffled Autocorrelogram Module process"));
		inSignal = _InSig_EarObject(data, 0);
		maxInterval = (p->maxInterval > 0.0)? p->maxInterval: _GetDuration_SignalData(inSignal);
		p->maxIntervalIndex = (ChanLen) floor(maxInterval / inSignal->dt + 0.5);
		if (!InitOutSignal_EarObject(data, 1, (ChanLen) floor(maxInterval / p->binWidth + 0.5),
		  p->binWidth)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		SetStaticTimeFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), 0.0);
		if (!InitProcessVariables_Analysis_SAC(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		p->maxSpikes = (p->order > 0)? p->order: abs((int) inSignal->length);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	GenerateList_SpikeList(p->spikeListSpec, p->eventThreshold, data);
	outSignal = _OutSig_EarObject(data);
	binScale = inSignal->dt / p->binWidth;
	/*binScale = (Float) p->maxIntervalIndex / inSignal->length;*/
	sL = p->spikeListSpec;
	spikeCount = 0;
	if (p->normalisation) {
		outPtr = outSignal->channel[0];
		for (i = 0; i < outSignal->length; i++)
			*outPtr++ /= p->lastNormalisationFactor;
	}

	outPtr = outSignal->channel[0];
	for (chan = 0; chan < inSignal->numChannels; chan++) {
		if (sL->tail[chan])
			spikeCount += sL->tail[chan]->number;
		for (sChan = 0; sChan < inSignal->numChannels; sChan++) {
			if (sChan == chan)
				continue;
			for (p1 = sL->head[chan]; p1 && (p1 != sL->current[chan]->next); p1 = p1->next) {
				for (p2 = sL->head[sChan]; p2 && (p2 != sL->current[sChan]->next) &&
				  (p2->number - p1->number <= p->maxSpikes); p2 = p2->next) {
					if (p2->timeIndex < p1->timeIndex)
						continue;
					if ((spikeIntervalIndex = p2->timeIndex - p1->timeIndex) <
					  p->maxIntervalIndex)
						outPtr[(ChanLen) floor(spikeIntervalIndex * binScale)]++;
				}
			}
		}
	}
	if (p->normalisation) {
		normalisationFactor = binScale * (inSignal->length * inSignal->numChannels) /
		  ((inSignal->numChannels - 1) * SQR(spikeCount));
		outPtr = outSignal->channel[0];
		for (i = 0; i < outSignal->length; i++)
			*outPtr++ *= normalisationFactor;
	}
	SetTimeContinuity_SpikeList(sL, outSignal->offset, outSignal->numChannels,
	  inSignal->length);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

