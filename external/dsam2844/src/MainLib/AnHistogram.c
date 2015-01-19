/**********************
 *
 * File:		AnHistogram.c
 * Purpose:		This program calculates period or post-stimulus time histograms.
 * Comments:	Written using ModuleProducer version 1.3.
 *				A negative binwidth will instruct instruct the process to use
 *				the previous signal's dt value.
 *				As for most of the analysis routines, this routine is not
 *				segmented mode friendly.
 *				13-02-98 LPO: Corrected time offset checking.
 *				04-03-99 LPO: Corrected the double precsision error using SGI's
 *				by introducing the 'DBL_GREATER macro'.
 * Author:		L. P. O'Mard
 * Created:		17 Nov 1995
 * Updated:		04 Mar 1999
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
#include "UtString.h"
#include "AnHistogram.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

HistogramPtr	histogramPtr = NULL;

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
Free_Analysis_Histogram(void)
{

	if (histogramPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_Histogram();
	if (histogramPtr->parList)
		FreeList_UniParMgr(&histogramPtr->parList);
	if (histogramPtr->parSpec == GLOBAL) {
		free(histogramPtr);
		histogramPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitDetectionModeList *************************/

/*
 * This routine intialises the Dectection Mode list array.
 */

BOOLN
InitDetectionModeList_Analysis_Histogram(void)
{
	static NameSpecifier	detectionModeList[] = {

					{wxT("SPIKE"),		HISTOGRAM_DETECT_SPIKES },
					{wxT("CONTINUOUS"),	HISTOGRAM_CONTINUOUS },
					{NULL,				HISTOGRAM_DETECT_NULL }

				};


	histogramPtr->detectionModeList = detectionModeList;
	return(TRUE);

}

/****************************** InitOutputModeList ****************************/

/*
 * This routine intialises the output Mode list array.
 */

BOOLN
InitOutputModeList_Analysis_Histogram(void)
{
	static NameSpecifier	outputModeList[] = {

					{wxT("BIN_COUNTS"),	HISTOGRAM_OUTPUT_BIN_COUNTS },
					{wxT("SPIKE_RATE"),	HISTOGRAM_OUTPUT_SPIKE_RATE },
					{NULL,				HISTOGRAM_OUTPUT_NULL }

				};
	histogramPtr->outputModeList = outputModeList;
	return(TRUE);

}

/****************************** InitTypeModeList ******************************/

/*
 * This routine intialises the Type Mode list array.
 */

BOOLN
InitTypeModeList_Analysis_Histogram(void)
{
	static NameSpecifier	typeModeList[] = {

					{wxT("PSTH"),	HISTOGRAM_PSTH },
					{wxT("PH"),		HISTOGRAM_PH },
					{NULL,			HISTOGRAM_TYPE_NULL }

				};
	histogramPtr->typeModeList = typeModeList;
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
Init_Analysis_Histogram(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_Histogram");

	if (parSpec == GLOBAL) {
		if (histogramPtr != NULL)
			Free_Analysis_Histogram();
		if ((histogramPtr = (HistogramPtr) malloc(sizeof(Histogram))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (histogramPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	histogramPtr->parSpec = parSpec;
	histogramPtr->updateProcessVariablesFlag = TRUE;
	histogramPtr->detectionMode = HISTOGRAM_DETECT_SPIKES;
	histogramPtr->outputMode = HISTOGRAM_OUTPUT_BIN_COUNTS;
	histogramPtr->typeMode = HISTOGRAM_PSTH;
	histogramPtr->eventThreshold = 0.0;
	histogramPtr->binWidth = -1.0;
	histogramPtr->period = -1.0;
	histogramPtr->timeOffset = 0.0;

	InitDetectionModeList_Analysis_Histogram();
	InitOutputModeList_Analysis_Histogram();
	InitTypeModeList_Analysis_Histogram();
	if (!SetUniParList_Analysis_Histogram()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_Histogram();
		return(FALSE);
	}
	histogramPtr->numInitialisedThreads = 0;
	histogramPtr->riseDetected = NULL;
	histogramPtr->dataBuffer = NULL;
	histogramPtr->hState = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_Histogram(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_Histogram");
	UniParPtr	pars;

	if ((histogramPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_HISTOGRAM_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = histogramPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_DETECTIONMODE], wxT(
	  "DETECT_MODE"),
	  wxT("Detection mode: 'spike' (detection) or 'continuous' (signal)."),
	  UNIPAR_NAME_SPEC,
	  &histogramPtr->detectionMode, histogramPtr->detectionModeList,
	  (void * (*)) SetDetectionMode_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_OUTPUT_MODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode: 'bin_counts'  or 'spike_rate'."),
	  UNIPAR_NAME_SPEC,
	  &histogramPtr->outputMode, histogramPtr->outputModeList,
	  (void * (*)) SetOutputMode_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_TYPEMODE], wxT("TYPE_MODE"),
	  wxT("Histogram type: 'PSTH', or 'PH'."),
	  UNIPAR_NAME_SPEC,
	  &histogramPtr->typeMode, histogramPtr->typeModeList,
	  (void * (*)) SetTypeMode_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_EVENTTHRESHOLD], wxT("THRESHOLD"),
	  wxT("Event threshold for bin counts (units as appropriate)."),
	  UNIPAR_REAL,
	  &histogramPtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_BINWIDTH], wxT("BIN_WIDTH"),
	  wxT("Histogram Bin width (-ve: previous signal dt, zero: one bin = input ")
	    wxT(" signal length) (s)."),
	  UNIPAR_REAL,
	  &histogramPtr->binWidth, NULL,
	  (void * (*)) SetBinWidth_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_PERIOD], wxT("PERIOD"),
	  wxT("Period for period histogram: used in 'PH' mode only (s)."),
	  UNIPAR_REAL,
	  &histogramPtr->period, NULL,
	  (void * (*)) SetPeriod_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Time offset for histogram (s)."),
	  UNIPAR_REAL,
	  &histogramPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_Histogram);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_Histogram(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_Histogram");

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (histogramPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(histogramPtr->parList);

}

/****************************** SetDetectionMode ******************************/

/*
 * This function sets the module's detectionMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDetectionMode_Analysis_Histogram(WChar *theDetectionMode)
{
	static const WChar	*funcName = wxT("SetDetectionMode_Analysis_Histogram");
	int specifier;

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theDetectionMode,
	  histogramPtr->detectionModeList)) == HISTOGRAM_DETECT_NULL) {
		NotifyError(wxT("%s: Illegal detection mode  (%s)."), funcName,
		  theDetectionMode);
		return(FALSE);
	}
	histogramPtr->detectionMode = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's output mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_Analysis_Histogram(WChar *theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_Analysis_Histogram");
	int		specifier;

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
	  histogramPtr->outputModeList)) == HISTOGRAM_OUTPUT_NULL) {
		NotifyError(wxT("%s: Illegal output mode (%s)."), funcName,
		  theOutputMode);
		return(FALSE);
	}
	histogramPtr->outputMode = specifier;
	return(TRUE);

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the module's typeMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTypeMode_Analysis_Histogram(WChar *theTypeMode)
{
	static const WChar	*funcName = wxT("SetTypeMode_Analysis_Histogram");
	int		specifier;

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTypeMode,
	  histogramPtr->typeModeList)) == HISTOGRAM_TYPE_NULL) {
		NotifyError(wxT("%s: Illegal type mode (%s)."), funcName,
		  theTypeMode);
		return(FALSE);
	}
	histogramPtr->updateProcessVariablesFlag = TRUE;
	histogramPtr->typeMode = specifier;
	return(TRUE);

}

/****************************** SetEventThreshold *****************************/

/*
 * This function sets the module's eventThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEventThreshold_Analysis_Histogram(Float theEventThreshold)
{
	static const WChar	*funcName = wxT("SetEventThreshold_Analysis_Histogram");

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	histogramPtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetBinWidth ***********************************/

/*
 * This function sets the module's binWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinWidth_Analysis_Histogram(Float theBinWidth)
{
	static const WChar	*funcName = wxT("SetBinWidth_Analysis_Histogram");

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	histogramPtr->binWidth = theBinWidth;
	return(TRUE);

}

/****************************** SetPeriod *************************************/

/*
 * This function sets the module's period parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPeriod_Analysis_Histogram(Float thePeriod)
{
	static const WChar	*funcName = wxT("SetPeriod_Analysis_Histogram");

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	histogramPtr->updateProcessVariablesFlag = TRUE;
	histogramPtr->period = thePeriod;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_Histogram(Float theTimeOffset)
{
	static const WChar	*funcName = wxT("SetTimeOffset_Analysis_Histogram");

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	histogramPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_Histogram(void)
{
	static const WChar	*funcName = wxT("PrintPars_Analysis_Histogram");

	if (histogramPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Histogram analysis module parameters:-\n"));
	DPrint(wxT("\tDetection mode = %s,"), histogramPtr->detectionModeList[
	  histogramPtr->detectionMode].name);
	DPrint(wxT("\tOutput mode = %s,"), histogramPtr->outputModeList[
	  histogramPtr->outputMode].name);
	DPrint(wxT("\tType mode = %s,\n"), histogramPtr->typeModeList[histogramPtr->
	  typeMode].name);
	if (histogramPtr->detectionMode == HISTOGRAM_DETECT_SPIKES)
		DPrint(wxT("\tEvent threshold = %g units,\n"), histogramPtr->
		  eventThreshold);
	DPrint(wxT("\tBin width = %g ms"), MSEC(histogramPtr->binWidth));
	if (histogramPtr->binWidth < 0.0)
		DPrint(wxT(" (prev. signal dt)"));
	else if (histogramPtr->binWidth == 0.0)
		DPrint(wxT("(1 bin, prev. signal length)"));
	DPrint(wxT(","));
	switch (histogramPtr->typeMode) {
	case HISTOGRAM_PSTH:
	case HISTOGRAM_PH:
		DPrint(wxT("\tPeriod = "));
		if (histogramPtr->period > 0.0)
			DPrint(wxT("%g ms\n"), MSEC(histogramPtr->period));
		else
			DPrint(wxT("<Signal duration>\n"));
		break;
	default:
		DPrint(wxT("\tTime offset = %g ms\n"), MSEC(histogramPtr->timeOffset));
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_Histogram(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_Histogram");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	histogramPtr = (HistogramPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_Histogram(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_Histogram");

	if (!SetParsPointer_Analysis_Histogram(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_Histogram(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = histogramPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_Histogram;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_Histogram;
	theModule->PrintPars = PrintPars_Analysis_Histogram;
	theModule->ResetProcess = ResetProcess_Analysis_Histogram;
	theModule->RunProcess = Calc_Analysis_Histogram;
	theModule->SetParsPointer = SetParsPointer_Analysis_Histogram;
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
CheckData_Analysis_Histogram(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_Histogram");
	SignalDataPtr	inSignal;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	inSignal = _InSig_EarObject(data, 0);
	if ((histogramPtr->binWidth > 0.0) && (histogramPtr->binWidth < inSignal->
	  dt)) {
		NotifyError(wxT("%s: Bin width is too small for sampling interval ")
		  wxT("(%g ms)."), funcName, MSEC(histogramPtr->binWidth));
		return(FALSE);
	}
	if (histogramPtr->typeMode == HISTOGRAM_PH) {
		if ((histogramPtr->period > 0.0) && (histogramPtr->period < inSignal->
		  dt)) {
			NotifyError(wxT("%s: The period (%g ms) is less than the input ")
			  wxT("signal sampling interval (%g ms)!"), funcName, MILLI(
			  histogramPtr->period), MILLI(inSignal->dt));
			return(FALSE);
		}
	}
	if (histogramPtr->typeMode == HISTOGRAM_PH) {
		if ((histogramPtr->period > 0.0) && (histogramPtr->binWidth >
		  histogramPtr->period)) {
			NotifyError(wxT("%s: The bin width is too small for the period ")
			  wxT("value (%g ms & %g ms respectively)."), funcName,
			  MSEC(histogramPtr->binWidth), MSEC(histogramPtr->period));
			return(FALSE);
		}
	}
	if ((histogramPtr->detectionMode == HISTOGRAM_CONTINUOUS) && (histogramPtr->
	  outputMode == HISTOGRAM_OUTPUT_SPIKE_RATE)) {
		NotifyError(wxT("%s: The spike rate histogram modes cannot be used in ")
		  wxT("continuous detection mode."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** ResetProcessThread ******************************/

/*
 * This routine resets the process thread-related variables.
 * The 'numPeriods' values should only be reset when the module's
 * 'updateProcessVariablesFlag' or the process' 'updateProcessFlag' is set.
 */

void
ResetProcessThread_Analysis_Histogram(EarObjectPtr data, int i)
{
	HistogramPtr	p = histogramPtr;

	p->hState[i].bufferSamples = 0;
	p->hState[i].offsetIndex = (ChanLen) floor(p->timeOffset /
	  _InSig_EarObject(data, 0)->dt + 0.5);
	p->hState[i].extraSample = (p->hState[i].offsetIndex)? 0: 1;

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 * The 'numInitialisedThreads' variable ensures that when the pre-thread
 * initialisation takes place, no more than the number of initialised
 * threads are reset.
 */

void
ResetProcess_Analysis_Histogram(EarObjectPtr data)
{
	int		i;
	HistogramPtr	p = histogramPtr;
	HistogramStatePtr	hState0 = &p->hState[0];

	ResetOutSignal_EarObject(data);
	ResetOutSignal_EarObject(p->dataBuffer);
	if (data->threadRunFlag)
		ResetProcessThread_Analysis_Histogram(data, data->threadIndex);
	else  {
		for (i = 0; i < p->numInitialisedThreads; i++) {
			ResetProcessThread_Analysis_Histogram(data, i);
		}
	}
	if (hState0->offsetIndex < _InSig_EarObject(data, 0)->length)
		for (i = _OutSig_EarObject(data)->offset; i < _OutSig_EarObject(data)->
		  numChannels; i++)
			p->dataBuffer->outSignal->channel[i][0] = _InSig_EarObject(data,
			  0)->channel[i][hState0->offsetIndex + hState0->extraSample - 1];
	for (i = _OutSig_EarObject(data)->offset; i < _OutSig_EarObject(data)->
	  numChannels; i++)
		p->riseDetected[i] = FALSE;

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables if necessary
 * at sets the initial process values as required.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 * When setting the dataBuffer channels, the "offsetIndex" and "extraSample"
 * array elements all have the same values respectively.
 */

BOOLN
InitProcessVariables_Analysis_Histogram(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_Analysis_Histogram");
	ChanLen	bufferLength;
	HistogramPtr	p = histogramPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Analysis_Histogram();
			if ((p->dataBuffer = Init_EarObject(wxT("NULL"))) == NULL) {
				NotifyError(wxT("%s: Could not initialise data buffer ")
				  wxT("EarObject"), funcName);
				return(FALSE);
			}
			if (!InitSubProcessList_EarObject(data,
			  ANALYSIS_HISTOGRAM_NUM_SUB_PROCESSES)) {
				NotifyError(wxT("%s: Could not initialise %d sub-process list ")
				  wxT("for process."), funcName,
				  ANALYSIS_HISTOGRAM_NUM_SUB_PROCESSES);
				return(FALSE);
			}
			data->subProcessList[ANALYSIS_HISTOGRAM_DATABUFFER] = p->dataBuffer;
			if ((p->riseDetected = (BOOLN *) calloc(
			  _OutSig_EarObject(data)->numChannels, sizeof(BOOLN))) == NULL) {
				NotifyError(wxT("%s: Out of memory for 'riseDetected' array."),
				  funcName);
				return(FALSE);
			}
			if ((p->hState = (HistogramState *) calloc(data->numThreads, sizeof(
			  HistogramState))) == NULL) {
				NotifyError(wxT("%s: Out of memory for hState array."),
				  funcName);
				return(FALSE);
			}
			p->numInitialisedThreads = data->numThreads;
			bufferLength = (p->typeMode == HISTOGRAM_PSTH)? 1: (p->period >
			  0.0)? (ChanLen) floor(p->period / _InSig_EarObject(data, 0)->dt +
			  0.5):
			  _InSig_EarObject(data, 0)->length;
			if (!InitOutSignal_EarObject(p->dataBuffer, _OutSig_EarObject(
			  data)->numChannels, bufferLength, _InSig_EarObject(data, 0)->
			  dt)) {
				NotifyError(wxT("%s: Cannot initialise channels for PH ")
				  wxT("Buffer."), funcName);
				return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		ResetProcess_Analysis_Histogram(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Analysis_Histogram(void)
{
	if (histogramPtr->riseDetected) {
		free(histogramPtr->riseDetected);
		histogramPtr->riseDetected = NULL;
	}
	if (histogramPtr->hState) {
		free(histogramPtr->hState);
		histogramPtr->hState = NULL;
	}
	if (histogramPtr->dataBuffer)
		Free_EarObject(&histogramPtr->dataBuffer);
	histogramPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** PushDataBuffer **********************************/

/*
 * This routine pushes sections from the end of the input signal into the data
 * buffer.
 */

void
PushDataBuffer_Analysis_Histogram(EarObjectPtr data, ChanLen lastSamples)
{
	register	ChanData	*inPtr, *buffPtr;
	int		chan;
	ChanLen	i;

	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(
	  data)->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan] + _InSig_EarObject(
		  data, 0)->length - lastSamples;
		buffPtr = data->subProcessList[ANALYSIS_HISTOGRAM_DATABUFFER]->
		  outSignal->channel[chan] + histogramPtr->hState[
		  data->threadIndex].bufferSamples;
		for (i = 0; i < lastSamples; i++)
			*buffPtr++ = *inPtr++;
	}
	histogramPtr->hState[data->threadIndex].bufferSamples = lastSamples;

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
 * The 'extraSample' and 'extraTimeInterval' are only required when the time
 * offset is zero.
 */

BOOLN
Calc_Analysis_Histogram(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_Histogram");
	register	ChanData	*inPtr, *outPtr, *buffPtr, binSum;
	int		chan;
	Float	nextCutOff, nextBinCutOff, time, totalBinDuration;
	Float	extraTimeInterval;
	ChanLen	i, bufferSamples, processLength, availableLength;
	SignalDataPtr	inSignal, outSignal;
	HistogramPtr	p = histogramPtr;
	HistogramStatePtr	hState;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_Histogram(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Histogram Analysis Module"));
		p->dt = _InSig_EarObject(data, 0)->dt;
		p->wPeriod = ((p->typeMode == HISTOGRAM_PSTH) || (p->period <=
		  0.0))? _InSig_EarObject(data, 0)->length * p->dt: p->period;
		p->wBinWidth = (p->binWidth <= 0.0)? p->dt: p->binWidth;
		if (p->binWidth < 0.0)
			p->wBinWidth = p->dt;
		else if (p->binWidth == 0.0)
			p->wBinWidth = _GetDuration_SignalData(_InSig_EarObject(data, 0));
		else
			p->wBinWidth = p->binWidth;
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, (ChanLen) floor(p->wPeriod / p->wBinWidth + 0.5), p->
		  wBinWidth)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), p->wBinWidth);
		SetStaticTimeFlag_SignalData(_OutSig_EarObject(data), TRUE);
		if (!InitProcessVariables_Analysis_Histogram(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	hState = &p->hState[data->threadIndex];
	if (hState->offsetIndex > inSignal->length) {
		processLength = 0;
		availableLength = 0; /* ?? is this right?*/
	} else {
		availableLength = inSignal->length - hState->offsetIndex -
		  hState->extraSample + hState->bufferSamples + 1;
		processLength = (p->typeMode == HISTOGRAM_PSTH)? availableLength:
		  (ChanLen) floor(floor(availableLength * p->dt / p->wPeriod) *
		  p->wPeriod / p->dt + DSAM_EPSILON);
	}
	if (processLength) {
		if ((p->detectionMode == HISTOGRAM_DETECT_SPIKES) && (p->outputMode ==
		  HISTOGRAM_OUTPUT_SPIKE_RATE) && hState->numPeriods) {
			totalBinDuration = p->wBinWidth * hState->numPeriods;
			for (chan = outSignal->offset; chan < inSignal->numChannels;
			   chan++) {
				outPtr = outSignal->channel[chan];
				for (i = 0; i < outSignal->length; i++)
					*outPtr++ *= totalBinDuration;
			}
		}
		for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
			inPtr = inSignal->channel[chan] + hState->offsetIndex + hState->extraSample;
			outPtr = outSignal->channel[chan];
			buffPtr = data->subProcessList[ANALYSIS_HISTOGRAM_DATABUFFER]->
			  outSignal->channel[chan];
			bufferSamples = hState->bufferSamples;
			extraTimeInterval = p->dt * hState->extraSample;
			nextCutOff = p->wPeriod - extraTimeInterval;
			nextBinCutOff = p->wBinWidth - extraTimeInterval;
			for (i = 1, binSum = 0; i < processLength; i++) {
				switch (p->detectionMode) {
				case HISTOGRAM_DETECT_SPIKES:
					if (!p->riseDetected[chan])
						p->riseDetected[chan] = (*inPtr > *buffPtr);
					else {
						if (*inPtr < *buffPtr) {
							p->riseDetected[chan] = FALSE;
							if (*buffPtr > p->eventThreshold)
								binSum++;
						}
					}
					break;
				case HISTOGRAM_CONTINUOUS:
					binSum += *buffPtr;
					break;
				} /* switch */
				if (!bufferSamples)
					*buffPtr = *(inPtr++);
				else {
					buffPtr++;
					bufferSamples--;
				}
				time = i * p->dt;
				if (DBL_GREATER(time, nextBinCutOff)) {
					if ((ChanLen) (outPtr - outSignal->channel[chan]) <
					  outSignal->length) /* - because of rounding errors*/
						*outPtr++ += binSum;
					binSum = 0;
					nextBinCutOff += p->wBinWidth;
				}
				if (DBL_GREATER(time, nextCutOff)) {
					outPtr = outSignal->channel[chan];
					nextCutOff += p->wPeriod;
					hState->numPeriods++;
				}
			}
			if ((ChanLen) (outPtr - outSignal->channel[chan]) < outSignal->
			  length)	/* Last incomplete bin */
				*outPtr +=  binSum;
		}
		if (p->typeMode == HISTOGRAM_PSTH)
			hState->numPeriods++;
		if ((p->detectionMode == HISTOGRAM_DETECT_SPIKES) && (p->outputMode ==
		  HISTOGRAM_OUTPUT_SPIKE_RATE)) {
			totalBinDuration = p->wBinWidth * hState->numPeriods;
			for (chan = outSignal->offset; chan < inSignal->numChannels;
			  chan++) {
				outPtr = outSignal->channel[chan];
				for (i = 0; i < outSignal->length; i++)
					*outPtr++ /= totalBinDuration;
			}
		}
	}
	if (hState->offsetIndex + hState->extraSample) {
		if (hState->offsetIndex < inSignal->length) {
			if (processLength < availableLength)
				PushDataBuffer_Analysis_Histogram(data, availableLength -
				  processLength);
			hState->offsetIndex = 0;
		} else {
			hState->offsetIndex -= inSignal->length;
			PushDataBuffer_Analysis_Histogram(data, 1);
		}
		hState->extraSample = 0;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
