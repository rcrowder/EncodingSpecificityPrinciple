/**********************
 *
 * File:		MoANSGMeddis02.c
 * Purpose:		Auditory nerve spike generation program, based on vessicle
 *				release with a refractory time an recovery period.
 * Comments:	Written using ModuleProducer version 1.4.0 (Oct 10 2002).
 * Author:		L. P. O'Mard from a matlab routine from R. Meddis
 * Created:		08 Nov 2002
 * Updated:
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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
#include "FiParFile.h"
#include "UtRandom.h"
#include "UtParArray.h"
#include "UtANSGUtils.h"
#include "MoANSGMeddis02.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

Meddis02SGPtr	meddis02SGPtr = NULL;

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
Free_ANSpikeGen_Meddis02(void)
{
	if (meddis02SGPtr == NULL)
		return(FALSE);
	FreeProcessVariables_ANSpikeGen_Meddis02();
	Free_ANSGUtils(&meddis02SGPtr->aNDist);
	Free_ParArray(&meddis02SGPtr->distribution);
	if (meddis02SGPtr->diagnosticModeList)
		free(meddis02SGPtr->diagnosticModeList);
	if (meddis02SGPtr->parList)
		FreeList_UniParMgr(&meddis02SGPtr->parList);
	if (meddis02SGPtr->parSpec == GLOBAL) {
		free(meddis02SGPtr);
		meddis02SGPtr = NULL;
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
Init_ANSpikeGen_Meddis02(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_ANSpikeGen_Meddis02");

	if (parSpec == GLOBAL) {
		if (meddis02SGPtr != NULL)
			Free_ANSpikeGen_Meddis02();
		if ((meddis02SGPtr = (Meddis02SGPtr) malloc(sizeof(Meddis02SG))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (meddis02SGPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	meddis02SGPtr->parSpec = parSpec;
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	meddis02SGPtr->outputMode = ANSGUTILS_DISTRIBUTION_OUTPUTMODE_SQUARE_PULSE;
	meddis02SGPtr->ranSeed = 0;
	meddis02SGPtr->numFibres = 1;
	meddis02SGPtr->pulseDurationCoeff = -1.0;
	meddis02SGPtr->pulseMagnitude = 1.0;
	meddis02SGPtr->refractoryPeriod = 0.75e-3;
	meddis02SGPtr->recoveryTau = 0.8e-3;
	if ((meddis02SGPtr->distribution = Init_ParArray((WChar *) wxT("Distribution"),
	  ModeList_ANSGUtils(0), GetNumDistributionPars_ANSGUtils,
	  CheckFuncPars_ANSGUtils)) == NULL) {
		NotifyError(wxT("%s: Could not initialise distribution parArray structure"),
		  funcName);
		Free_ANSpikeGen_Meddis02();
		return(FALSE);
	}
	SetDefaultDistribution_ANSGUtils(meddis02SGPtr->distribution);

	if ((meddis02SGPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), meddis02SGPtr->diagFileName)) == NULL)
		return(FALSE);
	if (!SetUniParList_ANSpikeGen_Meddis02()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ANSpikeGen_Meddis02();
		return(FALSE);
	}
	meddis02SGPtr->pulse = NULL;
	meddis02SGPtr->pulseIndex = NULL;
	meddis02SGPtr->timerIndex = NULL;
	meddis02SGPtr->aNDist = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ANSpikeGen_Meddis02(void)
{
	static const WChar *funcName = wxT("SetUniParList_ANSpikeGen_Meddis02");
	UniParPtr	pars;

	if ((meddis02SGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_MEDDIS02_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = meddis02SGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_DIAGNOSTICMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostic mode ('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &meddis02SGPtr->diagnosticMode, meddis02SGPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode, 'square_pulse' or 'alpha_wave'."),
	  UNIPAR_NAME_SPEC,
	  &meddis02SGPtr->outputMode, OutputModeList_ANSGUtils(0),
	  (void * (*)) SetOutputMode_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 produces a different seed each run)."),
	  UNIPAR_LONG,
	  &meddis02SGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of parallel independent fibres contacting the same IHC."),
	  UNIPAR_INT,
	  &meddis02SGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_PULSEDURATION_COEFF], wxT(
	  "PULSE_DURATION_COEFF"),
	  wxT("Excitary post-synaptic pulse duration (s)."),
	  UNIPAR_REAL,
	  &meddis02SGPtr->pulseDurationCoeff, NULL,
	  (void * (*)) SetPulseDurationCoeff_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_PULSEMAGNITUDE], wxT(
	  "MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &meddis02SGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_REFRACTORYPERIOD],
	  wxT("REFRAC_PERIOD"),
	  wxT("Absolute refractory period (s)."),
	  UNIPAR_REAL,
	  &meddis02SGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_RECOVERYTAU], wxT(
	  "RECOVERY_TAU"),
	  wxT("Recovery time constant, tau (s)."),
	  UNIPAR_REAL,
	  &meddis02SGPtr->recoveryTau, NULL,
	  (void * (*)) SetRecoveryTau_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_DISTRIBUTION],
	  wxT("DISTRIBUTION"),
	  wxT("Channel fibre distribution mode ('standard', 'Gaussian' or ")
	  wxT("'dbl_Guassian')."),
	  UNIPAR_PARARRAY,
	  &meddis02SGPtr->distribution, NULL,
	  (void * (*)) SetDistribution_ANSpikeGen_Meddis02);

	meddis02SGPtr->parList->pars[ANSPIKEGEN_MEDDIS02_NUMFIBRES].enabled = FALSE;
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ANSpikeGen_Meddis02(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (meddis02SGPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(meddis02SGPtr->parList);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_ANSpikeGen_Meddis02(WChar * theDiagnosticMode)
{
	static const WChar	*funcName = wxT(
	  "SetDiagnosticMode_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	meddis02SGPtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  meddis02SGPtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_ANSpikeGen_Meddis02(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_ANSpikeGen_Meddis02");
	int		specifier;

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
	  OutputModeList_ANSGUtils(0))) == ANSGUTILS_DISTRIBUTION_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->outputMode = specifier;
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_ANSpikeGen_Meddis02(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetNumFibres **********************************/

/*
 * This function sets the module's numFibres parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFibres_ANSpikeGen_Meddis02(int theNumFibres)
{
	static const WChar	*funcName = wxT("SetNumFibres_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumFibres < 1) {
		NotifyError(wxT("%s: Illegal no. of fibres (%d)."), funcName,
		  theNumFibres);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->numFibres = theNumFibres;
	SetStandardNumFibres_ANSGUtils(meddis02SGPtr->distribution, theNumFibres);
	return(TRUE);

}

/****************************** SetPulseDurationCoeff ******************************/

/*
 * This function sets the module's pulseDurationCoeff parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDurationCoeff_ANSpikeGen_Meddis02(Float thePulseDurationCoeff)
{
	static const WChar	*funcName = wxT("SetPulseDurationCoeff_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->pulseDurationCoeff = thePulseDurationCoeff;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_ANSpikeGen_Meddis02(Float thePulseMagnitude)
{
	static const WChar	*funcName = wxT(
	  "SetPulseMagnitude_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/****************************** SetRefractoryPeriod ***************************/

/*
 * This function sets the module's refractoryPeriod parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefractoryPeriod_ANSpikeGen_Meddis02(Float theRefractoryPeriod)
{
	static const WChar	*funcName = wxT(
	  "SetRefractoryPeriod_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->refractoryPeriod = theRefractoryPeriod;
	return(TRUE);

}

/****************************** SetRecoveryTau ********************************/

/*
 * This function sets the module's recoveryTau parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRecoveryTau_ANSpikeGen_Meddis02(Float theRecoveryTau)
{
	static const WChar	*funcName = wxT("SetRecoveryTau_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->recoveryTau = theRecoveryTau;
	return(TRUE);

}

/****************************** SetDistribution *******************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDistribution_ANSpikeGen_Meddis02(ParArrayPtr theDistribution)
{
	static const WChar	*funcName = wxT("SetDistribution_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theDistribution, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."),  funcName);
		return(FALSE);
	}
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->distribution = theDistribution;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_ANSpikeGen_Meddis02(void)
{
	DPrint(wxT("Meddis 2002 post-synaptic firing module Parameters:-\n"));
	DPrint(wxT("\tDiagnostic mode = %s\n"), meddis02SGPtr->diagnosticModeList[
	  meddis02SGPtr->diagnosticMode].name);
	DPrint(wxT("\tOutput mode = %s\n"), OutputModeList_ANSGUtils(
	  meddis02SGPtr->outputMode)->name);
	DPrint(wxT("\tRandom number seed = %ld,"), meddis02SGPtr->ranSeed);
	DPrint(wxT("\tNumber of fibres = %d,\n"), meddis02SGPtr->numFibres);
	DPrint(wxT("\tPulse duration coefficient = "));
	if (meddis02SGPtr->pulseDurationCoeff > 0.0)
		DPrint(wxT("%g ms,"), MSEC(meddis02SGPtr->pulseDurationCoeff));
	else
		DPrint(wxT("< prev. signal dt>,"));
	DPrint(wxT("\tPulse magnitude = %g (nA?),\n"), meddis02SGPtr->
	  pulseMagnitude);
	DPrint(wxT("\tAbsolute refractory period = %g (s),"),
	  meddis02SGPtr->refractoryPeriod);
	DPrint(wxT("\tRecovery time constant, tau = %g (s)\n"),
	  meddis02SGPtr->recoveryTau);
	PrintPars_ParArray(meddis02SGPtr->distribution);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_ANSpikeGen_Meddis02(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_ANSpikeGen_Meddis02");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	meddis02SGPtr = (Meddis02SGPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_ANSpikeGen_Meddis02(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_ANSpikeGen_Meddis02");

	if (!SetParsPointer_ANSpikeGen_Meddis02(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Meddis02(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = meddis02SGPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_ANSpikeGen_Meddis02;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Meddis02;
	theModule->PrintPars = PrintPars_ANSpikeGen_Meddis02;
	theModule->ResetProcess = ResetProcess_ANSpikeGen_Meddis02;
	theModule->RunProcess = RunModel_ANSpikeGen_Meddis02;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Meddis02;
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
CheckData_ANSpikeGen_Meddis02(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_ANSpikeGen_Meddis02");
	Float	pulseDuration;
	Meddis02SGPtr	p = meddis02SGPtr;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	pulseDuration = CalcPulseDuration_ANSGUtils(p->outputMode, p->pulseDurationCoeff);
	if ((pulseDuration > 0.0) && (pulseDuration >= p->refractoryPeriod)) {
		NotifyError(wxT("%s: Pulse duration must be smaller than the\n")
		  wxT("refractory period, %g ms (%g ms)."), funcName, MSEC(
		  p->refractoryPeriod), MSEC(pulseDuration));
		return(FALSE);
	}
	if ((pulseDuration > 0.0) && (pulseDuration < _InSig_EarObject(data, 0)->dt)) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n")
		  wxT("interval, %g ms (%g ms)\n"), funcName,
		  MSEC(_InSig_EarObject(data, 0)->dt), MSEC(pulseDuration));
		return(FALSE);
	}
	if (!CheckPars_ParArray(p->distribution, _InSig_EarObject(data, 0))) {
		NotifyError(wxT("%s: Distribution parameters invalid."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_ANSpikeGen_Meddis02(EarObjectPtr data)
{
	int		chan;
	ChanLen	i, *pulseIndexPtr, *timerIndexPtr;
	Meddis02SGPtr	p = meddis02SGPtr;
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

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_ANSpikeGen_Meddis02(EarObjectPtr data)
{
	static const WChar	*funcName =
	  wxT("InitProcessVariables_ANSpikeGen_Meddis02");
	int		i;
	Float	pulseDurationCoeff;
	SignalDataPtr	outSignal;
	Meddis02SGPtr	p = meddis02SGPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Meddis02();
			outSignal = _OutSig_EarObject(data);
			p->dt = outSignal->dt;
			if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
				return(FALSE);
			if (!SetFibres_ANSGUtils(&p->aNDist, p->distribution, outSignal->info.cFArray,
			  outSignal->numChannels)) {
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
			pulseDurationCoeff = (p->pulseDurationCoeff > 0.0)?
			  p->pulseDurationCoeff: p->dt;
			p->pulseDurationIndex = CalcPulseDurationIndex_ANSGUtils(p->outputMode,
			  pulseDurationCoeff, outSignal->dt);
			if ((p->pulse = (GeneratePulse_ANSGUtils(p->outputMode,
			  p->pulseDurationIndex, pulseDurationCoeff, p->pulseMagnitude,
			  outSignal->dt))) == NULL) {
				NotifyError(wxT("%s: Out of memory for pulse array."),
				  funcName);
				return(FALSE);
			}
			p->refractoryPeriodIndex = (ChanLen) floor(p->refractoryPeriod /
			  outSignal->dt + 0.5);
			p->updateProcessVariablesFlag = FALSE;
		}
		ResetProcess_ANSpikeGen_Meddis02(data);
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_ANSpikeGen_Meddis02(void)
{
	int		i;
	Meddis02SGPtr	p = meddis02SGPtr;

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
	return(TRUE);

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
 */

BOOLN
RunModel_ANSpikeGen_Meddis02(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_ANSpikeGen_Meddis02");
	register	ChanData	*inPtr, *outPtr, *pulsePtr, *endPtr;
	register	ChanLen		*pulseIndexPtr;
	int		i, chan;
	Float	timer, spikeProb, excessTime;
	ChanLen	j, pulseDurationIndex, *timerIndexPtr;
	RandParsPtr		randParsPtr;
	SignalDataPtr	inSignal, outSignal;
	Meddis02SGPtr	p = meddis02SGPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_ANSpikeGen_Meddis02(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		inSignal = _InSig_EarObject(data, 0);
		SetProcessName_EarObject(data, wxT("Meddis 2002 Post-Synaptic Spike ")
		  wxT("Firing"));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels,
		  inSignal->length, inSignal->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Meddis02(data)) {
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
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = _OutSig_EarObject(data)->channel[chan];
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
				if ((*inPtr > 0.0) && (timer > p->refractoryPeriod)) {
					excessTime = timer - p->refractoryPeriod;
					spikeProb = 1.0 - exp(-excessTime / p->recoveryTau);
					if (spikeProb > Ran01_Random(randParsPtr)) {
						outPtr = outSignal->channel[chan] + (inPtr - inSignal->channel[chan]);
						pulsePtr = p->pulse;
						if ((inPtr + pulseDurationIndex) >= endPtr) {
							pulseDurationIndex = (ChanLen) (endPtr - inPtr);
							*pulseIndexPtr = pulseDurationIndex;
						}
						for (j = pulseDurationIndex; j; j--)
							*outPtr++ += *pulsePtr++;
						timer = p->refractoryPeriod + p->dt;
						inPtr += p->refractoryPeriodIndex;
						continue;
					}
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

}

