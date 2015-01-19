/**********************
 *
 * File:		UtTranspose.c
 * Purpose:		This module takes an input signal and converts the n channels
 *				of m samples to m channels of n samples.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 *				02-03-05 LPO: This module is not being thread enabled as the
 *				number of input channels defines the channel processing.
 * Author:		L. P. O'Mard
 * Created:		13 Jul 2004
 * Updated:
 * Copyright:	(c) 2005, 2010 Lowel P. O'Mard
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
#include <float.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtTranspose.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

TransposePtr	transposePtr = NULL;

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
Free_Utility_Transpose(void)
{

	if (transposePtr == NULL)
		return(FALSE);
	if (transposePtr->parList)
		FreeList_UniParMgr(&transposePtr->parList);
	if (transposePtr->parSpec == GLOBAL) {
		free(transposePtr);
		transposePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This function initialises the 'mode' list array
 */

NameSpecifier *
ModeList_Utility_Transpose(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("STANDARD"),		UTILITY_TRANSPOSE_STANDARD_MODE },
			{ wxT("FIRST_CHANNEL"),	UTILITY_TRANSPOSE_FIRST_CHANNEL_MODE },
			{ 0,					UTILITY_TRANSPOSE_MODE_NULL },
		};
	return(&modeList[index]);

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
Init_Utility_Transpose(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_Transpose");

	if (parSpec == GLOBAL) {
		if (transposePtr != NULL)
			Free_Utility_Transpose();
		if ((transposePtr = (TransposePtr) malloc(sizeof(Transpose))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (transposePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	transposePtr->parSpec = parSpec;
	transposePtr->mode = UTILITY_TRANSPOSE_STANDARD_MODE;

	if (!SetUniParList_Utility_Transpose()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_Transpose();
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
SetUniParList_Utility_Transpose(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_Transpose");
	UniParPtr	pars;

	if ((transposePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_TRANSPOSE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = transposePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_TRANSPOSE_MODE], wxT("MODE"),
	  wxT("Sample labelling mode ('standard' or 'first channel'."),
	  UNIPAR_NAME_SPEC,
	  &transposePtr->mode, ModeList_Utility_Transpose(0),
	  (void * (*)) SetMode_Utility_Transpose);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_Transpose(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_Transpose");

	if (transposePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (transposePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(transposePtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_Transpose(WChar * theMode)
{
	static const WChar	*funcName = wxT("SetMode_Utility_Transpose");
	int		specifier;

	if (transposePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
			ModeList_Utility_Transpose(0))) == UTILITY_TRANSPOSE_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	transposePtr->mode = specifier;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_Transpose(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_Transpose");

	if (transposePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Transpose Utility Module Parameters:-\n"));
	DPrint(wxT("\tSample labelling mode = %s\n"), ModeList_Utility_Transpose(
	  transposePtr->mode)->name);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_Transpose(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_Transpose");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	transposePtr = (TransposePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_Transpose(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_Transpose");

	if (!SetParsPointer_Utility_Transpose(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_Transpose(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = transposePtr;
	theModule->Free = Free_Utility_Transpose;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_Transpose;
	theModule->PrintPars = PrintPars_Utility_Transpose;
	theModule->RunProcess = Process_Utility_Transpose;
	theModule->SetParsPointer = SetParsPointer_Utility_Transpose;
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
CheckData_Utility_Transpose(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_Transpose");

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
Process_Utility_Transpose(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_Transpose");
	register ChanData	 *inPtr;
	BOOLN	regularLabels;
	int		i, chan;
	Float	*chanLabel, delta1, newDt, timeOffset;
	SignalDataPtr	inSignal, outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_Transpose(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Transpose Utility Module process"));
		chanLabel = _InSig_EarObject(data, 0)->info.chanLabel;
		if ((_InSig_EarObject(data, 0)->numChannels > 2)) {
			delta1 = chanLabel[1] - chanLabel[0];
			regularLabels = (!DBL_GREATER(delta1, chanLabel[2] - chanLabel[1]));
		} else
			regularLabels = FALSE;
		if ((transposePtr->mode == UTILITY_TRANSPOSE_STANDARD_MODE) &&
		  regularLabels) {
			newDt = delta1;
			timeOffset = chanLabel[0];
		} else {
			newDt = 1.0;
			timeOffset = 0.0;
		}

		if (!InitOutSignal_EarObject(data, (uShort) _InSig_EarObject(data, 0)->
		  length, (ChanLen) _InSig_EarObject(data, 0)->numChannels, newDt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), timeOffset);
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoChanDataTitle_SignalData(_OutSig_EarObject(data),
		  _InSig_EarObject(data, 0)->info.sampleTitle);
		SetInfoSampleTitle_SignalData(_OutSig_EarObject(data), _InSig_EarObject(
		  data, 0)->info.chanDataTitle);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	for (chan = 0; chan < inSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan];
		for (i = 0; i < outSignal->numChannels; i++)
			outSignal->channel[i][chan] = *inPtr++;
	}
	for (chan = 0; chan < outSignal->numChannels; chan++)
		outSignal->info.chanLabel[chan] = chan * inSignal->dt;

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
