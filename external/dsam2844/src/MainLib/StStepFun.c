/**********************
 *
 * File:		StStepFun.c
 * Purpose:		This module contains the step function stimulus generation
 *				paradigm.
 * Comments:	The module generates a constant signal preceded and ended by
 *				periods of of specified amplitude.
 *				The magnitude of the step function has no units, i.e. they
 *				must be implicitly defined by the user (Arbitrary units)
 *				09-11-98 LPO: Changed parameter names 'stepAmplitude' ->
 *				'amplitude' and 'duration' -> 'duration'.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		09 Nov 1998
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
#include "StStepFun.h"

/********************************* Global variables ***************************/

StepFunPtr	stepFunPtr = NULL;

/******************************************************************************/
/********************************* Subroutines and functions ******************/
/******************************************************************************/

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
Init_StepFunction(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_StepFunction");

	if (parSpec == GLOBAL) {
		if (stepFunPtr != NULL)
			free(stepFunPtr);
		if ((stepFunPtr = (StepFunPtr) malloc(sizeof(StepFun))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (stepFunPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	stepFunPtr->parSpec = parSpec;
	stepFunPtr->amplitude = 30.0;
	stepFunPtr->duration = 0.08;
	stepFunPtr->dt = DEFAULT_DT;
	stepFunPtr->beginPeriodDuration  = 0.01;
	stepFunPtr->endPeriodDuration = 0.01;
	stepFunPtr->beginEndAmplitude = 0.0;

	if (!SetUniParList_StepFunction()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_StepFunction();
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
Free_StepFunction(void)
{
	if (stepFunPtr == NULL)
		return(TRUE);
	if (stepFunPtr->parList)
		FreeList_UniParMgr(&stepFunPtr->parList);
	if (stepFunPtr->parSpec == GLOBAL) {
		free(stepFunPtr);
		stepFunPtr = NULL;
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
SetUniParList_StepFunction(void)
{
	static const WChar *funcName = wxT("SetUniParList_StepFunction");
	UniParPtr	pars;

	if ((stepFunPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  STEPFUNCTION_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = stepFunPtr->parList->pars;
	SetPar_UniParMgr(&pars[STEPFUNCTION_AMPLITUDE], wxT("AMPLITUDE"),
	  wxT("Amplitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &stepFunPtr->amplitude, NULL,
	  (void * (*)) SetAmplitude_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_BEGINENDAMPLITUDE], wxT("B_E_AMP"),
	  wxT("Begin-end period amplitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &stepFunPtr->beginEndAmplitude, NULL,
	  (void * (*)) SetBeginEndAmplitude_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_BEGINPERIODDURATION], wxT("BEGIN_PERIOD"),
	  wxT("Period before the signal begins (s)."),
	  UNIPAR_REAL,
	  &stepFunPtr->beginPeriodDuration, NULL,
	  (void * (*)) SetBeginPeriodDuration_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_ENDPERIODDURATION], wxT("END_PERIOD"),
	  wxT("Period after the signal ends (s)."),
	  UNIPAR_REAL,
	  &stepFunPtr->endPeriodDuration, NULL,
	  (void * (*)) SetEndPeriodDuration_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &stepFunPtr->duration, NULL,
	  (void * (*)) SetDuration_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &stepFunPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_StepFunction);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_StepFunction(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (stepFunPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(stepFunPtr->parList);

}

/********************************* SetAmplitude *******************************/

/*
 * This function sets the module's amplitude parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetAmplitude_StepFunction(Float theAmplitude)
{
	static const WChar *funcName = wxT("SetAmplitude_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->amplitude = theAmplitude;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_StepFunction(Float theDuration)
{
	static const WChar *funcName = wxT("SetDuration_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetBeginPeriodDuration *********************/

/*
 * This function sets the module's beginPeriodDuration parameter.  It first
 * checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeginPeriodDuration_StepFunction(Float theBeginPeriodDuration)
{
	static const WChar *funcName = wxT("SetBeginPeriodDuration_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->beginPeriodDuration = theBeginPeriodDuration;
	return(TRUE);

}

/********************************* SetEndPeriodDuration ***********************/

/*
 * This function sets the module's endPeriodDuration parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetEndPeriodDuration_StepFunction(Float theEndPeriodDuration)
{
	static const WChar *funcName = wxT("SetEndPeriodDuration_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->endPeriodDuration = theEndPeriodDuration;
	return(TRUE);

}

/********************************* SetBeginEndAmplitude ***********************/

/*
 * This function sets the module's beginEndAmplitude parameter.  It first
 * checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeginEndAmplitude_StepFunction(Float theBeginEndAmplitude)
{
	static const WChar *funcName = wxT("SetBeginEndAmplitude_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->beginEndAmplitude = theBeginEndAmplitude;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's timeStep parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_StepFunction(Float theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	stepFunPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_StepFunction(void)
{
	DPrint(wxT("Step Function Module Parameters:-\n"));
	DPrint(wxT("\tStep amplitude = %g,\t\tStep duration = ")
	  wxT("%g ms,\n"), stepFunPtr->amplitude, MSEC(stepFunPtr->duration));
	DPrint(wxT("\tBegin-end Amplitude = %g,\tBeing/end period ")
	  wxT("duration = %g/%g ms\n"), stepFunPtr->beginEndAmplitude,
	  MSEC(stepFunPtr->beginPeriodDuration),
	  MSEC(stepFunPtr->endPeriodDuration));
	DPrint(wxT("\tSampling interval = %g ms\n"),
	  MSEC(stepFunPtr->dt));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_StepFunction(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_StepFunction");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	stepFunPtr = (StepFunPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_StepFunction(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_StepFunction");

	if (!SetParsPointer_StepFunction(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_StepFunction(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = stepFunPtr;
	theModule->Free = Free_StepFunction;
	theModule->GetUniParListPtr = GetUniParListPtr_StepFunction;
	theModule->PrintPars = PrintPars_StepFunction;
	theModule->RunProcess = GenerateSignal_StepFunction;
	theModule->SetParsPointer = SetParsPointer_StepFunction;
	return(TRUE);

}

/********************************* GenerateSignal *****************************/

/*
 * This routine allocates memory for the output signal, if necessary, and
 * generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * It can be called repeatedly with different parameter values if required.
 * Stimulus generation only sets the output signal, the input signal is not
 * used.
 */

BOOLN
GenerateSignal_StepFunction(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_StepFunction");
	Float		totalDuration;
	register	ChanData	*dataPtr;
	ChanLen		i, beginPeriodDurationIndex, endSignalIndex;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Step-function stimulus"));
		totalDuration = stepFunPtr->beginPeriodDuration +
		  stepFunPtr->duration + stepFunPtr->endPeriodDuration;
		data->updateProcessFlag = TRUE;
		if ( !InitOutSignal_EarObject(data, 1, (ChanLen) floor(totalDuration /
		  stepFunPtr->dt + 0.5), stepFunPtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	beginPeriodDurationIndex = (ChanLen) floor(stepFunPtr->beginPeriodDuration /
	  _OutSig_EarObject(data)->dt + 0.5);
	endSignalIndex = beginPeriodDurationIndex + (ChanLen) floor(stepFunPtr->
	  duration / stepFunPtr->dt + 0.5);
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0; i < _OutSig_EarObject(data)->length; i++)
		if ( (i >= beginPeriodDurationIndex) && (i <= endSignalIndex) )
			*(dataPtr++) = stepFunPtr->amplitude;
		else
			*(dataPtr++) = stepFunPtr->beginEndAmplitude;
	_OutSig_EarObject(data)->rampFlag = TRUE;/* Step functions should not be ramped. */
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_StepFunction */

