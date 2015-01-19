/**********************
 *
 * File:		StMPTone.c
 * Purpose:		This module contains the methods for the multiple pure-tone
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
#include "StMPTone.h"
#include "UtString.h"
#include "FiParFile.h"

/******************************************************************************/
/********************************* Global variables ***************************/
/******************************************************************************/

MPureTonePtr	mPureTonePtr = NULL;

/******************************************************************************/
/********************************* Subroutines and functions ******************/
/******************************************************************************/

/********************************* SetDefaultNumPTonesArrays ******************/

/*
 * This function sets the default arrays and array values.
 */

BOOLN
SetDefaultNumPTonesArrays_PureTone_Multi(void)
{
	static const WChar	*funcName = wxT(
	  "SetDefaultNumPTonesArrays_PureTone_Multi");
	int		i;
	Float	intensities[] = {DEFAULT_INTENSITY, DEFAULT_INTENSITY};
	Float	frequencies[] = {100.0, 200.0};
	Float	phases[] = {0.0, 0.0};

	if (!AllocNumPTones_PureTone_Multi(2)) {
		NotifyError(wxT("%s: Could not allocate default arrays."), funcName);
		return(FALSE);
	}
	for (i = 0; i < mPureTonePtr->numPTones; i++) {
		mPureTonePtr->intensities[i] = intensities[i];
		mPureTonePtr->frequencies[i] = frequencies[i];
		mPureTonePtr->phases[i] = phases[i];
	}
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
Init_PureTone_Multi(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_PureTone_Multi");

	if (parSpec == GLOBAL) {
		if (mPureTonePtr != NULL)
			free(mPureTonePtr);
		if ((mPureTonePtr = (MPureTonePtr) malloc(sizeof(MPureTone))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (mPureTonePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	mPureTonePtr->parSpec = parSpec;
	mPureTonePtr->numPTones = 0;
	mPureTonePtr->duration = 0.1;
	mPureTonePtr->dt = DEFAULT_DT;
	mPureTonePtr->intensities = NULL;
	mPureTonePtr->frequencies = NULL;
	mPureTonePtr->phases = NULL;

	if (!SetDefaultNumPTonesArrays_PureTone_Multi()) {
		NotifyError(wxT("%s: Could not set the default 'numPTones' arrays."),
		  funcName);
		return(FALSE);
	}

	if (!SetUniParList_PureTone_Multi()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PureTone_Multi();
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
Free_PureTone_Multi(void)
{
	if (mPureTonePtr == NULL)
		return(TRUE);
	free(mPureTonePtr->intensities);
	mPureTonePtr->intensities = NULL;
	free(mPureTonePtr->frequencies);
	mPureTonePtr->frequencies = NULL;
	free(mPureTonePtr->phases);
	mPureTonePtr->phases = NULL;
	if (mPureTonePtr->parList)
		FreeList_UniParMgr(&mPureTonePtr->parList);
	if (mPureTonePtr->parSpec == GLOBAL) {
		free(mPureTonePtr);
		mPureTonePtr = NULL;
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
SetUniParList_PureTone_Multi(void)
{
	static const WChar *funcName = wxT("SetUniParList_PureTone_Multi");
	UniParPtr	pars;

	if ((mPureTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_MULTI_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = mPureTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_MULTI_NUMPTONES], wxT("NUM"),
	  wxT("Number of pure tones."),
	  UNIPAR_INT_AL,
	  &mPureTonePtr->numPTones, NULL,
	  (void * (*)) SetNumPTones_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_FREQUENCIES], wxT("FREQUENCY"),
	  wxT("Pure tone frequencies (Hz)."),
	  UNIPAR_REAL_ARRAY,
	  &mPureTonePtr->frequencies, &mPureTonePtr->numPTones,
	  (void * (*)) SetIndividualFreq_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_INTENSITIES], wxT("INTENSITY"),
	  wxT("Pure tone intensities (dB SPL)."),
	  UNIPAR_REAL_ARRAY,
	  &mPureTonePtr->intensities, &mPureTonePtr->numPTones,
	  (void * (*)) SetIndividualIntensity_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_PHASES], wxT("PHASE"),
	  wxT("Pure tone phases (degrees)."),
	  UNIPAR_REAL_ARRAY,
	  &mPureTonePtr->phases, &mPureTonePtr->numPTones,
	  (void * (*)) SetIndividualPhase_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &mPureTonePtr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &mPureTonePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_Multi);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_Multi(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (mPureTonePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(mPureTonePtr->parList);

}

/********************************* SetNumPTones *******************************/

/*
 * This function sets the module's numPTones parameter.  It first checks that
 * the module has been initialised.
 * The 'numPTones' variable is set by the 'AllocNumPTones_PureTone_Multi'
 * routine.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNumPTones_PureTone_Multi(int theNumPTones)
{
	static const WChar *funcName = wxT("SetNumPTones_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumPTones < 1) {
		NotifyError(wxT("%s: Value must be greater then zero (%d)."), funcName,
		  theNumPTones);
		return(FALSE);
	}
	if (!AllocNumPTones_PureTone_Multi(theNumPTones)) {
		NotifyError(wxT("%s: Cannot allocate memory for numPTone arrays."),
		  funcName);
		return(FALSE);
 	}
	return(TRUE);

}

/********************************* SetFrequencies *****************************/

/*
 * This function sets up the module's frequencies array.
 * The pointer to the frequencies lists is passed to the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequencies_PureTone_Multi(Float *theFrequencies)
{
	static const WChar *funcName = wxT("SetFrequencies_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mPureTonePtr->frequencies = theFrequencies;
	return(TRUE);

}

/********************************* SetIntensities *****************************/

/*
 * This function sets up the module's intensities array.
 * The pointer to the intensities lists is passed to the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensities_PureTone_Multi(Float *theIntensities)
{
	static const WChar *funcName = wxT("SetIntensities_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mPureTonePtr->intensities = theIntensities;
	return(TRUE);

}

/********************************* SetPhases **********************************/

/*
 * This function sets up the module's phases array.
 * The pointer to the phases lists is passed to the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPhases_PureTone_Multi(Float *thePhases)
{
	static const WChar *funcName = wxT("SetPhases_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mPureTonePtr->phases = thePhases;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_PureTone_Multi(Float theDuration)
{
	static const WChar *funcName = wxT("SetDuration_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mPureTonePtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_PureTone_Multi(Float theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	mPureTonePtr->dt = theSamplingInterval;
	return(TRUE);

}

/********************************* SetIndividualFreq **************************/

/*
 * This function sets the individual frequency values.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualFreq_PureTone_Multi(int theIndex, Float theFrequency)
{
	static const WChar *funcName = wxT("SetIndividualFreq_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (mPureTonePtr->frequencies == NULL) {
		NotifyError(wxT("%s: Frequencies not set."), funcName);
		return(FALSE);
	}
	if (theIndex > mPureTonePtr->numPTones - 1) {
		NotifyError(wxT("%s: Index value must be in the\nrange 0 - %d (%d).\n"),
		  funcName, mPureTonePtr->numPTones - 1, theIndex);
		return(FALSE);
	}
	mPureTonePtr->frequencies[theIndex] = theFrequency;
	return(TRUE);

}

/********************************* SetIndividualIntensity *********************/

/*
 * This function sets the individual intensity values.
 * It first checks if the intensities have been set.
 */

BOOLN
SetIndividualIntensity_PureTone_Multi(int theIndex, Float theIntensity)
{
	static const WChar *funcName = wxT("SetIndividualIntensity_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (mPureTonePtr->intensities == NULL) {
		NotifyError(wxT("%s: Intensities not set."), funcName);
		return(FALSE);
	}
	if (theIndex > mPureTonePtr->numPTones - 1) {
		NotifyError(wxT("%s: Index value must be in the\nrange 0 - %d (%d).\n"),
		  funcName, mPureTonePtr->numPTones - 1,
		  theIndex);
		return(FALSE);
	}
	mPureTonePtr->intensities[theIndex] = theIntensity;
	return(TRUE);

}

/********************************* SetIndividualPhase *************************/

/*
 * This function sets the individual phase values.
 * It first checks if the phases have been set.
 */

BOOLN
SetIndividualPhase_PureTone_Multi(int theIndex, Float thePhase)
{
	static const WChar *funcName = wxT("SetIndividualPhase_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (mPureTonePtr->phases == NULL) {
		NotifyError(wxT("%s: Intensities not set."), funcName);
		return(FALSE);
	}
	if (theIndex > mPureTonePtr->numPTones - 1) {
		NotifyError(wxT("%s: Index value must be in the\nrange 0 - %d (%d).\n"),
		  funcName, mPureTonePtr->numPTones - 1, theIndex);
		return(FALSE);
	}
	mPureTonePtr->phases[theIndex] = thePhase;
	return(TRUE);

}

/****************************** GetIndividualFreq *****************************/

/*
 * This routine returns an individual frequency from the frequency list.
 */

Float
GetIndividualFreq_PureTone_Multi(int index)
{
	static const WChar *funcName = wxT("GetIndividualFreq_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (index < 0 || index >= mPureTonePtr->numPTones) {
		NotifyError(wxT("%s: The valid index range is 0 to %d.  Returning ")
		  wxT("zero.\n"), funcName, mPureTonePtr->numPTones - 1);
		return(0.0);
	}
	return(mPureTonePtr->frequencies[index]);

}

/****************************** GetIndividualIntensity ************************/

/*
 * This routine returns an individual intensity from the intensity list.
 */

Float
GetIndividualIntensity_PureTone_Multi(int index)
{
	static const WChar *funcName = wxT("GetIndividualIntensity_PureTone_Multi");

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (index < 0 || index >= mPureTonePtr->numPTones) {
		NotifyError(wxT("%s: The valid index range is 0 to %d.  Returning ")
		  wxT("zero.\n"), funcName, mPureTonePtr->numPTones - 1);
		return(0.0);
	}
	return(mPureTonePtr->intensities[index]);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_PureTone_Multi(void)
{
	static const WChar *funcName = wxT("PrintPars_PureTone_Multi");
	int		i;

	if (mPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Multiple Pure Tone Module Parameters:-\n"));
	DPrint(wxT("\t%10s\t%10s\t%10s\n"), wxT("Frequency "), wxT("Intensity"),
	  wxT("   Phase  "));
	DPrint(wxT("\t%10s\t%10s\t%10s\n"), wxT("   (Hz)   "), wxT(" (dB SPL)"),
	  wxT(" (degrees)"));
	for (i = 0; i < mPureTonePtr->numPTones; i++)
		DPrint(wxT("\t%10g\t%10g\t%10g\n"),
		  mPureTonePtr->frequencies[i], mPureTonePtr->intensities[i],
		  mPureTonePtr->phases[i]);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"),
	  MSEC(mPureTonePtr->duration), MSEC(mPureTonePtr->dt));
	return(TRUE);

}

/****************************** AllocNumPTones ********************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the specified numPTones is
 * the same as the current structure member value.
 * To make this work, it needs to set the numPTones parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumPTones_PureTone_Multi(int numPTones)
{
	static const WChar *funcName = wxT("AllocNumPTones_PureTone_Multi");

	if (numPTones == mPureTonePtr->numPTones)
		return(TRUE);
	if (mPureTonePtr->frequencies)
		free(mPureTonePtr->frequencies);
	if ((mPureTonePtr->frequencies = (Float *) calloc(numPTones, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for %d frequencies."),
		  funcName, numPTones);
		return(FALSE);
 	}
	if (mPureTonePtr->intensities)
		free(mPureTonePtr->intensities);
	if ((mPureTonePtr->intensities = (Float *) calloc(numPTones, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' intensities."),
		  funcName, numPTones);
		return(FALSE);
 	}
	if (mPureTonePtr->phases)
		free(mPureTonePtr->phases);
	if ((mPureTonePtr->phases = (Float *) calloc(numPTones, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' phases."), funcName,
		  numPTones);
		return(FALSE);
 	}
	mPureTonePtr->numPTones = numPTones;
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_PureTone_Multi(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PureTone_Multi");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	mPureTonePtr = (MPureTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_Multi(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PureTone_Multi");

	if (!SetParsPointer_PureTone_Multi(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PureTone_Multi(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = mPureTonePtr;
	theModule->Free = Free_PureTone_Multi;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_Multi;
	theModule->PrintPars = PrintPars_PureTone_Multi;
	theModule->RunProcess = GenerateSignal_PureTone_Multi;
	theModule->SetParsPointer = SetParsPointer_PureTone_Multi;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It also checks that the Nyquist critical frequency is not exceeded.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckData_PureTone_Multi(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_PureTone_Multi");
	int		i;
	Float	criticalFrequency;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	criticalFrequency = 1.0 / (2.0 * mPureTonePtr->dt);
	for (i = 0; i < mPureTonePtr->numPTones; i++)
		if (criticalFrequency <= mPureTonePtr->frequencies[i]) {
			NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low ")
			  wxT("for one of the stimulus\nfrequencies (frequency[%d] = ")
			  wxT("%g)."), funcName, MSEC(mPureTonePtr->dt), i,
			  mPureTonePtr->frequencies[i]);
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
GenerateSignal_PureTone_Multi(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_PureTone_Multi");
	int			j;
	ChanLen		i,t;
	register	Float		amplitude, phase;
	register	ChanData	*dataPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_PureTone_Multi(data))
			return(FALSE);
		SetProcessName_EarObject(data, wxT("Multiple pure tone stimulus"));
		data->updateProcessFlag = TRUE;	/* Ensure signal is set to zero. */
		if ( !InitOutSignal_EarObject(data, PURE_TONE_NUM_CHANNELS,
		  (ChanLen) floor(mPureTonePtr->duration / mPureTonePtr->dt + 0.5),
		  mPureTonePtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal."), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	ResetOutSignal_EarObject(data);
	for (j = 0; j < mPureTonePtr->numPTones; j++) {
		amplitude = RMS_AMP(mPureTonePtr->intensities[j]) * SQRT_2;
		phase = DEGREES_TO_RADS(mPureTonePtr->phases[j]);
		dataPtr = _OutSig_EarObject(data)->channel[0];
		for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->length; i++,
		  t++)
			*dataPtr++ += (ChanData) (amplitude * sin(PIx2 * mPureTonePtr->
			  frequencies[j] * (t * _OutSig_EarObject(data)->dt) + phase));
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone_Multi */

