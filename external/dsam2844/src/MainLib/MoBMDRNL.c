/**********************
 *
 * File:		MoBMDRNL.c
 * Purpose:		The DRNL Composite filter is used to model the response of the
 *				basilar membrane.
 * Comments:	This is an implementation of a digital filter using the
 *				filters.c module.
 *				28-05-97 LPO: Corrected linearFLPCutOffScale parameter - it
 *				wasn't being used, and now a negative or zero value disables
 *				the filter altogether.
 *				06-06-97 LPO: added outputScale parameter.
 *				21-12-98 LPO: Implemented new parameters for filter bank.
 *				These parameters use the broken stick compression - see
 *				UtFilters.
 *				12-01-99 LPO: Implemented universal parameters list.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Jan 1999
 * Copyright:	(c) 1999, 2001, 2010 Lowel P. O'Mard
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
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtParArray.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtFilters.h"
#include "UtString.h"
#include "MoBMDRNL.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMDRNLPtr	bMDRNLPtr = NULL;

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
Free_BasilarM_DRNL(void)
{
	if (bMDRNLPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_DRNL();
	Free_ParArray(&bMDRNLPtr->nonLinBwidth);
	Free_ParArray(&bMDRNLPtr->nonLinScaleG);
	Free_ParArray(&bMDRNLPtr->comprScaleA);
	Free_ParArray(&bMDRNLPtr->comprScaleB);
	Free_ParArray(&bMDRNLPtr->linCF);
	Free_ParArray(&bMDRNLPtr->linBwidth);
	Free_ParArray(&bMDRNLPtr->linScaleG);
	Free_CFList(&bMDRNLPtr->theCFs);
	if (bMDRNLPtr->parList)
		FreeList_UniParMgr(&bMDRNLPtr->parList);
	if (bMDRNLPtr->parSpec == GLOBAL) {
		free(bMDRNLPtr);
		bMDRNLPtr = NULL;
	}
	return(TRUE);

}

/****************************** GetFitFuncPars ********************************/

/*
 * This function returns the number of parameters for the respective fit
 * function mode. Using it helps maintain the correspondence
 * between the mode names.
 */

int
GetFitFuncPars_BasilarM_DRNL(int mode)
{
	static const WChar	*funcName = wxT("GetFitFuncPars_BasilarM_DRNL");

	switch (mode) {
	case GENERAL_FIT_FUNC_EXP1_MODE:
	case GENERAL_FIT_FUNC_LOG1_MODE:
	case GENERAL_FIT_FUNC_LOG2_MODE:
	case GENERAL_FIT_FUNC_LINEAR1_MODE:
		return(2);
	case GENERAL_FIT_FUNC_POLY1_MODE:
		return(3);
	default:
		NotifyError(wxT("%s: Mode not listed (%d), returning zero."), funcName,
		  mode);
	}
	return(0);

}

/****************************** GetFitFuncValue *******************************/

/*
 * This function returns the value for the respective fit function mode.
 * Using it helps maintain the correspondence between the mode names.
 */

Float
GetFitFuncValue_BasilarM_DRNL(ParArrayPtr p, Float linCF)
{
	static const WChar	*funcName = wxT("GetFitFuncValue_BasilarM_DRNL");

	switch (p->mode) {
	case GENERAL_FIT_FUNC_EXP1_MODE:
		return(p->params[0] * exp(p->params[1] * linCF));
	case GENERAL_FIT_FUNC_LINEAR1_MODE:
		return(p->params[0] + p->params[1] * linCF);
	case GENERAL_FIT_FUNC_LOG1_MODE:
		return(pow(10.0, p->params[0] + p->params[1] * log10(linCF)));
	case GENERAL_FIT_FUNC_LOG2_MODE:
		return(p->params[0] + p->params[1] * log(linCF));
	case GENERAL_FIT_FUNC_POLY1_MODE:
		return(p->params[0] + p->params[1] * linCF + p->params[2] * linCF *
		  linCF);
	default:
		NotifyError(wxT("%s: Mode (%d) not listed, returning zero."), funcName,
		  p->mode);
	}
	return(0.0);

}

/****************************** SetDefaultParArrayPars ************************/

/*
 * This routine sets the default values for the parameter array parameters.
 */

void
SetDefaultParArrayPars_BasilarM_DRNL(void)
{
	int		i;
	Float	nonLinBwidth[] = {0.8, 58};
	Float	nonLinScaleG[] = {1.0, 0.0};
	Float	comprScaleA[] = {1.67, 0.45};
	Float	comprScaleB[] = {-5.85, 0.875};
	Float	linCF[] = {0.14, 0.95};
	Float	linBwidth[] = {1.3, 0.53};
	Float	linScaleG[] = {5.48, -0.97};

	SetMode_ParArray(bMDRNLPtr->nonLinBwidth, wxT("Log_func1"));
	for (i = 0; i < bMDRNLPtr->nonLinBwidth->numParams; i++)
		bMDRNLPtr->nonLinBwidth->params[i] = nonLinBwidth[i];
	SetMode_ParArray(bMDRNLPtr->nonLinScaleG, wxT("Linear_func1"));
	for (i = 0; i < bMDRNLPtr->nonLinScaleG->numParams; i++)
		bMDRNLPtr->nonLinScaleG->params[i] = nonLinScaleG[i];
	SetMode_ParArray(bMDRNLPtr->comprScaleA, wxT("Log_func1"));
	for (i = 0; i < bMDRNLPtr->comprScaleA->numParams; i++)
		bMDRNLPtr->comprScaleA->params[i] = comprScaleA[i];
	SetMode_ParArray(bMDRNLPtr->comprScaleB, wxT("Log_func1"));
	for (i = 0; i < bMDRNLPtr->comprScaleB->numParams; i++)
		bMDRNLPtr->comprScaleB->params[i] = comprScaleB[i];
	SetMode_ParArray(bMDRNLPtr->linCF, wxT("Log_func1"));
	for (i = 0; i < bMDRNLPtr->linCF->numParams; i++)
		bMDRNLPtr->linCF->params[i] = linCF[i];
	SetMode_ParArray(bMDRNLPtr->linBwidth, wxT("Log_func1"));
	for (i = 0; i < bMDRNLPtr->linBwidth->numParams; i++)
		bMDRNLPtr->linBwidth->params[i] = linBwidth[i];
	SetMode_ParArray(bMDRNLPtr->linScaleG, wxT("Log_func1"));
	for (i = 0; i < bMDRNLPtr->linScaleG->numParams; i++)
		bMDRNLPtr->linScaleG->params[i] = linScaleG[i];

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
Init_BasilarM_DRNL(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_BasilarM_DRNL");

	if (parSpec == GLOBAL) {
		if (bMDRNLPtr != NULL)
			Free_BasilarM_DRNL();
		if ((bMDRNLPtr = (BMDRNLPtr) malloc(sizeof(BMDRNL))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMDRNLPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bMDRNLPtr->parSpec = parSpec;
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->nonLinGTCascade = 3;
	bMDRNLPtr->nonLinLPCascade = 4;
	if ((bMDRNLPtr->nonLinBwidth = Init_ParArray(wxT("NonLinBwidth"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise NonLinBwidth parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->nonLinScaleG = Init_ParArray(wxT("nonLinScaleG"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise nonLinScaleG parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->comprScaleA = Init_ParArray(wxT("ComprScaleA"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise ComprScaleA parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->comprScaleB = Init_ParArray(wxT("ComprScaleB"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise ComprScaleB parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	bMDRNLPtr->comprExponent = 0.1;
	bMDRNLPtr->linGTCascade = 3;
	bMDRNLPtr->linLPCascade = 4;
	if ((bMDRNLPtr->linCF = Init_ParArray(wxT("LinCF"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise LinCF parArray structure"),
		  funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->linBwidth = Init_ParArray(wxT("LinBwidth"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise LinBwidth parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->linScaleG = Init_ParArray(wxT("linScaleG"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise linScaleG parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->theCFs = GenerateDefault_CFList(
	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS,
	  CFLIST_DEFAULT_LOW_FREQ, CFLIST_DEFAULT_HIGH_FREQ, wxT("internal_static"),
	  GetNonLinBandwidth_BasilarM_DRNL)) == NULL) {
		NotifyError(wxT("%s: could not set default CFList."), funcName);
		return(FALSE);
	}
	SetDefaultParArrayPars_BasilarM_DRNL();

	if (!SetUniParList_BasilarM_DRNL()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	bMDRNLPtr->numChannels = 0;
	bMDRNLPtr->compressionA = NULL;
	bMDRNLPtr->compressionB = NULL;
	bMDRNLPtr->nonLinearGT1 = NULL;
	bMDRNLPtr->nonLinearGT2 = NULL;
	bMDRNLPtr->linearGT = NULL;
	bMDRNLPtr->nonLinearLP = NULL;
	bMDRNLPtr->linearLP = NULL;
	bMDRNLPtr->linearF = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_DRNL(void)
{
	static const WChar *funcName = wxT("SetUniParList_BasilarM_DRNL");
	UniParPtr	pars;

	if ((bMDRNLPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BM_DRNL_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bMDRNLPtr->parList->pars;
	SetPar_UniParMgr(&pars[BM_DRNL_NONLINGTCASCADE], wxT("NL_GT_CASCADE"),
	  wxT("Nonlinear gammatone filter cascade."),
	  UNIPAR_INT,
	  &bMDRNLPtr->nonLinGTCascade, NULL,
	  (void * (*)) SetNonLinGTCascade_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_NONLINLPCASCADE], wxT("NL_LP_CASCADE"),
	  wxT("Nonlinear low-pass filter cascade."),
	  UNIPAR_INT,
	  &bMDRNLPtr->nonLinLPCascade, NULL,
	  (void * (*)) SetNonLinLPCascade_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_NONLINBWIDTH], wxT("NL_BW_VAR_FUNC"),
	  wxT("Non-linear bandwidth variable function (Hz vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->nonLinBwidth, NULL,
	  (void * (*)) SetNonLinBwidth_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_NONLINSCALEG], wxT("NL_SCALER"),
	  wxT("Nonlinear filter scale variable function (vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->nonLinScaleG, NULL,
	  (void * (*)) SetNonLinScaleG_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_COMPRSCALEA], wxT("COMP_A_VAR_FUNC"),
	  wxT("Compression A (linear) scale variable function (vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->comprScaleA, NULL,
	  (void * (*)) SetComprScaleA_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_COMPRSCALEB], wxT("COMP_B_VAR_FUNC"),
	  wxT("Compression b (gain) scale variable function (vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->comprScaleB, NULL,
	  (void * (*)) SetComprScaleB_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_COMPREXPONENT], wxT("COMP_N_EXPON"),
	  wxT("Compression exponent, n (units)."),
	  UNIPAR_REAL,
	  &bMDRNLPtr->comprExponent, NULL,
	  (void * (*)) SetComprExponent_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_LINGTCASCADE], wxT("L_GT_CASCADE"),
	  wxT("Linear gammatone filter cascade."),
	  UNIPAR_INT,
	  &bMDRNLPtr->linGTCascade, NULL,
	  (void * (*)) SetLinGTCascade_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_LINLPCASCADE], wxT("L_LP_CASCADE"),
	  wxT("Linear low-pass filter cascade."),
	  UNIPAR_INT,
	  &bMDRNLPtr->linLPCascade, NULL,
	  (void * (*)) SetLinLPCascade_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_LINCF], wxT("L_CF_VAR_FUNC"),
	  wxT("Linear CF variable function (Hz vs linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->linCF, NULL,
	  (void * (*)) SetLinCF_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_LINBWIDTH], wxT("L_BW_VAR_FUNC"),
	  wxT("Linear bandwidth variable function (Hz vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->linBwidth, NULL,
	  (void * (*)) SetLinBwidth_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_LINSCALEG], wxT("L_SCALER"),
	  wxT("Linear filter scale variable function (vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->linScaleG, NULL,
	  (void * (*)) SetLinScaleG_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BM_DRNL_THECFS], wxT("CFLIST"),
	  wxT("Centre frequency specification."),
	  UNIPAR_CFLIST,
	  &bMDRNLPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_DRNL);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_DRNL(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMDRNLPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bMDRNLPtr->parList);

}

/****************************** SetNonLinGTCascade ****************************/

/*
 * This function sets the module's nonLinGTCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNonLinGTCascade_BasilarM_DRNL(int theNonLinGTCascade)
{
	static const WChar	*funcName = wxT("SetNonLinGTCascade_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->nonLinGTCascade = theNonLinGTCascade;
	return(TRUE);

}

/****************************** SetNonLinLPCascade ****************************/

/*
 * This function sets the module's nonLinLPCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNonLinLPCascade_BasilarM_DRNL(int theNonLinLPCascade)
{
	static const WChar	*funcName = wxT("SetNonLinLPCascade_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->nonLinLPCascade = theNonLinLPCascade;
	return(TRUE);

}

/****************************** SetNonLinBwidth *******************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNonLinBwidth_BasilarM_DRNL(ParArrayPtr theNonLinBwidth)
{
	static const WChar	*funcName = wxT("SetNonLinBwidth_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theNonLinBwidth, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	if (bMDRNLPtr->theCFs) {
		bMDRNLPtr->theCFs->bandwidthMode.Func =
		  GetNonLinBandwidth_BasilarM_DRNL;
		if (!SetBandwidths_CFList(bMDRNLPtr->theCFs, wxT("internal_static"),
		  NULL)) {
			NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
			return(FALSE);
		}
	}
	bMDRNLPtr->nonLinBwidth = theNonLinBwidth;
	return(TRUE);

}

/****************************** SetNonLinScaleG *******************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNonLinScaleG_BasilarM_DRNL(ParArrayPtr theNonLinScaleG)
{
	static const WChar	*funcName = wxT("SetNonLinScaleG_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theNonLinScaleG, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->nonLinScaleG = theNonLinScaleG;
	return(TRUE);

}

/****************************** SetComprScaleA ********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetComprScaleA_BasilarM_DRNL(ParArrayPtr theComprScaleA)
{
	static const WChar	*funcName = wxT("SetComprScaleA_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theComprScaleA, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->comprScaleA = theComprScaleA;
	return(TRUE);

}

/****************************** SetComprScaleB ********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetComprScaleB_BasilarM_DRNL(ParArrayPtr theComprScaleB)
{
	static const WChar	*funcName = wxT("SetComprScaleB_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theComprScaleB, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->comprScaleB = theComprScaleB;
	return(TRUE);

}

/****************************** SetComprExponent ******************************/

/*
 * This function sets the module's comprExponent parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetComprExponent_BasilarM_DRNL(Float theComprExponent)
{
	static const WChar	*funcName = wxT("SetComprExponent_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->comprExponent = theComprExponent;
	return(TRUE);

}

/****************************** SetLinGTCascade *******************************/

/*
 * This function sets the module's linGTCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinGTCascade_BasilarM_DRNL(int theLinGTCascade)
{
	static const WChar	*funcName = wxT("SetLinGTCascade_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->linGTCascade = theLinGTCascade;
	return(TRUE);

}

/****************************** SetLinLPCascade *******************************/

/*
 * This function sets the module's linLPCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinLPCascade_BasilarM_DRNL(int theLinLPCascade)
{
	static const WChar	*funcName = wxT("SetLinLPCascade_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->linLPCascade = theLinLPCascade;
	return(TRUE);

}

/****************************** SetLinCF **************************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinCF_BasilarM_DRNL(ParArrayPtr theLinCF)
{
	static const WChar	*funcName = wxT("SetLinCF_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinCF, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->linCF = theLinCF;
	return(TRUE);

}

/****************************** SetLinBwidth **********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinBwidth_BasilarM_DRNL(ParArrayPtr theLinBwidth)
{
	static const WChar	*funcName = wxT("SetLinBwidth_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinBwidth, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->linBwidth = theLinBwidth;
	return(TRUE);

}

/****************************** SetLinScaleG **********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinScaleG_BasilarM_DRNL(ParArrayPtr theLinScaleG)
{
	static const WChar	*funcName = wxT("SetLinScaleG_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinScaleG, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->linScaleG = theLinScaleG;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_DRNL(CFListPtr theCFList)
{
	static const WChar	*funcName = wxT("SetCFList_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not correctly set."),
		  funcName);
		return(FALSE);
	}
	if (bMDRNLPtr->nonLinBwidth) {
		theCFList->bandwidthMode.Func = GetNonLinBandwidth_BasilarM_DRNL;
		if (!SetBandwidths_CFList(theCFList, wxT("internal_static"), NULL)) {
			NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
			return(FALSE);
		}
		theCFList->bParList->pars[BANDWIDTH_PAR_MODE].enabled = FALSE;
	}
	if (bMDRNLPtr->theCFs != NULL)
		Free_CFList(&bMDRNLPtr->theCFs);
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->theCFs = theCFList;
	return(TRUE);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

CFListPtr
GetCFListPtr_BasilarM_DRNL(void)
{
	static const WChar	*funcName = wxT("GetCFListPtr_BasilarM_DRNL");

	if (bMDRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(NULL);
	}
	if (bMDRNLPtr->theCFs == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly ")
		  wxT("set.  NULL returned."), funcName);
		return(NULL);
	}
	return(bMDRNLPtr->theCFs);

}

/****************************** GetNonLinBandwidth ****************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

Float
GetNonLinBandwidth_BasilarM_DRNL(BandwidthModePtr modePtr, Float theCF)
{
	return(GetFitFuncValue_BasilarM_DRNL(bMDRNLPtr->nonLinBwidth, theCF));

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_DRNL(void)
{
	DPrint(wxT("DRNL Basilar Membrane Filter Module Parameters:-\n"));
	DPrint(wxT("\tNonlinear gammatone filter cascade  = %d,\n"),
	  bMDRNLPtr->nonLinGTCascade);
	DPrint(wxT("\tNonlinear low-pass filter cascade = %d,\n"),
	  bMDRNLPtr->nonLinLPCascade);
	PrintPars_ParArray(bMDRNLPtr->nonLinBwidth);
	PrintPars_ParArray(bMDRNLPtr->nonLinScaleG);
	PrintPars_ParArray(bMDRNLPtr->comprScaleA);
	PrintPars_ParArray(bMDRNLPtr->comprScaleB);
	DPrint(wxT("\tCompression exponent  = %g,\n"), bMDRNLPtr->comprExponent);
	DPrint(wxT("\tLinear gammatone filter cascade = %d,\n"),
	  bMDRNLPtr->linGTCascade);
	DPrint(wxT("\tLinear low-pass filter cascade = %d,\n"), bMDRNLPtr->
	  linLPCascade);
	PrintPars_ParArray(bMDRNLPtr->linCF);
	PrintPars_ParArray(bMDRNLPtr->linBwidth);
	PrintPars_ParArray(bMDRNLPtr->linScaleG);
	PrintPars_CFList(bMDRNLPtr->theCFs);
	return(TRUE);

}

/************************** SetParsPointer ************************************/

/*
 * This routine sets the global parameter pointer for the module to that
 * associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_DRNL(ModulePtr theModule)
{
	static const WChar *funcName = wxT("SetParsPointer_BasilarM_DRNL");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bMDRNLPtr = (BMDRNLPtr) theModule->parsPtr;
	return(TRUE);

}

/************************** InitModule ****************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_DRNL(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_DRNL");

	if (!SetParsPointer_BasilarM_DRNL(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_DRNL(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMDRNLPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_BasilarM_DRNL;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_DRNL;
	theModule->PrintPars = PrintPars_BasilarM_DRNL;
	theModule->RunProcess = RunModel_BasilarM_DRNL;
	theModule->SetParsPointer = SetParsPointer_BasilarM_DRNL;
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
CheckData_BasilarM_DRNL(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_BasilarM_DRNL");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the coefficients for all the filters involved in
 * the DRNL composite filter.
 * It assumes that the centre frequency list is set up correctly.
 * It also assumes that the output signal has already been initialised.
 * Two nonLinearGT filters are initialised, so that each has its own state
 * vectors.
 */

BOOLN
InitProcessVariables_BasilarM_DRNL(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_BasilarM_DRNL");
	int		i, j, cFIndex;
	Float	sampleRate, centreFreq, linearFCentreFreq;
	BMDRNLPtr	p = bMDRNLPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  p->theCFs->updateFlag) {
		FreeProcessVariables_BasilarM_DRNL();
		p->linearF = Init_EarObject(wxT("NULL"));
		p->numChannels = _OutSig_EarObject(data)->numChannels;
		if (!InitSubProcessList_EarObject(data, BM_DRNL_NUM_SUB_PROCESSES)) {
			NotifyError(wxT("%s: Could not initialise %d sub-process list for ")
			  wxT("process."), funcName, BM_DRNL_NUM_SUB_PROCESSES);
			return(FALSE);
		}
		data->subProcessList[BM_DRNL_LINEARF] = p->linearF;
		if ((p->compressionA = (Float *) calloc(p->numChannels, sizeof(
		  Float))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory (compressionA)."), funcName);
		 	return(FALSE);
		}
		if ((p->compressionB = (Float *) calloc(p->numChannels, sizeof(
		  Float))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory (compressionB)."), funcName);
		 	return(FALSE);
		}
		if ((p->nonLinearGT1 = (GammaToneCoeffsPtr *) calloc(p->numChannels,
		  sizeof(GammaToneCoeffsPtr))) == NULL) {
			NotifyError(wxT("%s: Out of memory (nonLinearGT1)."), funcName);
			return(FALSE);
		}
		if ((p->nonLinearGT2 = (GammaToneCoeffsPtr *) calloc(p->numChannels,
		  sizeof(GammaToneCoeffsPtr))) == NULL) {
			NotifyError(wxT("%s: Out of memory (nonLinearGT2)."), funcName);
			return(FALSE);
		}
		if ((p->linearGT = (GammaToneCoeffsPtr *) calloc(p->numChannels,
		  sizeof(GammaToneCoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory (linearGT)."), funcName);
		 	return(FALSE);
		}
		if ((p->nonLinLPCascade > 0) && ((p->nonLinearLP = (ContButtCoeffsPtr *)
		  calloc( p->numChannels, sizeof(ContButtCoeffsPtr))) == NULL)) {
		 	NotifyError(wxT("%s: Out of memory (linearLP)."), funcName);
		 	return(FALSE);
		}
		if ((p->linLPCascade > 0) && ((p->linearLP = (ContButtCoeffsPtr *)
		  calloc(p->numChannels, sizeof(ContButtCoeffsPtr))) == NULL)) {
		 	NotifyError(wxT("%s: Out of memory (linearLP)."), funcName);
		 	return(FALSE);
		}
		sampleRate = 1.0 / _InSig_EarObject(data, 0)->dt;
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			cFIndex = i / _InSig_EarObject(data, 0)->interleaveLevel;
			centreFreq = p->theCFs->frequency[cFIndex];
			if ((p->nonLinearGT1[i] = InitGammaToneCoeffs_Filters(centreFreq,
			  p->theCFs->bandwidth[cFIndex], p->nonLinGTCascade, sampleRate)) ==
			  NULL) {
				NotifyError(wxT("%s: Could not set nonLinearGT1[%d]."),
				  funcName, i);
				return(FALSE);
			}
			if ((p->nonLinearGT2[i] = InitGammaToneCoeffs_Filters(centreFreq,
			  p->theCFs->bandwidth[cFIndex], p->nonLinGTCascade, sampleRate)) ==
			  NULL) {
				NotifyError(wxT("%s: Could not set nonLinearGT2[%d]."),
				  funcName, i);
				return(FALSE);
			}
			if (p->nonLinearLP && ((p->nonLinearLP[i] =
			  InitIIR2ContCoeffs_Filters(p->nonLinLPCascade, centreFreq,
			  _InSig_EarObject(data, 0)->dt, LOWPASS)) == NULL)) {
				NotifyError(wxT("%s: Could not set nonLinearLP[%d]."), funcName,
				  i);
				return(FALSE);
			}
			linearFCentreFreq = GetFitFuncValue_BasilarM_DRNL(p->linCF,
			  centreFreq);
			if ((p->linearGT[i] = InitGammaToneCoeffs_Filters(linearFCentreFreq,
			  GetFitFuncValue_BasilarM_DRNL(p->linBwidth, centreFreq),
			  p->linGTCascade, sampleRate)) == NULL) {
				NotifyError(wxT("%s: Could not set linearGT[%d]."), funcName,
				  i);
				return(FALSE);
			}
			if (p->linearLP && ((p->linearLP[i] = InitIIR2ContCoeffs_Filters(
			  p->linLPCascade, linearFCentreFreq, _InSig_EarObject(data, 0)->dt,
			  LOWPASS)) == NULL)) {
				NotifyError(wxT("%s: Could not set linearLP[%d]."), funcName,
				  i);
				return(FALSE);
			}
			p->compressionA[i] = GetFitFuncValue_BasilarM_DRNL(p->comprScaleA,
			  centreFreq);
			p->compressionB[i] = GetFitFuncValue_BasilarM_DRNL(p->comprScaleB,
			  centreFreq);
		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data), wxT(
		  "Frequency (Hz)"));
		SetInfoChannelLabels_SignalData(_OutSig_EarObject(data), p->theCFs->
		  frequency);
		SetInfoCFArray_SignalData(_OutSig_EarObject(data), p->theCFs->
		  frequency);
		p->updateProcessVariablesFlag = FALSE;
		p->theCFs->updateFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			for (j = 0; j < p->nonLinGTCascade *
			  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER; j++) {
				p->nonLinearGT1[i]->stateVector[j] = 0.0;
				p->nonLinearGT2[i]->stateVector[j] = 0.0;
			}
			for (j = 0; j < p->linGTCascade *
			  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER; j++)
				p->linearGT[i]->stateVector[j] = 0.0;
			for (j = 0; j < p->nonLinLPCascade *
			  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
				p->nonLinearLP[i]->state[j] = 0.0;
			for (j = 0; j < p->linLPCascade *
			  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
				p->linearLP[i]->state[j] = 0.0;
		}
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 * It just checks the first set of coefficients, and assumes that the others
 * are in the same state.
 */

BOOLN
FreeProcessVariables_BasilarM_DRNL(void)
{
	int		i;

	if (bMDRNLPtr->linearF)
		Free_EarObject(&bMDRNLPtr->linearF);
	for (i = 0; i < bMDRNLPtr->numChannels; i++) {
		if (bMDRNLPtr->nonLinearGT1)
			FreeGammaToneCoeffs_Filters(&bMDRNLPtr->nonLinearGT1[i]);
		if (bMDRNLPtr->nonLinearGT2)
			FreeGammaToneCoeffs_Filters(&bMDRNLPtr->nonLinearGT2[i]);
		if (bMDRNLPtr->linearGT)
			FreeGammaToneCoeffs_Filters(&bMDRNLPtr->linearGT[i]);
	}
	if (bMDRNLPtr->nonLinearLP)
		for (i = 0; i < bMDRNLPtr->numChannels; i++)
			FreeIIR2ContCoeffs_Filters(&bMDRNLPtr->nonLinearLP[i]);
	if (bMDRNLPtr->linearLP)
		for (i = 0; i < bMDRNLPtr->numChannels; i++)
			FreeIIR2ContCoeffs_Filters(&bMDRNLPtr->linearLP[i]);
	if (bMDRNLPtr->compressionA)
		free(bMDRNLPtr->compressionA);
	if (bMDRNLPtr->compressionB)
		free(bMDRNLPtr->compressionB);
	if (bMDRNLPtr->nonLinearGT1)
		free(bMDRNLPtr->nonLinearGT1);
	if (bMDRNLPtr->nonLinearGT2)
		free(bMDRNLPtr->nonLinearGT2);
	if (bMDRNLPtr->linearGT)
		free(bMDRNLPtr->linearGT);
	if (bMDRNLPtr->nonLinearLP)
		free(bMDRNLPtr->nonLinearLP);
	if (bMDRNLPtr->linearLP)
		free(bMDRNLPtr->linearLP);
	bMDRNLPtr->compressionA = NULL;
	bMDRNLPtr->compressionB = NULL;
	bMDRNLPtr->nonLinearGT1 = NULL;
	bMDRNLPtr->nonLinearGT2 = NULL;
	bMDRNLPtr->linearGT = NULL;
	bMDRNLPtr->nonLinearLP = NULL;
	bMDRNLPtr->linearLP = NULL;
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** ApplyScale **************************************/

/*
 * This routine applies the respective scales each channel of the signal.
 * It assumes that the signal and the parameter array have been correctly
 * initialised.
 * This function returns without scaling if the default linear scale of 1.0 is
 * set.
 */

void
ApplyScale_BasilarM_DRNL(EarObjectPtr data, SignalDataPtr signal, ParArrayPtr p)
{
	int		chan;
	Float	scale;
	ChanLen	i;
	ChanData	*dataPtr;

	if ((p->mode == GENERAL_FIT_FUNC_LINEAR1_MODE) && (p->params[0] == 1.0) &&
	  (p->params[1] == 0.0))
		return;
	for (chan = _OutSig_EarObject(data)->offset; chan < signal->numChannels;
	  chan++) {
		scale = GetFitFuncValue_BasilarM_DRNL(p, _OutSig_EarObject(data)->info.
		  cFArray[chan]);
		for (i = 0, dataPtr = signal->channel[chan]; i < signal->length; i++)
			*(dataPtr++) *= scale;
	}

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
RunModel_BasilarM_DRNL(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_BasilarM_DRNL");
	uShort	totalChannels;
	EarObjectPtr	linearF;
	SignalDataPtr	outSignal;
	BMDRNLPtr	p = bMDRNLPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_BasilarM_DRNL(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("DRNL Basilar Membrane Filtering"));
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
		totalChannels = p->theCFs->numChannels * _InSig_EarObject(data, 0)->
		  numChannels;
		if (!InitOutTypeFromInSignal_EarObject(data, totalChannels)) {
			NotifyError(wxT("%s: Output channels not initialised (%d)."),
			  funcName, totalChannels);
			return(FALSE);
		}
		if (!InitProcessVariables_BasilarM_DRNL(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		TempInputConnection_EarObject(data, p->linearF, 1);
		/*InitOutFromInSignal_EarObject(p->linearF, totalChannels);*/
		InitOutTypeFromInSignal_EarObject(p->linearF, totalChannels);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	InitOutDataFromInSignal_EarObject(data);
	linearF = data->subProcessList[BM_DRNL_LINEARF];
	InitOutDataFromInSignal_EarObject(linearF);
	outSignal = _OutSig_EarObject(data);

	/* Filter signal */
	ApplyScale_BasilarM_DRNL(data, outSignal, p->nonLinScaleG);
	GammaTone_Filters(outSignal, p->nonLinearGT1);
	ApplyScale_BasilarM_DRNL(data, outSignal, p->nonLinScaleG);
	BrokenStick1Compression2_Filters(outSignal, p->compressionA, p->
	  compressionB, p->comprExponent);
	GammaTone_Filters(outSignal, p->nonLinearGT2);
	if (p->nonLinearLP)
		IIR2Cont_Filters(outSignal, p->nonLinearLP);

	GammaTone_Filters(linearF->outSignal, p->linearGT);
	if (p->linearLP)
		IIR2Cont_Filters(linearF->outSignal, p->linearLP);

	ApplyScale_BasilarM_DRNL(data, linearF->outSignal, p->linScaleG);
	Add_SignalData(outSignal, linearF->outSignal);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

