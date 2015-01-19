/**********************
 *
 * File:		AnIntensity.c
 * Purpose:		This module calculates the intensity for a signal, starting
 *				from an offset position.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard
 * Created:		12 Jun 1996
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

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
#include "AnIntensity.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IntensityPtr	intensityPtr = NULL;

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
Free_Analysis_Intensity(void)
{
	if (intensityPtr == NULL)
		return(FALSE);
	if (intensityPtr->parList)
		FreeList_UniParMgr(&intensityPtr->parList);
	if (intensityPtr->parSpec == GLOBAL) {
		free(intensityPtr);
		intensityPtr = NULL;
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
Init_Analysis_Intensity(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_Intensity");

	if (parSpec == GLOBAL) {
		if (intensityPtr != NULL)
			Free_Analysis_Intensity();
		if ((intensityPtr = (IntensityPtr) malloc(sizeof(Intensity))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (intensityPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	intensityPtr->parSpec = parSpec;
	intensityPtr->timeOffset = 2.5e-3;
	intensityPtr->extent = -1.0;

	if (!SetUniParList_Analysis_Intensity()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_Intensity();
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
SetUniParList_Analysis_Intensity(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_Intensity");
	UniParPtr	pars;

	if ((intensityPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_INTENSITY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = intensityPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_INTENSITY_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Time from which to start calculation (s)."),
	  UNIPAR_REAL,
	  &intensityPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_Intensity);
	SetPar_UniParMgr(&pars[ANALYSIS_INTENSITY_EXTENT], wxT("EXTENT"),
	  wxT("Time over which calculation is performed: -ve value assumes end of ")
	  wxT("signal (s)."),
	  UNIPAR_REAL,
	  &intensityPtr->extent, NULL,
	  (void * (*)) SetExtent_Analysis_Intensity);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_Intensity(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_Intensity");

	if (intensityPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (intensityPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(intensityPtr->parList);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_Intensity(Float theTimeOffset)
{
	static const WChar	*funcName = wxT("SetTimeOffset_Analysis_Intensity");

	if (intensityPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError(wxT("%s: Illegal offset (%g ms)."), funcName, MSEC(
		  theTimeOffset));
		return(FALSE);
	}
	intensityPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetExtent *************************************/

/*
 * This function sets the module's extent parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetExtent_Analysis_Intensity(Float theExtent)
{
	static const WChar	*funcName = wxT("SetExtent_Analysis_Intensity");

	if (intensityPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	intensityPtr->extent = theExtent;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_Intensity(void)
{
	DPrint(wxT("Intensity Analysis Module Parameters:-\n"));
	DPrint(wxT("\tTime offset = %g ms,"), MSEC(intensityPtr->timeOffset));
	DPrint(wxT("\tTime extent = "));
	if (intensityPtr->extent < 0.0)
		DPrint(wxT("<end of signal>"));
	else
		DPrint(wxT("%g ms\n"), MSEC(intensityPtr->extent));
	DPrint(wxT(".\n"));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_Intensity(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_Intensity");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	intensityPtr = (IntensityPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_Intensity(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_Intensity");

	if (!SetParsPointer_Analysis_Intensity(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_Intensity(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = intensityPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_Intensity;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_Intensity;
	theModule->PrintPars = PrintPars_Analysis_Intensity;
	theModule->RunProcess = Calc_Analysis_Intensity;
	theModule->SetParsPointer = SetParsPointer_Analysis_Intensity;
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
CheckData_Analysis_Intensity(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_Intensity");
	Float	duration;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	duration =  _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if (intensityPtr->timeOffset >= duration) {
		NotifyError(wxT("%s: Time offset (%g ms) is longer than signal ")
		  wxT("duration (%g ms)."), funcName, MSEC(intensityPtr->timeOffset),
		    MSEC(duration));
		return(FALSE);
	}
	if (intensityPtr->extent > 0.0) {
		if ((intensityPtr->timeOffset + intensityPtr->extent) > duration) {
			NotifyError(wxT("%s: Time offset (%g ms) + extent (%g ms) is ")
			  wxT("longer than signal duration (%g)."), funcName,  MSEC(
			  intensityPtr->timeOffset), MSEC(intensityPtr->extent), MSEC(
			  duration));
			return(FALSE);
		}
		if (intensityPtr->extent < _InSig_EarObject(data, 0)->dt) {
			NotifyError(wxT("%s: Time extent is too small (%g ms).  It should ")
			  wxT("be greater than the sampling interval (%g ms)."), funcName,
			  MSEC(intensityPtr->extent), MSEC(_InSig_EarObject(data, 0)->dt));
			return(FALSE);
		}
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
Calc_Analysis_Intensity(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_Intensity");
	register	ChanData	 *inPtr, sum;
	int		chan;
	ChanLen	i;
	SignalDataPtr	outSignal;
	IntensityPtr	p = intensityPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_Intensity(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Intensity Analysis Module"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, 1, 1.0)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		p->timeOffsetIndex = (ChanLen) (p->timeOffset / _InSig_EarObject(data,
		  0)->dt + 0.5);
		p->wExtent = (p->extent < 0.0)? _InSig_EarObject(data, 0)->length - p->
		  timeOffsetIndex: (ChanLen) (p->extent / _InSig_EarObject(data, 0)->dt +
		  0.5);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan] + p->timeOffsetIndex;
		for (i = 0, sum = 0.0; i < p->wExtent; i++, inPtr++)
			sum += *inPtr * *inPtr;
		outSignal->channel[chan][0] = (ChanData) DB_SPL(sqrt(sum / p->wExtent));
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

