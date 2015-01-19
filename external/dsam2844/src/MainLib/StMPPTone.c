/**********************
 *
 * File:		StMPPTone.c
 * Purpose:		The module generates a signal which contains multiple puretone
 *				pulses at different frequencies.
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
#include "StMPPTone.h"

/********************************* Global variables ***************************/

PureTone4Ptr	pureTone4Ptr = NULL;

/******************************************************************************/
/********************************* Subroutines and functions ******************/
/******************************************************************************/

/********************************* SetDefaultNumPulsesArrays ******************/

/*
 * This routine sets the default arrays and array values.
 */

BOOLN
SetDefaultNumPulsesArrays_PureTone_MultiPulse(void)
{
	static const WChar *funcName =
	  wxT("SetDefaultNumPulsesArrays_PureTone_MultiPulse");
	int		i;
	Float	frequencies[] = {1000.0, 2000.0};

	if (!AllocNumPulses_PureTone_MultiPulse(2)) {
		NotifyError(wxT("%s: Could not allocate default arrays."), funcName);
		return(FALSE);
	}
	for (i = 0; i < pureTone4Ptr->numPulses; i++)
		pureTone4Ptr->frequencies[i] = frequencies[i];
	return(TRUE);

}

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
Init_PureTone_MultiPulse(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_PureTone_MultiPulse");

	if (parSpec == GLOBAL) {
		if (pureTone4Ptr != NULL)
			Free_PureTone_MultiPulse();
		if ((pureTone4Ptr = (PureTone4Ptr) malloc(sizeof(PureTone4))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global'pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pureTone4Ptr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	pureTone4Ptr->parSpec = parSpec;
	pureTone4Ptr->numPulses = 0;
	pureTone4Ptr->intensity = DEFAULT_INTENSITY;
	pureTone4Ptr->duration = 0.08;
	pureTone4Ptr->dt = 0.1e-3;
	pureTone4Ptr->beginPeriodDuration = 0.01;
	pureTone4Ptr->pulseDuration = 5e-3;
	pureTone4Ptr->repetitionPeriod = 7e-3;
	pureTone4Ptr->frequencies = NULL;

	if (!SetDefaultNumPulsesArrays_PureTone_MultiPulse()) {
		NotifyError(wxT("%s: Could not set the default 'numPulses' arrays."),
		  funcName);
		return(FALSE);
	}

	if (!SetUniParList_PureTone_MultiPulse()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PureTone_MultiPulse();
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
Free_PureTone_MultiPulse(void)
{
	if (pureTone4Ptr == NULL)
		return(TRUE);
	free(pureTone4Ptr->frequencies);
	pureTone4Ptr->frequencies = NULL;
	if (pureTone4Ptr->parList)
		FreeList_UniParMgr(&pureTone4Ptr->parList);
	if (pureTone4Ptr->parSpec == GLOBAL) {
		free(pureTone4Ptr);
		pureTone4Ptr = NULL;
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
SetUniParList_PureTone_MultiPulse(void)
{
	static const WChar *funcName = wxT("SetUniParList_PureTone_Multi");
	UniParPtr	pars;

	if ((pureTone4Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_MULTIPULSE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = pureTone4Ptr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_NUMPULSES], wxT("NUM_PULSES"),
	  wxT("Number of Pulse Frequencies"),
	  UNIPAR_INT_AL,
	  &pureTone4Ptr->numPulses, NULL,
	  (void * (*)) SetNumPulses_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_FREQUENCIES], wxT("PULSE_FREQ"),
	  wxT("Pulse frequencies (Hz)."),
	  UNIPAR_REAL_ARRAY,
	  &pureTone4Ptr->frequencies, &pureTone4Ptr->numPulses,
	  (void * (*)) SetIndividualFreq_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &pureTone4Ptr->intensity, NULL,
	  (void * (*)) SetIntensity_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_BEGINPERIODDURATION], wxT(
	  "SILENCE"),
	  wxT("Silent period, before first pulse (s)."),
	  UNIPAR_REAL,
	  &pureTone4Ptr->beginPeriodDuration, NULL,
	  (void * (*)) SetBeginPeriodDuration_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_PULSEDURATION], wxT(
	  "PULSE_DURATION"),
	  wxT("Pulse duration (s)."),
	  UNIPAR_REAL,
	  &pureTone4Ptr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_REPETITIONPERIOD], wxT(
	  "REP_PERIOD"),
	  wxT("Repetition period (s)."),
	  UNIPAR_REAL,
	  &pureTone4Ptr->repetitionPeriod, NULL,
	  (void * (*)) SetRepetitionPeriod_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &pureTone4Ptr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Stimulus sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &pureTone4Ptr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_MultiPulse);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_MultiPulse(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_PureTone_Multi");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (pureTone4Ptr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(pureTone4Ptr->parList);

}

/****************************** AllocNumPulses ********************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the 'numPulses'
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numPulses'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumPulses_PureTone_MultiPulse(int numPulses)
{
	static const WChar	*funcName = wxT("AllocNumPulses_PureTone_MultiPulse");

	if (numPulses == pureTone4Ptr->numPulses)
		return(TRUE);
	if (pureTone4Ptr->frequencies)
		free(pureTone4Ptr->frequencies);
	if ((pureTone4Ptr->frequencies = (Float *) calloc(numPulses, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' frequencies."),
		  funcName, numPulses);
		return(FALSE);
	}
	pureTone4Ptr->numPulses = numPulses;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_PureTone_MultiPulse(Float theIntensity)
{
	static const WChar *funcName = wxT("SetIntensity_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone4Ptr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetFrequencies *****************************/

/*
 * This function sets up the module's frequencies array.
 * The pointer to the frequencies lists is passed to the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequencies_PureTone_MultiPulse(Float *theFrequencies)
{
	static const WChar *funcName = wxT("SetFrequencies_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone4Ptr->frequencies = theFrequencies;
	return(TRUE);

}

/****************************** SetNumPulses **********************************/

/*
 * This function sets the module's numPulses parameter.
 * It returns TRUE if the operation is successful.
 * The 'numPulses' variable is set by the 'AllocNumPulses_PureTone_MultiPulse'
 * routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumPulses_PureTone_MultiPulse(int theNumPulses)
{
	static const WChar	*funcName = wxT("SetNumPulses_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumPulses < 1) {
		NotifyError(wxT("%s: Value must be greater then zero (%d)."), funcName,
		  theNumPulses);
		return(FALSE);
	}
	if (!AllocNumPulses_PureTone_MultiPulse(theNumPulses)) {
		NotifyError(wxT("%%s: Cannot allocate memory for the 'numPulses' ")
		  wxT("arrays."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	return(TRUE);

}

/********************************* SetIndividualFreq **************************/

/*
 * This function sets the individual frequency values.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualFreq_PureTone_MultiPulse(int theIndex, Float theFrequency)
{
	static const WChar *funcName = wxT("SetIndividualFreq_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (pureTone4Ptr->frequencies == NULL) {
		NotifyError(wxT("%s: Frequencies not set."), funcName);
		return(FALSE);
	}
	if (theIndex > pureTone4Ptr->numPulses - 1) {
		NotifyError(wxT("%s: Index value must be in the\nrange 0 - %d (%d).\n"),
		  funcName, pureTone4Ptr->numPulses - 1, theIndex);
		return(FALSE);
	}
	pureTone4Ptr->frequencies[theIndex] = theFrequency;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_PureTone_MultiPulse(Float theDuration)
{
	static const WChar *funcName = wxT("SetDuration_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone4Ptr->duration = theDuration;
	return(TRUE);

}

/********************************* SetBeginPeriodDuration *********************/

/*
 * This function sets the module's beginPeriodDuration parameter.  It first
 * checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeginPeriodDuration_PureTone_MultiPulse(Float theBeginPeriodDuration)
{
	static const WChar *funcName = wxT(
	  "SetBeginPeriodDuration_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTone4Ptr->beginPeriodDuration = theBeginPeriodDuration;
	return(TRUE);

}

/********************************* SetPulseDuration ***************************/

/*
 * This function sets the module's pulseDuration parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPulseDuration_PureTone_MultiPulse(Float thePulseDuration)
{
	static const WChar *funcName = wxT("SetPulseDuration_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePulseDuration < 0.0) {
		NotifyError(wxT("%s: Illegal negative time!"), funcName);
		return(FALSE);
	}
	pureTone4Ptr->pulseDuration = thePulseDuration;
	return(TRUE);

}

/********************************* SetRepetitionPeriod ************************/

/*
 * This function sets the module's repetitionPeriod parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRepetitionPeriod_PureTone_MultiPulse(Float theRepetitionPeriod)
{
	static const WChar *funcName = wxT(
	  "SetRepetitionPeriod_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRepetitionPeriod < 0.0) {
		NotifyError(wxT("%s: Illegal negative time!"), funcName);
		return(FALSE);
	}
	pureTone4Ptr->repetitionPeriod = theRepetitionPeriod;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's timeStep parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_PureTone_MultiPulse(Float theSamplingInterval)
{
	static const WChar *funcName = wxT(
	  "SetSamplingInterval_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	pureTone4Ptr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** GetIndividualFreq *****************************/

/*
 * This routine returns an individual frequency from the frequency list.
 */

Float
GetIndividualFreq_PureTone_MultiPulse(int index)
{
	static const WChar *funcName = wxT("GetIndividualFreq_PureTone_MultiPulse");

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (index < 0 || index >= pureTone4Ptr->numPulses) {
		NotifyError(wxT("%s: The valid index range is 0 to %d.  Returning ")
		  wxT("zero.\n"), funcName, pureTone4Ptr->numPulses - 1);
		return(0.0);
	}
	return(pureTone4Ptr->frequencies[index]);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_PureTone_MultiPulse(void)
{
	static const WChar *funcName = wxT("PrintPars_PureTone_MultiPulse");
	int	i;

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Multi-Frequency Pulsed Pure Tone Module Parameters:-\n"));
	DPrint(wxT("\tIntensity = %g (Hz)\n"), pureTone4Ptr->intensity);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"),
	  MSEC(pureTone4Ptr->duration), MSEC(pureTone4Ptr->dt));
	DPrint(wxT("\t%10s\t%s\n"), wxT("Pulse No."), wxT("Frequency (Hz)"));
	for (i = 0; i < pureTone4Ptr->numPulses; i++)
		DPrint(wxT("\t%10d\t%10g\n"), i, pureTone4Ptr->frequencies[i]);
	DPrint(wxT("\tBegin silence = %g ms,\tPulse duration = %g ms\n"), MSEC(
	  pureTone4Ptr->beginPeriodDuration), MSEC(pureTone4Ptr->pulseDuration));
	DPrint(wxT("\tRepetition period = %g ms.\n"), MSEC(pureTone4Ptr->
	  repetitionPeriod));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_PureTone_MultiPulse(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PureTone_MultiPulse");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	pureTone4Ptr = (PureTone4Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_MultiPulse(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PureTone_MultiPulse");

	if (!SetParsPointer_PureTone_MultiPulse(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PureTone_MultiPulse(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = pureTone4Ptr;
	theModule->Free = Free_PureTone_MultiPulse;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_MultiPulse;
	theModule->PrintPars = PrintPars_PureTone_MultiPulse;
	theModule->RunProcess = GenerateSignal_PureTone_MultiPulse;
	theModule->SetParsPointer = SetParsPointer_PureTone_MultiPulse;
	return(TRUE);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It checks that the Nyquist critical intensity is not exceeded.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckData_PureTone_MultiPulse(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_PureTone_MultiPulse");
	int		i;
	Float	criticalFrequency;

	if (pureTone4Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if ( (pureTone4Ptr->pulseDuration > pureTone4Ptr->duration) ) {
	  	NotifyError(wxT("%s: The pulse duration  is longer than the signal ")
		  wxT("duration."), funcName);
		return(FALSE);
	}
	if ( (pureTone4Ptr->pulseDuration > pureTone4Ptr->repetitionPeriod) ) {
	  	NotifyError(wxT("%s: The pulse duration  is longer than the ")
		  wxT("repetition period."), funcName);
		return(FALSE);
	}
	criticalFrequency = 1.0 / (2.0 * pureTone4Ptr->dt);
	for (i = 0; i < pureTone4Ptr->numPulses; i++)
		if (criticalFrequency <= pureTone4Ptr->frequencies[i]) {
			NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low ")
			  wxT("for frequency No. %d."), funcName, MSEC(pureTone4Ptr->dt), i);
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
GenerateSignal_PureTone_MultiPulse(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_PureTone_MultiPulse");
	Float		amplitude;
	register	ChanData	*dataPtr;
	ChanLen		i, t, pulseDurationIndex, repetitionPeriodIndex;

	if (!data->threadRunFlag) {
		if (!CheckData_PureTone_MultiPulse(data))
			return(FALSE);
		SetProcessName_EarObject(data, wxT("Multi-frequency pulsed pure-tone ")
		  wxT("stimulus"));
		if ( !InitOutSignal_EarObject(data, PURE_TONE_3_NUM_CHANNELS,
		  (ChanLen) (pureTone4Ptr->duration / pureTone4Ptr->dt + 0.5),
		  pureTone4Ptr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	amplitude = RMS_AMP(pureTone4Ptr->intensity) * SQRT_2;
	pulseDurationIndex = (ChanLen) floor(pureTone4Ptr->pulseDuration /
	  pureTone4Ptr->dt + 0.5);
	repetitionPeriodIndex = (ChanLen) floor(pureTone4Ptr->repetitionPeriod /
	  pureTone4Ptr->dt + 0.5);
	if (data->timeIndex == PROCESS_START_TIME) {
		pureTone4Ptr->beginIndex = (ChanLen) floor(pureTone4Ptr->
		  beginPeriodDuration / _OutSig_EarObject(data)->dt + 0.5);
		pureTone4Ptr->pulseOn = TRUE;
		pureTone4Ptr->pulseNumber = 0;
		pureTone4Ptr->pulseCount = 0;
	}
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->length;
	  i++, t++, dataPtr++) {
	  	if (t < pureTone4Ptr->beginIndex)
	  		continue;
		if (pureTone4Ptr->pulseOn) {
			*dataPtr = amplitude * sin(PIx2 *
			  pureTone4Ptr->frequencies[pureTone4Ptr->pulseNumber] *
			  (pureTone4Ptr->pulseCount++ * _OutSig_EarObject(data)->dt));
			pureTone4Ptr->pulseOn = (pureTone4Ptr->pulseCount <
			  pulseDurationIndex);
		} else {
			*dataPtr = 0.0;
			if (((t - pureTone4Ptr->beginIndex) % repetitionPeriodIndex) == 1) {
				pureTone4Ptr->pulseOn = TRUE;
				pureTone4Ptr->pulseCount = 0;
				pureTone4Ptr->pulseNumber++;
				pureTone4Ptr->pulseNumber %= pureTone4Ptr->numPulses;
			}
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone_MultiPulse */

