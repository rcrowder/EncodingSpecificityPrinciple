/**********************
 *
 * File:		UtFilters.c
 * Purpose:		This module contains the functions that initialise and
 *				generate various filters., e.g. the  2-pole GammaTone filters,
 *				the 2-pole Butterworth low pass filter, the Bessel low pass
 *				filter and the non-linear compresion filter. (They were
 *				revised from M.A. Stone's HUMAN.c 8-10-92 (Exp. Psych.
 *				Cambridge, England).
 * Comments:	Some IIR filters are used, which performs z plane filters for
 *				two-pole coefficients. A digital band pass filter has been
 *				added.
 *				29-08-95 E.A. Lopez-Poveda: According to the theory, DSP,
 *				William D. Stanley, Reston press, 1st. Ed., p. 172, tan(theta)
 *				must always be positive, ie, the argument of the tangent must
 *				be always smaller than PI / 2. This is being checked here in
 *				all relevant routines.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		26 Jan 1997
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
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "UtFilters.h"
#include "UtCmplxM.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/* Bessel coefficents plus scale factor for gain loss. */

Float	Filters_bess2Poly[4] = {3.0, 3.0, 1.0, 1.2589};

/* Butterworth coefficients plus scale factor for gain loss. */

Float	Filters_butt2Poly[4] = {1.0, 1.414213562, 1.0, 1.4142};

/**************************** ZeroArray ***************************************/

/*
 * This routine zero's the elements of an array.
 */

void
ZeroArray_Filters(Float *p, int length)
{
	int		i;

	for (i = 0; i < length; i++)
		*(p++) = 0.0;
}

/**************************** InitGammaToneCoeffs *****************************/

/*-----------------------------------------------------------------*/
/*---- biquadratic filter table                                    */
/*
 * The function returns a pointer to the coefficients structure.
 */

GammaToneCoeffsPtr
InitGammaToneCoeffs_Filters(Float centreFreq, Float bWidth3dB, int cascade,
  Float sampleClk)
{
	static const WChar *funcName = wxT("InitGammaToneCoeffs_Filters");
	int		stateVectorLength;
	Float theta, bWidth3dBdt, sin_theta, cos_theta;
	Float k0num, k1num;	/* numerator coefficients */
	Float k1denom, k2denom;	/* denominator coefficients */
	Complex cf_num, cf_denom, Tf, var1, var2, var3;
    GammaToneCoeffsPtr p;

	bWidth3dBdt = (TwoPi * bWidth3dB) / sampleClk;
	theta = TwoPi * centreFreq/sampleClk;
	cos_theta = cos(theta);
	sin_theta = sin(theta);

	k1num   = exp( -bWidth3dBdt) * cos_theta; k0num = 1.;
	k1denom = -2. * k1num; k2denom =  exp(-2.0 * bWidth3dBdt);
	k1num   = -k1num;

	/* normalise filter to unity gain at centre frequency (needs Complex maths)
	 */

	var2.re = k1num; var2.im = 0.0;	var3.re = cos_theta; var3.im = -sin_theta;
	Mult_CmplxM(&var2,&var3,&var2); /* a1*exp(-sT) */
	var1.re = k0num; var1.im = 0.0;
	Add_CmplxM (&var1,&var2,&cf_num); /* a0+a1*exp(-sT) */
	var2.re = k1denom; var2.im = 0.0;
	Mult_CmplxM(&var2,&var3,&var2); /* b1*exp(-sT) */
	var2.re += 1.0; /* 1+b1*exp(-sT) */
	var3.re =  cos(2.0 * theta);
	var3.im = -sin(2.0 * theta);
	var1.re = k2denom; var1.im = 0.0;
	Mult_CmplxM(&var3,&var1,&var1); /* b2*exp(-2sT) */
	Add_CmplxM (&var1,&var2,&cf_denom); /* sigma(bi*exp(-isT) */

	if(!Div_CmplxM(&cf_denom,&cf_num,&Tf)){/* invert to get normalised values */
		NotifyError(wxT("%s: Filter failed to initialise."), funcName);
		return(NULL);
	}
	Convert_CmplxM(&Tf,&Tf); k1num *= Tf.re; k0num *= Tf.re;

	if ((p = (GammaToneCoeffs *) malloc(sizeof(GammaToneCoeffs))) == NULL) {
		NotifyError(wxT("%s: Out of memory!"), funcName);
		return(NULL);
    }
	stateVectorLength = cascade * FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER;
	if ((p->stateVector = (Float *) calloc(stateVectorLength,
	  sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for state vector."),
		  funcName);
		free(p);
		return(NULL);
	}
	p->a0 = k0num; p->a1 = k1num; p->b1 = k1denom; p->b2 = k2denom;
	p->cascade = cascade;
	return(p);

} /* InitGammaToneCoeffs_Filters */

/**************************** RecalculateGammaToneCoeffs *****************************/

/*-----------------------------------------------------------------*/
/*---- biquadratic filter table                                    */
/*
 * The function recalculates the coefficients for a GammaToneCoeffs structure.
 */

BOOLN
RecalculateGammaToneCoeffs_Filters(GammaToneCoeffsPtr p, Float centreFreq,
  Float bWidth3dB, Float sampleClk)
{
	static const WChar *funcName = wxT("InitGammaToneCoeffs_Filters");
	Float theta, bWidth3dBdt, sin_theta, cos_theta;
	Float k0num, k1num;	/* numerator coefficients */
	Float k1denom;	/* denominator coefficients */
	Complex cf_num, cf_denom, Tf, var1, var2, var3;

	bWidth3dBdt = (TwoPi * bWidth3dB) / sampleClk;
	theta = TwoPi * centreFreq/sampleClk;
	cos_theta = cos(theta);
	sin_theta = sin(theta);

	k1num   = exp( -bWidth3dBdt) * cos_theta; k0num = 1.;
	k1denom = -2. * k1num;
	k1num   = -k1num;

	/* normalise filter to unity gain at centre frequency (needs Complex maths)
	 */

	var2.re = k1num; var2.im = 0.0;	var3.re = cos_theta; var3.im = -sin_theta;
	Mult_CmplxM(&var2,&var3,&var2); /* a1*exp(-sT) */
	var1.re = k0num; var1.im = 0.0;
	Add_CmplxM (&var1,&var2,&cf_num); /* a0+a1*exp(-sT) */
	var2.re = k1denom; var2.im = 0.0;
	Mult_CmplxM(&var2,&var3,&var2); /* b1*exp(-sT) */
	var2.re += 1.0; /* 1+b1*exp(-sT) */
	var3.re =  cos(2.0 * theta);
	var3.im = -sin(2.0 * theta);
	var1.re = p->b2; var1.im = 0.0;
	Mult_CmplxM(&var3,&var1,&var1); /* b2*exp(-2sT) */
	Add_CmplxM (&var1,&var2,&cf_denom); /* sigma(bi*exp(-isT) */

	if(!Div_CmplxM(&cf_denom,&cf_num,&Tf)){/* invert to get normalised values */
		NotifyError(wxT("%s: Filter failed to initialise."), funcName);
		return(FALSE);
	}
	Convert_CmplxM(&Tf,&Tf); k1num *= Tf.re; k0num *= Tf.re;
	p->a0 = k0num; p->a1 = k1num; p->b1 = k1denom;;
	return(TRUE);

} /* RecalculateGammaToneCoeffs_Filters */

/**************************** FreeGammaToneCoeffs *****************************/

/*
 * This routine releases the memory allocated for the gamma tone coefficients.
 * A custom routine is required because memory is allocated dynamically for
 * the state vector, according to the filter cascade.
 */

void
FreeGammaToneCoeffs_Filters(GammaToneCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	free((*p)->stateVector);
	free(*p);
	*p = NULL;

}

/**************************** InitIIR2Coeffs **********************************/

/* f3dB is the corner frequency, fs sampling freq, coeffs points to an array of
	 5*Float for output, splane contains ascending cfts of the denom
	 polynom. */
 /*
  * Low pass filter, which can be either the ButterWorth or Bessel two pole
  * filters, depending on the splane coefficients.
  * The function returns a pointer to the  coefficients structure.
  * The variables a0,a1,a2,b1,b2 are used in the z plane transformation
  * (b0 = 1.).
  */

TwoPoleCoeffs *
InitIIR2Coeffs_Filters(Float *splane, int cascade, Float f3dB, Float fs,
  int low_or_high)
{
	static const WChar *funcName = wxT("InitIIR2Coeffs_Filters");
	Float A,A0,A1,A2,B0,B1,B2,C;
	Float a0,a1,a2,b1,b2;
	Float ai_tot, bi_tot, Tf;
    TwoPoleCoeffs	*p;

	if (cascade < 1) {
		NotifyError(wxT("%s: Illegal cascade (%d)."), funcName, cascade);
		return(NULL);
	}
	if (low_or_high == HIGHPASS){ /* reverse coefficient order out of splane */
		A0 = 0.; A1 = 0.; A2 = 1.;
		B2 = *splane; B1 = *(splane+1); B0 = *(splane+2); /* general 2 pole */
	}
	else { /* LOWPASS */
		A0 = 1.; A1 = 0.; A2 = 0.;
		B0 = *splane; B1 = *(splane+1); B2 = *(splane+2); /* general 2 pole */
	}
	if (DBL_GREATER(f3dB / fs, 0.5)) {
		NotifyError(wxT("%s: Maximum sampling for filter specification, dt = ")
		  wxT("%g ms."), funcName, MSEC(0.5 / f3dB));
		return(NULL);
	}
	C = 1.0 / tan(Pi * f3dB / fs); /* true bilinear transform eq 7-8, p172 */

	/* first biquadratic section */

	 /* DSP, William D.Stanley, Reston press, pp 174-175 */
	A  = (B0 + B1*C + (B2*C*C));
	a0 = (A0 + A1*C + A2*C*C)/A;
	a1 = 2.*(A0 - A2*C*C)/A;
	a2 = (A0 - A1*C + A2*C*C)/A;
	b1 = 2.*(B0 - B2*C*C)/A;
	b2 = (B0 - B1*C + B2*C*C)/A;
	if (low_or_high == HIGHPASS){/* now normalise coefficents at z=exp(s0T),
	                              * s0 = PI rads/sec */
		ai_tot = a0; ai_tot -= a1; ai_tot += a2; /* NB a1 and b1 change sign */
		bi_tot = 1.; bi_tot -= b1; bi_tot += b2;
		Tf = bi_tot/ai_tot; /* calculate normalisation factor */
	}
	else { /* LOWPASS */
		/* now normalise coefficents at z=exp(s0T), s0 = 0 rads/sec */
		ai_tot = a0; ai_tot += a1; ai_tot += a2;
		bi_tot = 1.; bi_tot += b1; bi_tot += b2;
		Tf = bi_tot/ai_tot; /* calculate normalisation factor */
	}
	a0 *= Tf; a1 *= Tf; a2 *= Tf; /* perform normalisation */

	if ((p = (TwoPoleCoeffs *) calloc(cascade, sizeof(TwoPoleCoeffs))) ==
	  NULL) {
		NotifyError(wxT("%s: Out of memory!"), funcName);
		return(NULL);
	}

	p->cascade = cascade;
	p->b1 = b1; p->b2 = b2;
	p->a0 = a0; p->a1 = a1; p->a2 = a2;
	p->gainLossFactor = *(splane+3);
	if ((p->state = (Float *) calloc(cascade * FILTERS_NUM_IIR2_STATE_VARS,
	  sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Out of memory!"), funcName);
		return(NULL);
	}
	return(p);

}

/**************************** FreeTwoPoleCoeffs *******************************/

/*
 * This routine releases the memory allocated for the two pole coefficients.
 * A custom routine is required because memory is allocated dynamically for
 * the state vector, according to the filter cascade.
 */

void
FreeTwoPoleCoeffs_Filters(TwoPoleCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	free((*p)->state);
	free(*p);
	*p = NULL;

}

/**************************** FreeIIR2ContCoeffs ******************************/

/*
 * This routine releases the memory allocated for the IIRCont coefficients.
 * A custom routine is required because memory is allocated dynamically for
 * the state vector, according to the filter cascade.
 */

void
FreeIIR2ContCoeffs_Filters(ContButtCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	free((*p)->state);
	free(*p);
	*p = NULL;

}

/**************************** IIR2 ********************************************/

/*--- 2 pole biquadratic filter code: coefficient order is d1,d2,n0,n1,n2 ---*/
/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 */

BOOLN
IIR2_Filters(SignalDataPtr theSignal, TwoPoleCoeffs *p[])
{
	static const WChar *funcName = wxT("IIR2_Filters");
	int		i, chan;
	ChanLen	j;
	register	Float 		temp0, temp1, *state, *state1;
	register	ChanData	*data;

	if (!theSignal->lengthFlag) {
		NotifyError(wxT("%s: Signal data length is not set!"), funcName);
		return(FALSE);
	}
	if (theSignal->numChannels == 0) {
		NotifyError(wxT("%s: No signal channels have been initialised!"),
		  funcName);
		exit(1);
	}
	for (chan = theSignal->offset; chan < theSignal->numChannels; chan++) {
		for (i = 0, state = p[chan]->state; i < p[chan]->cascade; i++, state +=
		  FILTERS_NUM_IIR2_STATE_VARS) {
			state1 = state + 1;
			data = theSignal->channel[chan];
			for (j = 0; j < theSignal->length; j++) {
				temp0  = *data - (p[chan]->b1 * *state);/* b1 term */
				temp0 -=         (p[chan]->b2 * *state1); /* b2 term */
				temp1  = temp0 * p[chan]->a0; /* a0 term */
				temp1 += (p[chan]->a1 * *state); /* a1 term */
				temp1 +=         (p[chan]->a2 * *state1); /* a2 term */
				*state1 = *state;
				*state = temp0;
				*(data++) = temp1 /* * p[chan]->gainLossFactor */;
			}
		}
	}
	return(TRUE);

}

/**************************** Compression *************************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 */

BOOLN
Compression_Filters(SignalDataPtr theSignal, Float nrwthr, Float nrwcr)
{
	static const WChar *funcName = wxT("Compression_Filters");
	int		chan;
	ChanLen	i;
	register	ChanData	*data;

	if (!theSignal->lengthFlag) {
		NotifyError(wxT("%s: Signal data length is not set!"), funcName);
		return(FALSE);
	}
	if (theSignal->numChannels == 0) {
		NotifyError(wxT("%s: No signal channels have been initialised!"),
		  funcName);
		return(FALSE);
	}
	for (chan = theSignal->offset; chan <theSignal->numChannels; chan++)
		for (i = 0, data = theSignal->channel[chan]; i < theSignal->length; i++,
		  data++)
			if(*data >= 0.0) { 			/* positive signal */
				if(*data > nrwthr)		/* linear up to threshold */
					*data = pow((*data - nrwthr), nrwcr) + nrwthr;
					/* compresses excess of signal above threshold */
			} else { 					/* negative signal */
				if(*data < -nrwthr) /* linear up to threshold */
					*data = -pow((-*data - nrwthr), nrwcr) - nrwthr;
			}
	return(TRUE);

}

/**************************** InversePowerCompression *************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * It uses Inverse Power compresion: y = sign(x) / (1 + shift * |x|^(-slope)).
 * It expects the signal to be correctly initialised.
 */

BOOLN
InversePowerCompression_Filters(SignalDataPtr theSignal, Float shift,
  Float slope)
{
	int		chan;
	ChanLen	i;
	register	ChanData	*data;

	for (chan = theSignal->offset; chan <theSignal->numChannels; chan++)
		for (i = 0, data = theSignal->channel[chan]; i < theSignal->length; i++,
		  data++)
			*data = (*data < 0.0)? -1.0 / (1.0 + shift * pow(-*data, -slope)):
			  1.0 / (1.0 + shift * pow(*data, -slope));
	return(TRUE);

}

/**************************** BrokenStick1Compression *************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * It uses Broken stick 1 compresion: y = ax if (a|x| <= b|x|^c), otherwise
 * y = sign(x) * b|x|^c.
 * It expects the signal to be correctly initialised.
 */

BOOLN
BrokenStick1Compression_Filters(SignalDataPtr theSignal, Float aA,
  Float bB, Float cC)
{
	register ChanData	*data;
	register Float		aTerm, bCTerm;
	int		chan;
	ChanLen	i;

	for (chan = theSignal->offset; chan <theSignal->numChannels; chan++)
		for (i = theSignal->length, data = theSignal->channel[chan]; i--;
		  data++) {
			if (*data < 0.0) {
		  		aTerm = aA * -*data;
				bCTerm = bB * pow(-*data, cC);
				*data = (aTerm < bCTerm)? -aTerm: -bCTerm;
			} else {
		  		aTerm = aA * *data;
				bCTerm = bB * pow(*data, cC);
				*data = (aTerm < bCTerm)? aTerm: bCTerm;
			}
		}
	return(TRUE);

}

/**************************** BrokenStick1Compression2 ************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * It uses Broken stick 1 compresion: y = ax if (a|x| <= b|x|^c), otherwise
 * y = sign(x) * b|x|^c.
 * This version uses arrays for the A and B parameters.
 * It expects the signal to be correctly initialised.
 */

BOOLN
BrokenStick1Compression2_Filters(SignalDataPtr theSignal, Float *aA,
  Float *bB, Float cC)
{
	register ChanData	*data, a, b, c;
	register Float		aTerm, bCTerm;
	int		chan;
	ChanLen	i;

	for (chan = theSignal->offset; chan <theSignal->numChannels; chan++) {
		a = aA[chan];
		b = bB[chan];
		c = cC;
		for (i = theSignal->length, data = theSignal->channel[chan]; i--;
		  data++) {
			if (*data < 0.0) {
		  		aTerm = a * -*data;
				bCTerm = b * pow(-*data, c);
				*data = (aTerm < bCTerm)? -aTerm: -bCTerm;
			} else {
		  		aTerm = a * *data;
				bCTerm = b * pow(*data, c);
				*data = (aTerm < bCTerm)? aTerm: bCTerm;
			}
		}
	}
	return(TRUE);

}

/**************************** UptonBStick1Compression *************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * It uses G Uptons (gupton@essex.ac.uk) broken stick 1 compresion:
 * y = sign(x) * (a|x|^b + c|x|^d).
 * This routine assumes that the signal has been correctly initialised.
 */

#define	FUNC(X)		(aA * pow((X), bB) + cC * pow((X), dD))

BOOLN
UptonBStick1Compression_Filters(SignalDataPtr theSignal, Float aA,
  Float bB, Float cC, Float dD)
{
	register ChanData	*data;
	int		chan;
	ChanLen	i;

	for (chan = theSignal->offset; chan <theSignal->numChannels; chan++)
		for (i = theSignal->length, data = theSignal->channel[chan]; i--;
		  data++) {
			*data = (*data < 0.0)? -FUNC(-*data): ((*data == 0.0)? 0.0:
			  FUNC(*data));
		}
	return(TRUE);

}

#undef FUNC

/**************************** GammaTone ***************************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * No checks are made here as to whether or not the gamma tone coefficients,
 * passed as an argument array, have been initialised.
 * Each channel has its own filter (coefficients).
 * It expects the signal to be correctly initialised.
 */

BOOLN
GammaTone_Filters(SignalDataPtr theSignal, GammaToneCoeffs *p[])
{
	int		j, chan;
	ChanLen	i;
	GammaToneCoeffs *gC;
	register	Float		*y_1, wn, *ptr1;	/* Inner loop variables */
	register	ChanData	*data;

	/* For the allocation of space to the state vector, the filter for all
	 * channels are assumed to have the same cascade as the first. */

	for (chan = theSignal->offset; chan < theSignal->numChannels; chan++) {
		gC = p[chan];
		for (j = gC->cascade, ptr1 = gC->stateVector; j--; ) {
			y_1 = ptr1++;
			for (i = theSignal->length, data = theSignal->channel[chan]; i--;
			   data++) {
				wn = *data - gC->b1 * *y_1 - gC->b2 * *ptr1;/* Temp. var. */
				*data = wn * gC->a0 + gC->a1 * *y_1; /* Final Yn */
				*ptr1 = *y_1;	/* Update Yn-1 to Yn-2 */
				*y_1 = wn;
			}
			ptr1++;
		}
	}
	return(TRUE);

}

/**************************** InitBandPassCoeffs ******************************/

/*
 * This function initialises the band pass filter coefficents, and returns
 * a pointer the the allocated structure.
 * Enrique Lopez-poveda: checking to ensure that (theta < PI/2):-
 * See Beauchamp and Yuen, Digital Methods for signal analysis, p. 257.
 * Note that according to the theory, tanTheta should always be positive
 * which means that the product (dt * frequencyDiff) must be < 0.5.
 * Therefore, once de is fixed, the maximum value of frequencyDiff is
 * fixed, or viceversa.
 */

BandPassCoeffsPtr
InitBandPassCoeffs_Filters(int cascade, Float lowerCutOffFreq,
  Float upperCutOffFreq, Float dt)
{
	static const WChar *funcName = wxT("InitBandPassCoeffs_Filters");
	Float	theta, tanTheta, frequencyDiff;
	BandPassCoeffsPtr	p;

	if (cascade < 1) {
		NotifyError(wxT("%s: Illegal cascade (%d)."), funcName, cascade);
		return(NULL);
	}
	if ((frequencyDiff = upperCutOffFreq - lowerCutOffFreq) < 0.001) {
		NotifyError(wxT("%s: Illegal frequency band width = %g Hz."), funcName,
		  frequencyDiff);
		return(NULL);
	}
	if ((p = (BandPassCoeffsPtr) malloc(sizeof(BandPassCoeffs))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for band pass filter ")
		  wxT("coefficients."), funcName);
		return(NULL);
	}

	if (DBL_GREATER(dt * frequencyDiff, 0.5)) {
		NotifyError(wxT("%s: Sampling interval, dt = %g ms is too low for ")
		  wxT("the\nfilter specification (largest, dt = %g ms."), funcName,
		  MSEC(dt), MSEC(0.5 / frequencyDiff));
		free(p);
		return(NULL);
	}
	p->cascade = cascade;
	theta = PI * dt * frequencyDiff;
	tanTheta = tan(theta);
	p->j = 1.0 / (1.0 + 1.0 / tanTheta);
	p->k = 2.0 * cos(PI * dt * (upperCutOffFreq + lowerCutOffFreq)) / ((1.0 +
	  tanTheta) * cos(theta));
	p->l = (tanTheta - 1.0) / (tanTheta + 1.0);
	p->gainLossFactor = 1.0 / p->j;
	if ((p->state = (Float *) calloc(cascade * FILTERS_NUM_BP_STATE_VARS,
	  sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Out of memory!"), funcName);
		free(p);
		return(NULL);
	}
	return(p);

}

/**************************** FreeBandPassCoeffs ******************************/

/*
 * This routine releases the memory allocated for the band pass coefficients.
 * A custom routine is required because memory is allocated dynamically for
 * the state vector, according to the filter cascade.
 */

void
FreeBandPassCoeffs_Filters(BandPassCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	free((*p)->state);
	free(*p);
	*p = NULL;

}

/**************************** BandPass ****************************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * The signal is a 1st order band-pass filter.  Yi = J*X[i] - J*X[i-2] +
 * K*Y[i-1] + L*Y[i-2] - where X and Y are the input and output values
 * respectively. See Beauchamp p.257 - 258.
 * No checks are made here as to whether or not the coefficients, passed as an
 * argument array, have been initialised.
 * The state variables are stored in the format X[i-1], X[i-2], Y[i-1], Y[i-2].
 * In continuous mode, there will not be a complete match with the output of the
 * full signal, while not in continuous mode.  This is because of the reverse
 * pass being slightly different for a different length signal.
 */

BOOLN
BandPass_Filters(SignalDataPtr theSignal, BandPassCoeffsPtr p[])
{
	static const WChar *funcName = wxT("BandPass_Filters");
	int		i, chan;
	ChanLen	j;
	register ChanData	*yi, xi, *xState, *yState, *state;

	if (!CheckPars_SignalData(theSignal)) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
		return(FALSE);
	}
	for (chan = theSignal->offset; chan < theSignal->numChannels; chan++) {
		for (i = 0, state = p[chan]->state; i < p[chan]->cascade; i++, state +=
		 FILTERS_NUM_BP_STATE_VARS ) {
			xState = state;
			yState = state + 2;
			yi = theSignal->channel[chan];
			for (j = 0; j < theSignal->length; j++, yi++) {
				xi = *yi;
				/* Start of formula evaluation */
				*yi = xi * p[chan]->j - p[chan]->j * *(xState + 1);
				*(xState + 1) = *xState;
				*xState = xi;
				*yi += p[chan]->k * *yState + p[chan]->l * *(yState + 1);
				*(yState + 1) = *yState;
				*yState = *yi;
			}
		}
	}
	return(FALSE);

}

/*************************** InitIIR2ContCoeffs *******************************/

/*
 * Second order continuous Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 * The cut-off frequency is the frequency at which the signal is 3dB down.
 * The coefficients for both low- and high-pass filters can be returned.
 * Enrique Lopez-Poveda suggested multiplying D and E by C to get the correct
 * behaviour.
 */

ContButtCoeffsPtr
InitIIR2ContCoeffs_Filters(int cascade, Float cutOffFrequency,
  Float samplingInterval, int highOrLowPass)
{
	static const WChar *funcName = wxT("InitIIR2ContCoeffs_Filters");
	int		i;
	Float	sqrt2, theta, cotOrTan, cotOrTanSquared, cC, dD, eE;
	ContButtCoeffsPtr	p;

	if (cascade < 1) {
		NotifyError(wxT("%s: Illegal cascade (%d)."), funcName, cascade);
		return(NULL);
	}
	sqrt2 = sqrt(2.0);
	if (DBL_GREATER(cutOffFrequency * samplingInterval, 0.5)) {
		NotifyError(wxT("%s: Maximum sampling for filter specification, dt = ")
		  wxT("%g ms."), funcName, MSEC(0.5 / cutOffFrequency));
		return(NULL);
	}
	theta = Pi * cutOffFrequency * samplingInterval;
	cotOrTan = (highOrLowPass == HIGHPASS)? tan(theta): 1.0 / tan(theta);
	cotOrTanSquared = cotOrTan * cotOrTan;

	cC = 1.0 / (1.0 + sqrt2 * cotOrTan + cotOrTanSquared);
	dD = cC * 2.0 * (1.0 - cotOrTanSquared);
	eE = cC * (1.0 - sqrt2 * cotOrTan + cotOrTanSquared);

	if ((p = (ContButtCoeffsPtr) malloc(sizeof(ContButtCoeffs))) == NULL) {
		NotifyError(wxT("%s: Out of memory."), funcName);
		return(NULL);
	}
	if ((p->state = (Float *) calloc(FILTERS_NUM_CONTBUTT2_STATE_VARS *
	  cascade, sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Could not allocate the state vector (%d)."),
		  funcName, FILTERS_NUM_CONTBUTT2_STATE_VARS * cascade);
		return(NULL);
	}
	p->cascade = cascade;
	p->cC = cC;
	p->eE = eE;
	if (highOrLowPass == HIGHPASS) {
		p->twoC = -2.0 * cC;
		p->dD = -dD;
	} else {
		p->twoC = 2.0 * cC;
		p->dD = dD;
	}
	for (i = 0; i < cascade * FILTERS_NUM_CONTBUTT2_STATE_VARS; i++)
		p->state[i] = 0.0;
	return(p);

}

/*************************** IIR2Cont *****************************************/

/*
 * Second order continuous Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 *
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * This is a infinite impulse response filter:
 *  Yi = C * X[i] + 2C * X[i-1] + C * X[i-2] - D * Y[i-1] - E * Y[i-2].
 * - where X and Y are the input and output values respectively.
 * No checks are made here as to whether or not the coefficients, passed as an
 * argument array, have been initialised.
 */

BOOLN
IIR2Cont_Filters(SignalDataPtr theSignal, ContButtCoeffsPtr pArray[])
{
	static const WChar *funcName = wxT("IIR2Cont_Filters");
	register Float		*xState, *yState;
	register ChanData	*yi, xi;				/* Inner loop variables */
	int		j, chan;
	ChanLen		i;
	ContButtCoeffsPtr	p;

	if (!CheckPars_SignalData(theSignal)) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
		return(FALSE);
	}
	for (chan = theSignal->offset; chan < theSignal->numChannels; chan++) {
		p = pArray[chan];
		for (j = 0; j < p->cascade; j++) {
			yi = theSignal->channel[chan];
			xState = p->state + j * FILTERS_NUM_CONTBUTT2_STATE_VARS;
			yState = xState + 2;
			for (i = 0; i < theSignal->length; i++, yi++) {
				xi = *yi;
				/* Start of formula evaluation */
				*yi = xi * p->cC + p->twoC * *xState + p->cC * *(xState + 1);
				*(xState + 1) = *xState;
				*xState = xi;
				*yi -= (p->dD * *yState + p->eE * *(yState + 1));
				*(yState + 1) = *yState;
				*yState = *yi;
			}
		}
	}
	return(TRUE);

}

/*************************** InitIIR1ContCoeffs *******************************/

/*
 * First order continuous Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 * The cut-off frequency is the frequency at which the signal is 3dB down.
 * The coefficients for both low- and high-pass filters can be returned.
 */

ContButt1CoeffsPtr
InitIIR1ContCoeffs_Filters(Float cutOffFrequency, Float samplingInterval,
 int highOrLowPass)
{
	static const WChar *funcName = wxT("InitIIR1ContCoeffs_Filters");
	int		i;
	Float	theta, cotOrTan;
	ContButt1CoeffsPtr	p;

	if (DBL_GREATER(cutOffFrequency * samplingInterval, 0.5)) {
		NotifyError(wxT("%s: Maximum sampling for filter specification, dt = ")
		  wxT("%g ms."), funcName, MSEC(0.5 / cutOffFrequency));
		return(NULL);
	}
	theta = Pi * cutOffFrequency * samplingInterval;
	cotOrTan = (highOrLowPass == HIGHPASS)? tan(theta): 1.0 / tan(theta);

	if ((p = (ContButt1CoeffsPtr) malloc(sizeof(ContButtCoeffs))) == NULL) {
		NotifyError(wxT("%s: Out of memory."), funcName);
		return(NULL);
	}
	p->gG = 1.0 / (1.0 + cotOrTan);
	p->gGG = (highOrLowPass == HIGHPASS)? -p->gG: p->gG;
	p->hH = (1.0 - cotOrTan) / (1.0 + cotOrTan);
	p->gainLossFactor = 1.0 / p->gG;
	for (i = 0; i < FILTERS_NUM_CONTBUTT1_STATE_VARS; i++)
		p->state[i] = 0.0;
	return(p);

}

/*************************** IIR1ContSingle ***********************************/

/*
 * First order continuous Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 *
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * This is a infinite impulse response filters:
 * No checks are made here as to whether or not the coefficients, passed as an
 * argument array, have been initialised.
 */

BOOLN
IIR1ContSingle_Filters(SignalDataPtr theSignal, ContButt1CoeffsPtr p)
{
	static const WChar *funcName = wxT("IIR1ContSingle_Filters");
	int		chan;
	ChanLen	i;
	register Float		*xi_1, *yi_1;	/* Inner loop variables */
	register ChanData	*yi, xi;	/*  wxT("		"	")		*/

	if (!CheckPars_SignalData(theSignal)) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
		return(FALSE);
	}
	for (chan = theSignal->offset; chan < theSignal->numChannels; chan++) {
		xi_1 = p->state;
		yi_1 = p->state + 1;
		yi = theSignal->channel[chan];
		for (i = 0; i < theSignal->length; i++) {
			xi = *yi;
			/* Start of formula evaluation */
			*yi = p->gG * xi + p->gGG * *xi_1 - p->hH * *yi_1;
			*xi_1 = xi;
			*yi_1 = *(yi++);
		}
	}
	return(TRUE);

}

/*************************** IIR1Cont ****************************************/

/*
 * First order continuous Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 *
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * This is a infinite impulse response filters:
 * No checks are made here as to whether or not the coefficients, passed as an
 * argument array, have been initialised.
 */

BOOLN
IIR1Cont_Filters(SignalDataPtr theSignal, ContButt1CoeffsPtr p[])
{
	static const WChar *funcName = wxT("IIR1Cont_Filters");
	int		chan;
	ChanLen	i;
	ContButt1CoeffsPtr	ptr;
	register Float		*xi_1, *yi_1;	/* Inner loop variables */
	register ChanData	*yi, xi;	/*  wxT("		"	")		*/

	if (!CheckPars_SignalData(theSignal)) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
		return(FALSE);
	}
	for (chan = theSignal->offset; chan < theSignal->numChannels; chan++) {
		if ((ptr = p[chan]) == NULL)
			continue;
		xi_1 = ptr->state;
		yi_1 = ptr->state + 1;
		yi = theSignal->channel[chan];
		for (i = theSignal->length; i--; ) {
			xi = *yi;
			/* Start of formula evaluation */
			*yi = ptr->gG * xi + ptr->gGG * *xi_1 - ptr->hH * *yi_1;
			*xi_1 = xi;
			*yi_1 = *(yi++);
		}
	}
	return(TRUE);

}

/********************************* BandPassFD_Filters *************************/

/*
 * This is a frequency domain butterworth filter.
 */

Float
BandPassFD_Filters(Float freq, Float lowerCutOffFreq, Float upperCutOffFreq,
  int order)
{
	Float	centralFreq, filterAmp;

	centralFreq = (upperCutOffFreq + lowerCutOffFreq) / 2.0;
	if (order <= 0)
		filterAmp = 1.0;
	else {
		filterAmp = sqrt ( 1 / (1 + pow((freq - centralFreq)/
		(upperCutOffFreq - centralFreq), 2.0 * order)));
	}
	return (filterAmp);

}

