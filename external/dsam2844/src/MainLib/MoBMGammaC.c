/**********************
 *
 * File:		MoBMGammaC.c
 * Purpose:		This is an implementation of a GammaChirp filer using the
 *				UtGcFilters.c module.
 * Comments:	Written using ModuleProducer version 1.2.10 (Oct  5 2000).
 * Authors:		Masashi Unoki and L. P. O'Mard
 * Created:		06 Oct 2000
 * Updated:		30 Jan 2001
 * Copyright:	(c) 2000, 2001, 2010 Lowel P. O'Mard
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
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtFilters.h"
#include "UtGCFilters.h"
#include "UtString.h"
#include "MoBMGammaC.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMGammaCPtr	bMGammaCPtr = NULL;

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
Free_BasilarM_GammaChirp(void)
{
	if (bMGammaCPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_GammaChirp();
	Free_CFList(&bMGammaCPtr->theCFs);
	if (bMGammaCPtr->diagnosticModeList)
		free(bMGammaCPtr->diagnosticModeList);
	if (bMGammaCPtr->parList)
		FreeList_UniParMgr(&bMGammaCPtr->parList);
	if (bMGammaCPtr->parSpec == GLOBAL) {
		free(bMGammaCPtr);
		bMGammaCPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOpModeList ********************************/

/*
 * This function initialises the 'opMode' list array
 */

BOOLN
InitOpModeList_BasilarM_GammaChirp(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("FEED_BACK"),		BASILARM_GAMMACHIRP_OPMODE_FEEDBACK },
			{ wxT("FEED_FORWARD"),	BASILARM_GAMMACHIRP_OPMODE_FEEDFORWARD },
			{ wxT("NO_CONTROL"),	BASILARM_GAMMACHIRP_OPMODE_NOCONTROL },
			{ wxT(""),				BASILARM_GAMMACHIRP_OPMODE_NULL },
		};
	bMGammaCPtr->opModeList = modeList;
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
Init_BasilarM_GammaChirp(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_BasilarM_GammaChirp");

	if (parSpec == GLOBAL) {
		if (bMGammaCPtr != NULL)
			Free_BasilarM_GammaChirp();
		if ((bMGammaCPtr = (BMGammaCPtr) malloc(sizeof(BMGammaC))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMGammaCPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bMGammaCPtr->parSpec = parSpec;
	bMGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMGammaCPtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	bMGammaCPtr->opMode = BASILARM_GAMMACHIRP_OPMODE_FEEDBACK;
	bMGammaCPtr->cascade = 4;
	bMGammaCPtr->bCoeff = 1.019;
	bMGammaCPtr->cCoeff0 = 3.38;
	bMGammaCPtr->cCoeff1 = -0.107;
	bMGammaCPtr->cLowerLim = -3.5;
	bMGammaCPtr->cUpperLim = 0.0;
	bMGammaCPtr->theCFs = NULL;
	if ((bMGammaCPtr->theCFs = GenerateDefault_CFList(
	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS,
	  CFLIST_DEFAULT_LOW_FREQ, CFLIST_DEFAULT_HIGH_FREQ,
	  CFLIST_DEFAULT_BW_MODE_NAME, CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {
		NotifyError(wxT("%s: could not set default CFList."), funcName);
		return(FALSE);
	}

	InitOpModeList_BasilarM_GammaChirp();
	if ((bMGammaCPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), bMGammaCPtr->diagnosticString)) == NULL)
		return(FALSE);
	if (!SetUniParList_BasilarM_GammaChirp()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_GammaChirp();
		return(FALSE);
	}
	bMGammaCPtr->numChannels = 0;
	bMGammaCPtr->coefficientsGT = NULL;
	bMGammaCPtr->coefficientsERBGT = NULL;
	bMGammaCPtr->coefficientsAC = NULL;
	bMGammaCPtr->coefficientsLI = NULL;
	bMGammaCPtr->winPsEst = NULL;
	bMGammaCPtr->coefPsEst = 0.0;
	bMGammaCPtr->cmprs = 0.0;
	bMGammaCPtr->delaytimeLI = 0.0;
	bMGammaCPtr->cntlGammaC = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_GammaChirp(void)
{
	static const WChar *funcName = wxT("SetUniParList_BasilarM_GammaChirp");
	UniParPtr	pars;

	if ((bMGammaCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BASILARM_GAMMACHIRP_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bMGammaCPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_DIAGNOSTICMODE], wxT(
	  "DIAG_MODE"),
	  wxT("Diagnostic mode ('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC,
	  &bMGammaCPtr->diagnosticMode, bMGammaCPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_OPMODE], wxT("OP_MODE"),
	  wxT("Operation mode ('FEED_BACK', 'FEED_FORWARD' or 'NO_CONTROL')."),
	  UNIPAR_NAME_SPEC,
	  &bMGammaCPtr->opMode, bMGammaCPtr->opModeList,
	  (void * (*)) SetOpMode_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CASCADE], wxT("CASCADE"),
	  wxT("Filter cascade."),
	  UNIPAR_INT,
	  &bMGammaCPtr->cascade, NULL,
	  (void * (*)) SetCascade_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_BCOEFF], wxT("B_COEFF"),
	  wxT("Gamma distribution envelope, 'b' coefficient (units)"),
	  UNIPAR_REAL,
	  &bMGammaCPtr->bCoeff, NULL,
	  (void * (*)) SetBCoeff_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CCOEFF0], wxT("C0_COEFF"),
	  wxT("'c0' coefficient."),
	  UNIPAR_REAL,
	  &bMGammaCPtr->cCoeff0, NULL,
	  (void * (*)) SetCCoeff0_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CCOEFF1], wxT("C1_COEFF"),
	  wxT("'c1' coefficient."),
	  UNIPAR_REAL,
	  &bMGammaCPtr->cCoeff1, NULL,
	  (void * (*)) SetCCoeff1_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CLOWERLIM], wxT("LOWER_C_LIM"),
	  wxT("Lower 'c' coefficient limit."),
	  UNIPAR_REAL,
	  &bMGammaCPtr->cLowerLim, NULL,
	  (void * (*)) SetCLowerLim_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CUPPERLIM], wxT("UPPER_C_LIM"),
	  wxT("Upper 'c' coefficient limit."),
	  UNIPAR_REAL,
	  &bMGammaCPtr->cUpperLim, NULL,
	  (void * (*)) SetCUpperLim_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_THECFS], wxT("CFLIST"),
	  wxT("Centre frequency specification"),
	  UNIPAR_CFLIST,
	  &bMGammaCPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_GammaChirp);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_GammaChirp(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMGammaCPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bMGammaCPtr->parList);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_BasilarM_GammaChirp(WChar * theDiagnosticMode)
{
	static const WChar	*funcName = wxT(
	  "SetDiagnosticMode_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMGammaCPtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  bMGammaCPtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetOpMode *************************************/

/*
 * This function sets the module's opMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOpMode_BasilarM_GammaChirp(WChar * theOpMode)
{
	static const WChar	*funcName = wxT("SetOpMode_BasilarM_GammaChirp");
	int		specifier;

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOpMode,
		bMGammaCPtr->opModeList)) == BASILARM_GAMMACHIRP_OPMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOpMode);
		return(FALSE);
	}
	bMGammaCPtr->opMode = specifier;
	return(TRUE);

}

/****************************** SetCascade ************************************/

/*
 * This function sets the module's cascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCascade_BasilarM_GammaChirp(int theCascade)
{
	static const WChar	*funcName = wxT("SetCascade_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMGammaCPtr->cascade = theCascade;
	return(TRUE);

}

/****************************** SetBCoeff *************************************/

/*
 * This function sets the module's bCoeff parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBCoeff_BasilarM_GammaChirp(Float theBCoeff)
{
	static const WChar	*funcName = wxT("SetBCoeff_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMGammaCPtr->bCoeff = theBCoeff;
	return(TRUE);

}

/****************************** SetCCoeff0 ************************************/

/*
 * This function sets the module's cCoeff0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCCoeff0_BasilarM_GammaChirp(Float theCCoeff0)
{
	static const WChar	*funcName = wxT("SetCCoeff0_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMGammaCPtr->cCoeff0 = theCCoeff0;
	return(TRUE);

}

/****************************** SetCCoeff1 ************************************/

/*
 * This function sets the module's cCoeff1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCCoeff1_BasilarM_GammaChirp(Float theCCoeff1)
{
	static const WChar	*funcName = wxT("SetCCoeff1_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMGammaCPtr->cCoeff1 = theCCoeff1;
	return(TRUE);

}

/****************************** SetCLowerLim **********************************/

/*
 * This function sets the module's cLowerLim parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCLowerLim_BasilarM_GammaChirp(Float theCLowerLim)
{
	static const WChar	*funcName = wxT("SetCLowerLim_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMGammaCPtr->cLowerLim = theCLowerLim;
	return(TRUE);

}

/****************************** SetCUpperLim **********************************/

/*
 * This function sets the module's cUpperLim parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCUpperLim_BasilarM_GammaChirp(Float theCUpperLim)
{
	static const WChar	*funcName = wxT("SetCUpperLim_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMGammaCPtr->cUpperLim = theCUpperLim;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_GammaChirp(CFListPtr theCFList)
{
	static const WChar	*funcName = wxT("SetCFList_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not correctly set."),
		  funcName);
		return(FALSE);
	}
	if (bMGammaCPtr->theCFs != NULL)
		Free_CFList(&bMGammaCPtr->theCFs);
	bMGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMGammaCPtr->theCFs = theCFList;
	return(TRUE);

}

/****************************** SetBandWidths *********************************/

/*
 * This function sets the band width mode for the gamma tone filters.
 * The band width mode defines the function for calculating the gamma tone
 * filter band width 3 dB down.
 * No checks are made on the correct length for the bandwidth array.
 */

BOOLN
SetBandWidths_BasilarM_GammaChirp(WChar *theBandwidthMode, Float
  *theBandwidths)
{
	static const WChar	*funcName = wxT("SetBandWidths_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(bMGammaCPtr->theCFs, theBandwidthMode,
	  theBandwidths)) {
		NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
		return(FALSE);
	}
	bMGammaCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

CFListPtr
GetCFListPtr_BasilarM_GammaChirp(void)
{
	static const WChar	*funcName = wxT("GetCFListPtr_BasilarM_GammaChirp");

	if (bMGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMGammaCPtr->theCFs == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly ")
		  wxT("set.  NULL returned."), funcName);
		return(NULL);
	}
	return(bMGammaCPtr->theCFs);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_GammaChirp(void)
{
	DPrint(wxT("GammaChirp BM Filter Module Parameters:-\n"));
	DPrint(wxT("\tDiagnostic mode: %s,"), bMGammaCPtr->diagnosticModeList[
	  bMGammaCPtr->diagnosticMode].name);
	DPrint(wxT("\tOperation mode: %s \n"), bMGammaCPtr->opModeList[
	  bMGammaCPtr->opMode].name);
	DPrint(wxT("\tFilter cascade = %d,"), bMGammaCPtr->cascade);
	DPrint(wxT("\tCoefficient, b = %g,\n"), bMGammaCPtr->bCoeff);
	DPrint(wxT("\tCoefficient, c0 = %g,"), bMGammaCPtr->cCoeff0);
	DPrint(wxT("\tCoefficient, c1 = %g\n"), bMGammaCPtr->cCoeff1);
	DPrint(wxT("\tLower/upper c coefficient limits = %g / %g\n"),
	  bMGammaCPtr->cLowerLim, bMGammaCPtr->cUpperLim);
	PrintPars_CFList(bMGammaCPtr->theCFs);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_GammaChirp(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_BasilarM_GammaChirp");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bMGammaCPtr = (BMGammaCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_GammaChirp(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_GammaChirp");

	if (!SetParsPointer_BasilarM_GammaChirp(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_GammaChirp(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMGammaCPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_BasilarM_GammaChirp;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_GammaChirp;
	theModule->PrintPars = PrintPars_BasilarM_GammaChirp;
	theModule->RunProcess = RunModel_BasilarM_GammaChirp;
	theModule->SetParsPointer = SetParsPointer_BasilarM_GammaChirp;
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
CheckData_BasilarM_GammaChirp(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_BasilarM_GammaChirp");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_BasilarM_GammaChirp(void)
{
	int	nch;
	BMGammaCPtr	p = bMGammaCPtr;


				/* Free of GammaToneCoeffs_Filters */
	if (p->coefficientsERBGT) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeERBGammaToneCoeffs_GCFilters(&p->coefficientsERBGT[nch]);
		free(p->coefficientsERBGT);
		p->coefficientsERBGT = NULL;
	}
	if (p->coefficientsGT) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeGammaToneCoeffs_Filters(&p->coefficientsGT[nch]);
		free(p->coefficientsGT);
		p->coefficientsGT = NULL;
	}
				/* Free of AsymCmpCoeffs_Filters */
	if (p->coefficientsAC) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeAsymCmpCoeffs_GCFilters(&p->coefficientsAC[nch]);
		free(p->coefficientsAC);
		p->coefficientsAC = NULL;
	}
				/* Free of LeakyIntCoeffs_GCFilters */
	if (p->coefficientsLI) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeLeakyIntCoeffs_GCFilters(&p->coefficientsLI[nch]);
		free(p->coefficientsLI);
		p->coefficientsLI = NULL;
	}
				/* Free of ERBWindow_Filters */
	if (p->winPsEst) {
 		FreeFloatArray_Common(&p->winPsEst);
		free(p->winPsEst);
		p->winPsEst = NULL;
	}
				/* Free of CntlGammaChirp_Filters */
	if (p->cntlGammaC) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeCntlGammaChirp_GCFilters(&p->cntlGammaC[nch]);
		free(p->cntlGammaC);
		p->cntlGammaC = NULL;
	}

	p->updateProcessVariablesFlag = TRUE;

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_BasilarM_GammaChirp(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_BasilarM_GammaChirp");
	int	nch, nsmpl, cFIndex, stateVectorLength;
	Float	sampleRate, *ptr;
	BMGammaCPtr	p = bMGammaCPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  p->theCFs->updateFlag) {
		FreeProcessVariables_BasilarM_GammaChirp();
		p->numChannels = _OutSig_EarObject(data)->numChannels;
				/* Memory allocate of GammaToneCoeffsPtr */
		if (p->cascade == 4) {
			if ((p->coefficientsERBGT = (ERBGammaToneCoeffsPtr *) calloc(
			  p->numChannels, sizeof(ERBGammaToneCoeffsPtr))) == NULL) {
				NotifyError(wxT("%s: Out of memory."), funcName);
				return(FALSE);
			}
		} else {
			if ((p->coefficientsGT = (GammaToneCoeffsPtr *) calloc(
			  p->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
				NotifyError(wxT("%s: Out of memory."), funcName);
				return(FALSE);
			}
		}
				/* Memory allocate of AsymCmpCoeffsPtr */
		if ((p->coefficientsAC = (AsymCmpCoeffsPtr *) calloc(p->numChannels,
		  sizeof(AsymCmpCoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory."), funcName);
		 	return(FALSE);
		}
				/* Memory allocate of OnePoleCoeffsPtr */
		if ((p->coefficientsLI = (OnePoleCoeffsPtr *) calloc(p->numChannels,
		  sizeof(OnePoleCoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory."), funcName);
		 	return(FALSE);
		}

		if ((p->winPsEst = (Float *) calloc(p->numChannels * p->numChannels,
		  sizeof(Float))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory."), funcName);
		 	return(FALSE);
		}

		if ((p->cntlGammaC = (CntlGammaCPtr *) calloc(p->numChannels,
		  sizeof(CntlGammaCPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory."), funcName);
		 	return(FALSE);
		}

		sampleRate = 1.0 / _InSig_EarObject(data, 0)->dt;

		p->coefPsEst = BASILARM_GAMMACHIRP_COEFF_PS_EST;
		p->cmprs = BASILARM_GAMMACHIRP_COEFF_CMPRS;
		p->delaytimeLI = BASILARM_GAMMACHIRP_DELAY_TIME_LI;

		if (p->cascade == 4) {
			for (nch = 0; nch < p->numChannels; nch++) {
				cFIndex = nch / _InSig_EarObject(data, 0)->interleaveLevel;
				p->coefficientsERBGT[nch] = InitERBGammaToneCoeffs_GCFilters(
				  p->theCFs->frequency[cFIndex], p->theCFs->bandwidth[cFIndex],
				  p->bCoeff, p->cascade, sampleRate);
			}
		} else {
			for (nch = 0; nch < p->numChannels; nch++) {
				cFIndex = nch / _InSig_EarObject(data, 0)->interleaveLevel;
				if ((p->coefficientsGT[nch] = InitGammaToneCoeffs_Filters(
				  p->theCFs->frequency[cFIndex], p->theCFs->bandwidth[cFIndex],
				  p->cascade, sampleRate)) == NULL) {
					NotifyError(wxT("%s: Could not initialise coefficients ")
					  wxT("for channel %d."), funcName, nch);
					return(FALSE);
				}
			}
		}
		for (nch = 0; nch < p->numChannels; nch++) {
			p->coefficientsAC[nch] = InitAsymCmpCoeffs_GCFilters();
			p->coefficientsLI[nch] = InitLeakyIntCoeffs_GCFilters(
			  p->delaytimeLI, sampleRate);
			p->cntlGammaC[nch] = InitCntlGammaChirp_GCFilters();
		}
		p->winPsEst = InitERBWindow_GCFilters(p->theCFs->eRBDensity,
		  p->numChannels);

		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data), wxT("Frequency (Hz)"));
		SetInfoChannelLabels_SignalData(_OutSig_EarObject(data), p->theCFs->frequency);
		SetInfoCFArray_SignalData(_OutSig_EarObject(data), p->theCFs->frequency);
		p->updateProcessVariablesFlag = FALSE;
		p->theCFs->updateFlag = FALSE;

	} else if (data->timeIndex == PROCESS_START_TIME) {
		stateVectorLength = p->cascade *
		  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER;
		if (p->cascade == 4) {
			for (nch = 0; nch < _OutSig_EarObject(data)->numChannels; nch++) {
				ptr = p->coefficientsERBGT[nch]->stateVector;
				for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
					*ptr++ = 0.0;
			}
		} else {
			for (nch = 0; nch < _OutSig_EarObject(data)->numChannels; nch++) {
				ptr = p->coefficientsGT[nch]->stateVector;
				for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
					*ptr++ = 0.0;
			}
		}
		stateVectorLength = GCFILTERS_NUM_CASCADE_ACF_FILTER *
			GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER;
		for (nch = 0; nch < _OutSig_EarObject(data)->numChannels; nch++) {
			ptr = p->coefficientsAC[nch]->stateFVector;
			for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
				*ptr++ = 0.0;
			ptr = p->coefficientsAC[nch]->stateBVector;
			for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
				*ptr++ = 0.0;
		}
		stateVectorLength = GCFILTERS_NUM_LI_STATE_VARS_PER_FILTER;
		for (nch = 0; nch < _OutSig_EarObject(data)->numChannels; nch++) {
			ptr = p->coefficientsLI[nch]->stateVector;
			for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
				*ptr++ = 0.0;
		}
		for (nch = 0; nch < _OutSig_EarObject(data)->numChannels; nch++) {
			p->cntlGammaC[nch]->outSignalLI = 0.0;
			p->cntlGammaC[nch]->aEst = 0.0;
			p->cntlGammaC[nch]->psEst = 0.0;
			if (p->opMode == BASILARM_GAMMACHIRP_OPMODE_NOCONTROL)
					/* opMode=1 -> wxT("NC") */
				p->cntlGammaC[nch]->cEst = p->cCoeff0;
			else
				p->cntlGammaC[nch]->cEst = 0.0;
		}
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
RunModel_BasilarM_GammaChirp(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_BasilarM_GammaChirp");
	uShort	totalChannels;
	int		nch, cEstCnt, cFIndex;
	Float	sampleRate;
	SignalDataPtr	outSignal;
	ChanLen	nsmpl;

	if (!data->threadRunFlag) {
		if (!CheckData_BasilarM_GammaChirp(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("GammaChirp basilar membrane ")
		  wxT("filtering (Slaney GT)"));
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
		totalChannels = bMGammaCPtr->theCFs->numChannels * _InSig_EarObject(
		  data, 0)->numChannels;
		if (!InitOutTypeFromInSignal_EarObject(data, totalChannels)) {
			NotifyError(wxT("%s: Cannot initialise output channel."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_BasilarM_GammaChirp(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		/* Beginig of GammaChirp_Filters */

		for (nch = 0; nch < _OutSig_EarObject(data)->numChannels; nch++) {
			bMGammaCPtr->cntlGammaC[nch]->outSignalLI = 0.0;
			bMGammaCPtr->cntlGammaC[nch]->aEst = 0.0;
			bMGammaCPtr->cntlGammaC[nch]->psEst = 0.0;
			if (bMGammaCPtr->opMode == BASILARM_GAMMACHIRP_OPMODE_NOCONTROL)
				 			/* opMode=1 -> wxT("NC") */
				bMGammaCPtr->cntlGammaC[nch]->cEst = bMGammaCPtr->cCoeff0;
			else
				bMGammaCPtr->cntlGammaC[nch]->cEst = 0.0;
		}

		if (data->initThreadRunFlag)
			return(TRUE);
	}
	InitOutDataFromInSignal_EarObject(data);
	outSignal = _OutSig_EarObject(data);
	if (bMGammaCPtr->cascade == 4)
		ERBGammaTone_GCFilters(outSignal, bMGammaCPtr->coefficientsERBGT);
	else
		GammaTone_Filters(outSignal, bMGammaCPtr->coefficientsGT);

	cEstCnt = 0;
	for (nch = outSignal->offset; nch < outSignal->numChannels; nch++)
		if (bMGammaCPtr->cntlGammaC[nch]->cEst != 0.0)
			cEstCnt++;

	if ((bMGammaCPtr->opMode != BASILARM_GAMMACHIRP_OPMODE_NOCONTROL) ||
				(cEstCnt != 0)) {	/* opMode=2 -> "NC" */

		sampleRate = 1.0 / _InSig_EarObject(data, 0)->dt;
		for (nsmpl = 0; nsmpl < outSignal->length; nsmpl++) {

			if (bMGammaCPtr->opMode == BASILARM_GAMMACHIRP_OPMODE_FEEDFORWARD) {
						/* opMode=1 -> "FF" */
				CntlGammaChirp_GCFilters(outSignal, nsmpl,
					bMGammaCPtr->cntlGammaC, bMGammaCPtr->cCoeff0,
					bMGammaCPtr->cCoeff1, bMGammaCPtr->cLowerLim,
					bMGammaCPtr->cUpperLim, bMGammaCPtr->winPsEst,
					bMGammaCPtr->coefPsEst, bMGammaCPtr->cmprs,
					bMGammaCPtr->coefficientsLI);
            }

			for (nch = outSignal->offset; nch < outSignal->numChannels; nch++) {
				cFIndex = nch / _InSig_EarObject(data, 0)->interleaveLevel;
				CalcAsymCmpCoeffs_GCFilters(bMGammaCPtr->coefficientsAC[nch],
					bMGammaCPtr->theCFs->frequency[cFIndex],
					bMGammaCPtr->theCFs->bandwidth[cFIndex],
					bMGammaCPtr->bCoeff, bMGammaCPtr->cntlGammaC[nch]->cEst,
					bMGammaCPtr->cascade, sampleRate);
			}

			AsymCmp_GCFilters(outSignal, nsmpl, bMGammaCPtr->coefficientsAC);

			if (bMGammaCPtr->opMode == BASILARM_GAMMACHIRP_OPMODE_FEEDBACK) {
						/* opMode=0 -> "FB" */
				CntlGammaChirp_GCFilters(outSignal, nsmpl,
					bMGammaCPtr->cntlGammaC, bMGammaCPtr->cCoeff0,
					bMGammaCPtr->cCoeff1, bMGammaCPtr->cLowerLim,
					bMGammaCPtr->cUpperLim, bMGammaCPtr->winPsEst,
					bMGammaCPtr->coefPsEst, bMGammaCPtr->cmprs,
					bMGammaCPtr->coefficientsLI);
			}
		}
	}

	/* end of GammaChirp_GCFilters */

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
