/**********************
 *
 * File:		UtANSGUtils.h
 * Purpose:		This file contains the auditory nerve spike distribution
 * 				management and other utility routines.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jun 2009
 * Updated:
 * Copyright:	(c) 2010 Lowel P. O'Mard
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
#include "UtParArray.h"
#include "UtANSGUtils.h"

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/****************************** OutputModeList ********************************/

/*
 * This function defines the 'outputMode' list array
 */

NameSpecifier *
OutputModeList_ANSGUtils(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("SQUARE_PULSE"),	ANSGUTILS_DISTRIBUTION_OUTPUTMODE_SQUARE_PULSE },
			{ wxT("ALPHA_WAVE"),	ANSGUTILS_DISTRIBUTION_OUTPUTMODE_ALPHA_WAVE },
			{ 0,					ANSGUTILS_DISTRIBUTION_OUTPUTMODE_NULL }
		};
	return(&modeList[index]);

}

/****************************** ModeList **************************************/

/*
 * This routine returns a name specifier for the distribution mode list.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
ModeList_ANSGUtils(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("DBL_GAUSSIAN"),	ANSGUTILS_DISTRIBUTION_DBL_GAUSSIAN_MODE},
					{ wxT("GAUSSIAN"),		ANSGUTILS_DISTRIBUTION_GAUSSIAN_MODE},
					{ wxT("STANDARD"),		ANSGUTILS_DISTRIBUTION_STANDARD_MODE},
					{ wxT("USER"),			ANSGUTILS_DISTRIBUTION_USER_MODE},
					{ 0,					ANSGUTILS_DISTRIBUTION_NULL},

				};
	return (&modeList[index]);

}

/****************************** GetNumDistributionPars ****************************/

/*
 * This function returns the number of parameters for the respective distribution
 * parameter array structure. Using it helps maintain the correspondence
 * between the mode names.
 */

int
GetNumDistributionPars_ANSGUtils(int mode)
{
	static const WChar	*funcName = wxT("GetNumDistributionPars_ANSGUtils");

	switch (mode) {
	case ANSGUTILS_DISTRIBUTION_DBL_GAUSSIAN_MODE:
		return(4);
	case ANSGUTILS_DISTRIBUTION_GAUSSIAN_MODE:
		return(3);
	case ANSGUTILS_DISTRIBUTION_STANDARD_MODE:
		return(1);
	case ANSGUTILS_DISTRIBUTION_USER_MODE:
		return(0);
	default:
		NotifyError(wxT("%s: Mode not listed (%d), returning zero."), funcName,
		  mode);
		return(0);
	}
}

/****************************** FreeFibres *******************************/

/*
 * This routine deallocates the memory for the ANSGDist structure.
 */

void
Free_ANSGUtils(ANSGDistPtr *p)
{
	if (!(*p))
		return;
	if ((*p)->numFibres)
		free((*p)->numFibres);
	free(*p);
	*p = NULL;

}

/****************************** Init ************************************/

/*
 * This function initialises the AN distribution structure.
 * It expects an uninitialised pointer to be set to NULL;
 * It returns FALSE if it fails in any way.
 */

BOOLN
Init_ANSGUtils(ANSGDistPtr *p, int numChannels)
{
	static const WChar	*funcName = wxT("Init_ANSGUtils");

	if (!*p) {
		if (((*p) = (ANSGDistPtr) malloc(sizeof(ANSGDist))) == NULL) {
			NotifyError(wxT("%s: Could not allocate AN distribution."), funcName);
			return(FALSE);
		}
		(*p)->numFibres = NULL;
	}
	if ((*p)->numFibres && ((*p)->numChannels != numChannels)) {
		free((*p)->numFibres);
		(*p)->numFibres = NULL;
	}
	if (!(*p)->numFibres && ((*p)->numFibres = (int *) calloc(numChannels,
	  sizeof(int))) == NULL) {
		NotifyError(wxT("%s: Could not allocate memory for (%d) fibre array"),
		  funcName, numChannels);
		return(FALSE);
	}
	(*p)->numChannels = numChannels;
	return(TRUE);
}

/****************************** SetDistribution ************************/

/*
 * This routine sets the default values for the distribution array parameters.
 */

void
SetDefaultDistribution_ANSGUtils(ParArrayPtr distribution)
{
	SetMode_ParArray(distribution, wxT("standard"));
	distribution->params[0] = 5;

}

/****************************** SetStandardNumFibres ********************/

/*
 * This routine sets the standard values for the obsolete numFibres parameter.
 * It will be removed when it is no longer used.
 */

void
SetStandardNumFibres_ANSGUtils(ParArrayPtr distribution, int numFibres)
{
	SetMode_ParArray(distribution, wxT("standard"));
	distribution->params[0] = numFibres;

}

/****************************** CheckFuncPars *******************************/

/*
 * This function checks the the function parameters are valid.
 * It returns FALSE if it fails in any way.
 */

BOOLN
CheckFuncPars_ANSGUtils(ParArrayPtr p, SignalDataPtr signal)
{
	static const WChar	*funcName = wxT("CheckFuncPars_ANSGUtils");
	BOOLN	ok = TRUE;

	switch (p->mode) {
	case ANSGUTILS_DISTRIBUTION_STANDARD_MODE:
		break;
	case ANSGUTILS_DISTRIBUTION_DBL_GAUSSIAN_MODE:
	case ANSGUTILS_DISTRIBUTION_GAUSSIAN_MODE:
		if (p->params[ANSGUTILS_GAUSS_VAR1] == 0.0) {
			NotifyError(wxT("%s: Variance1 (parameter %g) must be greater than ")
			  wxT("zero (%g)"), funcName, ANSGUTILS_GAUSS_VAR1,
			  p->params[ANSGUTILS_GAUSS_VAR1]);
			ok = FALSE;
		}
		if ((p->mode == ANSGUTILS_DISTRIBUTION_DBL_GAUSSIAN_MODE) &&
		  (p->params[ANSGUTILS_GAUSS_VAR2] == 0.0)) {
			NotifyError(wxT("%s: Variance2 (parameter %d) must be greater than ")
			  wxT("zero (%g)"), funcName, ANSGUTILS_GAUSS_VAR2,
			  p->params[ANSGUTILS_GAUSS_VAR2]);
			ok = FALSE;
		}
		break;
	case ANSGUTILS_DISTRIBUTION_USER_MODE:
		break;
	default:
		NotifyError(wxT("%s: Mode (%d) not listed, returning zero."), funcName,
		  p->mode);
		ok = FALSE;
	}
	return(ok);

}

/****************************** GetFitFuncValue *******************************/

/*
 * This function returns the value for the respective distribution function mode.
 * Using it helps maintain the correspondence between the mode names.
 */

int
GetDistFuncValue_ANSGUtils(ParArrayPtr p, int numChannels, int chan)
{
	static const WChar	*funcName = wxT("GetDistFuncValue_ANSGUtils");
	int		meanChan;

	switch (p->mode) {
	case ANSGUTILS_DISTRIBUTION_DBL_GAUSSIAN_MODE:
		meanChan = (int) ((p->params[ANSGUTILS_GAUSS_MEAN] < 0)? numChannels / 2:
		p->params[ANSGUTILS_GAUSS_MEAN]);
		return((int) floor(p->params[ANSGUTILS_GAUSS_NUM_FIBRES] *
		  (ANSGUTILS_GAUSSIAN(p->params[ANSGUTILS_GAUSS_VAR1], chan, meanChan) +
		  ANSGUTILS_GAUSSIAN(p->params[ANSGUTILS_GAUSS_VAR2], chan, meanChan)) /
		  2.0 + 0.5));
	case ANSGUTILS_DISTRIBUTION_GAUSSIAN_MODE:
		meanChan = (int) ((p->params[ANSGUTILS_GAUSS_MEAN] < 0)? numChannels / 2:
		  p->params[ANSGUTILS_GAUSS_MEAN]);
		return((int) floor(p->params[ANSGUTILS_GAUSS_NUM_FIBRES] * ANSGUTILS_GAUSSIAN(
		  p->params[ANSGUTILS_GAUSS_VAR1], chan, meanChan) + 0.5));
	case ANSGUTILS_DISTRIBUTION_STANDARD_MODE:
		return((int) p->params[0]);
	default:
		NotifyError(wxT("%s: Mode (%d) not listed, returning zero."), funcName,
		  p->mode);
	}
	return(0);

}

/****************************** GetMeanChan *****************************/

/*
 * This routine returns the required centre channel from the specified
 * frequency and frequency list.
 * It expects the frequencies array to be correctly initialised.
 */

int
GetMeanChan_ANGSDist(Float *frequencies, int numFreqs, Float bF)
{
	int		i, bFIndex = -1;
	Float	minDiff, diff;

	if (bF < 0.0)
		return(numFreqs / 2);
	for (i = 0, minDiff = DBL_MAX; i < numFreqs; i++)
		if ((diff = fabs(frequencies[i] - bF)) < minDiff) {
			minDiff = diff;
			bFIndex = i;
		}
	return(bFIndex);
}

/****************************** SetFibres *******************************/

/*
 * This function returns the value for the respective distribution function mode.
 * Using it helps maintain the correspondence between the mode names.
 * It expects the frequencies array to be correctly initialised.
 * It expects the "fibreDist" fibre distribution structure to be correctly initialised.
 */

BOOLN
SetFibres_ANSGUtils(ANSGDistPtr *aNDist, ParArrayPtr p, Float *frequencies,
  int numChannels)
{
	static const WChar	*funcName = wxT("SetFibres_ANSGUtils");
	BOOLN	ok = TRUE;
	int		i, meanChan;

	if (!Init_ANSGUtils(aNDist, numChannels)) {
		NotifyError(wxT("%s: Could not allocate AN distribution."), funcName);
		return(FALSE);
	}
	switch (p->mode) {
	case ANSGUTILS_DISTRIBUTION_DBL_GAUSSIAN_MODE:
		meanChan = GetMeanChan_ANGSDist(frequencies, numChannels,
		  p->params[ANSGUTILS_GAUSS_MEAN]);
		for (i = 0; i < numChannels; i++)
			(*aNDist)->numFibres[i] = (int) floor(p->params[ANSGUTILS_GAUSS_NUM_FIBRES] *
			  (ANSGUTILS_GAUSSIAN(p->params[ANSGUTILS_GAUSS_VAR1], i, meanChan) +
			  ANSGUTILS_GAUSSIAN(p->params[ANSGUTILS_GAUSS_VAR2], i, meanChan)) /
			  2.0 + 0.5);
		break;
	case ANSGUTILS_DISTRIBUTION_GAUSSIAN_MODE:
		meanChan = GetMeanChan_ANGSDist(frequencies, numChannels,
		  p->params[ANSGUTILS_GAUSS_MEAN]);
		for (i = 0; i < numChannels; i++)
			(*aNDist)->numFibres[i] = (int) floor(p->params[ANSGUTILS_GAUSS_NUM_FIBRES] *
			  ANSGUTILS_GAUSSIAN(p->params[ANSGUTILS_GAUSS_VAR1], i,
			  meanChan) + 0.5);
		break;
	case ANSGUTILS_DISTRIBUTION_STANDARD_MODE:
		for (i = 0; i < numChannels; i++)
			(*aNDist)->numFibres[i] = (int) p->params[0];
		break;
	case ANSGUTILS_DISTRIBUTION_USER_MODE:
		if (p->varNumParams < numChannels) {
			NotifyError(wxT("%s: The are %d channels, but only %d have settings."),
			  funcName, numChannels, p->varNumParams);
			return(FALSE);
		}
		for (i = 0; i < numChannels; i++)
			(*aNDist)->numFibres[i] = (int) p->params[i];
		break;
	default:
		NotifyError(wxT("%s: Mode (%d) not listed, returning zero."), funcName,
		  p->mode);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintFibres *******************************/

/*
 * This prints the fibre information if available.
 * It expects the "fibres" array to be correctly initialised.
 */

void
PrintFibres_ANSGUtils(FILE *fp, const WChar *prefix, int *fibres,
  Float *frequencies, int numChannels)
{
	int		i;

	if (!fibres || !frequencies)
		return;
	DSAM_fprintf(fp, wxT("%SChannel fibre distribution list:-\n"), prefix);
	DSAM_fprintf(fp, wxT("%S\t%10S\t%10S\t%4S\n"), prefix, wxT("Chan. Index"),
	  wxT("Freq. kHz"), wxT("No. Fibres"));
	for (i = 0; i < numChannels; i++)
		DSAM_fprintf(fp, wxT("%S\t%10d\t%10g\t%4d\n"), prefix, i, frequencies[i],
		  fibres[i]);

}

/****************************** CalcPulseDuration ************************/

/*
 * This function generates the AN pulse duration.
 */

Float
CalcPulseDuration_ANSGUtils(int outputMode, Float durationCoeff)
{
	return(((outputMode == ANSGUTILS_DISTRIBUTION_OUTPUTMODE_ALPHA_WAVE)?
	  ANSGUTILS_DISTRIBUTION_ALPHA_WAVE_LIMIT_COEFF * durationCoeff:
	  durationCoeff));

}

/****************************** CalcPulseDurationIndex ********************/

/*
 * This function generates the AN pulse.
 */

ChanLen
CalcPulseDurationIndex_ANSGUtils(int outputMode, Float durationCoeff, Float dt)
{

	return((ChanLen) floor(CalcPulseDuration_ANSGUtils(outputMode,
	  durationCoeff) / dt + 0.5));

}

/****************************** GeneratePulse *******************************/

/*
 * This function generates the AN pulse.
 */

ChanData *
GeneratePulse_ANSGUtils(int outputMode, int durationIndex, Float timeToPeak,
  Float magnitude, Float dt)
{
	static const WChar *funcName = wxT("GeneratePulse_ANSGUtils");
	int		i;
	ChanData	*pulse, *pPtr;

	if ((pulse = (ChanData *) calloc(durationIndex, sizeof(ChanData))) == NULL) {
		NotifyError(wxT("%s: Out of memory for alphaWave array."),
		  funcName);
		return(NULL);
	}
	pPtr = pulse;
	if (outputMode == ANSGUTILS_DISTRIBUTION_OUTPUTMODE_ALPHA_WAVE)
		for (i = 0; i < durationIndex; i++)
			*pPtr++ = magnitude * i * dt / timeToPeak * exp(1.0 - i * dt /
			  timeToPeak);
	else
		for (i = 0; i < durationIndex; i++)
			*pPtr++ = magnitude;

	return(pulse);

}

/****************************** AddRemainingPulse *******************************/

/*
 * This routine adds any remaining pulse required.
 */

void
AddRemainingPulse_ANSGUtils(ChanData *outPtr, Float *pulse, ChanLen *pulseIndexPtr,
  ChanLen pulseDurationIndex)
{
	ChanData	*pulsePtr;
	ChanLen		i;

	pulsePtr = pulse + *pulseIndexPtr;
	for (i = *pulseIndexPtr; i < pulseDurationIndex; i++)
		*outPtr++ += *pulsePtr++;
	*pulseIndexPtr = 0;

}
