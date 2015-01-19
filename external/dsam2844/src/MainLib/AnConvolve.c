/**********************
 *
 * File:		AnConvolve.c
 * Purpose:		This routine creates convoluted signal from two EarObject's
 *				output signals.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				The output signal will be monaural or binural (with interleaved
 *				channels: LRLRLR) depending upon the format of the two input
 *				signals, which will both be the same.
 *				The convolution is defined as (R*S)[j] = sum(j = 0, m - 1)
 *				{ S[j-k]*R[k]}.
 * Author:		Lowel O'Mard and Enrique Lopez-Poveda.
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
#include "GeModuleMgr.h"
#include "AnConvolve.h"

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
SetParsPointer_Analysis_Convolution(ModulePtr theModule)
{
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_Convolution(ModulePtr theModule)
{
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->RunProcess = Calc_Analysis_Convolution;
	theModule->SetParsPointer = SetParsPointer_Analysis_Convolution;
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
CheckData_Analysis_Convolution(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_Convolution");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (data->numInSignals != 2) {
		NotifyError(wxT("%s: The process EarObject must have 2 inputs from ")
		  wxT("other processes."), funcName);
		return(FALSE);
	}
	if (!CheckPars_SignalData(_InSig_EarObject(data, 0)) ||
	  !CheckPars_SignalData(_InSig_EarObject(data, 1))) {
		NotifyError(wxT("%s: Input signals not correctly set."), funcName);		
		return(FALSE);
	}
	if (fabs(_InSig_EarObject(data, 0)->dt - _InSig_EarObject(data, 1)->dt) >
	  DSAM_EPSILON) {
		NotifyError(wxT("%s: Input signals do not have the same sampling ")
		  wxT("interval (%.10g/%.10g s)."), funcName, _InSig_EarObject(data,
		  0)->dt, _InSig_EarObject(data, 1)->dt);		
		return(FALSE);
	}
	if (_InSig_EarObject(data, 0)->numChannels != _InSig_EarObject(data,
	  1)->numChannels) {
		NotifyError(wxT("%s: Input signals have different numbers of channels ")
		  wxT(" (%d/%d)."), funcName, _InSig_EarObject(data, 0)->
		    numChannels, _InSig_EarObject(data, 1)->numChannels);		
		return(FALSE);
	}
	if (_InSig_EarObject(data, 0)->interleaveLevel != _InSig_EarObject(data,
	  1)->interleaveLevel) {
		NotifyError(wxT("%s: Input signals do not have the same interleave ")
		  wxT("level."), funcName);		
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
Calc_Analysis_Convolution(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_Convolution");
	int		chan;
	register	ChanData	*inR, *inS, *outPtr, *endInR;
	ChanLen	i;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_Convolution(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Auto-convolution Process"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data),
		  _InSig_EarObject(data, 0)->interleaveLevel);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++, outPtr++) {
			inR = _InSig_EarObject(data, 1)->channel[chan];
			endInR = inR + _InSig_EarObject(data, 1)->length;
			inS = _InSig_EarObject(data, 0)->channel[chan] + i;
			for (*outPtr = 0.0;(inS >= _InSig_EarObject(data, 0)->channel[
			  chan]) && (inR < endInR); inR++, inS--)
				*outPtr += *inR * *inS;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
