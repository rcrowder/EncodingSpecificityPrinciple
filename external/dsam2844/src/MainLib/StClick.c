/**********************
 *
 * File:		StClick.c
 * Purpose:		This module contains the methods for the Click stimulus
 *				generation paradigm.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		12 Mar 1997
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
#include "StClick.h"
#include "UtString.h"
#include "FiParFile.h"

/******************************************************************************/
/********************************* Global variables ***************************/
/******************************************************************************/

ClickPtr		clickPtr = NULL;

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
Init_Click(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_Click");

	if (parSpec == GLOBAL) {
		if (clickPtr != NULL)
			Free_Click();
		if ((clickPtr = (ClickPtr) malloc(sizeof(Click))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (clickPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	clickPtr->parSpec = parSpec;
	clickPtr->clickTime = 0.01;
	clickPtr->amplitude = 1.0;
	clickPtr->duration = 0.1;
	clickPtr->dt = DEFAULT_DT;

	if (!SetUniParList_Click()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Click();
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
Free_Click(void)
{
	if (clickPtr == NULL)
		return(TRUE);
	if (clickPtr->parList)
		FreeList_UniParMgr(&clickPtr->parList);
	if (clickPtr->parSpec == GLOBAL) {
		free(clickPtr);
		clickPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_Click(void)
{
	static const WChar *funcName = wxT("SetUniParList_Click");
	UniParPtr	pars;

	if ((clickPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  CLICK_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = clickPtr->parList->pars;
	SetPar_UniParMgr(&pars[CLICK_CLICKTIME], wxT("TIME"),
	  wxT("Time for the delta-function click (s)."),
	  UNIPAR_REAL,
	  &clickPtr->clickTime, NULL,
	  (void * (*)) SetClickTime_Click);
	SetPar_UniParMgr(&pars[CLICK_AMPLITUDE], wxT("AMPLITUDE"),
	  wxT("Amplitude (uPa)."),
	  UNIPAR_REAL,
	  &clickPtr->amplitude, NULL,
	  (void * (*)) SetAmplitude_Click);
	SetPar_UniParMgr(&pars[CLICK_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &clickPtr->duration, NULL,
	  (void * (*)) SetDuration_Click);
	SetPar_UniParMgr(&pars[CLICK_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &clickPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_Click);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Click(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_Click");

	if (clickPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (clickPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(clickPtr->parList);

}

/********************************* SetClickTime *******************************/

/*
 * This function sets the module's clickTime parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetClickTime_Click(Float theClickTime)
{
	static const WChar *funcName = wxT("SetClickTime_Click");

	if (clickPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	clickPtr->clickTime = theClickTime;
	return(TRUE);

}

/********************************* SetAmplitude *******************************/

/*
 * This function sets the module's amplitude parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetAmplitude_Click(Float theAmplitude)
{
	static const WChar *funcName = wxT("SetAmplitude_Click");

	if (clickPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	clickPtr->amplitude = theAmplitude;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_Click(Float theDuration)
{
	static const WChar *funcName = wxT("SetDuration_Click");

	if (clickPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	clickPtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_Click(Float theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_Click");

	if (clickPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	clickPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_Click(void)
{
	DPrint(wxT("Click Module Parameters:-\n"));
	DPrint(wxT("\tClick time = %g ms,\tAmplitude = %g uPa,\n"),
	  MSEC(clickPtr->clickTime), clickPtr->amplitude);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"),
	  MSEC(clickPtr->duration), MSEC(clickPtr->dt));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Click(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Click");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	clickPtr = (ClickPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Click(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Click");

	if (!SetParsPointer_Click(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Click(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = clickPtr;
	theModule->Free = Free_Click;
	theModule->GetUniParListPtr = GetUniParListPtr_Click;
	theModule->PrintPars = PrintPars_Click;
	theModule->RunProcess = GenerateSignal_Click;
	theModule->SetParsPointer = SetParsPointer_Click;
	return(TRUE);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckData_Click(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_Click");
	Float	remainder;


	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	remainder = fmod(clickPtr->clickTime, clickPtr->dt);
	if ((remainder > DSAM_EPSILON) && (fabs(remainder - clickPtr->dt) >
	  DSAM_EPSILON)) {
		NotifyError(wxT("%s: The click time should be a multiple of the ")
		  wxT("sampling interval (%g ms)"), funcName, MILLI(clickPtr->dt));
		return(FALSE);
	}
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
GenerateSignal_Click(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_Click");
	ChanLen		timeIndex;
	ChanData	*dataPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Click(data))
			return(FALSE);
		SetProcessName_EarObject(data, wxT("Click (delta-function) stimulus"));
		if (!InitOutSignal_EarObject(data, 1, (ChanLen) (clickPtr->duration /
			clickPtr->dt + 0.5), clickPtr->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	dataPtr = _OutSig_EarObject(data)->channel[0];
	/* The channel is initialised to zero by InitOutSignal_EarObject. */
	timeIndex = (ChanLen) floor(clickPtr->clickTime / clickPtr->dt + 0.5) - 1;
	dataPtr[timeIndex] = clickPtr->amplitude;
	_OutSig_EarObject(data)->rampFlag = TRUE;	/* Clicks cannot really be ramped. */
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_Click */

