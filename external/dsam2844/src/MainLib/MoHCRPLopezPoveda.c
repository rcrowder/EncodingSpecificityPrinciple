/**********************
 *
 * File:		MoHCRPLopezPoveda.c
 * Purpose:		This module is the hair-cell receptor potential from
 * 				Lopez-Poveda E.A., Eustaquio-Martın A. (2006) "A biophysical
 *				model of the inner hair cell: the contribution of potassium
 *				current to peripheral compression", J. Assoc. Res. Otolaryngol.,
 *				7, 218–235.
 * Comments:	Written using ModuleProducer version 1.6.0 (Jul 14 2008).
 *				The BM-cilia coupling process was added to this module to make
 * 				it equivalent to the previous HCRP_... modules.  IT can be
 * 				turned-off to get the IHC output as produced in the the paper.
 * Author:		L. P. O'Mard
 * Created:		14 Jul 2008
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
#include "GeNSpecLists.h"
#include "UtString.h"
#include "MoHCRPLopezPoveda.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

LopezPovedaPtr	lopezPovedaPtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** FreeLabels ************************************/

/*
 * This function releases the memory allocated for the list of current labels.
 */

void
FreeLabels_IHCRP_LopezPoveda(void)
{
	int		i;

	if (lopezPovedaPtr->label) {
		for (i = 0; i < lopezPovedaPtr->numCurrents; i++)
			free(lopezPovedaPtr->label[i]);
		free(lopezPovedaPtr->label);
		lopezPovedaPtr->label = NULL;
	}
}

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_IHCRP_LopezPoveda(void)
{
	/* static const WChar	*funcName = wxT("Free_IHCRP_LopezPoveda"); */

	if (lopezPovedaPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHCRP_LopezPoveda();
	FreeLabels_IHCRP_LopezPoveda();
	if (lopezPovedaPtr->Ek) {
		free(lopezPovedaPtr->Ek);
		lopezPovedaPtr->Ek = NULL;
	}
	if (lopezPovedaPtr->G) {
		free(lopezPovedaPtr->G);
		lopezPovedaPtr->G = NULL;
	}
	if (lopezPovedaPtr->V1) {
		free(lopezPovedaPtr->V1);
		lopezPovedaPtr->V1 = NULL;
	}
	if (lopezPovedaPtr->vS1) {
		free(lopezPovedaPtr->vS1);
		lopezPovedaPtr->vS1 = NULL;
	}
	if (lopezPovedaPtr->V2) {
		free(lopezPovedaPtr->V2);
		lopezPovedaPtr->V2 = NULL;
	}
	if (lopezPovedaPtr->vS2) {
		free(lopezPovedaPtr->vS2);
		lopezPovedaPtr->vS2 = NULL;
	}
	if (lopezPovedaPtr->tau1Max) {
		free(lopezPovedaPtr->tau1Max);
		lopezPovedaPtr->tau1Max = NULL;
	}
	if (lopezPovedaPtr->A1) {
		free(lopezPovedaPtr->A1);
		lopezPovedaPtr->A1 = NULL;
	}
	if (lopezPovedaPtr->B1) {
		free(lopezPovedaPtr->B1);
		lopezPovedaPtr->B1 = NULL;
	}
	if (lopezPovedaPtr->tau1Min) {
		free(lopezPovedaPtr->tau1Min);
		lopezPovedaPtr->tau1Min = NULL;
	}
	if (lopezPovedaPtr->tau2Max) {
		free(lopezPovedaPtr->tau2Max);
		lopezPovedaPtr->tau2Max = NULL;
	}
	if (lopezPovedaPtr->A2) {
		free(lopezPovedaPtr->A2);
		lopezPovedaPtr->A2 = NULL;
	}
	if (lopezPovedaPtr->B2) {
		free(lopezPovedaPtr->B2);
		lopezPovedaPtr->B2 = NULL;
	}
	if (lopezPovedaPtr->tau2Min) {
		free(lopezPovedaPtr->tau2Min);
		lopezPovedaPtr->tau2Min = NULL;
	}
	if (lopezPovedaPtr->parList)
		FreeList_UniParMgr(&lopezPovedaPtr->parList);
	if (lopezPovedaPtr->parSpec == GLOBAL) {
		free(lopezPovedaPtr);
		lopezPovedaPtr = NULL;
	}
	return(TRUE);

}

/****************************** SetDefaultNumCurrentsArrays *******************/

/*
 * This function sets the default arrays and array values for the
 * 'numCurrents' variable.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetDefaultNumCurrentsArrays_IHCRP_LopezPoveda(void)
{
	static const WChar	*funcName = wxT("SetDefaultNumCurrentsArrays_IHCRP_LopezPoveda");
	int		i;
	WChar *label[] = {wxT("Fast"), wxT("Slow")};
	Float	Ek[] = {-78.0e-3, -75.0e-3};
	Float	G[] = {30.72e-9, 28.71e-9};
	Float	V1[] = {-43.20e-3, -52.22e-3};
	Float	vS1[] = {11.99e-3, 12.66e-3};
	Float	V2[] = {-64.20e-3, -85.22e-3};
	Float	vS2[] = {9.6e-3, 16.9e-3};
	Float	tau1Max[] = {0.33e-3, 9.9e-3};
	Float	A1[] = {31.25e-3, 15.27e-3};
	Float	B1[] = {5.42e-3, 7.27e-3};
	Float	tau1Min[] = {0.1e-3, 1.3e-3};
	Float	tau2Max[] = {0.1e-3, 4.27e-3};
	Float	A2[] = {1e-3, 48.2e-3};
	Float	B2[] = {1e-3, 8.72e-3};
	Float	tau2Min[] = {0.09e-3, 0.01e-3};

	if (!AllocNumCurrents_IHCRP_LopezPoveda(2)) {
		NotifyError(wxT("%s: Could not allocate default arrays."), funcName);
		return(FALSE);
	}
	for (i = 0; i < lopezPovedaPtr->numCurrents; i++) {
		lopezPovedaPtr->label[i] = InitString_Utility_String(label[i]);
		lopezPovedaPtr->Ek[i] = Ek[i];
		lopezPovedaPtr->G[i] = G[i];
		lopezPovedaPtr->V1[i] = V1[i];
		lopezPovedaPtr->vS1[i] = vS1[i];
		lopezPovedaPtr->V2[i] = V2[i];
		lopezPovedaPtr->vS2[i] = vS2[i];
		lopezPovedaPtr->tau1Max[i] = tau1Max[i];
		lopezPovedaPtr->A1[i] = A1[i];
		lopezPovedaPtr->B1[i] = B1[i];
		lopezPovedaPtr->tau1Min[i] = tau1Min[i];
		lopezPovedaPtr->tau2Max[i] = tau2Max[i];
		lopezPovedaPtr->A2[i] = A2[i];
		lopezPovedaPtr->B2[i] = B2[i];
		lopezPovedaPtr->tau2Min[i] = tau2Min[i];
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
Init_IHCRP_LopezPoveda(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IHCRP_LopezPoveda");

	if (parSpec == GLOBAL) {
		if (lopezPovedaPtr != NULL)
			Free_IHCRP_LopezPoveda();
		if ((lopezPovedaPtr = (LopezPovedaPtr) malloc(sizeof(LopezPoveda))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (lopezPovedaPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	lopezPovedaPtr->parSpec = parSpec;
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->bMCiliaCouplingMode = GENERAL_BOOLEAN_ON;
	lopezPovedaPtr->ciliaTC = 1.0 / 530.0;
	lopezPovedaPtr->ciliaCGain = 2.0;
	lopezPovedaPtr->Et = 100.0e-3;
	lopezPovedaPtr->Rp = 0.01;
	lopezPovedaPtr->Rt = 0.24;
	lopezPovedaPtr->gL = 0.33e-9;
	lopezPovedaPtr->Gm = 9.45e-9;
	lopezPovedaPtr->s0 = 63.1e-9;
	lopezPovedaPtr->s1 = 12.7e-9;
	lopezPovedaPtr->u0 = 52.7e-9;
	lopezPovedaPtr->u1 = 29.4e-9;
	lopezPovedaPtr->Ca = 0.89e-12;
	lopezPovedaPtr->Cb = 8.0e-12;
	lopezPovedaPtr->numCurrents = 0;
	lopezPovedaPtr->label = NULL;
	lopezPovedaPtr->Ek = NULL;
	lopezPovedaPtr->G = NULL;
	lopezPovedaPtr->V1 = NULL;
	lopezPovedaPtr->vS1 = NULL;
	lopezPovedaPtr->V2 = NULL;
	lopezPovedaPtr->vS2 = NULL;
	lopezPovedaPtr->tau1Max = NULL;
	lopezPovedaPtr->A1 = NULL;
	lopezPovedaPtr->B1 = NULL;
	lopezPovedaPtr->tau1Min = NULL;
	lopezPovedaPtr->tau2Max = NULL;
	lopezPovedaPtr->A2 = NULL;
	lopezPovedaPtr->B2 = NULL;
	lopezPovedaPtr->tau2Min = NULL;
	lopezPovedaPtr->VMRest = -60.0e-3;

	if (!SetDefaultNumCurrentsArrays_IHCRP_LopezPoveda()) {
		NotifyError(wxT("%s: Could not set the default 'numCurrents' arrays."),
		  funcName);
		return(FALSE);
	}

	if (!SetUniParList_IHCRP_LopezPoveda()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHCRP_LopezPoveda();
		return(FALSE);
	}
	lopezPovedaPtr->lPICList = NULL;
	lopezPovedaPtr->lastOutput = NULL;
	lopezPovedaPtr->lastCiliaDisplacement_u = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHCRP_LopezPoveda(void)
{
	static const WChar	*funcName = wxT("SetUniParList_IHCRP_LopezPoveda");
	UniParPtr	pars;

	if ((lopezPovedaPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_LOPEZPOVEDA_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = lopezPovedaPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_BMCILIACOUPLINGMODE], wxT("BM_CILIA_COUPLING_MODE"),
	  wxT("BM-cilia coupling mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &lopezPovedaPtr->bMCiliaCouplingMode, NULL,
	  (void * (*)) SetBMCiliaCouplingMode_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_CILIATC], wxT("T_C"),
	  wxT("BM/cilia displacement time constant, s."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->ciliaTC, NULL,
	  (void * (*)) SetCiliaTC_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_CILIACGAIN], wxT("GAIN_C"),
	  wxT("Cilia coupling gain."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->ciliaCGain, NULL,
	  (void * (*)) SetCiliaCGain_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_ET], wxT("E_T"),
	  wxT("Endocochlear potential, Et (V)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->Et, NULL,
	  (void * (*)) SetEt_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_RP], wxT("R_P"),
	  wxT("Epithelium resistance, Rp (Ohm)"),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->Rp, NULL,
	  (void * (*)) SetRp_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_RT], wxT("R_T"),
	  wxT("Epithelium resistance, Rt (Ohm)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->Rt, NULL,
	  (void * (*)) SetRt_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_GL], wxT("G_L"),
	  wxT("Apical leakage conductance, gL (S)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->gL, NULL,
	  (void * (*)) SetGL_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_GM], wxT("G_M"),
	  wxT("Maximum mechanical conductance, Gm (S)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->Gm, NULL,
	  (void * (*)) SetGm_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_S0], wxT("S_0"),
	  wxT("Displacement sensitivity, s0 (1/m)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->s0, NULL,
	  (void * (*)) SetS0_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_S1], wxT("S_1"),
	  wxT("Displacement sensitivity, s1 (1/m)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->s1, NULL,
	  (void * (*)) SetS1_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_U0], wxT("U_0"),
	  wxT("Displacement offset, u0 (m)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->u0, NULL,
	  (void * (*)) SetU0_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_U1], wxT("U_1"),
	  wxT("Displacement offset, u1 (m)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->u1, NULL,
	  (void * (*)) SetU1_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_CA], wxT("C_A"),
	  wxT("Apical capacitance, Ca (F)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->Ca, NULL,
	  (void * (*)) SetCa_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_CB], wxT("C_B"),
	  wxT("Basal capacitance, Cb (F)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->Cb, NULL,
	  (void * (*)) SetCb_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_NUMCURRENTS], wxT("NUM_CURRENTS"),
	  wxT("The number of currents."),
	  UNIPAR_INT_AL,
	  &lopezPovedaPtr->numCurrents, NULL,
	  (void * (*)) SetNumCurrents_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_LABEL], wxT("LABEL"),
	  wxT("Conductance label."),
	  UNIPAR_STRING_ARRAY,
	  &lopezPovedaPtr->label, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualLabel_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_EK], wxT("E_K"),
	  wxT("Conductance reversal potential, Ek (V)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->Ek, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualEk_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_G], wxT("G1"),
	  wxT("Maximum conductance, G (S)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->G, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualG_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_V1], wxT("V_1"),
	  wxT("Conductance half-activation potential, V1 (V)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->V1, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualV1_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_VS1], wxT("VS_1"),
	  wxT("Conductance voltage sensitivity constant, S1 (V)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->vS1, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualVS1_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_V2], wxT("V_2"),
	  wxT("Conductance half-activation potential, V2 (V)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->V2, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualV2_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_VS2], wxT("VS_2"),
	  wxT("Conductance voltage sensitivity constant, S2 (V)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->vS2, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualVS2_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_TAU1MAX], wxT("TAU_1MAX"),
	  wxT("Conductance voltage time constant, tau1 max (s)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->tau1Max, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualTau1Max_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_A1], wxT("A_1"),
	  wxT("Conductance voltage time constant, A1 (s)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->A1, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualA1_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_B1], wxT("B_1"),
	  wxT("Conductance voltage time constant, B1 (s)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->B1, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualB1_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_TAU1MIN], wxT("TAU_1MIN"),
	  wxT("Conductance voltage time constant, tau1Min (s)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->tau1Min, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualTau1Min_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_TAU2MAX], wxT("TAU_2MAX"),
	  wxT("Conductance voltage time constant, tau2Max (s)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->tau2Max, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualTau2Max_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_A2], wxT("A_2"),
	  wxT("Conductance voltage time constant, A2 (s)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->A2, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualA2_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_B2], wxT("B_2"),
	  wxT("Conductance voltage time constant, B2 (s)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->B2, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualB2_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_TAU2MIN], wxT("TAU_2MIN"),
	  wxT("Conductance voltage time constant, tau2Min (s)."),
	  UNIPAR_REAL_ARRAY,
	  &lopezPovedaPtr->tau2Min, &lopezPovedaPtr->numCurrents,
	  (void * (*)) SetIndividualTau2Min_IHCRP_LopezPoveda);
	SetPar_UniParMgr(&pars[IHCRP_LOPEZPOVEDA_VMREST], wxT("V_MREST"),
	  wxT("Resting membrane potential, Vmrest (V)."),
	  UNIPAR_REAL,
	  &lopezPovedaPtr->VMRest, NULL,
	  (void * (*)) SetVMRest_IHCRP_LopezPoveda);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHCRP_LopezPoveda(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(lopezPovedaPtr->parList);

}

/****************************** AllocNumCurrents ******************************/

/*
 * This function allocates the memory for the module arrays.
 * It will assume that nothing needs to be done if the 'numCurrents'
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numCurrents'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumCurrents_IHCRP_LopezPoveda(int numCurrents)
{
	static const WChar	*funcName = wxT("AllocNumCurrents_IHCRP_LopezPoveda");

	if (numCurrents == lopezPovedaPtr->numCurrents)
		return(TRUE);
	if (lopezPovedaPtr->label)
		FreeLabels_IHCRP_LopezPoveda();
	if ((lopezPovedaPtr->label = (WChar **) calloc(numCurrents,
	  sizeof(WChar *))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' labels."), funcName,
		   numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->Ek)
		free(lopezPovedaPtr->Ek);
	if ((lopezPovedaPtr->Ek = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' Ek."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->G)
		free(lopezPovedaPtr->G);
	if ((lopezPovedaPtr->G = (Float *) calloc(numCurrents, sizeof(Float)))
	  == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' G."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->V1)
		free(lopezPovedaPtr->V1);
	if ((lopezPovedaPtr->V1 = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' V1."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->vS1)
		free(lopezPovedaPtr->vS1);
	if ((lopezPovedaPtr->vS1 = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' vS1."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->V2)
		free(lopezPovedaPtr->V2);
	if ((lopezPovedaPtr->V2 = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' V2."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->vS2)
		free(lopezPovedaPtr->vS2);
	if ((lopezPovedaPtr->vS2 = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' vS2."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->tau1Max)
		free(lopezPovedaPtr->tau1Max);
	if ((lopezPovedaPtr->tau1Max = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' tau1Max."),
		  funcName, numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->A1)
		free(lopezPovedaPtr->A1);
	if ((lopezPovedaPtr->A1 = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' A1."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->B1)
		free(lopezPovedaPtr->B1);
	if ((lopezPovedaPtr->B1 = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' B1."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->tau1Min)
		free(lopezPovedaPtr->tau1Min);
	if ((lopezPovedaPtr->tau1Min = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' tau1Min."),
		  funcName, numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->tau2Max)
		free(lopezPovedaPtr->tau2Max);
	if ((lopezPovedaPtr->tau2Max = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' tau2Max."),
		  funcName, numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->A2)
		free(lopezPovedaPtr->A2);
	if ((lopezPovedaPtr->A2 = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' A2."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->B2)
		free(lopezPovedaPtr->B2);
	if ((lopezPovedaPtr->B2 = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' B2."), funcName,
		  numCurrents);
		return(FALSE);
	}
	if (lopezPovedaPtr->tau2Min)
		free(lopezPovedaPtr->tau2Min);
	if ((lopezPovedaPtr->tau2Min = (Float *) calloc(numCurrents, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' tau2Min."),
		  funcName, numCurrents);
		return(FALSE);
	}
	lopezPovedaPtr->numCurrents = numCurrents;
	return(TRUE);

}

/****************************** SetBMCiliaCouplingMode ************************/

/*
 * This function sets the module's bMCiliaCouplingMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBMCiliaCouplingMode_IHCRP_LopezPoveda(WChar * theBMCiliaCouplingMode)
{
	static const WChar	*funcName = wxT(
	  "SetBMCiliaCouplingMode_IHCRP_LopezPoveda");
	int		specifier;

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theBMCiliaCouplingMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName, theBMCiliaCouplingMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->bMCiliaCouplingMode = specifier;
	return(TRUE);

}

/****************************** SetCiliaTC ************************************/

/*
 * This function sets the module's ciliaTC parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCiliaTC_IHCRP_LopezPoveda(Float theCiliaTC)
{
	static const WChar	*funcName = wxT("SetCiliaTC_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->ciliaTC = theCiliaTC;
	return(TRUE);

}

/****************************** SetCiliaCGain *********************************/

/*
 * This function sets the module's ciliaCGain parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCiliaCGain_IHCRP_LopezPoveda(Float theCiliaCGain)
{
	static const WChar	*funcName = wxT("SetCiliaCGain_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->ciliaCGain = theCiliaCGain;
	return(TRUE);

}

/****************************** SetEt *****************************************/

/*
 * This function sets the module's Et parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEt_IHCRP_LopezPoveda(Float theEt)
{
	static const WChar	*funcName = wxT("SetEt_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->Et = theEt;
	return(TRUE);

}

/****************************** SetRp *****************************************/

/*
 * This function sets the module's Rp parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRp_IHCRP_LopezPoveda(Float theRp)
{
	static const WChar	*funcName = wxT("SetRp_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->Rp = theRp;
	return(TRUE);

}

/****************************** SetRt *****************************************/

/*
 * This function sets the module's Rt parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRt_IHCRP_LopezPoveda(Float theRt)
{
	static const WChar	*funcName = wxT("SetRt_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->Rt = theRt;
	return(TRUE);

}

/****************************** SetGL *****************************************/

/*
 * This function sets the module's gL parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGL_IHCRP_LopezPoveda(Float theGL)
{
	static const WChar	*funcName = wxT("SetGL_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->gL = theGL;
	return(TRUE);

}

/****************************** SetGm *****************************************/

/*
 * This function sets the module's Gm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGm_IHCRP_LopezPoveda(Float theGm)
{
	static const WChar	*funcName = wxT("SetGm_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->Gm = theGm;
	return(TRUE);

}

/****************************** SetS0 *****************************************/

/*
 * This function sets the module's s0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetS0_IHCRP_LopezPoveda(Float theS0)
{
	static const WChar	*funcName = wxT("SetS0_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->s0 = theS0;
	return(TRUE);

}

/****************************** SetS1 *****************************************/

/*
 * This function sets the module's s1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetS1_IHCRP_LopezPoveda(Float theS1)
{
	static const WChar	*funcName = wxT("SetS1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->s1 = theS1;
	return(TRUE);

}

/****************************** SetU0 *****************************************/

/*
 * This function sets the module's u0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetU0_IHCRP_LopezPoveda(Float theU0)
{
	static const WChar	*funcName = wxT("SetU0_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->u0 = theU0;
	return(TRUE);

}

/****************************** SetU1 *****************************************/

/*
 * This function sets the module's u1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetU1_IHCRP_LopezPoveda(Float theU1)
{
	static const WChar	*funcName = wxT("SetU1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->u1 = theU1;
	return(TRUE);

}

/****************************** SetCa *****************************************/

/*
 * This function sets the module's Ca parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCa_IHCRP_LopezPoveda(Float theCa)
{
	static const WChar	*funcName = wxT("SetCa_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->Ca = theCa;
	return(TRUE);

}

/****************************** SetCb *****************************************/

/*
 * This function sets the module's Cb parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCb_IHCRP_LopezPoveda(Float theCb)
{
	static const WChar	*funcName = wxT("SetCb_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->Cb = theCb;
	return(TRUE);

}

/****************************** SetNumCurrents ********************************/

/*
 * This function sets the module's numCurrents parameter.
 * It returns TRUE if the operation is successful.
 * The 'numCurrents' variable is set by the
 * 'AllocNumCurrents_IHCRP_LopezPoveda' routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumCurrents_IHCRP_LopezPoveda(int theNumCurrents)
{
	static const WChar	*funcName = wxT("SetNumCurrents_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumCurrents < 1) {
		NotifyError(wxT("%s: Value must be greater then zero (%d)."),
		  funcName, theNumCurrents);
		return(FALSE);
	}
	if (!AllocNumCurrents_IHCRP_LopezPoveda(theNumCurrents)) {
		NotifyError(wxT(
		  "%s: Cannot allocate memory for the 'numCurrents' arrays."),
		  funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetLabel **************************************/

/*
 * This function sets the module's label array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLabel_IHCRP_LopezPoveda(WChar **theLabel)
{
	static const WChar	*funcName = wxT("SetLabel_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->label = theLabel;
	return(TRUE);

}

/****************************** SetIndividualLabel ****************************/

/*
 * This function sets the module's label array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualLabel_IHCRP_LopezPoveda(int theIndex, WChar *theLabel)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualLabel_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->label == NULL) {
		NotifyError(wxT("%s: Label not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	free(lopezPovedaPtr->label[theIndex]);
	lopezPovedaPtr->label[theIndex] = InitString_Utility_String(theLabel);
	return(TRUE);

}

/****************************** SetEk *****************************************/

/*
 * This function sets the module's Ek array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEk_IHCRP_LopezPoveda(Float *theEk)
{
	static const WChar	*funcName = wxT("SetEk_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->Ek = theEk;
	return(TRUE);

}

/****************************** SetIndividualEk *******************************/

/*
 * This function sets the module's Ek array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualEk_IHCRP_LopezPoveda(int theIndex, Float theEk)
{
	static const WChar	*funcName = wxT("SetIndividualEk_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->Ek == NULL) {
		NotifyError(wxT("%s: Ek not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->Ek[theIndex] = theEk;
	return(TRUE);

}

/****************************** SetG ******************************************/

/*
 * This function sets the module's G array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetG_IHCRP_LopezPoveda(Float *theG)
{
	static const WChar	*funcName = wxT("SetG_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->G = theG;
	return(TRUE);

}

/****************************** SetIndividualG ********************************/

/*
 * This function sets the module's G array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualG_IHCRP_LopezPoveda(int theIndex, Float theG)
{
	static const WChar	*funcName = wxT("SetIndividualG_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->G == NULL) {
		NotifyError(wxT("%s: G not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->G[theIndex] = theG;
	return(TRUE);

}

/****************************** SetV1 *****************************************/

/*
 * This function sets the module's V1 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetV1_IHCRP_LopezPoveda(Float *theV1)
{
	static const WChar	*funcName = wxT("SetV1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->V1 = theV1;
	return(TRUE);

}

/****************************** SetIndividualV1 *******************************/

/*
 * This function sets the module's V1 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualV1_IHCRP_LopezPoveda(int theIndex, Float theV1)
{
	static const WChar	*funcName = wxT("SetIndividualV1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->V1 == NULL) {
		NotifyError(wxT("%s: V1 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->V1[theIndex] = theV1;
	return(TRUE);

}

/****************************** SetVS1 ****************************************/

/*
 * This function sets the module's vS1 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetVS1_IHCRP_LopezPoveda(Float *theVS1)
{
	static const WChar	*funcName = wxT("SetVS1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->vS1 = theVS1;
	return(TRUE);

}

/****************************** SetIndividualVS1 *******************************/

/*
 * This function sets the module's vS1 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualVS1_IHCRP_LopezPoveda(int theIndex, Float theVS1)
{
	static const WChar	*funcName = wxT("SetIndividualVS1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->vS1 == NULL) {
		NotifyError(wxT("%s: vS1 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->vS1[theIndex] = theVS1;
	return(TRUE);

}

/****************************** SetV2 *****************************************/

/*
 * This function sets the module's V2 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetV2_IHCRP_LopezPoveda(Float *theV2)
{
	static const WChar	*funcName = wxT("SetV2_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->V2 = theV2;
	return(TRUE);

}

/****************************** SetIndividualV2 *******************************/

/*
 * This function sets the module's V2 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualV2_IHCRP_LopezPoveda(int theIndex, Float theV2)
{
	static const WChar	*funcName = wxT("SetIndividualV2_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->V2 == NULL) {
		NotifyError(wxT("%s: V2 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->V2[theIndex] = theV2;
	return(TRUE);

}

/****************************** SetVS2 *****************************************/

/*
 * This function sets the module's vS2 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetVS2_IHCRP_LopezPoveda(Float *theVS2)
{
	static const WChar	*funcName = wxT("SetVS2_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->vS2 = theVS2;
	return(TRUE);

}

/****************************** SetIndividualVS2 *******************************/

/*
 * This function sets the module's vS2 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualVS2_IHCRP_LopezPoveda(int theIndex, Float theVS2)
{
	static const WChar	*funcName = wxT("SetIndividualVS2_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->vS2 == NULL) {
		NotifyError(wxT("%s: vS2 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->vS2[theIndex] = theVS2;
	return(TRUE);

}

/****************************** SetTau1Max ************************************/

/*
 * This function sets the module's tau1Max array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTau1Max_IHCRP_LopezPoveda(Float *theTau1Max)
{
	static const WChar	*funcName = wxT("SetTau1Max_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->tau1Max = theTau1Max;
	return(TRUE);

}

/****************************** SetIndividualTau1Max **************************/

/*
 * This function sets the module's tau1Max array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualTau1Max_IHCRP_LopezPoveda(int theIndex, Float theTau1Max)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualTau1Max_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->tau1Max == NULL) {
		NotifyError(wxT("%s: Tau1Max not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->tau1Max[theIndex] = theTau1Max;
	return(TRUE);

}

/****************************** SetA1 *****************************************/

/*
 * This function sets the module's A1 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetA1_IHCRP_LopezPoveda(Float *theA1)
{
	static const WChar	*funcName = wxT("SetA1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->A1 = theA1;
	return(TRUE);

}

/****************************** SetIndividualA1 *******************************/

/*
 * This function sets the module's A1 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualA1_IHCRP_LopezPoveda(int theIndex, Float theA1)
{
	static const WChar	*funcName = wxT("SetIndividualA1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->A1 == NULL) {
		NotifyError(wxT("%s: A1 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->A1[theIndex] = theA1;
	return(TRUE);

}

/****************************** SetB1 *****************************************/

/*
 * This function sets the module's B1 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetB1_IHCRP_LopezPoveda(Float *theB1)
{
	static const WChar	*funcName = wxT("SetB1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->B1 = theB1;
	return(TRUE);

}

/****************************** SetIndividualB1 *******************************/

/*
 * This function sets the module's B1 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualB1_IHCRP_LopezPoveda(int theIndex, Float theB1)
{
	static const WChar	*funcName = wxT("SetIndividualB1_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->B1 == NULL) {
		NotifyError(wxT("%s: B1 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->B1[theIndex] = theB1;
	return(TRUE);

}

/****************************** SetTau1Min ************************************/

/*
 * This function sets the module's tau1Min array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTau1Min_IHCRP_LopezPoveda(Float *theTau1Min)
{
	static const WChar	*funcName = wxT("SetTau1Min_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->tau1Min = theTau1Min;
	return(TRUE);

}

/****************************** SetIndividualTau1Min **************************/

/*
 * This function sets the module's tau1Min array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualTau1Min_IHCRP_LopezPoveda(int theIndex, Float theTau1Min)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualTau1Min_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->tau1Min == NULL) {
		NotifyError(wxT("%s: Tau1Min not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->tau1Min[theIndex] = theTau1Min;
	return(TRUE);

}

/****************************** SetTau2Max ************************************/

/*
 * This function sets the module's tau2Max array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTau2Max_IHCRP_LopezPoveda(Float *theTau2Max)
{
	static const WChar	*funcName = wxT("SetTau2Max_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->tau2Max = theTau2Max;
	return(TRUE);

}

/****************************** SetIndividualTau2Max **************************/

/*
 * This function sets the module's tau2Max array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualTau2Max_IHCRP_LopezPoveda(int theIndex, Float theTau2Max)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualTau2Max_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->tau2Max == NULL) {
		NotifyError(wxT("%s: Tau2Max not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->tau2Max[theIndex] = theTau2Max;
	return(TRUE);

}

/****************************** SetA2 *****************************************/

/*
 * This function sets the module's A2 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetA2_IHCRP_LopezPoveda(Float *theA2)
{
	static const WChar	*funcName = wxT("SetA2_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->A2 = theA2;
	return(TRUE);

}

/****************************** SetIndividualA2 *******************************/

/*
 * This function sets the module's A2 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualA2_IHCRP_LopezPoveda(int theIndex, Float theA2)
{
	static const WChar	*funcName = wxT("SetIndividualA2_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->A2 == NULL) {
		NotifyError(wxT("%s: A2 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->A2[theIndex] = theA2;
	return(TRUE);

}

/****************************** SetB2 *****************************************/

/*
 * This function sets the module's B2 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetB2_IHCRP_LopezPoveda(Float *theB2)
{
	static const WChar	*funcName = wxT("SetB2_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->B2 = theB2;
	return(TRUE);

}

/****************************** SetIndividualB2 *******************************/

/*
 * This function sets the module's B2 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualB2_IHCRP_LopezPoveda(int theIndex, Float theB2)
{
	static const WChar	*funcName = wxT("SetIndividualB2_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->B2 == NULL) {
		NotifyError(wxT("%s: B2 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->B2[theIndex] = theB2;
	return(TRUE);

}

/****************************** SetTau2Min ************************************/

/*
 * This function sets the module's tau2Min array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTau2Min_IHCRP_LopezPoveda(Float *theTau2Min)
{
	static const WChar	*funcName = wxT("SetTau2Min_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->tau2Min = theTau2Min;
	return(TRUE);

}

/****************************** SetIndividualTau2Min **************************/

/*
 * This function sets the module's tau2Min array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualTau2Min_IHCRP_LopezPoveda(int theIndex, Float theTau2Min)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualTau2Min_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (lopezPovedaPtr->tau2Min == NULL) {
		NotifyError(wxT("%s: Tau2Min not set."), funcName);
		return(FALSE);
	}
	if (theIndex > lopezPovedaPtr->numCurrents - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, lopezPovedaPtr->numCurrents - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->tau2Min[theIndex] = theTau2Min;
	return(TRUE);

}

/****************************** SetVMRest *************************************/

/*
 * This function sets the module's VMRest parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetVMRest_IHCRP_LopezPoveda(Float theVMRest)
{
	static const WChar	*funcName = wxT("SetVMRest_IHCRP_LopezPoveda");

	if (lopezPovedaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	lopezPovedaPtr->updateProcessVariablesFlag = TRUE;
	lopezPovedaPtr->VMRest = theVMRest;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_IHCRP_LopezPoveda(void)
{
	int		i;

	DPrint(wxT("Lopez-Poveda IHCRP Module Parameters:-\n"));
	DPrint(wxT("\tbMCiliaCouplingMode = %s\n"), BooleanList_NSpecLists(
	  lopezPovedaPtr->bMCiliaCouplingMode)->name);
	DPrint(wxT("\tciliaTC = %g ??\n"), lopezPovedaPtr->ciliaTC);
	DPrint(wxT("\tciliaCGain = %g ??\n"), lopezPovedaPtr->ciliaCGain);
	DPrint(wxT("\tEt = %g ??\n"), lopezPovedaPtr->Et);
	DPrint(wxT("\tRp = %g ??\n"), lopezPovedaPtr->Rp);
	DPrint(wxT("\tRt = %g ??\n"), lopezPovedaPtr->Rt);
	DPrint(wxT("\tgL = %g ??\n"), lopezPovedaPtr->gL);
	DPrint(wxT("\tGm = %g ??\n"), lopezPovedaPtr->Gm);
	DPrint(wxT("\ts0 = %g ??\n"), lopezPovedaPtr->s0);
	DPrint(wxT("\ts1 = %g ??\n"), lopezPovedaPtr->s1);
	DPrint(wxT("\tu0 = %g ??\n"), lopezPovedaPtr->u0);
	DPrint(wxT("\tu1 = %g ??\n"), lopezPovedaPtr->u1);
	DPrint(wxT("\tCa = %g ??\n"), lopezPovedaPtr->Ca);
	DPrint(wxT("\tCb = %g ??\n"), lopezPovedaPtr->Cb);
	DPrint(wxT("\tnumCurrents = %d ??\n"), lopezPovedaPtr->numCurrents);
	DPrint(wxT("\t%10s\t%10s\t%10s\t%10s\n"), wxT("Label"), wxT("Ek (V)"),
	  wxT("G (S)"), wxT("V1 (V)"), wxT("VS1 (V)"));
	DPrint(wxT("\t%10s\t%10s\t%10s\t%10s\n"), wxT("----------"), wxT("----------"),
	  wxT("----------"), wxT("----------"), wxT("----------"));
	for (i = 0; i < lopezPovedaPtr->numCurrents; i++)
		DPrint(wxT("\t%10s\t%10g\t%10g\t%10g\n"), lopezPovedaPtr->label[i],
		  lopezPovedaPtr->Ek[i], lopezPovedaPtr->G[i], lopezPovedaPtr->V1[i]);
	DPrint(wxT("\n"));
	DPrint(wxT("\t%10s\t%10s\t%10s\t%10s\n"), wxT("Label"), wxT("V2 (V)"),
	  wxT("VS2 (s)"), wxT("tau1Max (s)"), wxT("A1 (s)"));
	DPrint(wxT("\t%10s\t%10s\t%10s\t%10s\n"), wxT("----------"), wxT("----------"),
	  wxT("----------"), wxT("----------"), wxT("----------"));
	for (i = 0; i < lopezPovedaPtr->numCurrents; i++)
		DPrint(wxT("\t%10s\t%10g\t%10g\t%10g\n"), lopezPovedaPtr->label[i],
		  lopezPovedaPtr->V2[i], lopezPovedaPtr->vS2[i], lopezPovedaPtr->
		  tau1Max[i], lopezPovedaPtr->A1[i]);
	DPrint(wxT("\n"));
	DPrint(wxT("\t%10s\t%10s\t%10s\t%10s\n"), wxT("Label"), wxT("B1 (s)"),
	  wxT("tau1Min (s)"), wxT("tau2Max (s)"), wxT("A2 (s)"));
	DPrint(wxT("\t%10s\t%10s\t%10s\t%10s\n"), wxT("----------"), wxT("----------"),
	  wxT("----------"), wxT("----------"), wxT("----------"));
	for (i = 0; i < lopezPovedaPtr->numCurrents; i++)
		DPrint(wxT("\t%10s\t%10g\t%10g\t%10g\n"), lopezPovedaPtr->label[i],
		  lopezPovedaPtr->B1[i], lopezPovedaPtr->tau1Min[i],
		  lopezPovedaPtr->tau2Max[i], lopezPovedaPtr->A2[i]);
	DPrint(wxT("\n"));
	DPrint(wxT("\t%10s\t%10s\t%10s\n"), wxT("Label"), wxT("B2 (s)"),
	  wxT("tau2Min (s)"));
	DPrint(wxT("\t%10s\t%10s\t%10s\n"), wxT("----------"), wxT("----------"),
	  wxT("----------"));
	for (i = 0; i < lopezPovedaPtr->numCurrents; i++)
		DPrint(wxT("\t%10s\t%10g\t%10g\n"), lopezPovedaPtr->label[i],
		  lopezPovedaPtr->B2[i], lopezPovedaPtr->tau2Min[i]);
	DPrint(wxT("\n"));
	DPrint(wxT("\tVMRest = %g ??\n"), lopezPovedaPtr->VMRest);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHCRP_LopezPoveda(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHCRP_LopezPoveda");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	lopezPovedaPtr = (LopezPovedaPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_IHCRP_LopezPoveda(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHCRP_LopezPoveda");

	if (!SetParsPointer_IHCRP_LopezPoveda(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_LopezPoveda(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = lopezPovedaPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHCRP_LopezPoveda;
	theModule->GetUniParListPtr = GetUniParListPtr_IHCRP_LopezPoveda;
	theModule->PrintPars = PrintPars_IHCRP_LopezPoveda;
	theModule->RunProcess = RunModel_IHCRP_LopezPoveda;
	theModule->SetParsPointer = SetParsPointer_IHCRP_LopezPoveda;
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
CheckData_IHCRP_LopezPoveda(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IHCRP_LopezPoveda");

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
InitProcessVariables_IHCRP_LopezPoveda(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_IHCRP_LopezPoveda");
	int		i, j, k;
	Float	vm;
	LPIonChannelPtr	iC;
	LopezPovedaPtr	p = lopezPovedaPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_IHCRP_LopezPoveda();
		p->numChannels = _OutSig_EarObject(data)->numChannels;
		if ((p->lastOutput = (Float *) calloc(p->numChannels, sizeof(Float))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'lastOutput'."),
			  funcName);
			return(FALSE);
		}
		if ((p->lastCiliaDisplacement_u = (Float *) calloc(p->numChannels,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for ")
			  wxT("'lastCiliaDisplacement_u'."), funcName);
			return(FALSE);
		}
		if ((p->lPICList = (LPIonChannelPtr *) calloc(p->numChannels,
		  sizeof(LPIonChannelPtr))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'LPIonChannelPtr'."), funcName);
			return(FALSE);
		}
		for (i = 0; i < p->numChannels; i++)
			if ((p->lPICList[i] = (LPIonChannel *) calloc(p->numCurrents,
			  sizeof(LPIonChannel))) == NULL) {
				NotifyError(wxT("%s: Out of memory for ion channel list %d"), funcName,
				  i);
				return(FALSE);
			}
		p->VOC = p->Et * p->Rp / (p->Rp + p->Rt);
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		vm = p->VMRest - p->VOC;
		for (i = 0; i < p->numChannels; i++) {
			p->lastOutput[i] = p->VMRest;
			p->lastCiliaDisplacement_u[i] = 0.0;
			for (j = 0, iC = p->lPICList[i]; j < p->numCurrents; j++, iC++) {
				iC->Ekp = p->Ek[j] + p->VOC;
				iC->tau1Diff = p->tau1Max[j] - p->tau1Min[j];
				iC->tau2Diff = p->tau2Max[j] - p->tau2Min[j];
				for (k = 0; k < IHCRP_LOPEZPOVEDA_NUM_STATE_VARS; k++)
					iC->stateVector[k] = BOLTZMANN_2ORDER(1.0, vm, p->V1[j],
					  p->vS1[j], p->V2[j], p->vS2[j]);
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
FreeProcessVariables_IHCRP_LopezPoveda(void)
{
	/*static const WChar	*funcName = wxT("FreeProcessVariables_IHCRP_LopezPoveda");*/
	int		i;
	LopezPovedaPtr	p = lopezPovedaPtr;

	if (p->lPICList) {
		for (i = 0; i < p->numChannels; i++)
			if (p->lPICList[i])
				free(p->lPICList[i]);
		free(p->lPICList);
		p->lPICList = NULL;
	}
	if (p->lastOutput != NULL) {
		free(p->lastOutput);
		p->lastOutput = NULL;
	}
	if (p->lastCiliaDisplacement_u != NULL) {
		free(p->lastCiliaDisplacement_u);
		p->lastCiliaDisplacement_u = NULL;
	}
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
RunModel_IHCRP_LopezPoveda(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_IHCRP_LopezPoveda");
	register ChanData	 *inPtr, *outPtr;
	int		j, chan;
	ChanLen	i;
	Float	dt, Gu, GkVN, Vtm, u, V_1, T1, T2, M1, M2, A, B, C, currentSum, GkVTSum;
	LPIonChannelPtr	iC;
	SignalDataPtr	inSignal, outSignal;
	LopezPovedaPtr	p = lopezPovedaPtr;

	inSignal = _InSig_EarObject(data, 0);
	if (!data->threadRunFlag) {
		if (!CheckData_IHCRP_LopezPoveda(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Module process ??"));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels, inSignal->length,
		  inSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHCRP_LopezPoveda(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		p->CTotalOverDt = (p->Ca + p->Cb) / inSignal->dt;
		p->cGaindt = pow(10.0, p->ciliaCGain / 20.0) * inSignal->dt;
		p->dtOverTc = inSignal->dt / p->ciliaTC;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	dt = outSignal->dt;
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan];
		outPtr = outSignal->channel[chan];
		V_1 = p->lastOutput[chan];
		u = p->lastCiliaDisplacement_u[chan];
		for (i = 0; i < data->outSignal->length; i++, inPtr++, outPtr++) {
			if (p->bMCiliaCouplingMode)
				u += p->cGaindt * *inPtr - u * p->dtOverTc;
			else
				u = *inPtr;
			Gu = BOLTZMANN_2ORDER(p->Gm, u, p->u0, p->s0, p->u1, p->s1) + p->gL;
			Vtm = V_1 - p->VOC;
			for (j = 0, iC = p->lPICList[chan], currentSum = GkVTSum = 0.0; j <
			  p->numCurrents; j++, iC++) {
				T1 = CURRENT_TIME_CONST(p->tau1Min[j], iC->tau1Diff, p->A1[j],
				  p->B1[j], Vtm);
				T2 = CURRENT_TIME_CONST(p->tau2Min[j], iC->tau2Diff, p->A2[j],
				  p->B2[j], Vtm);
				M1 = dt / T1;
				M2 = dt / T2;
				A = 2.0 + M1 + M2;
			    B = M1 * M2;
			    C = 1.0 + M1 + M2 + M1 * M2;
				GkVN = BOLTZMANN_2ORDER(1.0, Vtm, p->V1[j], p->vS1[j], p->V2[j],
				  p->vS2[j]);
				iC->GkVTN = (A * *iC->stateVector - *(iC->stateVector + 1) +
				  GkVN * B) / C;
				*(iC->stateVector + 1) =  *iC->stateVector;
				*iC->stateVector = iC->GkVTN;
				iC->GkVT = p->G[j] * iC->GkVTN;
				GkVTSum += iC->GkVT;
				currentSum += iC->Ekp * iC->GkVT;
			}
		    *outPtr = (p->Et * Gu + currentSum + p->CTotalOverDt * V_1) / (Gu +
		      p->CTotalOverDt + GkVTSum);
		    V_1 = *outPtr;
		}
		p->lastOutput[chan] = V_1;
		p->lastCiliaDisplacement_u[chan] = u;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
