/**********************
 *
 * File:		MoHCRPSham3StVIn.c
 * Purpose:		This module contains a revised model for the Shamme hair cell
 *				receptor potential: Shamm, S. A. Chadwick R. S. Wilbur W. J.
 *				Morrish K. A. and Rinzel J.(1986) "A biophysical model of
 *				cochlear processing: Intensity dependence of pure tone
 *				responses", J. Acoust. Soc. Am. [80], pp 133-145.
 * Comments:	Written using ModuleProducer version 1.3.2 (Mar 27 2001).
 * Author:		C. J. Sumner
 * Created:		20 Aug 2001
 * Updated:
 * Copyright:	(c) 2001, 2010 Lowel P. O'Mard
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
#include "FiParFile.h"
#include "UtString.h"
#include "MoHCRPSham3StVIn.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

Sham3StVInPtr	sham3StVInPtr = NULL;

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
Free_IHCRP_Shamma3StateVelIn(void)
{
	if (sham3StVInPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHCRP_Shamma3StateVelIn();
	if (sham3StVInPtr->parList)
		FreeList_UniParMgr(&sham3StVInPtr->parList);
	if (sham3StVInPtr->parSpec == GLOBAL) {
		free(sham3StVInPtr);
		sham3StVInPtr = NULL;
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
Init_IHCRP_Shamma3StateVelIn(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IHCRP_Shamma3StateVelIn");

	if (parSpec == GLOBAL) {
		if (sham3StVInPtr != NULL)
			Free_IHCRP_Shamma3StateVelIn();
		if ((sham3StVInPtr = (Sham3StVInPtr) malloc(sizeof(Sham3StVIn))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (sham3StVInPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	sham3StVInPtr->parSpec = parSpec;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->endocochlearPot_Et = 0.1;
	sham3StVInPtr->reversalPot_Ek = -0.07045;
	sham3StVInPtr->reversalPotCorrection = 0.04;
	sham3StVInPtr->totalCapacitance_C = 6e-12;
	sham3StVInPtr->restingConductance_G0 = 1.974e-09;
	sham3StVInPtr->kConductance_Gk = 1.8e-08;
	sham3StVInPtr->maxMConductance_Gmax = 8e-09;
	sham3StVInPtr->ciliaTimeConst_tc = 0.00213;
	sham3StVInPtr->ciliaCouplingGain_C = 16;
	sham3StVInPtr->referencePot = 0.0;
	sham3StVInPtr->sensitivity_s0 = 8.5e-08;
	sham3StVInPtr->sensitivity_s1 = 5e-09;
	sham3StVInPtr->offset_u0 = 7e-09;
	sham3StVInPtr->offset_u1 = 7e-09;

	if (!SetUniParList_IHCRP_Shamma3StateVelIn()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHCRP_Shamma3StateVelIn();
		return(FALSE);
	}
	sham3StVInPtr->lastCiliaDisplacement_u = NULL;
	sham3StVInPtr->lastOutput = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHCRP_Shamma3StateVelIn(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHCRP_Shamma3StateVelIn");
	UniParPtr	pars;

	if ((sham3StVInPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_SHAMMA3STATEVELIN_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = sham3StVInPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_ENDOCOCHLEARPOT_ET], wxT(
	  "E_T"),
	  wxT("Endocochlear potential, Et (V)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->endocochlearPot_Et, NULL,
	  (void * (*)) SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_REVERSALPOT_EK], wxT("E_K"),
	  wxT("Reversal potential, Ek (V)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->reversalPot_Ek, NULL,
	  (void * (*)) SetReversalPot_Ek_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_REVERSALPOTCORRECTION],
	  wxT("RP_CORRECTION"),
	  wxT("Reversal potential correction, Rp/(Rt+Rp)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->reversalPotCorrection, NULL,
	  (void * (*)) SetReversalPotCorrection_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_TOTALCAPACITANCE_C],
	  wxT("C_TOTAL"),
	  wxT("Total capacitance, C = Ca + Cb (F)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->totalCapacitance_C, NULL,
	  (void * (*)) SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_RESTINGCONDUCTANCE_G0], wxT(
	  "G0"),
	  wxT("Resting conductance, G0 (S)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->restingConductance_G0, NULL,
	  (void * (*)) SetRestingConductance_G0_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_KCONDUCTANCE_GK], wxT("G_K"),
	  wxT("Potassium conductance, Gk (S = Siemens)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->kConductance_Gk, NULL,
	  (void * (*)) SetKConductance_Gk_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_MAXMCONDUCTANCE_GMAX],
	  wxT("G_MAXC"),
	  wxT("Maximum mechanical conductance, Gmax (S)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->maxMConductance_Gmax, NULL,
	  (void * (*)) SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_CILIATIMECONST_TC], wxT(
	  "T_C"),
	  wxT("Cilia/BM time constant (s)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->ciliaTimeConst_tc, NULL,
	  (void * (*)) SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_CILIACOUPLINGGAIN_C],
	  wxT("GAIN_C"),
	  wxT("Cilia/BM coupling gain, C (dB)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->ciliaCouplingGain_C, NULL,
	  (void * (*)) SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_REFERENCEPOT], wxT(
	  "REF_POT"),
	  wxT("Reference potential (V)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->referencePot, NULL,
	  (void * (*)) SetReferencePot_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_SENSITIVITY_S0], wxT("S0"),
	  wxT("Sensitivity constant, S0 (/m)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->sensitivity_s0, NULL,
	  (void * (*)) SetSensitivity_s0_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_SENSITIVITY_S1], wxT("S1"),
	  wxT("Sensitivity constant, S1 (/m)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->sensitivity_s1, NULL,
	  (void * (*)) SetSensitivity_s1_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_OFFSET_U0], wxT("U0"),
	  wxT("Offset constant, U0 (m)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->offset_u0, NULL,
	  (void * (*)) SetOffset_u0_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_OFFSET_U1], wxT("U1"),
	  wxT("Offset constant, U1 (m)."),
	  UNIPAR_REAL,
	  &sham3StVInPtr->offset_u1, NULL,
	  (void * (*)) SetOffset_u1_IHCRP_Shamma3StateVelIn);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHCRP_Shamma3StateVelIn(void)
{
	static const WChar	*funcName = wxT(
	  "GetUniParListPtr_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (sham3StVInPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(sham3StVInPtr->parList);

}

/****************************** SetEndocochlearPot_Et *************************/

/*
 * This function sets the module's endocochlearPot_Et parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn(Float theEndocochlearPot_Et)
{
	static const WChar	*funcName =
	  wxT("SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->endocochlearPot_Et = theEndocochlearPot_Et;
	return(TRUE);

}

/****************************** SetReversalPot_Ek *****************************/

/*
 * This function sets the module's reversalPot_Ek parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReversalPot_Ek_IHCRP_Shamma3StateVelIn(Float theReversalPot_Ek)
{
	static const WChar	*funcName =
	  wxT("SetReversalPot_Ek_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->reversalPot_Ek = theReversalPot_Ek;
	return(TRUE);

}

/****************************** SetReversalPotCorrection **********************/

/*
 * This function sets the module's reversalPotCorrection parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReversalPotCorrection_IHCRP_Shamma3StateVelIn(Float
  theReversalPotCorrection)
{
	static const WChar	*funcName =
	  wxT("SetReversalPotCorrection_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->reversalPotCorrection = theReversalPotCorrection;
	return(TRUE);

}

/****************************** SetTotalCapacitance_C *************************/

/*
 * This function sets the module's totalCapacitance_C parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn(Float theTotalCapacitance_C)
{
	static const WChar	*funcName =
	  wxT("SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->totalCapacitance_C = theTotalCapacitance_C;
	return(TRUE);

}

/****************************** SetRestingConductance_G0 **********************/

/*
 * This function sets the module's restingConductance_G0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingConductance_G0_IHCRP_Shamma3StateVelIn(Float
  theRestingConductance_G0)
{
	static const WChar	*funcName =
	  wxT("SetRestingConductance_G0_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->restingConductance_G0 = theRestingConductance_G0;
	return(TRUE);

}

/****************************** SetKConductance_Gk ****************************/

/*
 * This function sets the module's kConductance_Gk parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetKConductance_Gk_IHCRP_Shamma3StateVelIn(Float theKConductance_Gk)
{
	static const WChar	*funcName =
	  wxT("SetKConductance_Gk_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->kConductance_Gk = theKConductance_Gk;
	return(TRUE);

}

/****************************** SetMaxMConductance_Gmax ***********************/

/*
 * This function sets the module's maxMConductance_Gmax parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn(Float theMaxMConductance_Gmax)
{
	static const WChar	*funcName =
	  wxT("SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->maxMConductance_Gmax = theMaxMConductance_Gmax;
	return(TRUE);

}

/****************************** SetCiliaTimeConst_tc **************************/

/*
 * This function sets the module's ciliaTimeConst_tc parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn(Float theCiliaTimeConst_tc)
{
	static const WChar	*funcName =
	  wxT("SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->ciliaTimeConst_tc = theCiliaTimeConst_tc;
	return(TRUE);

}

/****************************** SetCiliaCouplingGain_C ************************/

/*
 * This function sets the module's ciliaCouplingGain_C parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn(Float theCiliaCouplingGain_C)
{
	static const WChar	*funcName =
	  wxT("SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->ciliaCouplingGain_C = theCiliaCouplingGain_C;
	return(TRUE);

}

/****************************** SetReferencePot *******************************/

/*
 * This function sets the module's referencePot parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReferencePot_IHCRP_Shamma3StateVelIn(Float theReferencePot)
{
	static const WChar	*funcName = wxT(
	  "SetReferencePot_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->referencePot = theReferencePot;
	return(TRUE);

}

/****************************** SetSensitivity_s0 *****************************/

/*
 * This function sets the module's sensitivity_s0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSensitivity_s0_IHCRP_Shamma3StateVelIn(Float theSensitivity_s0)
{
	static const WChar	*funcName =
	  wxT("SetSensitivity_s0_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->sensitivity_s0 = theSensitivity_s0;
	return(TRUE);

}

/****************************** SetSensitivity_s1 *****************************/

/*
 * This function sets the module's sensitivity_s1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSensitivity_s1_IHCRP_Shamma3StateVelIn(Float theSensitivity_s1)
{
	static const WChar	*funcName =
	  wxT("SetSensitivity_s1_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->sensitivity_s1 = theSensitivity_s1;
	return(TRUE);

}

/****************************** SetOffset_u0 **********************************/

/*
 * This function sets the module's offset_u0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOffset_u0_IHCRP_Shamma3StateVelIn(Float theOffset_u0)
{
	static const WChar	*funcName = wxT("SetOffset_u0_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->offset_u0 = theOffset_u0;
	return(TRUE);

}

/****************************** SetOffset_u1 **********************************/

/*
 * This function sets the module's offset_u1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOffset_u1_IHCRP_Shamma3StateVelIn(Float theOffset_u1)
{
	static const WChar	*funcName = wxT("SetOffset_u1_IHCRP_Shamma3StateVelIn");

	if (sham3StVInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->offset_u1 = theOffset_u1;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_IHCRP_Shamma3StateVelIn(void)
{
	DPrint(wxT("Modified Shamma Receptor Potential Module  Module Parameters:-")
	  wxT("\n"));
	DPrint(wxT("\tEndocochlear potential, Et = %g V,\n"), sham3StVInPtr->
	  endocochlearPot_Et);
	DPrint( wxT("\tReversal potential, Ek = %g V,\n"), sham3StVInPtr->
	  reversalPot_Ek);
	DPrint(wxT("\tReversal potential correction Rp/(Rt+Rp) = %g,\n"),
	  sham3StVInPtr->reversalPotCorrection);
	DPrint(wxT("\tTotal capacitance, Cm = %g F,\n"), sham3StVInPtr->
	  totalCapacitance_C);
	DPrint(wxT("\tResting conductance, G0 = %g S,\n"), sham3StVInPtr->
	  restingConductance_G0);
	DPrint(wxT("\tPotassium conductance, Gk = %g S,\n"), sham3StVInPtr->
	  kConductance_Gk);
	DPrint(wxT("\tMaximum mechanically sensitive Conductance, Gmax = %g S,\n"),
	  sham3StVInPtr->maxMConductance_Gmax);
	DPrint(wxT("\tBM/Cilia: time constant = %g ms,"), sham3StVInPtr->
	  ciliaTimeConst_tc);
	DPrint(wxT("\tGain = %g dB,\n"), sham3StVInPtr->ciliaCouplingGain_C);
	DPrint(wxT("\tReference potential = %g V,\n"), sham3StVInPtr->referencePot);
	DPrint(wxT("\tTransduction function 0:\n\t\tSensitivity, s0 = %g (/m), "),
	  sham3StVInPtr->sensitivity_s0);
	DPrint(wxT("\tOffset, u0 = %g (m)\n"), sham3StVInPtr->offset_u0  );
	DPrint(wxT("\tTransduction function 1:\n\t\tSensitivity, s1 = %g (/m),"),
	  sham3StVInPtr->sensitivity_s1);
	DPrint(wxT("\tOffset, u1 = %g (m)\n"), sham3StVInPtr->offset_u1 );
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHCRP_Shamma3StateVelIn(ModulePtr theModule)
{
	static const WChar	*funcName = wxT(
	  "SetParsPointer_IHCRP_Shamma3StateVelIn");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	sham3StVInPtr = (Sham3StVInPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_IHCRP_Shamma3StateVelIn(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHCRP_Shamma3StateVelIn");

	if (!SetParsPointer_IHCRP_Shamma3StateVelIn(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_Shamma3StateVelIn(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = sham3StVInPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHCRP_Shamma3StateVelIn;
	theModule->GetUniParListPtr = GetUniParListPtr_IHCRP_Shamma3StateVelIn;
	theModule->PrintPars = PrintPars_IHCRP_Shamma3StateVelIn;
	theModule->RunProcess = RunModel_IHCRP_Shamma3StateVelIn;
	theModule->SetParsPointer = SetParsPointer_IHCRP_Shamma3StateVelIn;
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
CheckData_IHCRP_Shamma3StateVelIn(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IHCRP_Shamma3StateVelIn");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_IHCRP_Shamma3StateVelIn(EarObjectPtr data)
{
	static const WChar	*funcName =
	  wxT("InitProcessVariables_IHCRP_Shamma3StateVelIn");
	int		i;
	Float	restingPotential_V0;
	Sham3StVInPtr p = sham3StVInPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {

			FreeProcessVariables_IHCRP_Shamma3StateVelIn();
			if ((p->lastOutput = (Float *) calloc(_OutSig_EarObject(data)->numChannels,
			  sizeof(Float))) == NULL) {
				NotifyError(wxT("%s: Out of memory for 'lastOutput'."),
				  funcName);
				return(FALSE);
			}
			if ((p->lastCiliaDisplacement_u = (Float *)
				calloc(_OutSig_EarObject(data)->numChannels, sizeof(Float))) == NULL) {
				NotifyError(wxT("%s: Out of memory for ")
				  wxT("'lastCiliaDisplacement_u'."), funcName);
				return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}

		restingPotential_V0 = (p->restingConductance_G0 *
		  p->endocochlearPot_Et + p->kConductance_Gk * (p->reversalPot_Ek +
		  p->endocochlearPot_Et * p->reversalPotCorrection)) /
		  (p->restingConductance_G0 + p->kConductance_Gk);

		if (data->timeIndex == PROCESS_START_TIME) {
			for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
				p->lastOutput[i] = restingPotential_V0;
				p->lastCiliaDisplacement_u[i] = 0.0;
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
FreeProcessVariables_IHCRP_Shamma3StateVelIn(void)
{
	if (sham3StVInPtr->lastOutput != NULL) {
		free(sham3StVInPtr->lastOutput);
		sham3StVInPtr->lastOutput = NULL;
	}
	if (sham3StVInPtr->lastCiliaDisplacement_u != NULL) {
		free(sham3StVInPtr->lastCiliaDisplacement_u);
		sham3StVInPtr->lastCiliaDisplacement_u = NULL;
	}
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

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
RunModel_IHCRP_Shamma3StateVelIn(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_IHCRP_Shamma3StateVelIn");
	register ChanData	 *inPtr, *outPtr;
	int	chan;
	ChanLen	i;
	Float	conductance_G, potential_V;
	Float	ciliaDisplacement_u, ciliaAct;
	SignalDataPtr	outSignal;
	Sham3StVInPtr p = sham3StVInPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_IHCRP_Shamma3StateVelIn(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
 		SetProcessName_EarObject(data, wxT("Modified Shamma hair cell receptor ")
		  wxT("potential"));

		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHCRP_Shamma3StateVelIn(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}

		p->dtOverC = _OutSig_EarObject(data)->dt / p->totalCapacitance_C;
		p->gkEpk = p->kConductance_Gk * (p->reversalPot_Ek +
		  p->endocochlearPot_Et * p->reversalPotCorrection);
		ciliaAct = 1.0 / (1.0 + exp(p->offset_u0 / p->sensitivity_s0) *
		  (1.0 + exp(p->offset_u1 / p->sensitivity_s1)));
		p->leakageConductance_Ga = p->restingConductance_G0 -
		  p->maxMConductance_Gmax * ciliaAct;

		p->cGaindt = pow(10.0, p->ciliaCouplingGain_C / 20.0) * data->
		  outSignal->dt;
		p->dtOverTc = _OutSig_EarObject(data)->dt / p->ciliaTimeConst_tc;
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		ciliaDisplacement_u = p->lastCiliaDisplacement_u[chan];
		potential_V = p->lastOutput[chan];
		for (i = 0; i < outSignal->length; i++, inPtr++, outPtr++) {
			ciliaDisplacement_u += p->cGaindt * (*inPtr ) -
			  ciliaDisplacement_u * p->dtOverTc;
			ciliaAct = 1.0 / (1.0 + exp((p->offset_u0 - ciliaDisplacement_u) /
			  p->sensitivity_s0) * (1.0 + exp((p->offset_u1 -
			  ciliaDisplacement_u) / p->sensitivity_s1)));
			conductance_G = p->maxMConductance_Gmax * ciliaAct +
			  p->leakageConductance_Ga;
			*outPtr = (ChanData) (potential_V - p->dtOverC * (conductance_G *
			  (potential_V - p->endocochlearPot_Et) + p->kConductance_Gk *
			  potential_V - p->gkEpk));
			potential_V = *outPtr;
		}
		p->lastCiliaDisplacement_u[chan] = ciliaDisplacement_u;
		p->lastOutput[chan] = potential_V;
		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++)
			*outPtr++ += p->referencePot;
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
