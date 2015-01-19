/**********************
 *
 * File:		MoIHC86Meddis.c
 * Purpose:		This module contains the model Meddis 86 hair cell module.
 * Comments:	The parameters A and B have been changed so that the hair cell
 *				responds correctly for  Intensity levels in dB (SPL).
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		07 May 1998
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
#include "MoIHC86Meddis.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

HairCellPtr	hairCellPtr = NULL;

/******************************************************************************/
/*************************** Subroutines and Functions ************************/
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
Init_IHC_Meddis86(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_IHC_Meddis86");

	if (parSpec == GLOBAL) {
		if (hairCellPtr != NULL)
			Free_IHC_Meddis86();
		if ((hairCellPtr = (HairCellPtr) malloc(sizeof(HairCell))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (hairCellPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	hairCellPtr->parSpec = parSpec;
	hairCellPtr->updateProcessVariablesFlag = TRUE;

	hairCellPtr->diagMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	hairCellPtr->permConst_A = 100.0;
	hairCellPtr->permConst_B = 6000.0;
	hairCellPtr->releaseRate_g = 2000.0;
	hairCellPtr->replenishRate_y = 5.05;
	hairCellPtr->lossRate_l = 2500.0;
	hairCellPtr->reprocessRate_x = 66.31;
	hairCellPtr->recoveryRate_r = 6580.0;
	hairCellPtr->maxFreePool_M = 1.0;
	hairCellPtr->firingRate_h = 50000.0;

	if ((hairCellPtr->diagModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), hairCellPtr->diagFileName)) == NULL)
		return(FALSE);

	if (!SetUniParList_IHC_Meddis86()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHC_Meddis86();
		return(FALSE);
	}
	DSAM_strcpy(hairCellPtr->diagFileName, DEFAULT_FILE_NAME);
	hairCellPtr->hCChannels = NULL;
	hairCellPtr->fp = NULL;
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
Free_IHC_Meddis86(void)
{
	if (hairCellPtr == NULL)
		return(TRUE);
	FreeProcessVariables_IHC_Meddis86();
	if (hairCellPtr->parList)
		FreeList_UniParMgr(&hairCellPtr->parList);
	if (hairCellPtr->parSpec == GLOBAL) {
		free(hairCellPtr);
		hairCellPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_IHC_Meddis86(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHC_Meddis86");
	UniParPtr	pars;

	if ((hairCellPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  MEDDIS86_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = hairCellPtr->parList->pars;
	SetPar_UniParMgr(&pars[MEDDIS86_DIAGMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostic mode. Outputs internal states of running model in non-")
	    wxT("threaded mode('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &hairCellPtr->diagMode, hairCellPtr->diagModeList,
	  (void * (*)) SetDiagMode_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_PERM_CONST_A], wxT("PERM_CONST_A"),
	  wxT("Permeability constant A (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->permConst_A, NULL,
	  (void * (*)) SetPermConstA_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_PERM_CONST_B], wxT("PERM_CONST_B"),
	  wxT("Permeability constant B (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->permConst_B, NULL,
	  (void * (*)) SetPermConstB_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_RELEASE_RATE_G], wxT("RELEASE_G"),
	  wxT("Release rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->releaseRate_g, NULL,
	  (void * (*)) SetReleaseRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_REPLENISH_RATE_Y], wxT("REPLENISH_Y"),
	  wxT("Replenishment rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->replenishRate_y, NULL,
	  (void * (*)) SetReplenishRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_LOSS_RATE_L], wxT("LOSS_L"),
	  wxT("Loss rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->lossRate_l, NULL,
	  (void * (*)) SetLossRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_REPROCESS_RATE_X], wxT("REPROCESS_X"),
	  wxT("Reprocessing rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->reprocessRate_x, NULL,
	  (void * (*)) SetReprocessRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_RECOVERY_RATE_R], wxT("RECOVERY_R"),
	  wxT("Recovery rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->recoveryRate_r, NULL,
	  (void * (*)) SetRecoveryRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_MAX_FREE_POOL_M], wxT("MAX_FREE_POOL_M"),
	  wxT("Max. no. of transmitter packets in free pool."),
	  UNIPAR_REAL,
	  &hairCellPtr->maxFreePool_M, NULL,
	  (void * (*)) SetMaxFreePool_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_FIRING_RATE_H], wxT("FIRING_RATE_H"),
	  wxT("Firing rate (spikes/s)"),
	  UNIPAR_REAL,
	  &hairCellPtr->firingRate_h, NULL,
	  (void * (*)) SetFiringRate_IHC_Meddis86);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_IHC_Meddis86(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (hairCellPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(hairCellPtr->parList);

}

/****************************** SetDiagMode ***********************************/

/*
 * This function sets the module's diagMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagMode_IHC_Meddis86(WChar * theDiagMode)
{
	static const WChar	*funcName = wxT("SetDiagMode_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}

	/*** Put any other required checks here. ***/
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	hairCellPtr->diagMode = IdentifyDiag_NSpecLists(theDiagMode,
	  hairCellPtr->diagModeList);
	return(TRUE);

}

/********************************* SetPermConstA ******************************/

/*
 * This function sets the module's permeability constant A parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermConstA_IHC_Meddis86(Float thePermConstA)
{
	static const WChar	*funcName = wxT("SetPermConstA_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->permConst_A = thePermConstA;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPermConstB ******************************/

/*
 * This function sets the module's permeability constant B parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermConstB_IHC_Meddis86(Float thePermConstB)
{
	static const WChar	*funcName = wxT("SetPermConstB_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->permConst_B = thePermConstB;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReleaseRate *****************************/

/*
 * This function sets the module's release rate constant, r, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReleaseRate_IHC_Meddis86(Float theReleaseRate)
{
	static const WChar	*funcName = wxT("SetReleaseRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->releaseRate_g = theReleaseRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReplenishRate ***************************/

/*
 * This function sets the module's replenish rate, y, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReplenishRate_IHC_Meddis86(Float theReplenishRate)
{
	static const WChar	*funcName = wxT("SetReplenishRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->replenishRate_y = theReplenishRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetLossRate ********************************/

/*
 * This function sets the module's loss rate, l, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLossRate_IHC_Meddis86(Float theLossRate)
{
	static const WChar	*funcName = wxT("SetLossRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->lossRate_l = theLossRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReprocessRate ***************************/

/*
 * This function sets the module's permeability constant B parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReprocessRate_IHC_Meddis86(Float theReprocessRate)
{
	static const WChar	*funcName = wxT("SetReprocessRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->reprocessRate_x = theReprocessRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetRecoveryRate ****************************/

/*
 * This function sets the module's permeability constant B parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRecoveryRate_IHC_Meddis86(Float theRecoveryRate)
{
	static const WChar	*funcName = wxT("SetRecoveryRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->recoveryRate_r = theRecoveryRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxFreePool *****************************/

/*
 * This function sets the module's permeability constant B parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMaxFreePool_IHC_Meddis86(Float theMaxFreePool)
{
	static const WChar	*funcName = wxT("SetMaxFreePool_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->maxFreePool_M = theMaxFreePool;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetFiringRate ******************************/

/*
 * This function sets the module's permeability constant h parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFiringRate_IHC_Meddis86(Float theFiringRate)
{
	static const WChar	*funcName = wxT("SetFiringRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->firingRate_h = theFiringRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_IHC_Meddis86(void)
{
	DPrint(wxT("Meddis 86 IHC Synapse Module Parameters:-\n"));
	DPrint(wxT("\tPermeability constant, A = %g,\tand B = %g\n"),
	  hairCellPtr->permConst_A, hairCellPtr->permConst_B);
	DPrint(wxT("\tRelease rate, g = %g /s,\tReplenishment rate, y = %g /s\n"),
	  hairCellPtr->releaseRate_g, hairCellPtr->replenishRate_y);
	DPrint(wxT("\tLoss rate, l = %g /s,\tReprocessing rate, x = %g /s\n"),
	  hairCellPtr->lossRate_l, hairCellPtr->reprocessRate_x);
	DPrint(wxT("\tRecovery rate, r = %g /s,\t Firing rate, h = %g spikes/s\n"),
	  hairCellPtr->recoveryRate_r, hairCellPtr->firingRate_h);
	DPrint(wxT("\tMaximum transmitter packets in free pool, M = %g.\n"),
	  hairCellPtr->maxFreePool_M);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHC_Meddis86(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHC_Meddis86");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	hairCellPtr = (HairCellPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Meddis86(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHC_Meddis86");

	if (!SetParsPointer_IHC_Meddis86(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHC_Meddis86(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = hairCellPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHC_Meddis86;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Meddis86;
	theModule->PrintPars = PrintPars_IHC_Meddis86;
	theModule->RunProcess = RunModel_IHC_Meddis86;
	theModule->SetParsPointer = SetParsPointer_IHC_Meddis86;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the input signal is correctly initialised
 * (using CheckRamp_SignalData), and determines whether the parameter values
 * are valid for the signal sampling interval.
 */

BOOLN
CheckData_IHC_Meddis86(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_IHC_Meddis86");
	Float	dt;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(_InSig_EarObject(data, 0)))
		return(FALSE);
	dt = _InSig_EarObject(data, 0)->dt;
	if (dt > MEDDIS86_MAX_DT) {
		NotifyError(wxT("%s: Maximum sampling interval exceeded."), funcName);
		return(FALSE);
	}
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
InitProcessVariables_IHC_Meddis86(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_IHC_Meddis86");
	int		i;
	Float	spontPerm_k0, spontCleft_c0, spontFreePool_q0, spontReprocess_w0;
	HairCellPtr	p = hairCellPtr;

	if (hairCellPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_IHC_Meddis86();
		OpenDiagnostics_NSpecLists(&p->fp, p->diagModeList, p->diagMode);
			if ((p->hCChannels = (HairCellVarsPtr) calloc(
			  _OutSig_EarObject(data)->numChannels, sizeof (HairCellVars))) == NULL) {
				NotifyError(wxT("%s: Out of memory."), funcName);
				return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		spontPerm_k0 = p->releaseRate_g * p->permConst_A / (p->permConst_A +
		  p->permConst_B);
		spontCleft_c0 = (p->maxFreePool_M * p->replenishRate_y *
		  spontPerm_k0 / (p->replenishRate_y * (p->lossRate_l +
		  p->recoveryRate_r) + spontPerm_k0 * p->lossRate_l));
		spontFreePool_q0 = spontCleft_c0 * (p->lossRate_l +
		  p->recoveryRate_r) / spontPerm_k0;
		spontReprocess_w0 = spontCleft_c0 * p->recoveryRate_r /
		  p->reprocessRate_x;

		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			p->hCChannels[i].cleftC = spontCleft_c0;
			p->hCChannels[i].reservoirQ = spontFreePool_q0;
			p->hCChannels[i].reprocessedW = spontReprocess_w0;
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
FreeProcessVariables_IHC_Meddis86(void)
{
	if (hairCellPtr->hCChannels == NULL)
		return;
	free(hairCellPtr->hCChannels);
	hairCellPtr->hCChannels = NULL;
	hairCellPtr->updateProcessVariablesFlag = TRUE;

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

BOOLN
RunModel_IHC_Meddis86(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_IHC_Meddis86");
	register	Float	replenish, reprocessed, ejected, reUptake;
	register	Float	reUptakeAndLost;
	BOOLN	debug;
	int		i;
	ChanLen	j;
	Float	st_Plus_A, kdt;
	ChanData	*inPtr, *outPtr;
	SignalDataPtr	outSignal;
	HairCellPtr	p = hairCellPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_IHC_Meddis86(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Meddis 86 IHC Synapse"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHC_Meddis86(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->dt = _OutSig_EarObject(data)->dt;
		p->ymdt = p->replenishRate_y * p->maxFreePool_M * p->dt;
		p->xdt = p->reprocessRate_x * p->dt;
		p->ydt = p->replenishRate_y * p->dt;
		p->l_Plus_rdt = (p->lossRate_l + p->recoveryRate_r) * p->dt;
		p->rdt = p->recoveryRate_r * p->dt;
		p->gdt = p->releaseRate_g * p->dt;
		p->hdt = p->firingRate_h * p->dt;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	debug = (!data->threadRunFlag && (p->diagMode != GENERAL_DIAGNOSTIC_OFF_MODE));
	if (debug)
		DSAM_fprintf(p->fp, wxT("Time(s)\tInput\tkdt (P)\tQ \tC \tW \tEjected\n"));

	outSignal = _OutSig_EarObject(data);
	for (i = outSignal->offset; i < outSignal->numChannels; i++) {
		inPtr = _InSig_EarObject(data, 0)->channel[i];
		outPtr = outSignal->channel[i];
		for (j = 0; j < outSignal->length; j++) {
			if ((st_Plus_A = *inPtr++ + p->permConst_A) > 0.0)
				kdt = p->gdt * st_Plus_A / (st_Plus_A + p->permConst_B);
			else
				kdt = 0.0;
			replenish = (p->hCChannels[i].reservoirQ < p->maxFreePool_M)?
			  p->ymdt - p->ydt * p->hCChannels[i].reservoirQ: 0.0;
			reprocessed = p->xdt * p->hCChannels[i].reprocessedW;
			ejected = kdt * p->hCChannels[i].reservoirQ;
			reUptake = p->rdt * p->hCChannels[i].cleftC;
			reUptakeAndLost = p->l_Plus_rdt * p->hCChannels[i].cleftC;

			p->hCChannels[i].reservoirQ += replenish - ejected + reprocessed;
			p->hCChannels[i].cleftC += ejected - reUptakeAndLost;
			p->hCChannels[i].reprocessedW += reUptake - reprocessed;

			/* Spike prob. */
			*outPtr++ = (ChanData) (p->hdt * p->hCChannels[i].cleftC);
			if (debug)
				DSAM_fprintf(p->fp,
				  wxT("%g\t%g\t%g\t%g\t%g\t%g\t%g\n"), j * outSignal->
				  dt, *(inPtr - 1), kdt, p->hCChannels[i].reservoirQ, p->hCChannels[i].cleftC, p->
				  hCChannels[i].reprocessedW, ejected);
		}
	}
	if (debug && p->fp)
		fclose(p->fp);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* RunModel_IHC_Meddis86 */
