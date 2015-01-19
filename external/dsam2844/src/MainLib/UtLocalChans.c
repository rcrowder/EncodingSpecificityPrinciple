/**********************
 *
 * File:		UtLocalChans.c
 * Purpose:		It calculates measures of a multi-channel input, grouping
 *				the channels according to a specified algorithm.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 * Author:		L. P. O'Mard
 * Created:		17 Jan 2002
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
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtLocalChans.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

LocalChansPtr	localChansPtr = NULL;

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
Free_Utility_LocalChans(void)
{
	if (localChansPtr == NULL)
		return(FALSE);
	if (localChansPtr->parList)
		FreeList_UniParMgr(&localChansPtr->parList);
	if (localChansPtr->parSpec == GLOBAL) {
		free(localChansPtr);
		localChansPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This function initialises the 'mode' list array
 */

BOOLN
InitModeList_Utility_LocalChans(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("SUM"),		UTILITY_LOCALCHANS_MODE_SUM },
			{ wxT("AVERAGE"),	UTILITY_LOCALCHANS_MODE_AVERAGE },
			{ NULL,				UTILITY_LOCALCHANS_MODE_NULL },
		};
	localChansPtr->modeList = modeList;
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
Init_Utility_LocalChans(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_LocalChans");

	if (parSpec == GLOBAL) {
		if (localChansPtr != NULL)
			Free_Utility_LocalChans();
		if ((localChansPtr = (LocalChansPtr) malloc(sizeof(LocalChans))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (localChansPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	localChansPtr->parSpec = parSpec;
	localChansPtr->mode = UTILITY_LOCALCHANS_MODE_SUM;
	localChansPtr->limitMode = SIGNALDATA_LIMIT_MODE_OCTAVE;
	localChansPtr->lowerLimit = -1.0;
	localChansPtr->upperLimit = 1.0;

	InitModeList_Utility_LocalChans();
	if (!SetUniParList_Utility_LocalChans()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_LocalChans();
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
SetUniParList_Utility_LocalChans(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_LocalChans");
	UniParPtr	pars;

	if ((localChansPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_LOCALCHANS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = localChansPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_LOCALCHANS_MODE], wxT("MODE"),
	  wxT("Operation mode ('sum' or 'average')."),
	  UNIPAR_NAME_SPEC,
	  &localChansPtr->mode, localChansPtr->modeList,
	  (void * (*)) SetMode_Utility_LocalChans);
	SetPar_UniParMgr(&pars[UTILITY_LOCALCHANS_LIMITMODE], wxT("LIMIT_MODE"),
	  wxT("Limit mode ('octave' or 'channel')."),
	  UNIPAR_NAME_SPEC,
	  &localChansPtr->limitMode, LimitModeList_SignalData(0),
	  (void * (*)) SetLimitMode_Utility_LocalChans);
	SetPar_UniParMgr(&pars[UTILITY_LOCALCHANS_LOWERAVELIMIT],
	  wxT("LOWER_AVE_LIMIT"),
	  wxT("Local window limit below channel."),
	  UNIPAR_REAL,
	  &localChansPtr->lowerLimit, NULL,
	  (void * (*)) SetLowerLimit_Utility_LocalChans);
	SetPar_UniParMgr(&pars[UTILITY_LOCALCHANS_UPPERAVELIMIT],
	  wxT("UPPER_AVE_LIMIT"),
	  wxT("Local window limit above channel."),
	  UNIPAR_REAL,
	  &localChansPtr->upperLimit, NULL,
	  (void * (*)) SetUpperLimit_Utility_LocalChans);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_LocalChans(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_LocalChans");

	if (localChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (localChansPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(localChansPtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_LocalChans(WChar * theMode)
{
	static const WChar	*funcName = wxT("SetMode_Utility_LocalChans");
	int		specifier;

	if (localChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
		localChansPtr->modeList)) == UTILITY_LOCALCHANS_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	localChansPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetLimitMode **********************************/

/*
 * This function sets the module's limitMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLimitMode_Utility_LocalChans(WChar * theLimitMode)
{
	static const WChar	*funcName = wxT("SetLimitMode_Utility_LocalChans");
	int		specifier;

	if (localChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theLimitMode,
	  LimitModeList_SignalData(0))) == SIGNALDATA_LIMIT_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theLimitMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	localChansPtr->limitMode = specifier;
	return(TRUE);

}

/****************************** SetLowerLimit *********************************/

/*
 * This function sets the module's lowerLimit parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowerLimit_Utility_LocalChans(Float theLowerLimit)
{
	static const WChar	*funcName = wxT("SetLowerLimit_Utility_LocalChans");

	if (localChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	localChansPtr->lowerLimit = theLowerLimit;
	return(TRUE);

}

/****************************** SetUpperLimit *********************************/

/*
 * This function sets the module's upperLimit parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUpperLimit_Utility_LocalChans(Float theUpperLimit)
{
	static const WChar	*funcName = wxT("SetUpperLimit_Utility_LocalChans");

	if (localChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	localChansPtr->upperLimit = theUpperLimit;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_LocalChans(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_LocalChans");

	if (localChansPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Channel localisation Utility Module Parameters:-\n"));
	DPrint(wxT("\tOperation mode = %s,"), localChansPtr->modeList[
	  localChansPtr->mode].name);
	DPrint(wxT("\tLimit mode = %s,\n"), LimitModeList_SignalData(localChansPtr->
	  limitMode)->name);
	DPrint(wxT("\tLower window limit below channel = %g (%s),\n"),
	  localChansPtr-> lowerLimit, LimitModeList_SignalData(localChansPtr->
	  limitMode)->name);
	DPrint(wxT("\tUpper window limit above channel = %g (%s).\n"),
	  localChansPtr->upperLimit, LimitModeList_SignalData(localChansPtr->
	  limitMode)->name);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_LocalChans(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_LocalChans");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	localChansPtr = (LocalChansPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_LocalChans(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_LocalChans");

	if (!SetParsPointer_Utility_LocalChans(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_LocalChans(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = localChansPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_LocalChans;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_LocalChans;
	theModule->PrintPars = PrintPars_Utility_LocalChans;
	theModule->ResetProcess = ResetProcess_Utility_LocalChans;
	theModule->RunProcess = Calc_Utility_LocalChans;
	theModule->SetParsPointer = SetParsPointer_Utility_LocalChans;
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
CheckData_Utility_LocalChans(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_LocalChans");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (_InSig_EarObject(data, 0)->numChannels < 2) {
		NotifyError(wxT("%s: This module expects multi-channel input."),
		  funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Utility_LocalChans(EarObjectPtr data)
{
	ResetOutSignal_EarObject(data);

}

/****************************** Calc ******************************************/

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
Calc_Utility_LocalChans(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Utility_LocalChans");
	register ChanData	 *inPtr, *outPtr;
	WChar	channelTitle[MAXLINE];
	int		outChan, inChan, maxChan, lowerChanLimit, upperChanLimit;
	int		numChannels;
	ChanLen	i;
	SignalDataPtr	inSignal, outSignal;
	LocalChansPtr	p = localChansPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_LocalChans(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Channel localisation Module ")
		  wxT("process"));

		if (!GetChannelLimits_SignalData(_InSig_EarObject(data, 0), &p->minChan,
		  &maxChan, p->lowerLimit, p->upperLimit, p->limitMode)) {
			NotifyError(wxT("%s: Could not find a channel limits for signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(data, (uShort) (maxChan - p->minChan + 1),
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		ResetProcess_Utility_LocalChans(data);
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		Snprintf_Utility_String(channelTitle, MAXLINE, wxT("Averaged channels ")
		  wxT("('%s' mode)"), LimitModeList_SignalData(p->limitMode)->name);
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data), channelTitle);
		SetInfoSampleTitle_SignalData(_OutSig_EarObject(data), _InSig_EarObject(
		  data, 0)->info.sampleTitle);
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	for (outChan = outSignal->offset; outChan < outSignal->numChannels;
	  outChan++) {
		inChan = p->minChan + outChan;
		outSignal->info.chanLabel[outChan] = inSignal->info.chanLabel[inChan];
		outSignal->info.cFArray[outChan] = inSignal->info.cFArray[inChan];
		GetWindowLimits_SignalData(inSignal, &lowerChanLimit, &upperChanLimit,
		  inSignal->info.cFArray[inChan], p->lowerLimit, p->upperLimit,
		  p->limitMode);
		for (inChan = lowerChanLimit; inChan <= upperChanLimit; inChan++) {
			inPtr = inSignal->channel[inChan];
			outPtr = outSignal->channel[outChan];
			for (i = 0; i < outSignal->length; i++)
				*outPtr++ += *inPtr++;
		}
		if ((p->mode == UTILITY_LOCALCHANS_MODE_AVERAGE) &&
		  (lowerChanLimit != upperChanLimit)) {
			numChannels = upperChanLimit - lowerChanLimit + 1;
			outPtr = outSignal->channel[outChan];
			for (i = 0; i < outSignal->length; i++)
				*outPtr++ /= numChannels;
		}

	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

