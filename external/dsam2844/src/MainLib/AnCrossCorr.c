/**********************
 *
 * File:		AnCrossCorr.c
 * Purpose:		This routine generates a cross-correlation function from
 *				the EarObject's input signal i.e.
 *				channel[i] * channel[i + 1]...etc.
 * Comments:	Written using ModuleProducer version 1.9.
 *				It expects a signal to be a 2N multi-channel signal, i.e. it
 *				assumes that an interleaved signal only has an
 *				interleaveLevel = 2.
 *				It overwrites previous data if the output signal has already
 *				been initialised.
 *				The function is calculated backwards from the time specified.
 *				An exponential decay function is included.
 *				The same binning as for the input signal is used.
 *				The "SignalInfo" continuity needs some work, the CF should be
 *				set automatically.
 *				02-09-97 LPO: Corrected the checking of the cross-correlation
 *				bounds in "CheckData_...".
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 * Author:		L. P. O'Mard & E. A. Lopez-Poveda
 * Created:		14 Feb 1996
 * Updated:		30 Jun 1998
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
#include "FiParFile.h"
#include "UtString.h"
#include "GeModuleMgr.h"
#include "AnCrossCorr.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CrossCorrPtr	crossCorrPtr = NULL;

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
Free_Analysis_CCF(void)
{
	if (crossCorrPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_CCF();
	if (crossCorrPtr->parList)
		FreeList_UniParMgr(&crossCorrPtr->parList);
	if (crossCorrPtr->parSpec == GLOBAL) {
		free(crossCorrPtr);
		crossCorrPtr = NULL;
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
Init_Analysis_CCF(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_CCF");

	if (parSpec == GLOBAL) {
		if (crossCorrPtr != NULL)
			Free_Analysis_CCF();
		if ((crossCorrPtr = (CrossCorrPtr) malloc(sizeof(CrossCorr))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (crossCorrPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	crossCorrPtr->parSpec = parSpec;
	crossCorrPtr->updateProcessVariablesFlag = TRUE;
	crossCorrPtr->timeOffset = 0.0;
	crossCorrPtr->timeConstant = 0.0025;
	crossCorrPtr->period = 0.005;
	crossCorrPtr->exponentDt = NULL;

	if (!SetUniParList_Analysis_CCF()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_CCF();
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
SetUniParList_Analysis_CCF(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_CCF");
	UniParPtr	pars;

	if ((crossCorrPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_CCF_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = crossCorrPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_CCF_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Time offset, t0 (s)."),
	  UNIPAR_REAL,
	  &crossCorrPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_CCF);
	SetPar_UniParMgr(&pars[ANALYSIS_CCF_TIMECONSTANT], wxT("TIME_CONST"),
	  wxT("Time constant, tw (s)."),
	  UNIPAR_REAL,
	  &crossCorrPtr->timeConstant, NULL,
	  (void * (*)) SetTimeConstant_Analysis_CCF);
	SetPar_UniParMgr(&pars[ANALYSIS_CCF_PERIOD], wxT("MAX_LAG"),
	  wxT("Maximum lag period, t (s)."),
	  UNIPAR_REAL,
	  &crossCorrPtr->period, NULL,
	  (void * (*)) SetPeriod_Analysis_CCF);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_CCF(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_CCF");

	if (crossCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (crossCorrPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been  ")
		  wxT("initialised.NULL returned."), funcName);
		return(NULL);
	}
	return(crossCorrPtr->parList);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_CCF(Float theTimeOffset)
{
	static const WChar	*funcName = wxT("SetTimeOffset_Analysis_CCF");

	if (crossCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g s)"), funcName,
		  theTimeOffset);
		return(FALSE);
	}
	crossCorrPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeConstant *******************************/

/*
 * This function sets the module's timeConstant parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeConstant_Analysis_CCF(Float theTimeConstant)
{
	static const WChar	*funcName = wxT("SetTimeConstant_Analysis_CCF");

	if (crossCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeConstant < 0.0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g s)"), funcName,
		  theTimeConstant);
		return(FALSE);
	}
	crossCorrPtr->timeConstant = theTimeConstant;
	return(TRUE);

}

/****************************** SetPeriod *************************************/

/*
 * This function sets the module's period parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPeriod_Analysis_CCF(Float thePeriod)
{
	static const WChar	*funcName = wxT("SetPeriod_Analysis_CCF");

	if (crossCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePeriod < 0.0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g s)"), funcName,
		  thePeriod);
		return(FALSE);
	}
	crossCorrPtr->period = thePeriod;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_CCF(void)
{

	DPrint(wxT("Cross-correlation Analysis Module Parameters:-\n"));
	DPrint(wxT("\tTime Offset = "));
	if (crossCorrPtr->timeOffset < 0.0)
		DPrint(wxT("end of signal,\n"));
	else
		DPrint(wxT("%g ms,\n"), MSEC(crossCorrPtr->timeOffset));
	DPrint(wxT("\tTime constant = %g ms,\t"),
	  MSEC(crossCorrPtr->timeConstant));
	DPrint(wxT("\tPeriod = %g ms\n"), MSEC(crossCorrPtr->period));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_CCF(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_CCF");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	crossCorrPtr = (CrossCorrPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_CCF(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_CCF");

	if (!SetParsPointer_Analysis_CCF(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_CCF(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = crossCorrPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_CCF;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_CCF;
	theModule->PrintPars = PrintPars_Analysis_CCF;
	theModule->RunProcess = Calc_Analysis_CCF;
	theModule->SetParsPointer = SetParsPointer_Analysis_CCF;
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
 * A specific check for when timeOffset < 0.0 (set to signal duration) need
 * not be made.
 */

BOOLN
CheckData_Analysis_CCF(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_CCF");
	Float	signalDuration;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if (crossCorrPtr->timeOffset > signalDuration) {
		NotifyError(wxT("%s: Time offset is longer than signal duration."),
		  funcName);
		return(FALSE);
	}
	if ((crossCorrPtr->timeOffset - crossCorrPtr->period * 2.0 < 0.0) ||
	  (crossCorrPtr->timeOffset + crossCorrPtr->period > signalDuration)) {
		NotifyError(wxT("%s: cross-correlation period extends outside the ")
		  wxT("range of the signal."), funcName);
		return(FALSE);
	}
	if (_InSig_EarObject(data, 0)->numChannels % 2 != 0) {
		NotifyError(wxT("%s: Number of channels must be a factor of 2."),
		  funcName);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the exponential table.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_Analysis_CCF(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Analysis_CCF");

	Float	*expDtPtr, dt;
	ChanLen	i, maxPeriodIndex;
	CrossCorrPtr	p = crossCorrPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_CCF();
		dt = _InSig_EarObject(data, 0)->dt;
		maxPeriodIndex = (ChanLen) floor(p->period / dt + 0.05);
		if ((p->exponentDt = (Float *) calloc(maxPeriodIndex, sizeof(
		  Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for exponent lookup table."),
			  funcName);
			return(FALSE);
		}
		SetNumWindowFrames_SignalData(_OutSig_EarObject(data), 0);
		for (i = 0, expDtPtr = p->exponentDt; i < maxPeriodIndex; i++,
		  expDtPtr++)
			*expDtPtr = exp( -(i * dt) / p->timeConstant);
		p->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Analysis_CCF(void)
{
	if (crossCorrPtr->exponentDt) {
		free(crossCorrPtr->exponentDt);
		crossCorrPtr->exponentDt = NULL;
	}
	crossCorrPtr->updateProcessVariablesFlag = TRUE;
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
Calc_Analysis_CCF(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_CCF");
	register	Float	*expDtPtr;
	register	ChanData	 *inPtrL, *inPtrR, *outPtr;
	int		chan, inChan;
	long	deltaT;
	Float	dt;
	ChanLen	i, totalPeriodIndex;
	SignalDataPtr	outSignal;
	CrossCorrPtr	p = crossCorrPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_CCF(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Cross Correlation Function (CCF) ")
		  wxT("analysis"));
		dt = _InSig_EarObject(data, 0)->dt;
		p->periodIndex = (ChanLen) floor(p->period / dt + 0.5);
		totalPeriodIndex = p->periodIndex * 2 + 1;
		if (!InitOutSignal_EarObject(data, (uShort) (_InSig_EarObject(data, 0)->
		  numChannels / 2), totalPeriodIndex, dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), 1);
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		for (chan = 0; chan < _OutSig_EarObject(data)->numChannels; chan++)
			_OutSig_EarObject(data)->info.cFArray[chan] =
			  _InSig_EarObject(data, 0)->info.cFArray[chan];
		SetInfoSampleTitle_SignalData(_OutSig_EarObject(data), wxT("Delay ")
		  wxT("period (s)"));
		SetStaticTimeFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), -p->period);
		p->timeOffsetIndex = (ChanLen) floor(p->timeOffset / dt + 0.5) - 1;
		if (!InitProcessVariables_Analysis_CCF(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		inChan = chan * 2;
		for (deltaT = -((long) p->periodIndex); deltaT <= (long) p->periodIndex;
		  deltaT++, outPtr++) {
			inPtrL = _InSig_EarObject(data, 0)->channel[inChan] + p->
			  timeOffsetIndex;
			inPtrR = _InSig_EarObject(data, 0)->channel[inChan + 1] + p->
			  timeOffsetIndex;
			for (i = 0, *outPtr = 0.0, expDtPtr = p->exponentDt;
			  i < p->periodIndex; i++, expDtPtr++, inPtrL--, inPtrR--)
				*outPtr += *inPtrL * *(inPtrR - deltaT) * *expDtPtr;
			*outPtr /= p->periodIndex;
		}
	}
	if (!outSignal->offset)	/* Do this only for one (first) thread */
		outSignal->numWindowFrames++;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

