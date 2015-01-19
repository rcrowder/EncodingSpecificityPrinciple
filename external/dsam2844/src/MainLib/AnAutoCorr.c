/**********************
 *
 * File:		AnAutoCorr.c
 * Purpose:		Auto correlation analysis routine.
 * Comments:	Written using ModuleProducer version 1.1.
 *				This routine always sets the data->updateProcessFlag, to reset
 *				the output signal to zero.
 *				The exponetDt look up table needs to be set up as a process
 *				variable.
 *				12-03-97 LPO: Added DSAM_EPSILON additions in CheckData for PC
 *				version.
 *				29-06-98 LPO: Introduced summation limit + maxLag < timeoffset
 *				limit check in CheckData.
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 *				10-11-98 LPO: Normalisation mode now uses 'NameSpecifier'.
 *				13 Jan 2003 LPO: Added options for the Weigrebe proportional
 *				time constant (Wiegrebe L. (2001) "Searching for the time
 *				constant of neural pitch extraction" JASA, 109, 1082-1091.
 * Author:		L. P. O'Mard
 * Created:		14 Nov 1995
 * Updated:		10 Nov 1998
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
#include <float.h>

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
#include "AnAutoCorr.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AutoCorrPtr	autoCorrPtr = NULL;

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
Free_Analysis_ACF(void)
{
	if (autoCorrPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_ACF();
	if (autoCorrPtr->parList)
		FreeList_UniParMgr(&autoCorrPtr->parList);
	if (autoCorrPtr->parSpec == GLOBAL) {
		free(autoCorrPtr);
		autoCorrPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitNormalisationModeList *********************/

/*
 * This function initialises the 'normalisationMode' list array
 */

BOOLN
InitNormalisationModeList_Analysis_ACF(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("NONE"),		ANALYSIS_NORM_MODE_NONE },
			{ wxT("STANDARD"),	ANALYSIS_NORM_MODE_STANDARD },
			{ wxT("UNITY"),		ANALYSIS_NORM_MODE_UNITY },
			{ NULL,				ANALYSIS_NORM_MODE_NULL }
		};
	autoCorrPtr->normalisationModeList = modeList;
	return(TRUE);

}

/****************************** InitTimeConstModeList *************************/

/*
 * This function initialises the 'timeConstMode' list array
 */

BOOLN
InitTimeConstModeList_Analysis_ACF(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("LICKLIDER"),	ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER },
			{ wxT("NONE"),		ANALYSIS_ACF_TIMECONSTMODE_NONE },
			{ wxT("WIEGREBE"),	ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE },
			{ NULL,				ANALYSIS_ACF_TIMECONSTMODE_NULL },
		};
	autoCorrPtr->timeConstModeList = modeList;
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
Init_Analysis_ACF(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_ACF");

	if (parSpec == GLOBAL) {
		if (autoCorrPtr != NULL)
			Free_Analysis_ACF();
		if ((autoCorrPtr = (AutoCorrPtr) malloc(sizeof(AutoCorr))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (autoCorrPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	autoCorrPtr->parSpec = parSpec;
	autoCorrPtr->updateProcessVariablesFlag = TRUE;
	autoCorrPtr->normalisationMode = ANALYSIS_NORM_MODE_STANDARD;
	autoCorrPtr->timeConstMode = ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER;
	autoCorrPtr->timeOffset = -1.0;
	autoCorrPtr->timeConstant = 0.0025;
	autoCorrPtr->timeConstScale = 2.0;
	autoCorrPtr->maxLag = 0.0075;

	InitNormalisationModeList_Analysis_ACF();
	InitTimeConstModeList_Analysis_ACF();
	if (!SetUniParList_Analysis_ACF()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_ACF();
		return(FALSE);
	}
	autoCorrPtr->exponentDt = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_ACF(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_ACF");
	UniParPtr	pars;

	if ((autoCorrPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_ACF_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = autoCorrPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_NORMALISATIONMODE], wxT("MODE"),
	  wxT("Normalisation mode ('none', 'standard' or 'unity')."),
	  UNIPAR_NAME_SPEC,
	  &autoCorrPtr->normalisationMode, autoCorrPtr->normalisationModeList,
	  (void * (*)) SetNormalisationMode_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_TIMECONSTMODE], wxT("T_CONST_MODE"),
	  wxT("Time constant mode ('Licklider', 'Wiegrebe' or 'none'.)"),
	  UNIPAR_NAME_SPEC,
	  &autoCorrPtr->timeConstMode, autoCorrPtr->timeConstModeList,
	  (void * (*)) SetTimeConstMode_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Time offset, t0 (if -ve the end of the signal is assumed) (s)."),
	  UNIPAR_REAL,
	  &autoCorrPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_TIMECONSTANT], wxT("TIME_CONST"),
	  wxT("Time constant (or minimum time constant in 'Wiegrebe' mode) (s)."),
	  UNIPAR_REAL,
	  &autoCorrPtr->timeConstant, NULL,
	  (void * (*)) SetTimeConstant_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_TIMECONSTSCALE], wxT("T_CONST_SCALE"),
	  wxT("Time constant scale (only used in 'Wiegrebe' mode)."),
	  UNIPAR_REAL,
	  &autoCorrPtr->timeConstScale, NULL,
	  (void * (*)) SetTimeConstScale_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_MAXLAG], wxT("MAX_LAG"),
	  wxT("Maximum autocorrelation lag (s)."),
	  UNIPAR_REAL,
	  &autoCorrPtr->maxLag, NULL,
	  (void * (*)) SetMaxLag_Analysis_ACF);

	SetEnabledPars_Analysis_ACF();
	return(TRUE);

}

/********************************* SetDefaulEnabledPars ***********************/

/*
 * This routine sets the parameter list so that the correct parameters
 * are enabled/disabled.
 */

BOOLN
SetEnabledPars_Analysis_ACF(void)
{
	static const WChar *funcName = wxT("SetEnabledPars_Analysis_ACF");
	AutoCorrPtr	p = autoCorrPtr;

	if (p == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	p->parList->pars[ANALYSIS_ACF_TIMECONSTSCALE].enabled =
	  (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_ACF(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_ACF");

	if (autoCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (autoCorrPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(autoCorrPtr->parList);

}

/****************************** SetNormalisationMode **************************/

/*
 * This function sets the module's normalisationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNormalisationMode_Analysis_ACF(WChar * theNormalisationMode)
{
	static const WChar	*funcName = wxT("SetNormalisationMode_Analysis_ACF");
	int		specifier;

	if (autoCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theNormalisationMode,
		autoCorrPtr->normalisationModeList)) == ANALYSIS_NORM_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName,
		  theNormalisationMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	autoCorrPtr->normalisationMode = specifier;
	return(TRUE);

}

/****************************** SetTimeConstMode ******************************/

/*
 * This function sets the module's timeConstMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeConstMode_Analysis_ACF(WChar * theTimeConstMode)
{
	static const WChar	*funcName = wxT("SetTimeConstMode_Analysis_ACF");
	int		specifier;

	if (autoCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTimeConstMode,
		autoCorrPtr->timeConstModeList)) ==
		  ANALYSIS_ACF_TIMECONSTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theTimeConstMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	autoCorrPtr->updateProcessVariablesFlag = TRUE;
	autoCorrPtr->timeConstMode = specifier;
	SetEnabledPars_Analysis_ACF();
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_ACF(Float theTimeOffset)
{
	static const WChar	*funcName = wxT("SetTimeOffset_Analysis_ACF");

	if (autoCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	autoCorrPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeConstant *******************************/

/*
 * This function sets the module's timeConstant parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeConstant_Analysis_ACF(Float theTimeConstant)
{
	static const WChar	*funcName = wxT("SetTimeConstant_Analysis_ACF");

	if (autoCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeConstant < 0.0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g s)"), funcName,
		  theTimeConstant);
		return(FALSE);
	}
	autoCorrPtr->updateProcessVariablesFlag = TRUE;
	autoCorrPtr->timeConstant = theTimeConstant;
	return(TRUE);

}

/****************************** SetTimeConstScale *****************************/

/*
 * This function sets the module's timeConstScale parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeConstScale_Analysis_ACF(Float theTimeConstScale)
{
	static const WChar	*funcName = wxT("SetTimeConstScale_Analysis_ACF");

	if (autoCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeConstScale < 0.0) {
		NotifyError(wxT("%s: The time constant scale must be greater than zero ")
		  wxT("(%g)."), funcName, theTimeConstScale);
		return(FALSE);
	}
	autoCorrPtr->updateProcessVariablesFlag = TRUE;
	autoCorrPtr->timeConstScale = theTimeConstScale;
	return(TRUE);

}

/****************************** SetMaxLag *************************************/

/*
 * This function sets the module's maxLag parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxLag_Analysis_ACF(Float theMaxLag)
{
	static const WChar	*funcName = wxT("SetMaxLag_Analysis_ACF");

	if (autoCorrPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theMaxLag < 0.0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g s)"), funcName,
		  theMaxLag);
		return(FALSE);
	}
	autoCorrPtr->maxLag = theMaxLag;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_ACF(void)
{

	DPrint(wxT("Auto-correlation analysis module parameters:-\n"));
	DPrint(wxT("\tNormalisation mode = %s,"),
	  autoCorrPtr->normalisationModeList[autoCorrPtr->normalisationMode].name);
	DPrint(wxT("\tTime constant mode = %s,\n"), autoCorrPtr->timeConstModeList[
	  autoCorrPtr->timeConstMode].name);
	DPrint(wxT("\tTime offset = "));
	if (autoCorrPtr->timeOffset < 0.0)
		DPrint(wxT("end of signal,\n"));
	else
		DPrint(wxT("%g ms,\n"), MSEC(autoCorrPtr->timeOffset));
	if (autoCorrPtr->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE) {
		DPrint(wxT("\tMinimum time constant = %g ms,"), MSEC(autoCorrPtr->
		  timeConstant));
		DPrint(wxT("\tTime constant scale = %g (units),\n"), autoCorrPtr->
		  timeConstScale);
	} else
		DPrint(wxT("\tTime constant = %g ms,\n"), MSEC(autoCorrPtr->
		  timeConstant));
	DPrint(wxT("\tMaximum lag = %g ms.\n"), MSEC(autoCorrPtr->maxLag));
	return(TRUE);

}

/************************** SetParsPointer ************************************/

/*
 * This routine sets the global parameter pointer for the module to that
 * associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_ACF(ModulePtr theModule)
{
	static const WChar *funcName = wxT("SetParsPointer_Analysis_ACF");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	autoCorrPtr = (AutoCorrPtr) theModule->parsPtr;
	return(TRUE);

}

/************************** InitModule ****************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_ACF(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_ACF");

	if (!SetParsPointer_Analysis_ACF(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_ACF(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = autoCorrPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_ACF;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_ACF;
	theModule->PrintPars = PrintPars_Analysis_ACF;
	theModule->RunProcess = Calc_Analysis_ACF;
	theModule->SetParsPointer = SetParsPointer_Analysis_ACF;
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
CheckData_Analysis_ACF(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_ACF");
	Float	signalDuration, timeOffset;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if (autoCorrPtr->timeOffset > signalDuration + DSAM_EPSILON) {
		NotifyError(wxT("%s: Time offset is longer than signal duration."),
		  funcName);
		return(FALSE);
	}
	if (autoCorrPtr->maxLag > signalDuration + DSAM_EPSILON) {
		NotifyError(wxT("%s: maximum auto-correlation lag is longer than ")
		  wxT("signal duration."), funcName);
		return(FALSE);
	}
	timeOffset = (autoCorrPtr->timeOffset < 0.0)? signalDuration:
	  autoCorrPtr->timeOffset;
	if (autoCorrPtr->maxLag > (timeOffset + DSAM_EPSILON)) {
		NotifyError(wxT("%s: Time offset (%g ms) too for short maximum lag (%g ")
		  wxT("ms)."), funcName, MILLI(autoCorrPtr->timeOffset), MILLI(autoCorrPtr->
		  maxLag));
		return(FALSE);
	}
	if ((autoCorrPtr->normalisationMode == ANALYSIS_NORM_MODE_UNITY) &&
	  (autoCorrPtr->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE)) {
		NotifyError(wxT("%s: The 'unity' normalisation mode cannot be used ")
		  wxT("with the\n'Wiegrebe' tau mode.\n"));
		return(FALSE);
	}
	return(TRUE);

}

/**************************** TimeConstant ************************************/

/*
 * This function calculates the Wiegrebe proportional time constant.
 * It assumes that the module has been correctly initialised.
 */

Float
TimeConstant_Analysis_ACF(Float lag)
{
	Float	 timeConstant = lag * autoCorrPtr->timeConstScale;

	return((timeConstant > autoCorrPtr->timeConstant)? timeConstant:
	  autoCorrPtr->timeConstant);

}

/**************************** SunLimitIndex ***********************************/

/*
 * This function returns the summation limit index.
 * It assumes that the data object have been correctly initialised.
 */

ChanLen
SunLimitIndex_Analysis_ACF(EarObjectPtr data, Float timeConstant)
{
	ChanLen	sumLimitIndex;

	sumLimitIndex = (ChanLen) floor(timeConstant / _OutSig_EarObject(data)->dt +
	  0.5);
	return((sumLimitIndex < _OutSig_EarObject(data)->length)? sumLimitIndex:
	  _OutSig_EarObject(data)->length);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the exponential table.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_Analysis_ACF(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Analysis_ACF");
	register Float	*expDtPtr, dt;
	int		chan;
	Float	minDecay;
	ChanLen	i, minLagIndex;
	AutoCorrPtr	p = autoCorrPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_ACF();
		if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE) {
			dt = _InSig_EarObject(data, 0)->dt;
			if ((p->exponentDt = (Float *) calloc(p->maxLagIndex, sizeof(
			  Float))) == NULL) {
				NotifyError(wxT("%s: Out of memory for exponent lookup table."),
				  funcName);
				return(FALSE);
			}
			minLagIndex = (ChanLen) floor(p->timeConstant / (dt *
			  p->timeConstScale) + 0.5);
			minDecay = exp(-dt / p->timeConstant);
			for (i = 0, expDtPtr = p->exponentDt; i < p->maxLagIndex; i++,
			  expDtPtr++)
				*expDtPtr = (i > minLagIndex)? exp(-1.0 / (i * p->
				  timeConstScale)): minDecay;

		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetStaticTimeFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), 0.0);
		for (chan = 0; chan < _OutSig_EarObject(data)->numChannels; chan++)
			_OutSig_EarObject(data)->info.cFArray[chan] = _InSig_EarObject(data,
			  0)->info.cFArray[chan];
		SetInfoSampleTitle_SignalData(_OutSig_EarObject(data), wxT("Delay Lag ")
		  wxT("(s)"));
		SetNumWindowFrames_SignalData(_OutSig_EarObject(data), 0);
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
FreeProcessVariables_Analysis_ACF(void)
{
	if (autoCorrPtr->exponentDt) {
		free(autoCorrPtr->exponentDt);
		autoCorrPtr->exponentDt = NULL;
	}
	autoCorrPtr->updateProcessVariablesFlag = TRUE;

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
 * The setting of the static 'expDtPtr' pointer is done outside of the
 * 'initThreadRun' for the sake of speed, i.e. because it is defined as a
 * register.
 */

BOOLN
Calc_Analysis_ACF(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_ACF");
	register    Float  *expDtPtr = NULL;
	register    ChanData    *inPtr, *outPtr;
	int		chan;
	Float	wiegrebeTimeConst = 0.0;
	ChanLen i, deltaT;
	SignalDataPtr	inSignal, outSignal;
	AutoCorrPtr	p = autoCorrPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_ACF(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Auto Correlation Function (ACF) ")
		  wxT("analysis"));
		p->dt = _InSig_EarObject(data, 0)->dt;
		p->maxLagIndex = (ChanLen) floor(p->maxLag / p->dt + 0.5);
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, p->maxLagIndex, p->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Analysis_ACF(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->timeOffsetIndex = (ChanLen) ((p->timeOffset < 0.0)? _InSig_EarObject(
		  data, 0)->length: p->timeOffset / p->dt + 0.5);
		switch (p->timeConstMode) {
		case ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER:
			p->sumLimitIndex = SunLimitIndex_Analysis_ACF(data,
			  p->timeConstant * 3.0);
			p->expDecay = exp(-p->dt / p->timeConstant);
			break;
		case ANALYSIS_ACF_TIMECONSTMODE_NONE:
			p->sumLimitIndex = _OutSig_EarObject(data)->length;
			break;
		default:
			;
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER)
		expDtPtr = &p->expDecay;
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE)
			expDtPtr = p->exponentDt;
		for (deltaT = 0; deltaT < p->maxLagIndex; deltaT++, outPtr++) {
			if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE) {
				wiegrebeTimeConst = TimeConstant_Analysis_ACF(deltaT * p->dt);
				p->sumLimitIndex = SunLimitIndex_Analysis_ACF(data,
				  wiegrebeTimeConst);
			}
			inPtr = inSignal->channel[chan] + p->timeOffsetIndex - p->
			  sumLimitIndex;
			if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_NONE)
				for (i = 0, *outPtr = 0.0; i < p->sumLimitIndex; i++, inPtr++)
					*outPtr += (*inPtr * *(inPtr - deltaT));
			else
				for (i = 0, *outPtr = 0.0; i < p->sumLimitIndex; i++, inPtr++)
					*outPtr = (*outPtr * *expDtPtr) + (*inPtr * *(inPtr -
					  deltaT));
			if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE) {
				expDtPtr++;
				*outPtr /= wiegrebeTimeConst;
			}
			switch(p->normalisationMode) {
			case ANALYSIS_NORM_MODE_STANDARD:
				*outPtr /= p->maxLagIndex;
				break;
			case ANALYSIS_NORM_MODE_UNITY:
				sqrt(*outPtr /= p->timeConstant);
			default:
				;
			} /* switch */
		}
	}
	if (!outSignal->offset)	/* - only for one (first) thread */
		outSignal->numWindowFrames++;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

