/**********************
 *
 * File:		AnSpikeReg.c
 * Purpose:		This module carries out a spike regularity analysis,
 *				calculating the mean, standard deviation and coeffient of variation.
 *				measures for a spike train.
 *				The results for each channel are stored in the order:
 *				 - mean, standard deviation, covariance'.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				See Hewitt M. J. & Meddis R. (1993) "Regularity of cochlear
 *				nucleus stellate cells: A computational Modeling study",
 *				J. of the Acoust. Soc. Am, 93, pp 3390-3399.
 *				If the standard deviation results are only valid if the
 *				covariance measure is greater than 0.  This enables the case
 *				when there are less than two counts to be marked.
 *				10-1-97: LPO - added dead-time correction for
 *				covariance CV' = S.D. / (mean - dead time) - see Rothman J. S.
 *				Young E. D. and Manis P. B. "Convergence of Auditory Nerve
 *				Fibers in the Ventral Cochlear Nucleus: Implications of a
 *				Computational Model" J. of NeuroPhysiology, 70:2562-2583.
 *				04-01-05: LPO: The 'countEarObj' EarObject does need to be
 *				registered as a subprocess for the thread processing because
 *				access to the channels is controlled by the main output process
 *				channel access.
 * Author:		L. P. O'Mard
 * Created:		01 Apr 1996
 * Updated:		20 Feb 1997
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
#include "AnSpikeReg.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SpikeRegPtr	spikeRegPtr = NULL;

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
Free_Analysis_SpikeRegularity(void)
{
	if (spikeRegPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_SpikeRegularity();
	if (spikeRegPtr->parList)
		FreeList_UniParMgr(&spikeRegPtr->parList);
	if (spikeRegPtr->parSpec == GLOBAL) {
		free(spikeRegPtr);
		spikeRegPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOutputModeList ****************************/

/*
 * This function initialises the 'outputMode' list array
 */

BOOLN
InitOutputModeList_Analysis_SpikeRegularity(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("COUNT"),			ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_COUNT },
			{ wxT("COEFF_VAR"),		ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_COVARIANCE },
			{ wxT("MEAN"),			ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_MEAN },
			{ wxT("REGULARITY"),	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_REGULARITY },
			{ wxT("STANDARD_DEV"),	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_STANDARD_DEV },
			{ NULL,					ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_NULL },
		};
	spikeRegPtr->outputModeList = modeList;
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
Init_Analysis_SpikeRegularity(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_SpikeRegularity");

	if (parSpec == GLOBAL) {
		if (spikeRegPtr != NULL)
			Free_Analysis_SpikeRegularity();
		if ((spikeRegPtr = (SpikeRegPtr) malloc(sizeof(SpikeReg))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (spikeRegPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	spikeRegPtr->parSpec = parSpec;
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->outputMode = ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_COVARIANCE;
	spikeRegPtr->eventThreshold = 0.0;
	spikeRegPtr->windowWidth = -1.0;
	spikeRegPtr->timeOffset = 0.0;
	spikeRegPtr->timeRange = -10.0;
	spikeRegPtr->deadTime = 0.0;
	spikeRegPtr->countThreshold = SPIKE_REG_MIN_SPIKES_FOR_STATISTICS;

	InitOutputModeList_Analysis_SpikeRegularity();
	if (!SetUniParList_Analysis_SpikeRegularity()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_SpikeRegularity();
		return(FALSE);
	}
	spikeRegPtr->spikeListSpec= NULL;
	spikeRegPtr->countEarObj = NULL;
	spikeRegPtr->spikeTimeHistIndex = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_SpikeRegularity(void)
{
	static const WChar *funcName = wxT(
	  "SetUniParList_Analysis_SpikeRegularity");
	UniParPtr	pars;

	if ((spikeRegPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_SPIKEREGULARITY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = spikeRegPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode: 'Regularity, 'Coeff_Var','Mean' or 'Standard_dev'."),
	  UNIPAR_NAME_SPEC,
	  &spikeRegPtr->outputMode, spikeRegPtr->outputModeList,
	  (void * (*)) SetOutputMode_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_EVENTTHRESHOLD],
	  wxT("THRESHOLD"),
	  wxT("Event threshold (arbitrary units)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_WINDOWWIDTH],
	  wxT("WINDOW_WIDTH"),
	  wxT("Width of analysis window: -ve assumes total analysis range (s)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->windowWidth, NULL,
	  (void * (*)) SetWindowWidth_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Time offset for start of analysis (s)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_SpikeRegularity);
	  SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_TIMERANGE], wxT("RANGE"),
	  wxT("Time range for analysis: -ve assumes to end of signal (s)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->timeRange, NULL,
	  (void * (*)) SetTimeRange_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_DEADTIME], wxT("DEAD_TIME"),
	  wxT("Spike dead time or absolute refractory period (s)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->deadTime, NULL,
	  (void * (*)) SetDeadTime_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_COUNTTHRESHOLD], wxT("COUNT_THRESHOLD"),
	  wxT("Counts above which statistics are calculated (int.)"),
	  UNIPAR_REAL,
	  &spikeRegPtr->countThreshold, NULL,
	  (void * (*)) SetCountThreshold_Analysis_SpikeRegularity);

	SetAltAbbreviation_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_TIMERANGE],
	  wxT("MAX_RANGE"));
	SetAltAbbreviation_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_WINDOWWIDTH],
	  wxT("BIN_WIDTH"));
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_SpikeRegularity(void)
{
	static const WChar	*funcName = wxT(
	  "GetUniParListPtr_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (spikeRegPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(spikeRegPtr->parList);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_Analysis_SpikeRegularity(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT(
	  "SetOutputMode_Analysis_SpikeRegularity");
	int		specifier;

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
		spikeRegPtr->outputModeList)) == ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->outputMode = specifier;
	return(TRUE);

}

/****************************** SetEventThreshold *****************************/

/*
 * This function sets the module's eventThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEventThreshold_Analysis_SpikeRegularity(Float theEventThreshold)
{
	static const WChar	*funcName =
	  wxT("SetEventThreshold_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetWindowWidth ********************************/

/*
 * This function sets the module's windowWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetWindowWidth_Analysis_SpikeRegularity(Float theWindowWidth)
{
	static const WChar	*funcName = wxT(
	  "SetWindowWidth_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->windowWidth = theWindowWidth;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_SpikeRegularity(Float theTimeOffset)
{
	static const WChar	*funcName = wxT(
	  "SetTimeOffset_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError(wxT("%s: Time offset must be greater than zero (%g ms)."),
		  funcName, MSEC(theTimeOffset));
		return(FALSE);
	}
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeRange **********************************/

/*
 * This function sets the module's timeRange parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeRange_Analysis_SpikeRegularity(Float theTimeRange)
{
	static const WChar	*funcName = wxT(
	  "SetTimeRange_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->timeRange = theTimeRange;
	return(TRUE);

}

/****************************** SetDeadTime ***********************************/

/*
 * This function sets the module's deadTime parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeadTime_Analysis_SpikeRegularity(Float theDeadTime)
{
	static const WChar	*funcName = wxT("SetDeadTime_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theDeadTime < 0.0) {
		NotifyError(wxT("%s: Dead time cannot be negative. (%d ms)."), funcName,
		  MILLI(theDeadTime));
		return(FALSE);
	}
	spikeRegPtr->deadTime = theDeadTime;
	return(TRUE);

}

/****************************** SetCountThreshold *****************************/

/*
 * This function sets the module's countThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCountThreshold_Analysis_SpikeRegularity(Float theCountThreshold)
{
	static const WChar	*funcName = wxT(
	  "SetCountThreshold_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theCountThreshold < SPIKE_REG_MIN_SPIKES_FOR_STATISTICS) {
		NotifyError(wxT("%s: Count threshold must be greated than %g (%g)"),
		  funcName, SPIKE_REG_MIN_SPIKES_FOR_STATISTICS, theCountThreshold);
		return(FALSE);
	}
	spikeRegPtr->countThreshold = theCountThreshold;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_SpikeRegularity(void)
{
	DPrint(wxT("Spike Regularity Analysis Module Parameters:-\n"));
	DPrint(wxT("\tOutput mode = %s,"), spikeRegPtr->outputModeList[
	  spikeRegPtr->outputMode].name);
	DPrint(wxT("\tEvent threshold = %g units,\n"),
	  spikeRegPtr->eventThreshold);
	DPrint(wxT("\tBin width = "));
	if (spikeRegPtr->windowWidth <= 0.0)
		DPrint(wxT("<prev. signal dt>,"));
	else
		DPrint(wxT("%g ms,"), MSEC(spikeRegPtr->windowWidth));
	DPrint(wxT("\tTime offset = %g ms,\n"), MSEC(spikeRegPtr->timeOffset));
	DPrint(wxT("\tTime range = "));
	if (spikeRegPtr->timeRange > 0.0)
		DPrint(wxT("%g ms,"), MSEC(spikeRegPtr->timeRange));
	else
		DPrint(wxT("end of signal, "));
	DPrint(wxT("\tDead time = %g ms\n"), MILLI(spikeRegPtr->deadTime));
	DPrint(wxT("\tCount threshold = %g.\n"), spikeRegPtr->countThreshold);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_SpikeRegularity(ModulePtr theModule)
{
	static const WChar	*funcName = wxT(
	  "SetParsPointer_Analysis_SpikeRegularity");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	spikeRegPtr = (SpikeRegPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_SpikeRegularity(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_SpikeRegularity");

	if (!SetParsPointer_Analysis_SpikeRegularity(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_SpikeRegularity(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = spikeRegPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_SpikeRegularity;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_SpikeRegularity;
	theModule->PrintPars = PrintPars_Analysis_SpikeRegularity;
	theModule->ResetProcess = ResetProcess_Analysis_SpikeRegularity;
	theModule->RunProcess = Calc_Analysis_SpikeRegularity;
	theModule->SetParsPointer = SetParsPointer_Analysis_SpikeRegularity;
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
CheckData_Analysis_SpikeRegularity(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_SpikeRegularity");
	Float	signalDuration;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((spikeRegPtr->windowWidth > 0.0) && (spikeRegPtr->windowWidth <
	 _InSig_EarObject(data, 0)->dt)) {
		NotifyError(wxT("%s: Bin width (%g ms) is too small for sampling\n")
		  wxT("interval (%g ms)."), funcName, MSEC(spikeRegPtr->windowWidth),
		  MSEC(_InSig_EarObject(data, 0)->dt));
		return(FALSE);
	}
	signalDuration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if ((spikeRegPtr->timeRange > 0.0) && ((spikeRegPtr->timeRange +
	  spikeRegPtr->timeOffset) > signalDuration)) {
		NotifyError(wxT("%s: Time offset (%g ms) plus range (%g ms) is ")
		  wxT("longer than signal (%g ms)."), funcName, MSEC(spikeRegPtr->
		  timeOffset), MSEC(spikeRegPtr->timeRange), signalDuration);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetStatistics *********************************/

/*
 * This routine resets the statistics to zero values.
 * It expects the process EarObject, "countEarObj" and 'data' to be correctly
 * initialised.
 * The 'data' EarObject channel parameters are used, as these are correctly set
 * when in the threaded modes.
 */

void
ResetStatistics_Analysis_SpikeRegularity(EarObjectPtr data)
{
	register ChanData	*sumPtr, *sumSqrsPtr, *countPtr;
	int		chan;
	ChanLen	i;
	SpikeRegPtr	p = spikeRegPtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);
	SignalDataPtr	countSigDat = _OutSig_EarObject(p->countEarObj);

	if (!data->updateProcessFlag)
		return;
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		countPtr = countSigDat->channel[chan];
		sumPtr = countSigDat->channel[chan + SPIKE_REG_SUM];
		sumSqrsPtr = countSigDat->channel[chan + SPIKE_REG_SUMSQRS ];
		for (i = 0; i < countSigDat->length; i++) {
			*sumPtr++ = 0.0;
			*sumSqrsPtr++ = 0.0;
			*countPtr++ = 0.0;
		}
	}

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 * During the reset process the input signal will have the same
 * offset and numChannels as the output signal.
 */

void
ResetProcess_Analysis_SpikeRegularity(EarObjectPtr data)
{
	int		chan;
	SpikeRegPtr	p = spikeRegPtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	ResetOutSignal_EarObject(p->countEarObj);
	ResetListSpec_SpikeList(p->spikeListSpec, outSignal->offset,
	  outSignal->numChannels);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++)
		p->spikeTimeHistIndex[chan] = 0;

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the exponential table.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 * This routine assumes that calloc sets all of the SpikeSpecPtr
 * pointers to NULL.
 */

BOOLN
InitProcessVariables_Analysis_SpikeRegularity(EarObjectPtr data)
{
	static const WChar *funcName =
	  wxT("InitProcessVariables_Analysis_SpikeRegularity");
	SignalDataPtr	inSignal, outSignal;
	SpikeRegPtr	p = spikeRegPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
	  	inSignal = _InSig_EarObject(data, 0);
	  	outSignal = _OutSig_EarObject(data);
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Analysis_SpikeRegularity();
			if ((p->spikeListSpec = InitListSpec_SpikeList(inSignal->numChannels)) ==
			  NULL) {
				NotifyError(wxT("%s: Out of memory for spikeListSpec."),
				  funcName);
				return(FALSE);
			}
			p->countEarObj = Init_EarObject(wxT("NULL"));
			p->updateProcessVariablesFlag = FALSE;
		}
		if (!InitOutSignal_EarObject(p->countEarObj, inSignal->numChannels *
		  SPIKE_REG_NUM_ACCUMULATORS, outSignal->length, outSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise countEarObj."), funcName);
			return(FALSE);
		}
		if ((p->spikeTimeHistIndex = (ChanLen *) calloc(inSignal->numChannels,
		  sizeof(ChanLen))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'spikeTimeHistIndex")
			  wxT("[%d]' array."), funcName, inSignal->numChannels);
			return(FALSE);
		}
		ResetProcess_Analysis_SpikeRegularity(data);
		ResetStatistics_Analysis_SpikeRegularity(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Analysis_SpikeRegularity(void)
{
	SpikeRegPtr	p = spikeRegPtr;

	if (p->spikeTimeHistIndex) {
		free(p->spikeTimeHistIndex);
		p->spikeTimeHistIndex = NULL;
	}
	FreeListSpec_SpikeList(&p->spikeListSpec);
	Free_EarObject(&p->countEarObj);

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
 * The variable "diff" is tested for negative values, due to high-precision
 * rounding errors.
 */

BOOLN
Calc_Analysis_SpikeRegularity(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_SpikeRegularity");
	register ChanData	*outPtr, *sumPtr, *sumSqrsPtr, *countPtr, diff, mean, std;
	int		chan, countChan;
	Float	interval, spikeTime, windowWidth, timeRange;
	ChanLen	i, *spikeTimeHistIndex, timeRangeIndex;
	ChanLen	timeOffsetIndex, runningTimeOffsetIndex;
	SpikeSpecPtr	s, headSpikeList, currentSpikeSpec;
	SignalDataPtr	outSignal, inSignal, countSigDat;
	SpikeRegPtr	p = spikeRegPtr;

	inSignal = _InSig_EarObject(data, 0);
	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_SpikeRegularity(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Spike Regularity Analysis"));
		p->dt = inSignal->dt;
		timeRange = (p->timeRange > 0.0)? p->timeRange: _GetDuration_SignalData(
		  inSignal) - p->timeOffset;
		windowWidth = (p->windowWidth > 0.0)? p->windowWidth: timeRange;
		timeRangeIndex = (ChanLen) floor(timeRange / windowWidth + 0.5);
		if (!InitOutSignal_EarObject(data, inSignal->numChannels, timeRangeIndex,
		  windowWidth)) {
			NotifyError(wxT("%s: Cannot initialise sumEarObj."), funcName);
			return(FALSE);
		}
		outSignal = _OutSig_EarObject(data);
		SetOutputTimeOffset_SignalData(outSignal, p->timeOffset + outSignal->dt);
		SetInterleaveLevel_SignalData(outSignal, inSignal->interleaveLevel);
		if (!InitProcessVariables_Analysis_SpikeRegularity(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	GenerateList_SpikeList(p->spikeListSpec, p->eventThreshold, data);
	outSignal = _OutSig_EarObject(data);
	countSigDat = _OutSig_EarObject(p->countEarObj);
	/* Add additional sums. */
	timeOffsetIndex = (ChanLen) floor(p->timeOffset / p->dt + 0.5);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		countChan = chan * SPIKE_REG_NUM_ACCUMULATORS;
		countPtr = countSigDat->channel[countChan];
		sumPtr = countSigDat->channel[countChan + SPIKE_REG_SUM];
		sumSqrsPtr = countSigDat->channel[countChan + SPIKE_REG_SUMSQRS];
		spikeTimeHistIndex = p->spikeTimeHistIndex + chan;
		if ((runningTimeOffsetIndex = p->spikeListSpec->timeIndex[chan]) <
		  timeOffsetIndex)
			runningTimeOffsetIndex += timeOffsetIndex - runningTimeOffsetIndex;
		spikeTime = p->spikeListSpec->lastSpikeTimeIndex[chan] * p->dt;
		if ((headSpikeList = p->spikeListSpec->head[chan]) == NULL)
			continue;
		if ((currentSpikeSpec = p->spikeListSpec->current[chan]) == NULL)
			continue;
		for (s = headSpikeList; s && (s != currentSpikeSpec->next); s = s->next) {
			if ((spikeTime > p->timeOffset) && (*spikeTimeHistIndex <
			  outSignal->length)) {
				interval = s->timeIndex * p->dt - spikeTime;
				*(sumPtr + *spikeTimeHistIndex) += interval;
				*(sumSqrsPtr + *spikeTimeHistIndex) += interval * interval;
				*(countPtr + *spikeTimeHistIndex) += 1.0;
			}
			spikeTime = s->timeIndex * p->dt;
			*spikeTimeHistIndex = (ChanLen) floor((spikeTime -
			  runningTimeOffsetIndex * p->dt) / outSignal->dt);
		}
	}
	/* Calculate statics */
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		countChan = chan * SPIKE_REG_NUM_ACCUMULATORS;
		countPtr = countSigDat->channel[countChan];
		sumPtr = countSigDat->channel[countChan + SPIKE_REG_SUM];
		sumSqrsPtr = countSigDat->channel[countChan + SPIKE_REG_SUMSQRS];
		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++, sumPtr++, sumSqrsPtr++, countPtr++,
		  outPtr++) {
			*outPtr = 0.0;
			if (*countPtr > 0.0) {
				mean = *sumPtr / *countPtr;
				if (p->outputMode == ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_MEAN) {
					*outPtr = mean;
					continue;
				} else if (p->outputMode == ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_COUNT) {
					*outPtr = *countPtr;
					continue;
				}
				if (*countPtr > p->countThreshold) {
					diff = *sumSqrsPtr + *countPtr * mean * mean - 2.0 * mean * *sumPtr;
					if (diff < 0.0)
						diff = 0.0;
					std = sqrt(diff / (*countPtr - 1.0));
					if (p->outputMode == ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_STANDARD_DEV) {
						*outPtr = std;
						continue;
					}
					switch (p->outputMode) {
					case ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_COVARIANCE:
						*outPtr = std / (mean - p->deadTime);
						break;
					case ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_REGULARITY:
						*outPtr = 10.0 * log10((mean - p->deadTime) / ((std >
						  DSAM_EPSILON)? std: DSAM_EPSILON));
						break;
					}
				}
			}
		}
	}
	SetTimeContinuity_SpikeList(p->spikeListSpec, outSignal->offset,
	  outSignal->numChannels, inSignal->length);
	SetProcessContinuity_EarObject(p->countEarObj);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

