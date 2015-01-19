/**********************
 *
 * File:		UtStddise.c
 * Purpose:		This routine Standardises a signal: sum(Si) = 0,
 *			 	sum(Si * Si)/N = 1.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				Reference (LUT:542b)
 *				The method used uses the standard deviation and mean of the
 *				data i.e sd = sum{(x - mean)^2 / n). The standardised values,
 *				Si = (X - mean) / sd.
 * Author:		L. P. O'Mard
 * Created:		27 May 1996
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
#include "UtStddise.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_Standardise(ModulePtr theModule)
{
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Standardise(ModulePtr theModule)
{

	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->RunProcess = Process_Utility_Standardise;
	theModule->SetParsPointer = SetParsPointer_Utility_Standardise;
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
CheckData_Utility_Standardise(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_Standardise");

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
Process_Utility_Standardise(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_Standardise");
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	Float	mean, sumXX, sumX, standardDev;
	ChanLen	i;
	SignalDataPtr	inSignal, outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_Standardise(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Signal Standardisation Process"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal= _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan];
		outPtr = outSignal->channel[chan];
		for (i = 0, sumX = sumXX = 0.0; i < inSignal->length; i++, inPtr++) {
			sumX += *inPtr;
			sumXX += SQR(*inPtr);
		}
		mean = sumX / inSignal->length;
		standardDev = sqrt((sumXX - 2.0 * mean * sumX) / inSignal->length +
		  SQR(mean));
		inPtr = inSignal->channel[chan];
		for (i = 0; i < inSignal->length; i++)
			*outPtr++ = (*inPtr++ - mean) / standardDev;

	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

