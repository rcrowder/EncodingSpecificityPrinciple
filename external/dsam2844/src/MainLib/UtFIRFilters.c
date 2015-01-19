/**********************
 *
 * File:		UtFIRFilter.c
 * Purpose:		This contains the structures for the FIR Filter.
 * Comments:	This module is to be put into UtFilters.
 * Author:		L. P. O'Mard.
 * Created:		04 Dec 2000
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
#include "FiParFile.h"
#include "UtRemez.h"
#include "UtFIRFilters.h"
#include "FlFIR.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/**************************** InitFIRCoeffs ***********************************/

/*
 * This function initialises the FIR filter coefficents, and returns
 * a pointer the the allocated structure.
 * When the 'type' is -BANDPASS, then 'numBands' is sent to the 'remez' function
 * as a negative number, to instruct 'remez' to use the arbitrary response
 * algorithm revisions.
 */

FIRCoeffsPtr
InitFIRCoeffs_FIRFilters(int numChannels, int numTaps, int numBands,
  Float *bands, Float *desired, Float *weights, int type)
{
	static const WChar *funcName = wxT("InitFIRCoeffs_FIRFilters");
	int		i;
	Float	*scaledFreq, nyquestFreq;
	FIRCoeffsPtr	p;

	if (numTaps < 1) {
		NotifyError(wxT("%s: Illegal number of coefficients (%d)."), funcName,
		  numTaps);
		return(NULL);
	}
	if (numBands < 3) {
		NotifyError(wxT("%s: Illegal number of coefficients (%d)."), funcName,
		  numBands);
		return(NULL);
	}
	if (numChannels < 1) {
		NotifyError(wxT("%s: Illegal number of channels (%d)."), funcName,
		  numChannels);
		return(NULL);
	}
	if ((p = (FIRCoeffsPtr) malloc(sizeof(FIRCoeffs))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for FIR filter ")
		  wxT("coefficients."), funcName);
		return(NULL);
	}
	p->numChannels = numChannels;
	if ((p->c = (Float *) calloc(numTaps, sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Out of memory for coefficients!"), funcName);
		FreeFIRCoeffs_FIRFilters(&p);
		return(NULL);
	}
	if ((p->state = (Float *) calloc(numTaps * numChannels, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Out of memory state variables!"), funcName);
		FreeFIRCoeffs_FIRFilters(&p);
		return(NULL);
	}
	if (type < 0) {		/* Straight user coeffs. type */
		for (i = 0; i < numTaps; i++)
			p->c[i] = desired[i];
		p->m = numTaps;
	} else {
		if ((scaledFreq = (Float *) calloc(numTaps, sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for frequency scale."),
			  funcName);
			FreeFIRCoeffs_FIRFilters(&p);
			return(NULL);
		}
		nyquestFreq = bands[numBands - 1] * 2.0;
		for (i = 0; i < numBands; i++) {
			scaledFreq[i] = bands[i] / nyquestFreq;
			/*printf(wxT("freq[%3d] = %g\n"), i, scaledFreq[i]);*/
		}
		if (type == -BANDPASS) {
			type = -type;
			numBands = -numBands;
		}
		remez(p->c, numTaps, numBands, scaledFreq, desired, weights, type);
		p->m = numTaps;
		free(scaledFreq);
	}
	return(p);

}

/**************************** FreeFIRCoeffs ***********************************/

/*
 * This routine releases the memory allocated for the FIR coefficients.
 * A custom routine is required because memory is allocated dynamically for
 * the state vector, according to the filter cascade.
 */

void
FreeFIRCoeffs_FIRFilters(FIRCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	if ((*p)->c)
		free((*p)->c);
	if ((*p)->state)
		free((*p)->state);
	free(*p);
	*p = NULL;

}

/**************************** ProcessBuffer ***********************************/

/*
 * This routine pushes the signal data into the state buffer, allowing the old
 * data to 'fall off the end.'
 * It assumes that the EarObjectPtr 'data' has already been checked.
 * A buffer is used to host the previous 'x' values.  The buffer data is stored
 * in the buffer using, "First in First OUt" (FIFO), operation mode.
 * The start of the buffer is 'left/0' and the end of the buffer is
 * 'right/m'.
 */

void
ProcessBuffer_FIRFilters(EarObjectPtr data, FIRCoeffsPtr p)
{
	int		chan;
	ChanLen	i, stateSampleLen;
	ChanData	*stateBuffer;
	register ChanData	*xi, *s, *s2;

	if ((ChanLen) p->m < _OutSig_EarObject(data)->length)		/* Shift unnessary */
		stateSampleLen = p->m;
	else {
		stateSampleLen = _OutSig_EarObject(data)->length;
		for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->
		  numChannels; chan++) {
			stateBuffer =  p->state + p->m * chan;
			s = stateBuffer + p->m - 1;
			s2 = s - stateSampleLen;
			for (i = stateSampleLen; i < (ChanLen) p->m; i++)
				*s-- = *s2--;
		}
	}
	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->numChannels;
	  chan++) {
		s = p->state + p->m * chan;
		xi = _InSig_EarObject(data, 0)->channel[chan] + stateSampleLen - 1;
		for (i = 0; i < stateSampleLen; i++)
			*s++ = *xi--;
	}

}

/**************************** FIR *********************************************/

/*
 * FIR filter application.
 *
 *         M
 * y(n) = SUM c(k).x(n-k)
 *        k=0
 *
 * It assumes that the EarObjectPtr 'data' has already been checked.
 */

void
FIR_FIRFilters(EarObjectPtr data, FIRCoeffsPtr p)
{
	/*static const WChar *funcName = wxT("FIR_FIRFilters");*/
	int		chan;
	ChanLen	i, j;
	register ChanData	*yi, *xi, *xi2, *state, *c, *xStart, summ;

	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->numChannels;
	  chan++) {
		xi = xStart = _InSig_EarObject(data, 0)->channel[chan];
		yi = _OutSig_EarObject(data)->channel[chan];
		for (i = _OutSig_EarObject(data)->length; i ; i--, xi++, yi++) {
			c = p->c;
			for (j = p->m, xi2 = xi, summ = 0.0; j && (xi2 >= xStart); j--)
				summ += *c++ * *xi2--;
			state = p->state + p->m * chan;
			while (j) {
				summ += *c++ * *state++;
				j--;
			}
			*yi = summ;
		}
	}
	ProcessBuffer_FIRFilters(data, p);

}

