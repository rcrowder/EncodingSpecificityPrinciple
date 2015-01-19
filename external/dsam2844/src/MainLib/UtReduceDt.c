/**********************
 *
 * File:		UtReduceDt.c
 * Purpose:		This routine produces an output signal whose sampling rate is
 *				increased by reducing the sampling interval of the input signal
 *				by a specified denominator.
 * Comments:	Written using ModuleProducer version 1.8.
 *				The length of the signal will be increased accordingly.
 *				A new output signal is created, if required, or the old output
 *				signal is overwritten.
 * Author:		L. P. O'Mard
 * Created:		21 Dec 1995
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
#include "UtReduceDt.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ReduceDtPtr	reduceDtPtr = NULL;

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
Free_Utility_ReduceDt(void)
{
	if (reduceDtPtr == NULL)
		return(FALSE);
	if (reduceDtPtr->parList)
		FreeList_UniParMgr(&reduceDtPtr->parList);
	if (reduceDtPtr->parSpec == GLOBAL) {
		free(reduceDtPtr);
		reduceDtPtr = NULL;
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
Init_Utility_ReduceDt(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_ReduceDt");

	if (parSpec == GLOBAL) {
		if (reduceDtPtr != NULL)
			Free_Utility_ReduceDt();
		if ((reduceDtPtr = (ReduceDtPtr) malloc(sizeof(ReduceDt))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (reduceDtPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	reduceDtPtr->parSpec = parSpec;
	reduceDtPtr->denominator = 1;

	if (!SetUniParList_Utility_ReduceDt()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_ReduceDt();
		return(FALSE);
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
SetUniParList_Utility_ReduceDt(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_ReduceDt");
	UniParPtr	pars;

	if ((reduceDtPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_REDUCEDT_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = reduceDtPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_REDUCEDT_DENOMINATOR], wxT("DENOMINATOR"),
	  wxT("Reduction denominator (integer)."),
	  UNIPAR_INT,
	  &reduceDtPtr->denominator, NULL,
	  (void * (*)) SetDenominator_Utility_ReduceDt);

	SetAltAbbreviation_UniParMgr(&pars[UTILITY_REDUCEDT_DENOMINATOR],
	  wxT("DENOMINTOR"));
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_ReduceDt(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_ReduceDt");

	if (reduceDtPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (reduceDtPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(reduceDtPtr->parList);

}

/****************************** SetDenominator ********************************/

/*
 * This function sets the module's denominator parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDenominator_Utility_ReduceDt(int theDenominator)
{
	static const WChar	*funcName =
	  wxT("SetDenominator_Utility_ReduceDt");

	if (reduceDtPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theDenominator < 1) {
		NotifyError(wxT("%s: Illegal denomitor = %d; valid values are\n")
		  wxT("greater than 0."), funcName, theDenominator);
		return(FALSE);
	}
	reduceDtPtr->denominator = theDenominator;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_ReduceDt(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_ReduceDt");

	if (reduceDtPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Reduce Sampling Interval, dt, Utility Module parameters:-\n"));
	DPrint(wxT("\tReduction denominator = %d.\n"),
	  reduceDtPtr->denominator);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_ReduceDt(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_ReduceDt");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	reduceDtPtr = (ReduceDtPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_ReduceDt(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_ReduceDt");

	if (!SetParsPointer_Utility_ReduceDt(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_ReduceDt(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = reduceDtPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_ReduceDt;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_ReduceDt;
	theModule->PrintPars = PrintPars_Utility_ReduceDt;
	theModule->ResetProcess = ResetProcess_Utility_ReduceDt;
	theModule->RunProcess = Process_Utility_ReduceDt;
	theModule->SetParsPointer = SetParsPointer_Utility_ReduceDt;
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
CheckData_Utility_ReduceDt(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_ReduceDt");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Utility_ReduceDt(EarObjectPtr data)
{
	ResetOutSignal_EarObject(data);

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
Process_Utility_ReduceDt(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_ReduceDt");
	register	ChanData	 *inPtr, *outPtr;
	int		chan, j;
	Float	gradient;
	ChanLen	i;
	SignalDataPtr	outSignal;
	ReduceDtPtr	p = reduceDtPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_ReduceDt(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Module process ??"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length * p->denominator, _InSig_EarObject(data, 0)->dt /
		  p->denominator)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		gradient = 0.0;
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		for (i = 0; i < _InSig_EarObject(data, 0)->length - 1; i++, inPtr++) {
			gradient = (*(inPtr + 1) - *inPtr) / p->denominator;
			for (j = 0; j < p->denominator; j++)
				*(outPtr++) = *inPtr + gradient * j;
		}
		/* Use the previous gradient for the last input sample. */
		for (j = 0; j < p->denominator; j++)
			*(outPtr++) = *inPtr + gradient * j;
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

