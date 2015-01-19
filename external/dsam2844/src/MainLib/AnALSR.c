/**********************
 *
 * File:		AnALSR.c
 * Purpose:		This proces module calculates the average localised
 *				synchronised rate (ALSR).
 *				Yound E. D. and Sachs M. B. (1979) "Representation of steady-
 *				state vowels in the temporal aspects of the discharge patterns
 *				of populations of auditory-nerve fibers", J. Acoust. Soc. Am,
 *				Vol 66, pages 1381-1403.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 *				It expects multi-channel input from a period histogram
 * Author:		L. P. O'Mard
 * Created:		22 Jan 2002
 * Updated:
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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
#include "FiParFile.h"
#include "UtString.h"
#include "AnALSR.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ALSRPtr	aLSRPtr = NULL;

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
Free_Analysis_ALSR(void)
{
	if (aLSRPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_ALSR();
	if (aLSRPtr->parList)
		FreeList_UniParMgr(&aLSRPtr->parList);
	if (aLSRPtr->parSpec == GLOBAL) {
		free(aLSRPtr);
		aLSRPtr = NULL;
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
Init_Analysis_ALSR(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_ALSR");

	if (parSpec == GLOBAL) {
		if (aLSRPtr != NULL)
			Free_Analysis_ALSR();
		if ((aLSRPtr = (ALSRPtr) malloc(sizeof(ALSR))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (aLSRPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	aLSRPtr->parSpec = parSpec;
	aLSRPtr->updateProcessVariablesFlag = TRUE;
	aLSRPtr->lowerAveLimit = -0.25;
	aLSRPtr->upperAveLimit = 0.25;
	aLSRPtr->normalise = 1.0;

	if (!SetUniParList_Analysis_ALSR()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_ALSR();
		return(FALSE);
	}
	aLSRPtr->modulusFT = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_ALSR(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_ALSR");
	UniParPtr	pars;

	if ((aLSRPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_ALSR_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = aLSRPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_ALSR_LOWERAVELIMIT], wxT("LOWER_LIMIT"),
	  wxT("Averaging window limit below CF (octaves)"),
	  UNIPAR_REAL,
	  &aLSRPtr->lowerAveLimit, NULL,
	  (void * (*)) SetLowerAveLimit_Analysis_ALSR);
	SetPar_UniParMgr(&pars[ANALYSIS_ALSR_UPPERAVELIMIT], wxT("UPPER_LIMIT"),
	  wxT("Averaging window limit above CF (octaves)"),
	  UNIPAR_REAL,
	  &aLSRPtr->upperAveLimit, NULL,
	  (void * (*)) SetUpperAveLimit_Analysis_ALSR);
	SetPar_UniParMgr(&pars[ANALYSIS_ALSR_NORMALISE], wxT("NORMALISE"),
	  wxT("Normalisation factor (units.)"),
	  UNIPAR_REAL,
	  &aLSRPtr->normalise, NULL,
	  (void * (*)) SetNormalise_Analysis_ALSR);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_ALSR(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_ALSR");

	if (aLSRPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (aLSRPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(aLSRPtr->parList);

}

/****************************** SetLowerAveLimit ******************************/

/*
 * This function sets the module's lowerAveLimit parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowerAveLimit_Analysis_ALSR(Float theLowerAveLimit)
{
	static const WChar	*funcName = wxT("SetLowerAveLimit_Analysis_ALSR");

	if (aLSRPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	aLSRPtr->updateProcessVariablesFlag = TRUE;
	aLSRPtr->lowerAveLimit = theLowerAveLimit;
	return(TRUE);

}

/****************************** SetUpperAveLimit ******************************/

/*
 * This function sets the module's upperAveLimit parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUpperAveLimit_Analysis_ALSR(Float theUpperAveLimit)
{
	static const WChar	*funcName = wxT("SetUpperAveLimit_Analysis_ALSR");

	if (aLSRPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	aLSRPtr->updateProcessVariablesFlag = TRUE;
	aLSRPtr->upperAveLimit = theUpperAveLimit;
	return(TRUE);

}

/****************************** SetNormalise **********************************/

/*
 * This function sets the module's normalise parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNormalise_Analysis_ALSR(Float theNormalise)
{
	static const WChar	*funcName = wxT("SetNormalise_Analysis_ALSR");

	if (aLSRPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	aLSRPtr->updateProcessVariablesFlag = TRUE;
	aLSRPtr->normalise = theNormalise;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_ALSR(void)
{
	DPrint(wxT("ALSR Analysis Module Parameters:-\n"));
	DPrint(wxT("\tAveraging window limit below CF = %g,\n"), aLSRPtr->
	  lowerAveLimit);
	DPrint(wxT("\tAveraging window limit above CF = %g,\n"), aLSRPtr->
	  upperAveLimit);
	DPrint(wxT("\tNormalisation factor = %g(units).\n"), aLSRPtr->normalise);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_ALSR(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_ALSR");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	aLSRPtr = (ALSRPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Analysis_ALSR(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_ALSR");

	if (!SetParsPointer_Analysis_ALSR(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_ALSR(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = aLSRPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_ALSR;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_ALSR;
	theModule->PrintPars = PrintPars_Analysis_ALSR;
	theModule->ResetProcess = ResetProcess_Analysis_ALSR;
	theModule->RunProcess = Calc_Analysis_ALSR;
	theModule->SetParsPointer = SetParsPointer_Analysis_ALSR;
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
CheckData_Analysis_ALSR(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_ALSR");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (_InSig_EarObject(data, 0)->numChannels < 2) {
		NotifyError(wxT("%s: This module expects multi-channel input."),
		  funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Analysis_ALSR(EarObjectPtr data)
{
	data->timeIndex = data->timeIndex + 0;	/* So the compiler doesn't complain. */
	ResetProcess_EarObject(aLSRPtr->modulusFT);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_Analysis_ALSR(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_Analysis_ALSR");
	ALSRPtr	p = aLSRPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		/*** Additional update flags can be added to above line ***/
		FreeProcessVariables_Analysis_ALSR();
		if ((p->modulusFT = Init_EarObject(wxT("Ana_FourierT"))) == NULL) {
			NotifyError(wxT("%s: Could not initialise the 'modulusFT' ")
			  wxT("EarObject."), funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (!SetPar_ModuleMgr(p->modulusFT, wxT("output_mode"), wxT("modulus"))) {
		NotifyError(wxT("%s: Could not set the 'Ana_FourierT' output mode."),
		  funcName);
		return(FALSE);
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetProcess_Analysis_ALSR(data);
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
FreeProcessVariables_Analysis_ALSR(void)
{
	Free_EarObject(&aLSRPtr->modulusFT);
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
 * The output is normalised by the first (the second sample) FFT component.
 */

BOOLN
Calc_Analysis_ALSR(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_ALSR");
	register ChanData	 *outPtr;
	WChar	channelTitle[MAXLINE];
	int		chan, minChan, maxChan, minWinChan, maxWinChan, numChannels;
	Float	dF, *cFs;
	ChanLen	i, minIndex, maxIndex;
	SignalDataPtr	inSignal, outSignal;
	ALSRPtr	p = aLSRPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_ALSR(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("ALSR Analysis Module process"));

		/*** Example Initialise output signal - ammend/change if required. ***/
		if (!InitOutSignal_EarObject(data, 1, _InSig_EarObject(data, 0)->length,
		  _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_Analysis_ALSR(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoSampleTitle_SignalData(_OutSig_EarObject(data), wxT("Frequency ")
		  wxT("(Hz) "));
		Snprintf_Utility_String(channelTitle, MAXLINE, wxT("ALSR function ")
		  wxT("(+%g / -%g octaves)"), p->lowerAveLimit, p->upperAveLimit);
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data), channelTitle);
		SetInfoChannelLabels_SignalData(_OutSig_EarObject(data), NULL);
		SetInfoCFArray_SignalData(_OutSig_EarObject(data), NULL);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	if (!GetChannelLimits_SignalData(inSignal, &minChan, &maxChan, p->
	  lowerAveLimit, p->upperAveLimit, SIGNALDATA_LIMIT_MODE_OCTAVE)) {
		NotifyError(wxT("%s: Could not find a channel limits for signal."),
		  funcName);
		return(FALSE);
	}
	TempInputConnection_EarObject(data, p->modulusFT, 1);
	if (!RunProcess_ModuleMgr(p->modulusFT)) {
		NotifyError(wxT("%s: Could not run 'Ana_FourierT' process."), funcName);
		return(FALSE);
	}
	dF = p->modulusFT->outSignal->dt;
	cFs = inSignal->info.cFArray;
	SetSamplingInterval_SignalData(outSignal, dF);
	minIndex = (ChanLen) floor(cFs[minChan] / dF + 0.5);
	maxIndex = (ChanLen) floor(cFs[maxChan] / dF + 0.5);
	outPtr = outSignal->channel[0];
	for (i = 0; i < minIndex - 1; i++)
		*outPtr++ = 0.0;
	outPtr = outSignal->channel[0] + minIndex;
	for (i = minIndex; i <= maxIndex; i++, outPtr++) {
		GetWindowLimits_SignalData(inSignal, &minWinChan, &maxWinChan,
		  i * dF, p->lowerAveLimit, p->upperAveLimit,
		  SIGNALDATA_LIMIT_MODE_OCTAVE);
		for (chan = minWinChan, *outPtr = 0.0; chan <= maxWinChan; chan++)
			*outPtr += p->modulusFT->outSignal->channel[chan][i];
	}
	for (i = maxIndex + 1; i < outSignal->length; i++)
		*outPtr++ = 0.0;
	outPtr = outSignal->channel[0] + minIndex;
	numChannels = maxIndex - minIndex + 1;
	for (i = minIndex; i <= maxIndex; i++)
		*outPtr++ /= numChannels;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
