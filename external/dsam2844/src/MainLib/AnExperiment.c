/**********************
 *
 * File:		AnExperiment.c
 * Purpose:		This module contains various analysis routines, which have
 *				been used in real experiments.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		19 Mar 1993
 * Updated:		10 Oct 1996
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
#include "AnGeneral.h"
#include "AnExperiment.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** FindThresholdIntensity ************************/

/*
 * This general routine calculates the threshold intensity of model, starting
 * from an initial 'guess' intensity.
 * It returns the respective intensity that produces the threshold intensity.
 * It works only on a single channel.
 * This routine now expects the intensity calculation to be done at the end of
 * the "RunModel" simulation routine.
 */

Float
FindThresholdIntensity_ExpAnalysis(EarObjectPtr (* RunModel)(void),
  BOOLN (* SetIntensity)(Float), Float targetThreshold,
  Float targetAccuracy, BOOLN *firstPass)
{
	static const WChar *funcName = wxT("FindThresholdIntensity_ExpAnalysis");
	int		i;
	Float	intensityDiff, gradient, presentInputIntensity, lastOutIntensity;
	Float	absoluteIntensityDiff, bestIntensityDiff = 0.0;
	Float	bestOutputIntensity = 0.0, bestInputIntensity = 0.0;
	static Float	lastInputIntensity, presentOutIntensity;
	EarObjectPtr	modelResp;

	if (targetAccuracy <= 0.0) {
		NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Illegal target ")
		  wxT("threshold accuracy (= %g)."), targetAccuracy);
		return(0.0);
	}
	if (*firstPass) {
		*firstPass = FALSE;
		lastInputIntensity = ANALYSIS_MAX_INTENSITY_GUESS;
		if (!(* SetIntensity)(lastInputIntensity)) {
			NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Intensity ")
			  wxT("not set."));
			return(0.0);
		}
		if ((modelResp = (* RunModel)()) == NULL) {
			NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Could not ")
			  wxT("calculate present intensity (first pass).  Returning ")
			  wxT("zero."));
			return(0.0);
		}
		presentOutIntensity = GetResult_EarObject(modelResp, ANALYSIS_CHANNEL);
	}
	presentInputIntensity = 1.01 * lastInputIntensity;
	for (i = 0, bestIntensityDiff = HUGE_VAL; i < MAXIMUM_THRESHOLD_LOOP; i++) {
		lastOutIntensity = presentOutIntensity;
		if (!(* SetIntensity)(presentInputIntensity)) {
			NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Could not ")
			  wxT("set intensity."));
			return(0.0);
		}
		if ((modelResp = (* RunModel)()) == NULL) {
			NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Could not ")
			  wxT("calculate present output intensity.  Returning zero."));
			return(0.0);
		}
		presentOutIntensity = GetResult_EarObject(modelResp, ANALYSIS_CHANNEL);
		intensityDiff = targetThreshold - presentOutIntensity;
		absoluteIntensityDiff = fabs(intensityDiff);
		if (absoluteIntensityDiff <= targetAccuracy)
			return(presentInputIntensity);
		if (absoluteIntensityDiff < bestIntensityDiff) {
			bestIntensityDiff = absoluteIntensityDiff;
			bestInputIntensity = presentInputIntensity;
			bestOutputIntensity = presentOutIntensity;
		}
		gradient = (presentOutIntensity - lastOutIntensity) /
		  (presentInputIntensity - lastInputIntensity);
		lastInputIntensity = presentInputIntensity;
		presentInputIntensity = intensityDiff / gradient +
		  presentInputIntensity;
	}
	NotifyWarning(wxT("%s: Looped %d times and couldn't\n\tcome any closer to ")
	  wxT("the threshold than %g dB.\n"), funcName, MAXIMUM_THRESHOLD_LOOP,
	  bestIntensityDiff);
	lastInputIntensity = bestInputIntensity;
	presentOutIntensity = bestOutputIntensity;
	return(bestInputIntensity);

}

/****************************** FindThresholdIntensity...Slow *****************/

/*
 * This general routine calculates the threshold intensity of model, starting
 * from an initial 'guess' intensity, set at the target threshold.
 * It returns the respective intensity that produces the threshold intensity.
 * It works only on a single channel.
 * This routine now expects the intensity calculation to be done at the end of
 * the "RunModel" simulation routine.
 */

Float
FindThresholdIntensity_ExpAnalysis_Slow(EarObjectPtr (* RunModel)(void),
  BOOLN (* SetIntensity)(Float), Float targetThreshold,
  Float targetAccuracy, Float initialDeltaIntensity)
{
	static const WChar *funcName = wxT(
	  "FindThresholdIntensity_ExpAnalysis_Slow");
	Float	inputIntensity, deltaIntensity;
	Float	outIntensity;
	EarObjectPtr	modelResp;

	if (targetAccuracy <= 0.0) {
		NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis_Slow: Illegal ")
		  wxT("target threshold accuracy (= %g)."), targetAccuracy);
		return(0.0);
	}
	inputIntensity = targetThreshold;
	deltaIntensity = initialDeltaIntensity;
	do {
		if (!(* SetIntensity)(inputIntensity + deltaIntensity)) {
			NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis_Slow: Could")
			  wxT(" not set intensity."));
			return(0.0);
		}
		if ((modelResp = (* RunModel)()) == NULL) {
			NotifyWarning(wxT("%s: Could not calculate present output ")
			  wxT("intensity.  Returning zero."), funcName);
			return(0.0);
		}
		outIntensity = GetResult_EarObject(modelResp, ANALYSIS_CHANNEL);
		if (outIntensity > targetThreshold)
			deltaIntensity /= 2.0;
		else
			inputIntensity += deltaIntensity;
	} while (fabs(targetThreshold - outIntensity) >
	  targetAccuracy);
	return(inputIntensity - deltaIntensity);

}

/****************************** CalcQValue ************************************/

/*
 * This function calculates the Q-value for a model.
 * It works only on a single channel.
 * This routine now expects the intensity calculation to be done at the end of
 * the "RunModel" simulation routine.
 */

Float
CalcQValue_ExpAnalysis(EarObjectPtr (* RunModel)(void),
  BOOLN (* SetStimulusFreq)(Float), Float centreFreq,
  Float variationFromCF, Float dBDownDiff, Float initialFreqIncrement,
  Float accuracy)
{
	static const WChar *funcName = wxT("CalcQValue_ExpAnalysis");
	BOOLN	forewards;
	Float	intensity, maxIntensity, frequency;
	Float	minTestFreq, maxTestFreq, minFreq, intensityDiff, freqIncrement;
	Float	freqVariation;
	EarObjectPtr	modelResp;

	if ( !(* SetStimulusFreq)(centreFreq) ) {
		NotifyError(wxT("%s: Could not set model frequency parameter (CF).  ")
		  wxT("Returning Q = -1.0."), funcName);
		return(-1.0);
	}
	if ((modelResp = (* RunModel)()) == NULL) {
		NotifyError(wxT("%s: Could not calculate model intensity (at CF).  ")
		  wxT("Returning Q = -1.0."), funcName);
		return(-1.0);
	}
	maxIntensity = GetResult_EarObject(modelResp, ANALYSIS_CHANNEL);
	initialFreqIncrement = fabs(initialFreqIncrement);
	freqVariation = centreFreq * variationFromCF;
	if ((initialFreqIncrement - freqVariation) <= 0.1)
		initialFreqIncrement = freqVariation - 1.0;
	minTestFreq = centreFreq - freqVariation;
	maxTestFreq = centreFreq + freqVariation;
	frequency = centreFreq;
	freqIncrement = -initialFreqIncrement;
	minFreq = 0.0;
	forewards = FALSE;

	while ((frequency > minTestFreq) && (frequency < maxTestFreq) ) {
		frequency += freqIncrement;
		if ( !(* SetStimulusFreq)(frequency) ) {
			NotifyWarning(wxT("%s: Could not set model frequency parameter.  ")
			  wxT("Returning Q = -1.0."), funcName);
			return(-1.0);
		}
		if ((modelResp = (* RunModel)()) == NULL) {
			NotifyError(wxT("%s: Could not calculate model intensity.  ")
			  wxT("Returning Q = -1.0."), funcName);
			return(-1.0);
		}
		intensity = GetResult_EarObject(modelResp, ANALYSIS_CHANNEL);
		if ( (intensityDiff = (maxIntensity - intensity)) >= dBDownDiff ) {
			if (fabs(freqIncrement) < DSAM_EPSILON) {
				NotifyError(wxT("%s: Couldn't get closer to %g dB down than\n")
				  wxT("%g dB down."), funcName, dBDownDiff, intensityDiff);
				return(centreFreq / (frequency - minFreq));
			}
			if ((intensityDiff - dBDownDiff) < accuracy) {
				if (forewards)
					return(centreFreq / (frequency - minFreq));
				forewards = TRUE;
				minFreq = frequency;
				/* Now search forwards. */
				freqIncrement = initialFreqIncrement;
				frequency = centreFreq;
			} else {
				frequency -= freqIncrement;
				freqIncrement /= 2.0;
			}
		}
	}
	NotifyError(wxT("%s: Q value %g dB down not in frequency\nrange %g - %g ")
	  wxT("Hz (variation from CF = %g).  Returning Q = -1.0."), funcName,
	  dBDownDiff, minTestFreq, maxTestFreq, variationFromCF);
	return(-1.0);

}

/****************************** Calc2CompAdapt ********************************/

/*
 * This analysis routine calculates the two component adaptation constants.
 * It expects the "work" EarObject to be connected to a post stimulus time
 * histogram.
 * This algorithm uses the Meddis Approximation: Y ~= Ast*exp(-t/Tst) + Ass,
 * for t > ~40 ms.
 * The algorithm expects the signal to have a long length.
 * It assumes that the highest peak is the onset peak.
 * Ass is calculated over the last 5% of the signal.
 * This routine destroys its output signal when it has finished using it.
 *
 */

#define	STEADY_STATE_REGION_PERCENTAGE	0.8

BOOLN
Calc2CompAdapt_ExpAnalysis(Float Tst[], Float Tr[], EarObjectPtr data,
  Float shortTermT1, Float shortTermPeriod, Float rapidAdaptPeriod)
{
	static const WChar *funcName = wxT("Calc2CompAdapt_ExpAnalysis");
	BOOLN	ok = TRUE;
	int		chan;
	Float	Ast, Ass, sum;
	Float	gradient, constant, maxValue, dt, deltaY;
	register	ChanData	*inPtr, *outPtr;
	ChanLen	sT1Indx, sTPeriodIndx, i, rT1Indx, rTPeriodIndx;
	ChanLen	aveTimeOffsetIndex, avePeriodIndex;
	SignalDataPtr	inSignal, outSignal;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, wxT("Two component adaptation ")
	  wxT("calculation."));
	if (!CheckPars_SignalData(_InSig_EarObject(data, 0))) {
		NotifyError(wxT("%s: Input signal not correctly set."), funcName);
		return(FALSE);
	}
	inSignal = _InSig_EarObject(data, 0);
	dt = inSignal->dt;
	sT1Indx = (ChanLen) (shortTermT1 / dt);
	if (sT1Indx > inSignal->length) {
		NotifyError(wxT("%s: Invalid short term T1 (%g ms)."), funcName,
		  MSEC(shortTermT1));
		return(FALSE);
	}
	sTPeriodIndx = (ChanLen) (shortTermPeriod / dt);
	if (sTPeriodIndx + sT1Indx > inSignal->length) {
		NotifyError(wxT("%s: Invalid short term period (%g ms)."), funcName,
		  MSEC(shortTermPeriod));
		return(FALSE);
	}
	rTPeriodIndx = (ChanLen) (rapidAdaptPeriod / dt);
	if (!InitOutSignal_EarObject(data, inSignal->numChannels, inSignal->length,
	  inSignal->dt)) {
		NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
		return(FALSE);
	}
	ResetOutSignal_EarObject(data);
	outSignal = _OutSig_EarObject(data);
	aveTimeOffsetIndex = (ChanLen) (inSignal->length *
	  STEADY_STATE_REGION_PERCENTAGE);
	avePeriodIndex = inSignal->length - aveTimeOffsetIndex - 1;/*Miss last bin*/
	for (chan = 0; chan < inSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan] + aveTimeOffsetIndex;
		for (i = 0, sum = 0.0; i < avePeriodIndex; i++)
			sum += *inPtr++;
		Ass = sum / avePeriodIndex;
		inPtr = inSignal->channel[chan];
		outPtr = _OutSig_EarObject(data)->channel[chan];
		for (i = 0; i < inSignal->length; i++) {
			deltaY = *inPtr++ - Ass;
			*outPtr++ = log(deltaY * deltaY);
		}
		if (!LinearRegression_GenAnalysis(&constant, &gradient,
		  _OutSig_EarObject(data)->channel[chan], dt, sT1Indx, sTPeriodIndx))
			ok = FALSE;
		Tst[chan] = -2.0 / gradient;
		Ast = exp(constant / 2.0);
		rT1Indx = 0;
		for (i = 0, maxValue = 0.0, inPtr = inSignal->channel[chan]; i <
		  inSignal->length; i++, inPtr++)
			if (*inPtr > maxValue) {
				maxValue = *inPtr;
				rT1Indx = i;
			}
		if (rT1Indx + rTPeriodIndx > inSignal->length) {
			NotifyError(wxT("%s: Invalid rapid adaptation period (%g ms)."),
			  funcName, MSEC(rapidAdaptPeriod));
			return(FALSE);
		}
		inPtr = inSignal->channel[chan];
		outPtr = _OutSig_EarObject(data)->channel[chan];
		for (i = rT1Indx; i < (rT1Indx + rTPeriodIndx); i++) {
			deltaY = *inPtr++ - Ast * exp(-(i * dt) / Tst[chan]) - Ass;
			*outPtr++ = log(deltaY * deltaY);
		}
		if (!LinearRegression_GenAnalysis(&constant, &gradient,
		  _OutSig_EarObject(data)->channel[chan], dt, rT1Indx, rTPeriodIndx))
			ok = FALSE;
		Tr[chan] = -2.0 / gradient;
	}
	return(ok);

}

#undef		STEADY_STATE_REGION_PERCENTAGE

