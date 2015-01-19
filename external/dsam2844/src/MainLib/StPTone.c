/**********************
 *
 * File:		StPTone.c
 * Purpose:		This module contains the methods for the simple pure-tone
 *				signal generation paradigm.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Mar 1997
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
#include "StPTone.h"

/********************************* Global variables ***************************/

PureTonePtr	pureTonePtr = NULL;

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
Init_PureTone(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_PureTone");

	if (parSpec == GLOBAL) {
		if (pureTonePtr != NULL)
			Free_PureTone();
		if ((pureTonePtr = (PureTonePtr) malloc(sizeof(PureTone))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pureTonePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	pureTonePtr->parSpec = parSpec;
	pureTonePtr->frequency = 1000.0;
	pureTonePtr->intensity = DEFAULT_INTENSITY;
	pureTonePtr->duration = 0.1;
	pureTonePtr->dt = DEFAULT_DT;

	if (!SetUniParList_PureTone()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PureTone();
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
Free_PureTone(void)
{
	if (pureTonePtr == NULL)
		return(TRUE);
	if (pureTonePtr->parList)
		FreeList_UniParMgr(&pureTonePtr->parList);
	if (pureTonePtr->parSpec == GLOBAL) {
		free(pureTonePtr);
		pureTonePtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_PureTone(void)
{
	static const WChar *funcName = wxT("SetUniParList_PureTone");
	UniParPtr	pars;

	if ((pureTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = pureTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_FREQUENCY], wxT("FREQUENCY"),
	  wxT("Frequency (Hz)."),
	  UNIPAR_REAL,
	  &pureTonePtr->frequency, NULL,
	  (void * (*)) SetFrequency_PureTone);
	SetPar_UniParMgr(&pars[PURETONE_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &pureTonePtr->intensity, NULL,
	  (void *(*)) SetIntensity_PureTone);
	SetPar_UniParMgr(&pars[PURETONE_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &pureTonePtr->duration, NULL,
	  (void *(*)) SetDuration_PureTone);
	SetPar_UniParMgr(&pars[PURETONE_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &pureTonePtr->dt, NULL,
	  (void *(*)) SetSamplingInterval_PureTone);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_PureTone(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (pureTonePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(pureTonePtr->parList);

}

/********************************* SetFrequency *******************************/

/*
 * This function sets the module's frequency parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequency_PureTone(Float theFrequency)
{
	static const WChar *funcName = wxT("SetFrequency_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theFrequency <= 0.0) {
		NotifyError(wxT("%s: Frequency must be greater than zero (%g Hz)."),
		  funcName, theFrequency);
		return(FALSE);
	}
	pureTonePtr->frequency = theFrequency;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_PureTone(Float theIntensity)
{
	static const WChar *funcName = wxT("SetIntensity_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTonePtr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_PureTone(Float theDuration)
{
	static const WChar *funcName = wxT("SetDuration_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTonePtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_PureTone(Float theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	pureTonePtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_PureTone(void)
{
	static const WChar *funcName = wxT("PrintPars_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Pure Tone Module Parameters:-\n"));
	DPrint(wxT("\tFrequency = %g Hz,\tIntensity = %g dB SPL,\n"),
	  pureTonePtr->frequency, pureTonePtr->intensity);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"),
	  MSEC(pureTonePtr->duration), MSEC(pureTonePtr->dt));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_PureTone(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PureTone");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	pureTonePtr = (PureTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_PureTone(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PureTone");

	if (!SetParsPointer_PureTone(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PureTone(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = pureTonePtr;
	theModule->Free = Free_PureTone;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone;
	theModule->PrintPars = PrintPars_PureTone;
	theModule->RunProcess = GenerateSignal_PureTone;
	theModule->SetParsPointer = SetParsPointer_PureTone;
	return(TRUE);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It also checks that the Nyquist critical frequency is not exceeded.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckData_PureTone(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_PureTone");
	Float	criticalFrequency;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	criticalFrequency = 1.0 / (2.0 * pureTonePtr->dt);
	if (criticalFrequency <= pureTonePtr->frequency) {
		NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low for the ")
		  wxT("frequency."), funcName, MSEC(pureTonePtr->dt));
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
 * With repeated calls the Signal memory is only allocated once, then re-used.
 */

BOOLN
GenerateSignal_PureTone(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_PureTone");
	ChanLen		i, t;
	register	Float		amplitude;
	register	ChanData	*dataPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_PureTone(data))
			return(FALSE);
		SetProcessName_EarObject(data, wxT("Pure Tone stimulus"));
		if ( !InitOutSignal_EarObject(data, PURETONE_NUM_CHANNELS,
		  (ChanLen) floor(pureTonePtr->duration / pureTonePtr->dt + 0.5),
		  pureTonePtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	amplitude = RMS_AMP(pureTonePtr->intensity) * SQRT_2;
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->length; i++, t++)
		*(dataPtr++) = amplitude * sin(PIx2 * pureTonePtr->frequency *
		  (t * _OutSig_EarObject(data)->dt));
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone */

