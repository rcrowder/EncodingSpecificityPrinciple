/**********************
 *
 * File:		UtSample.c
 * Purpose:		This module samples a signal at selected intervals.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 18 1996).
 * Author:		L. P. O'Mard
 * Created:		22 Oct 1996
 * Updated:
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
#include "UtSample.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SamplePtr	samplePtr = NULL;

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
Free_Utility_Sample(void)
{
	if (samplePtr == NULL)
		return(FALSE);
	if (samplePtr->parList)
		FreeList_UniParMgr(&samplePtr->parList);
	if (samplePtr->parSpec == GLOBAL) {
		free(samplePtr);
		samplePtr = NULL;
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
Init_Utility_Sample(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_Sample");

	if (parSpec == GLOBAL) {
		if (samplePtr != NULL)
			Free_Utility_Sample();
		if ((samplePtr = (SamplePtr) malloc(sizeof(Sample))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (samplePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	samplePtr->parSpec = parSpec;
	samplePtr->timeOffset = 0.0;
	samplePtr->dt = -1.0;

	if (!SetUniParList_Utility_Sample()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_Sample();
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
SetUniParList_Utility_Sample(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_Sample");
	UniParPtr	pars;

	if ((samplePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_SAMPLE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = samplePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_SAMPLE_TIMEOFFSET], wxT("TIMEOFFSET"),
	  wxT("Time offset (s)."),
	  UNIPAR_REAL,
	  &samplePtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Utility_Sample);
	SetPar_UniParMgr(&pars[UTILITY_SAMPLE_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s) (-ve assumes prev. signal dt)."),
	  UNIPAR_REAL,
	  &samplePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_Utility_Sample);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_Sample(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_Sample");

	if (samplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (samplePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(samplePtr->parList);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Utility_Sample(Float theTimeOffset)
{
	static const WChar	*funcName = wxT("SetTimeOffset_Utility_Sample");

	if (samplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	samplePtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_Utility_Sample(Float theSamplingInterval)
{
	static const WChar	*funcName = wxT("SetSamplingInterval_Utility_Sample");

	if (samplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	samplePtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_Sample(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_Sample");

	if (samplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Sample Utility Module Parameters:-\n"));
	DPrint(wxT("\tTime offset = %g ms\n"), MILLI(samplePtr->timeOffset));
	DPrint(wxT("\tSampling interval = "));
	if (samplePtr->dt > 0.0)
		DPrint(wxT("%g ms\n"), MILLI(samplePtr->dt));
	else
		DPrint(wxT("<prev. signal dt>.\n"));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_Sample(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_Sample");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	samplePtr = (SamplePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Sample(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_Sample");

	if (!SetParsPointer_Utility_Sample(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_Sample(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = samplePtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_Sample;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_Sample;
	theModule->PrintPars = PrintPars_Utility_Sample;
	theModule->RunProcess = Process_Utility_Sample;
	theModule->SetParsPointer = SetParsPointer_Utility_Sample;
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
CheckData_Utility_Sample(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_Sample");
	Float	signalDuration, dt;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	dt = _InSig_EarObject(data, 0)->dt;
	if ((samplePtr->dt > 0.0) && (samplePtr->dt < dt)) {
		NotifyError(wxT("%s: Sampling interval (%g ms) is less than signal\n")
		  wxT("sampling interval (%g ms)."), funcName, MSEC(samplePtr->dt),
		  MSEC(dt));
		return(FALSE);
	}
	signalDuration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if (samplePtr->timeOffset >= signalDuration) {
		NotifyError(wxT("%s: Time offset (%g ms) is too long for the signal ")
		  wxT("duration (%g ms)."), funcName, MSEC(samplePtr->timeOffset),
		  MSEC(signalDuration));
		return(FALSE);
	}
	return(TRUE);

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
Process_Utility_Sample(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_Sample");
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	Float	dt;
	ChanLen	i;
	SignalDataPtr	outSignal;
	SamplePtr	p = samplePtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_Sample(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Sample utility process"));
		dt = (p->dt > 0.0)? p->dt: _InSig_EarObject(data, 0)->dt;
		p->dtIndex = (ChanLen) (dt / _InSig_EarObject(data, 0)->dt + 0.5);
		p->timeOffsetIndex = (data->timeIndex != PROCESS_START_TIME)? 0:
		  (ChanLen) (p->timeOffset / _InSig_EarObject(data, 0)->dt + 0.5);
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  (_InSig_EarObject(data, 0)->length - p->timeOffsetIndex) / p->dtIndex, dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan] + p->timeOffsetIndex;
		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++, inPtr += p->dtIndex)
			*outPtr++ = *inPtr;
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

