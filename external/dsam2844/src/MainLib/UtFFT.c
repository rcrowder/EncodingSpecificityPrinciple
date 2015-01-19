/**********************
 *
 * File:		UtFFT.c
 * Purpose:		This module contains the fast fourier transform analysis
 *				routines.
 * Comments:	This was renamed from the old AnFourierT module.
 * Author:		L. P. O'Mard
 * Created:		17-07-00
 * Updated:
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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
#include "UtCmplxM.h"
#include "UtFFT.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** FFT...Complex *********************************/

/*
 * This routine carries out an FFT, returning the result to the same
 * array of complex data.
 * It uses the Danielson-Lanczos Lemma, and is based upon (but not copied
 * from) the Numerical Recipes algorithm.
 */

#define	SWAP(a, b)		tempVal = (a); (a) = (b); (b) = tempVal;

void
CalcComplex_FFT(Complex data[], unsigned long nn, int isign)
{
	unsigned long	mmax,m,j,istep,i;
	Float		wtemp,wr,wpr,wpi,wi,theta, tempVal;
	Complex		temp;

	j = 0;
	for (i = 0; i < nn; i++) {
		if (j > i) {
			SWAP(data[j].re, data[i].re);
			SWAP(data[j].im, data[i].im);
		}
		m = nn >> 1;
		while (m >= 2 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	for (mmax = 1; nn > mmax; mmax <<= 1) {
		istep = mmax << 1;
		theta = 3.141592653589793 / (isign * mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 0; m < mmax; m++) {
			for (i = m; i < nn; i += istep) {
				j = i + mmax;
				temp.re = wr * data[j].re - wi * data[j].im;
				temp.im = wr * data[j].im + wi * data[j].re;
				data[j].re = data[i].re - temp.re;
				data[j].im = data[i].im - temp.im;
				data[i].re += temp.re;
				data[i].im += temp.im;
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
	}

}

#undef SWAP

/****************************** FFT *******************************************/

/*
 * This routine carries out an FFT, returning the result to the same
 * data array.
 * It uses the Danielson-Lanczos Lemma, and is based upon (but not copied
 * from) the Numerical Recipes algorithm, i.e. it uses pointers.
 * The data is in the form *data = real, *(data + 1) = imaginary.
 */

#define SWAP(a, b)	tempr = (a); (a) = (b); (b) = tempr

void
Calc_FFT(Float *data, unsigned long nn, int isign)
{
	unsigned long	n, mmax, m, j, istep, i;
	Float	wtemp, wr, wpr, wpi, wi, theta;
	Float	tempr, tempi, *dm, *di, *dj;

	n = nn << 1;
	for (i = 0, j = 0; i < n; i += 2) {
		if (j > i) {
			SWAP(data[j], data[i]);
			SWAP(data[j + 1], data[i + 1]);
		}
		m = n >> 1;
		while (m >= 2 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	for (mmax = 2; n > mmax; mmax <<= 1) {
		istep = mmax << 1;
		theta = isign * (6.28318530717959 / mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr=1.0;
		wi=0.0;
		for (dm = data; dm < data + mmax; dm += 2) {
			for (di = dm; di < data + n; di += istep) {
				dj = di + mmax;
				tempr = wr * *dj - wi * *(dj + 1);
				tempi = wr * *(dj + 1) + wi * *dj;
				*dj = *di - tempr;
				*(dj + 1) = *(di + 1) - tempi;
				*di += tempr;
				*(di + 1) += tempi;
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
	}
}

#undef SWAP

/****************************** RealFT ****************************************/

/*
 * This function calculates the normalised Fourier transformation of a real data
 * set.
 * The '**ft' argument has been added so that the workspace can be assigned from
 * outside of the routine.  If it is set to NULL, then the workspace will be
 * assigned inside of the routine.
 * If it is assigned from outside of the routine, then the calling routine must
 * ensure that the correct size is used, and the the memory is deallocated after
 * use.
 * It returns FALSE if the routine fails in any way.
 * N.B. dfn = n / (dt * length * 2).
 */

#define	C1	0.5

BOOLN
CalcReal_FFT(SignalDataPtr signal, Float *fT, int direction)
{
	static const WChar *funcName = wxT("CalcReal_FFT");
	BOOLN	localFTArray = FALSE;
	ChanLen	i, k, n;
	Float	h1r, h1i, h2r, h2i;
	Float	wr, wi, wpr, wpi, wtemp, theta;
	register Float		c2, c3, *fT1, *fT2;
	register ChanData	*ptr;

	if (!CheckPars_SignalData(signal)) {
		NotifyError(wxT("%s: Signal not correctly set."), funcName);
		return(FALSE);
	}
	n = Length_FFT(signal->length);
	if (!fT) {
		localFTArray = TRUE;
		if ((fT = (Float *) calloc(n, sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Couldn't allocate memory for complex data ")
			  wxT("array."), funcName);
			return(FALSE);
		}
	}
	SetSamplingInterval_SignalData(signal, 1.0 / (signal->dt * n * 2.0));
	theta = M_PI / (Float) (n >> 1);
	for (k = 0; k < signal->numChannels; k++) {
		fT[0] = 0.0;
		ptr = signal->channel[k] + 1;
		for (i = 1; i < signal->length; i++)
			fT[i] = *ptr++;
		if (direction == FORWARD_FT) {
			c2 = -0.5;
			c3 = 1.0;
			Calc_FFT(fT, n >> 1, 1);
		} else {
			c2 = 0.5;
			theta = -theta;
			c3 = 2.0 / n;
		}
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0 + wpr;
		wi = wpi;
		for (fT1 = fT + 2, fT2 = fT + n - 2; fT1 < fT + (n >> 1);
		  fT1 += 2, fT2 -= 2) {
			h1r = C1 * (*fT1 + *fT2);
			h1i = C1 * (*(fT1 + 1) - *(fT2 + 1));
			h2r = -c2 * (*(fT1 + 1) + *(fT2 + 1));
			h2i = c2 * (*fT1 - *fT2);
			*fT1 = h1r + wr * h2r - wi * h2i;
			*(fT1 + 1) = h1i + wr * h2i + wi * h2r;
			*fT2 = h1r - wr * h2r + wi * h2i;
			*(fT2 + 1) = -h1i + wr * h2i + wi * h2r;
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		if (direction == FORWARD_FT) {
			*fT = (h1r = *fT) + *(fT + 1);
			*(fT + 1) = h1r - *(fT + 1);
		} else {
			*fT = C1 * ((h1r = *fT) + *(fT + 1));
			*(fT + 1) = C1 * (h1r - *(fT + 1));
			Calc_FFT(fT, n >> 1, -1);
		}
		ptr = signal->channel[k];
		for (fT1 = fT; fT1 < fT + signal->length; fT1++)
			*ptr++ = *fT1 * c3;
	}
	if (localFTArray)
		free(fT);
	return(TRUE);

}

#undef C1

/****************************** Length ****************************************/

/*
 * This function returns the nearest length which is a power of two for use
 * with the FFT algorithm.
 */

unsigned long
Length_FFT(unsigned long length)
{
	unsigned long	ln;

	ln = (unsigned long) ceil(log(length) / log(2.0));
	return((unsigned long) pow(2.0, (Float) ln));

}

#if HAVE_FFTW3

/****************************** InitArray *************************************/

/*
 * Initialise an FFTArray structure.
 */

FFTArrayPtr
InitArray_FFT(unsigned long dataLen, BOOLN forInPlaceFFT, int numPlans)
{
	static const WChar *funcName = wxT("InitArray_FFT");
	FFTArrayPtr	p;

	if (dataLen < 2) {
		NotifyError(wxT("%s: The data length must be greater than 2 (%d)."),
		  funcName, dataLen);
		return(NULL);
	}
	if ((p = (FFTArrayPtr) malloc(sizeof(FFTArray))) == NULL) {
		NotifyError(wxT("%s: Out of memory for structure."), funcName);
		return(NULL);
	}
	if (numPlans < 1)
		p->plan = NULL;
	else {
		if ((p->plan = (Fftw_plan *) calloc(numPlans, sizeof(Fftw_plan))) == NULL) {
			NotifyError(wxT("%s: output of memory for %d fftw plans."), funcName,
			  numPlans);
			free (p);
			return(NULL);
		}
		p->numPlans = numPlans;
	}
	p->fftLen = Length_FFT(dataLen);
	p->arrayLen = (forInPlaceFFT)? (p->fftLen << 1) + 2: p->fftLen;
	if ((p->data = (Float *) DSAM_FFTW_NAME(malloc)(p->arrayLen * sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: output of memory for physical array length: %lu."),
		  funcName, p->arrayLen);
		free (p);
		return(NULL);
	}
	p->dataLen = dataLen;
	return(p);

}

/****************************** FreeArray *************************************/

void
FreeArray_FFT(FFTArrayPtr *p)
{
	int		i;

	if (!*p)
		return;
	if ((*p)->data)
		DSAM_FFTW_NAME(free)((*p)->data);
	if ((*p)->plan) {
		for (i = 0; i < (*p)->numPlans; i++)
			DSAM_FFTW_NAME(destroy_plan)((*p)->plan[i]);
		free((*p)->plan);
	}
	free(*p);
	*p = NULL;

}

#endif /* HAVE_FFTW3 */

/****************************** CreateNoiseBand ********************************/

/*
 * Routine to create a band of noise given the half upper frequency band in
 * k-space.
 * It expects the FFTW plan to have been correctly set up for a reverse FFT.
 * A value of kLow = 1 is required for no DC.
 */

void
CreateNoiseBand_FFT(FFTArrayPtr fTInv, int plan, RandParsPtr randPars,
  int kLow, int kUpp)
{
	int		k;
	ChanLen	j;
	Float	phase;
	Complx	*c1;

	c1 = (Complx *) fTInv->data;
	for (k = 0; k < kLow; k++)
		CMPLX_RE(c1[k]) = CMPLX_IM(c1[k]) = 0.0;
	for (k = kLow; k < (kUpp + 1); k++) {
		phase = Ran01_Random(randPars) * PIx2;
		CMPLX_RE(c1[k]) = cos(phase);
		CMPLX_IM(c1[k]) = sin(phase);
	}
	for (j = kUpp + 1; j < fTInv->fftLen / 2 + 1; j++)
		CMPLX_RE(c1[j]) = CMPLX_IM(c1[j]) = 0.0;
	DSAM_FFTW_NAME(execute)(fTInv->plan[plan]);

}
