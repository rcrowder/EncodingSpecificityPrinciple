/**********************
 *
 * File:		MoIHCCarney.c
 * Purpose:		Laurel H. Carney IHC synapse module: Carney L. H. (1993)
 *				"A model for the responses of low-frequency auditory-nerve
 *				fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		30 April 1996
 * Updated:
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
#include <float.h>

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
#include "MoIHCCarney.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CarneyHCPtr	carneyHCPtr = NULL;

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
Free_IHC_Carney(void)
{
	if (carneyHCPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHC_Carney();
	if (carneyHCPtr->parList)
		FreeList_UniParMgr(&carneyHCPtr->parList);
	if (carneyHCPtr->parSpec == GLOBAL) {
		free(carneyHCPtr);
		carneyHCPtr = NULL;
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
Init_IHC_Carney(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IHC_Carney");

	if (parSpec == GLOBAL) {
		if (carneyHCPtr != NULL)
			Free_IHC_Carney();
		if ((carneyHCPtr = (CarneyHCPtr) malloc(sizeof(CarneyHC))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (carneyHCPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	carneyHCPtr->parSpec = parSpec;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxHCVoltage = 10.0;
	carneyHCPtr->restingReleaseRate = 70.0;
	carneyHCPtr->restingPerm = 0.015;
	carneyHCPtr->maxGlobalPerm = 0.08;
	carneyHCPtr->maxLocalPerm = 0.1;
	carneyHCPtr->maxImmediatePerm = 1.5	;
	carneyHCPtr->maxLocalVolume = 0.003;
	carneyHCPtr->minLocalVolume = 0.001;
	carneyHCPtr->maxImmediateVolume = 0.0003;
	carneyHCPtr->minImmediateVolume = 0.0001;

	if (!SetUniParList_IHC_Carney()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHC_Carney();
		return(FALSE);
	}
	carneyHCPtr->hCChannels = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHC_Carney(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHC_Carney");
	UniParPtr	pars;

	if ((carneyHCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_CARNEY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = carneyHCPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXHCVOLTAGE], wxT("V_MAX"),
	  wxT("Maximum depolarising hair cell voltage, Vmax (V)."),
	  UNIPAR_REAL,
	  &carneyHCPtr->maxHCVoltage, NULL,
	  (void * (*)) SetMaxHCVoltage_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_RESTINGRELEASERATE], wxT("R0"),
	  wxT("Resting release from synapse, R0 (spikes/s)."),
	  UNIPAR_REAL,
	  &carneyHCPtr->restingReleaseRate, NULL,
	  (void * (*)) SetRestingReleaseRate_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_RESTINGPERM], wxT("P_REST"),
	  wxT("Resting permeability, Prest ('volume'/s)."),
	  UNIPAR_REAL,
	  &carneyHCPtr->restingPerm, NULL,
	  (void * (*)) SetRestingPerm_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXGLOBALPERM], wxT("PG_MAX"),
	  wxT("Maximum global permeability, PGmax ('volume'/s)."),
	  UNIPAR_REAL,
	  &carneyHCPtr->maxGlobalPerm, NULL,
	  (void * (*)) SetMaxGlobalPerm_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXLOCALPERM], wxT("PL_MAX"),
	  wxT("Maximum local permeability, PLmax ('volume'/s)."),
	  UNIPAR_REAL,
	  &carneyHCPtr->maxLocalPerm, NULL,
	  (void * (*)) SetMaxLocalPerm_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXIMMEDIATEPERM], wxT("PI_MAX"),
	  wxT("Maximum immediate permeability, PImax ('volume'/s)."),
	  UNIPAR_REAL,
	  &carneyHCPtr->maxImmediatePerm, NULL,
	  (void * (*)) SetMaxImmediatePerm_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXLOCALVOLUME], wxT("VL_MAX"),
	  wxT("Maximum local volume, VLmax ('volume')."),
	  UNIPAR_REAL,
	  &carneyHCPtr->maxLocalVolume, NULL,
	  (void * (*)) SetMaxLocalVolume_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MINLOCALVOLUME], wxT("VL_MIN"),
	  wxT("Minimum local volume, VLmin ('volume')."),
	  UNIPAR_REAL,
	  &carneyHCPtr->minLocalVolume, NULL,
	  (void * (*)) SetMinLocalVolume_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXIMMEDIATEVOLUME], wxT("VI_MAX"),
	  wxT("Maximum immediate volume, VImax ('volume')."),
	  UNIPAR_REAL,
	  &carneyHCPtr->maxImmediateVolume, NULL,
	  (void * (*)) SetMaxImmediateVolume_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MINIMMEDIATEVOLUME], wxT("VI_MIN"),
	  wxT("Minimum immediate volume, VImin ('volume')."),
	  UNIPAR_REAL,
	  &carneyHCPtr->minImmediateVolume, NULL,
	  (void * (*)) SetMinImmediateVolume_IHC_Carney);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_Carney(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (carneyHCPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(carneyHCPtr->parList);

}

/****************************** SetMaxHCVoltage *******************************/

/*
 * This function sets the module's maxHCVoltage parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxHCVoltage_IHC_Carney(Float theMaxHCVoltage)
{
	static const WChar	*funcName = wxT("SetMaxHCVoltage_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (fabs(theMaxHCVoltage) < DSAM_EPSILON) {
		NotifyError(wxT("%s: Illegal value (%g V)."), theMaxHCVoltage);
		return(FALSE);
	}
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxHCVoltage = theMaxHCVoltage;
	return(TRUE);

}

/****************************** SetRestingReleaseRate *************************/

/*
 * This function sets the module's restingReleaseRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingReleaseRate_IHC_Carney(Float theRestingReleaseRate)
{
	static const WChar	*funcName = wxT("SetRestingReleaseRate_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->restingReleaseRate = theRestingReleaseRate;
	return(TRUE);

}

/****************************** SetRestingPerm ********************************/

/*
 * This function sets the module's restingPerm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingPerm_IHC_Carney(Float theRestingPerm)
{
	static const WChar	*funcName = wxT("SetRestingPerm_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->restingPerm = theRestingPerm;
	return(TRUE);

}

/****************************** SetMaxGlobalPerm ******************************/

/*
 * This function sets the module's maxGlobalPerm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxGlobalPerm_IHC_Carney(Float theMaxGlobalPerm)
{
	static const WChar	*funcName = wxT("SetMaxGlobalPerm_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxGlobalPerm = theMaxGlobalPerm;
	return(TRUE);

}

/****************************** SetMaxLocalPerm *******************************/

/*
 * This function sets the module's maxLocalPerm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxLocalPerm_IHC_Carney(Float theMaxLocalPerm)
{
	static const WChar	*funcName = wxT("SetMaxLocalPerm_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxLocalPerm = theMaxLocalPerm;
	return(TRUE);

}

/****************************** SetMaxImmediatePerm ***************************/

/*
 * This function sets the module's maxImmediatePerm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxImmediatePerm_IHC_Carney(Float theMaxImmediatePerm)
{
	static const WChar	*funcName = wxT("SetMaxImmediatePerm_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxImmediatePerm = theMaxImmediatePerm;
	return(TRUE);

}

/****************************** SetMaxLocalVolume *****************************/

/*
 * This function sets the module's maxLocalVolume parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxLocalVolume_IHC_Carney(Float theMaxLocalVolume)
{
	static const WChar	*funcName = wxT("SetMaxLocalVolume_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxLocalVolume = theMaxLocalVolume;
	return(TRUE);

}

/****************************** SetMinLocalVolume *****************************/

/*
 * This function sets the module's minLocalVolume parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMinLocalVolume_IHC_Carney(Float theMinLocalVolume)
{
	static const WChar	*funcName = wxT("SetMinLocalVolume_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->minLocalVolume = theMinLocalVolume;
	return(TRUE);

}

/****************************** SetMaxImmediateVolume *************************/

/*
 * This function sets the module's maxImmediateVolume parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxImmediateVolume_IHC_Carney(Float theMaxImmediateVolume)
{
	static const WChar	*funcName = wxT("SetMaxImmediateVolume_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxImmediateVolume = theMaxImmediateVolume;
	return(TRUE);

}

/****************************** SetMinImmediateVolume *************************/

/*
 * This function sets the module's minImmediateVolume parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMinImmediateVolume_IHC_Carney(Float theMinImmediateVolume)
{
	static const WChar	*funcName = wxT("SetMinImmediateVolume_IHC_Carney");

	if (carneyHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->minImmediateVolume = theMinImmediateVolume;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_IHC_Carney(void)
{
	DPrint(wxT("Carney IHC Synapse Module Parameters:-\n"));
	DPrint(wxT("\tMax. depolarizing HC voltage, Vmax = %g (V)\n"),
	  carneyHCPtr->maxHCVoltage);
	DPrint(wxT("\tResting release rate, R0 = %g (spikes/s)\n"),
	  carneyHCPtr->restingReleaseRate);
	DPrint(wxT("\tResting permeability, Prest = %g ('volume'/s)\n"),
	  carneyHCPtr->restingPerm);
	DPrint(wxT("\tMax. global permeability, PGmax = %g ('volume'/s)\n"),
	  carneyHCPtr->maxGlobalPerm);
	DPrint(wxT("\tMax. local , PLmax = %g ('volume'/s)\n"),
	  carneyHCPtr->maxLocalPerm);
	DPrint(wxT("\tMax. immediate permeability, PImax = %g ('volume'/s)\n"),
	  carneyHCPtr->maxImmediatePerm);
	DPrint(wxT("\tMax. local volume, VLmax = %g ('volume')\n"),
	  carneyHCPtr->maxLocalVolume);
	DPrint(wxT("\tMin. local volume, VLmin = %g ('volume')\n"),
	  carneyHCPtr->minLocalVolume);
	DPrint(wxT("\tMax. immediate volume, VImax = %g ('volume')\n"),
	  carneyHCPtr->maxImmediateVolume);
	DPrint(wxT("\tMin. immediate volume, VLmax = %g ('volume')\n"),
	  carneyHCPtr->minImmediateVolume);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHC_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHC_Carney");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	carneyHCPtr = (CarneyHCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHC_Carney");

	if (!SetParsPointer_IHC_Carney(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHC_Carney(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = carneyHCPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHC_Carney;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Carney;
	theModule->PrintPars = PrintPars_IHC_Carney;
	theModule->RunProcess = RunModel_IHC_Carney;
	theModule->SetParsPointer = SetParsPointer_IHC_Carney;
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
CheckData_IHC_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IHC_Carney");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the hair cells to the spontaneous firing rate.
 * The Spontaneous firing rate is determined from the equilbrium state of the
 * system with no input: the reservoir contents do not change in time.
 */

BOOLN
InitProcessVariables_IHC_Carney(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_IHC_Carney");
	int		i;
	Float	restingImmediateConc_CI0, restingLocalConc_CL0;
	CarneyHCPtr	p = carneyHCPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (carneyHCPtr->updateProcessVariablesFlag || data->
		  updateProcessFlag) {
			FreeProcessVariables_IHC_Carney();
			if ((p->hCChannels = (CarneyHCVarsPtr) calloc(
			  _OutSig_EarObject(data)->numChannels, sizeof (CarneyHCVars))) == NULL) {
				NotifyError(wxT("%s: Out of memory."), funcName);
				return(FALSE);
			}
			carneyHCPtr->updateProcessVariablesFlag = FALSE;
		}
		restingImmediateConc_CI0 = carneyHCPtr->restingReleaseRate /
		  carneyHCPtr->restingPerm;
		restingLocalConc_CL0 = CARNEY_IHC_RESTING_LOCAL_CONC_FACTOR *
		  restingImmediateConc_CI0;
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			p->hCChannels[i].vI = carneyHCPtr->minImmediateVolume;
			p->hCChannels[i].vL = carneyHCPtr->minLocalVolume;
			p->hCChannels[i].cI = restingImmediateConc_CI0;
			p->hCChannels[i].cL = restingLocalConc_CL0;
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
FreeProcessVariables_IHC_Carney(void)
{
	if (carneyHCPtr->hCChannels == NULL)
		return;
	free(carneyHCPtr->hCChannels);
	carneyHCPtr->hCChannels = NULL;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;

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
RunModel_IHC_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_IHC_Carney");
	register	ChanData	 *inPtr, *outPtr;
	int			chan;
	Float		releaseProb, pI, pL, pG;
	ChanLen		i;
	CarneyHCPtr	p = carneyHCPtr;
	SignalDataPtr	outSignal;
	CarneyHCVarsPtr	vPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_IHC_Carney(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Carney IHC Synapse"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_IHC_Carney(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->dt = _OutSig_EarObject(data)->dt;
		p->cG = CARNEY_IHC_RESTING_GLOBAL_CONC_FACTOR * p->restingReleaseRate /
		  p->restingPerm;
		p->pIMaxMinusPrest = p->maxImmediatePerm - p->restingPerm;
		p->pLMaxMinusPrest = p->maxLocalPerm - p->restingPerm;
		p->pGMaxMinusPrest = p->maxGlobalPerm - p->restingPerm;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		vPtr = &p->hCChannels[chan];
		for (i = 0; i < outSignal->length; i++) {
			releaseProb = *inPtr++ / p->maxHCVoltage;
			if ((pI = p->pIMaxMinusPrest * releaseProb + p->restingPerm) < 0.0)
				pI = 0.0;
			if ((pL = p->pLMaxMinusPrest * releaseProb + p->restingPerm) < 0.0)
				pL = 0.0;
			if ((pG = p->pGMaxMinusPrest * releaseProb + p->restingPerm) < 0.0)
				pG = 0.0;

			if (releaseProb < 0.0)
				releaseProb = 0.0;
			vPtr->vI += (p->maxImmediateVolume - vPtr->vI) * releaseProb;
			vPtr->vL += (p->maxLocalVolume - vPtr->vL) * releaseProb;

			/* Use Euler Method to solve Diff EQ's */
			vPtr->cI += p->dt / vPtr->vI * (-pI * vPtr->cI + pL * (vPtr->cL -
			  vPtr->cI));
			vPtr->cL += p->dt / vPtr->vL * (-pL * (vPtr->cL - vPtr->cI) + pG *
			  (p->cG - vPtr->cL));

			/* Spike prob. */
			*outPtr++ = (ChanData) (vPtr->cI * pI * p->dt);
		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

