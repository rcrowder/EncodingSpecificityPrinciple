/**********************
 *
 * File:		StPTone2.c
 * Purpose:		The module generates a pure-tone signal preceded and ended by
 *				periods of silence.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
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
#include "StPTone2.h"

/********************************* Global variables ***************************/

PureTone2Ptr	pureTone2Ptr = NULL;

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
Init_PureTone_2(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_PureTone_2");

	if (parSpec == GLOBAL) {
		if (pureTone2Ptr != NULL)
			Free_PureTone_2();
		if ((pureTone2Ptr = (PureTone2Ptr) malloc(sizeof(PureTone2))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pureTone2Ptr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	pureTone2Ptr->parSpec = parSpec;
	pureTone2Ptr->frequency = 1000.0;
	pureTone2Ptr->intensity = DEFAULT_INTENSITY;
	pureTone2Ptr->duration = 0.08;
	pureTone2Ptr->dt = DEFAULT_DT;
	pureTone2Ptr->beginPeriodDuration = 0.01;
	pureTone2Ptr->endPeriodDuration = 0.01;

	if (!SetUniParList_PureTone_2()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PureTone_2();
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
Free_PureTone_2(void)
{
	if (pureTone2Ptr == NULL)
		return(TRUE);
	if (pureTone2Ptr->parList)
		FreeList_UniParMgr(&pureTone2Ptr->parList);
	if (pureTone2Ptr->parSpec == GLOBAL) {
		free(pureTone2Ptr);
		pureTone2Ptr = NULL;
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
SetUniParList_PureTone_2(void)
{
	static const WChar *funcName = wxT("SetUniParList_PureTone_2");
	UniParPtr	pars;

	if ((pureTone2Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_2_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = pureTone2Ptr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_2_FREQUENCY], wxT("FREQUENCY"),
	  wxT("Frequency (Hz)."),
	  UNIPAR_REAL,
	  &pureTone2Ptr->frequency, NULL,
	  (void * (*)) SetFrequency_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &pureTone2Ptr->intensity, NULL,
	  (void * (*)) SetIntensity_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &pureTone2Ptr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &pureTone2Ptr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_BEGINPERIODDURATION], wxT(
	  "BEGIN_SILENCE"),
	  wxT("Silence period before the signal begins (s)."),
	  UNIPAR_REAL,
	  &pureTone2Ptr->beginPeriodDuration, NULL,
	  (void * (*)) SetBeginPeriodDuration_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_ENDPERIODDURATION], wxT("END_SILENCE"),
	  wxT("Silence period after the signal ends (s)."),
	  UNIPAR_REAL,
	  &pureTone2Ptr->endPeriodDuration, NULL,
	  (void * (*)) SetEndPeriodDuration_PureTone_2);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_2(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_PureTone_2");

	if (pureTone2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (pureTone2Ptr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(pureTone2Ptr->parList);

}

/********************************* SetFrequency *******************************/

/*
 * This function sets the module's frequency parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequency_PureTone_2(Float theFrequency)
{
	static const WChar *funcName = wxT("SetFrequency_PureTone_2");

	if (pureTone2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone2Ptr->frequency = theFrequency;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_PureTone_2(Float theIntensity)
{
	static const WChar *funcName = wxT("SetIntensity_PureTone_2");

	if (pureTone2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone2Ptr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_PureTone_2(Float theDuration)
{
	static const WChar *funcName = wxT("SetDuration_PureTone_2");

	if (pureTone2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone2Ptr->duration = theDuration;
	return(TRUE);

}

/********************************* SetBeginPeriodDuration *********************/

/*
 * This function sets the module's beginPeriodDuration parameter.  It first
 * checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeginPeriodDuration_PureTone_2(Float theBeginPeriodDuration)
{
	static const WChar *funcName = wxT("SetBeginPeriodDuration_PureTone_2");

	if (pureTone2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone2Ptr->beginPeriodDuration = theBeginPeriodDuration;
	return(TRUE);

}

/********************************* SetEndPeriodDuration ***********************/

/*
 * This function sets the module's endPeriodDuration parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetEndPeriodDuration_PureTone_2(Float theEndPeriodDuration)
{
	static const WChar *funcName = wxT("SetEndPeriodDuration_PureTone_2");

	if (pureTone2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone2Ptr->endPeriodDuration = theEndPeriodDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's timeStep parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_PureTone_2(Float theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_PureTone_2");

	if (pureTone2Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	pureTone2Ptr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_PureTone_2(void)
{
	DPrint(wxT("Pure Tone 2 (surrounded by silence) Module Parameters:-\n"));
	DPrint(wxT("\tFrequency = %g (Hz),\tIntensity = %g dB SPL,\n"),
	  pureTone2Ptr->frequency, pureTone2Ptr->intensity);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"),
	  MSEC(pureTone2Ptr->duration), MSEC(pureTone2Ptr->dt));
	DPrint(wxT("\tBegin silence = %g ms,\tEnd silence = %g ms\n"),
	  MSEC(pureTone2Ptr->beginPeriodDuration),
	  MSEC(pureTone2Ptr->endPeriodDuration));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_PureTone_2(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PureTone_2");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	pureTone2Ptr = (PureTone2Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_2(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PureTone_2");

	if (!SetParsPointer_PureTone_2(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PureTone_2(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = pureTone2Ptr;
	theModule->Free = Free_PureTone_2;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_2;
	theModule->PrintPars = PrintPars_PureTone_2;
	theModule->RunProcess = GenerateSignal_PureTone_2;
	theModule->SetParsPointer = SetParsPointer_PureTone_2;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It checks that the Nyquist critical frequency is not exceeded.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckData_PureTone_2(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_PureTone_2");
	Float	criticalFrequency;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	criticalFrequency = 1.0 / (2.0 * pureTone2Ptr->dt);
	if (criticalFrequency <= pureTone2Ptr->frequency) {
		NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low for the ")
		  wxT("frequency."), funcName, MSEC(pureTone2Ptr->dt));
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
GenerateSignal_PureTone_2(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_PureTone_2");
	Float		totalDuration, amplitude;
	register	Float	dt, time, endSignal, startSignal;
	register	ChanData	*dataPtr;
	ChanLen		i;
	PureTone2Ptr	p = pureTone2Ptr;

	if (!data->threadRunFlag) {
		SetProcessName_EarObject(data, wxT("Silence pure-tone stimulus"));
		totalDuration = p->beginPeriodDuration + p->duration +
		  p->endPeriodDuration;
		if ( !InitOutSignal_EarObject(data, PURE_TONE_2_NUM_CHANNELS,
		  (ChanLen) floor(totalDuration / p->dt + 0.5), p->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	dt = _OutSig_EarObject(data)->dt;
	startSignal = p->beginPeriodDuration;
	endSignal = p->beginPeriodDuration + p->duration;
	amplitude = RMS_AMP(p->intensity) * SQRT_2;
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0; i < _OutSig_EarObject(data)->length; i++, dataPtr++) {
		time = (1 + i) * p->dt;
		if ( (time > startSignal) && (time < endSignal) )
			*dataPtr = amplitude * sin(PIx2 * p->frequency *
			  (time - p->beginPeriodDuration));
		else
			*dataPtr = 0.0;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone_2 */

