/**********************
 *
 * File:		AnSynchIndex.c
 * Purpose:		Calculates the synchronisation index (vector strength) from
 *				the input signal.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				The input is expected to come from a period histogram (PH).
 * Author:		L. P. O'Mard
 * Created:		1 Mar 1996
 * Updated:
 * Copyright:	(c) 1997, 2010 Lowel P. O'Mard
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
#include "AnSynchIndex.h"

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
SetParsPointer_Analysis_SynchIndex(ModulePtr theModule)
{
	/* static const WChar *funcName = "SetParsPointer_Analysis_SynchIndex"; */

	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_SynchIndex(ModulePtr theModule)
{
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->RunProcess = Calc_Analysis_SynchIndex;
	theModule->SetParsPointer = SetParsPointer_Analysis_SynchIndex;
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
CheckData_Analysis_SynchIndex(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_SynchIndex");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
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
Calc_Analysis_SynchIndex(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_SynchIndex");
	register	ChanData	 *r;
	int		chan;
	Float	theta, sineSum, cosineSum, rSum;
	ChanLen	i;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_SynchIndex(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Synchronisation Index Analysis"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels, 1,
		  1.0)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		sineSum = cosineSum = rSum = 0.0;
		r = _InSig_EarObject(data, 0)->channel[chan];
		for (i = 0; i < _InSig_EarObject(data, 0)->length; i++) {
			theta = PIx2 * i / _InSig_EarObject(data, 0)->length;
			sineSum += *r * sin(theta);
			cosineSum += *r * cos(theta);
			rSum += fabs(*r++);
		}
		outSignal->channel[chan][0] = (rSum == 0.0)? 0.0: sqrt(sineSum *
		  sineSum + cosineSum * cosineSum) / rSum;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

