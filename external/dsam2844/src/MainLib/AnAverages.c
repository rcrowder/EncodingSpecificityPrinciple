/**********************
 *
 * File:		AnAverages.c
 * Purpose:		This routine calculates the average value for each channel.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				This module assumes that a negative timeRange assumes a
 *				period to the end of the signal.
 * Author:		L. P. O'Mard
 * Created:		21 May 1996
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
#include "GeModuleMgr.h"
#include "UtString.h"
#include "AnAverages.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AveragesPtr	averagesPtr = NULL;

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
Free_Analysis_Averages(void)
{
	if (averagesPtr == NULL)
		return(FALSE);
	if (averagesPtr->parList)
		FreeList_UniParMgr(&averagesPtr->parList);
	if (averagesPtr->parSpec == GLOBAL) {
		free(averagesPtr);
		averagesPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Analysis_Averages(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("FULL"),	AVERAGES_FULL },
					{ wxT("-WAVE"),	AVERAGES_NEGATIVE_WAVE },
					{ wxT("+WAVE"),	AVERAGES_POSITIVE_WAVE },
					{ NULL, 		AVERAGES_NULL }
				};
	averagesPtr->modeList = modeList;
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
Init_Analysis_Averages(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_Averages");

	if (parSpec == GLOBAL) {
		if (averagesPtr != NULL)
			Free_Analysis_Averages();
		if ((averagesPtr = (AveragesPtr) malloc(sizeof(Averages))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (averagesPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	averagesPtr->parSpec = parSpec;
	averagesPtr->mode = AVERAGES_FULL;
	averagesPtr->timeOffset = 0.0;
	averagesPtr->timeRange = -1.0;

	InitModeList_Analysis_Averages();
	if (!SetUniParList_Analysis_Averages()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_Averages();
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
SetUniParList_Analysis_Averages(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_Averages");
	UniParPtr	pars;

	if ((averagesPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_AVERAGES_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = averagesPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_AVERAGES_MODE], wxT("MODE"),
	  wxT("Averaging mode ('full', '-wave' or '+wave')."),
	  UNIPAR_NAME_SPEC,
	  &averagesPtr->mode, averagesPtr->modeList,
	  (void * (*)) SetMode_Analysis_Averages);
	SetPar_UniParMgr(&pars[ANALYSIS_AVERAGES_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Time offset for start of averaging period (s)."),
	  UNIPAR_REAL,
	  &averagesPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_Averages);
	SetPar_UniParMgr(&pars[ANALYSIS_AVERAGES_TIMERANGE], wxT("DURATION"),
	  wxT("Averaging period duration: -ve assumes to end of signal (s)."),
	  UNIPAR_REAL,
	  &averagesPtr->timeRange, NULL,
	  (void * (*)) SetTimeRange_Analysis_Averages);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_Averages(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_Averages");

	if (averagesPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (averagesPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(averagesPtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Analysis_Averages(WChar *theMode)
{
	static const WChar	*funcName = wxT("SetMode_Analysis_Averages");
	int		specifier;

	if (averagesPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, averagesPtr->modeList)) ==
	  AVERAGES_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	averagesPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_Averages(Float theTimeOffset)
{
	static const WChar	*funcName = wxT("SetTimeOffset_Analysis_Averages");

	if (averagesPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError(wxT("%s: Illegal value (%g ms)."), funcName,
		  MSEC(theTimeOffset));
		return(FALSE);
	}
	averagesPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeRange **********************************/

/*
 * This function sets the module's timeRange parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeRange_Analysis_Averages(Float theTimeRange)
{
	static const WChar	*funcName = wxT("SetTimeRange_Analysis_Averages");

	if (averagesPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	averagesPtr->timeRange = theTimeRange;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_Averages(void)
{

	DPrint(wxT("Averages Analysis Module Parameters:-\n"));
	DPrint(wxT("\tMode = %s,"),
	  averagesPtr->modeList[averagesPtr->mode].name);
	DPrint(wxT("\tTime offset = %g ms,\n"),
	  MSEC(averagesPtr->timeOffset));
	DPrint(wxT("\tTime range = "));
	if (averagesPtr->timeRange < 0.0)
		DPrint(wxT("<end of signal>\n"));
	else
		DPrint(wxT("%g ms\n"), MSEC(averagesPtr->timeRange));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_Averages(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_Averages");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	averagesPtr = (AveragesPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_Averages(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_Averages");

	if (!SetParsPointer_Analysis_Averages(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_Averages(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = averagesPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_Averages;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_Averages;
	theModule->PrintPars = PrintPars_Analysis_Averages;
	theModule->RunProcess = Calc_Analysis_Averages;
	theModule->SetParsPointer = SetParsPointer_Analysis_Averages;
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
CheckData_Analysis_Averages(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_Averages");
	Float	signalDuration;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if (averagesPtr->timeOffset > signalDuration) {
		NotifyError(wxT("%s: Time offset (%g ms) must be less than the ")
		  wxT("signal\nduration (%g ms)."), funcName, MSEC(averagesPtr->
		  timeOffset), MSEC(signalDuration));
		return(FALSE);
	}
	if ((averagesPtr->timeRange >= 0.0) && (averagesPtr->timeOffset +
	  averagesPtr->timeRange > signalDuration)) {
		NotifyError(wxT("%s: Time offset (%g ms) + timeRange (%g ms) must be\n")
		  wxT("less than the signal duration (%g ms)."), funcName,
		  MSEC(averagesPtr->timeOffset), MSEC(averagesPtr->timeRange),
		  MSEC(signalDuration));
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
Calc_Analysis_Averages(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_Averages");
	register	ChanData	 *inPtr, sum;
	int		chan;
	Float	dt;
	ChanLen	i;
	SignalDataPtr	outSignal;
	AveragesPtr	p = averagesPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_Averages(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Averages Analysis"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, 1, 1.0)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		dt = _InSig_EarObject(data, 0)->dt;
		p->timeOffsetIndex = (ChanLen) floor(p->timeOffset / dt + 0.5);
		p->timeRangeIndex = (p->timeRange <= 0.0)? _InSig_EarObject(data, 0)->
		  length - p->timeOffsetIndex: (ChanLen) floor(p->timeRange / dt + 0.5);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan] + p->timeOffsetIndex;
		for (i = 0, sum = 0.0; i < p->timeRangeIndex; i++, inPtr++)
			switch (p->mode) {
			case AVERAGES_FULL:
				sum += *inPtr;
				break;
			case AVERAGES_NEGATIVE_WAVE:
				if (*inPtr < 0.0)
					sum += *inPtr;
				break;
			case AVERAGES_POSITIVE_WAVE:
				if (*inPtr > 0.0)
					sum += *inPtr;
				break;
			} /* switch */

		outSignal->channel[chan][0] = (ChanData) (sum /
		  p->timeRangeIndex);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
