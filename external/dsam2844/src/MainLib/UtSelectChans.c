/**********************
 *
 * File:		UtSelectChans.c
 * Purpose:		This routine copies the input signal to the output signal,
 *				but copies only the specified input channels to the output
 *				signal.
 *				The channels are marked as zero in the array supplied as an
 *				argument.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				N. B. the array setting is uncontrolled, but this cannot be
 *				helped right now.
 *				There are no PrintPars nor ReadPars routines, as these are not
 *				appropriate for this type of module.
 *				19-03-97 LPO: Added "mode" parameter.
 *				02-03-05 LPO: This module is not being thread enabled as the
 *				number of input channels defines the channel processing.
 * Author:		L. P. O'Mard
 * Created:		11 Jun 1996
 * Updated:		19 Mar 1997
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
#include "UtSelectChans.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SelectChanPtr	selectChanPtr = NULL;

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
Free_Utility_SelectChannels(void)
{
	if (selectChanPtr == NULL)
		return(FALSE);
	if (selectChanPtr->selectionArray) {
		free(selectChanPtr->selectionArray);
		selectChanPtr->selectionArray = NULL;
	}
	if (selectChanPtr->parList)
		FreeList_UniParMgr(&selectChanPtr->parList);
	if (selectChanPtr->parSpec == GLOBAL) {
		free(selectChanPtr);
		selectChanPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Utility_SelectChannels(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("ZERO"),		SELECT_CHANS_ZERO_MODE },
					{ wxT("REMOVE"),	SELECT_CHANS_REMOVE_MODE },
					{ wxT("EXPAND"),	SELECT_CHANS_EXPAND_MODE },
					{ NULL, 			SELECT_CHANS_NULL }
				};
	selectChanPtr->modeList = modeList;
	return(TRUE);

}

/****************************** InitSelectionModeList *************************/

/*
 * This function initialises the 'selectionMode' list array
 */

BOOLN
InitSelectionModeList_Utility_SelectChannels(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("ALL"),		UTILITY_SELECTCHANNELS_SELECTIONMODE_ALL },
			{ wxT("MIDDLE"),	UTILITY_SELECTCHANNELS_SELECTIONMODE_MIDDLE },
			{ wxT("LOWEST"),	UTILITY_SELECTCHANNELS_SELECTIONMODE_LOWEST },
			{ wxT("HIGHEST"),	UTILITY_SELECTCHANNELS_SELECTIONMODE_HIGHEST },
			{ wxT("USER"),		UTILITY_SELECTCHANNELS_SELECTIONMODE_USER },
			{ NULL,				UTILITY_SELECTCHANNELS_SELECTIONMODE_NULL },
		};
	selectChanPtr->selectionModeList = modeList;
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
Init_Utility_SelectChannels(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_SelectChannels");

	if (parSpec == GLOBAL) {
		if (selectChanPtr != NULL)
			Free_Utility_SelectChannels();
		if ((selectChanPtr = (SelectChanPtr) malloc(sizeof(SelectChan))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (selectChanPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	selectChanPtr->parSpec = parSpec;
	selectChanPtr->updateProcessVariablesFlag = TRUE;
	selectChanPtr->mode = SELECT_CHANS_REMOVE_MODE;
	selectChanPtr->selectionMode = UTILITY_SELECTCHANNELS_SELECTIONMODE_ALL;
	selectChanPtr->selectionArray = NULL;

	InitModeList_Utility_SelectChannels();
	InitSelectionModeList_Utility_SelectChannels();
	if (!SetUniParList_Utility_SelectChannels()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_SelectChannels();
		return(FALSE);
	}
	selectChanPtr->numChannels = 0;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_SelectChannels(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_SelectChannels");
	UniParPtr	pars;

	if ((selectChanPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_SELECTCHANNELS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = selectChanPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_SELECTCHANNELS_MODE], wxT("MODE"),
	  wxT("Selection mode - 'zero', 'remove' or 'expand'."),
	  UNIPAR_NAME_SPEC,
	  &selectChanPtr->mode, selectChanPtr->modeList,
	  (void * (*)) SetMode_Utility_SelectChannels);
	SetPar_UniParMgr(&pars[UTILITY_SELECTCHANNELS_SELECTIONMODE],
	  wxT("SELECTION_MODE"),
	  wxT("Channel selection mode ('all', 'middle', 'lowest', 'highest' or ")
	  wxT("'user'."),
	  UNIPAR_NAME_SPEC,
	  &selectChanPtr->selectionMode, selectChanPtr->selectionModeList,
	  (void * (*)) SetSelectionMode_Utility_SelectChannels);
	SetPar_UniParMgr(&pars[UTILITY_SELECTCHANNELS_NUMCHANNELS], wxT(
	  "NUM_CHANNELS"),
	  wxT("No. of channels in selection field (This is no longer used)."),
	  UNIPAR_INT_AL,
	  &selectChanPtr->numChannels, NULL,
	  (void * (*)) SetNumChannels_Utility_SelectChannels);
	SetPar_UniParMgr(&pars[UTILITY_SELECTCHANNELS_SELECTIONARRAY], wxT("ARRAY"),
	  wxT("Selection array 0 = off, 1 = on."),
	  UNIPAR_REAL_DYN_ARRAY,
	  &selectChanPtr->selectionArray, &selectChanPtr->numChannels,
	  (void * (*)) SetIndividualSelection_Utility_SelectChannels);

	SetEnabledState_Utility_SelectChannels();
	return(TRUE);

}

/****************************** SetEnabledState *******************************/

/*
 * This routine sets the enabled states.
 */

void
SetEnabledState_Utility_SelectChannels(void)
{
	selectChanPtr->parList->pars[UTILITY_SELECTCHANNELS_NUMCHANNELS].enabled =
	  FALSE;
	selectChanPtr->parList->pars[UTILITY_SELECTCHANNELS_SELECTIONARRAY].
	  enabled = (selectChanPtr->selectionMode ==
	  UTILITY_SELECTCHANNELS_SELECTIONMODE_USER);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_SelectChannels(void)
{
	static const WChar	*funcName =
	  wxT("GetUniParListPtr_Utility_SelectChannels");

	if (selectChanPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (selectChanPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(selectChanPtr->parList);

}

/****************************** AllocNumChannels ******************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the 'numChannels'
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numChannels'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumChannels_Utility_SelectChannels(int numChannels)
{
	static const WChar	*funcName =
	  wxT("AllocNumChannels_Utility_SelectChannels");

	if (numChannels == selectChanPtr->numChannels)
		return(TRUE);
	if (selectChanPtr->selectionArray)
		free(selectChanPtr->selectionArray);
	if ((selectChanPtr->selectionArray = (Float *) calloc(numChannels,
	  sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' selectionArray."),
		  funcName, numChannels);
		return(FALSE);
	}
	selectChanPtr->numChannels = numChannels;
	return(TRUE);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_SelectChannels(WChar *theMode)
{
	static const WChar	*funcName = wxT("SetMode_Utility_SelectChannels");
	int		specifier;

	if (selectChanPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
	  selectChanPtr->modeList)) == SELECT_CHANS_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	selectChanPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetSelectionMode ******************************/

/*
 * This function sets the module's selectionMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSelectionMode_Utility_SelectChannels(WChar * theSelectionMode)
{
	static const WChar	*funcName = wxT(
	  "SetSelectionMode_Utility_SelectChannels");
	int		specifier;

	if (selectChanPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSelectionMode,
	  selectChanPtr->selectionModeList)) ==
	  UTILITY_SELECTCHANNELS_SELECTIONMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSelectionMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	selectChanPtr->updateProcessVariablesFlag = TRUE;
	selectChanPtr->selectionMode = specifier;
	SetEnabledState_Utility_SelectChannels();
	return(TRUE);

}

/****************************** SetNumChannels ********************************/

/*
 * This function sets the module's numChannels parameter.
 * It returns TRUE if the operation is successful.
 * The 'numChannels' variable is set by the
 * 'AllocNumChannels_Utility_SelectChannels' routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumChannels_Utility_SelectChannels(int theNumChannels)
{
	static const WChar	*funcName = wxT(
	  "SetNumChannels_Utility_SelectChannels");

    if (selectChanPtr == NULL) {
        NotifyError(wxT("%s: Module not initialised."), funcName);
        return(FALSE);
    }
    if (theNumChannels < 1) {
        NotifyError(wxT("%s: Illegal number of selection channels.\n"),
		  funcName, theNumChannels);
        return(FALSE);
    }
    if (!AllocNumChannels_Utility_SelectChannels(theNumChannels)) {
        NotifyError(wxT("%s: Cannot allocate memory for the 'numChannels' ")
		  wxT("arrays."), funcName);
        return(FALSE);
    }
    /*** Put any other required checks here. ***/
    selectChanPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetSelectionArray *****************************/

/*
 * This function sets the module's selectionArray array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSelectionArray_Utility_SelectChannels(Float *theSelectionArray)
{
	static const WChar	*funcName = wxT(
	 "SetSelectionArray_Utility_SelectChannels");

	if (selectChanPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	selectChanPtr->selectionArray = theSelectionArray;
	return(TRUE);

}

/****************************** SetIndividualSelection ************************/

/*
 * This function sets the module's selectionArray array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualSelection_Utility_SelectChannels(int theIndex, Float theSelection)
{
	static const WChar *funcName =
	  wxT("SetIndividualSelectionArray_Utility_SelectChannels");

	if (selectChanPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (selectChanPtr->selectionMode !=
	  UTILITY_SELECTCHANNELS_SELECTIONMODE_USER) {
		NotifyError(wxT("%s: Channels can only be selected in 'user' ")
		  wxT("selection mode."), funcName);
		return(FALSE);
	}
	if (theSelection < 0.0) {
		NotifyError(wxT("%s: The selection values must be greater than zero ")
		  wxT("('%g'"), funcName, theSelection);
		return(FALSE);
	}
	if ((theIndex > selectChanPtr->numChannels - 1) &&
	  !ResizeFloatArray_UniParMgr(&selectChanPtr->selectionArray,
	    &selectChanPtr->numChannels, theIndex + 1)) {
		NotifyError(wxT("%s: Could not resize the selection array."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	selectChanPtr->selectionArray[theIndex] = theSelection;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_SelectChannels(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_SelectChannels");
	int		i;

	if (selectChanPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Channel Selection Utility Module Parameters:-\n"));
	DPrint(wxT("\t%10s\n"), wxT("Selection Array"));
	DPrint(wxT("\t%10s\n"), wxT("(state)"));
	for (i = 0; i < selectChanPtr->numChannels; i++)
		if (selectChanPtr->selectionArray[i] > 0.0)
			DPrint(wxT("\t    on * %g\n"), selectChanPtr->selectionArray[i]);
		else
			DPrint(wxT("\t    off\n"));
	DPrint(wxT("\tMode = %s,"), selectChanPtr->modeList[selectChanPtr->mode].
	  name);
	DPrint(wxT("\tChannel selection mode = %s.\n"), selectChanPtr->
	  selectionModeList[selectChanPtr->selectionMode].name);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_SelectChannels(ModulePtr theModule)
{
	static const WChar	*funcName = wxT(
	  "SetParsPointer_Utility_SelectChannels");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	selectChanPtr = (SelectChanPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_SelectChannels(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_SelectChannels");

	if (!SetParsPointer_Utility_SelectChannels(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_SelectChannels(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = selectChanPtr;
	theModule->Free = Free_Utility_SelectChannels;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_SelectChannels;
	theModule->PrintPars = PrintPars_Utility_SelectChannels;
	theModule->RunProcess = Process_Utility_SelectChannels;
	theModule->SetParsPointer = SetParsPointer_Utility_SelectChannels;
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
CheckData_Utility_SelectChannels(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_SelectChannels");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_Utility_SelectChannels(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_Utility_SelectChannels");
	int		i, numInChans;
	SelectChanPtr	p = selectChanPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		/*** Additional update flags can be added to above line ***/
		numInChans = _InSig_EarObject(data, 0)->numChannels / _InSig_EarObject(
		  data, 0)->interleaveLevel;
		if ((p->numChannels < numInChans) && !ResizeFloatArray_UniParMgr(
		  &p->selectionArray, &p->numChannels, numInChans)) {
			NotifyError(wxT("%s: Could not set selection array."), funcName);
			return(FALSE);
		}
		switch (p->selectionMode) {
		case UTILITY_SELECTCHANNELS_SELECTIONMODE_ALL:
			for (i = 0; i < p->numChannels; i++)
				p->selectionArray[i] = 1.0;
			break;
		case UTILITY_SELECTCHANNELS_SELECTIONMODE_MIDDLE:
			for (i = 0; i < p->numChannels; i++)
				p->selectionArray[i] = 0.0;
			p->selectionArray[p->numChannels / 2] = 1.0;
			break;
		case UTILITY_SELECTCHANNELS_SELECTIONMODE_LOWEST:
			for (i = 0; i < p->numChannels; i++)
				p->selectionArray[i] = 0.0;
			p->selectionArray[0] = 1.0;
			break;
		case UTILITY_SELECTCHANNELS_SELECTIONMODE_HIGHEST:
			for (i = 0; i < p->numChannels; i++)
				p->selectionArray[i] = 0.0;
			p->selectionArray[p->numChannels - 1] = 1.0;
			break;
		default:
			;
		}
		p->updateProcessVariablesFlag = FALSE;
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
Process_Utility_SelectChannels(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_SelectChannels");
	register	ChanData	 *inPtr, *outPtr, multiplier;
	uShort	numChannels = 0;
	int		i, k, l, chan, inChanIndex;
	ChanLen	j;
	SelectChanPtr	p = selectChanPtr;
	SignalDataPtr	inSignal, outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_SelectChannels(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Select Channels Utility Module ")
		  wxT("process"));
		if (!InitProcessVariables_Utility_SelectChannels(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		switch (p->mode) {
		case SELECT_CHANS_ZERO_MODE:
			numChannels = _InSig_EarObject(data, 0)->numChannels;
			break;
		case SELECT_CHANS_REMOVE_MODE:
			for (i = 0, numChannels = 0; i < p->numChannels; i++)
				if (p->selectionArray[i] > 0.0)
					numChannels++;
				numChannels *= _InSig_EarObject(data, 0)->interleaveLevel;
				break;
		case SELECT_CHANS_EXPAND_MODE:
			for (i = 0, numChannels = 0; i < p->numChannels; i++)
				if (p->selectionArray[i] > 0.0)
					numChannels += (uShort) p->selectionArray[i];
				numChannels *= _InSig_EarObject(data, 0)->interleaveLevel;
			break;
		} /* switch */
		if (!InitOutSignal_EarObject(data, numChannels, _InSig_EarObject(data,
		  0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), _InSig_EarObject(data,
		  0)->interleaveLevel);
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	switch (p->mode) {
	case SELECT_CHANS_ZERO_MODE:
		for (i = 0; i < inSignal->numChannels; i++) {
			outPtr = outSignal->channel[i];
			if (p->selectionArray[i] > 0.0) {
			inChanIndex = i / inSignal->interleaveLevel;
				inPtr = inSignal->channel[i];
				multiplier = p->selectionArray[inChanIndex];
				for (j = 0; j < outSignal->length; j++)
					*outPtr++ = *inPtr++ * multiplier;
			} else
				for (j = 0; j < outSignal->length; j++)
					*outPtr++ = 0.0;
		}
		break;
	case SELECT_CHANS_REMOVE_MODE:
		for (i = 0, chan = 0; i < inSignal->numChannels; i++) {
			inChanIndex = i / inSignal->interleaveLevel;
			if (p->selectionArray[inChanIndex] > 0.0) {
				SetInfoChannelLabel_SignalData(outSignal, chan, inSignal->info.
				  chanLabel[i]);
				SetInfoCF_SignalData(outSignal, chan, inSignal->info.cFArray[
				  i]);
				inPtr = inSignal->channel[i];
				outPtr = outSignal->channel[chan];
				multiplier = p->selectionArray[inChanIndex];
				for (j = 0; j < outSignal->length; j++)
					*outPtr++ = *inPtr++ * multiplier;
				chan++;
			}
		}
		break;
	case SELECT_CHANS_EXPAND_MODE:
		for (i = 0, chan = 0; i < inSignal->numChannels; i += inSignal->
		  interleaveLevel) {
			inChanIndex = i / inSignal->interleaveLevel;
			if (p->selectionArray[inChanIndex] > 0.0)
				for (k = 0; k < p->selectionArray[inChanIndex]; k++)
					for (l = 0; l < outSignal->interleaveLevel; l++) {
						SetInfoChannelLabel_SignalData(outSignal, chan,
						  inSignal->info.chanLabel[i + l]);
						SetInfoCF_SignalData(outSignal, chan, inSignal->info.
						  cFArray[i + l]);
						inPtr = inSignal->channel[i + l];
						outPtr = outSignal->channel[chan];
						for (j = 0; j < outSignal->length; j++)
							*outPtr++ = *inPtr++;
						chan++;
					}
		}
		break;
	} /* switch */
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

