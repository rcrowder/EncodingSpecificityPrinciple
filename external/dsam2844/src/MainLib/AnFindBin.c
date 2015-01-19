/**********************
 *
 * File:		AnFindBin.c
 * Purpose:		This module finds the maximum/minimum bin values/indexes for a
 *				signal
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				The search is implemented using a bin "window" of "binWidth".
 * Author:		L. P. O'Mard
 * Created:		8 Mar 1996
 * Updated:		9 Jun 1996
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
#include <limits.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "AnFindBin.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FindBinPtr	findBinPtr = NULL;

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
Free_Analysis_FindBin(void)
{
	if (findBinPtr == NULL)
		return(FALSE);
	if (findBinPtr->parList)
		FreeList_UniParMgr(&findBinPtr->parList);
	if (findBinPtr->parSpec == GLOBAL) {
		free(findBinPtr);
		findBinPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Analysis_FindBin(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("MIN_VALUE"), FIND_BIN_MIN_VALUE_MODE},
					{ wxT("MIN_INDEX"), FIND_BIN_MIN_INDEX_MODE},
					{ wxT("MAX_VALUE"), FIND_BIN_MAX_VALUE_MODE},
					{ wxT("MAX_INDEX"), FIND_BIN_MAX_INDEX_MODE},
					{ NULL, FIND_BIN_NULL }

				};
	findBinPtr->modeList = modeList;
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
Init_Analysis_FindBin(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_FindBin");

	if (parSpec == GLOBAL) {
		if (findBinPtr != NULL)
			Free_Analysis_FindBin();
		if ((findBinPtr = (FindBinPtr) malloc(sizeof(FindBin))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (findBinPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	findBinPtr->parSpec = parSpec;
	findBinPtr->mode = FIND_BIN_MIN_VALUE_MODE;
	findBinPtr->binWidth = -1.0;
	findBinPtr->timeOffset = 0.0;
	findBinPtr->timeWidth = -1.0;

	InitModeList_Analysis_FindBin();
	if (!SetUniParList_Analysis_FindBin()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_FindBin();
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
SetUniParList_Analysis_FindBin(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_FindBin");
	UniParPtr	pars;

	if ((findBinPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_FINDBIN_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = findBinPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_FINDBIN_MODE], wxT("MODE"),
	  wxT("Search mode ('max_value', 'max_index', 'min_value' or ")
	    wxT("'min_index')."),
	  UNIPAR_NAME_SPEC,
	  &findBinPtr->mode, findBinPtr->modeList,
	  (void * (*)) SetMode_Analysis_FindBin);
	SetPar_UniParMgr(&pars[ANALYSIS_FINDBIN_BINWIDTH], wxT("BIN_WIDTH"),
	  wxT("Bin width (time window) for search bin: -ve assumes dt for ")
	    wxT("previous signal (s)."),
	  UNIPAR_REAL,
	  &findBinPtr->binWidth, NULL,
	  (void * (*)) SetBinWidth_Analysis_FindBin);
	SetPar_UniParMgr(&pars[ANALYSIS_FINDBIN_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Offset from which to start search (s?)."),
	  UNIPAR_REAL,
	  &findBinPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_FindBin);
	SetPar_UniParMgr(&pars[ANALYSIS_FINDBIN_TIMEWIDTH], wxT("WIDTH"),
	  wxT("Analysis window width for search: -ve assume to end of signal ")
	    wxT("(s?)."),
	  UNIPAR_REAL,
	  &findBinPtr->timeWidth, NULL,
	  (void * (*)) SetTimeWidth_Analysis_FindBin);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_FindBin(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_FindBin");

	if (findBinPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (findBinPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(findBinPtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Analysis_FindBin(WChar *theMode)
{
	static const WChar	*funcName = wxT("SetMode_Analysis_FindBin");
	int		specifier;

	if (findBinPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, findBinPtr->modeList)) ==
	  FIND_BIN_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	findBinPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetBinWidth ***********************************/

/*
 * This function sets the module's binWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinWidth_Analysis_FindBin(Float theBinWidth)
{
	static const WChar	*funcName = wxT("SetBinWidth_Analysis_FindBin");

	if (findBinPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	findBinPtr->binWidth = theBinWidth;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_FindBin(Float theTimeOffset)
{
	static const WChar	*funcName = wxT("SetTimeOffset_Analysis_FindBin");

	if (findBinPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError(wxT("%s: Illegal time offset = %g ms."), funcName,
		  theTimeOffset);
		return(FALSE);
	}
	findBinPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeWidth **********************************/

/*
 * This function sets the module's timeWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeWidth_Analysis_FindBin(Float theTimeWidth)
{
	static const WChar	*funcName = wxT("SetTimeWidth_Analysis_FindBin");

	if (findBinPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	findBinPtr->timeWidth = theTimeWidth;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_FindBin(void)
{

	DPrint(wxT("Find Max. bin Values Module Parameters:-\n"));
	DPrint(wxT("\tSearch mode = %s,"),
	  findBinPtr->modeList[findBinPtr->mode].name);
	DPrint(wxT("\tBin width = "));
	if (findBinPtr->binWidth <= 0.0)
		DPrint(wxT("<prev. signal dt>,\n"));
	else
		DPrint(wxT("%g ms,\n"), MSEC(findBinPtr->binWidth));
	DPrint(wxT("\tTime offset = %g ms,"),
	  MSEC(findBinPtr->timeOffset));
	DPrint(wxT("\tTime range = "));
	if (findBinPtr->timeWidth < 0.0)
		DPrint(wxT("<end of signal>\n"));
	else
		DPrint(wxT("%g ms\n"), MSEC(findBinPtr->timeWidth));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_FindBin(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_FindBin");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	findBinPtr = (FindBinPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_FindBin(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_FindBin");

	if (!SetParsPointer_Analysis_FindBin(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_FindBin(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = findBinPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_FindBin;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_FindBin;
	theModule->PrintPars = PrintPars_Analysis_FindBin;
	theModule->RunProcess = Calc_Analysis_FindBin;
	theModule->SetParsPointer = SetParsPointer_Analysis_FindBin;
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
CheckData_Analysis_FindBin(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_FindBin");
	Float	signalDuration, dt;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	dt = _InSig_EarObject(data, 0)->dt;
	if ((findBinPtr->binWidth > 0.0) && (findBinPtr->binWidth < dt)) {
		NotifyError(wxT("%s: Bin width (%g ms) is less than sampling interval, ")
		  wxT("(%g ms)."), funcName, MSEC(findBinPtr->binWidth), MSEC(dt));
		return(FALSE);
	}
	signalDuration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if (findBinPtr->timeOffset >= signalDuration) {
		NotifyError(wxT("%s: Time offset (%g ms) is too long for the signal ")
		  wxT("duration (%g ms)."), funcName, MSEC(findBinPtr->timeOffset),
		  MSEC(signalDuration));
		return(FALSE);
	}
	if ((findBinPtr->binWidth > 0.0) && (findBinPtr->timeOffset +
	  findBinPtr->binWidth >= signalDuration)) {
		NotifyError(wxT("%s: Time offset + bin width (%g ms) must not ")
		  wxT("exceed the signal duration (%g ms)."), funcName, MSEC(
		  findBinPtr->timeOffset + findBinPtr->binWidth), MSEC(signalDuration));
		return(FALSE);
	}
	if ((findBinPtr->timeWidth > 0.0) && (findBinPtr->timeOffset +
	  findBinPtr->timeWidth >= signalDuration)) {
		NotifyError(wxT("%s: Time offset + range (%g ms) must not exceed the ")
		  wxT("signal duration (%g ms)."), funcName, MSEC(findBinPtr->
		  timeOffset + findBinPtr->timeWidth), MSEC(signalDuration));
		return(FALSE);
	}
	return(TRUE);

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
Calc_Analysis_FindBin(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_FindBin");
	register	ChanData	*inPtr, *binPtr;
	register	Float		sum, binSum;
	int		chan;
	Float	dt;
	ChanLen	i, j, binIndex = 0;
	FindBinPtr	p = findBinPtr;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_FindBin(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Find Maximum Bin Value Analysis"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, 1, 1.0)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		p->findMinimum = (p->mode == FIND_BIN_MIN_VALUE_MODE) || (p->mode ==
		  FIND_BIN_MIN_INDEX_MODE);
		dt = _InSig_EarObject(data, 0)->dt;
		p->binWidthIndex = (p->binWidth <= 0.0)? (ChanLen) 1: (ChanLen) (p->
		  binWidth / dt + 0.5);
		p->timeOffsetIndex = (ChanLen) (p->timeOffset / dt + 0.5);
		p->timeWidthIndex = (p->timeWidth <= 0.0)? _InSig_EarObject(data, 0)->
		  length - p->timeOffsetIndex: (ChanLen) (p->timeWidth / dt + 0.5);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan] + p->timeOffsetIndex;
		binSum = (p->findMinimum)? DBL_MAX: -DBL_MAX;
		for (i = 0; i < p->timeWidthIndex - p->binWidthIndex; i++, inPtr++) {
			for (j = 0, sum = 0.0, binPtr = inPtr; j < p->binWidthIndex; j++,
			  binPtr++)
				sum += *binPtr;
			if ((p->findMinimum)? (sum < binSum): (sum > binSum)) {
				binSum = sum;
				binIndex = p->timeOffsetIndex + i - p->binWidthIndex / 2;
			}
		}
		switch (p->mode) {
		case FIND_BIN_MIN_VALUE_MODE:
		case FIND_BIN_MAX_VALUE_MODE:
			outSignal->channel[chan][0] = binSum / p->binWidthIndex;
			break;
		case FIND_BIN_MIN_INDEX_MODE:
		case FIND_BIN_MAX_INDEX_MODE:
			outSignal->channel[chan][0] = binIndex;
			break;
		} /* switch */
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

