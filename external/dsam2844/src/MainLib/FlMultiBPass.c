/**********************
 *
 * File:		FlMultiBPass.c
 * Purpose:		This is an implementation of an outer-/middle-ear pre-emphasis
 *				filter using the filters.c module.
 * Comments:	Written using ModuleProducer version 1.2.2 (Nov 11 1998).
 *				A parallel, multiple band-pass filter is used to model the
 *				effects of the outer- and middle-ear on an input signal.
 *				The first data EarObject in the BPass is the process EarObject.
 *				12-11-98 LPO: I have changed this module to use arrays rather
 *				than a data structure.  This change makes it easier to do the
 *				universal parameter implementation.
 * Author:		L. P. O'Mard
 * Created:		20 Jul 1998
 * Updated:		12 Nov 1998
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
#include <float.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtFilters.h"
#include "FlMultiBPass.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

MultiBPassFPtr	multiBPassFPtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_Filter_MultiBPass(void)
{
	if (multiBPassFPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Filter_MultiBPass();
	if (multiBPassFPtr->cascade) {
		free(multiBPassFPtr->cascade);
		multiBPassFPtr->cascade = NULL;
	}
	if (multiBPassFPtr->gain) {
		free(multiBPassFPtr->gain);
		multiBPassFPtr->gain = NULL;
	}
	if (multiBPassFPtr->upperCutOffFreq) {
		free(multiBPassFPtr->upperCutOffFreq);
		multiBPassFPtr->upperCutOffFreq = NULL;
	}
	if (multiBPassFPtr->lowerCutOffFreq) {
		free(multiBPassFPtr->lowerCutOffFreq);
		multiBPassFPtr->lowerCutOffFreq = NULL;
	}
	if (multiBPassFPtr->parList)
		FreeList_UniParMgr(&multiBPassFPtr->parList);
	if (multiBPassFPtr->parSpec == GLOBAL) {
		free(multiBPassFPtr);
		multiBPassFPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetDefaultNumFiltersArrays *****************/

/*
 * This routine sets the default arrays and array values.
 */

BOOLN
SetDefaultNumFiltersArrays_Filter_MultiBPass(void)
{
	static const WChar *funcName =
	  wxT("SetDefaultNumFiltersArrays_Filter_MultiBPass");
	int		i;
	int		cascade[] = {2, 2, 2};
	Float	gain[] = {1.5, 6.0, -11.0};
	Float	lowerCutOffFreq[] = {330.0, 1900.0, 7500.0};
	Float	upperCutOffFreq[] = {5500.0, 5000.0, 14000.0};

	if (!AllocNumFilters_Filter_MultiBPass(3)) {
		NotifyError(wxT("%s: Could not allocate default arrays."), funcName);
		return(FALSE);
	}
	for (i = 0; i < multiBPassFPtr->numFilters; i++) {
		multiBPassFPtr->cascade[i] = cascade[i];
		multiBPassFPtr->gain[i] = gain[i];
		multiBPassFPtr->upperCutOffFreq[i] = upperCutOffFreq[i];
		multiBPassFPtr->lowerCutOffFreq[i] = lowerCutOffFreq[i];
	}
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This function initialises the module by setting module's parameter
 * pointer structure.
 * The GLOBAL option is for hard programming - it sets the module's
 * pointer to the global parameter structure and initialises the
 * parameters. The LOCAL option is for generic programming - it
 * initialises the parameter structure currently pointed to by the
 * module's parameter pointer.
 */

BOOLN
Init_Filter_MultiBPass(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Filter_MultiBPass");

	if (parSpec == GLOBAL) {
		if (multiBPassFPtr != NULL)
			Free_Filter_MultiBPass();
		if ((multiBPassFPtr = (MultiBPassFPtr) malloc(sizeof(MultiBPassF))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (multiBPassFPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	multiBPassFPtr->parSpec = parSpec;
	multiBPassFPtr->updateProcessVariablesFlag = TRUE;
	multiBPassFPtr->numFilters = 0;
	multiBPassFPtr->cascade = NULL;
	multiBPassFPtr->gain = NULL;
	multiBPassFPtr->upperCutOffFreq = NULL;
	multiBPassFPtr->lowerCutOffFreq = NULL;

	if (!SetDefaultNumFiltersArrays_Filter_MultiBPass()) {
		NotifyError(wxT("%s: Could not set the default 'numFilters' arrays."),
		  funcName);
		return(FALSE);
	}

	if (!SetUniParList_Filter_MultiBPass()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Filter_MultiBPass();
		return(FALSE);
	}
	multiBPassFPtr->numChannels = 0;
	multiBPassFPtr->bPassPars = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Filter_MultiBPass(void)
{
	static const WChar *funcName = wxT("SetUniParList_Filter_MultiBPass");
	UniParPtr	pars;

	if ((multiBPassFPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  MULTIBPASS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = multiBPassFPtr->parList->pars;
	SetPar_UniParMgr(&pars[MULTIBPASS_NUMFILTERS], wxT("NUM_FILTERS"),
	  wxT("No. of parallel band pass filters."),
	  UNIPAR_INT_AL,
	  &multiBPassFPtr->numFilters, NULL,
	  (void * (*)) SetNumFilters_Filter_MultiBPass);
	SetPar_UniParMgr(&pars[MULTIBPASS_CASCADE], wxT("CASCADE"),
	  wxT("Filter cascades."),
	  UNIPAR_INT_ARRAY,
	  &multiBPassFPtr->cascade, &multiBPassFPtr->numFilters,
	  (void * (*)) SetIndividualCascade_Filter_MultiBPass);
	SetPar_UniParMgr(&pars[MULTIBPASS_GAIN], wxT("GAIN"),
	  wxT("Filter pre-attentuation (dB SPL)."),
	  UNIPAR_REAL_ARRAY,
	  &multiBPassFPtr->gain, &multiBPassFPtr->numFilters,
	  (void * (*)) SetIndividualGain_Filter_MultiBPass);
	SetPar_UniParMgr(&pars[MULTIBPASS_LOWERCUTOFFFREQ],
	  wxT("LOWER_FREQ"),
	  wxT("Filter lower cut-off frequencies (Hz)."),
	  UNIPAR_REAL_ARRAY,
	  &multiBPassFPtr->lowerCutOffFreq, &multiBPassFPtr->numFilters,
	  (void * (*)) SetIndividualLowerCutOffFreq_Filter_MultiBPass);
	SetPar_UniParMgr(&pars[MULTIBPASS_UPPERCUFOFFFREQ],
	  wxT("UPPER_FREQ"),
	  wxT("Filter upper cut-off frequencies (Hz)."),
	  UNIPAR_REAL_ARRAY,
	  &multiBPassFPtr->upperCutOffFreq, &multiBPassFPtr->numFilters,
	  (void * (*)) SetIndividualUpperCutOffFreq_Filter_MultiBPass);

	SetAltAbbreviation_UniParMgr(&pars[MULTIBPASS_GAIN], wxT("ATTENUATION"));
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Filter_MultiBPass(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(multiBPassFPtr->parList);

}

/****************************** AllocNumFilters *******************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the 'numFilters'
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numFilters'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumFilters_Filter_MultiBPass(int numFilters)
{
	static const WChar	*funcName = wxT("AllocNumFilters_Filter_MultiBPass");

	if (numFilters == multiBPassFPtr->numFilters)
		return(TRUE);
	if (multiBPassFPtr->cascade)
		free(multiBPassFPtr->cascade);
	if ((multiBPassFPtr->cascade = (int *) calloc(numFilters, sizeof(int))) ==
	  NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' cascade."),
		  funcName, numFilters);
		return(FALSE);
	}
	if (multiBPassFPtr->gain)
		free(multiBPassFPtr->gain);
	if ((multiBPassFPtr->gain = (Float *) calloc(numFilters,
	  sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' gain."),
		  funcName, numFilters);
		return(FALSE);
	}
	if (multiBPassFPtr->upperCutOffFreq)
		free(multiBPassFPtr->upperCutOffFreq);
	if ((multiBPassFPtr->upperCutOffFreq = (Float *) calloc(numFilters,
	  sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' upperCutOffFreq."),
		  funcName, numFilters);
		return(FALSE);
	}
	if (multiBPassFPtr->lowerCutOffFreq)
		free(multiBPassFPtr->lowerCutOffFreq);
	if ((multiBPassFPtr->lowerCutOffFreq = (Float *) calloc(numFilters,
	  sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' lowerCutOffFreq."),
		  funcName, numFilters);
		return(FALSE);
	}
	multiBPassFPtr->numFilters = numFilters;
	return(TRUE);

}

/****************************** SetNumFilters *********************************/

/*
 * This function sets the module's numFilters parameter.
 * It returns TRUE if the operation is successful.
 * The 'numFilters' variable is set by the 'AllocNumFilters_Filter_MultiBPass'
 * routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFilters_Filter_MultiBPass(int theNumFilters)
{
	static const WChar	*funcName = wxT("SetNumFilters_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumFilters < 1) {
		NotifyError(wxT("%s: Value must be greater then zero (%d)."), funcName,
		  theNumFilters);
		return(FALSE);
	}
	if (!AllocNumFilters_Filter_MultiBPass(theNumFilters)) {
		NotifyError(wxT("%s: Cannot allocate memory for the 'numFilters' ")
		  wxT("arrays."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetCascade ************************************/

/*
 * This function sets the module's cascade array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCascade_Filter_MultiBPass(int *theCascade)
{
	static const WChar	*funcName = wxT("SetCascade_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->cascade = theCascade;
	return(TRUE);

}

/****************************** SetIndividualCascade **************************/

/*
 * This function sets the module's cascade array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualCascade_Filter_MultiBPass(int theIndex, int theCascade)
{
	static const WChar	*funcName =
	  wxT("SetIndividualCascade_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->cascade == NULL) {
		NotifyError(wxT("%s: Cascade not set."), funcName);
		return(FALSE);
	}
	if (theIndex > multiBPassFPtr->numFilters - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, multiBPassFPtr->numFilters - 1, theIndex);
		return(FALSE);
	}
	if (theCascade < 1) {
		NotifyError(wxT("%s: This value must be greater than 0 (%d).\n"),
		  funcName, theCascade);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->cascade[theIndex] = theCascade;
	return(TRUE);

}

/****************************** SetGain ***************************************/

/*
 * This function sets the module's gain array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGain_Filter_MultiBPass(Float *theGain)
{
	static const WChar	*funcName = wxT("SetGain_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->gain = theGain;
	return(TRUE);

}

/****************************** SetIndividualGain *****************************/

/*
 * This function sets the module's gain array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualGain_Filter_MultiBPass(int theIndex,
  Float theGain)
{
	static const WChar *funcName =
	  wxT("SetIndividualGain_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->gain == NULL) {
		NotifyError(wxT("%s: Gain not set."), funcName);
		return(FALSE);
	}
	if (theIndex > multiBPassFPtr->numFilters - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, multiBPassFPtr->numFilters - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->gain[theIndex] = theGain;
	return(TRUE);

}

/****************************** SetUpperCutOffFreq ****************************/

/*
 * This function sets the module's upperCutOffFreq array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUpperCutOffFreq_Filter_MultiBPass(Float *theUpperCutOffFreq)
{
	static const WChar	*funcName =
	  wxT("SetUpperCutOffFreq_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->upperCutOffFreq = theUpperCutOffFreq;
	return(TRUE);

}

/****************************** SetIndividualUpperCutOffFreq ******************/

/*
 * This function sets the module's upperCutOffFreq array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualUpperCutOffFreq_Filter_MultiBPass(int theIndex,
  Float theUpperCutOffFreq)
{
	static const WChar	*funcName =
	  wxT("SetIndividualUpperCutOffFreq_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->upperCutOffFreq == NULL) {
		NotifyError(wxT("%s: UpperCutOffFreq not set."), funcName);
		return(FALSE);
	}
	if (theIndex > multiBPassFPtr->numFilters - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, multiBPassFPtr->numFilters - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->upperCutOffFreq[theIndex] = theUpperCutOffFreq;
	return(TRUE);

}

/****************************** SetLowerCutOffFreq ****************************/

/*
 * This function sets the module's lowerCutOffFreq array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowerCutOffFreq_Filter_MultiBPass(Float *theLowerCutOffFreq)
{
	static const WChar	*funcName =
	  wxT("SetLowerCutOffFreq_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->lowerCutOffFreq = theLowerCutOffFreq;
	return(TRUE);

}

/****************************** SetIndividualLowerCutOffFreq ******************/

/*
 * This function sets the module's lowerCutOffFreq array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualLowerCutOffFreq_Filter_MultiBPass(int theIndex,
 Float theLowerCutOffFreq)
{
	static const WChar	*funcName =
	  wxT("SetIndividualLowerCutOffFreq_Filter_MultiBPass");

	if (multiBPassFPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->lowerCutOffFreq == NULL) {
		NotifyError(wxT("%s: LowerCutOffFreq not set."), funcName);
		return(FALSE);
	}
	if (theIndex > multiBPassFPtr->numFilters - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, multiBPassFPtr->numFilters - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->lowerCutOffFreq[theIndex] = theLowerCutOffFreq;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Filter_MultiBPass(void)
{
	int		i;

	DPrint(wxT("Multiple Bandpass Filter Module Parameters:-\n"));
	DPrint(wxT("\tNo. Filters = %d\n"), multiBPassFPtr->numFilters);
	DPrint(wxT("\t%10s\t%10s\t%10s\t%10s\n"), wxT("Pre-atten. "), wxT(
	  "   Cascade  "), wxT(" L. Cutoff"), wxT(" H. Cutoff"));
	DPrint(wxT("\t%10s\t%10s\t%10s\t%10s\n"), wxT("(dB)"), wxT(""), wxT("(Hz)"),
	  wxT("(Hz)"));
	for (i = 0; i < multiBPassFPtr->numFilters; i++)
		DPrint(wxT("\t%10g\t%10d\t%10g\t%10g\n"), multiBPassFPtr->gain[i],
		  multiBPassFPtr->cascade[i], multiBPassFPtr->lowerCutOffFreq[i],
		  multiBPassFPtr->upperCutOffFreq[i]);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Filter_MultiBPass(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Filter_MultiBPass");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	multiBPassFPtr = (MultiBPassFPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Filter_MultiBPass(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Filter_MultiBPass");

	if (!SetParsPointer_Filter_MultiBPass(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Filter_MultiBPass(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = multiBPassFPtr;
	/*theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;*/
	theModule->Free = Free_Filter_MultiBPass;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_MultiBPass;
	theModule->PrintPars = PrintPars_Filter_MultiBPass;
	theModule->RunProcess = RunModel_Filter_MultiBPass;
	theModule->SetParsPointer = SetParsPointer_Filter_MultiBPass;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 */

BOOLN
CheckData_Filter_MultiBPass(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Filter_MultiBPass");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
		NotifyError(wxT("%s: Input signal not correctly initialised."),
		  funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Filter_MultiBPass(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_Filter_MultiBPass");
	BOOLN	ok = TRUE;
	int		i, j, k;
	Float	*statePtr;
	BPassParsPtr	bPParsPtr;
	MultiBPassFPtr	p = multiBPassFPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_MultiBPass();
		p->numChannels = _OutSig_EarObject(data)->numChannels;
		if ((p->bPassPars = (BPassParsPtr) calloc(p->numFilters, sizeof(
		  BPassPars))) == NULL) {
			NotifyError(wxT("%s: Cannot allocate memory for bPassPars array."),
			  funcName);
			return(FALSE);
		}
		for (i = 0; i < p->numFilters; i++) {
			bPParsPtr = &p->bPassPars[i];
			if ((bPParsPtr->coefficients = (BandPassCoeffsPtr *) calloc(
			  p->numChannels, sizeof(BandPassCoeffsPtr))) == NULL) {
		 		NotifyError(wxT("%s: Out of memory for filter %d's ")
				  wxT("coefficients."), funcName, i);
		 		return(FALSE);
			}
			if (i == 0)
				bPParsPtr->data = data;
			else {
				if ((bPParsPtr->data = Init_EarObject(wxT("NULL"))) == NULL) {
		 			NotifyError(wxT("%s: Out of memory for filter %d's data."),
					  funcName, i);
		 			return(FALSE);
				}
			}

		}
		if (!InitSubProcessList_EarObject(data, p->numFilters - 1)) {
			NotifyError(wxT("%s: Could not initialise %d sub-process list for ")
			  wxT("process."), funcName, p->numFilters);
			return(FALSE);
		}
	 	for (i = 0; (i < p->numFilters) && ok; i++) {
			bPParsPtr = &p->bPassPars[i];
			if (i != 0)
				data->subProcessList[i - 1] = bPParsPtr->data;
	 		for (j = 0; j < _OutSig_EarObject(data)->numChannels; j++)
				if ((bPParsPtr->coefficients[j] = InitBandPassCoeffs_Filters(
				  p->cascade[i], p->lowerCutOffFreq[i], p->upperCutOffFreq[i],
				  _InSig_EarObject(data, 0)->dt)) == NULL) {
				  	NotifyError(wxT("%s: Failed initialised filter %d, ")
					  wxT("channel %d."), funcName, i, j);
					ok = FALSE;
				}
		}
		if (!ok) {
			FreeProcessVariables_Filter_MultiBPass();
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (k = 0; k < p->numFilters; k++) {
			bPParsPtr = &p->bPassPars[k];
			for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
				statePtr = bPParsPtr->coefficients[i]->state;
				for (j = 0; j < p->cascade[k] *
				  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
					*statePtr++ = 0.0;
			}
		}
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Filter_MultiBPass(void)
{
	int			i, j;
	BPassParsPtr	bPParsPtr;

	if (!multiBPassFPtr->bPassPars)
		return;
	for (i = 0; i < multiBPassFPtr->numFilters; i++) {
		bPParsPtr = &multiBPassFPtr->bPassPars[i];
		if (bPParsPtr->coefficients) {
			for (j = 0; j < multiBPassFPtr->numChannels; j++)
				FreeBandPassCoeffs_Filters(&bPParsPtr->coefficients[j]);
			free(bPParsPtr->coefficients);
			bPParsPtr->coefficients = NULL;
		}
		if ((i != 0) && bPParsPtr->data)
			Free_EarObject(&bPParsPtr->data);
	}
	free(multiBPassFPtr->bPassPars);
	multiBPassFPtr->bPassPars = NULL;

}

/****************************** RunModel **************************************/

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
RunModel_Filter_MultiBPass(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_Filter_MultiBPass");
	register	ChanData	 *inPtr, *outPtr;
	int		i, chan;
	ChanLen	j;
	BPassParsPtr	bPParsPtr;
	SignalDataPtr	outSignal;
	MultiBPassFPtr	p = multiBPassFPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Filter_MultiBPass(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Multiple Bandpass filter Module ")
		  wxT("process."));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Could not initialise the process output ")
			  wxT("signal."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Filter_MultiBPass(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		for (i = 0; i < p->numFilters; i++) {
			bPParsPtr = &p->bPassPars[i];
			TempInputConnection_EarObject(data, bPParsPtr->data, 1);
			InitOutTypeFromInSignal_EarObject(bPParsPtr->data, 0);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (i = 0; i < p->numFilters; i++)
		InitOutDataFromInSignal_EarObject(p->bPassPars[i].data);
	for (i = 0; i < p->numFilters; i++) {
		bPParsPtr = &p->bPassPars[i];
		if (fabs(p->gain[i]) > DSAM_EPSILON)
			GaindB_SignalData(_OutSig_EarObject(bPParsPtr->data), p->gain[i]);
		BandPass_Filters(_OutSig_EarObject(bPParsPtr->data), bPParsPtr->
		  coefficients);
	}
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		for (i = 1; i < p->numFilters; i++) {
			inPtr = _OutSig_EarObject(p->bPassPars[i].data)->channel[chan];
			outPtr = outSignal->channel[chan];
			for (j = 0; j < outSignal->length; j++)
				*outPtr++ += *inPtr++;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
