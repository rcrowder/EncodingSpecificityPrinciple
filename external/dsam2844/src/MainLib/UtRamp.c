/**********************
 * File:		UtRamp.c
 * Purpose:		This is the signal ramp module. The ramps gradually raise or
 * 				lower the signal intensity to full strength or from full strength
 * 				to zero at the beginning or end of the signal respectively.
 * Comments:	Valid ramp functions are defined as a function which varies from
 * 				0 to 1 as a function of the ramp interval.
 *				The functions used must be defined in the format given below (see
 *				The Sine_Ramp and Ramp_linear) and are passed to the
 *				RampUpOutSignal_Ramp and RampDownOutSignal_Ramp functions as
 *				the arguments.
 *				Impulse signals, such as clicks, should not be ramped: if a
 *				warning message is given, in this case ignore it.
 * 				This module allows the processing of a simulation to be
 *				interrupted, producing a specified message.
 * Author:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:
 * Copyright:	(c) 1993, 2010 Lowel P. O'Mard
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
#include "UtRamp.h"

/****************************** CheckPars *************************************/

/*
 * This function checks that the signal to be ramped has been initialised, and
 * that the ramp interval is not greater than the signal length.
 * It also sets the signalData rampFlag to TRUE.
 */

BOOLN
CheckPars_Ramp(SignalData *theSignal, Float timeInterval)
{
	static const WChar *funcName = wxT("CheckPars_Ramp");

	if ((ChanLen) (timeInterval / theSignal->dt) > theSignal->length) {
		NotifyError(wxT("%s: Ramp time interval is longer than signal!"), funcName);
		return(FALSE);
	}
	theSignal->rampFlag = TRUE;
	return(TRUE);

}

/****************************** Sine ******************************************/

/*
 * This is a standard ramp function, to be passed as an argument to the
 * Ramp_UP and Ramp_Down routines.  It varies from 0 to 1 as the step ranges
 * from 0 to the interval index length.
 */

Float
Sine_Ramp(ChanLen step, ChanLen intervalIndex)
{
	return(sin(step * PI / 2.0 / intervalIndex));
}

/****************************** Linear ****************************************/

/*
 * This is a standard ramp function, to be passed as an argument to the
 * Ramp_UP and Ramp_Down routines.  It varies from 0 to 1 as the step ranges
 * from 0 to the interval index length.
 */

Float
Linear_Ramp(ChanLen step, ChanLen intervalIndex)
{
	return((Float) step / intervalIndex);
}

/****************************** RampUpOutSignal *******************************/

/*
 * This function searches for the first non-zero signal intensity in each
 * channel of a signal data set.  It then starts the ramp from the preceding
 * zero (or the start of the signal if there are no preceding zero's).
 * The ramp function is passed as an argument.
 * In segmented mode, this routine will only work for the first signal section.
 * i.e. if the ramp interval has expired, then all subsequent segments will
 * be markes as ramped.
 */

void
RampUpOutSignal_Ramp(EarObjectPtr data, Float (* RampFunction)(ChanLen,
  ChanLen), Float timeInterval)
{
	static const WChar *funcName = wxT("RampUpOutSignal_Ramp");
	int		chan;
	ChanLen	i, intervalIndex;
	ChanData	*dataPtr, *endPtr;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		exit(1);
	}
	if (!CheckPars_SignalData(_OutSig_EarObject(data))) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
		exit(1);
	}
	intervalIndex = (ChanLen) (timeInterval / _OutSig_EarObject(data)->dt);
	if (_WorldTime_EarObject(data) > intervalIndex) {
		_OutSig_EarObject(data)->rampFlag = TRUE;
		return;
	}
	if (!CheckPars_Ramp(_OutSig_EarObject(data), timeInterval)) {
		NotifyError(wxT("%s: Function arguments not correctly initialised\n")
		  wxT("for EarObject: %s."), funcName, data->processName);
		exit(1);
	}
	for (chan = 0; chan < _OutSig_EarObject(data)->numChannels; chan++) {
		dataPtr = _OutSig_EarObject(data)->channel[chan];
		endPtr = dataPtr + _OutSig_EarObject(data)->length;
		for ( ; (dataPtr < endPtr) && (*dataPtr == 0.0); dataPtr++)
			;
		if (dataPtr > _OutSig_EarObject(data)->channel[chan])
			dataPtr--;
		for (i = 0; (i < intervalIndex) && (dataPtr < endPtr); i++)
			*dataPtr++ *= (*RampFunction)(i, intervalIndex);
	}

}

/****************************** RampDownOutSignal *****************************/

/*
 * This function searches for the last non-zero signal intensity from the end
 * of each channel of a signal data set.
 * It then starts the ramp from the following zero (or from the end of
 * the signal if there are no following zero's).
 */

void
RampDownOutSignal_Ramp(EarObjectPtr data, Float (* RampFunction)(ChanLen,
  ChanLen), Float timeInterval)
{
	static const WChar *funcName = wxT("RampDownOutSignal_Ramp");
	int		chan;
	ChanLen	i, intervalIndex;
	ChanData	*dataPtr, *startPtr;
	SignalDataPtr	outSignal;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		exit(1);
	}
	if (!CheckPars_SignalData(_OutSig_EarObject(data))) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
		exit(1);
	}
	if (!CheckPars_Ramp(_OutSig_EarObject(data), timeInterval)) {
		NotifyError(wxT("%s: Function arguments not correctly initialised ")
		  wxT("for\nEarObject: %s."), funcName, data->processName);
		exit(1);
	}
	outSignal = _OutSig_EarObject(data);
	intervalIndex = (ChanLen) (timeInterval / outSignal->dt);
	for (chan = 0; chan < outSignal->numChannels; chan++) {
		startPtr = outSignal->channel[chan];
		dataPtr = startPtr + outSignal->length;
		for ( ; (dataPtr > startPtr) && (*dataPtr == 0.0); dataPtr--)
			;
		if (dataPtr < startPtr + outSignal->length)
			dataPtr++;
		for (i = 0; (i < intervalIndex) && ( dataPtr >= startPtr); i++)
			*--dataPtr *= (*RampFunction)(i, intervalIndex);
	}

}

