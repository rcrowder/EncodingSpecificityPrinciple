/******************
 *
 * File:		MoANSGSimple.c
 * Purpose:		This module contains the model for the hair cell post-synaptic
 *				firing
 * Comments:	24-02-97 LPO: Amended to allow multiple fibres.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		24 Feb 1997
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
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtRandom.h"
#include "UtParArray.h"
#include "UtANSGUtils.h"
#include "MoANSGSimple.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

SimpleSGPtr		simpleSGPtr = NULL;

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
Init_ANSpikeGen_Simple(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_ANSpikeGen_Simple");
	if (parSpec == GLOBAL) {
		if (simpleSGPtr != NULL)
			Free_ANSpikeGen_Simple();
		if ((simpleSGPtr = (SimpleSGPtr) malloc(sizeof(SimpleSG))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (simpleSGPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	simpleSGPtr->parSpec = parSpec;
	simpleSGPtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	simpleSGPtr->outputMode = ANSGUTILS_DISTRIBUTION_OUTPUTMODE_SQUARE_PULSE;
	simpleSGPtr->ranSeed = -1;
	simpleSGPtr->numFibres = 5;
	simpleSGPtr->pulseDurationCoeff = 0.1e-3;
	simpleSGPtr->pulseMagnitude = 4.3;
	simpleSGPtr->refractoryPeriod = PS_REFRACTORY_PERIOD;
	if ((simpleSGPtr->distribution = Init_ParArray((WChar *) wxT("Distribution"),
	  ModeList_ANSGUtils(0), GetNumDistributionPars_ANSGUtils,
	  CheckFuncPars_ANSGUtils)) == NULL) {
		NotifyError(wxT("%s: Could not initialise distribution parArray structure"),
		  funcName);
		Free_ANSpikeGen_Simple();
		return(FALSE);
	}
	SetDefaultDistribution_ANSGUtils(simpleSGPtr->distribution);

	if ((simpleSGPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), simpleSGPtr->diagFileName)) == NULL)
		return(FALSE);
	if (!SetUniParList_ANSpikeGen_Simple()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ANSpikeGen_Simple();
		return(FALSE);
	}
	simpleSGPtr->pulse = NULL;
	simpleSGPtr->pulseIndex = NULL;
	simpleSGPtr->timerIndex = NULL;
	simpleSGPtr->aNDist = NULL;
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
Free_ANSpikeGen_Simple(void)
{
	if (simpleSGPtr == NULL)
		return(TRUE);
	FreeProcessVariables_ANSpikeGen_Simple();
	Free_ANSGUtils(&simpleSGPtr->aNDist);
	Free_ParArray(&simpleSGPtr->distribution);
	if (simpleSGPtr->diagnosticModeList)
		free(simpleSGPtr->diagnosticModeList);
	if (simpleSGPtr->parList)
		FreeList_UniParMgr(&simpleSGPtr->parList);
	if (simpleSGPtr->parSpec == GLOBAL) {
		free(simpleSGPtr);
		simpleSGPtr = NULL;
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
SetUniParList_ANSpikeGen_Simple(void)
{
	static const WChar *funcName = wxT("SetUniParList_ANSpikeGen_Simple");
	UniParPtr	pars;

	if ((simpleSGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_SIMPLE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = simpleSGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_DIAGNOSTICMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostic mode ('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &simpleSGPtr->diagnosticMode, simpleSGPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode, 'square_pulse' or 'alpha_wave'."),
	  UNIPAR_NAME_SPEC,
	  &simpleSGPtr->outputMode, OutputModeList_ANSGUtils(0),
	  (void * (*)) SetOutputMode_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 produces a different seed each run."),
	  UNIPAR_LONG,
	  &simpleSGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of fibres *** now obsolete, set by the 'DIST_MODE'***."),
	  UNIPAR_INT,
	  &simpleSGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_PULSEDURATIONCOEFF], wxT(
	  "PULSE_DURATION_COEFF"),
	  wxT("Pulse duration coefficient - square Pulse: duration or ")
	    wxT("alpha-wave: time to peak, Tau_E (s)."),
	  UNIPAR_REAL,
	  &simpleSGPtr->pulseDurationCoeff, NULL,
	  (void * (*)) SetPulseDurationCoeff_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_PULSEMAGNITUDE], wxT("MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &simpleSGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_REFRACTORYPERIOD], wxT(
	  "REFRAC_PERIOD"),
	  wxT("Refractory period (s)."),
	  UNIPAR_REAL,
	  &simpleSGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_DISTRIBUTION], wxT("DISTRIBUTION"),
	  wxT("Channel fibre distribution mode ('standard', 'Gaussian' or ")
	  wxT("'dbl_Guassian')."),
	  UNIPAR_PARARRAY,
	  &simpleSGPtr->distribution, NULL,
	  (void * (*)) SetDistribution_ANSpikeGen_Simple);

	simpleSGPtr->parList->pars[ANSPIKEGEN_SIMPLE_NUMFIBRES].enabled = FALSE;
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ANSpikeGen_Simple(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (simpleSGPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(simpleSGPtr->parList);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_ANSpikeGen_Simple(WChar * theDiagnosticMode)
{
	static const WChar	*funcName = wxT("SetDiagnosticMode_ANSpikeGen_Simple");
	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	simpleSGPtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  simpleSGPtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_ANSpikeGen_Simple(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_ANSpikeGen_Simple");
	int		specifier;

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
	  OutputModeList_ANSGUtils(0))) == ANSGUTILS_DISTRIBUTION_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	simpleSGPtr->outputMode = specifier;
	return(TRUE);

}

/********************************* SetRanSeed *********************************/

/*
 * This function sets the module's random number seed parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRanSeed_ANSpikeGen_Simple(long theRanSeed)
{
	static const WChar *funcName = wxT("SetRanSeed_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	simpleSGPtr->ranSeed = theRanSeed;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetNumFibres **********************************/

/*
 * This function sets the module's numFibres parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFibres_ANSpikeGen_Simple(int theNumFibres)
{
	static const WChar	*funcName = wxT("SetNumFibres_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumFibres < 1) {
		NotifyError(wxT("%s: Illegal no. of fibres (%d)."), funcName,
		  theNumFibres);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	simpleSGPtr->numFibres = theNumFibres;
	SetStandardNumFibres_ANSGUtils(simpleSGPtr->distribution, theNumFibres);
	return(TRUE);

}

/****************************** SetPulseDurationCoeff *************************/

/*
 * This function sets the module's pulseDurationCoeff parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDurationCoeff_ANSpikeGen_Simple(Float thePulseDurationCoeff)
{
	static const WChar	*funcName = wxT("SetPulseDurationCoeff_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePulseDurationCoeff < 0.0) {
		NotifyError(wxT("%s: Pulse duration must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	simpleSGPtr->pulseDurationCoeff = thePulseDurationCoeff;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_ANSpikeGen_Simple(Float thePulseMagnitude)
{
	static const WChar	*funcName = wxT("SetPulseMagnitude_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	simpleSGPtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/********************************* SetRefractoryPeriod ************************/

/*
 * This function sets the module's refractory period parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRefractoryPeriod_ANSpikeGen_Simple(Float theRefractoryPeriod)
{
	static const WChar *funcName = wxT("SetRefractoryPeriod_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	simpleSGPtr->refractoryPeriod = theRefractoryPeriod;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetDistribution ***********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDistribution_ANSpikeGen_Simple(ParArrayPtr theDistribution)
{
	static const WChar	*funcName = wxT("SetDistribution_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theDistribution, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."),  funcName);
		return(FALSE);
	}
	simpleSGPtr->distribution = theDistribution; /* This doesn't actually do anything new */
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_ANSpikeGen_Simple(void)
{
	DPrint(wxT("Simple Post-Synaptic Firing Module Parameters:-\n"));
	DPrint(wxT("\tDiagnostic mode = %s\n"), simpleSGPtr->diagnosticModeList[
	  simpleSGPtr->diagnosticMode].name);
	DPrint(wxT("\tOutput mode = %s\n"), OutputModeList_ANSGUtils(
	  simpleSGPtr->outputMode)->name);
	DPrint(wxT("\tRandom number seed = %ld,"), simpleSGPtr->ranSeed);
	DPrint(wxT("\tNo. of fibres = %d,\n"), simpleSGPtr->numFibres);
	DPrint(wxT("\tPulse duration coefficient = %g ms,"), MSEC(simpleSGPtr->
	  pulseDurationCoeff));
	DPrint(wxT("\tPulse magnitude = %g (nA?),\n"), simpleSGPtr->pulseMagnitude);
	DPrint(wxT("\tRefractory Period = %g ms\n"), MSEC(simpleSGPtr->
	  refractoryPeriod));
	PrintPars_ParArray(simpleSGPtr->distribution);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_ANSpikeGen_Simple(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_ANSpikeGen_Simple");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	simpleSGPtr = (SimpleSGPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_ANSpikeGen_Simple(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_ANSpikeGen_Simple");

	if (!SetParsPointer_ANSpikeGen_Simple(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Simple(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = simpleSGPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_ANSpikeGen_Simple;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Simple;
	theModule->PrintPars = PrintPars_ANSpikeGen_Simple;
	theModule->ResetProcess = ResetProcess_ANSpikeGen_Simple;
	theModule->RunProcess = RunModel_ANSpikeGen_Simple;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Simple;
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_ANSpikeGen_Simple(EarObjectPtr data)
{
	int		chan;
	ChanLen	i, *pulseIndexPtr, *timerIndexPtr;
	SimpleSGPtr	p = simpleSGPtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	ResetOutSignal_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		pulseIndexPtr = p->pulseIndex[chan];
		timerIndexPtr = p->timerIndex[chan];
		for (i = 0; i < (ChanLen) p->aNDist->numFibres[chan]; i++) {
			*pulseIndexPtr++ = 0;
			*timerIndexPtr++ = 0;
		}
	}

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_ANSpikeGen_Simple(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_ANSpikeGen_Simple");
	int		i;
	SignalDataPtr	outSignal;
	SimpleSGPtr	p = simpleSGPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Simple();
			outSignal = _OutSig_EarObject(data);
			if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
				return(FALSE);
			if (!SetFibres_ANSGUtils(&p->aNDist, p->distribution,
			  outSignal->info.cFArray, outSignal->numChannels)) {
				NotifyError(wxT("%s: Could not initialise AN distribution."), funcName);
				return(FALSE);
			}
			if ((p->pulseIndex = (ChanLen **) calloc(p->aNDist->numChannels,
			  sizeof(ChanLen *))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for pulseIndex ")
				  wxT("pointer array."), funcName);
			 	return(FALSE);
			}
			if ((p->timerIndex = (ChanLen **) calloc(p->aNDist->numChannels,
			  sizeof(ChanLen *))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for timerIndex ")
				  wxT("pointer array."), funcName);
			 	return(FALSE);
			}
			for (i = 0; i < p->aNDist->numChannels; i++) {
				if ((p->pulseIndex[i] = (ChanLen *) calloc(p->aNDist->numFibres[i],
				  sizeof(ChanLen))) == NULL) {
			 		NotifyError(wxT("%s: Out of memory for pulseIndex array."), funcName);
			 		return(FALSE);
				}
				if ((p->timerIndex[i] = (ChanLen *) calloc(p->aNDist->numFibres[i],
				  sizeof(ChanLen))) == NULL) {
			 		NotifyError(wxT("%s: Out of memory for timerIndex array."), funcName);
			 		return(FALSE);
				}
			}
			p->pulseDurationIndex = CalcPulseDurationIndex_ANSGUtils(p->outputMode,
			  p->pulseDurationCoeff, outSignal->dt);
			if ((p->pulse = (GeneratePulse_ANSGUtils(p->outputMode,
			  p->pulseDurationIndex, p->pulseDurationCoeff, p->pulseMagnitude,
			  outSignal->dt))) == NULL) {
				NotifyError(wxT("%s: Out of memory for pulse array."),
				  funcName);
				return(FALSE);
			}
			p->refractoryPeriodIndex = (ChanLen) floor(p->refractoryPeriod /
			  outSignal->dt + 0.5);
			p->updateProcessVariablesFlag = FALSE;
		}
		ResetProcess_ANSpikeGen_Simple(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_ANSpikeGen_Simple(void)
{
	int		i;
	SimpleSGPtr	p = simpleSGPtr;

	if (p->pulse) {
		free(p->pulse);
		p->pulse = NULL;
	}
	if (p->pulseIndex) {
		for (i = 0; i < p->aNDist->numChannels; i++)
			if (p->pulseIndex[i])
				free(p->pulseIndex[i]);
		free(p->pulseIndex);
		p->pulseIndex = NULL;
	}
	if (p->timerIndex) {
		for (i = 0; i < p->aNDist->numChannels; i++)
			if (p->timerIndex[i])
				free(p->timerIndex[i]);
		free(p->timerIndex);
		p->timerIndex = NULL;
	}
	p->updateProcessVariablesFlag = TRUE;

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
CheckData_ANSpikeGen_Simple(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_ANSpikeGen_Simple");
	Float	pulseDuration;
	SimpleSGPtr	p = simpleSGPtr;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	pulseDuration = CalcPulseDuration_ANSGUtils(p->outputMode, p->pulseDurationCoeff);
	if (pulseDuration >= p->refractoryPeriod) {
		NotifyError(wxT("%s: Pulse duration must be smaller than the\n")
		  wxT("refractory period, %g ms (%g ms)."), funcName, MSEC(
		  p->refractoryPeriod), MSEC(pulseDuration));
		return(FALSE);
	}
	if (pulseDuration < _InSig_EarObject(data, 0)->dt) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n")
		  wxT("interval, %g ms (%g ms)\n"), funcName, MSEC(_InSig_EarObject(
		  data, 0)->dt), MSEC(pulseDuration));
		return(FALSE);
	}
	if (!CheckPars_ParArray(p->distribution, _InSig_EarObject(data, 0))) {
		NotifyError(wxT("%s: Distribution parameters invalid."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

BOOLN
RunModel_ANSpikeGen_Simple(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_ANSpikeGen_Simple");
	register	ChanData	*inPtr, *outPtr, *pulsePtr, *endPtr;
	register	ChanLen		*pulseIndexPtr;
	int		i, chan;
	Float	timer;
	ChanLen	j, pulseDurationIndex, *timerIndexPtr;
	SignalDataPtr	outSignal, inSignal;
	RandParsPtr		randParsPtr;
	SimpleSGPtr	p = simpleSGPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_ANSpikeGen_Simple(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		inSignal = _InSig_EarObject(data, 0);
		SetProcessName_EarObject(data, wxT("Simple Post-Synaptic Spike ")
		  wxT("Firing"));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels, inSignal->length,
		  inSignal->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Simple(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (p->diagnosticMode != GENERAL_DIAGNOSTIC_OFF_MODE) {
			OpenDiagnostics_NSpecLists(&p->fp, p->diagnosticModeList, p->diagnosticMode);
			PrintFibres_ANSGUtils(p->fp, wxT(""), p->aNDist->numFibres,
			  _OutSig_EarObject(data)->info.cFArray, _OutSig_EarObject(data)->numChannels);
			CloseDiagnostics_NSpecLists(&p->fp);
		}
		p->dt = inSignal->dt;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		for (j = outSignal->length; j; j--)
			*outPtr++ = 0.0;
		pulseIndexPtr = p->pulseIndex[chan];
		timerIndexPtr = p->timerIndex[chan];
		for (i = 0; i < p->aNDist->numFibres[chan]; i++) {
			if (*pulseIndexPtr)
				AddRemainingPulse_ANSGUtils(outSignal->channel[chan], p->pulse,
				  pulseIndexPtr, p->pulseDurationIndex);
			timer = p->refractoryPeriod + p->dt;
			inPtr = inSignal->channel[chan] + *timerIndexPtr;
			endPtr = inSignal->channel[chan] + inSignal->length;
			randParsPtr = &data->randPars[chan];
			pulseDurationIndex = p->pulseDurationIndex;
			while (inPtr < endPtr) {
				if ((timer > p->refractoryPeriod) && (*inPtr > Ran01_Random(
				  randParsPtr))) {
					outPtr = outSignal->channel[chan] + (inPtr - inSignal->channel[chan]);
					pulsePtr = p->pulse;
					if ((inPtr + pulseDurationIndex) > endPtr) {
						pulseDurationIndex = (ChanLen) (endPtr - inPtr);
						*pulseIndexPtr = pulseDurationIndex;
					}
					for (j = pulseDurationIndex; j; j--)
						*outPtr++ += *pulsePtr++;
					timer = p->refractoryPeriod + p->dt;
					inPtr += p->refractoryPeriodIndex;
					continue;
				}
				timer += p->dt;
				inPtr++;
			}
			pulseIndexPtr++;
			*timerIndexPtr++ = (outPtr > outSignal->channel[chan])? p->refractoryPeriodIndex -
			  (ChanLen) (outSignal->channel[chan] + outSignal->length - (outPtr -
			  pulseDurationIndex)): 0;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* RunModel_ANSpikeGen_Simple */
