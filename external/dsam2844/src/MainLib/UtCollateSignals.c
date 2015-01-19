/**********************
 *
 * File:		UtCollateSignals.c
 * Purpose:		This utility produces and output signal with a number of samples
 *				equal to the sum of the input signals.  All input signals
 *				will need to have the same number of channels, and this will define
 *				the number of output channels.
 * Comments:	Written using ModuleProducer version 1.5.0 (May  2 2007).
 * Author:		L. P. O'Mard
 * Created:		02 May 2007
 * Updated:
 * Copyright:	(c) 2010 Lowel P. O'Mard
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

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtCollateSignals.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CollateSigsPtr	collateSigsPtr = NULL;

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
Free_Utility_CollateSignals(void)
{
	/* static const WChar	*funcName = wxT("Free_Utility_CollateSignals"); */

	if (collateSigsPtr == NULL)
		return(FALSE);
	if (collateSigsPtr->labels) {
		free(collateSigsPtr->labels);
		collateSigsPtr->labels = NULL;
	}
	if (collateSigsPtr->parList)
		FreeList_UniParMgr(&collateSigsPtr->parList);
	if (collateSigsPtr->parSpec == GLOBAL) {
		free(collateSigsPtr);
		collateSigsPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitSummationModeList *************************/

/*
 * This function initialises the 'summationMode' list array
 */

BOOLN
InitSummationModeList_Utility_CollateSignals(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("AVERAGE"),	UTILITY_COLLATESIGNALS_SUMMATIONMODE_AVERAGE },
			{ wxT("SUM"),		UTILITY_COLLATESIGNALS_SUMMATIONMODE_SUM },
			{ NULL,				UTILITY_COLLATESIGNALS_SUMMATIONMODE_NULL },
		};
	collateSigsPtr->summationModeList = modeList;
	return(TRUE);

}

/****************************** InitLabelModeList *****************************/

/*
 * This function initialises the 'labelMode' list array
 */

BOOLN
InitLabelModeList_Utility_CollateSignals(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("CHAN_INDEX"),	UTILITY_COLLATESIGNALS_LABELMODE_CHAN_INDEX },
			{ wxT("INPUT_LABELS"),UTILITY_COLLATESIGNALS_LABELMODE_INPUT_LABELS },
			{ wxT("USER"),			UTILITY_COLLATESIGNALS_LABELMODE_USER },
			{ NULL,					UTILITY_COLLATESIGNALS_LABELMODE_NULL },
		};
	collateSigsPtr->labelModeList = modeList;
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
Init_Utility_CollateSignals(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_CollateSignals");

	if (parSpec == GLOBAL) {
		if (collateSigsPtr != NULL)
			Free_Utility_CollateSignals();
		if ((collateSigsPtr = (CollateSigsPtr) malloc(sizeof(CollateSigs))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (collateSigsPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	collateSigsPtr->parSpec = parSpec;
	collateSigsPtr->summationMode = UTILITY_COLLATESIGNALS_SUMMATIONMODE_SUM;
	collateSigsPtr->labelMode = UTILITY_COLLATESIGNALS_LABELMODE_CHAN_INDEX;
	collateSigsPtr->labels = NULL;

	InitSummationModeList_Utility_CollateSignals();
	InitLabelModeList_Utility_CollateSignals();
	if (!SetUniParList_Utility_CollateSignals()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_CollateSignals();
		return(FALSE);
	}
	collateSigsPtr->length = 0;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_CollateSignals(void)
{
	static const WChar	*funcName = wxT("SetUniParList_Utility_CollateSignals");
	UniParPtr	pars;

	if ((collateSigsPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_COLLATESIGNALS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = collateSigsPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_COLLATESIGNALS_SUMMATIONMODE], wxT("SUM_MODE"),
	  wxT("Summation mode - 'average' or simple 'sum'."),
	  UNIPAR_NAME_SPEC,
	  &collateSigsPtr->summationMode, collateSigsPtr->summationModeList,
	  (void * (*)) SetSummationMode_Utility_CollateSignals);
	SetPar_UniParMgr(&pars[UTILITY_COLLATESIGNALS_LABELMODE], wxT("LABEL_MODE"),
	  wxT("Channel labelling mode ('chan_index', 'input_labels' or 'user')."),
	  UNIPAR_NAME_SPEC,
	  &collateSigsPtr->labelMode, collateSigsPtr->labelModeList,
	  (void * (*)) SetLabelMode_Utility_CollateSignals);
	SetPar_UniParMgr(&pars[UTILITY_COLLATESIGNALS_LABELS], wxT("LABEL"),
	  wxT("Channel label array (floating point values)."),
	  UNIPAR_REAL_DYN_ARRAY,
	  &collateSigsPtr->labels, &collateSigsPtr->length,
	  (void * (*)) SetIndividualLabel_Utility_CollateSignals);

	SetEnabledState_Utility_CollateSignals();
	return(TRUE);

}

/****************************** SetEnabledState *******************************/

/*
 * This routine sets the enabled states.
 */

void
SetEnabledState_Utility_CollateSignals(void)
{
	collateSigsPtr->parList->pars[UTILITY_COLLATESIGNALS_LABELS].enabled =
	  (collateSigsPtr->labelMode == UTILITY_COLLATESIGNALS_LABELMODE_USER);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_CollateSignals(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_CollateSignals");

	if (collateSigsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (collateSigsPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(collateSigsPtr->parList);

}

/****************************** SetSummationMode ******************************/

/*
 * This function sets the module's summationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSummationMode_Utility_CollateSignals(WChar * theSummationMode)
{
	static const WChar	*funcName = wxT(
	  "SetSummationMode_Utility_CollateSignals");
	int		specifier;

	if (collateSigsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSummationMode,
		collateSigsPtr->summationModeList)) == UTILITY_COLLATESIGNALS_SUMMATIONMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSummationMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	collateSigsPtr->summationMode = specifier;
	return(TRUE);

}

/****************************** SetLabelMode **********************************/

/*
 * This function sets the module's labelMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLabelMode_Utility_CollateSignals(WChar * theLabelMode)
{
	static const WChar	*funcName = wxT("SetLabelMode_Utility_CollateSignals");
	int		specifier;

	if (collateSigsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theLabelMode,
		collateSigsPtr->labelModeList)) == UTILITY_COLLATESIGNALS_LABELMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theLabelMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	collateSigsPtr->labelMode = specifier;
	SetEnabledState_Utility_CollateSignals();
	return(TRUE);

}

/****************************** SetLabels *************************************/

/*
 * This function sets the module's labels array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLabels_Utility_CollateSignals(Float *theLabels)
{
	static const WChar	*funcName = wxT("SetLabels_Utility_CollateSignals");

	if (collateSigsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	collateSigsPtr->labels = theLabels;
	return(TRUE);

}

/****************************** SetIndividualLabel ****************************/

/*
 * This function sets the module's 'labels' array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualLabel_Utility_CollateSignals(int theIndex, Float theLabel)
{
	static const WChar *funcName =
	  wxT("SetIndividualLabelArray_Utility_CollateSignals");

	if (collateSigsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (collateSigsPtr->labelMode != UTILITY_COLLATESIGNALS_LABELMODE_USER) {
		NotifyError(wxT("%s: Channels can only be selected in 'user' Label ")
		  wxT("mode."), funcName);
		return(FALSE);
	}
	if ((theIndex > collateSigsPtr->length - 1) &&
	  !ResizeFloatArray_UniParMgr(&collateSigsPtr->labels,
	    &collateSigsPtr->length, theIndex + 1)) {
		NotifyError(wxT("%s: Could not resize the 'labels' array."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	collateSigsPtr->labels[theIndex] = theLabel;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_CollateSignals(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_CollateSignals");
	int		i;

	if (collateSigsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Collate Utility Module Parameters:-\n"));
	DPrint(wxT("\tsummationMode = %s \n"), collateSigsPtr->summationModeList[
	  collateSigsPtr->summationMode].name);
	if (collateSigsPtr->labelMode == UTILITY_COLLATESIGNALS_LABELMODE_USER) {
		DPrint(wxT("\tLabel Array:\n"));
		for (i = 0; i < collateSigsPtr->length; i++)
			DPrint(wxT("\tLabel[%2d]: %g\n"), i, collateSigsPtr->labels[i]);
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_CollateSignals(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_CollateSignals");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	collateSigsPtr = (CollateSigsPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_CollateSignals(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_CollateSignals");

	if (!SetParsPointer_Utility_CollateSignals(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_CollateSignals(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = collateSigsPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_CollateSignals;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_CollateSignals;
	theModule->PrintPars = PrintPars_Utility_CollateSignals;
	theModule->RunProcess = Process_Utility_CollateSignals;
	theModule->SetParsPointer = SetParsPointer_Utility_CollateSignals;
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
CheckData_Utility_CollateSignals(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_CollateSignals");
	int		i;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	for (i = 1; i < data->numInSignals; i++) {
		if ((_InSig_EarObject(data, 0)->numChannels != _InSig_EarObject(data, i)->
		  numChannels) || (_InSig_EarObject(data, 0)->interleaveLevel !=
		  _InSig_EarObject(data, i)->interleaveLevel)) {
			NotifyError(wxT("%s: All input signals must have the same channel ")
			  wxT("characteristics, i.e. no. of channels etc.."), funcName);
			return(FALSE);
		}
	}
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
Process_Utility_CollateSignals(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_CollateSignals");
	register ChanData	 *inPtr, *outPtr, sum;
	int		chan;
	ChanLen	i, j;
	SignalDataPtr	inSignal, outSignal;
	CollateSigsPtr	p = collateSigsPtr;

	inSignal = _InSig_EarObject(data, 0);
	if (!data->threadRunFlag) {
		if (!CheckData_Utility_CollateSignals(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Collate signals utility module"));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels, data->numInSignals,
		  1.0)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), inSignal->interleaveLevel);
		SetStaticTimeFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), 1.0);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		outSignal->info.chanLabel[chan] = _InSig_EarObject(data, 0)->info.chanLabel[chan];
		for (i = 0; i < data->outSignal->length; i++) {
			inPtr = _InSig_EarObject(data, i)->channel[chan];
			for (j = 0, sum = 0.0; j < _InSig_EarObject(data, i)->length; j++)
				sum += *inPtr++;
			*outPtr++ = (p->summationMode ==
			  UTILITY_COLLATESIGNALS_SUMMATIONMODE_SUM)? sum: sum /
			  _InSig_EarObject(data, i)->length;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

