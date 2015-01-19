/**********************
 *
 * File:		UtCFList.c
 * Purpose:		This file contains the centre frequency management routines.
 * Comments:	The "bandWidth" field is set using the UtBandWidth module.
 *				06-10-98 LPO: The CFList type is now saved as the specifier,
 *				rather than as a string.  The 'minCf', 'maxCF' and 'eRBDensity'
 *				values are now saved with the CFList, so that the parameter list
 *				can always know these values.
 *				12-10-98 LPO: Introduced the 'SetParentCFList_CFList' and the
 *				'parentPtr' so that a CFList can always set the pointer which is
 *				pointing to it.
 *				19-11-98 LPO: Changed code so that a NULL array for the user
 *				modes causes the frequency and bandwidth arrays to have space
 *				allocated for them.  This was needed for the universal
 *				parameters implementation.
 *				26-11-98 LPO: Added the 'oldNumChannels' private parameter so
 *				that the user mode can use the parameters of the old frequency
 *				array if it exists.
 *				17-06-99 LPO: The 'SetIndividualFreq' routine now sets the
 *				CFList's 'updateFlag' field when a frequency is set.  This is
 *				needed because modules, such as 'BM_Carney' need to know if the
 *				bandwidths need recalculating.
 *				06-11-00 LPO: Implented the Greenwood function with the
 *				functions and values provided by Steve D. Holmes.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		17 Jun 1999
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeUniParMgr.h"
#include "FiParFile.h"

#include "UtBandwidth.h"
#include "UtCFList.h"

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

static NameSpecifier	cFListDiagModeList[] = {

						{ wxT("LIST"), 			CFLIST_DIAG_MODE },
						{ wxT("PARAMETERS"),	CFLIST_PARAMETERS_DIAG_MODE },
						{ NULL,					CFLIST_DIAG_NULL }
					};

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/****************************** Init ******************************************/

/*
 * This function allocates memory for the centre frequency data structure.
 * The function returns a pointer to the prepared structure.
 */

CFListPtr
Init_CFList(const WChar *callingFunctionName)
{
	static const WChar *funcName = wxT("Init_CFList");
	CFListPtr theCFs;

	if ((theCFs = (CFListPtr) malloc(sizeof(CFList))) == NULL ) {
		NotifyError(wxT("%s: Out of Memory (called by %s)."), funcName,
		  callingFunctionName);
		return(NULL);
	}
	theCFs->minCFFlag = FALSE;
	theCFs->maxCFFlag = FALSE;
	theCFs->updateFlag = TRUE;
	theCFs->numChannels = 0;
	theCFs->minCF = 0.0;
	theCFs->maxCF = 0.0;
	theCFs->focalCF = -1.0;
	theCFs->eRBDensity = 0.0;
	theCFs->frequency = NULL;
	theCFs->bandwidth = NULL;
	theCFs->bandwidthMode.specifier = BANDWIDTH_NULL;
	theCFs->diagnosticMode = CFLIST_DIAG_NULL;
	theCFs->centreFreqMode = CFLIST_NULL;

	theCFs->cFParList = NULL;
	theCFs->bParList = NULL;
	theCFs->oldNumChannels = 0;
	return(theCFs);

}

/****************************** Free ******************************************/

/*
 * This function de-allocates memory for the centre frequency data structure.
 */

void
Free_CFList(CFListPtr *theCFs)
{
	if (*theCFs == NULL)
		return;
	if ((*theCFs)->frequency != NULL)
		free((*theCFs)->frequency);
	if ((*theCFs)->bandwidth != NULL)
		free((*theCFs)->bandwidth);
	FreeList_UniParMgr(&(*theCFs)->cFParList);
	FreeList_UniParMgr(&(*theCFs)->bParList);
	free((*theCFs));
	*theCFs = NULL;

}

/********************************* CFModeList *********************************/

/*
 * This function returns a CF mode list entry.
 */

NameSpecifier *
CFModeList_CFList(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("SINGLE"), 		CFLIST_SINGLE_MODE },
			{ wxT("USER"), 			CFLIST_USER_MODE },
			{ wxT("ERB"), 			CFLIST_ERB_MODE },
			{ wxT("ERB_N"), 		CFLIST_ERBN_MODE },
			{ wxT("LOG"), 			CFLIST_LOG_MODE },
			{ wxT("FOCAL_LOG"),		CFLIST_FOCAL_LOG_MODE },
			{ wxT("LINEAR"),		CFLIST_LINEAR_MODE },
			{ wxT("CAT"),			CFLIST_CAT_MODE },
			{ wxT("CHINCHILLA"),	CFLIST_CHINCHILLA_MODE },
			{ wxT("GUINEA_PIG"),	CFLIST_GPIG_MODE },
			{ wxT("HUMAN"),			CFLIST_HUMAN_MODE },
			{ wxT("MACAQUE"),		CFLIST_MACAQUEM_MODE },
			{ wxT("IDENTICAL"),		CFLIST_IDENTICAL_MODE },
			{ 0,					CFLIST_NULL }
		};
	return (&modeList[index]);

}

/********************************* RegenerateList *****************************/

/*
 * This routine regenerates a list from an existing CFList structure with the
 * appropriate parameters and arrays set for the current parameters.
 * It returns FALSE if it fails in any way.
 */

BOOLN
RegenerateList_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("RegenerateList_CFList");
	int		i, numChannels;
	Float	*frequencies, *bandwidths;

	if (((theCFs->centreFreqMode == CFLIST_USER_MODE) ||
	  (theCFs->centreFreqMode == CFLIST_SINGLE_MODE) ||
	  (theCFs->centreFreqMode == CFLIST_IDENTICAL_MODE)) &&
	  (theCFs->oldNumChannels != theCFs->numChannels)) {
		if ((frequencies = (Float *) calloc(theCFs->numChannels,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for frequency array (%d)."),
			  funcName, theCFs->numChannels);
			return(FALSE);
		}
		numChannels = MINIMUM(theCFs->oldNumChannels, theCFs->numChannels);
		for (i = 0; i < numChannels; i++)
			frequencies[i] = theCFs->frequency[i];
		free(theCFs->frequency);
		theCFs->frequency = frequencies;
	}
	if ((theCFs->bandwidthMode.specifier == BANDWIDTH_USER) &&
	  (theCFs->oldNumChannels != theCFs->numChannels)) {
		if ((bandwidths = (Float *) calloc(theCFs->numChannels,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for bandwidth array (%d)."),
			  funcName, theCFs->numChannels);
			return(FALSE);
		}
		numChannels = MINIMUM(theCFs->oldNumChannels, theCFs->numChannels);
		for (i = 0; i < numChannels; i++)
			bandwidths[i] = theCFs->bandwidth[i];
		free(theCFs->bandwidth);
		theCFs->bandwidth = bandwidths;
	}
	if (!SetGeneratedPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not regenerate CF list."), funcName);
		return(FALSE);
	}
	if (!SetBandwidthArray_CFList(theCFs, theCFs->bandwidth)) {
		NotifyError(wxT("%s: Could not regenerate CF list."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetDiagnosticMode **************************/

/*
 * This routine sets the diagnostic mode of a CFList.
 */

BOOLN
SetDiagnosticMode_CFList(CFListPtr theCFs, WChar *modeName)
{
	static const WChar *funcName = wxT("SetDiagnosticMode_CFList");
	int		mode;

	if ((mode = Identify_NameSpecifier(modeName, cFListDiagModeList)) ==
	  CFLIST_DIAG_NULL) {
		NotifyError(wxT("%s: Unknown diagnostic mode (%s)."), funcName,
		  modeName);
		return(FALSE);
	}
	theCFs->diagnosticMode = (CFListDiagModeSpecifier) mode;
	return(TRUE);

}

/********************************* SetCFMode **********************************/

/*
 * This routine sets the diagnostic mode of a CFList.
 */

BOOLN
SetCFMode_CFList(CFListPtr theCFs, WChar *modeName)
{
	static const WChar *funcName = wxT("SetCFMode_CFList");
	int		mode;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((mode = Identify_NameSpecifier(modeName, CFModeList_CFList(0))) ==
	  CFLIST_NULL) {
		NotifyError(wxT("%s: Unknown CF mode (%s)."), funcName, modeName);
		return(FALSE);
	}
	theCFs->centreFreqMode = (CFListSpecifier) mode;
	switch (mode) {
	case CFLIST_SINGLE_MODE:
	case CFLIST_IDENTICAL_MODE:
		theCFs->numChannels = 1;
		break;
	case CFLIST_ERB_MODE:
	case CFLIST_ERBN_MODE:
	case CFLIST_LOG_MODE:
	case CFLIST_FOCAL_LOG_MODE:
	case CFLIST_LINEAR_MODE:
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		if (theCFs->numChannels < 2)
			theCFs->numChannels = 2;
		break;
	default:
		;
	}
	if (theCFs->cFParList) {
		SetCFUniParListMode_CFList(theCFs);
		theCFs->cFParList->updateFlag = TRUE;
	}
	return(TRUE);

}

/********************************* SetMinCF ***********************************/

/*
 * This routine sets the minimum frequency of a CFList.
 */

BOOLN
SetMinCF_CFList(CFListPtr theCFs, Float minCF)
{
	static const WChar *funcName = wxT("SetMinCF_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (minCF <= 0.0) {
		NotifyError(wxT("%s: Illegal frequency (%g Hz)."), funcName, minCF);
		return(FALSE);
	}
	theCFs->minCF = minCF;
	theCFs->minCFFlag = TRUE;
	if (!theCFs->maxCFFlag && (theCFs->minCF > theCFs->maxCF))
		theCFs->maxCF = theCFs->minCF * 2.0;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxCF ***********************************/

/*
 * This routine sets the maximum frequency of a CFList.
 */

BOOLN
SetMaxCF_CFList(CFListPtr theCFs, Float maxCF)
{
	static const WChar *funcName = wxT("SetMaxCF_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (maxCF <= 0.0) {
		NotifyError(wxT("%s: Illegal frequency (%g Hz)."), funcName, maxCF);
		return(FALSE);
	}
	theCFs->maxCF = maxCF;
	theCFs->maxCFFlag = TRUE;
	if (!theCFs->minCFFlag && (theCFs->maxCF < theCFs->minCF))
		theCFs->minCF = theCFs->maxCF / 2.0;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetFocalCF *********************************/

/*
 * This routine sets the maximum frequency of a CFList.
 */

BOOLN
SetFocalCF_CFList(CFListPtr theCFs, Float focalCF)
{
	static const WChar *funcName = wxT("SetFocalCF_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	theCFs->focalCF = focalCF;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetNumChannels *****************************/

/*
 * This routine sets the numChannels field of a CFList.
 */

BOOLN
SetNumChannels_CFList(CFListPtr theCFs, int numChannels)
{
	static const WChar *funcName = wxT("SetNumChannels_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (numChannels <= 0) {
		NotifyError(wxT("%s: Illegal number of channels (%d)."), funcName,
		  numChannels);
		return(FALSE);
	}
	if ((theCFs->centreFreqMode == CFLIST_SINGLE_MODE) && (numChannels != 1)) {
		NotifyError(wxT("%s: You cannot set the number of channels to more ")
		  wxT("than 1 when the 'single' CF mode is set.  First change the ")
		  wxT("mode."), funcName);
		return(FALSE);
	}
	theCFs->numChannels = numChannels;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetERBDensity ******************************/

/*
 * This routine sets the maximum frequency of a CFList.
 */

BOOLN
SetERBDensity_CFList(CFListPtr theCFs, Float eRBDensity)
{
	static const WChar *funcName = wxT("SetERBDensity_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (eRBDensity <= 0.0) {
		NotifyError(wxT("%s: Illegal ERB density (%g filters/critical band)."),
		  funcName, eRBDensity);
		return(FALSE);
	}
	theCFs->eRBDensity = eRBDensity;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetSingleFrequency *************************/

/*
 * This function sets a single frequency value of a CFList.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetSingleFrequency_CFList(CFListPtr theCFs, Float theFrequency)
{
	static const WChar *funcName = wxT("SetSingleFrequency_CFList");

	if (!SetIndividualFreq_CFList(theCFs, 0, theFrequency)) {
		NotifyError(wxT("%s: Could not set frequency."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetIndividualFreq **************************/

/*
 * This function sets the individual frequency values of a CFList.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualFreq_CFList(CFListPtr theCFs, int theIndex, Float theFrequency)
{
	static const WChar *funcName = wxT("SetIndividualFreq_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (theCFs->frequency == NULL) {
		NotifyError(wxT("%s: Frequencies not set."), funcName);
		return(FALSE);
	}
	if (theIndex > theCFs->numChannels - 1) {
		NotifyError(wxT("%s: Index value must be in the\nrange 0 - %d (%d).\n"),
		  funcName, theCFs->numChannels - 1, theIndex);
		return(FALSE);
	}
	theCFs->frequency[theIndex] = theFrequency;
	theCFs->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetIndividualBandwidth *********************/

/*
 * This function sets the individual bandwidth values of a CFList.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualBandwidth_CFList(CFListPtr theCFs, int theIndex,
  Float theBandwidth)
{
	static const WChar *funcName = wxT("SetIndividualBandwidth_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (theCFs->bandwidth == NULL) {
		NotifyError(wxT("%s: Frequencies not set."), funcName);
		return(FALSE);
	}
	if (theIndex > theCFs->numChannels - 1) {
		NotifyError(wxT("%s: Index value must be in the\nrange 0 - %d (%d).\n"),
		  funcName, theCFs->numChannels - 1, theIndex);
		return(FALSE);
	}
	theCFs->bandwidth[theIndex] = theBandwidth;
	return(TRUE);

}

/********************************* SetCFUniParListMode ************************/

/*
 * This routine enables and disables the respective parameters for each CF mode.
 * It assumes that the parameter list has been correctly initialised.
 */

BOOLN
SetCFUniParListMode_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("SetCFUniParListMode_CFList");
	int		i;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	for (i = CFLIST_NUM_CONSTANT_PARS; i < CFLIST_NUM_PARS; i++)
		theCFs->cFParList->pars[i].enabled = FALSE;
	switch (theCFs->centreFreqMode) {
	case CFLIST_ERBN_MODE:
	case CFLIST_LOG_MODE:
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		theCFs->cFParList->pars[CFLIST_CF_MIN_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_MAX_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_NUM_CHANNELS].enabled = TRUE;
		break;
	case CFLIST_ERB_MODE:
		theCFs->cFParList->pars[CFLIST_CF_MIN_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_MAX_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_ERB_DENSITY].enabled = TRUE;
		break;
	case CFLIST_FOCAL_LOG_MODE:
	case CFLIST_LINEAR_MODE:
		theCFs->cFParList->pars[CFLIST_CF_FOCAL_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_MIN_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_MAX_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_NUM_CHANNELS].enabled = TRUE;
		break;
	case CFLIST_SINGLE_MODE:
		theCFs->cFParList->pars[CFLIST_CF_SINGLE_FREQ].enabled = TRUE;
		break;
	case CFLIST_USER_MODE:
		theCFs->cFParList->pars[CFLIST_CF_NUM_CHANNELS].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_FREQUENCIES].enabled = TRUE;
		break;
	case CFLIST_IDENTICAL_MODE:
		theCFs->cFParList->pars[CFLIST_CF_SINGLE_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_NUM_CHANNELS].enabled = TRUE;
		break;
	default:
		NotifyError(wxT("%s: CF mode (%d) not implemented."), funcName,
		  theCFs->centreFreqMode);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetCFUniParList ****************************/

/*
 * This routine initialises and sets the CFList's universal parameter list.
 * This list provides universal access to the CFList's parameters.
 */

BOOLN
SetCFUniParList_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("SetCFUniParList_CFList");
	UniParPtr	pars;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!theCFs->cFParList && ((theCFs->cFParList = InitList_UniParMgr(
	  UNIPAR_SET_CFLIST, CFLIST_NUM_PARS, theCFs)) == NULL)) {
		NotifyError(wxT("%s: Could not initialise CF parList."), funcName);
		return(FALSE);
	}
	pars = theCFs->cFParList->pars;
	SetPar_UniParMgr(&pars[CFLIST_CF_DIAGNOSTIC_MODE], wxT("DIAG_MODE"),
	 wxT("Diagnostic mode ('list' or 'parameters')."),
	  UNIPAR_NAME_SPEC,
	  &theCFs->diagnosticMode, cFListDiagModeList,
	  (void * (*)) SetDiagnosticMode_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_MODE], wxT("CF_MODE"),
	 wxT("Centre frequency mode ('single', 'ERB', 'ERB_n', 'log', 'linear', ")
	 wxT("'focal_log', 'user', 'human', 'cat', 'chinchilla', 'guinea_pig',  ")
	 wxT("'macaque' or 'identical')."),
	  UNIPAR_NAME_SPEC,
	  &theCFs->centreFreqMode, CFModeList_CFList(0),
	  (void * (*)) SetCFMode_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_SINGLE_FREQ], wxT("SINGLE_CF"),
	  wxT("Centre frequency (Hz)."),
	  UNIPAR_REAL,
	  &theCFs->frequency[0], NULL,
	  (void * (*)) SetSingleFrequency_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_FOCAL_FREQ], wxT("FOCAL_CF"),
	  wxT("Focal centre frequency (Hz)."),
	  UNIPAR_REAL,
	  &theCFs->focalCF, NULL,
	  (void * (*)) SetFocalCF_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_MIN_FREQ], wxT("MIN_CF"),
	  wxT("Minimum centre frequency (Hz)."),
	  UNIPAR_REAL,
	  &theCFs->minCF, NULL,
	  (void * (*)) SetMinCF_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_MAX_FREQ], wxT("MAX_CF"),
	  wxT("Maximum centre frequency (Hz)."),
	  UNIPAR_REAL,
	  &theCFs->maxCF, NULL,
	  (void * (*)) SetMaxCF_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_NUM_CHANNELS], wxT("CHANNELS"),
	  wxT("No. of centre frequencies."),
	  UNIPAR_INT,
	  &theCFs->numChannels, NULL,
	  (void *(*)) SetNumChannels_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_ERB_DENSITY], wxT("ERB_DENSITY"),
	  wxT("ERB density (filters/critical band)."),
	  UNIPAR_REAL,
	  &theCFs->eRBDensity, NULL,
	 (void *(*)) SetERBDensity_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_FREQUENCIES], wxT("CENTRE_FREQ"),
	  wxT("Centre frequencies (Hz)."),
	  UNIPAR_REAL_ARRAY,
	  &theCFs->frequency, &theCFs->numChannels,
	  (void * (*)) SetIndividualFreq_CFList);
	return(TRUE);

}

/********************************* SetBandwidthSpecifier **********************/

/*
 * This routine sets the bandwidth specifier.
 * It also initialises the CFList data structure, if it has not already been
 * initialised.
 */

BOOLN
SetBandwidthSpecifier_CFList(CFListPtr theCFs, WChar *modeName)
{
	static const WChar *funcName = wxT("SetBandwidthSpecifier_CFList(");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!SetMode_Bandwidth(&theCFs->bandwidthMode, modeName)) {
		NotifyError(wxT("%s: Could not set bandwidth mode."), funcName);
		return(FALSE);
	}
	SetBandwidthArray_CFList(theCFs, theCFs->bandwidth);
	theCFs->updateFlag = TRUE;
	if (theCFs->bParList)
		SetBandwidthUniParListMode_CFList(theCFs);
	return(TRUE);

}

/********************************* SetBandwidthMin ****************************/

/*
 * This routine sets the bandwidth bwMin parameter.
 * At present the 'USER' mode is not implemented.
 */

BOOLN
SetBandwidthMin_CFList(CFListPtr theCFs, Float bwMin)
{
	static const WChar *funcName = wxT("SetBandwidthMin_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (bwMin <= 0.0) {
		NotifyError(wxT("%s: Illegal minimum bandwith (%g Hz)."), funcName,
		  bwMin);
		return(FALSE);
	}
	theCFs->bandwidthMode.bwMin = bwMin;
	return(TRUE);

}

/********************************* SetBandwidthQuality ************************/

/*
 * This routine sets the bandwidth Quality parameter.
 * At present the 'USER' mode is not implemented.
 */

BOOLN
SetBandwidthQuality_CFList(CFListPtr theCFs, Float quality)
{
	static const WChar *funcName = wxT("SetBandwidthQuality_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (quality <= 0.0) {
		NotifyError(wxT("%s: Illegal quality value (%g)."), funcName,
		  quality);
		return(FALSE);
	}
	theCFs->bandwidthMode.quality = quality;
	return(TRUE);

}

/********************************* SetBandwidthScalar *************************/

/*
 * This routine sets the bandwidth scaler parameter.
 */

BOOLN
SetBandwidthScaler_CFList(CFListPtr theCFs, Float scaler)
{
	static const WChar *funcName = wxT("SetBandwidthScalar_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (fabs(scaler) < DSAM_EPSILON) {
		NotifyError(wxT("%s: Scaler value must be greater than zero (%g)."),
		  funcName, scaler);
		return(FALSE);
	}
	theCFs->bandwidthMode.scaler = scaler;
	return(TRUE);

}

/********************************* SetBandwidthUniParListMode *****************/

/*
 * This routine enables and disables the respective parameters for each
 * bandwidth mode.
 * It assumes that the parameter list has been correctly initialised.
 */

BOOLN
SetBandwidthUniParListMode_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("SetBandwidthUniParListMode_CFList");
	int		i;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	for (i = BANDWIDTH_NUM_CONSTANT_PARS; i < BANDWIDTH_NUM_PARS; i++)
		theCFs->bParList->pars[i].enabled = FALSE;

	switch (theCFs->bandwidthMode.specifier) {
	case BANDWIDTH_ERB:
	case BANDWIDTH_CAT:
	case BANDWIDTH_GUINEA_PIG:
	case BANDWIDTH_NONLINEAR:
	case BANDWIDTH_DISABLED:
	case BANDWIDTH_INTERNAL_DYNAMIC:
	case BANDWIDTH_INTERNAL_STATIC:
		break;
	case BANDWIDTH_GUINEA_PIG_SCALED:
		theCFs->bParList->pars[BANDWIDTH_PAR_SCALER].enabled = TRUE;
		break;
	case BANDWIDTH_USER:
		theCFs->bParList->pars[BANDWIDTH_PAR_BANDWIDTH].enabled = TRUE;
		break;
	case BANDWIDTH_CUSTOM_ERB:
		theCFs->bParList->pars[BANDWIDTH_PAR_MIN].enabled = TRUE;
		theCFs->bParList->pars[BANDWIDTH_PAR_QUALITY].enabled = TRUE;
		break;
	default:
		NotifyError(wxT("%s: Bandwidth mode (%d) not implemented."), funcName,
		  theCFs->bandwidthMode.specifier);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetBandwidthUniParList *********************/

/*
 * This routine initialises and sets the CFlist's universal parameter list.
 * This list provides universal access to the CFList's parameters.
 */

BOOLN
SetBandwidthUniParList_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("SetBandwidthUniParList_CFList");

	UniParPtr	pars;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!theCFs->bParList && ((theCFs->bParList = InitList_UniParMgr(
	  UNIPAR_SET_CFLIST, BANDWIDTH_NUM_PARS, theCFs))) == NULL) {
		NotifyError(wxT("%s: Could not initialise bandwidth parList."),
		  funcName);
		return(FALSE);
	}
	pars = theCFs->bParList->pars;
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_MODE], wxT("B_MODE"),
	 wxT("Bandwidth mode ('ERB', 'Custom_ERB', 'Guinea_Pig', 'user' or ")
	 wxT("'Nonlinear')."),
	  UNIPAR_NAME_SPEC,
	  &theCFs->bandwidthMode.specifier, ModeList_Bandwidth(0),
	  (void *(*)) SetBandwidthSpecifier_CFList);
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_MIN], wxT("BW_MIN"),
	  wxT("Minimum filter bandwidth (Hz)."),
	  UNIPAR_REAL,
	  &theCFs->bandwidthMode.bwMin, NULL,
	  (void *(*)) SetBandwidthMin_CFList);
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_QUALITY], wxT("QUALITY"),
	  wxT("Ultimate quality factor of filters."),
	  UNIPAR_REAL,
	  &theCFs->bandwidthMode.quality, NULL,
	  (void *(*)) SetBandwidthQuality_CFList);
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_SCALER], wxT("SCALER"),
	  wxT("Scaler multiplier for all bandwidths."),
	  UNIPAR_REAL,
	  &theCFs->bandwidthMode.scaler, NULL,
	  (void *(*)) SetBandwidthScaler_CFList);
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_BANDWIDTH], wxT("BANDWIDTH"),
	  wxT("Filter bandwidths (Hz)."),
	  UNIPAR_REAL_ARRAY,
	  &theCFs->bandwidth, &theCFs->numChannels,
	  (void *(*)) SetIndividualBandwidth_CFList);
	return(TRUE);

}

/****************************** CheckInit *************************************/

/*
 * This routine checks whether or not a CFList.has been initialised.
 *
 */

BOOLN
CheckInit_CFList(CFListPtr theCFs, const WChar *callingFunction)
{
	static const WChar *funcName = wxT("CheckInit_CFList");

	if (theCFs == NULL) {
		NotifyError(wxT("%s: Signal not set in %s."), funcName,
		  callingFunction);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** CheckPars ***************************************/

/*
 * This routine checks that the necessary parameters for a CFList.have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("CheckPars_CFList");
	BOOLN	ok;
	int	i;

	if (!CheckInit_CFList(theCFs, wxT("CheckPars_CFList")))
		return(FALSE);
	if (theCFs->numChannels <= 0) {
		NotifyError(wxT("%s: Number of Channels not correctly set."), funcName);
		return(FALSE);
	}
	if (theCFs->frequency == NULL) {
		NotifyError(wxT("%s: No frequencies set."), funcName);
		return(FALSE);
	}
	ok = TRUE;
	for (i = 0; i < theCFs->numChannels; i++)
		if (theCFs->frequency[i] < 0.0) {
			NotifyError(wxT("%s: Invalid frequency[%d] = %g Hz."), funcName, i,
			  theCFs->frequency[i]);
			ok = FALSE;
		}
	if (theCFs->bandwidth) {
		for (i = 0; i < theCFs->numChannels; i++)
			if (theCFs->bandwidth[i] < 0.0) {
				NotifyError(wxT("%s: Invalid bandwidth[%d] = %g Hz."), funcName,
				 i, theCFs->bandwidth[i]);
				ok = FALSE;
			}
	}
	return(ok);

}

/****************************** AllocateFrequencies ***************************/

/*
 * This function allocates the memory for the CFList frequencies.
 * If the 'frequency' field is not NULL, then it will check that the number of
 * channels has not changed, in which case it will do nothing.
 */

BOOLN
AllocateFrequencies_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("AllocateFrequencies_CFList(");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((theCFs->numChannels != theCFs->oldNumChannels) && theCFs->frequency) {
		free(theCFs->frequency);
		theCFs->frequency = NULL;
	}
	if (!theCFs->frequency) {
		if ((theCFs->frequency = (Float *) calloc(theCFs->numChannels,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for frequencies."), funcName);
			return(FALSE);
		}
	}
	return(TRUE);

}

/********************************* GenerateDefault ****************************/

/*
 * This routine generates a default CFList.
 * If the number of channels is one, then it is assumed that 'single' or 'user'
 * mode is being used.
 */

CFListPtr
GenerateDefault_CFList(WChar *modeName, int numberOfCFs, Float minCF,
  Float maxCF, WChar *bwModeName, Float (* BWidthFunc)(struct BandwidthMode *,
  Float))
{
	static const WChar *funcName = wxT("GenerateDefault_CFList");
	Float	*frequencies;
	CFListPtr theCFs;

	if (numberOfCFs < 1) {
		NotifyError(wxT("%s: Insufficient CF's (%d)."), funcName, numberOfCFs);
		return(NULL);
	}
	if (Identify_NameSpecifier(modeName, CFModeList_CFList(0)) ==
	  CFLIST_SINGLE_MODE) {
		if ((frequencies = (Float *) calloc(numberOfCFs, sizeof(Float))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for frequencies (%d)"), funcName,
			  numberOfCFs);
			return(NULL);
		}
		frequencies[0] = minCF;
	} else
		frequencies = NULL;
	if ((theCFs = GenerateList_CFList(modeName, wxT("parameters"), numberOfCFs,
	  minCF, maxCF, -1.0, 0.0, frequencies)) == NULL) {
		NotifyError(wxT("%s: Could not generate default CF list."), funcName);
		Free_CFList(&theCFs);
		return(NULL);
	}
	theCFs->minCFFlag = FALSE;
	theCFs->maxCFFlag = FALSE;
	if (BWidthFunc)
		theCFs->bandwidthMode.Func = BWidthFunc;
	if (!SetBandwidths_CFList(theCFs, bwModeName, NULL)) {
		NotifyError(wxT("%s: Could not set CF bandwidths."),
		  funcName);
		Free_CFList(&theCFs);
		return(NULL);
	}
	return(theCFs);

}

/****************************** GenerateERB ***********************************/

/*
 * This function generates the centre frequency list from the frequency
 * range and ERB density.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateERB_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("GenerateERB_CFList");
	int		i;
	Float	theERBRate;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	theCFs->numChannels = (int) ceil((ERBRateFromF_Bandwidth(theCFs->maxCF) -
	  ERBRateFromF_Bandwidth(theCFs->minCF)) * theCFs->eRBDensity);
	if (theCFs->numChannels < 1) {
		NotifyError(wxT("%s: Illegal frequency range %g - %g, or ERB density, ")
		  wxT("%g."), funcName, theCFs->minCF, theCFs->maxCF, theCFs->
		  eRBDensity);
		return(FALSE);
	}
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not allocate frequencies."), funcName);
		return(FALSE);
	}
	for (i = 0, theERBRate = ERBRateFromF_Bandwidth(theCFs->minCF); i <
	  theCFs->numChannels; i++) {
		theCFs->frequency[i] = FFromERBRate_Bandwidth(theERBRate);
		theERBRate += 1.0 / theCFs->eRBDensity;
	}
	return(TRUE);

}

/****************************** GenerateERBn **********************************/

/*
 * This function generates the centre frequency list from the frequency
 * range and no. of CF's.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateERBn_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("GenerateERBn_CFList");
	int		i;
	Float	theERBRate;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not allocate frequencies."), funcName);
		return(FALSE);
	}
	theCFs->eRBDensity = (theCFs->numChannels - 1) / (ERBRateFromF_Bandwidth(
	  theCFs->maxCF) - ERBRateFromF_Bandwidth(theCFs->minCF));
	for (i = 0, theERBRate = ERBRateFromF_Bandwidth(theCFs->minCF); i <
	  theCFs->numChannels; i++) {
		theCFs->frequency[i] = FFromERBRate_Bandwidth(theERBRate);
		theERBRate += 1.0 / theCFs->eRBDensity;
	}
	return(TRUE);

}

/********************************* GenerateUser *******************************/

/*
 * This function generates a user defined list of centre frequencies in a
 * CFList data structure.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 * No checks are made that the array is the correct length.
 * This routine also sets default values for the CFList structure's 'minCF' and
 * 'maxCF' fields.
 */

BOOLN
GenerateUser_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("GenerateUser_CFList");
	int		i;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (theCFs->frequency == NULL) {
		NotifyError(wxT("%s: Frequency array not initialised."), funcName);
		return(FALSE);
	}
	for (i = 0; i < theCFs->numChannels; i++) {
		if (theCFs->frequency[i] < 0.0) {
			NotifyError(wxT("%s: Illegal frequency[%d] = %g Hz."), funcName, i,
			  theCFs->frequency[i]);
			return(FALSE);
		}
	}
	theCFs->minCF = theCFs->frequency[0];
	theCFs->maxCF = (theCFs->centreFreqMode == CFLIST_SINGLE_MODE)?
	  theCFs->frequency[0] * 10.0: theCFs->frequency[theCFs->numChannels - 1];
	return(TRUE);

}

/****************************** GenerateLog ***********************************/

/*
 * This function generates a logarithmic centre frequency list from the
 * frequency range and the number of filters.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateLog_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("GenerateLog_CFList");
	int		i;
	Float	theLogRate, logMinCF;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	logMinCF = log10(theCFs->minCF);
	theLogRate = (log10(theCFs->maxCF) - logMinCF) / (theCFs->numChannels - 1);
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not allocate frequencies."), funcName);
		return(FALSE);
	}
	for (i = 0; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = pow(10.0, logMinCF + theLogRate * i);
	return(TRUE);

}

/****************************** GenerateFocalLog ******************************/

/*
 * This function generates a logarithmic centre frequency list from the
 * frequency range and the number of filters.
 * It ensures that the specified frequency is the focus of the centre frequency
 * list.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateFocalLog_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("GenerateFocalLog_CFList");
	int		i, focalIndex;
	Float	theLogRate, logMinCF, logMaxCF, logFocalCF;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((theCFs->focalCF <= theCFs->minCF) || (theCFs->focalCF >=
	  theCFs->maxCF)) {
		NotifyError(wxT("%s: Focal frequency %g is not within the frequency ")
		  wxT("range %g - %g."), funcName, theCFs->focalCF, theCFs->minCF,
		  theCFs->maxCF);
		return(FALSE);
	}
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not allocate frequencies."), funcName);
		return(FALSE);
	}
	logFocalCF = log10(theCFs->focalCF);
	logMinCF = log10(theCFs->minCF);
	logMaxCF = log10(theCFs->maxCF);
	theLogRate = (logMaxCF - logMinCF) / (theCFs->numChannels - 1);
	focalIndex = (int) ((logFocalCF - logMinCF) / (logMaxCF - logMinCF) *
	  theCFs->numChannels);
	for (i = 0; i < focalIndex + 1; i++)
		theCFs->frequency[focalIndex - i] = pow(10.0, logFocalCF - theLogRate *
		  i);
	for (i = focalIndex + 1; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = pow(10.0, logFocalCF + theLogRate * (i -
		  focalIndex));
	return(TRUE);

}

/****************************** GenerateLinear ********************************/

/*
 * This function generates a linear centre frequency list from the
 * frequency range and the number of filters.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateLinear_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("GenerateLinear_CFList");
	int		i;
	Float	scale, offset;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	scale = (theCFs->maxCF - theCFs->minCF) / (theCFs->numChannels - 1);
	if (theCFs->focalCF > 0.0) {
		if ((theCFs->focalCF <= theCFs->minCF) || (theCFs->focalCF >=
		  theCFs->maxCF)) {
			NotifyError(wxT("%s: Focal frequency (%g) must be within the frequency ")
			  wxT("range %g - %g."), funcName, theCFs->focalCF, theCFs->minCF,
			  theCFs->maxCF);
			return(FALSE);
		}
		offset = scale * (int) floor((theCFs->focalCF - theCFs->minCF) / scale) +
		  theCFs->minCF - theCFs->focalCF;
	} else
		offset = 0.0;
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not allocate frequencies."), funcName);
		return(FALSE);
	}
	for (i = 0; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = scale * i + theCFs->minCF - offset;
	return(TRUE);

}

/****************************** GetGreenwoodPars ******************************/

/*
 * This function returns a pointer to the Greenwood pointer structure specified
 * by the species type.
 */

GreenwoodParsPtr
GetGreenwoodPars_CFList(int	species)
{
	static const WChar *funcName = wxT("GetGreenwoodPars_CFList");
	static GreenwoodPars	greenwoodPars[] = {

			{ CFLIST_CAT_MODE,			456.0,	0.8,	2.1 },
			{ CFLIST_CHINCHILLA_MODE,	163.5,	0.85,	2.1 },
			{ CFLIST_GPIG_MODE,			350.0,	0.85,	2.1 },
			{ CFLIST_HUMAN_MODE,		165.4,	0.88,	2.1 },
			{ CFLIST_MACAQUEM_MODE,		360.0,	0.85,	2.1 },
			{ -1,						0.0,	0.0,	0.0 }
		};
	int		i;

	for (i = 0; greenwoodPars[i].species >= 0; i++)
		if (greenwoodPars[i].species == species)
			return (&greenwoodPars[i]);
	NotifyError(wxT("%s: Unknown species. (%d)."), funcName, species);
	return(NULL);

}

/****************************** GenerateGreenwood *****************************/

/*
 * This function generates a Greenwood species centre frequency list from the
 * global 'greenwoodPars' lookup table.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

#define GREENWOOD_X(cF, PARS) (log10((cF) / ((PARS)->aA) + ((PARS)->k)) / \
		  (PARS)->a)

BOOLN
GenerateGreenwood_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("GenerateGreenwood_CFList");
	int		i;
	Float	xMin, xMax, scale;
	GreenwoodParsPtr	gPtr;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((gPtr = GetGreenwoodPars_CFList(theCFs->centreFreqMode)) == NULL) {
		NotifyError(wxT("%s: Could not set Greenwood parameters."), funcName);
		return(FALSE);
	}
	xMin = GREENWOOD_X(theCFs->minCF, gPtr);
	xMax = GREENWOOD_X(theCFs->maxCF, gPtr);
	scale = (xMax - xMin) / (theCFs->numChannels - 1);
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not allocate frequencies."), funcName);
		return(FALSE);
	}
	for (i = 0; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = gPtr->aA * (pow(10.0, gPtr->a * (xMin + i *
		  scale)) - gPtr->k);
	return(TRUE);

}

#undef GREENWOOD_X

/****************************** GenerateIdentical ***********************************/

/*
 * This function generates frequency list with the same CF.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateIdentical_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("GenerateIdentical_CFList");
	int		i;
	Float	singleCF = theCFs->frequency[0];

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not allocate frequencies."), funcName);
		return(FALSE);
	}
	theCFs->minCF = singleCF;
	theCFs->maxCF = singleCF;
	for (i = 0; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = singleCF;
	return(TRUE);

}

/****************************** RatifyPars ************************************/

/*
 * This routine ensures that the various parameters have the correct relation-
 * ship to each other, even if they are not enabled, i.e., even when the
 * 'eRBDensity' field is not used it is nice to keep it up to date (for use by
 * the universal parameter lists implentation).
 * It assumes that the frequency list has been correctly initialised.
 */

BOOLN
RatifyPars_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("RatifyPars_CFList");

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((theCFs->centreFreqMode != CFLIST_ERB_MODE) &&
	  (theCFs->centreFreqMode != CFLIST_ERBN_MODE))
		theCFs->eRBDensity = theCFs->numChannels / (ERBRateFromF_Bandwidth(
		  theCFs->maxCF) - ERBRateFromF_Bandwidth(theCFs->minCF));
//	if (theCFs->centreFreqMode != CFLIST_CF_FOCAL_FREQ)
//		theCFs->focalCF = (theCFs->frequency[0] + theCFs->frequency[
//		  theCFs->numChannels - 1]) / 2.0;
	return(TRUE);

}

/****************************** SetGeneratedPars ******************************/

/*
 * This function takes all the required parameters and sets the generated
 * parameters of existing CFList structure.
 * It returns FALSE if it fails in any way.
 * The 'oldNumChannels' is remembered so that if there is no change to the
 * number of channels, then the old frequency array can be used.
 */

BOOLN
SetGeneratedPars_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("SetGeneratedPars_CFList");
	BOOLN	ok = TRUE;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	switch (theCFs->centreFreqMode) {
	case CFLIST_ERB_MODE:
	case CFLIST_ERBN_MODE:
	case CFLIST_LOG_MODE:
	case CFLIST_FOCAL_LOG_MODE:
	case CFLIST_LINEAR_MODE:
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		if (theCFs->maxCF <= theCFs->minCF) {
			NotifyError(wxT("%s: Illegal frequency range %g - %g."), funcName,
			  theCFs->minCF, theCFs->maxCF);
			return(FALSE);
		}
	default:
		;
	};
	if (((theCFs->centreFreqMode == CFLIST_SINGLE_MODE) || (theCFs->centreFreqMode ==
	  CFLIST_IDENTICAL_MODE)) && (theCFs->numChannels < 1)) {
		NotifyError(wxT("%s: Illegal no. of centre frequencies (%d)."),
		  funcName, theCFs->numChannels);
		return(FALSE);
	}
	if (((theCFs->centreFreqMode == CFLIST_ERBN_MODE) ||
	  (theCFs->centreFreqMode == CFLIST_LOG_MODE) || (theCFs->centreFreqMode ==
	  CFLIST_ERBN_MODE) || (theCFs->centreFreqMode == CFLIST_LOG_MODE) ||
	  (theCFs->centreFreqMode == CFLIST_LINEAR_MODE)) && (theCFs->numChannels <
	  2)) {
		NotifyError(wxT("%s: Illegal no. of centre frequencies (%d)."),
		  funcName, theCFs->numChannels);
		return(FALSE);
	}
	switch (theCFs->centreFreqMode) {
	case CFLIST_SINGLE_MODE:
		ok = GenerateUser_CFList(theCFs);
		break;
	case CFLIST_USER_MODE:
		ok = GenerateUser_CFList(theCFs);
		break;
	case CFLIST_ERB_MODE:
		ok = GenerateERB_CFList(theCFs);
		break;
	case CFLIST_ERBN_MODE:
		ok = GenerateERBn_CFList(theCFs);
		break;
	case CFLIST_LOG_MODE:
		ok = GenerateLog_CFList(theCFs);
		break;
	case CFLIST_LINEAR_MODE:
		ok = GenerateLinear_CFList(theCFs);
		break;
	case CFLIST_FOCAL_LOG_MODE:
		ok = GenerateFocalLog_CFList(theCFs);
		break;
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		ok = GenerateGreenwood_CFList(theCFs);
		break;
	case CFLIST_IDENTICAL_MODE:
		ok = GenerateIdentical_CFList(theCFs);
		break;
	default:
		;
	} /* Switch */
	if (!ok) {
		NotifyError(wxT("%s: Could not generate CFList for '%s' frequency ")
		  wxT("mode."), funcName, CFModeList_CFList(theCFs->centreFreqMode)->
		  name);
		return(FALSE);
	}
	theCFs->oldNumChannels = theCFs->numChannels;
	theCFs->updateFlag = TRUE;
	RatifyPars_CFList(theCFs);
	if (!SetCFUniParList_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		return(FALSE);
	}
	SetCFUniParListMode_CFList(theCFs);
	return(TRUE);

}

/****************************** GenerateList **********************************/

/*
 * This function takes all the required parameters and returns a pointer to a
 * CFList structure.
 * It returns with the address of the CFList, or NULL if it fails.
 * Even though when the 'eRBDensity' field is not used it is nice to keep it up
 * to date (for use by the universal parameter lists implentation).
 * The 'oldNumChannels' is remembered so that if a change to 'iser' mode is
 * made, then the old frequency array can be used.
 */

CFListPtr
GenerateList_CFList(WChar *modeName, WChar *diagModeName, int numberOfCFs,
  Float minCF, Float maxCF, Float focalCF, Float eRBDensity,
  Float *frequencies)
{
	static const WChar *funcName = wxT("GenerateList_CFList");
	BOOLN	ok = TRUE;
	CFListPtr	theCFs = NULL;

	if ((theCFs = Init_CFList(funcName)) == NULL) {
		NotifyError(wxT("%s: Out of memory."), funcName);
		return(NULL);
	}
	if (!SetCFMode_CFList(theCFs, modeName)) {
		Free_CFList(&theCFs);
		return(NULL);
	}
	if (!SetDiagnosticMode_CFList(theCFs, diagModeName))
		ok = FALSE;

	switch (theCFs->centreFreqMode ) {
	case CFLIST_USER_MODE:
	case CFLIST_IDENTICAL_MODE:
		if (!SetNumChannels_CFList(theCFs, numberOfCFs))
			ok = FALSE;
		break;
	case CFLIST_ERB_MODE:
		if (!SetMinCF_CFList(theCFs, minCF) || !SetMaxCF_CFList(theCFs, maxCF))
			ok = FALSE;
		break;
	case CFLIST_ERBN_MODE:
	case CFLIST_LOG_MODE:
	case CFLIST_LINEAR_MODE:
	case CFLIST_FOCAL_LOG_MODE:
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		if (!SetNumChannels_CFList(theCFs, numberOfCFs))
			ok = FALSE;
		if (!SetMinCF_CFList(theCFs, minCF) || !SetMaxCF_CFList(theCFs, maxCF))
			ok = FALSE;
		break;
	default:
		;
	}

	switch (theCFs->centreFreqMode) {
	case CFLIST_FOCAL_LOG_MODE:
	case CFLIST_LINEAR_MODE:
		if (!SetFocalCF_CFList(theCFs, focalCF))
			ok = FALSE;
		break;
	case CFLIST_ERB_MODE:
		if (!SetERBDensity_CFList(theCFs, eRBDensity))
			ok = FALSE;
		break;
	case CFLIST_USER_MODE:
	case CFLIST_SINGLE_MODE:
	case CFLIST_IDENTICAL_MODE:
		if (!frequencies)
			ok = FALSE;
	default:
		theCFs->frequency = frequencies;
	}

	if (!ok) {
		NotifyError(wxT("%s: Could not set the CF list parameters."), funcName);
		Free_CFList(&theCFs);
		return(NULL);
	}
	if (!SetGeneratedPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not set CF list generated parameters '%s'")
		  wxT("frequency mode."), funcName, modeName);
		Free_CFList(&theCFs);
		return(NULL);
	}
	return(theCFs);

}

/****************************** ReadPars **************************************/

/*
 * This routine reads the CFList parameters from a file stream.
 * It returns with the address of the CFList, or NULL if it fails.
 */

CFListPtr
ReadPars_CFList(FILE *fp)
{
	static const WChar *funcName = wxT("ReadPars_CFList");
	BOOLN	ok = TRUE;
	WChar	modeName[MAXLINE], diagModeName[MAXLINE];
	int		i, mode, numberOfCFs;
	Float	*frequencies = NULL, focalCFFrequency, lowestCFFrequency;
	Float	highestCFFrequency, eRBDensity;
	CFListPtr	theCFs = NULL;

	if (!GetPars_ParFile(fp, wxT("%s"), diagModeName)) {
		NotifyError(wxT("%s: Could not read diagnostic mode."), funcName);
		return(NULL);
	}
	if (!GetPars_ParFile(fp, wxT("%s"), modeName)) {
		NotifyError(wxT("%s: Could not read centre frequency mode."), funcName);
		return(NULL);
	}
	switch (mode = Identify_NameSpecifier(modeName, CFModeList_CFList(0))) {
	case	CFLIST_SINGLE_MODE:
	case	CFLIST_USER_MODE:
		if (mode == CFLIST_SINGLE_MODE)
			numberOfCFs = 1;
		else {
			if (!GetPars_ParFile(fp, wxT("%d"), &numberOfCFs))
				ok = FALSE;
		}
		if (numberOfCFs < 1) {
			NotifyError(wxT("%s: Insufficient CF's (%d)."), funcName,
			  numberOfCFs);
			ok = FALSE;
		} else {
			if ((frequencies = (Float *) calloc(numberOfCFs,
			  sizeof(Float))) == NULL) {
				NotifyError(wxT("%s: Out of memory for frequencies (%d)"),
				  funcName, numberOfCFs);
				ok = FALSE;
			}
			for (i = 0; (i < numberOfCFs) && ok; i++)
				if (!GetPars_ParFile(fp, wxT("%lf"), &frequencies[i]))
					ok = FALSE;
		}
		break;
	case	CFLIST_ERB_MODE:
		if (!GetPars_ParFile(fp, wxT("%lf"), &lowestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, wxT("%lf"), &highestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, wxT("%lf"), &eRBDensity))
			ok = FALSE;
		break;
	case	CFLIST_ERBN_MODE:
	case	CFLIST_LOG_MODE:
	case	CFLIST_LINEAR_MODE:
	case	CFLIST_CAT_MODE:
	case	CFLIST_CHINCHILLA_MODE:
	case	CFLIST_GPIG_MODE:
	case	CFLIST_HUMAN_MODE:
	case	CFLIST_MACAQUEM_MODE:
		if (!GetPars_ParFile(fp, wxT("%lf"), &lowestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, wxT("%lf"), &highestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, wxT("%d"), &numberOfCFs))
			ok = FALSE;
		break;
	case	CFLIST_FOCAL_LOG_MODE:
		if (!GetPars_ParFile(fp, wxT("%lf"), &focalCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, wxT("%lf"), &lowestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, wxT("%lf"), &highestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, wxT("%d"), &numberOfCFs))
			ok = FALSE;
		break;
	default	:
		NotifyError(wxT("%s: Unknown frequency mode (%s)."), funcName,
		  modeName);
		ok = FALSE;
		break;
	} /* Switch */
	if (!ok) {
		NotifyError(wxT("%s: Failed to read centre frequency list parameters."),
		  funcName);
		return(NULL);
	}
	if ((theCFs = GenerateList_CFList(modeName, diagModeName, numberOfCFs,
	  lowestCFFrequency, highestCFFrequency, focalCFFrequency, eRBDensity,
	  frequencies)) == NULL) {
		NotifyError(wxT("%s: Could not generate CF list."), funcName);
		return(NULL);
	}
	return(theCFs);

}

/********************************* SetBandwidthArray **************************/

/*
 * This void sets the array of bandwidths for a CFList structure.
 * It expects all parameters to be correctly set, though checks are made upon
 * 'USER' bandwidths, if they already exist, otherwise they are created.
 */

BOOLN
SetBandwidthArray_CFList(CFListPtr theCFs, Float *theBandwidths)
{
	static const WChar *funcName = wxT("SetBandwidthArray_CFList");
	int		i;

	switch (theCFs->bandwidthMode.specifier) {
	case BANDWIDTH_NULL:
		break;
	case BANDWIDTH_USER:
		if (theBandwidths == NULL) {
			NotifyError(wxT("%s: Bandwidth array not initialised."), funcName);
			return(FALSE);
		}
		for (i = 0; i < theCFs->numChannels; i++)
			if (theBandwidths[i] <= 0.0) {
				NotifyError(wxT("%s: Illegal bandwidth[%d] = %g Hz."), funcName,
				  i, theBandwidths[i]);
				return(FALSE);
			}
		theCFs->bandwidth = theBandwidths;
		break;
	case BANDWIDTH_INTERNAL_DYNAMIC:
	case BANDWIDTH_DISABLED:
		if (theCFs->bandwidth) {
			free(theCFs->bandwidth);
			theCFs->bandwidth = NULL;
		}
		break;
	default:
		if (theCFs->bandwidth)
			free(theCFs->bandwidth);
		if ((theCFs->bandwidth = (Float *) calloc(theCFs->numChannels,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for bandwidths (%d)."), funcName,
			  theCFs->numChannels);
			return(FALSE);
		}
		for (i = 0; i < theCFs->numChannels; i++)
			theCFs->bandwidth[i] = (* theCFs->bandwidthMode.Func)(
			  &theCFs->bandwidthMode, theCFs->frequency[i]);
	} /* switch */
	return(TRUE);

}

/********************************* SetBandwidths ******************************/

/*
 * This function sets the  bandwidths for a CFList data structure, used
 * by the basilar membrane filter banks.
 * It returns TRUE if the operation is successful.
 * If the 'modeName' is set to NULL, then the mode will not be reset.
 */

BOOLN
SetBandwidths_CFList(CFListPtr theCFs, WChar *modeName, Float *theBandwidths)
{
	static const WChar *funcName = wxT("SetBandwidths_CFList");

	if (!CheckPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Centre frequency list  parameters not correctly ")
		  wxT("set, cannot add bandwidths."), funcName);
		return(FALSE);
	}
	if (modeName && !SetMode_Bandwidth(&theCFs->bandwidthMode, modeName)) {
		NotifyError(wxT("%s: Could not set bandwidth mode."), funcName);
		return(FALSE);
	}
	if (!SetBandwidthArray_CFList(theCFs, theBandwidths)) {
		NotifyError(wxT("%s: Could not set bandwidths."), funcName);
		return(FALSE);
	}
	theCFs->updateFlag = TRUE;
	if (!SetBandwidthUniParList_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not initialise parameter list for ")
		  wxT("bandwidths."), funcName);
		return(FALSE);
	}
	SetBandwidthUniParListMode_CFList(theCFs);
	return(TRUE);

}

/****************************** ReadBandwidths ********************************/

/*
 * This routine sets the bandwidths of a CFList, using a bandwidth mode
 * read from file.
 * The use of pointers in assigning the bandwidths to the array was employed
 * because of some bizarre quirk of the Symantec C++ compiler.
 * It returns TRUE if successful.
 */

BOOLN
ReadBandwidths_CFList(FILE *fp, CFListPtr theCFs)
{
	static const WChar *funcName = wxT("ReadBandwidths_CFList");
	WChar	modeName[MAXLINE];
	int		i;

	if (!CheckPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Centre frequency list parameters not correctly ")
		  wxT("set, cannot add bandwidths."), funcName);
		return(FALSE);
	}
	if (!GetPars_ParFile(fp, wxT("%s"), modeName)) {
		NotifyError(wxT("%s: Could not read bandwidth mode."), funcName);
		return(FALSE);
	}
	if (!SetMode_Bandwidth(&theCFs->bandwidthMode, modeName)) {
		NotifyError(wxT("%s: Could not set bandwidth mode."), funcName);
		return(FALSE);
	}
	switch (theCFs->bandwidthMode.specifier) {
	case BANDWIDTH_USER:
		if ((theCFs->bandwidth = (Float *) calloc(theCFs->numChannels,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for bandwidths."), funcName);
			return(FALSE);
		}
		for (i = 0; i < theCFs->numChannels; i++)
			if (!GetPars_ParFile(fp, wxT("%lf"), &theCFs->bandwidth[i])) {
				NotifyError(wxT("%s: Failed to read %d user bandwidths."),
				  funcName, theCFs->numChannels);
				return(FALSE);
			}
	case BANDWIDTH_DISABLED:
	case BANDWIDTH_INTERNAL_DYNAMIC:
	case BANDWIDTH_INTERNAL_STATIC:
		break;
	case BANDWIDTH_CUSTOM_ERB:
		if (!GetPars_ParFile(fp, wxT("%lf"), &theCFs->bandwidthMode.bwMin)) {
			NotifyError(wxT("%s: Could not read 'bwMin' for Custom ERB mode."),
			  funcName);
			return(FALSE);
		}
		if (!GetPars_ParFile(fp, wxT("%lf"), &theCFs->bandwidthMode.quality)) {
			NotifyError(wxT("%s: Could not read 'quality' for Custom ERB ")
			  wxT("mode."), funcName);
			return(FALSE);
		}
	default:
		;
	} /* switch */
	if (!SetBandwidths_CFList(theCFs, NULL, theCFs->bandwidth)) {
		NotifyError(wxT("%s: Could initialise bandwidth parameters."),
		  funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GetCF *****************************************/

/*
 * This routine returns the respective centre frequency value for a CFList
 * structure.
 *
 */

Float
GetCF_CFList(CFListPtr theCFs, int channel)
{
	static const WChar *funcName = wxT("GetCF_CFList");

	if (!CheckPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Centre frequency list parameters not correctly ")
		  wxT("set.  Zero returned."), funcName);
		return(0.0);
	}
	if (channel < 0 || channel >= theCFs->numChannels) {
		NotifyError(wxT("%s: Channel number must be in the range 0 to %d.  ")
		  wxT("Zero returned."), funcName, theCFs->numChannels - 1);
		return(0.0);
	}
	return(theCFs->frequency[channel]);

}

/****************************** ResetCF ***************************************/

/*
 * This routine resets the respective centre frequency value for a CFList
 * structure.
 * If the bandwidth mode is set, and it is not a user bandwidth, then the
 * bandwidth is automatically reset.
 *
 */

BOOLN
ResetCF_CFList(CFListPtr theCFs, int channel, Float theFrequency)
{
	static const WChar *funcName = wxT("ResetCF_CFList");

	if (theFrequency < 0.0) {
		NotifyError(wxT("%s: Illegal frequency value = %g."), funcName,
		  theFrequency);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Centre frequency list parameters not correctly ")
		  wxT("set."), funcName);
		return(FALSE);
	}
	if (channel < 0 || channel >= theCFs->numChannels) {
		NotifyError(wxT("%s: Channel number must be in the range 0 to %d."),
		  funcName, theCFs->numChannels - 1);
		return(FALSE);
	}
	if (theFrequency < 0.0) {
		NotifyError(wxT("%s: Illegal frequency value = %g."), funcName,
		  theFrequency);
		return(FALSE);
	}
	theCFs->frequency[channel] = theFrequency;
	if ((theCFs->bandwidthMode.specifier != BANDWIDTH_USER) &&
	  (theCFs->bandwidthMode.specifier != BANDWIDTH_INTERNAL_DYNAMIC) &&
	  (theCFs->bandwidthMode.specifier != BANDWIDTH_DISABLED))
		theCFs->bandwidth[channel] = (* theCFs->bandwidthMode.Func)(&theCFs->
		  bandwidthMode, theFrequency);
	theCFs->updateFlag = TRUE;
	return(TRUE);

}

/****************************** ResetBandwidth ********************************/

/*
 * This routine resets the respective bandwidth value for a CFList
 * structure.
 *
 */

BOOLN
ResetBandwidth_CFList(CFListPtr theCFs, int channel, Float theBandwidth)
{
	static const WChar *funcName = wxT("ResetBandwidth_CFList");
	if (theBandwidth < 0.0) {
		NotifyError(wxT("%s: Illegal bandwidth value = %g."), funcName,
		  theBandwidth);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Centre frequency list parameters  not correctly ")
		  wxT("set"), funcName);
		return(FALSE);
	}
	if (theCFs->bandwidthMode.specifier != BANDWIDTH_USER) {
		NotifyError(wxT("%s: Individual bandwidths can only be set in USER ")
		  wxT("bandwidth mode."), funcName);
		return(FALSE);
	}
	if (channel < 0 || channel >= theCFs->numChannels) {
		NotifyError(wxT("%s: Channel number must be in the range 0 to %d."),
		  funcName, theCFs->numChannels - 1);
		return(FALSE);
	}
	theCFs->bandwidth[channel] = theBandwidth;
	theCFs->updateFlag = TRUE;
	return(TRUE);

}

/****************************** GetBandwidth **********************************/

/*
 * This routine returns the respective bandwidth value for a CFList
 * structure.
 *
 */

Float
GetBandwidth_CFList(CFListPtr theCFs, int channel)
{
	static const WChar *funcName = wxT("GetBandwidth_CFList");

	if (!CheckPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Centre frequency list parameters not correctly ")
		  wxT("set.  Zero returned."), funcName);
		return(0.0);
	}
	if ((theCFs->bandwidthMode.specifier == BANDWIDTH_INTERNAL_DYNAMIC) ||
	  (theCFs->bandwidthMode.specifier == BANDWIDTH_DISABLED)) {
		NotifyError(wxT("%s: Internal/disabled bandwidths cannot be read.  ")
		  wxT("Zero returned"), funcName);
		return(0.0);
	}
	if (channel < 0 || channel >= theCFs->numChannels) {
		NotifyError(wxT("%s: Channel number must be in the range 0 to %d, ")
		  wxT("zero returned."), funcName, theCFs->numChannels - 1);
		return(0.0);
	}
	return(theCFs->bandwidth[channel]);

}

/****************************** PrintList *************************************/

/*
 * This routine prints a list of the parameters of the CFList structure.
 */

void
PrintList_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("PrintList_CFList(");
	int		i;

	if (!CheckPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Centre frequency list parameters not correctly ")
		  wxT("set."), funcName);
		return;
	}
	DPrint(wxT("\t\t%10s\t%10s\t%10s\n"), wxT("Filter No."), wxT("Frequency"),
	  wxT("Bandwidths"));
	DPrint(wxT("\t\t%10s\t%10s\t%10s\n"), wxT("          "), wxT("   (Hz)  "),
	  wxT("   (Hz)  "));
	for (i = 0; i < theCFs->numChannels; i++) {
		DPrint(wxT("\t\t%10d\t%10g"), i, theCFs->frequency[i]);
		if (theCFs->bandwidthMode.specifier == BANDWIDTH_INTERNAL_DYNAMIC)
			DPrint(wxT("\t%10s\n"), wxT("<internal>"));
		else if (theCFs->bandwidthMode.specifier == BANDWIDTH_DISABLED)
			DPrint(wxT("\t%10s\n"), wxT("<disabled>"));
		else if (theCFs->bandwidth) {
				DPrint(wxT("\t%10g\n"), theCFs->bandwidth[i]);
		} else
			DPrint(wxT("\t%10s\n"), wxT("<unset>"));
	}
	DPrint(wxT("\t\tCF Spacing mode: %s, Bandwidth mode: %s\n"),
	  CFModeList_CFList(theCFs->centreFreqMode)->name, ModeList_Bandwidth(
	    theCFs->bandwidthMode.specifier)->name);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints the parameters associated with a CFList data structure.
 */

void
PrintPars_CFList(CFListPtr theCFs)
{
	static const WChar *funcName = wxT("PrintPars_CFList");

	if (!CheckPars_CFList(theCFs)) {
		NotifyError(wxT("%s: Centre frequency list parameters not correctly ")
		  wxT("set."), funcName);
		return;
	}
	DPrint(wxT("\tCentre Frequency structure parameters:-\n"));
	DPrint(wxT("\t\tDiagnostic mode: %s:\n"), cFListDiagModeList[
	  theCFs->diagnosticMode].name);
	switch (theCFs->diagnosticMode) {
	case CFLIST_PARAMETERS_DIAG_MODE:
		if ((theCFs->centreFreqMode == CFLIST_USER_MODE) ||
		  (theCFs->centreFreqMode == CFLIST_SINGLE_MODE) ||
		  (theCFs->bandwidthMode.specifier == BANDWIDTH_USER)) {
			PrintList_CFList(theCFs);
			return;
		}
		DPrint(wxT("\t\tCF Spacing mode: %s\n"),
		  CFModeList_CFList(theCFs->centreFreqMode)->name);
		switch (theCFs->centreFreqMode) {
		case CFLIST_ERB_MODE:
			DPrint(wxT("\t\tMinimum/maximum frequency: %g / %g Hz,\n"),
			  theCFs->minCF, theCFs->maxCF);
			DPrint(wxT("\t\tERB density: %g filters/critical band.\n"),
			  theCFs->eRBDensity);
			break;
		case CFLIST_ERBN_MODE:
		case CFLIST_LOG_MODE:
		case CFLIST_CAT_MODE:
		case CFLIST_CHINCHILLA_MODE:
		case CFLIST_GPIG_MODE:
		case CFLIST_HUMAN_MODE:
		case CFLIST_MACAQUEM_MODE:
			DPrint(wxT("\t\tMinimum/maximum frequency: %g / %g Hz,\n"),
			  theCFs->minCF, theCFs->maxCF);
			DPrint(wxT("\t\tNumber of CF's: %d.\n"), theCFs->numChannels);
			break;
		case CFLIST_FOCAL_LOG_MODE:
		case CFLIST_LINEAR_MODE:
			DPrint(wxT("\t\tMinimum/maximum frequency: %g / %g Hz,\n"),
			  theCFs->minCF, theCFs->maxCF);
			DPrint(wxT("\t\tFocal CF: %g (Hz),\tNumber of CF's: %d.\n"),
			  theCFs->focalCF, theCFs->numChannels);
			break;
		default:
			;
		} /* Switch */
		DPrint(wxT("\t\tBandwidth mode: %s\n"), ModeList_Bandwidth(
		  theCFs->bandwidthMode.specifier)->name);
		switch (theCFs->bandwidthMode.specifier) {
		case BANDWIDTH_CUSTOM_ERB:
			DPrint(wxT("\t\tMinimum bandwidth: %g (Hz),"),
			  theCFs->bandwidthMode.bwMin);
			DPrint(wxT("\tUltimate quality factor: %g.\n"),
			  theCFs->bandwidthMode.quality);
			break;
		default:
			;
		}
		break;
	default:
		PrintList_CFList(theCFs);
	}

}

/****************************** CFRateFromF ***********************************/

/*
 * This function returns the CF Rate according to the frequency spacing used.
 */

Float
CFRateFromF_CFList(CFListPtr theCFs, Float frequency)
{
	static const WChar *funcName = wxT("CFRateFromF_CFList");

	switch (theCFs->centreFreqMode) {
	case CFLIST_SINGLE_MODE:
	case CFLIST_ERB_MODE:
	case CFLIST_ERBN_MODE:
		return(ERBRateFromF_Bandwidth(frequency));
	case CFLIST_LOG_MODE:
	case CFLIST_FOCAL_LOG_MODE:
		return(log10(frequency));
	default:
		NotifyWarning(wxT("%s: Centre frequency mode '%s' not implemented - ")
		  wxT("returning log rate."), funcName, CFModeList_CFList(
		  theCFs->centreFreqMode)->name);
		return(log10(frequency));
	}
}

/****************************** CFSpace ***************************************/

/*
 * This routine calculates the mean difference in ERB rate for the range of
 * frequencies in the CF List structure.
 * It assumes that the CFList structure has been correctly initialised.
 * An undefined value of "1" is returned if there are less than two channels.
 */

Float
CFSpace_CFList(CFListPtr theCFs)
{
	int		i;
	Float	sum;

	if (theCFs->numChannels < 2)
		return(1.0);
	for (i = 1, sum = 0.0; i < theCFs->numChannels; i++)
		sum += CFRateFromF_CFList(theCFs, theCFs->frequency[i]) -
		  CFRateFromF_CFList(theCFs, theCFs->frequency[i - 1]);
	return(sum / (theCFs->numChannels - 1));

}

/****************************** FindCF ***************************************/

/*
 * This function looks for a specified CF within a CF list.
 * It returns the index or -1 if a match is not found.
 * It assumes that the CFList structure has been correctly initialised.
 */

int
FindCF_CFList(CFListPtr theCFs, Float theCF, Float accuracy)
{
	int		i;

	for (i = 0; i < theCFs->numChannels; i++)
		if (fabs(theCFs->frequency[i] - theCF) < fabs(accuracy))
			return(i);
	return(-1);

}
