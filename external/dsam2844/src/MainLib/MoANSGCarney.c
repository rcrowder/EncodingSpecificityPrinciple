/**********************
 *
 * File:		MoANSGCarney.c
 * Purpose:		Laurel H. Carney auditory nerve spike generation module:
 *				Carney L. H. (1993) "A model for the responses of low-frequency
 *				auditory-nerve fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				24-02-97 LPO: Amended to allow multiple fibres.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		7 May 1996
 * Updated:		24 Feb 1997
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
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtRandom.h"
#include "UtParArray.h"
#include "UtANSGUtils.h"
#include "MoANSGCarney.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CarneySGPtr	carneySGPtr = NULL;

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
Free_ANSpikeGen_Carney(void)
{
	if (carneySGPtr == NULL)
		return(FALSE);
	FreeProcessVariables_ANSpikeGen_Carney();
	Free_ANSGUtils(&carneySGPtr->aNDist);
	Free_ParArray(&carneySGPtr->distribution);
	if (carneySGPtr->diagnosticModeList)
		free(carneySGPtr->diagnosticModeList);
	if (carneySGPtr->parList)
		FreeList_UniParMgr(&carneySGPtr->parList);
	if (carneySGPtr->parSpec == GLOBAL) {
		free(carneySGPtr);
		carneySGPtr = NULL;
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
Init_ANSpikeGen_Carney(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_ANSpikeGen_Carney");

	if (parSpec == GLOBAL) {
		if (carneySGPtr != NULL)
			Free_ANSpikeGen_Carney();
		if ((carneySGPtr = (CarneySGPtr) malloc(sizeof(CarneySG))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (carneySGPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	carneySGPtr->parSpec = parSpec;
	carneySGPtr->updateProcessVariablesFlag = TRUE;
	carneySGPtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	carneySGPtr->inputMode = ANSPIKEGEN_CARNEY_INPUTMODE_ORIGINAL;
	carneySGPtr->outputMode = ANSGUTILS_DISTRIBUTION_OUTPUTMODE_SQUARE_PULSE;
	carneySGPtr->ranSeed = -1;
	carneySGPtr->numFibres = 1;
	carneySGPtr->pulseDurationCoeff = -1.0;
	carneySGPtr->pulseMagnitude = 1.0;
	carneySGPtr->refractoryPeriod = 0.75e-3;
	carneySGPtr->maxThreshold = 1.0;
	carneySGPtr->dischargeCoeffC0 = 0.5;
	carneySGPtr->dischargeCoeffC1 = 0.5;
	carneySGPtr->dischargeTConstS0 = 1e-3;
	carneySGPtr->dischargeTConstS1 = 12.5e-3;
	if ((carneySGPtr->distribution = Init_ParArray((WChar *) wxT("Distribution"),
	  ModeList_ANSGUtils(0), GetNumDistributionPars_ANSGUtils,
	  CheckFuncPars_ANSGUtils)) == NULL) {
		NotifyError(wxT("%s: Could not initialise distribution parArray structure"),
		  funcName);
		Free_ANSpikeGen_Carney();
		return(FALSE);
	}
	SetDefaultDistribution_ANSGUtils(carneySGPtr->distribution);

	if ((carneySGPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), carneySGPtr->diagFileName)) == NULL)
		return(FALSE);
	if (!SetUniParList_ANSpikeGen_Carney()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ANSpikeGen_Carney();
		return(FALSE);
	}
	carneySGPtr->pulse = NULL;
	carneySGPtr->pulseIndex = NULL;
	carneySGPtr->timerIndex = NULL;
	carneySGPtr->aNDist = NULL;
	return(TRUE);

}

/****************************** InputModeList *********************************/

/*
 * This function initialises the 'inputMode' list array
 */

NameSpecifier *
InputModeList_ANSpikeGen_Carney(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("CORRECTED"),	ANSPIKEGEN_CARNEY_INPUTMODE_CORRECTED },
			{ wxT("ORIGINAL"),	ANSPIKEGEN_CARNEY_INPUTMODE_ORIGINAL },
			{ wxT(""),			ANSPIKEGEN_CARNEY_INPUTMODE_NULL },
		};
	return(&modeList[index]);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ANSpikeGen_Carney(void)
{
	static const WChar *funcName = wxT("SetUniParList_ANSpikeGen_Carney");
	UniParPtr	pars;

	if ((carneySGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_CARNEY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = carneySGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DIAGNOSTICMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostic mode ('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &carneySGPtr->diagnosticMode, carneySGPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_INPUTMODE], wxT("INPUT_MODE"),
	  wxT("Input mode, 'corrected' (2001), or 'original' (1993) setting."),
	  UNIPAR_NAME_SPEC,
	  &carneySGPtr->inputMode, InputModeList_ANSpikeGen_Carney(0),
	  (void * (*)) SetInputMode_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode, 'square_pulse' or 'alpha_wave'."),
	  UNIPAR_NAME_SPEC,
	  &carneySGPtr->outputMode, OutputModeList_ANSGUtils(0),
	  (void * (*)) SetOutputMode_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 produces a different seed each run)."),
	  UNIPAR_LONG,
	  &carneySGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of fibres."),
	  UNIPAR_INT,
	  &carneySGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_PULSEDURATIONCOEFF], wxT(
	  "PULSE_DURATION_COEFF"),
	  wxT("Excitary post-synaptic pulse duration (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->pulseDurationCoeff, NULL,
	  (void * (*)) SetPulseDurationCoeff_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_PULSEMAGNITUDE], wxT("MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &carneySGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_REFRACTORYPERIOD], wxT(
	  "REFRAC_PERIOD"),
	  wxT("Refractory period (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_MAXTHRESHOLD], wxT(
	  "THRESHOLD_INC"),
	  wxT("Maximum threshold increase following discharge, Hmax (spikes/s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->maxThreshold, NULL,
	  (void * (*)) SetMaxThreshold_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISCHARGECOEFFC0], wxT("C0"),
	  wxT("Coefficient for discharge history effect, c0 (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->dischargeCoeffC0, NULL,
	  (void * (*)) SetDischargeCoeffC0_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISCHARGECOEFFC1], wxT("C1"),
	  wxT("Coefficient for discharge history effect, c1 (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->dischargeCoeffC1, NULL,
	  (void * (*)) SetDischargeCoeffC1_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISCHARGETCONSTS0], wxT("S0"),
	  wxT("Time constant for discharge history effect, s0 (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->dischargeTConstS0, NULL,
	  (void * (*)) SetDischargeTConstS0_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISCHARGETCONSTS1], wxT("S1"),
	  wxT("Time constant for discharge history effect, s1 (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->dischargeTConstS1, NULL,
	  (void * (*)) SetDischargeTConstS1_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISTRIBUTION], wxT("DISTRIBUTION"),
	  wxT("Channel fibre distribution mode ('standard', 'Gaussian' or ")
	  wxT("'dbl_Guassian')."),
	  UNIPAR_PARARRAY,
	  &carneySGPtr->distribution, NULL,
	  (void * (*)) SetDistribution_ANSpikeGen_Carney);

	carneySGPtr->parList->pars[ANSPIKEGEN_CARNEY_NUMFIBRES].enabled = FALSE;
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ANSpikeGen_Carney(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (carneySGPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(carneySGPtr->parList);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_ANSpikeGen_Carney(WChar * theDiagnosticMode)
{
	static const WChar	*funcName = wxT("SetDiagnosticMode_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	carneySGPtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  carneySGPtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetInputMode **********************************/

/*
 * This function sets the module's inputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInputMode_ANSpikeGen_Carney(WChar * theInputMode)
{
	static const WChar	*funcName = wxT("SetInputMode_ANSpikeGen_Carney");
	int		specifier;

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theInputMode,
	  InputModeList_ANSpikeGen_Carney(0))) == ANSPIKEGEN_CARNEY_INPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theInputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->inputMode = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_ANSpikeGen_Carney(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_ANSpikeGen_Carney");
	int		specifier;

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
	  OutputModeList_ANSGUtils(0))) == ANSGUTILS_DISTRIBUTION_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->updateProcessVariablesFlag = TRUE;
	carneySGPtr->outputMode = specifier;
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_ANSpikeGen_Carney(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	carneySGPtr->updateProcessVariablesFlag = TRUE;
	carneySGPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetNumFibres **********************************/

/*
 * This function sets the module's numFibres parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFibres_ANSpikeGen_Carney(int theNumFibres)
{
	static const WChar	*funcName = wxT("SetNumFibres_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumFibres < 1) {
		NotifyError(wxT("%s: Illegal no. of fibres (%d)."), funcName,
		  theNumFibres);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->numFibres = theNumFibres;
	SetStandardNumFibres_ANSGUtils(carneySGPtr->distribution, theNumFibres);
	return(TRUE);

}

/****************************** SetPulseDurationCoeff ******************************/

/*
 * This function sets the module's pulseDurationCoeff parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDurationCoeff_ANSpikeGen_Carney(Float thePulseDurationCoeff)
{
	static const WChar	*funcName = wxT("SetPulseDurationCoeff_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	carneySGPtr->pulseDurationCoeff = thePulseDurationCoeff;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_ANSpikeGen_Carney(Float thePulseMagnitude)
{
	static const WChar	*funcName = wxT("SetPulseMagnitude_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/****************************** SetRefractoryPeriod ***************************/

/*
 * This function sets the module's refractoryPeriod parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefractoryPeriod_ANSpikeGen_Carney(Float theRefractoryPeriod)
{
	static const WChar	*funcName = wxT(
	  "SetRefractoryPeriod_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	carneySGPtr->refractoryPeriod = theRefractoryPeriod;
	return(TRUE);

}

/****************************** SetMaxThreshold *******************************/

/*
 * This function sets the module's maxThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxThreshold_ANSpikeGen_Carney(Float theMaxThreshold)
{
	static const WChar	*funcName = wxT("SetMaxThreshold_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->maxThreshold = theMaxThreshold;
	return(TRUE);

}

/****************************** SetDischargeCoeffC0 ***************************/

/*
 * This function sets the module's dischargeCoeffC0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDischargeCoeffC0_ANSpikeGen_Carney(Float theDischargeCoeffC0)
{
	static const WChar	*funcName = wxT(
	  "SetDischargeCoeffC0_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->dischargeCoeffC0 = theDischargeCoeffC0;
	return(TRUE);

}

/****************************** SetDischargeCoeffC1 ***************************/

/*
 * This function sets the module's dischargeCoeffC1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDischargeCoeffC1_ANSpikeGen_Carney(Float theDischargeCoeffC1)
{
	static const WChar	*funcName = wxT(
	  "SetDischargeCoeffC1_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->dischargeCoeffC1 = theDischargeCoeffC1;
	return(TRUE);

}

/****************************** SetDischargeTConstS0 **************************/

/*
 * This function sets the module's dischargeTConstS0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDischargeTConstS0_ANSpikeGen_Carney(Float theDischargeTConstS0)
{
	static const WChar	*funcName = wxT(
	  "SetDischargeTConstS0_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->dischargeTConstS0 = theDischargeTConstS0;
	return(TRUE);

}

/****************************** SetDischargeTConstS1 **************************/

/*
 * This function sets the module's dischargeTConstS1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDischargeTConstS1_ANSpikeGen_Carney(Float theDischargeTConstS1)
{
	static const WChar	*funcName = wxT(
	  "SetDischargeTConstS1_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->dischargeTConstS1 = theDischargeTConstS1;
	return(TRUE);

}

/****************************** SetDistribution *******************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDistribution_ANSpikeGen_Carney(ParArrayPtr theDistribution)
{
	static const WChar	*funcName = wxT("SetDistribution_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theDistribution, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."),  funcName);
		return(FALSE);
	}
	carneySGPtr->updateProcessVariablesFlag = TRUE;
	carneySGPtr->distribution = theDistribution;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_ANSpikeGen_Carney(void)
{
	DPrint(wxT("Carney Post-Synaptic Firing Module Parameters:-\n"));
	DPrint(wxT("\tDiagnostic mode = %s\n"), carneySGPtr->diagnosticModeList[
	  carneySGPtr->diagnosticMode].name);
	DPrint(wxT("\tInput mode: %s,\n"), InputModeList_ANSpikeGen_Carney(
	  carneySGPtr->inputMode)->name);
	DPrint(wxT("\tOutput mode = %s\n"), OutputModeList_ANSGUtils(
	  carneySGPtr->outputMode)->name);
	DPrint(wxT("\tRandom number seed = %ld,"),
	  carneySGPtr->ranSeed);
	DPrint(wxT("\tNumber of fibres = %d,\n"), carneySGPtr->numFibres);
	DPrint(wxT("\tPulse duration = "));
	if (carneySGPtr->pulseDurationCoeff > 0.0)
		DPrint(wxT("%g ms,"), MSEC(carneySGPtr->pulseDurationCoeff));
	else
		DPrint(wxT("< prev. signal dt>,"));
	DPrint(wxT("\tPulse magnitude = %g (nA?)\n"), carneySGPtr->pulseMagnitude);
	DPrint(wxT("\tRefractory period, Ra = %g ms\n"), MSEC(
	  carneySGPtr->refractoryPeriod));
	DPrint(wxT("\tMax. threshold increase, Hmax = %g (spikes/s)\n"),
	  carneySGPtr->maxThreshold);
	DPrint(wxT("\tDischarge coefficent, C0 = %g,"), carneySGPtr->
	  dischargeCoeffC0);
	DPrint(wxT("\tDischarge coefficent, C1 = %g\n"), carneySGPtr->
	  dischargeCoeffC1);
	DPrint(wxT("\tDischarge time constant, S0 = %g ms,\n"), MSEC(
	  carneySGPtr->dischargeTConstS0));
	DPrint(wxT("\tDischarge time constant, S1 = %g ms.\n"), MSEC(
	  carneySGPtr->dischargeTConstS1));
	PrintPars_ParArray(carneySGPtr->distribution);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_ANSpikeGen_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_ANSpikeGen_Carney");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	carneySGPtr = (CarneySGPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_ANSpikeGen_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_ANSpikeGen_Carney");

	if (!SetParsPointer_ANSpikeGen_Carney(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Carney(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = carneySGPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_ANSpikeGen_Carney;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Carney;
	theModule->PrintPars = PrintPars_ANSpikeGen_Carney;
	theModule->ResetProcess = ResetProcess_ANSpikeGen_Carney;
	theModule->RunProcess = RunModel_ANSpikeGen_Carney;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Carney;
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
CheckData_ANSpikeGen_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_ANSpikeGen_Carney");
	Float	pulseDuration;
	CarneySGPtr	p = carneySGPtr;

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
	if ((pulseDuration > 0.0) && (pulseDuration < _InSig_EarObject(data,
	  0)->dt)) {
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
ResetProcess_ANSpikeGen_Carney(EarObjectPtr data)
{
	int		chan;
	ChanLen	i, *pulseIndexPtr, *timerIndexPtr;
	CarneySGPtr	p = carneySGPtr;
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
InitProcessVariables_ANSpikeGen_Carney(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_ANSpikeGen_Carney");
	int		i;
	Float	pulseDurationCoeff;
	SignalDataPtr	outSignal;
	CarneySGPtr	p = carneySGPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Carney();
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
			 		NotifyError(wxT("%s: Out of memory for pulseIndex ")
					  wxT("array."), funcName);
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
		ResetProcess_ANSpikeGen_Carney(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_ANSpikeGen_Carney(void)
{
	int		i;
	CarneySGPtr	p = carneySGPtr;

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
 * This code differs somewhat from Carney's because the 'timer' field records
 * the time from the last spike.
 */

BOOLN
RunModel_ANSpikeGen_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_ANSpikeGen_Carney");
	register	ChanData	*inPtr, *outPtr, *pulsePtr, *endPtr;
	register	ChanLen		*pulseIndexPtr;
	int		i, chan;
	Float	threshold, timer, excessTime;
	ChanLen	j, pulseDurationIndex, *timerIndexPtr;
	RandParsPtr		randParsPtr;
	SignalDataPtr	outSignal, inSignal;
	CarneySGPtr	p = carneySGPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_ANSpikeGen_Carney(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		inSignal = _InSig_EarObject(data, 0);
		SetProcessName_EarObject(data, wxT("Carney Post-Synaptic Spike ")
		  wxT("Firing"));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels,
		  inSignal->length, inSignal->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Carney(data)) {
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
				excessTime = timer - p->refractoryPeriod;
				threshold = p->maxThreshold * (p->dischargeCoeffC0 *
				  exp(-excessTime / p->dischargeTConstS0) +
				  p->dischargeCoeffC1 * exp(-excessTime /
				  p->dischargeTConstS1));
				if (((((p->inputMode ==
				  ANSPIKEGEN_CARNEY_INPUTMODE_ORIGINAL)? *inPtr / p->dt:
				  *inPtr) - threshold) * p->dt) > Ran01_Random(randParsPtr)) {
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
