/**********************
 *
 * File:		UtRedceChans.c
 * Purpose:		This routine reduces multiple channel data to a lower number of
 *				channels.
 * Comments:	Written using ModuleProducer version 1.8.
 *				The input channels are divided over the output channels.  Each
 *				time step of the appropriate number of channels of a signal
 *				data structure is summed, then the average is calculated by
 *				dividing by the number of channels summed.  The input channels
 *				are divided over the output channels. The no. of output
 *				channels must be a multiple of the number of input channels. It
 *				overwrites previous data if the output signal has already been
 *				initialised.
 *				This routine always sets the data->updateProcessFlag, to reset
 *				the output signal to zero.
 *				02-03-97 LPO: this module now treats interleaved signals
 *				correctly.
 *				02-03-05 LPO: This module is not being thread enabled as the
 *				number of input channels defines the channel processing.
 * Author:		L.P.O'Mard
 * Created:		21 Dec 1995
 * Updated:		02 Mar 1997
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
#include "FiParFile.h"
#include "UtString.h"
#include "UtRedceChans.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ReduceChansPtr	reduceChansPtr = NULL;

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
Free_Utility_ReduceChannels(void)
{
	if (reduceChansPtr == NULL)
		return(FALSE);
	if (reduceChansPtr->parList)
		FreeList_UniParMgr(&reduceChansPtr->parList);
	if (reduceChansPtr->parSpec == GLOBAL) {
		free(reduceChansPtr);
		reduceChansPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Utility_ReduceChannels(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("AVERAGE"),	REDUCE_CHANS_AVERAGE_MODE },
					{ wxT("SUM"), 		REDUCE_CHANS_SUM_MODE },
					{ NULL, 			REDUCE_CHANS_NULL }
				};
	reduceChansPtr->modeList = modeList;
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
Init_Utility_ReduceChannels(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_ReduceChannels");

	if (parSpec == GLOBAL) {
		if (reduceChansPtr != NULL)
			Free_Utility_ReduceChannels();
		if ((reduceChansPtr = (ReduceChansPtr) malloc(sizeof(ReduceChans))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (reduceChansPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	reduceChansPtr->parSpec = parSpec;
	reduceChansPtr->mode = REDUCE_CHANS_SUM_MODE;
	reduceChansPtr->numChannels = 1;

	InitModeList_Utility_ReduceChannels();
	if (!SetUniParList_Utility_ReduceChannels()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_ReduceChannels();
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
SetUniParList_Utility_ReduceChannels(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_ReduceChannels");
	UniParPtr	pars;

	if ((reduceChansPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_REDUCECHANNELS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = reduceChansPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_REDUCECHANNELS_MODE], wxT("MODE"),
	  wxT("Mode - 'average' or simple 'sum'."),
	  UNIPAR_NAME_SPEC,
	  &reduceChansPtr->mode, reduceChansPtr->modeList,
	  (void * (*)) SetMode_Utility_ReduceChannels);
	SetPar_UniParMgr(&pars[UTILITY_REDUCECHANNELS_NUMCHANNELS], wxT(
	  "NUM_CHANNELS"),
	  wxT("Number of channels signal is to be averaged to."),
	  UNIPAR_INT,
	  &reduceChansPtr->numChannels, NULL,
	  (void * (*)) SetNumChannels_Utility_ReduceChannels);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_ReduceChannels(void)
{
	static const WChar	*funcName = wxT(
	  "GetUniParListPtr_Utility_ReduceChannels");

	if (reduceChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (reduceChansPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(reduceChansPtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_ReduceChannels(WChar *theMode)
{
	static const WChar	*funcName = wxT("SetMode_Utility_ReduceChannels");
	int		specifier;

	if (reduceChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
	  reduceChansPtr->modeList)) == REDUCE_CHANS_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	reduceChansPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetNumChannels ********************************/

/*
 * This function sets the module's numChannels parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumChannels_Utility_ReduceChannels(int theNumChannels)
{
	static const WChar	*funcName = wxT(
	  "SetNumChannels_Utility_ReduceChannels");

	if (reduceChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumChannels < 1) {
		NotifyError(wxT("%s: Cannot reduce channels to %d.\n"), funcName,
		  theNumChannels);
		return(FALSE);
	}
	reduceChansPtr->numChannels = theNumChannels;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_ReduceChannels(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_ReduceChannels");

	if (reduceChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Reduce Channels Utility Module Parameters:-\n"));
	DPrint(wxT("\tMode = %s,"), reduceChansPtr->modeList[reduceChansPtr->mode].
	  name);
	DPrint(wxT("\tNo. of channels = %d.\n"), reduceChansPtr->numChannels);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_ReduceChannels(ModulePtr theModule)
{
	static const WChar	*funcName = wxT(
	  "SetParsPointer_Utility_ReduceChannels");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	reduceChansPtr = (ReduceChansPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_ReduceChannels(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_ReduceChannels");

	if (!SetParsPointer_Utility_ReduceChannels(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_ReduceChannels(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = reduceChansPtr;
	theModule->Free = Free_Utility_ReduceChannels;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_ReduceChannels;
	theModule->PrintPars = PrintPars_Utility_ReduceChannels;
	theModule->ResetProcess = ResetProcess_Utility_ReduceChannels;
	theModule->RunProcess = Process_Utility_ReduceChannels;
	theModule->SetParsPointer = SetParsPointer_Utility_ReduceChannels;
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
CheckData_Utility_ReduceChannels(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_ReduceChannels");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((_InSig_EarObject(data, 0)->numChannels % reduceChansPtr->numChannels) != 0) {
		NotifyError(wxT("%s: Cannot reduce %d channels to %d.\n"), funcName,
		  _InSig_EarObject(data, 0)->numChannels, reduceChansPtr->numChannels);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Utility_ReduceChannels(EarObjectPtr data)
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
Process_Utility_ReduceChannels(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_ReduceChannels");
	register	ChanData	 *inPtr, *outPtr;
	WChar	channelTitle[MAXLINE];
	int		j, chan, channelBinWidth, binRatio;
	ChanLen	i;
	SignalDataPtr	inSignal, outSignal;
	ReduceChansPtr	p = reduceChansPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_ReduceChannels(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Average across channels utility"));
		inSignal = _InSig_EarObject(data, 0);
		data->updateProcessFlag = TRUE;
		if (!InitOutSignal_EarObject(data, p->numChannels * inSignal->
		  interleaveLevel, inSignal->length, inSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channel."), funcName);
			return(FALSE);
		}
		ResetProcess_Utility_ReduceChannels(data);
		outSignal = _OutSig_EarObject(data);
		SetInterleaveLevel_SignalData(outSignal, inSignal->interleaveLevel);
		SetLocalInfoFlag_SignalData(outSignal, TRUE);
		SetInfoChannelLabels_SignalData(outSignal, NULL);
		SetInfoCFArray_SignalData(outSignal, NULL);
		DSAM_snprintf(channelTitle, MAXLINE, wxT("Channel summary (%d -> %d)"),
		  inSignal->numChannels, outSignal->numChannels);
		SetInfoChannelTitle_SignalData(outSignal, channelTitle);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	binRatio = inSignal->numChannels / p->numChannels;
	for (chan = 0; chan < inSignal->numChannels; chan += inSignal->
	  interleaveLevel)
		for (j = 0; j < inSignal->interleaveLevel; j++) {
			inPtr = inSignal->channel[chan + j];
			outPtr = outSignal->channel[chan / binRatio * inSignal->
			  interleaveLevel + j];
			for (i = 0; i < inSignal->length; i++)
				*outPtr++ += *inPtr++;
	}
	channelBinWidth = inSignal->numChannels / (p->numChannels * inSignal->
	  interleaveLevel);
	if (p->mode == REDUCE_CHANS_AVERAGE_MODE)
		for (chan = 0; chan < outSignal->numChannels; chan++) {
			outPtr = outSignal->channel[chan];
			for (i = 0; i < outSignal->length; i++)
				*outPtr++ /= channelBinWidth;
		}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

