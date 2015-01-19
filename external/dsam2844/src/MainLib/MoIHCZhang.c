/**********************
 *
 * File:		MoIHCZhang.c
 * Purpose:		Incorporates the Zhang et al. 'hppi' and 'syn' IHC synapse code.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		19 Aug 2002
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
#include "FiParFile.h"
#include "UtZhang.h"
#include "MoIHCZhang.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ZhangHCPtr	zhangHCPtr = NULL;

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
Free_IHC_Zhang(void)
{
	if (zhangHCPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHC_Zhang();
	if (zhangHCPtr->parList)
		FreeList_UniParMgr(&zhangHCPtr->parList);
	if (zhangHCPtr->parSpec == GLOBAL) {
		free(zhangHCPtr);
		zhangHCPtr = NULL;
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
Init_IHC_Zhang(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IHC_Zhang");

	if (parSpec == GLOBAL) {
		if (zhangHCPtr != NULL)
			Free_IHC_Zhang();
		if ((zhangHCPtr = (ZhangHCPtr) malloc(sizeof(ZhangHC))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (zhangHCPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	zhangHCPtr->parSpec = parSpec;
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->spont = 50.0;
	zhangHCPtr->aAss = 130.0;
	zhangHCPtr->tauST = 60e-3;
	zhangHCPtr->tauR = 2e-3;
	zhangHCPtr->aAROverAST = 6.0;
	zhangHCPtr->pIMax = 0.6;
	zhangHCPtr->ks = 1.3;
	zhangHCPtr->vs1 = 60.0;
	zhangHCPtr->vs2 = 1.0;
	zhangHCPtr->vs3 = 6.0;

	if (!SetUniParList_IHC_Zhang()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHC_Zhang();
		return(FALSE);
	}
	zhangHCPtr->iHCPPI = NULL;
	zhangHCPtr->synapse = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHC_Zhang(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHC_Zhang");
	UniParPtr	pars;

	if ((zhangHCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_ZHANG_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = zhangHCPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_ZHANG_SPONT], wxT("SPONT"),
	  wxT("Spontaneous rate of fiber (spikes/s)."),
	  UNIPAR_REAL,
	  &zhangHCPtr->spont, NULL,
	  (void * (*)) SetSpont_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_AASS], wxT("A_SS"),
	  wxT("Steady-state rate (spikes/s)."),
	  UNIPAR_REAL,
	  &zhangHCPtr->aAss, NULL,
	  (void * (*)) SetAAss_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_TAUST], wxT("TAU_ST"),
	  wxT("Short-term time constant (s)."),
	  UNIPAR_REAL,
	  &zhangHCPtr->tauST, NULL,
	  (void * (*)) SetTauST_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_TAUR], wxT("TAU_R"),
	  wxT("Rapid time constant (s)."),
	  UNIPAR_REAL,
	  &zhangHCPtr->tauR, NULL,
	  (void * (*)) SetTauR_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_AAROVERAST], wxT("AR_AST"),
	  wxT("Rapid response amplitude to short-term response amplitude ratio."),
	  UNIPAR_REAL,
	  &zhangHCPtr->aAROverAST, NULL,
	  (void * (*)) SetAAROverAST_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_PIMAX], wxT("PI_MAX"),
	  wxT("Permeability at high sound level."),
	  UNIPAR_REAL,
	  &zhangHCPtr->pIMax, NULL,
	  (void * (*)) SetPIMax_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_KS], wxT("K_S"),
	  wxT("Species dependend voltage staturation parameter."),
	  UNIPAR_REAL,
	  &zhangHCPtr->ks, NULL,
	  (void * (*)) SetKs_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_VS1], wxT("V_S1"),
	  wxT("Species dependend voltage staturation parameter."),
	  UNIPAR_REAL,
	  &zhangHCPtr->vs1, NULL,
	  (void * (*)) SetVs1_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_VS2], wxT("V_S2"),
	  wxT("Species dependend voltage staturation parameter."),
	  UNIPAR_REAL,
	  &zhangHCPtr->vs2, NULL,
	  (void * (*)) SetVs2_IHC_Zhang);
	SetPar_UniParMgr(&pars[IHC_ZHANG_VS3], wxT("V_S3"),
	  wxT("Species dependend voltage staturation parameter."),
	  UNIPAR_REAL,
	  &zhangHCPtr->vs3, NULL,
	  (void * (*)) SetVs3_IHC_Zhang);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_Zhang(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (zhangHCPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(zhangHCPtr->parList);

}

/****************************** SetSpont **************************************/

/*
 * This function sets the module's spont parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpont_IHC_Zhang(Float theSpont)
{
	static const WChar	*funcName = wxT("SetSpont_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->spont = theSpont;
	return(TRUE);

}

/****************************** SetAAss ***************************************/

/*
 * This function sets the module's aAss parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAAss_IHC_Zhang(Float theAAss)
{
	static const WChar	*funcName = wxT("SetAAss_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->aAss = theAAss;
	return(TRUE);

}

/****************************** SetTauST **************************************/

/*
 * This function sets the module's tauST parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTauST_IHC_Zhang(Float theTauST)
{
	static const WChar	*funcName = wxT("SetTauST_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->tauST = theTauST;
	return(TRUE);

}

/****************************** SetTauR ***************************************/

/*
 * This function sets the module's tauR parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTauR_IHC_Zhang(Float theTauR)
{
	static const WChar	*funcName = wxT("SetTauR_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->tauR = theTauR;
	return(TRUE);

}

/****************************** SetAAROverAST *********************************/

/*
 * This function sets the module's aAROverAST parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAAROverAST_IHC_Zhang(Float theAAROverAST)
{
	static const WChar	*funcName = wxT("SetAAROverAST_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->aAROverAST = theAAROverAST;
	return(TRUE);

}

/****************************** SetPIMax **************************************/

/*
 * This function sets the module's pIMax parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPIMax_IHC_Zhang(Float thePIMax)
{
	static const WChar	*funcName = wxT("SetPIMax_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->pIMax = thePIMax;
	return(TRUE);

}

/****************************** SetKs *****************************************/

/*
 * This function sets the module's ks parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetKs_IHC_Zhang(Float theKs)
{
	static const WChar	*funcName = wxT("SetKs_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->ks = theKs;
	return(TRUE);

}

/****************************** SetVs1 ****************************************/

/*
 * This function sets the module's vs1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetVs1_IHC_Zhang(Float theVs1)
{
	static const WChar	*funcName = wxT("SetVs1_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->vs1 = theVs1;
	return(TRUE);

}

/****************************** SetVs2 ****************************************/

/*
 * This function sets the module's vs2 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetVs2_IHC_Zhang(Float theVs2)
{
	static const WChar	*funcName = wxT("SetVs2_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->vs2 = theVs2;
	return(TRUE);

}

/****************************** SetVs3 ****************************************/

/*
 * This function sets the module's vs3 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetVs3_IHC_Zhang(Float theVs3)
{
	static const WChar	*funcName = wxT("SetVs3_IHC_Zhang");

	if (zhangHCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	zhangHCPtr->updateProcessVariablesFlag = TRUE;
	zhangHCPtr->vs3 = theVs3;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_IHC_Zhang(void)
{
	DPrint(wxT("Zhang IHC Synapse Module Parameters:-\n"));
	DPrint(wxT("\tSpontaneous rate of fiber, spont = %g (spikes/s),\n"),
	 zhangHCPtr->spont);
	DPrint(wxT("\tSteady-state rate, Ass = %g (spikes/s),\n"), zhangHCPtr->
	  aAss);
	DPrint(wxT("\tShort-term time constant, tauST = %g (ms),\n"), MILLI(
	  zhangHCPtr->tauST));
	DPrint(wxT("\tRapid time constant tauR = %g (ms),\n"), MILLI(zhangHCPtr->
	  tauR));
	DPrint(wxT("\tResponse ratio, A_R / A_ST = %g,\n"), zhangHCPtr->aAROverAST);
	DPrint(wxT("\tPermeability at high sound level, Pi_Max = %g,\n"),
	  zhangHCPtr->pIMax);
	DPrint(wxT("\tSpecies dependend voltage staturation parameter, ks = %g,\n"),
	  zhangHCPtr->ks);
	DPrint(wxT("\tSpecies dependend voltage staturation parameter, vs1 = %g,")
	  wxT("\n"), zhangHCPtr->vs1);
	DPrint(wxT("\tSpecies dependend voltage staturation parameter, vs2 = %g,")
	  wxT("\n"), zhangHCPtr->vs2);
	DPrint(wxT("\tSpecies dependend voltage staturation parameter, vs3 = %g.")
	  wxT("\n"), zhangHCPtr->vs3);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHC_Zhang(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHC_Zhang");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	zhangHCPtr = (ZhangHCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_IHC_Zhang(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHC_Zhang");

	if (!SetParsPointer_IHC_Zhang(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHC_Zhang(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = zhangHCPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHC_Zhang;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Zhang;
	theModule->PrintPars = PrintPars_IHC_Zhang;
	theModule->RunProcess = RunModel_IHC_Zhang;
	theModule->SetParsPointer = SetParsPointer_IHC_Zhang;
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
CheckData_IHC_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IHC_Zhang");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** RunSynapseDynamic *****************************/

/*
 * Original name: runsyn_dynamic
 * Original file: synapse.c
 * !!!!!!!!!! The Float *in and Float *out could be the same pointer,
 * SO be careful about this
 * The 'error' variable that awas in the original code was removed as it was,
 * not set anywhere in the routine.
 */

void
RunSynapseDynamic_IHC_Zhang(TSynapse *pthis, const Float *in, Float *out,
  const int length)
{
	int register i;
	Float PPIlast,PL,PG,CIlast,CLlast,CG,VI,VL;
	Float tdres;
	Float CInow,CLnow;

	tdres = pthis->tdres;
	PL = pthis->PL;
	PG = pthis->PG;
	CG = pthis->CG;
	VI = pthis->VI;
	VL = pthis->VL;
	CIlast = pthis->CIlast;
	CLlast = pthis->CLlast;
	PPIlast = pthis->PPIlast;

	for (i = 0; i < length; i++){
		CInow = CIlast + (tdres / VI) * (( -PPIlast * CIlast) + PL * (CLlast -
		  CIlast));
		CLnow = CLlast + (tdres / VL) * (-PL * (CLlast - CIlast) + PG *(CG -
		  CLlast));
		PPIlast = *in++;
		CIlast = CInow;
		CLlast = CLnow;
		*out++ = CInow * PPIlast;
	}

	pthis->CIlast = CIlast;
	pthis->CLlast = CLlast;
	pthis->PPIlast = PPIlast;

}

/****************************** InitSynapse ***********************************/

/*
 * Original name: initSynapse
 * Original file: synapse.c
 * The 'error' variable that awas in the original code was removed as it was,
 * not set anywhere in the routine.
 */

void
InitSynapse_IHC_Zhang(TSynapse *pthis)
{
	Float PTS,Ass,Aon,Ar_over_Ast,Ar,Ast;
	Float Pimax,spont;
	Float Prest;
	Float CG,gamma1,gamma2,tauR,tauST,kappa1,kappa2,VI0,VI1,VI;
	Float alpha,beta,theta1,theta2,theta3;
	Float PL,PG,VL,Cirest,CLrest;

	Float tdres;

	pthis->Run = NULL; /*run1syn_dynamic*/
	pthis->Run2 = RunSynapseDynamic_IHC_Zhang;
	tdres = pthis->tdres;
	/*/begin the Synapse dynamic */
	PTS = pthis->PTS;
	Ass = pthis->Ass; /* For Human, from M. G. Heinz */
	Aon = PTS * Ass;
	Ar_over_Ast = pthis->Ar_over_Ast;
	Ar = (Aon - Ass) * (Ar_over_Ast) / (1. + Ar_over_Ast);
	Ast = Aon - Ass - Ar;
	Pimax = pthis->Pimax;
	spont= pthis->spont; /*/50 in default */
	Prest = Pimax * spont/Aon;
	CG = spont * (Aon - spont)/(Aon * Prest * (1. - spont/Ass));
	gamma1 = CG / spont;
	gamma2 = CG / Ass;
	tauR= pthis->tauR;
	tauST= pthis->tauST;
	kappa1 = -( 1.0 / tauR);
	kappa2 = -( 1.0 / tauST);

	VI0 = (1.0 - Pimax / Prest) / (gamma1 * (Ar * (kappa1 - kappa2) / CG /
	  Pimax + kappa2 / Prest / gamma1 - kappa2 / Pimax / gamma2));
	VI1 = (1.0 - Pimax / Prest) / (gamma1 * (Ast * (kappa2 - kappa1) / CG /
	  Pimax + kappa1 / Prest / gamma1 - kappa1 / Pimax / gamma2));
	VI = (VI0 + VI1) / 2.0;

	alpha = gamma2 / (kappa1 * kappa2);
	beta = -(kappa1 + kappa2) * alpha;
	theta1 = alpha * Pimax / VI;
	theta2 = VI / Pimax;
	theta3 = gamma2 - 1.0 / Pimax;
	PL = (((beta - theta2 * theta3) / theta1) - 1.0) * Pimax;
	PG = 1.0 / (theta3 - 1.0 / PL);
	VL = theta1 * PL * PG;
	Cirest = spont / Prest;
	CLrest = Cirest * (Prest + PL) / PL;
	pthis->Prest = Prest;
	pthis->CIrest = Cirest;
	pthis->CLrest = CLrest;
	pthis->VI = VI;
	pthis->VL = VL;
	pthis->PL = PL;
	pthis->PG = PG;
	pthis->CG = CG;

	pthis->PPIlast = Prest;
	pthis->CLlast = CLrest;
	pthis->CIlast = Cirest;

}

/****************************** RunIHCPPI *************************************/

/*
 * Original name: runIHCPPI2
 * Original file: hc.c
 * The 'error' variable that awas in the original code was removed as it was,
 * not set anywhere in the routine.
 */

void
RunIHCPPI_IHC_Zhang(TNonLinear* p, const Float *in, Float *out,
  const int length)
{
	int register i;
	Float PPI;
	Float temp;

	for (i = 0; i < length; i++) {
		temp = p->p2 * in[i];
		PPI = (temp < 400.0)? p->p1 * log(1.0 + exp(temp)): p->p1 * temp;
		*out++ = PPI;
	}
	return;
}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 * PTS - Peak to Steady State Ratio, characteristic of PSTH.
 * The 'InitSynapse_IHC_Zhang' routine calcuate all other parameters as
 * described in the appendix.
 * iHCPPI is the recify function and parameters.
 */

BOOLN
InitProcessVariables_IHC_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_IHC_Zhang");
	int		i, cFIndex;
	Float	kKCF, temp, pst, psl, p2, p1;
	TSynapsePtr	syn;
	TNonLinearPtr	iHCPPI;
	ZhangHCPtr	p = zhangHCPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		/*** Additional update flags can be added to above line ***/
		FreeProcessVariables_IHC_Zhang();
		if ((p->iHCPPI = (TNonLinear *) calloc(_OutSig_EarObject(data)->
		  numChannels, sizeof(TNonLinear))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for iHCPPI coefficients array."),
			  funcName);
		 	return(FALSE);
		}
		if ((p->synapse = (TSynapse *) calloc(_OutSig_EarObject(data)->
		  numChannels, sizeof(TSynapse))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for synapse coefficients ")
			  wxT("array."), funcName);
		 	return(FALSE);
		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		CopyInfo_SignalData(_OutSig_EarObject(data), _InSig_EarObject(data, 0));
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			cFIndex = i / _OutSig_EarObject(data)->interleaveLevel;
			syn = &p->synapse[i];
			iHCPPI = &p->iHCPPI[i];
			syn->Ass = p->aAss;
			syn->tdres = _OutSig_EarObject(data)->dt;
			syn->cf = _OutSig_EarObject(data)->info.cFArray[cFIndex];
			syn->spont = p->spont;
			syn->Pimax = p->pIMax;
			syn->PTS = 1.0 + 9 * p->spont / (9 + p->spont);
			syn->Ar_over_Ast = p->aAROverAST;
			syn->tauR =  p->tauR;
			syn->tauST = p->tauST;
			InitSynapse_IHC_Zhang(syn);

			kKCF = 2.0 + p->ks * log10(syn->cf / 1000.0);
			if (kKCF < 1.5)
				kKCF = 1.5;
			syn->Vsat = syn->Pimax * kKCF * p->vs1 * (p->vs2 + p->spont) /
			  (p->vs3 + p->spont);

			temp = LN_2 * syn->Vsat / syn->Prest;
			pst = (temp < 400.0)? log(exp(temp) - 1.0): temp;
			psl = syn->Prest * pst / LN_2;
			p2 = pst;
			p1 = psl / pst;

			iHCPPI->psl = psl;
			iHCPPI->pst = pst;
			iHCPPI->p1 = p1;
			iHCPPI->p2 = p2;
			iHCPPI->Run = NULL; /*runIHCPPI */
			iHCPPI->Run2 = RunIHCPPI_IHC_Zhang;

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
FreeProcessVariables_IHC_Zhang(void)
{
	if (zhangHCPtr->iHCPPI) {
		free(zhangHCPtr->iHCPPI);
		zhangHCPtr->iHCPPI = NULL;
	}
	if (zhangHCPtr->synapse) {
		free(zhangHCPtr->synapse);
		zhangHCPtr->synapse = NULL;
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
RunModel_IHC_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_IHC_Zhang");
	register ChanData	 *inPtr, *outPtr;
	int		chan;
	TSynapsePtr	syn;
	TNonLinearPtr	iHCPPI;
	SignalDataPtr	outSignal;
	ZhangHCPtr	p = zhangHCPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_IHC_Zhang(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Zhang IHC Module process"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHC_Zhang(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		syn = &p->synapse[chan];
		iHCPPI = &p->iHCPPI[chan];
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		iHCPPI->Run2(iHCPPI, inPtr, outPtr, outSignal->length);
		syn->Run2(syn, outPtr, outPtr, outSignal->length);
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

