/**********************
 *
 * File:		MoHCRPMeddis.c
 * Purpose:		This module contains the model for the hair cell receptor
 *				potential.
 * Comments:	07-05-98 LPO: This model has now changed to a new version.  It
 *				uses the Meddis 86 permeability function with the low-pass
 *				filter.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "MoHCRPMeddis.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

MeddisRPPtr	meddisRPPtr = NULL;

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
Init_IHCRP_Meddis(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_IHCRP_Meddis");

	if (parSpec == GLOBAL) {
		if (meddisRPPtr != NULL)
			Free_IHCRP_Meddis();
		if ((meddisRPPtr = (MeddisRPPtr) malloc(sizeof(MeddisRP))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (meddisRPPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	meddisRPPtr->parSpec = parSpec;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	meddisRPPtr->permConst_A = 100.0;
	meddisRPPtr->permConst_B = 6000.0;
	meddisRPPtr->releaseRate_g = 2000.0;
	meddisRPPtr->mTimeConst_tm = 0.1e-3;

	if (!SetUniParList_IHCRP_Meddis()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHCRP_Meddis();
		return(FALSE);
	}
	meddisRPPtr->lastOutput = NULL;
	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface reaAuires that a non-void value be returned.
 */

BOOLN
Free_IHCRP_Meddis(void)
{
	if (meddisRPPtr == NULL)
		return(TRUE);
	FreeProcessVariables_IHCRP_Meddis();
	if (meddisRPPtr->parList)
		FreeList_UniParMgr(&meddisRPPtr->parList);
	if (meddisRPPtr->parSpec == GLOBAL) {
		free(meddisRPPtr);
		meddisRPPtr = NULL;
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
SetUniParList_IHCRP_Meddis(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHCRP_Meddis");
	UniParPtr	pars;

	if ((meddisRPPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_MEDDIS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = meddisRPPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_MEDDIS_PERMCONST_A], wxT("PERM_CONST_A"),
	  wxT("Permeability constant A (units/s)."),
	  UNIPAR_REAL,
	  &meddisRPPtr->permConst_A, NULL,
	  (void * (*)) SetPermConstA_IHCRP_Meddis);
	SetPar_UniParMgr(&pars[IHCRP_MEDDIS_PERMCONST_B], wxT("PERM_CONST_B"),
	  wxT("Permeability constant B (units/s)."),
	  UNIPAR_REAL,
	  &meddisRPPtr->permConst_B, NULL,
	  (void * (*)) SetPermConstB_IHCRP_Meddis);
	SetPar_UniParMgr(&pars[IHCRP_MEDDIS_RELEASERATE_G], wxT("RELEASE_G"),
	  wxT("Release rate (units/s)."),
	  UNIPAR_REAL,
	  &meddisRPPtr->releaseRate_g, NULL,
	  (void * (*)) SetReleaseRate_IHCRP_Meddis);
	SetPar_UniParMgr(&pars[IHCRP_MEDDIS_MTIMECONST_TM], wxT("TIME_CONST_TM"),
	  wxT("Receptor potential time constant for IHC model (s)."),
	  UNIPAR_REAL,
	  &meddisRPPtr->mTimeConst_tm, NULL,
	  (void * (*)) SetMTimeConstTm_IHCRP_Meddis);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHCRP_Meddis(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHCRP_Meddis");

	if (meddisRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (meddisRPPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(meddisRPPtr->parList);

}

/********************************* SetPermConstA ******************************/

/*
 * This function sets the module's , A, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermConstA_IHCRP_Meddis(Float thePermConstA)
{
	static const WChar	 *funcName = wxT("SetPermConstA_IHCRP_Meddis");

	if (meddisRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	meddisRPPtr->permConst_A = thePermConstA;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPermConstB ******************************/

/*
 * This function sets the module's amplitude scale, B, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermConstB_IHCRP_Meddis(Float thePermConstB)
{
	static const WChar	 *funcName = wxT("SetPermConstB_IHCRP_Meddis");

	if (meddisRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	meddisRPPtr->permConst_B = thePermConstB;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReleaseRate *****************************/

/*
 * This function sets the module's release rate constant, r, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReleaseRate_IHCRP_Meddis(Float theReleaseRate)
{
	static const WChar	*funcName = wxT("SetReleaseRate_IHCRP_Meddis");

	if (meddisRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	meddisRPPtr->releaseRate_g = theReleaseRate;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMTimeConstTm ****************************/

/*
 * This function sets the module's basilar membrane time constant, tm,
 * parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMTimeConstTm_IHCRP_Meddis(Float theMTimeConstTm)
{
	static const WChar	 *funcName = wxT("SetMTimeConstTm_IHCRP_Meddis");

	if (meddisRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	meddisRPPtr->mTimeConst_tm = theMTimeConstTm;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_IHCRP_Meddis(void)
{
	DPrint(wxT("Receptor Potential Module Parameters:-\n"));
	DPrint(wxT("\tPermeability constant, A = %g,\t"), meddisRPPtr->permConst_A);
	DPrint(wxT("\tPermeability constant, B = %g,\n"), meddisRPPtr->permConst_B);
	DPrint(wxT("\tRelease rate, g = %g,\t"), meddisRPPtr->releaseRate_g);
	DPrint(wxT("Time constant = %g ms\n"), MSEC(meddisRPPtr->mTimeConst_tm));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHCRP_Meddis(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHCRP_Meddis");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	meddisRPPtr = (MeddisRPPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHCRP_Meddis(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHCRP_Meddis");

	if (!SetParsPointer_IHCRP_Meddis(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_Meddis(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = meddisRPPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHCRP_Meddis;
	theModule->GetUniParListPtr = GetUniParListPtr_IHCRP_Meddis;
	theModule->PrintPars = PrintPars_IHCRP_Meddis;
	theModule->RunProcess = RunModel_IHCRP_Meddis;
	theModule->SetParsPointer = SetParsPointer_IHCRP_Meddis;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the input signal is correctly initialised
 * (using CheckRamp_SignalData), and determines whether the parameter values
 * are valid for the signal sampling interval.
 */

BOOLN
CheckData_IHCRP_Meddis(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_IHCRP_Meddis");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (meddisRPPtr->mTimeConst_tm <= _InSig_EarObject(data, 0)->dt / 2.0) {
		NotifyError(wxT("%s: Membrane time constant (%g ms) is too small it ")
		  wxT("must\nbe greater than %g ms"), funcName, MILLI(meddisRPPtr->
		  mTimeConst_tm),
		  MILLI(_InSig_EarObject(data, 0)->dt / 2.0));
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * The spontaneous probability assumes preceeding silence.
 * Initial, spontaneous probability calculation: 1st sample is set to zero.
 * This value is used by the hair cell reservoir models.
 */

BOOLN
InitProcessVariables_IHCRP_Meddis(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_IHCRP_Meddis");
	int		i;
	Float	spontPerm_k0;
	MeddisRPPtr p = meddisRPPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_IHCRP_Meddis();
			if ((p->lastOutput = (Float *) calloc(_OutSig_EarObject(data)->numChannels,
			   sizeof(Float))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory."), funcName);
			 	return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		spontPerm_k0 = p->releaseRate_g * p->permConst_A / (p->permConst_A +
		  p->permConst_B);
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++)
			p->lastOutput[i] = spontPerm_k0;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_IHCRP_Meddis(void)
{

	if (meddisRPPtr->lastOutput == NULL)
		return;
	free(meddisRPPtr->lastOutput);
	meddisRPPtr->lastOutput = NULL;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

#define	PERMEABILITY(INPUT)			(((st_Plus_A = (INPUT) + p->permConst_A) \
		  > 0.0)? p->releaseRate_g * st_Plus_A / (st_Plus_A + \
		  meddisRPPtr->permConst_B): 0.0)

#define LOWPASSFILTER(LASTOUTPUT)	((ChanData) ((LASTOUTPUT) + \
		  (permeability_K - (LASTOUTPUT)) * p->dtOverTm))

BOOLN
RunModel_IHCRP_Meddis(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_IHCRP_Meddis");
	register ChanData	*inPtr, *outPtr;
	register Float		permeability_K, st_Plus_A;
	int		chan;
	ChanLen	i;
	SignalDataPtr	outSignal;
	MeddisRPPtr p = meddisRPPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_IHCRP_Meddis(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Hair cell receptor potential"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_IHCRP_Meddis(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->dtOverTm = _OutSig_EarObject(data)->dt / p->mTimeConst_tm;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		permeability_K = PERMEABILITY(*inPtr++);
		*outPtr++ = LOWPASSFILTER(p->lastOutput[chan]);
		/* Probability calculation for the rest of the signal. */
		for (i = 1; i < outSignal->length; i++) {
			permeability_K = PERMEABILITY(*inPtr++);
			*outPtr = LOWPASSFILTER(*(outPtr - 1));
			outPtr++;	/* Compiler complains if things not done this way. */
		}
		p->lastOutput[chan] = *(outPtr - 1);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* RunModel_IHCRP_Meddis */

#undef PERMEABILITY
#undef LOWPASSFILTER
