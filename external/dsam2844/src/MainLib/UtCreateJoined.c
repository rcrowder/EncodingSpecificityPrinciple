/**********************
 *
 * File:		UtCreateJoined.c
 * Purpose:		To create a signal by joining the output signals of multiply
 *				connected EarObjects.
 * Comments:	Written using ModuleProducer version 1.11 (Apr  9 1997).
 * Author:		L. P. O'Mard
 * Created:		26 Aug 1997
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
#include "UtCreateJoined.h"

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
SetParsPointer_Utility_CreateJoined(ModulePtr theModule)
{
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_CreateJoined(ModulePtr theModule)
{
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->RunProcess = Process_Utility_CreateJoined;
	theModule->SetParsPointer = SetParsPointer_Utility_CreateJoined;
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
CheckData_Utility_CreateJoined(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_CreateJoined");
	int		i;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	for (i = 0; i < data->numInSignals; i++) {
		if ((i != 0) && ((_InSig_EarObject(data, 0)->dt != _InSig_EarObject(
		  data, i)->dt) || (_InSig_EarObject(data, 0)->numChannels !=
		  _InSig_EarObject(data, i)->numChannels))) {
			NotifyError(wxT("%s: All signals must of the same number of ")
			  wxT("channels\nand sampling Interval (signal %d is different).")
			  wxT("\n"), funcName, i);
			return(FALSE);
		}
	}
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
Process_Utility_CreateJoined(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_CreateJoined");
	register	ChanData	 *inPtr, *outPtr;
	int		i, chan;
	ChanLen	j, joinedLength;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_CreateJoined(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Create Joined Utility module ")
		  wxT("process"));
		for (i = 0, joinedLength = 0; i < data->numInSignals; i++)
			joinedLength += _InSig_EarObject(data, i)->length;
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, joinedLength, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		for (i = 0; i < data->numInSignals; i++) {
			inPtr = _InSig_EarObject(data, i)->channel[chan];
			for (j = 0; j < _InSig_EarObject(data, i)->length; j++)
				*outPtr++ = *inPtr++;
		}			
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

