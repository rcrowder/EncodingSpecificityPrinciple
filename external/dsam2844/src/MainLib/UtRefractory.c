/**********************
 *
 * File:		UtRefractory.c
 * Purpose:		Applies a refractory effect adjustment for spike probabilities.
 *				R. Meddis and M. J. Hewitt (1991)"Virtual pitch and phase
 *				sensitivity of a computer modle of the auditory periphery. 1:
 *				pitch identification" Journal of the Acoustical Society of
 *				America, 89, pp 2866-2882.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard, revised from code by M. J. Hewitt
 * Created:		26 Jun 1996
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
#include "UtRefractory.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

RefractAdjPtr	refractAdjPtr = NULL;

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
Free_Utility_RefractoryAdjust(void)
{

	if (refractAdjPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Utility_RefractoryAdjust();
	if (refractAdjPtr->parList)
		FreeList_UniParMgr(&refractAdjPtr->parList);
	if (refractAdjPtr->parSpec == GLOBAL) {
		free(refractAdjPtr);
		refractAdjPtr = NULL;
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
Init_Utility_RefractoryAdjust(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_RefractoryAdjust");

	if (parSpec == GLOBAL) {
		if (refractAdjPtr != NULL)
			Free_Utility_RefractoryAdjust();
		if ((refractAdjPtr = (RefractAdjPtr) malloc(sizeof(RefractAdj))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (refractAdjPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	refractAdjPtr->parSpec = parSpec;
	refractAdjPtr->updateProcessVariablesFlag = TRUE;
	refractAdjPtr->refractoryPeriod = 0.0;

	if (!SetUniParList_Utility_RefractoryAdjust()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_RefractoryAdjust();
		return(FALSE);
	}
	refractAdjPtr->numChannels = 0;
	refractAdjPtr->lastOutput = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_RefractoryAdjust(void)
{
	static const WChar *funcName = wxT(
	  "SetUniParList_Utility_RefractoryAdjust");
	UniParPtr	pars;

	if ((refractAdjPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_REFRACTORYADJUST_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = refractAdjPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_REFRACTORYADJUST_REFRACTORYPERIOD],
	  wxT("PERIOD"),
	  wxT("Refractory period (s)."),
	  UNIPAR_REAL,
	  &refractAdjPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_Utility_RefractoryAdjust);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_RefractoryAdjust(void)
{
	static const WChar	*funcName = wxT(
	  "GetUniParListPtr_Utility_RefractoryAdjust");

	if (refractAdjPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (refractAdjPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(refractAdjPtr->parList);

}

/****************************** SetRefractoryPeriod ***************************/

/*
 * This function sets the module's refractoryPeriod parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefractoryPeriod_Utility_RefractoryAdjust(Float theRefractoryPeriod)
{
	static const WChar	*funcName =
	  wxT("SetRefractoryPeriod_Utility_RefractoryAdjust");

	if (refractAdjPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero."),
		  funcName);
		return(FALSE);
	}
	refractAdjPtr->updateProcessVariablesFlag = TRUE;
	refractAdjPtr->refractoryPeriod = theRefractoryPeriod;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_RefractoryAdjust(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_RefractoryAdjust");

	if (refractAdjPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("AN Refractory Adjustment Utility Module Parameters:-\n"));
	DPrint(wxT("\tRefractory period = %g ms\n"), MSEC(refractAdjPtr->
	  refractoryPeriod));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_RefractoryAdjust(ModulePtr theModule)
{
	static const WChar	*funcName = wxT(
	  "SetParsPointer_Utility_RefractoryAdjust");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	refractAdjPtr = (RefractAdjPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_RefractoryAdjust(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_RefractoryAdjust");

	if (!SetParsPointer_Utility_RefractoryAdjust(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_RefractoryAdjust(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = refractAdjPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_RefractoryAdjust;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_RefractoryAdjust;
	theModule->PrintPars = PrintPars_Utility_RefractoryAdjust;
	theModule->RunProcess = Process_Utility_RefractoryAdjust;
	theModule->SetParsPointer = SetParsPointer_Utility_RefractoryAdjust;
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
CheckData_Utility_RefractoryAdjust(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_RefractoryAdjust");
	SignalDataPtr	inSignal;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	inSignal = _InSig_EarObject(data, 0);
	if (refractAdjPtr->refractoryPeriod >= _GetDuration_SignalData(inSignal)) {
		NotifyError(wxT("%s: Refractory period (%g ms) is too long for signal ")
		  wxT("length (%g ms)."), funcName, MSEC(refractAdjPtr->
		  refractoryPeriod), MSEC(_GetDuration_SignalData(inSignal)));
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * The initial input probabilities assume preceeding silence.
 */

BOOLN
InitProcessVariables_Utility_RefractoryAdjust(EarObjectPtr data)
{
	static const WChar *funcName =
	  wxT("InitProcessVariables_Utility_RefractoryAdjust");
	int		i, j;
	RefractAdjPtr	p = refractAdjPtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		p->refractoryPeriodIndex = (ChanLen) (p->refractoryPeriod / outSignal->
		  dt + 0.5);
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Utility_RefractoryAdjust();
		  	p->numChannels = outSignal->numChannels;
			if ((p->lastOutput = (Float **) calloc(p->numChannels, sizeof(
			  Float *))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for 'lastOutput pointers'."),
			 	  funcName);
			 	return(FALSE);
			}
			for (i = 0; i < p->numChannels; i++)
				if ((p->lastOutput[i] = (Float *) calloc(p->
				  refractoryPeriodIndex, sizeof(Float))) == NULL) {
					NotifyError(wxT("%s: Out of memory for 'lastOutput ")
					  wxT("arrays'."), funcName);
					for (j = 0; j < i - 1; j++)
						free(p->lastOutput[j]);
					free(p->lastOutput);
					p->lastOutput = NULL;
					return(FALSE);
				}
			p->updateProcessVariablesFlag = FALSE;
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
FreeProcessVariables_Utility_RefractoryAdjust(void)
{
	int		i;

	if (refractAdjPtr->lastOutput != NULL) {
		for (i = 0; i < refractAdjPtr->numChannels; i++)
			free(refractAdjPtr->lastOutput[i]);
		free(refractAdjPtr->lastOutput);
		refractAdjPtr->lastOutput = NULL;
	}
	refractAdjPtr->updateProcessVariablesFlag = TRUE;

}

/****************************** Process ***************************************/

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
Process_Utility_RefractoryAdjust(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_Utility_RefractoryAdjust");
	register	ChanData	 *outPtr, *sumPtr;
	register	Float		sum;
	int		chan;
	Float	*lastOutputPtr;
	ChanLen	i, j;
	RefractAdjPtr	p = refractAdjPtr;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_RefractoryAdjust(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Meddis 91 AN Refractory Adustment ")
		  wxT("Process"));
		if (!InitOutTypeFromInSignal_EarObject(data, 0)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Utility_RefractoryAdjust(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	InitOutDataFromInSignal_EarObject(data);
	outSignal = _OutSig_EarObject(data);
	if (data->timeIndex == PROCESS_START_TIME) {
		for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
			outPtr = p->lastOutput[chan];
			for (i = 0; i < p->refractoryPeriodIndex; i++)
				*outPtr++ = 0.0;
		}
	}
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++) {
			sum = 0.0;
			if (i < p->refractoryPeriodIndex) {
				lastOutputPtr = p->lastOutput[chan] + i;
				for (j = i; j < p->refractoryPeriodIndex; j++)
					sum += *lastOutputPtr++;
				sumPtr = outSignal->channel[chan];
			} else
				sumPtr = outPtr - p->refractoryPeriodIndex;
			while (sumPtr < outPtr)
				sum += *sumPtr++;
			*outPtr++ *= (1.0 - sum);
		}
		lastOutputPtr = p->lastOutput[chan] + p->refractoryPeriodIndex;
		for (i = 0; i < p->refractoryPeriodIndex; i++)
			*--lastOutputPtr = *--outPtr;
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

