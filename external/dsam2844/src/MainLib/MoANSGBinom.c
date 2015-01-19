/**********************
 *
 * File:		MoANSGBinom.c
 * Purpose:		Uses spike probabilities to produce the output from a number of
 *				fibres, using a binomial approximation.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				This is an approximation, so it should only be used when theree
 *				are a large number of fibres.  If in doubt, test against the
 *				output from the "Simple" spike generation module.
 *				It uses the refractory adjustment model module.
 * Author:		L. P. O'Mard, revised from code by M. J. Hewitt
 * Created:		25 Jun 1996
 * Updated:		19 Feb 1997
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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
#include "MoANSGBinom.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BinomialSGPtr	binomialSGPtr = NULL;

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
Free_ANSpikeGen_Binomial(void)
{
	if (binomialSGPtr == NULL)
		return(FALSE);
	FreeProcessVariables_ANSpikeGen_Binomial();
	Free_ANSGUtils(&binomialSGPtr->aNDist);
	Free_ParArray(&binomialSGPtr->distribution);
	if (binomialSGPtr->diagnosticModeList)
		free(binomialSGPtr->diagnosticModeList);
	if (binomialSGPtr->parList)
		FreeList_UniParMgr(&binomialSGPtr->parList);
	if (binomialSGPtr->parSpec == GLOBAL) {
		free(binomialSGPtr);
		binomialSGPtr = NULL;
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
Init_ANSpikeGen_Binomial(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_ANSpikeGen_Binomial");

	if (parSpec == GLOBAL) {
		if (binomialSGPtr != NULL)
			Free_ANSpikeGen_Binomial();
		if ((binomialSGPtr = (BinomialSGPtr) malloc(sizeof(
		  BinomialSG))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (binomialSGPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	binomialSGPtr->parSpec = parSpec;
	binomialSGPtr->updateProcessVariablesFlag = TRUE;
	binomialSGPtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	binomialSGPtr->numFibres = 5000;
	binomialSGPtr->ranSeed = -1;
	binomialSGPtr->pulseDuration = 2e-05;
	binomialSGPtr->pulseMagnitude = 1.0;
	binomialSGPtr->refractoryPeriod = 1e-3;
	if ((binomialSGPtr->distribution = Init_ParArray((WChar *) wxT("Distribution"),
	  ModeList_ANSGUtils(0), GetNumDistributionPars_ANSGUtils,
	  CheckFuncPars_ANSGUtils)) == NULL) {
		NotifyError(wxT("%s: Could not initialise distribution parArray structure"),
		  funcName);
		Free_ANSpikeGen_Binomial();
		return(FALSE);
	}
	SetDefaultDistribution_ANSGUtils(binomialSGPtr->distribution);

	if ((binomialSGPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), binomialSGPtr->diagFileName)) == NULL)
		return(FALSE);
	if (!SetUniParList_ANSpikeGen_Binomial()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ANSpikeGen_Binomial();
		return(FALSE);
	}
	binomialSGPtr->lastOutput = NULL;
	binomialSGPtr->remainingPulseIndex = NULL;
	binomialSGPtr->refractAdjData = NULL;
	binomialSGPtr->aNDist = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ANSpikeGen_Binomial(void)
{
	static const WChar *funcName = wxT("SetUniParList_ANSpikeGen_Binom");
	UniParPtr	pars;

	if ((binomialSGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_BINOM_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = binomialSGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOMIAL_DIAGNOSTICMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostic mode ('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &binomialSGPtr->diagnosticMode, binomialSGPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of fibres."),
	  UNIPAR_INT,
	  &binomialSGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 for different seed for each run)."),
	  UNIPAR_LONG,
	  &binomialSGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_PULSEDURATION], wxT(
	  "PULSE_DURATION"),
	  wxT("Pulse duration (s)."),
	  UNIPAR_REAL,
	  &binomialSGPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_PULSEMAGNITUDE], wxT("MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &binomialSGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_REFRACTORYPERIOD], wxT(
	  "REFRAC_PERIOD"),
	  wxT("Refractory period (s)."),
	  UNIPAR_REAL,
	  &binomialSGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_DISTRIBUTION], wxT("DISTRIBUTION"),
	  wxT("Channel fibre distribution mode ('standard', 'Gaussian' or ")
	  wxT("'dbl_Guassian')."),
	  UNIPAR_PARARRAY,
	  &binomialSGPtr->distribution, NULL,
	  (void * (*)) SetDistribution_ANSpikeGen_Binomial);

	binomialSGPtr->parList->pars[ANSPIKEGEN_BINOM_NUMFIBRES].enabled = FALSE;
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ANSpikeGen_Binomial(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_ANSpikeGen_Binom");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (binomialSGPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(binomialSGPtr->parList);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_ANSpikeGen_Binomial(WChar * theDiagnosticMode)
{
	static const WChar	*funcName = wxT(
	  "SetDiagnosticMode_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	binomialSGPtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  binomialSGPtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetNumFibres **********************************/

/*
 * This function sets the module's numFibres parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFibres_ANSpikeGen_Binomial(int theNumFibres)
{
	static const WChar	*funcName = wxT("SetNumFibres_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	binomialSGPtr->numFibres = theNumFibres;
	SetStandardNumFibres_ANSGUtils(binomialSGPtr->distribution, theNumFibres);
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_ANSpikeGen_Binomial(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	binomialSGPtr->updateProcessVariablesFlag = TRUE;
	binomialSGPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_ANSpikeGen_Binomial(Float thePulseDuration)
{
	static const WChar	*funcName = wxT("SetPulseDuration_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePulseDuration < 0.0) {
		NotifyError(wxT("%s: Pulse duration must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	binomialSGPtr->updateProcessVariablesFlag = TRUE;
	binomialSGPtr->pulseDuration = thePulseDuration;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_ANSpikeGen_Binomial(Float thePulseMagnitude)
{
	static const WChar	*funcName = wxT(
	  "SetPulseMagnitude_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	binomialSGPtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/****************************** SetRefractoryPeriod ***************************/

/*
 * This function sets the module's refractoryPeriod parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefractoryPeriod_ANSpikeGen_Binomial(Float theRefractoryPeriod)
{
	static const WChar	*funcName =
	  wxT("SetRefractoryPeriod_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero ")
		  wxT("(%g ms)."), funcName, MSEC(theRefractoryPeriod));
		return(FALSE);
	}
	binomialSGPtr->refractoryPeriod = theRefractoryPeriod;
	return(TRUE);

}

/****************************** SetDistribution *******************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDistribution_ANSpikeGen_Binomial(ParArrayPtr theDistribution)
{
	static const WChar	*funcName = wxT("SetDistribution_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theDistribution, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."),  funcName);
		return(FALSE);
	}
	binomialSGPtr->updateProcessVariablesFlag = TRUE;
	binomialSGPtr->distribution = theDistribution;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_ANSpikeGen_Binomial(void)
{
	DPrint(wxT("Binomial Post-synaptic Firing Module:-\n"));
	DPrint(wxT("\tDiagnostic mode = %s\n"), binomialSGPtr->diagnosticModeList[
	  binomialSGPtr->diagnosticMode].name);
	DPrint(wxT("\tNo. fibres = %d,"), binomialSGPtr->numFibres);
	DPrint(wxT("\tRandom no. seed = %ld\n"), binomialSGPtr->ranSeed);
	DPrint(wxT("\tPulse duration = %g ms,"), MSEC(binomialSGPtr->
	  pulseDuration));
	DPrint(wxT("\tPulse magnitude = %g (nA?),\n"), binomialSGPtr->
	  pulseMagnitude);
	DPrint(wxT("\trefractoryPeriod = %g ms\n"), MSEC(binomialSGPtr->
	  refractoryPeriod));
	PrintPars_ParArray(binomialSGPtr->distribution);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_ANSpikeGen_Binomial(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_ANSpikeGen_Binomial");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	binomialSGPtr = (BinomialSGPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_ANSpikeGen_Binomial(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_ANSpikeGen_Binomial");

	if (!SetParsPointer_ANSpikeGen_Binomial(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Binomial(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = binomialSGPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_ANSpikeGen_Binomial;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Binomial;
	theModule->PrintPars = PrintPars_ANSpikeGen_Binomial;
	theModule->RunProcess = RunModel_ANSpikeGen_Binomial;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Binomial;
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
CheckData_ANSpikeGen_Binomial(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_ANSpikeGen_Binomial");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (binomialSGPtr->pulseDuration >= binomialSGPtr->refractoryPeriod) {
		NotifyError(wxT("%s: Pulse duration must be smaller than the\n")
		  wxT("refractory period, %g ms (%g ms)."), funcName,
		  MSEC(binomialSGPtr->refractoryPeriod),
		  MSEC(binomialSGPtr->pulseDuration));
		return(FALSE);
	}
	if (binomialSGPtr->pulseDuration < _InSig_EarObject(data, 0)->dt) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n")
		  wxT("interval, %g ms (%g ms)\n"), funcName, MSEC(_InSig_EarObject(
		  data, 0)->dt),
		  MSEC(binomialSGPtr->pulseDuration));
		return(FALSE);
	}
	if (!CheckPars_ParArray(binomialSGPtr->distribution, _InSig_EarObject(data, 0))) {
		NotifyError(wxT("%s: Distribution parameters invalid."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the 'refractAdjData' EarObject used in the to create the
 * data adjusted for refractory effects.
 */

BOOLN
InitProcessVariables_ANSpikeGen_Binomial(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_ANSpikeGen_Binomial");
	int		i;
	BinomialSGPtr	p = binomialSGPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_ANSpikeGen_Binomial();
		if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
			return(FALSE);
		if (!SetFibres_ANSGUtils(&p->aNDist, p->distribution, _OutSig_EarObject(
		  data)->info.cFArray, _OutSig_EarObject(data)->numChannels)) {
			NotifyError(wxT("%s: Could not initialise AN distribution."), funcName);
			return(FALSE);
		}
		if ((p->refractAdjData = Init_EarObject(wxT("Util_Refractory"))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for refractAdjData EarObject."),
			  funcName);
			return(FALSE);
		}
		if (!InitSubProcessList_EarObject(data,
		  ANSPIKEGEN_BINOM_NUM_SUB_PROCESSES)) {
			NotifyError(wxT("%s: Could not initialise %d sub-process list for ")
			  wxT("process."), funcName, ANSPIKEGEN_BINOM_NUM_SUB_PROCESSES);
			return(FALSE);
		}
		data->subProcessList[ANSPIKEGEN_REFRACTADJDATA] = p->refractAdjData;
		SetRealPar_ModuleMgr(p->refractAdjData, wxT("period"), p->
		  refractoryPeriod);
		if ((p->remainingPulseIndex = (ChanLen *) calloc(p->aNDist->numChannels, sizeof(
		  ChanLen))) == NULL) {
			NotifyError(wxT("%s: Out of memory for remainingPulseIndex array."),
			  funcName);
			return(FALSE);
		}
		if ((p->lastOutput = (Float *) calloc(p->aNDist->numChannels, sizeof(
		  Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for lastOutput array."),
			  funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < p->aNDist->numChannels; i++) {
			p->remainingPulseIndex[i] = 0;
			p->lastOutput[i] = 0.0;
		}
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_ANSpikeGen_Binomial(void)
{
	BinomialSGPtr	p = binomialSGPtr;

	if (p->refractAdjData)
		Free_EarObject(&p->refractAdjData);
	if (p->remainingPulseIndex != NULL) {
		free(p->remainingPulseIndex);
		p->remainingPulseIndex = NULL;
	}
	if (p->lastOutput != NULL) {
		free(p->lastOutput);
		p->lastOutput = NULL;
	}
	p->updateProcessVariablesFlag = TRUE;

}

/****************************** RunModel **************************************/

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
 * The 'refractAdjData' is manually connected to the input signal.
 */

BOOLN
RunModel_ANSpikeGen_Binomial(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_ANSpikeGen_Binomial");
	register	ChanData	*inPtr, *outPtr, *pulsePtr;
	register	Float		output = 0.0;
	int		chan;
	Float	*lastOutputPtr;
	ChanLen	i, pulseTimer = 0;
	ChanLen	*remainingPulseIndexPtr;
	ChanData	*pastEndOfData;
	EarObjectPtr	refractAdjData;
	RandParsPtr		randParsPtr;
	SignalDataPtr	outSignal, inSignal;
	BinomialSGPtr	p = binomialSGPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_ANSpikeGen_Binomial(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		inSignal = _InSig_EarObject(data, 0);
		SetProcessName_EarObject(data, wxT("Binomial Post-synaptic Firing"));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels,
		  inSignal->length, inSignal->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Binomial(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (p->diagnosticMode != GENERAL_DIAGNOSTIC_OFF_MODE) {
			OpenDiagnostics_NSpecLists(&p->fp, p->diagnosticModeList, p->diagnosticMode);
			PrintFibres_ANSGUtils(p->fp, wxT(""), p->aNDist->numFibres,
			  _OutSig_EarObject(data)->info.cFArray, p->aNDist->numChannels);
			CloseDiagnostics_NSpecLists(&p->fp);
		}
		TempInputConnection_EarObject(data, p->refractAdjData, 1);
		p->pulseDurationIndex = (ChanLen) (p->pulseDuration / _InSig_EarObject(
		  data, 0)->dt + 0.5);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	refractAdjData = data->subProcessList[ANSPIKEGEN_REFRACTADJDATA];
	RunProcessStandard_ModuleMgr(refractAdjData);
	remainingPulseIndexPtr = p->remainingPulseIndex + outSignal->offset;
	lastOutputPtr = p->lastOutput + outSignal->offset;
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++)
			*outPtr++ = 0.0;
		inPtr = refractAdjData->outSignal->channel[chan];
		randParsPtr = &data->randPars[chan];
		if (p->numFibres < 1)
			continue;
		outPtr = outSignal->channel[chan];
		pastEndOfData = outSignal->channel[chan] + outSignal->length;
		for (i = 0; i < outSignal->length; i++, outPtr++) {
			output = p->pulseMagnitude * GeomDist_Random(*inPtr++,
			  p->aNDist->numFibres[chan], randParsPtr);
			if (output > 0.0) {
				for (pulseTimer = p->pulseDurationIndex, pulsePtr = outPtr;
				  pulseTimer && (pulsePtr < pastEndOfData); pulsePtr++,
				  pulseTimer--) {
					*pulsePtr += output;
					if (*remainingPulseIndexPtr) {
						*pulsePtr += *lastOutputPtr;
						(*remainingPulseIndexPtr)--;
					}
				}
			}
		}
		*lastOutputPtr++ = output;
		*remainingPulseIndexPtr++ = pulseTimer;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

