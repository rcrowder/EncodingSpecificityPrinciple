/******************
 *
 * File:		MoNCMcGregor.c
 * Purpose:		This module implements  the MacGregor Model.
 * Comments:	It was revised from code written by Trevor Shackleton and
 *				M. J. Hewitt.
 * Authors:		L. P. O'Mard.
 * Created:		Oct 1993
 * Updated:		24 May 1996
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
#include "MoNCMcGregor.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

McGregorPtr		mcGregorPtr = NULL;

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
Init_Neuron_McGregor(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_Neuron_McGregor");

	if (parSpec == GLOBAL) {
		if (mcGregorPtr != NULL)
			Free_Neuron_McGregor();
		if ((mcGregorPtr = (McGregorPtr) malloc(sizeof(McGregor))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (mcGregorPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	mcGregorPtr->parSpec = parSpec;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	mcGregorPtr->membraneTConst_Tm = 2e-3;
	mcGregorPtr->kDecayTConst_TGk = 0.9e-3;
	mcGregorPtr->thresholdTConst_TTh = 20e-3;
	mcGregorPtr->accomConst_c = 0.3;
	mcGregorPtr->delayedRectKCond_b = 0.017;
	mcGregorPtr->restingThreshold_Th0 = 10.0;
	mcGregorPtr->actionPotential = 50.0;
	mcGregorPtr->kEquilibriumPot_Ek = -10.0;
	mcGregorPtr->cellRestingPot_Er = -60.0;

	if (!SetUniParList_Neuron_McGregor()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Neuron_McGregor();
		return(FALSE);
	}
	mcGregorPtr->state = NULL;
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
Free_Neuron_McGregor(void)
{
	if (mcGregorPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Neuron_McGregor();
	if (mcGregorPtr->parList)
		FreeList_UniParMgr(&mcGregorPtr->parList);
	if (mcGregorPtr->parSpec == GLOBAL) {
		free(mcGregorPtr);
		mcGregorPtr = NULL;
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
SetUniParList_Neuron_McGregor(void)
{
	static const WChar *funcName = wxT("SetUniParList_Neuron_McGregor");
	UniParPtr	pars;

	if ((mcGregorPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  NEURON_MCGREGOR_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = mcGregorPtr->parList->pars;
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_MEMBRANETCONST_TM], wxT("T_M"),
	  wxT("Cell membrane time-constant, tm (s)."),
	  UNIPAR_REAL,
	  &mcGregorPtr->membraneTConst_Tm, NULL,
	  (void * (*)) SetMembraneTConst_Neuron_McGregor);
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_KDECAYTCONST_TGK], wxT("T_GK"),
	  wxT("Potassium decay time-constant, tGk (s)"),
	  UNIPAR_REAL,
	  &mcGregorPtr->kDecayTConst_TGk, NULL,
	  (void * (*)) SetKDecayTConst_Neuron_McGregor);
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_THRESHOLDTCONST_TTH], wxT("T_TH"),
	  wxT("Threshold time-constant, tTh (s)."),
	  UNIPAR_REAL,
	  &mcGregorPtr->thresholdTConst_TTh, NULL,
	  (void * (*)) SetThresholdTConst_Neuron_McGregor);
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_ACCOMCONST_C], wxT("C"),
	  wxT("Accommodation constant, c (dimensionless)."),
	  UNIPAR_REAL,
	  &mcGregorPtr->accomConst_c, NULL,
	  (void * (*)) SetAccomConst_Neuron_McGregor);
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_DELAYEDRECTKCOND_B], wxT("B"),
	  wxT("Delayed rectifier postassium conductance, b (nano-Siemens)."),
	  UNIPAR_REAL,
	  &mcGregorPtr->delayedRectKCond_b, NULL,
	  (void * (*)) SetDelayedRectKCond_Neuron_McGregor);
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_RESTINGTHRESHOLD_TH0], wxT("TH0"),
	  wxT("Cell resting Threshold, Th0 (mV)."),
	  UNIPAR_REAL,
	  &mcGregorPtr->restingThreshold_Th0, NULL,
	  (void * (*)) SetRestingThreshold_Neuron_McGregor);
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_ACTIONPOTENTIAL], wxT("ACTION_POT"),
	  wxT("Action potential (mV)."),
	  UNIPAR_REAL,
	  &mcGregorPtr->actionPotential, NULL,
	  (void * (*)) SetActionPotential_Neuron_McGregor);
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_KEQUILIBRIUMPOT_EK], wxT("E_K"),
	  wxT("Reversal potential of the potassium conductance, Ek (mV)."),
	  UNIPAR_REAL,
	  &mcGregorPtr->kEquilibriumPot_Ek, NULL,
	  (void * (*)) SetKEquilibriumPot_Neuron_McGregor);
	SetPar_UniParMgr(&pars[NEURON_MCGREGOR_CELLRESTINGPOT_ER], wxT("E_R"),
	  wxT("Cell resting potential Er (mV)."),
	  UNIPAR_REAL,
	  &mcGregorPtr->cellRestingPot_Er, NULL,
	  (void * (*)) SetCellRestingPot_Neuron_McGregor);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Neuron_McGregor(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (mcGregorPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(mcGregorPtr->parList);

}

/********************************* SetMembraneTConst **************************/

/*
 * This function sets the module's membrane time constant, Tm, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMembraneTConst_Neuron_McGregor(Float theMembraneTConst)
{
	static const WChar *funcName = wxT("SetMembraneTConst_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theMembraneTConst == 0.0) {
		NotifyError(wxT("%s: Invalid value = %g mV\n"), funcName,
		  theMembraneTConst);
		return(FALSE);
	}
	mcGregorPtr->membraneTConst_Tm = theMembraneTConst;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetKDecayTConst ****************************/

/*
 * This function sets the module's potassium conductance decay time constant,
 * TGk, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetKDecayTConst_Neuron_McGregor(Float theKDecayTConst)
{
	static const WChar *funcName = wxT("SetKDecayTConst_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theKDecayTConst == 0.0) {
		NotifyError(wxT("%s: Invalid value = %g mV\n"), funcName, theKDecayTConst);
		return(FALSE);
	}
	mcGregorPtr->kDecayTConst_TGk = theKDecayTConst;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetThresholdTConst *************************/

/*
 * This function sets the module's threshold time constant, TTh, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetThresholdTConst_Neuron_McGregor(Float theThresholdTConst)
{
	static const WChar *funcName = wxT("SetThresholdTConst_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theThresholdTConst == 0.0) {
		NotifyError(wxT("%s: Invalid value = %g mV\n"), funcName,
		  theThresholdTConst);
		return(FALSE);
	}
	mcGregorPtr->thresholdTConst_TTh = theThresholdTConst;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetAccomConst ******************************/

/*
 * This function sets the module's accomodation constant, c, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetAccomConst_Neuron_McGregor(Float theAccomConst)
{
	static const WChar	*funcName = wxT("SetAccomConst_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mcGregorPtr->accomConst_c = theAccomConst;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetDelayedRectKCond ************************/

/*
 * This function sets the module's delayed rectifier potassium conductance
 * strength, b, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDelayedRectKCond_Neuron_McGregor(Float theDelayedRectKCond)
{
	static const WChar	*funcName = wxT("SetDelayedRectKCond_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mcGregorPtr->delayedRectKCond_b = theDelayedRectKCond;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetRestingThreshold ************************/

/*
 * This function sets the module's resting threhold, Th0, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRestingThreshold_Neuron_McGregor(Float theRestingThreshold)
{
	static const WChar	*funcName = wxT("SetRestingThreshold_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mcGregorPtr->restingThreshold_Th0 = theRestingThreshold;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetActionPotential *************************/

/*
 * This function sets the module's action potential parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetActionPotential_Neuron_McGregor(Float theActionPotential)
{
	static const WChar	*funcName = wxT("SetActionPotential_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mcGregorPtr->actionPotential = theActionPotential;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetKEquilibriumPot *************************/

/*
 * This function sets the module's reversal potential of the K conductance
 * parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetKEquilibriumPot_Neuron_McGregor(Float theKEquilibriumPot)
{
	static const WChar	*funcName = wxT("SetKEquilibriumPot_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mcGregorPtr->kEquilibriumPot_Ek = theKEquilibriumPot;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetCellRestingPot **************************/

/*
 * This function sets the module's cell resting potential parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCellRestingPot_Neuron_McGregor(Float theCellRestingPot)
{
	static const WChar	*funcName = wxT("SetCellRestingPot_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mcGregorPtr->cellRestingPot_Er = theCellRestingPot;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** GetPotentialResponse **************************/

/*
 * This routine subtracts the action potential from the response if the
 * threshold is exceeded.
 *
 */

void *
GetPotentialResponse_Neuron_McGregor(void *potentialPtr)
{
	static const WChar *funcName = wxT("GetPotentialResponse_Neuron_McGregor");
	Float	*potential;

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	potential = (Float *) potentialPtr;
	if (*potential - mcGregorPtr->cellRestingPot_Er >
	  mcGregorPtr->restingThreshold_Th0 + mcGregorPtr->accomConst_c)
		*potential -= mcGregorPtr->actionPotential;

	return(potentialPtr);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_Neuron_McGregor(void)
{
	static const WChar *funcName = wxT("PrintPars_Neuron_McGregor");

	if (mcGregorPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("McGregor Neural Cell Module Parameters:-\n"));
	DPrint(wxT("\tMembrane time constant, Tm = %g ms,\n"),
	  MSEC(mcGregorPtr->membraneTConst_Tm));
	DPrint(wxT("\tPotassium decay time constant, TGk = %g ms,\n"),
	  MSEC(mcGregorPtr->kDecayTConst_TGk));
	DPrint(wxT("\tThreshold rise time constant, TTh = %g ms,\n"),
	  MSEC(mcGregorPtr->thresholdTConst_TTh));
	DPrint(wxT("\tAccommodation constant, c = %g,\n"),
	  mcGregorPtr->accomConst_c);
	DPrint(wxT("\tDelayed rectifier potassium conductance, ")
	  wxT("b = %g nano Siemens.\n"), mcGregorPtr->delayedRectKCond_b);
	DPrint(wxT("\tResting threshold, Th0 = %g mV,\tAction ")
	  wxT("potential = %g mV.\n"), mcGregorPtr->restingThreshold_Th0,
	  mcGregorPtr->actionPotential);
	DPrint(wxT("\tEquilibrium potential of the potassium conductance, Ek = %g ")
	  wxT("mV,\n"), mcGregorPtr->kEquilibriumPot_Ek);
	DPrint(wxT("\tCell resting potential, Er = %g mV,\n"), mcGregorPtr->
	  cellRestingPot_Er);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Neuron_McGregor(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Neuron_McGregor");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	mcGregorPtr = (McGregorPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Neuron_McGregor(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Neuron_McGregor");

	if (!SetParsPointer_Neuron_McGregor(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Neuron_McGregor(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = mcGregorPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Neuron_McGregor;
	theModule->GetData = GetPotentialResponse_Neuron_McGregor;
	theModule->GetUniParListPtr = GetUniParListPtr_Neuron_McGregor;
	theModule->PrintPars = PrintPars_Neuron_McGregor;
	theModule->RunProcess = RunModel_Neuron_McGregor;
	theModule->SetParsPointer = SetParsPointer_Neuron_McGregor;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It also initialises the variables.
 */

BOOLN
InitProcessVariables_Neuron_McGregor(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Neuron_McGregor");
	int		i;
	McGregorPtr	p = mcGregorPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Neuron_McGregor();
			if ((p->state = (McGregorStatePtr) calloc(_OutSig_EarObject(data)->
			  numChannels, sizeof(McGregorState))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory."), funcName);
			 	return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			p->state[i].potential_E = 0.0;
			p->state[i].kConductance_Gk = 0.0;
			p->state[i].threshold_Th = p->restingThreshold_Th0;
			p->state[i].lastSpikeState = 0;
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
FreeProcessVariables_Neuron_McGregor(void)
{
	if (mcGregorPtr->state == NULL)
		return;
	free(mcGregorPtr->state);
	mcGregorPtr->state = NULL;
	mcGregorPtr->updateProcessVariablesFlag = TRUE;

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 * potential_V = Cell-membrane pot. above resting level (mV);
 * kEquilibriumPot_Vk = Equlibrium potential of K conductance. (mV);
 * kConductance_Gk = Potassium conductance (ns);
 * threshold_Th = Time-varying cell threshold (mV);
 * spikeVariable_s = spike variable for cell.
 * This routine has been changed to use the exponential integration scheme for
 * solving the MacGregor model equations.
 */

BOOLN
RunModel_Neuron_McGregor(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_Neuron_McGregor");
	int			i, spikeState_s;
	Float		dt, totalConductance;
	register Float	potDecay;
	ChanLen		j;
	ChanData	*inPtr, *outPtr;
	McGregorPtr	c = mcGregorPtr;
	SignalDataPtr	outSignal;
	McGregorStatePtr	s;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}
		if (!CheckInSignal_EarObject(data, funcName))
			return(FALSE);
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0)))
			return(FALSE);
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("McGregor neural cell"));
		if (!InitProcessVariables_Neuron_McGregor(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		dt = _OutSig_EarObject(data)->dt;

		c->dtOverTm = dt / c->membraneTConst_Tm;
		c->condDecay = exp( -dt / c->kDecayTConst_TGk );
		c->threshDecay = exp( -dt / c->thresholdTConst_TTh );
		c->bOverDt = c->delayedRectKCond_b / dt;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (i = outSignal->offset; i < outSignal->numChannels; i++) {
		s = &c->state[i];
		spikeState_s = s->lastSpikeState;
		inPtr = _InSig_EarObject(data, 0)->channel[i];
		outPtr = outSignal->channel[i];
		for (j = 0; j < outSignal->length; j++, outPtr++) {
			totalConductance = 1.0 + s->kConductance_Gk;
			potDecay = exp(-totalConductance * c->dtOverTm);
			s->potential_E = s->potential_E * potDecay + (*inPtr++ +
			  s->kConductance_Gk * c->kEquilibriumPot_Ek) / totalConductance *
			  (1.0 - potDecay);
			s->threshold_Th = s->threshold_Th * c->threshDecay +
			  (c->accomConst_c * s->potential_E + c->restingThreshold_Th0) *
			  (1.0 - c->threshDecay);
			s->lastSpikeState = spikeState_s;
			spikeState_s = (s->potential_E < s->threshold_Th)? 0: 1;
			s->kConductance_Gk = s->kConductance_Gk * c->condDecay;
			*outPtr = s->potential_E + c->cellRestingPot_Er;
			if (spikeState_s) {
				if (s->lastSpikeState < spikeState_s)
					*outPtr = c->actionPotential + c->cellRestingPot_Er;
				s->kConductance_Gk += c->bOverDt * (1 - c->condDecay);
			}
		}
	}
	SetProcessContinuity_EarObject(data);
 	return(TRUE);

} /* RunModel_Neuron_McGregor */
