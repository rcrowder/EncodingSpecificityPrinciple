/**********************
 *
 * File:		StHarmonic.c
 * Purpose:		This module contains the methods for the harmonic series
 *				stimulus.
 * Comments:	This was amended by Almudena to include FM and also a
 *				frequency domain butterworth filter options.
 *				16-07-98 LPO: Addit negative and postive Shroeder phase.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		16 Jul 1998
 * Copyright:	(c) 2007, 2010 Lowel P. O'Mard
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
#include <ctype.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "UtRandom.h"
#include "FiParFile.h"
#include "StHarmonic.h"
#include "UtString.h"
#include "UtFilters.h"

/********************************* Global variables ***************************/

HarmonicPtr	harmonicPtr = NULL;

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
Init_Harmonic(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("nit_Harmonic");

	if (parSpec == GLOBAL) {
		if (harmonicPtr != NULL)
			Free_Harmonic();
		if ((harmonicPtr = (HarmonicPtr) malloc(sizeof(Harmonic))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (harmonicPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	harmonicPtr->parSpec = parSpec;
	harmonicPtr->updateProcessVariablesFlag = TRUE;
	harmonicPtr->lowestHarmonic = 1;
	harmonicPtr->highestHarmonic = 10;
	harmonicPtr->mistunedHarmonic = -1;
	harmonicPtr->phaseMode = GENERAL_PHASE_SINE;
	harmonicPtr->mistuningFactor = 40.0;
	harmonicPtr->phaseVariable = 1.0;
	harmonicPtr->frequency = 100.0;
	harmonicPtr->intensity = DEFAULT_INTENSITY;
	harmonicPtr->duration = 0.1;
	harmonicPtr->dt = DEFAULT_DT;
	harmonicPtr->modulationFrequency = 1.0;
	harmonicPtr->modulationPhase = 0.0;
	harmonicPtr->modulationDepth = 0.0;
	harmonicPtr->order = 0;
	harmonicPtr->lowerCutOffFreq = 200.0;
	harmonicPtr->upperCutOffFreq = 600.0;

	if (!SetUniParList_Harmonic()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Harmonic();
		return(FALSE);
	}
	harmonicPtr->phase = NULL;
	harmonicPtr->modIndex = NULL;
	harmonicPtr->harmonicFrequency = NULL;
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
Free_Harmonic(void)
{
	if (harmonicPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Harmonic();
	if (harmonicPtr->parList)
		FreeList_UniParMgr(&harmonicPtr->parList);
	if (harmonicPtr->parSpec == GLOBAL) {
		free(harmonicPtr);
		harmonicPtr = NULL;
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
SetUniParList_Harmonic(void)
{
	static const WChar *funcName = wxT("SetUniParList_Harmonic");
	UniParPtr	pars;

	if ((harmonicPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  HARMONIC_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = harmonicPtr->parList->pars;
	SetPar_UniParMgr(&pars[HARMONIC_LOWESTHARMONIC], wxT("LOW_HARMONIC"),
	  wxT("Lowest harmonic number."),
	  UNIPAR_INT,
	  &harmonicPtr->lowestHarmonic, NULL,
	  (void * (*)) SetLowestHarmonic_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_HIGHESTHARMONIC], wxT("HIGH_HARMONIC"),
	  wxT("Highest harmonic number."),
	  UNIPAR_INT,
	  &harmonicPtr->highestHarmonic, NULL,
	  (void * (*)) SetHighestHarmonic_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_PHASEMODE], wxT("PHASE_MODE"),
	  wxT("Phase mode (ALTERNATING, COSINE, RANDOM, SCHROEDER, SINE, ")
	  wxT("PLACK_AND_WHITE, USER)."),
	  UNIPAR_NAME_SPEC,
	  &harmonicPtr->phaseMode, PhaseModeList_NSpecLists(0),
	  (void * (*)) SetPhaseMode_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_PHASE_PAR], wxT("PHASE_PAR"),
	  wxT("Phase parameter (Shroeder phase: C value, Random: random number ")
	    wxT("seed)."),
	  UNIPAR_REAL,
	  &harmonicPtr->phaseVariable, NULL,
	  (void * (*)) SetPhaseVariable_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MISTUNEDHARMONIC], wxT("MISTUNED_HARM"),
	  wxT("Mistuned harmonic number (0 = F0, -ve implies none mistuned)."),
	  UNIPAR_INT,
	  &harmonicPtr->mistunedHarmonic, NULL,
	  (void * (*)) SetMistunedHarmonic_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MISTUNINGFACTOR], wxT("MT_FACTOR"),
	  wxT("Mistuning factor (%)."),
	  UNIPAR_REAL,
	  &harmonicPtr->mistuningFactor, NULL,
	  (void * (*)) SetMistuningFactor_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_FREQUENCY], wxT("FUND_FREQ"),
	  wxT("Fundamental frequency (Hz)."),
	  UNIPAR_REAL,
	  &harmonicPtr->frequency, NULL,
	  (void * (*)) SetFrequency_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity per harmonic (dB SPL)."),
	  UNIPAR_REAL,
	  &harmonicPtr->intensity, NULL,
	  (void * (*)) SetIntensity_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &harmonicPtr->duration, NULL,
	  (void * (*)) SetDuration_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &harmonicPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MODULATIONFREQUENCY], wxT("MOD_FREQ"),
	  wxT("Modulation Frequency (Hz)."),
	  UNIPAR_REAL,
	  &harmonicPtr->modulationFrequency, NULL,
	  (void * (*)) SetModulationFrequency_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MODULATIONPHASE], wxT("MOD_PHASE"),
	  wxT("Modulation Phase (degrees)."),
	  UNIPAR_REAL,
	  &harmonicPtr->modulationPhase, NULL,
	  (void * (*)) SetModulationPhase_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MODULATIONDEPTH], wxT("MOD_DEPTH"),
	  wxT("Modulation depth (%)."),
	  UNIPAR_REAL,
	  &harmonicPtr->modulationDepth, NULL,
	  (void * (*)) SetModulationDepth_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_ORDER], wxT("ORDER"),
	  wxT("Filter order."),
	  UNIPAR_INT,
	  &harmonicPtr->order, NULL,
	  (void * (*)) SetOrder_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_LOWERCUTOFFFREQ], wxT("LOW_CUTOFF"),
	  wxT("Lower cut off frequency 3 dB down (Hz)."),
	  UNIPAR_REAL,
	  &harmonicPtr->lowerCutOffFreq, NULL,
	  (void * (*)) SetLowerCutOffFreq_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_UPPERCUTOFFFREQ], wxT("UPPER_CUTOFF"),
	  wxT("Upper cut off frequency 3 dB down (Hz)."),
	  UNIPAR_REAL,
	  &harmonicPtr->upperCutOffFreq, NULL,
	  (void * (*)) SetUpperCutOffFreq_Harmonic);

	SetEnabledPars_Harmonic();
	return(TRUE);

}

/********************************* SetEnabledPars ***********************/

/*
 * This routine sets the parameter list so that the correct parameters
 * are enabled/disabled.
 */

BOOLN
SetEnabledPars_Harmonic(void)
{
	static const WChar *funcName = wxT("SetEnabledPars_Harmonic");
	HarmonicPtr	p = harmonicPtr;

	if (p == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	switch (p->phaseMode) {
	case GENERAL_PHASE_RANDOM:
	case GENERAL_PHASE_SCHROEDER:
		p->parList->pars[HARMONIC_PHASE_PAR].enabled = TRUE;
		break;
	default:
		p->parList->pars[HARMONIC_PHASE_PAR].enabled = FALSE;
	}
	if (p->mistunedHarmonic > 0)
		p->parList->pars[HARMONIC_MISTUNINGFACTOR].enabled = TRUE;
	else
		p->parList->pars[HARMONIC_MISTUNINGFACTOR].enabled = FALSE;
	if (p->modulationDepth > DBL_EPSILON) {
		p->parList->pars[HARMONIC_MODULATIONFREQUENCY].enabled = TRUE;
		p->parList->pars[HARMONIC_MODULATIONPHASE].enabled = TRUE;
	} else {
		p->parList->pars[HARMONIC_MODULATIONFREQUENCY].enabled = FALSE;
		p->parList->pars[HARMONIC_MODULATIONPHASE].enabled = FALSE;
	}
	if (p->order > 0) {
		p->parList->pars[HARMONIC_LOWERCUTOFFFREQ].enabled = TRUE;
		p->parList->pars[HARMONIC_UPPERCUTOFFFREQ].enabled = TRUE;
	} else {
		p->parList->pars[HARMONIC_LOWERCUTOFFFREQ].enabled = FALSE;
		p->parList->pars[HARMONIC_UPPERCUTOFFFREQ].enabled = FALSE;
	}
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Harmonic(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (harmonicPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(harmonicPtr->parList);

}

/********************************* SetLowestHarmonic **************************/

/*
 * This function sets the module's lowest harmonic parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLowestHarmonic_Harmonic(int theLowestHarmonic)
{
	static const WChar *funcName = wxT("SetLowestHarmonic_Harmonic");

	if (theLowestHarmonic < 1) {
		NotifyError(wxT("%s: Lowest harmonic must be > 1 (%d)."), funcName,
		  theLowestHarmonic);
		return(FALSE);
	}
	harmonicPtr->lowestHarmonic = theLowestHarmonic;
	harmonicPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetHighestHarmonic *************************/

/*
 * This function sets the module's highest harmonic parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetHighestHarmonic_Harmonic(int theHighestHarmonic)
{
	harmonicPtr->highestHarmonic = theHighestHarmonic;
	harmonicPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMistunedHarmonic ************************/

/*
 * This function sets the module's mistuned harmonic parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMistunedHarmonic_Harmonic(int theMistunedHarmonic)
{

	harmonicPtr->mistunedHarmonic = theMistunedHarmonic;
	return(TRUE);

}

/********************************* SetPhaseMode *******************************/

/*
 * This function sets the module's phase mode parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
C */

BOOLN
SetPhaseMode_Harmonic(WChar *thePhaseMode)
{
	static const WChar *funcName = wxT("SetPhaseMode_Harmonic");
	int		specifier;

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(thePhaseMode,
	  PhaseModeList_NSpecLists(0))) == GENERAL_PHASE_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, thePhaseMode);
		return(FALSE);
	}
	harmonicPtr->phaseMode = specifier;
	SetEnabledPars_Harmonic();
	return(TRUE);

}

/********************************* SetMistuningFactor *************************/

/*
 * This function sets the module's mistuningFactor parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMistuningFactor_Harmonic(Float theMistuningFactor)
{

	harmonicPtr->mistuningFactor = theMistuningFactor;
	return(TRUE);

}

/****************************** SetPhaseVariable ******************************/

/*
 * This function sets the module's phase mode variable.
 * It first checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPhaseVariable_Harmonic(Float thePhaseVariable)
{

	harmonicPtr->updateProcessVariablesFlag = TRUE;
	harmonicPtr->phaseVariable = thePhaseVariable;
	return(TRUE);

}

/********************************* SetFrequency *******************************/

/*
 * This function sets the module's frequency parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequency_Harmonic(Float theFrequency)
{

	harmonicPtr->frequency = theFrequency;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_Harmonic(Float theIntensity)
{

	harmonicPtr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_Harmonic(Float theDuration)
{

	harmonicPtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_Harmonic(Float theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_Harmonic");

	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	harmonicPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetModulationFrequency ************************/

/*
 * This function sets the module's modulationFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationFrequency_Harmonic(Float theModulationFrequency)
{
	static const WChar	*funcName = wxT("SetModulationFrequency_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->modulationFrequency = theModulationFrequency;
	if (harmonicPtr->modulationFrequency == 0.0) {
		NotifyError(wxT("%s: The modulation frequency must be greater than ")
		  wxT("zero."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetModulationPhase ****************************/

/*
 * This function sets the module's modulationPhase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationPhase_Harmonic(Float theModulationPhase)
{
	static const WChar	*funcName = wxT("SetModulationPhase_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->modulationPhase = theModulationPhase;
	return(TRUE);

}

/****************************** SetModulationDepth ****************************/

/*
 * This function sets the module's modulationDepth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationDepth_Harmonic(Float theModulationDepth)
{
	static const WChar	*funcName = wxT("SetModulationDepth_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->modulationDepth = (theModulationDepth < DSAM_EPSILON)? 0.0:
	  theModulationDepth;
	return(TRUE);

}

/****************************** SetOrder **************************************/

/*
 * This function sets the module's order parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOrder_Harmonic(int theorder)
{
	static const WChar	*funcName = wxT("SetOrder_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (harmonicPtr->order < 0)	{
		NotifyError(wxT("%s: Filter order must be > 0."), funcName);
		return(FALSE);
	}
	harmonicPtr->order = theorder;
	return(TRUE);

}

/****************************** SetLowerCutOffFreq ****************************/

/*
 * This function sets the module's lowerCutOffFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowerCutOffFreq_Harmonic(Float theLowerCutOffFreq)
{
	static const WChar	*funcName = wxT("SetLowerCutOffFreq_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->lowerCutOffFreq = theLowerCutOffFreq;
	return(TRUE);

}

/****************************** SetUpperCutOffFreq ****************************/

/*
 * This function sets the module's upperCutOffFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUpperCutOffFreq_Harmonic(Float theUpperCutOffFreq)
{
	static const WChar	*funcName = wxT("SetUpperCutOffFreq_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->upperCutOffFreq = theUpperCutOffFreq;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_Harmonic(void)
{
	static const WChar *funcName = wxT("PrintPars_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Harmonic series Module Parameters:-\n"));
	DPrint(wxT("\tLowest/highest harmonic = %d / %d,\n"),
	  harmonicPtr->lowestHarmonic, harmonicPtr->highestHarmonic);
	DPrint(wxT("\tPhase mode = %s,\t"), PhaseModeList_NSpecLists(
	  harmonicPtr->phaseMode)->name);
	DPrint(wxT("Phase variable = %g"), harmonicPtr->phaseVariable);
	switch (harmonicPtr->phaseMode) {
	case GENERAL_PHASE_SCHROEDER:
		DPrint(wxT(" (C value)"));
		break;
	case GENERAL_PHASE_RANDOM:
		DPrint(wxT(" (random number seed)"));
		break;
	default:
		;
	}
	DPrint(wxT(",\n"));
	DPrint(wxT("\tMistuned harmonic = %d,\tmistuning factor = %g %%,\n"),
	  harmonicPtr->mistunedHarmonic, harmonicPtr->mistuningFactor);
	DPrint(wxT("\tFundamental frequency = %g Hz,\tIntensity = %g dB SPL,\n"),
	  harmonicPtr->frequency, harmonicPtr->intensity);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"), MSEC(
	  harmonicPtr->duration), MSEC(harmonicPtr->dt));
	if (harmonicPtr->modulationDepth > DSAM_EPSILON) {
		DPrint(wxT("\tModulation frequency = %g Hz\n"), harmonicPtr->
		  modulationFrequency);
		DPrint(wxT("\tModulation phase = %g degrees,"), harmonicPtr->
		  modulationPhase);
		DPrint(wxT("\tmodulationDepth = %g %%\n"), harmonicPtr->
		  modulationDepth);
	}
	if (harmonicPtr->order > 0) {
		DPrint(wxT("\tFilter order = %d \n"), harmonicPtr->order);
		DPrint(wxT("\tFilter low/high cut-off frequencys = %g / %g Hz\n"),
		  harmonicPtr->lowerCutOffFreq, harmonicPtr->upperCutOffFreq);
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Harmonic(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Harmonic");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	harmonicPtr = (HarmonicPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Harmonic(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Harmonic");

	if (!SetParsPointer_Harmonic(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Harmonic(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = harmonicPtr;
	theModule->Free = Free_Harmonic;
	theModule->GetUniParListPtr = GetUniParListPtr_Harmonic;
	theModule->PrintPars = PrintPars_Harmonic;
	theModule->RunProcess = GenerateSignal_Harmonic;
	theModule->SetParsPointer = SetParsPointer_Harmonic;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Harmonic(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Harmonic");
	int		totalNumberOfHarmonics;
	HarmonicPtr	p = harmonicPtr;

	totalNumberOfHarmonics = p->highestHarmonic - p->lowestHarmonic + 1;
	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Harmonic();
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
		if ((p->modIndex = (Float *) calloc(totalNumberOfHarmonics, sizeof(
		  Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'modIndex' array"),
			  funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Harmonic(void)
{
	if (harmonicPtr->phase) {
		free(harmonicPtr->phase);
		harmonicPtr->phase = NULL;
	}
	if (harmonicPtr->harmonicFrequency) {
		free(harmonicPtr->harmonicFrequency);
		harmonicPtr->harmonicFrequency = NULL;
	}
	if (harmonicPtr->modIndex) {
		free(harmonicPtr->modIndex);
		harmonicPtr->modIndex = NULL;
	}
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
CheckData_Harmonic(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_Harmonic");
	Float	criticalFrequency;

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if ((harmonicPtr->order > 0) && (harmonicPtr->lowestHarmonic >
	  harmonicPtr->highestHarmonic)) {
		NotifyError(wxT("%s: Illegal harmonic range (%d - %d)."), funcName,
		  harmonicPtr->lowestHarmonic, harmonicPtr->highestHarmonic);
		return(FALSE);
	}
	criticalFrequency = 1.0 / (2.0 * harmonicPtr->dt);
	if ((criticalFrequency <= harmonicPtr->frequency *
	   harmonicPtr->highestHarmonic)) {
		NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low for the ")
		  wxT("highest frequency."), funcName, MSEC(harmonicPtr->dt));
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
GenerateSignal_Harmonic(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_Harmonic");
	int			j, totalNumberOfHarmonics, harmonicNumber;
	ChanLen		i, t;
	Float		instantFreq;
	Float		amplitude, timexPix2, filterAmp, modulation;
	register	ChanData	*dataPtr;
	HarmonicPtr	p = harmonicPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Harmonic(data))
			return(FALSE);
		SetProcessName_EarObject(data, wxT("Harmonic series stimulus"));
		if ( !InitOutSignal_EarObject(data, HARMONIC_NUM_CHANNELS,
		  (ChanLen) floor(harmonicPtr->duration / harmonicPtr->dt + 0.5),
	    	harmonicPtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Harmonic(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	totalNumberOfHarmonics = p->highestHarmonic - p->lowestHarmonic + 1;
	SetPhaseArray_NSpecLists(p->phase, &p->ranSeed, data->randPars, p->phaseMode,
	  p->phaseVariable, p->lowestHarmonic, totalNumberOfHarmonics);
	for (j = 0; j < totalNumberOfHarmonics; j++) {
		harmonicNumber = p->lowestHarmonic + j;
		p->harmonicFrequency[j] = p->frequency * harmonicNumber;
		if (harmonicNumber == p->mistunedHarmonic)
			p->harmonicFrequency[j] += p->harmonicFrequency[j] *
			p->mistuningFactor / 100.0;
		p->modIndex[j] = (p->modulationDepth < DSAM_EPSILON)? 0.0:
		  (p->modulationDepth / 100.0) * (p->harmonicFrequency[j] / p->
		  modulationFrequency);
	}
	amplitude = RMS_AMP(harmonicPtr->intensity) * SQRT_2;
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->length; i++, t++,
	  dataPtr++) {
	  	timexPix2 = PIx2 * t * _OutSig_EarObject(data)->dt;
		for (j = 0, *dataPtr = 0.0; j < totalNumberOfHarmonics; j++) {
	  		modulation = p->modIndex[j] * cos(p->modulationFrequency * timexPix2 +
	  		  DEGREES_TO_RADS(p->modulationPhase));
			instantFreq = p->harmonicFrequency[j] + p->modIndex[j] *
			  p->modulationFrequency * sin(p->modulationFrequency * timexPix2 +
		  	  DEGREES_TO_RADS(p->modulationPhase));
			filterAmp = BandPassFD_Filters(instantFreq, p->lowerCutOffFreq,
			  p->upperCutOffFreq, p->order);
			*dataPtr += amplitude * filterAmp * sin(p->harmonicFrequency[j] *
			  timexPix2 + p->phase[j] - modulation);
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_Harmonic */

