/**********************
 *
 * File:		StPulseTrain.c
 * Purpose:		The pulse train stimulus generation module.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				21-11-97 LPO: Amended routine so that the first pulse occurrs
 *				in the first sample of the output.
 * Author:		L. P. O'Mard
 * Created:		04 Mar 1996
 * Updated:		21 Nov 1997
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
#include "StPulseTrain.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

PulseTrainPtr	pulseTrainPtr = NULL;

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
Free_PulseTrain(void)
{
	if (pulseTrainPtr == NULL)
		return(FALSE);
	if (pulseTrainPtr->parList)
		FreeList_UniParMgr(&pulseTrainPtr->parList);
	if (pulseTrainPtr->parSpec == GLOBAL) {
		free(pulseTrainPtr);
		pulseTrainPtr = NULL;
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
Init_PulseTrain(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_PulseTrain");

	if (parSpec == GLOBAL) {
		if (pulseTrainPtr != NULL)
			Free_PulseTrain();
		if ((pulseTrainPtr = (PulseTrainPtr) malloc(sizeof(PulseTrain))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pulseTrainPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	pulseTrainPtr->parSpec = parSpec;
	pulseTrainPtr->pulseRate = 360.0;
	pulseTrainPtr->pulseDuration = 0.1e-3;
	pulseTrainPtr->amplitude = 3.4e-7;
	pulseTrainPtr->duration = 0.1;
	pulseTrainPtr->dt = DEFAULT_DT;

	if (!SetUniParList_PulseTrain()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PulseTrain();
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
SetUniParList_PulseTrain(void)
{
	static const WChar *funcName = wxT("SetUniParList_PulseTrain");
	UniParPtr	pars;

	if ((pulseTrainPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PULSETRAIN_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = pulseTrainPtr->parList->pars;
	SetPar_UniParMgr(&pars[PULSETRAIN_PULSERATE], wxT("RATE"),
	  wxT("Pulse rate (pulses/s)"),
	  UNIPAR_REAL,
	  &pulseTrainPtr->pulseRate, NULL,
	  (void * (*)) SetPulseRate_PulseTrain);
	SetPar_UniParMgr(&pars[PULSETRAIN_PULSEDURATION], wxT("PULSE_DURATION"),
	  wxT("Pulse duration (s)."),
	  UNIPAR_REAL,
	  &pulseTrainPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_PulseTrain);
	SetPar_UniParMgr(&pars[PULSETRAIN_AMPLITUDE], wxT("AMPLITUDE"),
	  wxT("Amplitude (arbitary units)."),
	  UNIPAR_REAL,
	  &pulseTrainPtr->amplitude, NULL,
	  (void * (*)) SetAmplitude_PulseTrain);
	SetPar_UniParMgr(&pars[PULSETRAIN_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &pulseTrainPtr->duration, NULL,
	  (void * (*)) SetDuration_PulseTrain);
	SetPar_UniParMgr(&pars[PULSETRAIN_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &pulseTrainPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PulseTrain);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PulseTrain(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_PulseTrain");

	if (pulseTrainPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (pulseTrainPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(pulseTrainPtr->parList);

}

/****************************** SetPulseRate **********************************/

/*
 * This function sets the module's pulseRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseRate_PulseTrain(Float thePulseRate)
{
	static const WChar	*funcName = wxT("SetPulseRate_PulseTrain");

	if (pulseTrainPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePulseRate < DSAM_EPSILON) {
		NotifyError(wxT("%s: Illegal value (%g)."), funcName, thePulseRate);
		return(FALSE);
	}
	pulseTrainPtr->pulseRate = thePulseRate;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_PulseTrain(Float thePulseDuration)
{
	static const WChar	*funcName = wxT("SetPulseDuration_PulseTrain");

	if (pulseTrainPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePulseDuration < DSAM_EPSILON) {
		NotifyError(wxT("%s: Illegal value (%g)."), funcName, thePulseDuration);
		return(FALSE);
	}
	pulseTrainPtr->pulseDuration = thePulseDuration;
	return(TRUE);

}

/****************************** SetAmplitude **********************************/

/*
 * This function sets the module's amplitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAmplitude_PulseTrain(Float theAmplitude)
{
	static const WChar	*funcName = wxT("SetAmplitude_PulseTrain");

	if (pulseTrainPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pulseTrainPtr->amplitude = theAmplitude;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_PulseTrain(Float theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_PulseTrain");

	if (pulseTrainPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theDuration < DSAM_EPSILON) {
		NotifyError(wxT("%s: Illegal value (%g)."), funcName, theDuration);
		return(FALSE);
	}
	pulseTrainPtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_PulseTrain(Float theSamplingInterval)
{
	static const WChar *funcName =wxT("SetSamplingInterval_PulseTrain");

	if (pulseTrainPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pulseTrainPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_PulseTrain(void)
{
	DPrint(wxT("Pulse-Train Module Parameters:-\n"));
	DPrint(wxT("\tPulse rate = %g /s,\t"),
	  pulseTrainPtr->pulseRate);
	DPrint(wxT("\tPulse duration = %g ms\n"),
	  MSEC(pulseTrainPtr->pulseDuration));
	DPrint(wxT("\tAmplitude = %g (arbitrary units),\n"),
	  pulseTrainPtr->amplitude);
	DPrint(wxT("\tDuration = %g ms,\t"),
	  MSEC(pulseTrainPtr->duration));
	DPrint(wxT("\tSampling interval = %g ms\n"),
	  MSEC(pulseTrainPtr->dt));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_PulseTrain(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PulseTrain");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	pulseTrainPtr = (PulseTrainPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PulseTrain(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PulseTrain");

	if (!SetParsPointer_PulseTrain(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PulseTrain(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = pulseTrainPtr;
	theModule->Free = Free_PulseTrain;
	theModule->GetUniParListPtr = GetUniParListPtr_PulseTrain;
	theModule->PrintPars = PrintPars_PulseTrain;
	theModule->RunProcess = GenerateSignal_PulseTrain;
	theModule->SetParsPointer = SetParsPointer_PulseTrain;
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
CheckData_PulseTrain(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_PulseTrain");
	Float	pulsePeriod;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	pulsePeriod = 1.0 / pulseTrainPtr->pulseRate;
	if (pulsePeriod <= pulseTrainPtr->pulseDuration) {
		NotifyError(wxT("%s: Pulse rate is too high for the pulse ")
		  wxT("duration\n(rate must be < %g /s)"), funcName, 1.0 /
		    pulseTrainPtr->pulseDuration);
		return(FALSE);
	}
	if (pulsePeriod / pulseTrainPtr->dt < 2.0) {
		NotifyError(wxT("%s: Pulse rate is too low for the sampling interval\n")
		  wxT("(rate must be > %g /s)"), funcName, 1.0 / (pulseTrainPtr->dt *
		  2.0));
		return(FALSE);
	}
	if (pulsePeriod > pulseTrainPtr->duration) {
		NotifyError(wxT("%s: Pulse rate is too low for the signal duration\n")
		  wxT("(rate must be > %g /s)"), funcName, 1.0 / pulseTrainPtr->
		  duration);
		return(FALSE);
	}
	if (pulseTrainPtr->pulseDuration < pulseTrainPtr->dt) {
		NotifyError(wxT("%s: Pulse duration is less then sampling interval ")
		  wxT("( use > %g ms)."), funcName, MSEC(pulseTrainPtr->pulseDuration));
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GenerateSignal ********************************/

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
GenerateSignal_PulseTrain(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("GenerateSignal_PulseTrain");
	register	ChanData	 *outPtr;
	Float		pulsePeriod, t;
	ChanLen		i;
	PulseTrainPtr	p = pulseTrainPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_PulseTrain(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Pulse-train stimulus"));
		data->updateProcessFlag = TRUE;
		if ( !InitOutSignal_EarObject(data, 1, (ChanLen) floor(p->
		  duration / p->dt + 0.5), p->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		_OutSig_EarObject(data)->rampFlag = TRUE; /* Do not ramp. */
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	pulsePeriod = 1.0 / p->pulseRate;
	if (data->timeIndex == PROCESS_START_TIME) {
		p->nextPulseTime = pulsePeriod;
		p->remainingPulseTime = p->pulseDuration;
	}
	outPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->
	  length; i++, outPtr++) {
		t = (data->timeIndex + 1 + i) * p->dt;
		if (p->remainingPulseTime > 0.0) {
			*outPtr = p->amplitude;
			p->remainingPulseTime -= p->dt;
		} else {
			*outPtr = 0.0;
			if ((t + DSAM_EPSILON) > p->nextPulseTime) {
				p->remainingPulseTime = p->pulseDuration;
				p->nextPulseTime = t + pulsePeriod;
			}
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

