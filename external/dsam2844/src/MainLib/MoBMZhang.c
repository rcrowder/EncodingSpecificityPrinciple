/**********************
 *
 * File:		MoBMZhang.c
 * Purpose:		Incorporates the Zhang et al. BM code
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		13 Jun 2002
 * Updated:
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtCmplxM.h"
#include "UtZhang.h"
#include "MoBMZhang.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMZhangPtr	bMZhangPtr = NULL;

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
Free_BasilarM_Zhang(void)
{
	if (bMZhangPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_Zhang();
	Free_CFList(&bMZhangPtr->cFList);
	if (bMZhangPtr->parList)
		FreeList_UniParMgr(&bMZhangPtr->parList);
	if (bMZhangPtr->parSpec == GLOBAL) {
		free(bMZhangPtr);
		bMZhangPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModelList *********************************/

/*
 * This function initialises the 'model' list array
 */

BOOLN
InitModelList_BasilarM_Zhang(void)
{
	static NameSpecifier	modeList[] = {

		{ wxT("FEED_FORWARD_NL"),	BASILARM_ZHANG_MODEL_FEED_FORWARD_NL },
		{ wxT("FEED_BACK_NL"),		BASILARM_ZHANG_MODEL_FEED_BACK_NL },
		{ wxT("SHARP_LINEAR"),		BASILARM_ZHANG_MODEL_SHARP_LINEAR },
		{ wxT("BROAD_LINEAR"),		BASILARM_ZHANG_MODEL_BROAD_LINEAR },
		{ wxT("BROAD_LINEAR_HIGH"),	BASILARM_ZHANG_MODEL_BROAD_LINEAR_HIGH },
		{ wxT(""),					BASILARM_ZHANG_MODEL_NULL },
	};
	bMZhangPtr->modelList = modeList;
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
Init_BasilarM_Zhang(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_BasilarM_Zhang");

	if (parSpec == GLOBAL) {
		if (bMZhangPtr != NULL)
			Free_BasilarM_Zhang();
		if ((bMZhangPtr = (BMZhangPtr) malloc(sizeof(
		  BMZhang))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMZhangPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bMZhangPtr->parSpec = parSpec;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->model = BASILARM_ZHANG_MODEL_FEED_FORWARD_NL;
	bMZhangPtr->species = UT_ZHANG_SPECIES_HUMAN;
	bMZhangPtr->microPaInput = GENERAL_BOOLEAN_ON;
	bMZhangPtr->nbOrder = 3;
	bMZhangPtr->wbOrder = 3;
	bMZhangPtr->cornerCP = BASILARM_ZHANG_ABS_DB - 12.0;
	bMZhangPtr->slopeCP = 0.22;
	bMZhangPtr->strenghCP = 0.08;
	bMZhangPtr->x0CP = 5.0;
	bMZhangPtr->s0CP = 12.0;
	bMZhangPtr->x1CP = 5.0;
	bMZhangPtr->s1CP = 5.0;
	bMZhangPtr->shiftCP = 7.0;
	bMZhangPtr->cutCP = 800.0;
	bMZhangPtr->kCP = 3;
	bMZhangPtr->r0 = 0.05;
	if ((bMZhangPtr->cFList = GenerateDefault_CFList(
	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS,
	  CFLIST_DEFAULT_LOW_FREQ, CFLIST_DEFAULT_HIGH_FREQ, wxT(
	    "internal_dynamic"),
	  CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {
		NotifyError(wxT("%s: could not set default CFList."), funcName);
		return(FALSE);
	}

	InitModelList_BasilarM_Zhang();
	if (!SetUniParList_BasilarM_Zhang()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_Zhang();
		return(FALSE);
	}
	bMZhangPtr->bM = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_Zhang(void)
{
	static const WChar *funcName = wxT("SetUniParList_BasilarM_Zhang");
	UniParPtr	pars;

	if ((bMZhangPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BASILARM_ZHANG_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bMZhangPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_MODEL], wxT("MODEL"),
	  wxT("Model type ('feed_forward_nl')."),
	  UNIPAR_NAME_SPEC,
	  &bMZhangPtr->model, bMZhangPtr->modelList,
	  (void * (*)) SetModel_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_SPECIES], wxT("SPECIES"),
	  wxT("Species type ('Human')."),
	  UNIPAR_NAME_SPEC,
	  &bMZhangPtr->species, SpeciesList_Utility_Zhang(0),
	  (void * (*)) SetSpecies_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_MICROPAINPUT], wxT("MICRO_PASCALS"),
	  wxT("Input expected in micro pascals instead of pascals ('on' or 'off'")
	    wxT(")."),
	  UNIPAR_BOOL,
	  &bMZhangPtr->microPaInput, NULL,
	  (void * (*)) SetMicroPaInput_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_NBORDER], wxT("N_ORDER"),
	  wxT("Order of the narrow bandpass filter (int)."),
	  UNIPAR_INT,
	  &bMZhangPtr->nbOrder, NULL,
	  (void * (*)) SetNbOrder_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_WBORDER], wxT("GAMMA_CP"),
	  wxT("Order of the wide bandpass filter (int)."),
	  UNIPAR_INT,
	  &bMZhangPtr->wbOrder, NULL,
	  (void * (*)) SetWbOrder_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_CORNERCP], wxT("CORNER_CP"),
	  wxT("Corner parameter at which the BM starts compression"),
	  UNIPAR_REAL,
	  &bMZhangPtr->cornerCP, NULL,
	  (void * (*)) SetCornerCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_SLOPECP], wxT("SLOPE_CP"),
	  wxT("Slope of compression nonlinearity in Boltzmann combination."),
	  UNIPAR_REAL,
	  &bMZhangPtr->slopeCP, NULL,
	  (void * (*)) SetSlopeCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_STRENGHCP], wxT("STRENGTH_CP"),
	  wxT("Strength of compression nonlinearity in Boltzmann combination."),
	  UNIPAR_REAL,
	  &bMZhangPtr->strenghCP, NULL,
	  (void * (*)) SetStrenghCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_X0CP], wxT("X0_CP"),
	  wxT("Parameter in Boltzman function."),
	  UNIPAR_REAL,
	  &bMZhangPtr->x0CP, NULL,
	  (void * (*)) SetX0CP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_S0CP], wxT("S0_CP"),
	  wxT("Parameter in Boltzman function."),
	  UNIPAR_REAL,
	  &bMZhangPtr->s0CP, NULL,
	  (void * (*)) SetS0CP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_X1CP], wxT("X1_CP"),
	  wxT("Parameter in Boltzman function."),
	  UNIPAR_REAL,
	  &bMZhangPtr->x1CP, NULL,
	  (void * (*)) SetX1CP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_S1CP], wxT("S1_CP"),
	  wxT("Parameter in Boltzman function."),
	  UNIPAR_REAL,
	  &bMZhangPtr->s1CP, NULL,
	  (void * (*)) SetS1CP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_SHIFTCP], wxT("SHIFT_CP"),
	  wxT("Parameter in Boltzman function."),
	  UNIPAR_REAL,
	  &bMZhangPtr->shiftCP, NULL,
	  (void * (*)) SetShiftCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_CUTCP], wxT("CUT_CP"),
	  wxT("Cut-off frequency of control-path low-path filter (Hz)."),
	  UNIPAR_REAL,
	  &bMZhangPtr->cutCP, NULL,
	  (void * (*)) SetCutCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_KCP], wxT("K_CP"),
	  wxT("Order of control-path low-pass filter."),
	  UNIPAR_INT,
	  &bMZhangPtr->kCP, NULL,
	  (void * (*)) SetKCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_R0], wxT("R0"),
	  wxT("Ratio of tau_LB (lower bound of tau_SP) to tau_narrow"),
	  UNIPAR_REAL,
	  &bMZhangPtr->r0, NULL,
	  (void * (*)) SetR0_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_CFLIST], wxT("CFLIST"),
	  wxT("Centre frequency list"),
	  UNIPAR_CFLIST,
	  &bMZhangPtr->cFList, NULL,
	  (void * (*)) SetCFList_BasilarM_Zhang);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_Zhang(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMZhangPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bMZhangPtr->parList);

}

/****************************** SetModel **************************************/

/*
 * This function sets the module's model parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModel_BasilarM_Zhang(WChar * theModel)
{
	static const WChar	*funcName = wxT("SetModel_BasilarM_Zhang");
	int		specifier;

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theModel,
		bMZhangPtr->modelList)) == BASILARM_ZHANG_MODEL_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theModel);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->model = specifier;
	return(TRUE);

}

/****************************** SetSpecies ************************************/

/*
 * This function sets the module's species parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpecies_BasilarM_Zhang(WChar * theSpecies)
{
	static const WChar	*funcName = wxT("SetSpecies_BasilarM_Zhang");
	int		specifier;

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSpecies,
		SpeciesList_Utility_Zhang(0))) ==
		  UT_ZHANG_SPECIES_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSpecies);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->species = specifier;
	return(TRUE);

}

/****************************** SetMicroPaInput *******************************/

/*
 * This function sets the module's microPaInput parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMicroPaInput_BasilarM_Zhang(WChar * theMicroPaInput)
{
	static const WChar	*funcName = wxT("SetMicroPaInput_BasilarM_Zhang");
	int		specifier;

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMicroPaInput,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName, theMicroPaInput);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->microPaInput = specifier;
	return(TRUE);

}

/****************************** SetNbOrder ************************************/

/*
 * This function sets the module's nbOrder parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNbOrder_BasilarM_Zhang(int theNbOrder)
{
	static const WChar	*funcName = wxT("SetNbOrder_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->nbOrder = theNbOrder;
	return(TRUE);

}

/****************************** SetWbOrder ************************************/

/*
 * This function sets the module's wbOrder parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetWbOrder_BasilarM_Zhang(int theWbOrder)
{
	static const WChar	*funcName = wxT("SetWbOrder_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->wbOrder = theWbOrder;
	return(TRUE);

}

/****************************** SetCornerCP ***********************************/

/*
 * This function sets the module's cornerCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCornerCP_BasilarM_Zhang(Float theCornerCP)
{
	static const WChar	*funcName = wxT("SetCornerCP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->cornerCP = theCornerCP;
	return(TRUE);

}

/****************************** SetSlopeCP ************************************/

/*
 * This function sets the module's slopeCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSlopeCP_BasilarM_Zhang(Float theSlopeCP)
{
	static const WChar	*funcName = wxT("SetSlopeCP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->slopeCP = theSlopeCP;
	return(TRUE);

}

/****************************** SetStrenghCP **********************************/

/*
 * This function sets the module's strenghCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStrenghCP_BasilarM_Zhang(Float theStrenghCP)
{
	static const WChar	*funcName = wxT("SetStrenghCP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->strenghCP = theStrenghCP;
	return(TRUE);

}

/****************************** SetX0CP ***************************************/

/*
 * This function sets the module's x0CP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetX0CP_BasilarM_Zhang(Float theX0CP)
{
	static const WChar	*funcName = wxT("SetX0CP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->x0CP = theX0CP;
	return(TRUE);

}

/****************************** SetS0CP ***************************************/

/*
 * This function sets the module's s0CP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetS0CP_BasilarM_Zhang(Float theS0CP)
{
	static const WChar	*funcName = wxT("SetS0CP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->s0CP = theS0CP;
	return(TRUE);

}

/****************************** SetX1CP ***************************************/

/*
 * This function sets the module's x1CP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetX1CP_BasilarM_Zhang(Float theX1CP)
{
	static const WChar	*funcName = wxT("SetX1CP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->x1CP = theX1CP;
	return(TRUE);

}

/****************************** SetS1CP ***************************************/

/*
 * This function sets the module's s1CP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetS1CP_BasilarM_Zhang(Float theS1CP)
{
	static const WChar	*funcName = wxT("SetS1CP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->s1CP = theS1CP;
	return(TRUE);

}

/****************************** SetShiftCP ************************************/

/*
 * This function sets the module's shiftCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetShiftCP_BasilarM_Zhang(Float theShiftCP)
{
	static const WChar	*funcName = wxT("SetShiftCP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->shiftCP = theShiftCP;
	return(TRUE);

}

/****************************** SetCutCP **************************************/

/*
 * This function sets the module's cutCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCutCP_BasilarM_Zhang(Float theCutCP)
{
	static const WChar	*funcName = wxT("SetCutCP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->cutCP = theCutCP;
	return(TRUE);

}

/****************************** SetKCP ****************************************/

/*
 * This function sets the module's kCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetKCP_BasilarM_Zhang(int theKCP)
{
	static const WChar	*funcName = wxT("SetKCP_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->kCP = theKCP;
	return(TRUE);

}

/****************************** SetR0 *****************************************/

/*
 * This function sets the module's r0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetR0_BasilarM_Zhang(Float theR0)
{
	static const WChar	*funcName = wxT("SetR0_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->r0 = theR0;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_Zhang(CFListPtr theCFList)
{
	static const WChar	*funcName = wxT("SetCFList_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not correctly set."),
		  funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(theCFList, wxT("internal_dynamic"), NULL)) {
		NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
		return(FALSE);
	}
	theCFList->bParList->pars[BANDWIDTH_PAR_MODE].enabled = FALSE;
	if (bMZhangPtr->cFList != NULL)
		Free_CFList(&bMZhangPtr->cFList);
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->cFList = theCFList;
	return(TRUE);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

CFListPtr
GetCFListPtr_BasilarM_Zhang(void)
{
	static const WChar	*funcName = wxT("GetCFListPtr_BasilarM_Zhang");

	if (bMZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMZhangPtr->cFList == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly ")
		  wxT("set. NULL returned."), funcName);
		return(NULL);
	}
	return(bMZhangPtr->cFList);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_Zhang(void)
{
	DPrint(wxT("Zhang et al. BM Module Parameters:-\n"));
	DPrint(wxT("\tModel = %s,"), bMZhangPtr->modelList[bMZhangPtr->
	  model].name);
	DPrint(wxT("\tspecies = %s \n"), SpeciesList_Utility_Zhang(bMZhangPtr->
	  species)->name);
	DPrint(wxT("\tMicro Pascal Input = %s,\n"), BooleanList_NSpecLists(
	  bMZhangPtr->microPaInput)->name);
	DPrint(wxT("\tNarrow/wide bandpass filter Orders = %d/%d\n"), bMZhangPtr->
	  nbOrder, bMZhangPtr->wbOrder);
	DPrint(wxT("\tNonlinearity corner/slope/strengh = %g/%g/%g,\n"),
	  bMZhangPtr->cornerCP, bMZhangPtr->slopeCP, bMZhangPtr->
	  strenghCP);
	DPrint(wxT("\tControl path Boltzmann parameters:\n"));
	DPrint(wxT("\t\tx0 = %g,"), bMZhangPtr->x0CP);
	DPrint(wxT("\ts0 = %g,"), bMZhangPtr->s0CP);
	DPrint(wxT("\tx1 = %g,"), bMZhangPtr->x1CP);
	DPrint(wxT("\ts1 = %g,"), bMZhangPtr->s1CP);
	DPrint(wxT("\tshift = %g,\n"), bMZhangPtr->shiftCP);
	DPrint(wxT("\tCut-off frequency of control-path low-path filter, cut = %g ")
	  wxT("(Hz)\n"), bMZhangPtr->cutCP);
	DPrint(wxT("\tOrder of control-path low-pass filter, k = %d,\n"),
	  bMZhangPtr->kCP);
	DPrint(wxT("\t Ratio of tau_LB (lower bound of tau_SP) to tau_narrow, r0 = ")
	  wxT("%g\n"), bMZhangPtr->r0);
	PrintPars_CFList(bMZhangPtr->cFList);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_Zhang(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_BasilarM_Zhang");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bMZhangPtr = (BMZhangPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_BasilarM_Zhang(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_Zhang");

	if (!SetParsPointer_BasilarM_Zhang(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_Zhang(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMZhangPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_BasilarM_Zhang;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_Zhang;
	theModule->PrintPars = PrintPars_BasilarM_Zhang;
	theModule->RunProcess = RunModel_BasilarM_Zhang;
	theModule->SetParsPointer = SetParsPointer_BasilarM_Zhang;
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
CheckData_BasilarM_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_BasilarM_Zhang");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** CochleaF2x ************************************/

/*
 * Original name: 'cochlea_f2x'.
 * Extracted from the cmpa.c code (LPO).
 */

/*/ ----------------------------------------------------------------------------
 ** Calculate the location on Basilar Membrane from best frequency
*/
Float
CochleaF2x_BasilarM_Zhang(int species,Float f)
{
  Float x;
  switch(species)
    {
    case 0: /*/human */
      x=(1.0/0.06)*log10((f/165.4)+0.88);
      break;
    default:
    case 1: /*/cat */
      x = 11.9 * log10(0.80 + f / 456.0);
      break;
    };
   return(x);
}

/****************************** CochleaX2f ************************************/

/*
 * Original name: 'cochlea_xf2'.
 * Extracted from the cmpa.c code (LPO).
 */

/* ----------------------------------------------------------------------------
 ** Calculate the best frequency from the location on basilar membrane
 */
Float
CochleaX2f_BasilarM_Zhang(int species,Float x)
{
  Float f;
  switch(species)
    {
    case 0: /*human */
      f=165.4*(pow(10,(0.06*x))-0.88);
      break;
    default:
    case 1: /*/cat */
      f = 456.0*(pow(10,x/11.9)-0.80);
      break;
    };
  return(f);
}

/****************************** GetTau ****************************************/

/*
 * Original name: get_tau
 * Orignal file: cmpa.a
 * Get TauMax, TauMin for the tuning filter. The TauMax is determined by the
 * bandwidth/Q10 of the tuning filter at low level. The TauMin is determined by
 * the gain change between high and low level
 * Also the calculation is different for different species.
 * LPO I use the DSAM 'ERBFromF_Bandwidth' function here instead of the
 * 'erbGM' function from the original.  I have added the (1.0 / 1.2) factor
 * which is where the original code differed.  I do not know where it came from.
 */

Float
GetTau_BasilarM_Zhang(int species, Float cf,int order, Float* _taumax,
  Float* _taumin, Float* _taurange)
{
  Float taumin,taumax = 0.0,taurange;
  Float ss0,cc0,ss1,cc1;
  Float Q10,bw,gain,ratio;
  Float xcf, x1000;
  gain = 20+42*log10(cf/1e3);     /*/ estimate compression gain of the filter */
  if(gain>70) gain = 70;
  if(gain<15) gain = 15;
  ratio = pow(10,(-gain/(20.0*order)));  /*/ ratio of TauMin/TauMax according
  										to the gain, order */

  /*/ Calculate the TauMax according to different species */
  switch(species)
    {
    case 1:
      /* Cat parameters: Tau0 vs. CF (from Carney & Yin 88) - only good for */
	  /* low  CFs. */
      /* Note: Tau0 is gammatone time constant for 'high' levels (80 dB rms) */
      /* parameters for cat Tau0 vs. CF */
      xcf = CochleaF2x_BasilarM_Zhang(species,cf);	/* position of cf unit;
	  													 from Liberman's map */
      x1000 = CochleaF2x_BasilarM_Zhang(species,1000);	/* position for 1
	  														Khz */
      ss0 = 6.; cc0 = 1.1; ss1 = 2.2; cc1 = 1.1;
      taumin = ( cc0 * exp( -xcf / ss0) + cc1 * exp( -xcf /ss1) ) * 1e-3;
	  /* above: in sec */
      taurange = taumin * xcf/x1000;
      taumax = taumin+taurange;
      break;
    case 0:
      /* Human Parameters: From Mike Heinz: */
      /* Bandwidths now are based on Glasberg and Moore's (1990) */
	  /* ERB=f(CF,level) equations  */
      taumax =  1.0 / (PIx2 * 1.019 * (1.0 / 1.2) * ERBFromF_Bandwidth(cf));
      break;
    case 9:
      /* Universal species from data fitting : From Xuedong Zhang,Ian
	   * (JASA 2001) */
      /* the Q10 determine the taumax(bandwidths at low level) Based on Cat*/
      Q10 = pow(10,0.4708*log10(cf/1e3)+0.4664);
      bw = cf/Q10;
      taumax = 2.0/(PIx2*bw);
    }; /*/end of switch */
  taumin =  taumax*ratio;
  taurange = taumax-taumin;
  *_taumin = taumin;
  *_taumax = taumax;
  *_taurange = taurange;
  return 0;

}

/****************************** RunGammaTone **********************************/

/*
 * Original name: runGammaTone
 * Original file: filters.c
 */

/**

   Pass the signal through the gammatone filter\\
   1. shift the signal by centeral frequency of the gamma tone filter\\
   2. low pass the shifted signal \\
   3. shift back the signal \\
   4. take the real part of the signal as output
   @author Xuedong Zhang
 */

Float
RunGammaTone_BasilarM_Zhang(TGammaTone *p, Float x)
{
	int		i,j;
	Float	out;
	Complex	c1, c2, c_phase;

	x *= p->gain;
	p->phase += p->delta_phase;

	EXP_CMPLXM(c_phase, p->phase); /*/ FREQUENCY SHIFT */
	SCALER_MULT_CMPLXM(p->gtf[0], c_phase,x);
	for( j = 1; j <= p->Order; j++) {     /*/ IIR Bilinear transformation LPF */
		ADD_CMPLXM(c1, p->gtf[j-1], p->gtfl[j-1]);
		SCALER_MULT_CMPLXM(c2, c1, p->c2LP);
		SCALER_MULT_CMPLXM(c1, p->gtfl[j], p->c1LP);
		ADD_CMPLXM(p->gtf[j], c1, c2);
	};
	CONV2CONJ_CMPLX(c_phase); /*/ FREQ SHIFT BACK UP */
	MULT_CMPLXM(c1, c_phase, p->gtf[p->Order]);
	out = c1.re;
	for(i = 0; i <= p->Order; i++)
		p->gtfl[i] = p->gtf[i];
	return(out);

}

/****************************** RunGammaTone2 *********************************/

/*
 * Original name: runGammaTone2
 * Original file: filters.c
 */

void
RunGammaTone2_BasilarM_Zhang(TGammaTone *p, const Float *in, Float *out,
  const int length)
{
  int register loopSig,loopGT;
  Float	x;
  Complex	c1,c2,c_phase;

	for(loopSig = 0; loopSig < length; loopSig++) {
		x = p->gain * in[loopSig];
		p->phase += p->delta_phase;

		EXP_CMPLXM(c_phase,p->phase); /*/ FREQUENCY SHIFT */
		SCALER_MULT_CMPLXM(p->gtf[0],c_phase, x);
		/*/ IIR Bilinear transformation LPF */
		for ( loopGT = 1; loopGT <= p->Order; loopGT++) {
			ADD_CMPLXM(c1,p->gtf[loopGT-1],p->gtfl[loopGT-1]);
			SCALER_MULT_CMPLXM(c2,c1,p->c2LP);
			SCALER_MULT_CMPLXM(c1,p->gtfl[loopGT],p->c1LP);
			ADD_CMPLXM(p->gtf[loopGT],c1,c2);
		}
		CONV2CONJ_CMPLX(c_phase); /* FREQ SHIFT BACK UP */
		MULT_CMPLXM(c1,c_phase,p->gtf[p->Order]);
		for(loopGT=0; loopGT<=p->Order;loopGT++)
			p->gtfl[loopGT] = p->gtf[loopGT];

		out[loopSig] = c1.re;
	}

}

/****************************** SetGammaToneTau *******************************/

/*
 * Original name: setGammaToneTau
 * Original file: filters.c
 */

/**

   Reset the tau of the gammatone filter\\
   it recalculate the c1 c2 used by the filtering function
 */
void
setGammaToneTau_BasilarM_Zhang(TGammaTone *p, Float tau)
{
  Float dtmp;
  p->tau = tau;
  dtmp = tau*2.0/p->tdres;
  p->c1LP = (dtmp-1)/(dtmp+1);
  p->c2LP = 1.0/(dtmp+1);

}

/****************************** SetZhangGTCoeffs ******************************/

/*
 * Original name: initgammatone
 * Original file: filters.c
 */

void
SetZhangGTCoeffs_BasilarM_Zhang(TGammaTone* res,Float _tdres,
  Float _Fshift,Float _tau,Float _gain,int _order)
{
	Float c;
	int	i;

	res->tdres = _tdres;
	res->F_shift = _Fshift;
	res->delta_phase = -TWOPI*_Fshift*_tdres;
	res->phase = 0;
	res->tau = _tau;

	c = 2.0/_tdres; /* for bilinear transformation */
	res->c1LP = (_tau*c-1)/(_tau*c+1);
	res->c2LP = 1/(_tau*c+1);
	res->gain = _gain;
	res->Order = _order;
	for( i = 0; i <= res->Order; i++)
		res->gtf[i].re = res->gtfl[i].re = res->gtf[i].im = res->gtfl[i].im =
		  0.0;
	res->Run = RunGammaTone_BasilarM_Zhang;
	res->Run2 = RunGammaTone2_BasilarM_Zhang;
	res->SetTau = setGammaToneTau_BasilarM_Zhang;

}

/****************************** InitBasilarMembrane ***************************/

/*
 * Extracted from the cmpa.c code (LPO).
 * User GetTau_BasilarM_Zhang, initGammaTone, initBoltzman
 *
 * ##### Get Basilar Membrane ########
 * 1. Get a structure of BasilarMembrane
 * 2. Specify wide band filter in the BasilarMembrane Model
 *    //WB filter not used for all model versions, but it's computed here
 *	  anyway
 * 3. Specify the OHC model in the control path
 *    3.2 Specify the NL function used in the OHC
 * 4. Specify the AfterOHC NL in the control path
 *
 * DSAM changes: L. P. O'Mard.
 * adsdb: I have removed this variable which is now encampessed in the
 * 'bMZhangPtr->cornerCP' parameter.
 */

BOOLN
InitBasilarMembrane_BasilarM_Zhang(TBasilarMembranePtr bm, int model,
  int species, Float tdres, Float cf)
{
	static const WChar *funcName = wxT("initBasilarMembrane");
	int		bmmodel = 0;
	Float	taumax,taumin,taurange; /* general */
	Float	x, centerfreq,tauwb,tauwbmin,dtmp,wb_gain; /* for wb */
	Float	dc,R,minR; /* for afterohc */
	TNonLinear *p;

	if (!bm) {
		NotifyError(wxT("%s: Basilar membrane structure not initialised."),
		  funcName);
		return(FALSE);
	}
	/* Model numbers as used in ARLO Heinz et al., Fig. 4 */
	if(model == 1)
		bmmodel = FeedForward_NL;
	else if(model == 2)
		bmmodel = FeedBack_NL;
	else if(model == 3)
		bmmodel = Sharp_Linear;
	else if(model == 4)
		bmmodel = Broad_Linear;
	else if(model == 5)
		bmmodel = Broad_Linear_High;
	bm->bmmodel = bmmodel;
	bm->tdres = tdres;

	/*
	*  Determine taumax,taumin,order here
	*/
	/* bm->Run2 = Run2BasilarMembrane; */

	bm->bmorder = bMZhangPtr->nbOrder;
	GetTau_BasilarM_Zhang(species, cf, bm->bmorder, &taumax, &taumin,
	  &taurange);

	bm->TauMax = taumax;
	bm->TauMin = taumin;
	if(bm->bmmodel&Broad_ALL)
		bm->tau = taumin;
	else
		bm->tau = taumax;

	SetZhangGTCoeffs_BasilarM_Zhang(&(bm->bmfilter), tdres, cf, bm->tau,
	  UT_ZHANG_GAIN(bMZhangPtr->microPaInput), bm->bmorder);
	SetZhangGTCoeffs_BasilarM_Zhang(&(bm->gfagain), tdres, cf, taumin, 1.0 /* *
	 BASILARM_ZHANG_FILTER_GAIN */, 1);
	/*
	* Get Wbfilter parameters
	*/
	x = CochleaF2x_BasilarM_Zhang(species,cf);
	/* shift the center freq Qing use 1.1 shift */
	centerfreq = CochleaX2f_BasilarM_Zhang(species,x+1.2);
	bm->wborder = bMZhangPtr->wbOrder;
	tauwb = taumin+0.2*(taumax-taumin);
	tauwbmin = tauwb/taumax*taumin;
	dtmp = tauwb*PIx2*(centerfreq-cf);
	wb_gain = pow((1+dtmp*dtmp), bm->wborder/2.0);
	bm->TauWB = tauwb;
	bm->TauWBMin = tauwbmin;
	SetZhangGTCoeffs_BasilarM_Zhang(&(bm->wbfilter), tdres, centerfreq,
	  tauwb, wb_gain, bm->wborder);

	bm->A = (tauwb / taumax - tauwbmin / taumin) / (taumax - taumin);
	bm->B= (taumin * taumin * tauwb - taumax * taumax * tauwbmin) / (taumax *
	  taumin * (taumax - taumin));

	/*
	* Init OHC model
	*/
	/* LPO absdb = 20;*/ /* The value that the BM starts compression */
	InitLowPass_Utility_Zhang(&(bm->ohc.hclp), tdres, bMZhangPtr->
	  cutCP, 1.0, bMZhangPtr->kCP);
	/* This is now use in both Human & Cat MODEL */
	/*/ parameter into boltzman is corner,slope,strength,x0,s0,x1,s1,asym
	// The corner determines the level that BM have compressive nonlinearity */
	/*/set OHC Nonlinearity as boltzman function combination */
	init_boltzman(&(bm->ohc.hcnl), bMZhangPtr->cornerCP, bMZhangPtr->
	  slopeCP, bMZhangPtr->strenghCP, bMZhangPtr->x0CP,
	  bMZhangPtr->s0CP, bMZhangPtr->x1CP, bMZhangPtr->s1CP,
	  bMZhangPtr->shiftCP);
	bm->ohc.Run = RunHairCell;
	/*bm->ohc.Run2 = RunHairCell2; */

	/*
	* Init AfterOHC model
	*/
	p = &(bm->afterohc);
	dc = (bMZhangPtr->shiftCP - 1) / (bMZhangPtr->shiftCP + 1.0) /
	  2.0;
	dc -= 0.05;
	minR = bMZhangPtr->r0;
	p->TauMax = taumax;
	p->TauMin = taumin;
	R = taumin/taumax;
	if (R<minR)
		p->minR = 0.5 * R;
	else
		p->minR = minR;
	p->A = p->minR / (1.0 - p->minR); /* makes x = 0; output = 1; */
	p->dc = dc;
	R = R-p->minR;
	/* This is for new nonlinearity */
	p->s0 = -dc / log(R / (1.0 - p->minR));
	p->Run = RunAfterOhcNL_Utility_Zhang;
	p->Run2 = RunAfterOhcNL2_Utility_Zhang;
	return (TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_BasilarM_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_BasilarM_Zhang");
	int		i, cFIndex;
	BMZhangPtr	p = bMZhangPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || p->cFList->
	  updateFlag) {
		/*** Additional update flags can be added to above line ***/
		FreeProcessVariables_BasilarM_Zhang();
		if ((p->bM = (TBasilarMembrane *) calloc(_OutSig_EarObject(data)->
		  numChannels, sizeof(TBasilarMembrane))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for coefficients array."),
			  funcName);
		 	return(FALSE);
		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data), wxT("Frequency ")
		  wxT("(Hz)"));
		SetInfoChannelLabels_SignalData(_OutSig_EarObject(data),
		   p->cFList->frequency);
		SetInfoCFArray_SignalData(_OutSig_EarObject(data), p->cFList->
		  frequency);
		p->updateProcessVariablesFlag = FALSE;
		p->cFList->updateFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			cFIndex = i / _InSig_EarObject(data, 0)->interleaveLevel;
			if (!InitBasilarMembrane_BasilarM_Zhang(&p->bM[i], p->model +
			  1, p->species, _OutSig_EarObject(data)->dt, p->cFList->frequency[
			  cFIndex])) {
				NotifyError(wxT("%s: Could not initialised BM for channel ")
				  wxT("(%d)."), funcName, i);
				FreeProcessVariables_BasilarM_Zhang();
				return(FALSE);
			}
		}
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_BasilarM_Zhang(void)
{
	if (bMZhangPtr->bM) {
		free(bMZhangPtr->bM);
		bMZhangPtr->bM = NULL;
	}
	return(TRUE);

}

/****************************** Run2BasilarMembrane ***************************/

/*
 * Extracted from the cmpa.c code (LPO).
 */

void
Run2BasilarMembrane_BasilarM_Zhang(TBasilarMembrane *bm, const Float *in,
  Float *out, const int length)
{
	register int i;
	Float wb_gain,dtmp,taunow;
	Float wbout,ohcout;
	Float x, x1,out1;

	for (i = 0; i < length; i++) {
		x = in[i];
		/*/ pass the signal through the tuning filter */
		x1 = bm->bmfilter.Run(&(bm->bmfilter),x);
		switch(bm->bmmodel){
		case Broad_Linear_High: /* /adjust the gain of the tuning filter */
			out1 = x1*pow((bm->TauMin/bm->TauMax),bm->bmfilter.Order);
			break;
		case FeedBack_NL:
			/*/get the output of the tuning filter as the control signal */
			wbout = x1;
			/*/ pass the control signal through OHC model */
			ohcout = bm->ohc.Run(&(bm->ohc),wbout);
			/*/ pass the control signal through nonliearity after OHC */
			bm->tau = bm->afterohc.Run(&(bm->afterohc),ohcout);
			/*/ set the tau of the tuning filter */
			bm->bmfilter.SetTau(&(bm->bmfilter),bm->tau);
			/*  Gain Control of the tuning filter */
			out1 = pow((bm->tau/bm->TauMax),bm->bmfilter.Order)*x1;
			break;
		case FeedForward_NL:
			/*/get the output of the wide-band pass as the control signal */
			wbout = bm->wbfilter.Run(&(bm->wbfilter),x);
			/*/ scale the tau for wide band filter in control path */
			taunow = bm->A*bm->tau*bm->tau-bm->B*bm->tau;

			/*/set the tau of the wide-band filter*/
			bm->wbfilter.SetTau(&(bm->wbfilter),taunow);
			/*/ normalize the gain of the wideband pass filter as 0dB at CF */
			dtmp = taunow*PIx2*(bm->wbfilter.F_shift-bm->bmfilter.F_shift);
			wb_gain = pow((1+dtmp*dtmp), bm->wbfilter.Order/2.0);
			bm->wbfilter.gain = wb_gain * UT_ZHANG_GAIN(bMZhangPtr->
			  microPaInput);

			/*/ pass the control signal through OHC model*/
			ohcout = bm->ohc.Run(&(bm->ohc),wbout);
			/*/ pass the control signal through nonliearity after OHC */
			bm->tau = bm->afterohc.Run(&(bm->afterohc),ohcout);
			/*/ set the tau of the tuning filter */
			bm->bmfilter.SetTau(&(bm->bmfilter),bm->tau);
			/*/ Gain Control of the tuning filter */
			out1 = pow((bm->tau/bm->TauMax),bm->bmfilter.Order)*x1;
			break;
		default:
		case Sharp_Linear:
		case Broad_Linear:
			/* / if linear model, needn't get the control signal */
			out1 = x1;
			break;
		};
		out[i] = out1;
	};
	bm->gfagain.Run2(&(bm->gfagain),out,out,length);

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
RunModel_BasilarM_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_BasilarM_Zhang");
	register ChanData	 *inPtr, *outPtr;
	uShort	totalChannels;
	int		chan;
	SignalDataPtr	outSignal;
	BMZhangPtr	p = bMZhangPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_BasilarM_Zhang(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Zhang et al. Non-linear BM ")
		  wxT("filtering"));

		totalChannels = p->cFList->numChannels * _InSig_EarObject(data, 0)->
		  numChannels;
		if (!InitOutTypeFromInSignal_EarObject(data, totalChannels)) {
			NotifyError(wxT("%s: Cannot initialise output channel."), funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_BasilarM_Zhang(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	/*InitOutDataFromInSignal_EarObject(data); - not needed */
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels;  chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan % _InSig_EarObject(
		  data, 0)->interleaveLevel];
		outPtr = outSignal->channel[chan];
		Run2BasilarMembrane_BasilarM_Zhang(&p->bM[chan], inPtr, outPtr,
		  outSignal->length);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

