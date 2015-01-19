/**********************
 *
 * File:		StFSHarmonic.c
 * Purpose:		Stimulus which consists of a harmonic series of untapered
 * 				linearly swept tones.
 * Comments:	Written using ModuleProducer version 1.5.0 (Feb 22 2007).
 * 				The stimulus formulation is taken from Aldridge, David F.,
 * 				“Mathematics of Linear Sweeps”, Canadian Journal of
 * 				Exploration Geophysics, (1992) Vol 28,  No. 1, p.62-69.
 * Author:		L. P. O'Mard
 * Created:		12 Mar 2007
 * Updated:
 * Copyright:	(c) 2010 Lowel P. O'Mard
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "UtRandom.h"
#include "FiParFile.h"
#include "StFSHarmonic.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FSHarmPtr	fSHarmPtr = NULL;

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
Free_Harmonic_FreqSwept(void)
{
	/* static const WChar	*funcName = wxT("Free_Harmonic_FreqSwept"); */

	if (fSHarmPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Harmonic_FreqSwept();
	if (fSHarmPtr->parList)
		FreeList_UniParMgr(&fSHarmPtr->parList);
	if (fSHarmPtr->parSpec == GLOBAL) {
		free(fSHarmPtr);
		fSHarmPtr = NULL;
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
Init_Harmonic_FreqSwept(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Harmonic_FreqSwept");

	if (parSpec == GLOBAL) {
		if (fSHarmPtr != NULL)
			Free_Harmonic_FreqSwept();
		if ((fSHarmPtr = (FSHarmPtr) malloc(sizeof(FSHarm))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fSHarmPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	fSHarmPtr->parSpec = parSpec;
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->lowestHarmonic = 1;
	fSHarmPtr->highestHarmonic = 10;
	fSHarmPtr->phaseMode = GENERAL_PHASE_COSINE;
	fSHarmPtr->ranSeed = 0;
	fSHarmPtr->phaseVariable = 1.0;
	fSHarmPtr->initialFreq = 200.0;
	fSHarmPtr->finalFreq = 400.0;
	fSHarmPtr->intensity = DEFAULT_INTENSITY;
	fSHarmPtr->duration = 0.1;
	fSHarmPtr->dt = DEFAULT_DT;

	if (!SetUniParList_Harmonic_FreqSwept()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Harmonic_FreqSwept();
		return(FALSE);
	}
	fSHarmPtr->phase = NULL;
	fSHarmPtr->harmonicFrequency = NULL;
	fSHarmPtr->sweepRate = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Harmonic_FreqSwept(void)
{
	static const WChar	*funcName = wxT("SetUniParList_Harmonic_FreqSwept");
	UniParPtr	pars;

	if ((fSHarmPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  HARMONIC_FREQSWEPT_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = fSHarmPtr->parList->pars;
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_LOWESTHARMONIC], wxT("LOW_HARMONIC"),
	  wxT("Lowest harmonic number."),
	  UNIPAR_INT,
	  &fSHarmPtr->lowestHarmonic, NULL,
	  (void * (*)) SetLowestHarmonic_Harmonic_FreqSwept);
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_HIGHESTHARMONIC], wxT("HIGH_HARMONIC"),
	  wxT("Highest harmonic number."),
	  UNIPAR_INT,
	  &fSHarmPtr->highestHarmonic, NULL,
	  (void * (*)) SetHighestHarmonic_Harmonic_FreqSwept);
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_PHASEMODE], wxT("PHASE_MODE"),
	  wxT("Phase mode (ALTERNATING, COSINE, RANDOM, SCHROEDER, SINE, ")
	  wxT("PLACK_AND_WHITE, USER)."),
	  UNIPAR_NAME_SPEC,
	  &fSHarmPtr->phaseMode, PhaseModeList_NSpecLists(0),
	  (void * (*)) SetPhaseMode_Harmonic_FreqSwept);
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_PHASEVARIABLE], wxT("PHASE_PAR"),
	  wxT("Phase parameter (Shroeder phase: C value, Random: random number ")
	    wxT("seed)."),
	  UNIPAR_REAL,
	  &fSHarmPtr->phaseVariable, NULL,
	  (void * (*)) SetPhaseVariable_Harmonic_FreqSwept);
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_INITIALFREQ], wxT("INITIAL_FREQ"),
	  wxT("Initial fundamental sweep frequency (Hz)."),
	  UNIPAR_REAL,
	  &fSHarmPtr->initialFreq, NULL,
	  (void * (*)) SetInitialFreq_Harmonic_FreqSwept);
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_FINALFREQ], wxT("FINAL_FREQ"),
	  wxT("Final fundamental sweep frequency (Hz)."),
	  UNIPAR_REAL,
	  &fSHarmPtr->finalFreq, NULL,
	  (void * (*)) SetFinalFreq_Harmonic_FreqSwept);
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity per harmonic (dB SPL)."),
	  UNIPAR_REAL,
	  &fSHarmPtr->intensity, NULL,
	  (void * (*)) SetIntensity_Harmonic_FreqSwept);
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &fSHarmPtr->duration, NULL,
	  (void * (*)) SetDuration_Harmonic_FreqSwept);
	SetPar_UniParMgr(&pars[HARMONIC_FREQSWEPT_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &fSHarmPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_Harmonic_FreqSwept);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Harmonic_FreqSwept(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (fSHarmPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(fSHarmPtr->parList);

}

/****************************** SetLowestHarmonic *****************************/

/*
 * This function sets the module's lowestHarmonic parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowestHarmonic_Harmonic_FreqSwept(int theLowestHarmonic)
{
	static const WChar	*funcName = wxT(
	  "SetLowestHarmonic_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theLowestHarmonic < 1) {
		NotifyError(wxT("%s: Lowest harmonic must be > 1 (%d)."), funcName,
		  theLowestHarmonic);
		return(FALSE);
	}
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->lowestHarmonic = theLowestHarmonic;
	return(TRUE);

}

/****************************** SetHighestHarmonic ****************************/

/*
 * This function sets the module's highestHarmonic parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetHighestHarmonic_Harmonic_FreqSwept(int theHighestHarmonic)
{
	static const WChar	*funcName = wxT(
	  "SetHighestHarmonic_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->highestHarmonic = theHighestHarmonic;
	return(TRUE);

}

/****************************** SetPhaseMode **********************************/

/*
 * This function sets the module's phaseMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPhaseMode_Harmonic_FreqSwept(WChar * thePhaseMode)
{
	static const WChar	*funcName = wxT("SetPhaseMode_Harmonic_FreqSwept");
	int		specifier;

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(thePhaseMode,
		PhaseModeList_NSpecLists(0))) == GENERAL_PHASE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, thePhaseMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSHarmPtr->phaseMode = specifier;
	return(TRUE);

}

/****************************** SetPhaseVariable ******************************/

/*
 * This function sets the module's phaseVariable parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPhaseVariable_Harmonic_FreqSwept(Float thePhaseVariable)
{
	static const WChar	*funcName = wxT("SetPhaseVariable_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->phaseVariable = thePhaseVariable;
	return(TRUE);

}

/****************************** SetInitialFreq ********************************/

/*
 * This function sets the module's initialFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialFreq_Harmonic_FreqSwept(Float theInitialFreq)
{
	static const WChar	*funcName = wxT("SetInitialFreq_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->initialFreq = theInitialFreq;
	return(TRUE);

}

/****************************** SetFinalFreq **********************************/

/*
 * This function sets the module's finalFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFinalFreq_Harmonic_FreqSwept(Float theFinalFreq)
{
	static const WChar	*funcName = wxT("SetFinalFreq_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->finalFreq = theFinalFreq;
	return(TRUE);

}

/****************************** SetIntensity **********************************/

/*
 * This function sets the module's intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIntensity_Harmonic_FreqSwept(Float theIntensity)
{
	static const WChar	*funcName = wxT("SetIntensity_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->intensity = theIntensity;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_Harmonic_FreqSwept(Float theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_Harmonic_FreqSwept(Float theSamplingInterval)
{
	static const WChar	*funcName = wxT(
	  "SetSamplingInterval_Harmonic_FreqSwept");

	if (fSHarmPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSHarmPtr->updateProcessVariablesFlag = TRUE;
	fSHarmPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Harmonic_FreqSwept(void)
{
	DPrint(wxT("Frequency Swept Harmonic Series Module Parameters:-\n"));
	DPrint(wxT("\tLowest/highest harmonic = %d / %d,\n"),
	  fSHarmPtr->lowestHarmonic, fSHarmPtr->highestHarmonic);
	DPrint(wxT("\tPhase mode = %s,\t"), PhaseModeList_NSpecLists(
	  fSHarmPtr->phaseMode)->name);
	DPrint(wxT("Phase variable = %g"), fSHarmPtr->phaseVariable);
	switch (fSHarmPtr->phaseMode) {
	case GENERAL_PHASE_SCHROEDER:
		DPrint(wxT(" (C value)"));
		break;
	case GENERAL_PHASE_RANDOM:
		DPrint(wxT(" (random number seed)"));
		break;
	default:
		;
	}
	DPrint(wxT("\n"));
	DPrint(wxT("\tInitial/final Frequency = %g/%g\n"), fSHarmPtr->initialFreq,
	  fSHarmPtr->finalFreq);
	DPrint(wxT("\tIntensity per harmonic = %g (dB SPL)\n"),
	  fSHarmPtr->intensity);
	DPrint(wxT("\tDuration = %g ms,"), MSEC(fSHarmPtr->duration));
	DPrint(wxT("\tSampling interval = %g ms.\n"), MSEC(fSHarmPtr->dt));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Harmonic_FreqSwept(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Harmonic_FreqSwept");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	fSHarmPtr = (FSHarmPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Harmonic_FreqSwept(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Harmonic_FreqSwept");

	if (!SetParsPointer_Harmonic_FreqSwept(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Harmonic_FreqSwept(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = fSHarmPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Harmonic_FreqSwept;
	theModule->GetUniParListPtr = GetUniParListPtr_Harmonic_FreqSwept;
	theModule->PrintPars = PrintPars_Harmonic_FreqSwept;
	theModule->RunProcess = GenerateSignal_Harmonic_FreqSwept;
	theModule->SetParsPointer = SetParsPointer_Harmonic_FreqSwept;
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
CheckData_Harmonic_FreqSwept(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Harmonic_FreqSwept");
	FSHarmPtr	p = fSHarmPtr;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (p->initialFreq > p->finalFreq) {
		NotifyError(wxT("%s: The initial frequency (%g Hz) must be less than ")
		  wxT("the final frequency (%g Hz)\n"), funcName, p->initialFreq,
		  p->finalFreq);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_Harmonic_FreqSwept(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_Harmonic_FreqSwept");
	int		totalNumberOfHarmonics;
	FSHarmPtr	p = fSHarmPtr;

	totalNumberOfHarmonics = p->highestHarmonic - p->lowestHarmonic + 1;
	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Harmonic_FreqSwept();
		if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
			return(FALSE);
		if ((p->phase = (Float *) calloc(totalNumberOfHarmonics, sizeof(
		  Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'phase' array"), funcName);
			return(FALSE);
		}
		if ((p->harmonicFrequency = (Float *) calloc(totalNumberOfHarmonics,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'harmonic frequencies' ")
			  wxT("array"), funcName);
			return(FALSE);
		}
		if ((p->sweepRate = (Float *) calloc(totalNumberOfHarmonics,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'sweep rate' ")
			  wxT("array"), funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		/*** Put reset (to zero ?) code here ***/
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Harmonic_FreqSwept(void)
{
	if (fSHarmPtr->phase) {
		free(fSHarmPtr->phase);
		fSHarmPtr->phase = NULL;
	}
	if (fSHarmPtr->harmonicFrequency) {
		free(fSHarmPtr->harmonicFrequency);
		fSHarmPtr->harmonicFrequency = NULL;
	}
	if (fSHarmPtr->sweepRate) {
		free(fSHarmPtr->sweepRate);
		fSHarmPtr->sweepRate = NULL;
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
GenerateSignal_Harmonic_FreqSwept(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("GenerateSignal_Harmonic_FreqSwept");
	register ChanData	 *outPtr;
	register 	Float		amplitude, timexPix2, time;
	int		j, totalNumberOfHarmonics, harmonicNumber;
	ChanLen	i, t;
	SignalDataPtr	outSignal;
	FSHarmPtr	p = fSHarmPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Harmonic_FreqSwept(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Frequency Swept Harmonic Series ")
		  wxT("Stimulus"));

		if (!InitOutSignal_EarObject(data, HARMONIC_FREQSWEPT_NUM_CHANNELS,
		  (ChanLen) floor(p->duration / p->dt + 0.5), p->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_Harmonic_FreqSwept(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	totalNumberOfHarmonics = p->highestHarmonic - p->lowestHarmonic + 1;
	SetPhaseArray_NSpecLists(p->phase, &p->ranSeed, data->randPars, p->phaseMode,
	  p->phaseVariable, p->lowestHarmonic, totalNumberOfHarmonics);
	for (j = 0; j < totalNumberOfHarmonics; j++) {
		harmonicNumber = p->lowestHarmonic + j;
		p->harmonicFrequency[j] = p->initialFreq * harmonicNumber;
		p->sweepRate[j] = (p->finalFreq - p->initialFreq) * harmonicNumber /
		  p->duration;
	}
	amplitude = RMS_AMP(p->intensity) * SQRT_2;
	outPtr = outSignal->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < outSignal->length; i++, t++,
	  outPtr++) {
	  	time = t * outSignal->dt;
	  	timexPix2 = PIx2 * time;
		for (j = 0, *outPtr = 0.0; j < totalNumberOfHarmonics; j++) {
			*outPtr += amplitude * sin(p->phase[j] + p->harmonicFrequency[j] *
			  timexPix2 + PI * p->sweepRate[j] * SQR(time));
		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

