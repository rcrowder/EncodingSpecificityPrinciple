/******************
 *
 * File:		MoIHC86aMeddis.c
 * Purpose:		This module contains the routines for the Meddis 86a Hair cell
 *				model module.
 * Comments:	It is expects input from a IHC receptor potential model module.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1996
 * Updated:		05 Sep 1996
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
#include "FiParFile.h"
#include "UtString.h"
#include "MoIHC86aMeddis.h"
#include "UtRandom.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/* #define DEBUG	1 */

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

HairCell3Ptr	hairCell3Ptr = NULL;

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
Init_IHC_Meddis86a(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_IHC_Meddis86a");

	if (parSpec == GLOBAL) {
		if (hairCell3Ptr != NULL)
			Free_IHC_Meddis86a();
		if ((hairCell3Ptr = (HairCell3Ptr) malloc(sizeof(HairCell3))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (hairCell3Ptr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	hairCell3Ptr->parSpec = parSpec;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;

	hairCell3Ptr->maxFreePool_M = 1;
	hairCell3Ptr->replenishRate_y = 5.05;
	hairCell3Ptr->lossRate_l = 2500.0;
	hairCell3Ptr->reprocessRate_x = 66.31;
	hairCell3Ptr->recoveryRate_r = 6580.0;
	hairCell3Ptr->permeabilityPH_h = 800.0;
	hairCell3Ptr->permeabilityPZ_z = 8.6538e+10;
	hairCell3Ptr->firingRate_h2 = 50000.0;

	if (!SetUniParList_IHC_Meddis86a()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHC_Meddis86a();
		return(FALSE);
	}
	hairCell3Ptr->hCChannels = NULL;
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
Free_IHC_Meddis86a(void)
{
	if (hairCell3Ptr == NULL)
		return(TRUE);
	FreeProcessVariables_IHC_Meddis86a();
	if (hairCell3Ptr->parList)
		FreeList_UniParMgr(&hairCell3Ptr->parList);
	if (hairCell3Ptr->parSpec == GLOBAL) {
		free(hairCell3Ptr);
		hairCell3Ptr = NULL;
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
SetUniParList_IHC_Meddis86a(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHC_Meddis86a");
	UniParPtr	pars;

	if ((hairCell3Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_MEDDIS86A_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = hairCell3Ptr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_MAXFREEPOOL_M], wxT("MAX_FREE_POOL"),
	  wxT("Max. no. of transmitter packets in free pool."),
	  UNIPAR_REAL,
	  &hairCell3Ptr->maxFreePool_M, NULL,
	  (void * (*)) SetMaxFreePool_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_REPLENISHRATE_Y], wxT("REPLENISH_Y"),
	  wxT("Replenishment rate (units per second)."),
	  UNIPAR_REAL,
	  &hairCell3Ptr->replenishRate_y, NULL,
	  (void * (*)) SetReplenishRate_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_LOSSRATE_L], wxT("LOSS_L"),
	  wxT("Loss rate (units per second)."),
	  UNIPAR_REAL,
	  &hairCell3Ptr->lossRate_l, NULL,
	  (void * (*)) SetLossRate_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_REPROCESSRATE_X], wxT("REPROCESS_X"),
	  wxT("Reprocessing rate (units per second)."),
	  UNIPAR_REAL,
	  &hairCell3Ptr->reprocessRate_x, NULL,
	  (void * (*)) SetReprocessRate_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_RECOVERYRATE_R], wxT("RECOVERY_R"),
	  wxT("Recovery rate (units per second)."),
	  UNIPAR_REAL,
	  &hairCell3Ptr->recoveryRate_r, NULL,
	  (void * (*)) SetRecoveryRate_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_PERMEABILITYPH_H], wxT("PERM_CONST_H"),
	  wxT("Permeability constant, h."),
	  UNIPAR_REAL,
	  &hairCell3Ptr->permeabilityPH_h, NULL,
	  (void * (*)) SetPermeabilityPH_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_PERMEABILITYPZ_Z], wxT("PERM_CONST_Z"),
	  wxT("Permeability constant, z."),
	  UNIPAR_REAL,
	  &hairCell3Ptr->permeabilityPZ_z, NULL,
	  (void * (*)) SetPermeabilityPZ_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_FIRINGRATE_H2], wxT("FIRING_RATE_H2"),
	  wxT("Firing rate (spikes per second)."),
	  UNIPAR_REAL,
	  &hairCell3Ptr->firingRate_h2, NULL,
	  (void * (*)) SetFiringRate_IHC_Meddis86a);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_Meddis86a(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (hairCell3Ptr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(hairCell3Ptr->parList);

}

/********************************* SetReplenishRate ***************************/

/*
 * This function sets the module's replenish rate, y, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReplenishRate_IHC_Meddis86a(Float theReplenishRate)
{
	static const WChar	*funcName = wxT("SetReplenishRate_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCell3Ptr->replenishRate_y = theReplenishRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetLossRate ********************************/

/*
 * This function sets the module's loss rate, l, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLossRate_IHC_Meddis86a(Float theLossRate)
{
	static const WChar	*funcName = wxT("SetLossRate_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCell3Ptr->lossRate_l = theLossRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReprocessRate ***************************/

/*
 * This function sets the module's reprocess rate, x  parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReprocessRate_IHC_Meddis86a(Float theReprocessRate)
{
	static const WChar	*funcName = wxT("SetReprocessRate_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCell3Ptr->reprocessRate_x = theReprocessRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetRecoveryRate ****************************/

/*
 * This function sets the module's recovery rate, r parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRecoveryRate_IHC_Meddis86a(Float theRecoveryRate)
{
	static const WChar	*funcName = wxT("SetRecoveryRate_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCell3Ptr->recoveryRate_r = theRecoveryRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxFreePool *****************************/

/*
 * This function sets the module's maximum free transmitter pool, M parameter.
 * This is a Float variable to maintain transparency with Meddis 86 model.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMaxFreePool_IHC_Meddis86a(Float theMaxFreePool)
{
	static const WChar *funcName = wxT("SetMaxFreePool_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theMaxFreePool < 1.0) {
		NotifyError(wxT("%s: Illegal initial value: M = %d."), funcName,
		  theMaxFreePool);
		return(FALSE);
	}
	hairCell3Ptr->maxFreePool_M = theMaxFreePool;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPermeabilityPH **************************/

/*
 * This function sets the module's permeabilityP constant H parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermeabilityPH_IHC_Meddis86a(Float thePermeabilityPH)
{
	static const WChar	*funcName = wxT("SetPermeabilityPH_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCell3Ptr->permeabilityPH_h = thePermeabilityPH;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPermeabilityPZ **************************/

/*
 * This function sets the module's permeability constant Z parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermeabilityPZ_IHC_Meddis86a(Float thePermeabilityPZ)
{
	static const WChar	*funcName = wxT("SetPermeabilityPZ_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCell3Ptr->permeabilityPZ_z = thePermeabilityPZ;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetFiringRate ******************************/

/*
 * This function sets the module's permeability constant h parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFiringRate_IHC_Meddis86a(Float theFiringRate)
{
	static const WChar	*funcName = wxT("SetFiringRate_IHC_Meddis86a");

	if (hairCell3Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCell3Ptr->firingRate_h2 = theFiringRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_IHC_Meddis86a(void)
{
	DPrint(wxT("Meddis 86a Inner Hair Cell Module Parameters:-\n"));
	DPrint(wxT("\tPermeability constants: z = %g,\th = %g \n"),
	  hairCell3Ptr->permeabilityPZ_z, hairCell3Ptr->permeabilityPH_h);
	DPrint(wxT("\tReplenishment rate, y = %g /s,"),
	  hairCell3Ptr->replenishRate_y);
	DPrint(wxT("\tLoss rate, l = %g /s,\n"), hairCell3Ptr->lossRate_l);
	DPrint(wxT("\tReprocess rate, x = %g /s,"), hairCell3Ptr->reprocessRate_x);
	DPrint(wxT("\tRecovery rate, r = %g /s,\n"), hairCell3Ptr->recoveryRate_r);
	DPrint(wxT("\tFiring rate, h2 = %g spikes/s\n"), hairCell3Ptr->
	  firingRate_h2);
	DPrint(wxT("\tMaximum transmitter packets in free pool, M = %g,\n"),
	  hairCell3Ptr->maxFreePool_M);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHC_Meddis86a(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHC_Meddis86a");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	hairCell3Ptr = (HairCell3Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Meddis86a(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHC_Meddis86a");

	if (!SetParsPointer_IHC_Meddis86a(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHC_Meddis86a(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = hairCell3Ptr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHC_Meddis86a;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Meddis86a;
	theModule->PrintPars = PrintPars_IHC_Meddis86a;
	theModule->RunProcess = RunModel_IHC_Meddis86a;
	theModule->SetParsPointer = SetParsPointer_IHC_Meddis86a;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the input signal is correctly initialised, and
 * determines whether the parameter values are valid for the signal
 * sampling interval.
 */

BOOLN
CheckData_IHC_Meddis86a(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_IHC_Meddis86a");
	BOOLN	ok;
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
	if (dt > MEDDIS86A_MAX_DT) {
		NotifyError(wxT("%s: Maximum sampling interval exceeded."), funcName);
		return(FALSE);
	}
	ok = TRUE;
	if (hairCell3Ptr->replenishRate_y * dt >= 1.0) {
		NotifyError(wxT("%s: Replenishment rate, y = %g /s is too high for the ")
		  wxT("sampling interval."), funcName, hairCell3Ptr->replenishRate_y);
		ok = FALSE;
	}
	if (hairCell3Ptr->lossRate_l * dt >= 1.0) {
		NotifyError(wxT("%s: Loss rate, l = %g /s is too high for the sampling ")
		  wxT("interval."), funcName, hairCell3Ptr->lossRate_l);
		ok = FALSE;
	}
	if (hairCell3Ptr->recoveryRate_r * dt >= 1.0) {
		NotifyError(wxT("%s: Recovery rate, r = %g /s is too high for the ")
		  wxT("sampling interval."), funcName, hairCell3Ptr->recoveryRate_r);
		ok = FALSE;
	}
	if (hairCell3Ptr->reprocessRate_x * dt >= 1.0) {
		NotifyError(wxT("%s: Reprocess rate, x = %g /s is too high for the ")
		  wxT("sampling interval."), funcName, hairCell3Ptr->reprocessRate_x);
		ok = FALSE;
	}
	/* if (hairCell3Ptr->permeabilityPH_h > maximumH) {
		maximumH = log(1.0 / (hairCell3Ptr->permeabilityPZ_z * dt));
		NotifyError(wxT("%s: The permeability constant, h = %g, is too high ")
		  wxT("for\nthe signal sampling interval, dt = %g ms, and the ")
		  wxT("permeability constant, z = %g (h < %g)."), funcName,
		  hairCell3Ptr->permeabilityPH_h, dt, hairCell3Ptr->permeabilityPZ_z);
		ok = FALSE;
	} */
	return(ok);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the hair cells to the spontaneous firing rate.
 * The Spontaneous firing rate is determined from the equilbrium state of the
 * system with no input: the reservoir contents do not change in time.
 * For the p'(0) value it uses the first value from the input signal, which it
 * assumes to contain the appropriate value.  The is assumed to be the same
 * for all channels.
 */

BOOLN
InitProcessVariables_IHC_Meddis86a(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_IHC_Meddis86a");
	int		i;
	Float	spontPerm_k0, spontCleft_c0, spontFreePool_q0, spontReprocess_w0;
	HairCell3Ptr	p = hairCell3Ptr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_IHC_Meddis86a();
			if ((p->hCChannels = (HairCellVars3Ptr) calloc(
			  _OutSig_EarObject(data)->numChannels, sizeof (HairCellVars3))) == NULL) {
				NotifyError(wxT("%s: Out of memory."), funcName);
				return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		spontPerm_k0 = p->permeabilityPZ_z * exp(p->permeabilityPH_h *
		  _InSig_EarObject(data, 0)->channel[0][0]);
		spontCleft_c0 = p->maxFreePool_M * p->replenishRate_y *
		   spontPerm_k0 / (p->replenishRate_y * (p->lossRate_l +
		   p->recoveryRate_r) + spontPerm_k0 * p->lossRate_l);
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
FreeProcessVariables_IHC_Meddis86a(void)
{
	if (hairCell3Ptr->hCChannels == NULL)
		return;
	free(hairCell3Ptr->hCChannels);
	hairCell3Ptr->hCChannels = NULL;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;

}
/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

BOOLN
RunModel_IHC_Meddis86a(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_IHC_Meddis86a");
	BOOLN	clipped;
	int		i;
	ChanLen	j;
	Float	dt, kdt;
	Float	reUptake, reUptakeAndLost,replenish, reprocessed, ejected;
	ChanData	*inPtr, *outPtr;
	SignalDataPtr	outSignal;
	HairCell3Ptr	p = hairCell3Ptr;

	if (!data->threadRunFlag) {
		if (!CheckData_IHC_Meddis86a(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Meddis 86a quantal hair cell"));
		if (!InitProcessVariables_IHC_Meddis86a(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		dt = _OutSig_EarObject(data)->dt;

		p->ymdt = p->replenishRate_y * p->maxFreePool_M * dt;
		p->xdt = p->reprocessRate_x * dt;
		p->ydt = p->replenishRate_y * dt;
		p->l_Plus_rdt = (p->lossRate_l + p->recoveryRate_r) * dt;
		p->rdt = p->recoveryRate_r * dt;
		p->zdt = p->permeabilityPZ_z * dt;
		p->h2dt = p->firingRate_h2 * dt;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (i = outSignal->offset, clipped = FALSE; i < outSignal->numChannels;
	  i++) {
		inPtr = _InSig_EarObject(data, 0)->channel[i];
		outPtr = outSignal->channel[i];
		for (j = 0; j < outSignal->length; j++) {
			kdt = p->zdt * exp(p->permeabilityPH_h * *inPtr++);
			if (kdt >= 1.0) {
				kdt = 0.99;
				clipped = TRUE;
			}
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
			*outPtr++ = (ChanData) (p->h2dt * p->hCChannels[i].cleftC);
		}
	}
	/*if (clipped)
		NotifyWarning(wxT("%s: Clipping has occurred."), funcName); */
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* RunModel_IHC_Meddis86a */
