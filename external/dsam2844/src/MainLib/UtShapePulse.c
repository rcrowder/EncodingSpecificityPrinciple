/**********************
 *
 * File:		UtShapePulse.c
 * Purpose:		This module turns a train of detected spikes into a pulse
 *				train with set magnitudes and durations.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 * Author:		L. P. O'Mard
 * Created:		18th April 1996
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
#include "UtShapePulse.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ShapePulsePtr	shapePulsePtr = NULL;

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
Free_Utility_ShapePulse(void)
{
	if (shapePulsePtr == NULL)
		return(FALSE);
	FreeProcessVariables_Utility_ShapePulse();
	if (shapePulsePtr->parList)
		FreeList_UniParMgr(&shapePulsePtr->parList);
	if (shapePulsePtr->parSpec == GLOBAL) {
		free(shapePulsePtr);
		shapePulsePtr = NULL;
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
Init_Utility_ShapePulse(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_ShapePulse");

	if (parSpec == GLOBAL) {
		if (shapePulsePtr != NULL)
			Free_Utility_ShapePulse();
		if ((shapePulsePtr = (ShapePulsePtr) malloc(sizeof(ShapePulse))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (shapePulsePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	shapePulsePtr->parSpec = parSpec;
	shapePulsePtr->updateProcessVariablesFlag = TRUE;
	shapePulsePtr->eventThreshold = 0.0;
	shapePulsePtr->pulseDuration = 0.2e-3;
	shapePulsePtr->pulseMagnitude = 3.8;
	shapePulsePtr->remainingPulseTime = NULL;

	if (!SetUniParList_Utility_ShapePulse()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_ShapePulse();
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
SetUniParList_Utility_ShapePulse(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_ShapePulse");
	UniParPtr	pars;

	if ((shapePulsePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_SHAPEPULSE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = shapePulsePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_SHAPEPULSE_EVENTTHRESHOLD], wxT("THRESHOLD"),
	  wxT("Event threshold (arbitrary units)."),
	  UNIPAR_REAL,
	  &shapePulsePtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Utility_ShapePulse);
	SetPar_UniParMgr(&pars[UTILITY_SHAPEPULSE_PULSEDURATION], wxT("DURATION"),
	  wxT("Pulse duration (s)."),
	  UNIPAR_REAL,
	  &shapePulsePtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_Utility_ShapePulse);
	SetPar_UniParMgr(&pars[UTILITY_SHAPEPULSE_PULSEMAGNITUDE], wxT("MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &shapePulsePtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_Utility_ShapePulse);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_ShapePulse(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_ShapePulse");

	if (shapePulsePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (shapePulsePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(shapePulsePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_ShapePulse(Float eventThreshold, Float
  pulseDuration, Float pulseMagnitude)
{
	static const WChar	*funcName = wxT("SetPars_Utility_ShapePulse");
	BOOLN	ok;

	ok = TRUE;
	if (!SetEventThreshold_Utility_ShapePulse(eventThreshold))
		ok = FALSE;
	if (!SetPulseDuration_Utility_ShapePulse(pulseDuration))
		ok = FALSE;
	if (!SetPulseMagnitude_Utility_ShapePulse(pulseMagnitude))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetEventThreshold *****************************/

/*
 * This function sets the module's eventThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEventThreshold_Utility_ShapePulse(Float theEventThreshold)
{
	static const WChar	*funcName = wxT("SetEventThreshold_Utility_ShapePulse");

	if (shapePulsePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	shapePulsePtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_Utility_ShapePulse(Float thePulseDuration)
{
	static const WChar	*funcName = wxT("SetPulseDuration_Utility_ShapePulse");

	if (shapePulsePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePulseDuration < 0.0) {
		NotifyError(wxT("%s: Pulse duration must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	shapePulsePtr->pulseDuration = thePulseDuration;
	shapePulsePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_Utility_ShapePulse(Float thePulseMagnitude)
{
	static const WChar	*funcName = wxT("SetPulseMagnitude_Utility_ShapePulse");

	if (shapePulsePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	shapePulsePtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_ShapePulse(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_ShapePulse");

	if (shapePulsePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Shape Pulse Utility Module Parameters:-\n"));
	DPrint(wxT("\tEvent threshold = %g (arbitrary units),\n"),
	  shapePulsePtr->eventThreshold);
	DPrint(wxT("\tPulse duration = %g ms,"),
	  MSEC(shapePulsePtr->pulseDuration));
	DPrint(wxT("\tPulse magnitude = %g (nA?).\n"),
	  shapePulsePtr->pulseMagnitude);
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_ShapePulse(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_ShapePulse");

	if (!SetParsPointer_Utility_ShapePulse(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_ShapePulse(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = shapePulsePtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_ShapePulse;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_ShapePulse;
	theModule->PrintPars = PrintPars_Utility_ShapePulse;
	theModule->ResetProcess = ResetProcess_Utility_ShapePulse;
	theModule->RunProcess = Process_Utility_ShapePulse;
	theModule->SetParsPointer = SetParsPointer_Utility_ShapePulse;
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Utility_ShapePulse(EarObjectPtr data)
{
	int		i;

	ResetOutSignal_EarObject(data);
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = _OutSig_EarObject(data)->offset; i < _OutSig_EarObject(data)->
		  numChannels; i++)
			shapePulsePtr->remainingPulseTime[i] = 0.0;
	}

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Utility_ShapePulse(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_Utility_ShapePulse");
	ShapePulsePtr	p = shapePulsePtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Utility_ShapePulse();
		if ((p->remainingPulseTime = (Float *) calloc(_OutSig_EarObject(data)->
		  numChannels, sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for remainingPulseTime array."),
			  funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetProcess_Utility_ShapePulse(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Utility_ShapePulse(void)
{
	if (shapePulsePtr->remainingPulseTime) {
		free(shapePulsePtr->remainingPulseTime);
		shapePulsePtr->remainingPulseTime = NULL;
	}
	shapePulsePtr->updateProcessVariablesFlag = TRUE;

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_ShapePulse(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_ShapePulse");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	shapePulsePtr = (ShapePulsePtr) theModule->parsPtr;
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
CheckData_Utility_ShapePulse(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_ShapePulse");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (shapePulsePtr->pulseDuration < _InSig_EarObject(data, 0)->dt) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n")
		  wxT("interval, %g ms (%g ms)\n"), funcName,
		  MSEC(_InSig_EarObject(data, 0)->dt), MSEC(shapePulsePtr->
		    pulseDuration));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
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
Process_Utility_ShapePulse(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_ShapePulse");
	register	ChanData	*inPtr, *outPtr, lastValue;
	register	Float		*remainingPulseTimePtr, dt;
	BOOLN	riseDetected;
	int		chan;
	ChanLen	j;
	SignalDataPtr	outSignal, inSignal;
	ShapePulsePtr	p = shapePulsePtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_ShapePulse(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Shape Pulse Utility Process"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Utility_ShapePulse(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	dt = inSignal->dt;
	remainingPulseTimePtr = p->remainingPulseTime + outSignal->offset;
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan];
		outPtr = outSignal->channel[chan];
		riseDetected = FALSE;
		lastValue = *inPtr++;
		for (j = 1; j < outSignal->length; j++) {
			if (!riseDetected)
				riseDetected = (*inPtr > lastValue);
			else {
				if (*inPtr < lastValue) {
					riseDetected = FALSE;
					if (lastValue > p->eventThreshold)
						*remainingPulseTimePtr = p->pulseDuration;
				}
			}
			if (*remainingPulseTimePtr > 0.0) {
				*outPtr += p->pulseMagnitude;
				*remainingPulseTimePtr -= dt;
			}
			lastValue = *inPtr++;
			outPtr++;
		}
		remainingPulseTimePtr++;
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

