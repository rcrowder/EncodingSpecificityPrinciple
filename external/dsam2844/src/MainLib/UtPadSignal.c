/**********************
 *
 * File:		UtPadSignal.c
 * Purpose:		This module produces an output signal which has been padded
 *				at the beginning and end of the signal with a specified value.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 * Author:		L. P. O'Mard
 * Created:		14 Mar 2002
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
#include <string.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtPadSignal.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

PadSignalPtr	padSignalPtr = NULL;

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
Free_Utility_PadSignal(void)
{
	if (padSignalPtr == NULL)
		return(FALSE);
	if (padSignalPtr->parList)
		FreeList_UniParMgr(&padSignalPtr->parList);
	if (padSignalPtr->parSpec == GLOBAL) {
		free(padSignalPtr);
		padSignalPtr = NULL;
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
Init_Utility_PadSignal(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_PadSignal");

	if (parSpec == GLOBAL) {
		if (padSignalPtr != NULL)
			Free_Utility_PadSignal();
		if ((padSignalPtr = (PadSignalPtr) malloc(sizeof(PadSignal))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (padSignalPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	padSignalPtr->parSpec = parSpec;
	padSignalPtr->beginDuration = 0.0;
	padSignalPtr->beginValue = 0.0;
	padSignalPtr->endDuration = 0.0;
	padSignalPtr->endValue = 0.0;

	if (!SetUniParList_Utility_PadSignal()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_PadSignal();
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
SetUniParList_Utility_PadSignal(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_PadSignal");
	UniParPtr	pars;

	if ((padSignalPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_PADSIGNAL_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = padSignalPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_PADSIGNAL_BEGINDURATION], wxT(
	  "BEGIN_DURATION"),
	  wxT("The pad duration for the beginning of the signal (s)."),
	  UNIPAR_REAL,
	  &padSignalPtr->beginDuration, NULL,
	  (void * (*)) SetBeginDuration_Utility_PadSignal);
	SetPar_UniParMgr(&pars[UTILITY_PADSIGNAL_BEGINVALUE], wxT("BEGIN_VALUE"),
	  wxT("The pad value for the beginning of the signal (units)."),
	  UNIPAR_REAL,
	  &padSignalPtr->beginValue, NULL,
	  (void * (*)) SetBeginValue_Utility_PadSignal);
	SetPar_UniParMgr(&pars[UTILITY_PADSIGNAL_ENDDURATION], wxT("END_DURATION"),
	  wxT("The pad duration for the end of the signal (s)."),
	  UNIPAR_REAL,
	  &padSignalPtr->endDuration, NULL,
	  (void * (*)) SetEndDuration_Utility_PadSignal);
	SetPar_UniParMgr(&pars[UTILITY_PADSIGNAL_ENDVALUE], wxT("END_VALUE"),
	  wxT("The pad value for the end of the signal (units)."),
	  UNIPAR_REAL,
	  &padSignalPtr->endValue, NULL,
	  (void * (*)) SetEndValue_Utility_PadSignal);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_PadSignal(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_PadSignal");

	if (padSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (padSignalPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(padSignalPtr->parList);

}

/****************************** SetBeginDuration ******************************/

/*
 * This function sets the module's beginDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBeginDuration_Utility_PadSignal(Float theBeginDuration)
{
	static const WChar	*funcName = wxT("SetBeginDuration_Utility_PadSignal");

	if (padSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theBeginDuration < 0.0) {
		NotifyError(wxT("%s: This value cannot be negative (%g)."), funcName,
		  theBeginDuration);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	padSignalPtr->beginDuration = theBeginDuration;
	return(TRUE);

}

/****************************** SetBeginValue *********************************/

/*
 * This function sets the module's beginValue parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBeginValue_Utility_PadSignal(Float theBeginValue)
{
	static const WChar	*funcName = wxT("SetBeginValue_Utility_PadSignal");

	if (padSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	padSignalPtr->beginValue = theBeginValue;
	return(TRUE);

}

/****************************** SetEndDuration ********************************/

/*
 * This function sets the module's endDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEndDuration_Utility_PadSignal(Float theEndDuration)
{
	static const WChar	*funcName = wxT("SetEndDuration_Utility_PadSignal");

	if (padSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theEndDuration < 0.0) {
		NotifyError(wxT("%s: This value cannot be negative (%g)."), funcName,
		  theEndDuration);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	padSignalPtr->endDuration = theEndDuration;
	return(TRUE);

}

/****************************** SetEndValue ***********************************/

/*
 * This function sets the module's endValue parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEndValue_Utility_PadSignal(Float theEndValue)
{
	static const WChar	*funcName = wxT("SetEndValue_Utility_PadSignal");

	if (padSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	padSignalPtr->endValue = theEndValue;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_PadSignal(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_PadSignal");

	if (padSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Signal padding Utility Module Parameters:-\n"));
	DPrint(wxT("\tBegin duration = %g ms,"), MILLI(padSignalPtr->
	  beginDuration));
	DPrint(wxT("\tBegin value = %g (units)\n"), padSignalPtr->beginValue);
	DPrint(wxT("\tEnd duration = %g ms,"), MILLI(padSignalPtr->endDuration));
	DPrint(wxT("\tEnd value = %g ((units.)\n"), padSignalPtr->endValue);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_PadSignal(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_PadSignal");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	padSignalPtr = (PadSignalPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_PadSignal(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_PadSignal");

	if (!SetParsPointer_Utility_PadSignal(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_PadSignal(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = padSignalPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_PadSignal;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_PadSignal;
	theModule->PrintPars = PrintPars_Utility_PadSignal;
	theModule->RunProcess = Process_Utility_PadSignal;
	theModule->SetParsPointer = SetParsPointer_Utility_PadSignal;
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
CheckData_Utility_PadSignal(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_PadSignal");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

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
Process_Utility_PadSignal(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_PadSignal");
	register ChanData	 *inPtr, *outPtr;
	int		chan;
	Float	dt;
	ChanLen	i;
	SignalDataPtr	outSignal;
	PadSignalPtr	p = padSignalPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_PadSignal(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Signal padding Module process"));

		dt = _InSig_EarObject(data, 0)->dt;
		p->beginDurationIndex = (ChanLen) floor(p->beginDuration / dt + 0.05);
		p->endDurationIndex = (ChanLen) floor(p->endDuration / dt + 0.05);
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  p->beginDurationIndex + _InSig_EarObject(data, 0)->length +
		  p->endDurationIndex, dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		for (i = 0; i < p->beginDurationIndex; i++)
			*outPtr++ = p->beginValue;
		for (i = 0; i < _InSig_EarObject(data, 0)->length; i++)
			*outPtr++ = *inPtr++;
		for (i = 0; i < p->endDurationIndex; i++)
			*outPtr++ = p->endValue;
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

