/**********************
 *
 * File:		MoBMDGammaC.c
 * Purpose:		An implementation of the dynamic compressive gammachirp: Irino T.
 * 				and Patterson R. D. (2006) "A Dynamic Compressive Gammachirp
 *				Auditory Filterbank", IEEE Transcation on Audio, Speech and
 * 				Language Processing, Vol. 14, No. 6, pp 2222-2232.
 * Comments:	Written using ModuleProducer version 1.6.0 (Jun 20 2007).
 *				This code module was revised from the implementation in the
 *				AIM2007a matlab code.
 * Author:		L. P. O'Mard
 * Created:		18 Jun 2007
 * Updated:
 * Copyright:	(c) 2007, 2010 Lowel P. O'Mard
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

/******************************************************************************/
/****************************** HAVE_FFTW3 compile ****************************/
/******************************************************************************/

#if	HAVE_FFTW3

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
#include "UtFFT.h"
#include "UtGCFilters.h"
#include "MoBMDGammaC.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMDGammaCPtr	bMDGammaCPtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** PanelList *************************************/

/*
 * This routine returns the name specifier for the display panel list.
 */

NameSpecifier *
GetPanelList_BasilarM_GammaChirp_Dyn(int index)
{
	static NameSpecifier	list[] = {

				{ wxT("General"),		BM_GC_DYN_DIAGNOSTICMODE },
				{ wxT("Gammachirp"),	BM_GC_DYN_GC_CTRL },
				{ wxT("Level Est."),	BM_GC_DYN_LVEST_LCTERB },
				{ wxT("CF Info."),		BM_GC_DYN_THECFS },
				{ NULL,					BM_GC_DYN_NULL }
			};
	;
	return(&list[index]);

}

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_BasilarM_GammaChirp_Dyn(void)
{
	/* static const WChar	*funcName = wxT("Free_BasilarM_GammaChirp_Dyn"); */

	if (bMDGammaCPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_GammaChirp_Dyn();
	if (bMDGammaCPtr->gC_frat) {
		free(bMDGammaCPtr->gC_frat);
		bMDGammaCPtr->gC_frat = NULL;
	}
	if (bMDGammaCPtr->gC_b2) {
		free(bMDGammaCPtr->gC_b2);
		bMDGammaCPtr->gC_b2 = NULL;
	}
	if (bMDGammaCPtr->gC_c2) {
		free(bMDGammaCPtr->gC_c2);
		bMDGammaCPtr->gC_c2 = NULL;
	}
	if (bMDGammaCPtr->lvEst_Pwr) {
		free(bMDGammaCPtr->lvEst_Pwr);
		bMDGammaCPtr->lvEst_Pwr = NULL;
	}
	Free_CFList(&bMDGammaCPtr->theCFs);
	if (bMDGammaCPtr->parList)
		FreeList_UniParMgr(&bMDGammaCPtr->parList);
	if (bMDGammaCPtr->parSpec == GLOBAL) {
		free(bMDGammaCPtr);
		bMDGammaCPtr = NULL;
	}
	return(TRUE);

}

/****************************** GC_ctrlList ************************************/

/*
 * This function initialises the 'gC_ctrl' list array
 */

NameSpecifier *
GC_CtrlList_BasilarM_GammaChirp_Dyn(int	index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("FIXED"),			BM_GC_DYN_GC_CTRL_FIXED },
			{ wxT("TIME_VARYING"),	BM_GC_DYN_GC_CTRL_TIME_VARYING },
			{ NULL,					BM_GC_DYN_GC_CTRL_NULL },
		};
	return(&modeList[index]);

}

/****************************** OutputModeList *********************************/

/*
 * This function initialises the 'outputMode' list array
 */

NameSpecifier *
OutputModeList_BasilarM_GammaChirp_Dyn(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("PASSIVE"),				BM_GC_DYN_OUTPUTMODE_PASSIVE },
			{ wxT("DYNAMIC_COMPRESSIVE"),	BM_GC_DYN_OUTPUTMODE_DYNA_COMP },
			{ wxT("TOTAL"),					BM_GC_DYN_OUTPUTMODE_TOTAL },
			{ NULL,							BM_GC_DYN_OUTPUTMODE_NULL },
		};
	return(&modeList[index]);

}

/****************************** SetDefaultGC_arrayNArrays *********************/

/*
 * This function sets the default arrays and array values for the
 * 'gC_arrayN' variable.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetDefaultGC_arrayNArrays_BasilarM_GammaChirp_Dyn(void)
{
	static const WChar	*funcName = wxT("SetDefaultGC_arrayNArrays_BasilarM_GammaChirp_Dyn");
	int		i;
	Float	gC_frat[] = {0.466, 0.0, 0.0109, 0.0};
	Float	gC_b2[] = {2.17, 0.0, 0.0, 0.0};
	Float	gC_c2[] = {2.2, 0.0, 0.0, 0.0};

	if (!AllocGC_arrayN_BasilarM_GammaChirp_Dyn(BM_GC_DYN_NUM_GC_ARRAYS_N)) {
		NotifyError(wxT("%s: Could not allocate default arrays."), funcName);
		return(FALSE);
	}
	for (i = 0; i < bMDGammaCPtr->gC_arrayN; i++) {
		bMDGammaCPtr->gC_frat[i] = gC_frat[i];
		bMDGammaCPtr->gC_b2[i] = gC_b2[i];
		bMDGammaCPtr->gC_c2[i] = gC_c2[i];
	}
	return(TRUE);

}

/****************************** SetDefaultLvEst_arrayNArrays ******************/

/*
 * This function sets the default arrays and array values for the
 * 'lvEst_arrayN' variable.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetDefaultLvEst_arrayNArrays_BasilarM_GammaChirp_Dyn(void)
{
	static const WChar	*funcName = wxT("SetDefaultLvEst_arrayNArrays_BasilarM_GammaChirp_Dyn");
	int		i;
	Float	lvEst_Pwr[] = {1.5, 0.5};

	if (!AllocLvEst_arrayN_BasilarM_GammaChirp_Dyn(BM_GC_DYN_NUM_LVLEST_ARRAYS_N)) {
		NotifyError(wxT("%s: Could not allocate default arrays."), funcName);
		return(FALSE);
	}
	for (i = 0; i < bMDGammaCPtr->lvEst_arrayN; i++) {
		bMDGammaCPtr->lvEst_Pwr[i] = lvEst_Pwr[i];
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
Init_BasilarM_GammaChirp_Dyn(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_BasilarM_GammaChirp_Dyn");

	if (parSpec == GLOBAL) {
		if (bMDGammaCPtr != NULL)
			Free_BasilarM_GammaChirp_Dyn();
		if ((bMDGammaCPtr = (BMDGammaCPtr) malloc(sizeof(BMDGammaC))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMDGammaCPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bMDGammaCPtr->parSpec = parSpec;
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->diagMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	bMDGammaCPtr->outputMode = BM_GC_DYN_OUTPUTMODE_TOTAL;
	bMDGammaCPtr->pGCCarrierMode = GCFILTER_CARRIER_MODE_COS;
	bMDGammaCPtr->peakSpectrumNormMode = GENERAL_BOOLEAN_ON;
	bMDGammaCPtr->gC_ctrl = BM_GC_DYN_GC_CTRL_TIME_VARYING;
	bMDGammaCPtr->gC_n = 4;
	bMDGammaCPtr->gC_b1 = 1.81;
	bMDGammaCPtr->gC_c1 = -2.96;
	bMDGammaCPtr->gC_arrayN = 0;
	bMDGammaCPtr->gC_frat = NULL;
	bMDGammaCPtr->gC_b2 = NULL;
	bMDGammaCPtr->gC_c2 = NULL;
	bMDGammaCPtr->gC_phase = 0.0;
	bMDGammaCPtr->gC_gainRefdB = 70.0;
	bMDGammaCPtr->gC_gainCmpnstdB = -1.0;
	bMDGammaCPtr->lvEst_LctERB = 1.5;
	bMDGammaCPtr->lvEst_DecayHL = 0.5;
	bMDGammaCPtr->lvEst_b2 = 2.17;
	bMDGammaCPtr->lvEst_c2 = 2.2;
	bMDGammaCPtr->lvEst_frat = 1.08;
	bMDGammaCPtr->lvEst_RMStoSPLdB = 30.0;
	bMDGammaCPtr->lvEst_Weight = 0.5;
	bMDGammaCPtr->lvEst_RefdB = 50.0;
	bMDGammaCPtr->lvEst_arrayN = 0;
	bMDGammaCPtr->lvEst_Pwr = NULL;
	if ((bMDGammaCPtr->theCFs = GenerateDefault_CFList(
	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS,
	  CFLIST_DEFAULT_LOW_FREQ, CFLIST_DEFAULT_HIGH_FREQ,
	  CFLIST_DEFAULT_BW_MODE_NAME, CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {
		NotifyError(wxT("%s: Could not set default CFList."), funcName);
		return(FALSE);
	}

	if (!SetDefaultGC_arrayNArrays_BasilarM_GammaChirp_Dyn()) {
		NotifyError(wxT("%s: Could not set the default 'gC_arrayN' arrays."),
		  funcName);
		return(FALSE);
	}
	if (!SetDefaultLvEst_arrayNArrays_BasilarM_GammaChirp_Dyn()) {
		NotifyError(wxT(
		  "%s: Could not set the default 'lvEst_arrayN' arrays."), funcName);
		return(FALSE);
	}

	if ((bMDGammaCPtr->diagModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), bMDGammaCPtr->diagFileName)) == NULL)
		return(FALSE);
	if (!SetUniParList_BasilarM_GammaChirp_Dyn()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_GammaChirp_Dyn();
		return(FALSE);
	}
	bMDGammaCPtr->numChannels = 0;
	bMDGammaCPtr->fp = NULL;
	DSAM_strcpy(bMDGammaCPtr->diagFileName, DEFAULT_FILE_NAME);
	bMDGammaCPtr->genChanInfo = NULL;
	bMDGammaCPtr->pGCoeffs = NULL;
	bMDGammaCPtr->aCFCoeffLvlEst = NULL;
	bMDGammaCPtr->aCFCoeffSigPath = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_GammaChirp_Dyn(void)
{
	static const WChar	*funcName = wxT("SetUniParList_BasilarM_GammaChirp_Dyn");
	UniParPtr	pars;

	if ((bMDGammaCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BM_GC_DYN_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	SetGetPanelListFunc_UniParMgr(bMDGammaCPtr->parList,
	  GetPanelList_BasilarM_GammaChirp_Dyn);

	pars = bMDGammaCPtr->parList->pars;
	SetPar_UniParMgr(&pars[BM_GC_DYN_DIAGNOSTICMODE], wxT("DIAGNOSTIC_MODE"),
	  wxT("Diagnostic mode. Outputs internal states of running model in non-")
	    wxT("threaded mode('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &bMDGammaCPtr->diagMode, bMDGammaCPtr->diagModeList,
	  (void * (*)) SetDiagMode_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode: 'passive', 'dynamic_compressive' or 'total' response paths."),
	  UNIPAR_NAME_SPEC,
	  &bMDGammaCPtr->outputMode, OutputModeList_BasilarM_GammaChirp_Dyn(0),
	  (void * (*)) SetOutputMode_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_PGCCARRIERMODE], wxT("CARRIER_MODE"),
	  wxT("Carrier mode ('cos','sin','complex' or 'envelope')"),
	  UNIPAR_NAME_SPEC,
	  &bMDGammaCPtr->pGCCarrierMode, PGCCarrierList_GCFilters(0),
	  (void * (*)) SetPGCCarrierMode_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_PEAKSPECTRUMNORMMODE], wxT("PEAK_NORM_MODE"),
	  wxT("Normalization of peak spectrum level ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &bMDGammaCPtr->peakSpectrumNormMode, NULL,
	  (void * (*)) SetPeakSpectrumNormMode_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_CTRL], wxT("CTRL_MODE"),
	  wxT("Control mode ('fixed' or 'time_varying')."),
	  UNIPAR_NAME_SPEC,
	  &bMDGammaCPtr->gC_ctrl, GC_CtrlList_BasilarM_GammaChirp_Dyn(0),
	  (void * (*)) SetGC_ctrl_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_N], wxT("ORDER_N"),
	  wxT("Order of Gamma function t^(n-1)."),
	  UNIPAR_INT,
	  &bMDGammaCPtr->gC_n, NULL,
	  (void * (*)) SetGC_n_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_B1], wxT("COEFF_B"),
	  wxT("Coeficient B -> exp(-2*pi*B*ERB(f)"),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->gC_b1, NULL,
	  (void * (*)) SetGC_b1_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_C1], wxT("COEFF_C"),
	  wxT("Coeficient C -> exp(j*2*pi*Frs + C*ln(t))"),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->gC_c1, NULL,
	  (void * (*)) SetGC_c1_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_ARRAYN], wxT("GC_ARRAY_N"),
	  wxT("No. of gamma chirp array parameters (this cannot be changed)"),
	  UNIPAR_INT_AL,
	  &bMDGammaCPtr->gC_arrayN, NULL,
	  (void * (*)) SetGC_arrayN_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_FRAT], wxT("GC_FRAT"),
	  wxT("Gamma chirp frat values."),
	  UNIPAR_REAL_ARRAY,
	  &bMDGammaCPtr->gC_frat, &bMDGammaCPtr->gC_arrayN,
	  (void * (*)) SetIndividualGC_frat_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_B2], wxT("GC_B2"),
	  wxT("Gamma chirp b2 cofficient values."),
	  UNIPAR_REAL_ARRAY,
	  &bMDGammaCPtr->gC_b2, &bMDGammaCPtr->gC_arrayN,
	  (void * (*)) SetIndividualGC_b2_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_C2], wxT("GC_C2"),
	  wxT("Gamma chirp c2 cofficient values."),
	  UNIPAR_REAL_ARRAY,
	  &bMDGammaCPtr->gC_c2, &bMDGammaCPtr->gC_arrayN,
	  (void * (*)) SetIndividualGC_c2_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_PHASE], wxT("GC_PHASE"),
	  wxT("Gamma chirp Starting phase (radians)."),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->gC_phase, NULL,
	  (void * (*)) SetGC_phase_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_GAINREFDB], wxT("GC_GAIN_REFDB"),
	  wxT("Gain at reference level for normalisation (dB)"),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->gC_gainRefdB, NULL,
	  (void * (*)) SetGC_gainRefdB_BM_GC_DYN);
	SetPar_UniParMgr(&pars[BM_GC_DYN_GC_GAINCMPNSTDB], wxT("GC_GAIN_CMPPNSTDB"),
	  wxT("Gain compensation value (dB)."),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->gC_gainCmpnstdB, NULL,
	  (void * (*)) SetGC_gainCmpnstdB_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_LCTERB], wxT("LVL_EST_LCTERB"),
	  wxT("Location of Level Estimation pGC relative to the signal pGC in ERB"),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->lvEst_LctERB, NULL,
	  (void * (*)) SetLvEst_LctERB_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_DECAYHL], wxT("LVL_EST_DECAYHL"),
	  wxT("Level Estimation "),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->lvEst_DecayHL, NULL,
	  (void * (*)) SetLvEst_DecayHL_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_B2], wxT("LVL_EST_B2"),
	  wxT("Level Estimation B2 parameter."),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->lvEst_b2, NULL,
	  (void * (*)) SetLvEst_b2_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_C2], wxT("LVL_EST_C2"),
	  wxT("Level Estimation C2 parameter."),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->lvEst_c2, NULL,
	  (void * (*)) SetLvEst_c2_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_FRAT], wxT("LVL_EST_FRAT"),
	  wxT("Level Estimation FRAT parameter."),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->lvEst_frat, NULL,
	  (void * (*)) SetLvEst_frat_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_RMSTOSPLDB], wxT("LVL_EST_RMSTOSPLDB"),
	  wxT("Level Estimation RMSTOSPLDB for other IHC (e.g. Meddis) hair cells parameter."),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->lvEst_RMStoSPLdB, NULL,
	  (void * (*)) SetLvEst_RMStoSPLdB_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_WEIGHT], wxT("LVL_EST_WEIGHT"),
	  wxT("Level Estimation 'weight' parameter."),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->lvEst_Weight, NULL,
	  (void * (*)) SetLvEst_Weight_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_REFDB], wxT("LVL_EST_REFDB"),
	  wxT("Level Estimation reference Ppgc level parameter."),
	  UNIPAR_REAL,
	  &bMDGammaCPtr->lvEst_RefdB, NULL,
	  (void * (*)) SetLvEst_RefdB_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_ARRAYN], wxT("LVL_EST_ARRAYN"),
	  wxT("Level Estimation Array N (cannot be changed)."),
	  UNIPAR_INT_AL,
	  &bMDGammaCPtr->lvEst_arrayN, NULL,
	  (void * (*)) SetLvEst_arrayN_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_LVEST_PWR], wxT("LVL_EST_PWR"),
	  wxT("Level Estimation PWR parameters."),
	  UNIPAR_REAL_ARRAY,
	  &bMDGammaCPtr->lvEst_Pwr, &bMDGammaCPtr->lvEst_arrayN,
	  (void * (*)) SetIndividualLvEst_Pwr_BasilarM_GammaChirp_Dyn);
	SetPar_UniParMgr(&pars[BM_GC_DYN_THECFS], wxT("CFLIST"),
	  wxT("Centre frequency specification"),
	  UNIPAR_CFLIST,
	  &bMDGammaCPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_GammaChirp_Dyn);

	bMDGammaCPtr->parList->pars[BM_GC_DYN_GC_ARRAYN].enabled = FALSE;
	bMDGammaCPtr->parList->pars[BM_GC_DYN_LVEST_ARRAYN].enabled = FALSE;
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_GammaChirp_Dyn(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMDGammaCPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(bMDGammaCPtr->parList);

}

/****************************** AllocGC_arrayN ********************************/

/*
 * This function allocates the memory for the module arrays.
 * It will assume that nothing needs to be done if the 'gC_arrayN'
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'gC_arrayN'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocGC_arrayN_BasilarM_GammaChirp_Dyn(int gC_arrayN)
{
	static const WChar	*funcName = wxT("AllocGC_arrayN_BasilarM_GammaChirp_Dyn");

	if (gC_arrayN == bMDGammaCPtr->gC_arrayN)
		return(TRUE);
	if (bMDGammaCPtr->gC_frat)
		free(bMDGammaCPtr->gC_frat);
	if ((bMDGammaCPtr->gC_frat = (Float *) calloc(gC_arrayN, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' gC_frat."),
		  funcName, gC_arrayN);
		return(FALSE);
	}
	if (bMDGammaCPtr->gC_b2)
		free(bMDGammaCPtr->gC_b2);
	if ((bMDGammaCPtr->gC_b2 = (Float *) calloc(gC_arrayN, sizeof(Float)))
	  == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' gC_b2."),
		  funcName, gC_arrayN);
		return(FALSE);
	}
	if (bMDGammaCPtr->gC_c2)
		free(bMDGammaCPtr->gC_c2);
	if ((bMDGammaCPtr->gC_c2 = (Float *) calloc(gC_arrayN, sizeof(Float)))
	  == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' gC_c2."),
		  funcName, gC_arrayN);
		return(FALSE);
	}
	bMDGammaCPtr->gC_arrayN = gC_arrayN;
	return(TRUE);

}

/****************************** AllocLvEst_arrayN *****************************/

/*
 * This function allocates the memory for the module arrays.
 * It will assume that nothing needs to be done if the 'lvEst_arrayN'
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'lvEst_arrayN'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocLvEst_arrayN_BasilarM_GammaChirp_Dyn(int lvEst_arrayN)
{
	static const WChar	*funcName = wxT("AllocLvEst_arrayN_BasilarM_GammaChirp_Dyn");

	if (lvEst_arrayN == bMDGammaCPtr->lvEst_arrayN)
		return(TRUE);
	if (bMDGammaCPtr->lvEst_Pwr)
		free(bMDGammaCPtr->lvEst_Pwr);
	if ((bMDGammaCPtr->lvEst_Pwr = (Float *) calloc(lvEst_arrayN, sizeof(
	  Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' lvEst_Pwr."),
		  funcName, lvEst_arrayN);
		return(FALSE);
	}
	bMDGammaCPtr->lvEst_arrayN = lvEst_arrayN;
	return(TRUE);

}

/****************************** SetDiagMode *****************************/

/*
 * This function sets the module's diagMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagMode_BasilarM_GammaChirp_Dyn(WChar * theDiagMode)
{
	static const WChar	*funcName = wxT("SetDiagMode_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->diagMode = IdentifyDiag_NSpecLists(theDiagMode,
	  bMDGammaCPtr->diagModeList);
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_BasilarM_GammaChirp_Dyn(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT(
	  "SetOutputMode_BasilarM_GammaChirp_Dyn");
	int		specifier;

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
	  OutputModeList_BasilarM_GammaChirp_Dyn(0))) ==
	  BM_GC_DYN_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->outputMode = specifier;
	return(TRUE);

}
/****************************** SetPGCCarrierMode *****************************/

/*
 * This function sets the module's pGCCarrierMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPGCCarrierMode_BasilarM_GammaChirp_Dyn(WChar * thePGCCarrierMode)
{
	static const WChar	*funcName = wxT(
	  "SetPGCCarrierMode_BasilarM_GammaChirp_Dyn");
	int		specifier;

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(thePGCCarrierMode,
		PGCCarrierList_GCFilters(0))) == GCFILTER_CARRIER_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, thePGCCarrierMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->pGCCarrierMode = specifier;
	return(TRUE);

}

/****************************** SetPeakSpectrumNormMode ***********************/

/*
 * This function sets the module's peakSpectrumNormMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPeakSpectrumNormMode_BasilarM_GammaChirp_Dyn(WChar *thePeakSpectrumNormMode)
{
	static const WChar	*funcName = wxT(
	  "SetPeakSpectrumNormMode_BasilarM_GammaChirp_Dyn");
	int		specifier;

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if ((specifier = Identify_NameSpecifier(thePeakSpectrumNormMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  thePeakSpectrumNormMode);
		return(FALSE);
	}
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->peakSpectrumNormMode = specifier;
	return(TRUE);

}

/****************************** SetGC_ctrl ************************************/

/*
 * This function sets the module's gC_ctrl parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_ctrl_BasilarM_GammaChirp_Dyn(WChar * theGC_ctrl)
{
	static const WChar	*funcName = wxT("SetGC_ctrl_BasilarM_GammaChirp_Dyn");
	int		specifier;

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theGC_ctrl,
		GC_CtrlList_BasilarM_GammaChirp_Dyn(0))) == BM_GC_DYN_GC_CTRL_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theGC_ctrl);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_ctrl = specifier;
	return(TRUE);

}

/****************************** SetGC_n ***************************************/

/*
 * This function sets the module's gC_n parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_n_BasilarM_GammaChirp_Dyn(int theGC_n)
{
	static const WChar	*funcName = wxT("SetGC_n_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_n = theGC_n;
	return(TRUE);

}

/****************************** SetGC_b1 **************************************/

/*
 * This function sets the module's gC_b1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_b1_BasilarM_GammaChirp_Dyn(Float theGC_b1)
{
	static const WChar	*funcName = wxT("SetGC_b1_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_b1 = theGC_b1;
	return(TRUE);

}

/****************************** SetGC_c1 **************************************/

/*
 * This function sets the module's gC_c1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_c1_BasilarM_GammaChirp_Dyn(Float theGC_c1)
{
	static const WChar	*funcName = wxT("SetGC_c1_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_c1 = theGC_c1;
	return(TRUE);

}

/****************************** SetGC_arrayN **********************************/

/*
 * This function sets the module's gC_arrayN parameter.
 * It returns TRUE if the operation is successful.
 * The 'gC_arrayN' variable is set by the
 * 'AllocGC_arrayN_BasilarM_GammaChirp_Dyn' routine.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_arrayN_BasilarM_GammaChirp_Dyn(int theGC_arrayN)
{
	static const WChar	*funcName = wxT(
	  "SetGC_arrayN_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theGC_arrayN < 1) {
		NotifyError(wxT("%s: Value must be greater then zero (%d)."),
		  funcName, theGC_arrayN);
		return(FALSE);
	}
	if (!AllocGC_arrayN_BasilarM_GammaChirp_Dyn(theGC_arrayN)) {
		NotifyError(wxT(
		  "%s: Cannot allocate memory for the 'gC_arrayN' arrays."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetGC_frat ************************************/

/*
 * This function sets the module's gC_frat array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_frat_BasilarM_GammaChirp_Dyn(Float *theGC_frat)
{
	static const WChar	*funcName = wxT("SetGC_frat_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_frat = theGC_frat;
	return(TRUE);

}

/****************************** SetIndividualGC_frat **************************/

/*
 * This function sets the module's gC_frat array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualGC_frat_BasilarM_GammaChirp_Dyn(int theIndex, Float theGC_frat)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualGC_frat_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMDGammaCPtr->gC_frat == NULL) {
		NotifyError(wxT("%s: GC_frat not set."), funcName);
		return(FALSE);
	}
	if (theIndex > bMDGammaCPtr->gC_arrayN - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, bMDGammaCPtr->gC_arrayN - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_frat[theIndex] = theGC_frat;
	return(TRUE);

}

/****************************** SetGC_b2 **************************************/

/*
 * This function sets the module's gC_b2 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_b2_BasilarM_GammaChirp_Dyn(Float *theGC_b2)
{
	static const WChar	*funcName = wxT("SetGC_b2_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_b2 = theGC_b2;
	return(TRUE);

}

/****************************** SetIndividualGC_b2 ****************************/

/*
 * This function sets the module's gC_b2 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualGC_b2_BasilarM_GammaChirp_Dyn(int theIndex, Float theGC_b2)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualGC_b2_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMDGammaCPtr->gC_b2 == NULL) {
		NotifyError(wxT("%s: GC_b2 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > bMDGammaCPtr->gC_arrayN - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, bMDGammaCPtr->gC_arrayN - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_b2[theIndex] = theGC_b2;
	return(TRUE);

}

/****************************** SetGC_c2 **************************************/

/*
 * This function sets the module's gC_c2 array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_c2_BasilarM_GammaChirp_Dyn(Float *theGC_c2)
{
	static const WChar	*funcName = wxT("SetGC_c2_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_c2 = theGC_c2;
	return(TRUE);

}

/****************************** SetIndividualGC_c2 ****************************/

/*
 * This function sets the module's gC_c2 array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualGC_c2_BasilarM_GammaChirp_Dyn(int theIndex, Float theGC_c2)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualGC_c2_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMDGammaCPtr->gC_c2 == NULL) {
		NotifyError(wxT("%s: GC_c2 not set."), funcName);
		return(FALSE);
	}
	if (theIndex > bMDGammaCPtr->gC_arrayN - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, bMDGammaCPtr->gC_arrayN - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_c2[theIndex] = theGC_c2;
	return(TRUE);

}

/****************************** SetGC_phase ***********************************/

/*
 * This function sets the module's gC_phase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_phase_BasilarM_GammaChirp_Dyn(Float theGC_phase)
{
	static const WChar	*funcName = wxT("SetGC_phase_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_phase = theGC_phase;
	return(TRUE);

}

/****************************** SetGC_gainRefdB ******************************/

/*
 * This function sets the module's gC_gainRefdB parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_gainRefdB_BM_GC_DYN(Float theGC_gainRefdB)
{
	static const WChar	*funcName = wxT(
	  "SetGC_gainRefdB_BM_GC_DYN");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_gainRefdB = theGC_gainRefdB;
	return(TRUE);

}

/****************************** SetGC_gainCmpnstdB ****************************/

/*
 * This function sets the module's gC_gainCmpnstdB parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGC_gainCmpnstdB_BasilarM_GammaChirp_Dyn(Float theGC_gainCmpnstdB)
{
	static const WChar	*funcName = wxT(
	  "SetGC_gainCmpnstdB_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->gC_gainCmpnstdB = theGC_gainCmpnstdB;
	return(TRUE);

}

/****************************** SetLvEst_LctERB *******************************/

/*
 * This function sets the module's lvEst_LctERB parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_LctERB_BasilarM_GammaChirp_Dyn(Float theLvEst_LctERB)
{
	static const WChar	*funcName = wxT(
	  "SetLvEst_LctERB_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_LctERB = theLvEst_LctERB;
	return(TRUE);

}

/****************************** SetLvEst_DecayHL ******************************/

/*
 * This function sets the module's lvEst_DecayHL parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_DecayHL_BasilarM_GammaChirp_Dyn(Float theLvEst_DecayHL)
{
	static const WChar	*funcName = wxT(
	  "SetLvEst_DecayHL_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_DecayHL = theLvEst_DecayHL;
	return(TRUE);

}

/****************************** SetLvEst_b2 ***********************************/

/*
 * This function sets the module's lvEst_b2 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_b2_BasilarM_GammaChirp_Dyn(Float theLvEst_b2)
{
	static const WChar	*funcName = wxT("SetLvEst_b2_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_b2 = theLvEst_b2;
	return(TRUE);

}

/****************************** SetLvEst_c2 ***********************************/

/*
 * This function sets the module's lvEst_c2 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_c2_BasilarM_GammaChirp_Dyn(Float theLvEst_c2)
{
	static const WChar	*funcName = wxT("SetLvEst_c2_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_c2 = theLvEst_c2;
	return(TRUE);

}

/****************************** SetLvEst_frat *********************************/

/*
 * This function sets the module's lvEst_frat parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_frat_BasilarM_GammaChirp_Dyn(Float theLvEst_frat)
{
	static const WChar	*funcName = wxT(
	  "SetLvEst_frat_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_frat = theLvEst_frat;
	return(TRUE);

}

/****************************** SetLvEst_RMStoSPLdB ***************************/

/*
 * This function sets the module's lvEst_RMStoSPLdB parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_RMStoSPLdB_BasilarM_GammaChirp_Dyn(Float theLvEst_RMStoSPLdB)
{
	static const WChar	*funcName = wxT(
	  "SetLvEst_RMStoSPLdB_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_RMStoSPLdB = theLvEst_RMStoSPLdB;
	return(TRUE);

}

/****************************** SetLvEst_Weight *******************************/

/*
 * This function sets the module's lvEst_Weight parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_Weight_BasilarM_GammaChirp_Dyn(Float theLvEst_Weight)
{
	static const WChar	*funcName = wxT(
	  "SetLvEst_Weight_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_Weight = theLvEst_Weight;
	return(TRUE);

}

/****************************** SetLvEst_RefdB ********************************/

/*
 * This function sets the module's lvEst_RefdB parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_RefdB_BasilarM_GammaChirp_Dyn(Float theLvEst_RefdB)
{
	static const WChar	*funcName = wxT(
	  "SetLvEst_RefdB_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_RefdB = theLvEst_RefdB;
	return(TRUE);

}

/****************************** SetLvEst_arrayN *******************************/

/*
 * This function sets the module's lvEst_arrayN parameter.
 * It returns TRUE if the operation is successful.
 * The 'lvEst_arrayN' variable is set by the
 * 'AllocLvEst_arrayN_BasilarM_GammaChirp_Dyn' routine.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_arrayN_BasilarM_GammaChirp_Dyn(int theLvEst_arrayN)
{
	static const WChar	*funcName = wxT(
	  "SetLvEst_arrayN_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theLvEst_arrayN < 1) {
		NotifyError(wxT("%s: Value must be greater then zero (%d)."),
		  funcName, theLvEst_arrayN);
		return(FALSE);
	}
	if (!AllocLvEst_arrayN_BasilarM_GammaChirp_Dyn(theLvEst_arrayN)) {
		NotifyError(wxT(
		  "%s: Cannot allocate memory for the 'lvEst_arrayN' arrays."),
		  funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetLvEst_Pwr **********************************/

/*
 * This function sets the module's lvEst_Pwr array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLvEst_Pwr_BasilarM_GammaChirp_Dyn(Float *theLvEst_Pwr)
{
	static const WChar	*funcName = wxT(
	  "SetLvEst_Pwr_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_Pwr = theLvEst_Pwr;
	return(TRUE);

}

/****************************** SetIndividualLvEst_Pwr ************************/

/*
 * This function sets the module's lvEst_Pwr array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualLvEst_Pwr_BasilarM_GammaChirp_Dyn(int theIndex, Float theLvEst_Pwr)
{
	static const WChar	*funcName = wxT(
	  "SetIndividualLvEst_Pwr_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMDGammaCPtr->lvEst_Pwr == NULL) {
		NotifyError(wxT("%s: LvEst_Pwr not set."), funcName);
		return(FALSE);
	}
	if (theIndex > bMDGammaCPtr->lvEst_arrayN - 1) {
		NotifyError(wxT("%s: Index value must be in the range 0 - %d (%d)."),
		  funcName, bMDGammaCPtr->lvEst_arrayN - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->lvEst_Pwr[theIndex] = theLvEst_Pwr;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_GammaChirp_Dyn(CFListPtr theCFList)
{
	static const WChar	*funcName = wxT("SetCFList_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not correctly set."),
		  funcName);
		return(FALSE);
	}
	if (bMDGammaCPtr->theCFs != NULL)
		Free_CFList(&bMDGammaCPtr->theCFs);
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMDGammaCPtr->theCFs = theCFList;
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
SetBandWidths_BasilarM_GammaChirp_Dyn(WChar *theBandwidthMode, Float *theBandwidths)
{
	static const WChar	*funcName = wxT(
	  "SetBandWidths_BasilarM_GammaChirp_Dyn");

	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!SetBandwidths_CFList(bMDGammaCPtr->theCFs, theBandwidthMode,
	  theBandwidths)) {
		NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
		return(FALSE);
	}
	bMDGammaCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

CFListPtr
GetCFListPtr_BasilarM_GammaChirp_Dyn(void)
{
	static const WChar	*funcName = wxT("GetCFListPtr_BasilarM_GammaChirp_Dyn");
	if (bMDGammaCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMDGammaCPtr->theCFs == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly set.  ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(bMDGammaCPtr->theCFs);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_GammaChirp_Dyn(void)
{
	int		i;

	DPrint(wxT("?? BasilarM Module Parameters:-\n"));
	DPrint(wxT("\t%10s\t%10s\t%10s\n"), wxT("gC_frat"), wxT("gC_b2"), wxT("gC_c2"));
	for (i = 0; i < bMDGammaCPtr->gC_arrayN; i++)
		DPrint(wxT("\t%10g\t%10g\t%10g\n"), bMDGammaCPtr->gC_frat[i],
		  bMDGammaCPtr->gC_b2[i], bMDGammaCPtr->gC_c2[i]);
	DPrint(wxT("\t%10s\n"), wxT("lvEst_Pwr"));
	for (i = 0; i < bMDGammaCPtr->lvEst_arrayN; i++)
		DPrint(wxT("\t%10g\n"), bMDGammaCPtr->lvEst_Pwr[i]);
	DPrint(wxT("\tdiagMode = %s \n"), bMDGammaCPtr->diagModeList[
	  bMDGammaCPtr->diagMode].name);
	DPrint(wxT("\toutputMode = %s \n"), OutputModeList_BasilarM_GammaChirp_Dyn(
	  bMDGammaCPtr->outputMode)->name);
	DPrint(wxT("\tpGCCarrierMode = %s \n"), PGCCarrierList_GCFilters(
	  bMDGammaCPtr->pGCCarrierMode)->name);
	DPrint(wxT("\tpeakSpectrumNormMode = %s \n"), BooleanList_NSpecLists(bMDGammaCPtr->
	  peakSpectrumNormMode)->name);
	DPrint(wxT("\tgC_ctrl = %s \n"), GC_CtrlList_BasilarM_GammaChirp_Dyn(bMDGammaCPtr->
	  gC_ctrl)->name);
	DPrint(wxT("\tgC_n = %d ??\n"), bMDGammaCPtr->gC_n);
	DPrint(wxT("\tgC_b1 = %g ??\n"), bMDGammaCPtr->gC_b1);
	DPrint(wxT("\tgC_c1 = %g ??\n"), bMDGammaCPtr->gC_c1);
	DPrint(wxT("\tgC_arrayN = %d ??\n"), bMDGammaCPtr->gC_arrayN);
	DPrint(wxT("\tgC_phase = %g ??\n"), bMDGammaCPtr->gC_phase);
	DPrint(wxT("\tgC_gainRefdB = %g ??\n"), bMDGammaCPtr->gC_gainRefdB);
	DPrint(wxT("\tgC_gainCmpnstdB = %g ??\n"), bMDGammaCPtr->gC_gainCmpnstdB);
	DPrint(wxT("\tlvEst_LctERB = %g ??\n"), bMDGammaCPtr->lvEst_LctERB);
	DPrint(wxT("\tlvEst_DecayHL = %g ??\n"), bMDGammaCPtr->lvEst_DecayHL);
	DPrint(wxT("\tlvEst_b2 = %g ??\n"), bMDGammaCPtr->lvEst_b2);
	DPrint(wxT("\tlvEst_c2 = %g ??\n"), bMDGammaCPtr->lvEst_c2);
	DPrint(wxT("\tlvEst_frat = %g ??\n"), bMDGammaCPtr->lvEst_frat);
	DPrint(wxT("\tlvEst_RMStoSPLdB = %g ??\n"), bMDGammaCPtr->
	  lvEst_RMStoSPLdB);
	DPrint(wxT("\tlvEst_Weight = %g ??\n"), bMDGammaCPtr->lvEst_Weight);
	DPrint(wxT("\tlvEst_RefdB = %g ??\n"), bMDGammaCPtr->lvEst_RefdB);
	DPrint(wxT("\tlvEst_arrayN = %d ??\n"), bMDGammaCPtr->lvEst_arrayN);
	PrintPars_CFList(bMDGammaCPtr->theCFs);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_GammaChirp_Dyn(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_BasilarM_GammaChirp_Dyn");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bMDGammaCPtr = (BMDGammaCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_BasilarM_GammaChirp_Dyn(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_GammaChirp_Dyn");

	if (!SetParsPointer_BasilarM_GammaChirp_Dyn(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_GammaChirp_Dyn(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMDGammaCPtr;
	/*theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;*/
	theModule->Free = Free_BasilarM_GammaChirp_Dyn;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_GammaChirp_Dyn;
	theModule->PrintPars = PrintPars_BasilarM_GammaChirp_Dyn;
	theModule->RunProcess = RunModel_BasilarM_GammaChirp_Dyn;
	theModule->SetParsPointer = SetParsPointer_BasilarM_GammaChirp_Dyn;
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
CheckData_BasilarM_GammaChirp_Dyn(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_BasilarM_GammaChirp_Dyn");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((bMDGammaCPtr->outputMode != BM_GC_DYN_OUTPUTMODE_TOTAL) &&
	  (bMDGammaCPtr->diagMode != GENERAL_DIAGNOSTIC_OFF_MODE)) {
		NotifyError(wxT("%s: Diagnostic mode can only be used when the 'output ")
		  wxT("mode' is set to  'TOTAL'."));
		return(FALSE);
	}
	return(TRUE);

}

/****************************** CmprsGCFreqResp *******************************/

/*
 * This routine calculates the signal path gain normalisation at reference level.
 * It was revised from the aim2007b CmprsGCFrsp.m matlab script.
 */

void
CmprsGCFreqResp_BasilarM_GammaChirp_Dyn(EarObjectPtr data)
{
	int		i, chan;
	Float	*p1, *p2, fs, fratRef, maxResp, valFp2, normFactFp2;
	Float	cGCFrsp[GCFILTERS_NUM_FRQ_RSL], asymFuncFrsp[GCFILTERS_NUM_FRQ_RSL];
	BMGCDGenChanInfo	*cInfo;
	BMDGammaCPtr	p = bMDGammaCPtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	fs = 1.0 / outSignal->dt;
	for (chan = outSignal->offset, cInfo = p->genChanInfo; chan <
	  outSignal->numChannels; chan++, cInfo++) {
		GammaChirpAmpFreqResp_GCFilters(cGCFrsp, p->theCFs->frequency[chan],
		  p->theCFs->bandwidth[chan], fs, p->gC_n, p->gC_b1,  p->gC_c1,
		  p->gC_phase);
		fratRef = cInfo->fratVal[0] + cInfo->fratVal[1] * p->gC_gainRefdB;
		ASympCmpFreqResp_GCFilters(asymFuncFrsp, fratRef * cInfo->fp1, fs,
		  cInfo->b2Val,	cInfo->c2Val, &p->theCFs->bandwidthMode);
		for (i = 0, maxResp = 0.0, p1 = cGCFrsp, p2 = asymFuncFrsp; i <
		  GCFILTERS_NUM_FRQ_RSL; i++)
			if ((valFp2 = *p1++ * *p2++) >  maxResp)
				maxResp = valFp2;
		normFactFp2 = 1.0 / maxResp;
		cInfo->gainFactor = pow(10.0, p->gC_gainCmpnstdB / 20.0) * normFactFp2;
	}
}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_BasilarM_GammaChirp_Dyn(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_BasilarM_GammaChirp_Dyn");
	int		i, j, cFIndex, nchShift, cfIndexLvlEst;
	Float	sR, fr2LvlEst, eRBrate1kHz, *pp;
	BMGCDGenChanInfo	*cInfo;
	SignalDataPtr	inSignal, outSignal;
	BMDGammaCPtr	p = bMDGammaCPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || p->theCFs->
	  updateFlag) {
		FreeProcessVariables_BasilarM_GammaChirp_Dyn();
		OpenDiagnostics_NSpecLists(&p->fp, p->diagModeList, p->diagMode);
		p->numChannels = _OutSig_EarObject(data)->numChannels;
		if ((p->genChanInfo = (BMGCDGenChanInfo *) calloc(p->numChannels,
		  sizeof(BMGCDGenChanInfo))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for genChanInfo array (%d)."), funcName,
		 	  p->numChannels);
		 	return(FALSE);
		}
		if ((p->pGCoeffs = (GammaChirpCoeffsPtr *) calloc(p->numChannels,
		  sizeof(GammaChirpCoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for pGCoeffs array (%d)."), funcName,
		 	  p->numChannels);
		 	return(FALSE);
		}
		if ((p->aCFCoeffLvlEst = (AsymCmpCoeffs2Ptr *) calloc(p->numChannels,
		  sizeof(AsymCmpCoeffs2Ptr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for aCFCoeffLvlEst array (%d)."),
		 	  funcName, p->numChannels);
		 	return(FALSE);
		}
		if ((p->aCFCoeffSigPath = (AsymCmpCoeffs2Ptr *) calloc(p->numChannels,
		  sizeof(AsymCmpCoeffs2Ptr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for aCFCoeffSigPath array (%d)."),
		 	  funcName, p->numChannels);
		 	return(FALSE);
		}
		inSignal = _InSig_EarObject(data, 0);
		outSignal = _OutSig_EarObject(data);
		sR = 1.0 / inSignal->dt;
		eRBrate1kHz = CFRateFromF_CFList(p->theCFs, GCFILTERS_REF_FREQ);
		nchShift = (int) floor(p->lvEst_LctERB / CFSpace_CFList(p->theCFs) + 0.5);
		for (i = 0, cInfo = p->genChanInfo; i < outSignal->numChannels; i++, cInfo++) {
			cFIndex = i / inSignal->interleaveLevel;
			if ((p->pGCoeffs[i] = InitPGammaChirpCoeffs_GCFilters(p->theCFs->
			  frequency[cFIndex], p->theCFs->bandwidth[cFIndex], sR, p->gC_n,
			  p->gC_b1, p->gC_c1, p->gC_phase, p->pGCCarrierMode,
			  p->peakSpectrumNormMode, inSignal)) == NULL) {
				NotifyError(wxT("%s: Could not initialise passive gammachirp ")
				  wxT("coefficients for channel %d."), funcName, i);
				return(FALSE);
			}
			cfIndexLvlEst = i + nchShift;
			if (cfIndexLvlEst > outSignal->numChannels - 1)
				cfIndexLvlEst = outSignal->numChannels - 1;
			cInfo->nchLvlEst = cfIndexLvlEst;
			cInfo->lvlEstChan = outSignal->channel[cInfo->nchLvlEst];
			cInfo->fp1 = GCFILTERS_FR2FPEAK(p->gC_n, p->gC_b1, p->gC_c1,
			  p->theCFs->frequency[cFIndex], p->theCFs->bandwidth[cFIndex]);
			fr2LvlEst = p->lvEst_frat * cInfo->fp1;
			if ((p->aCFCoeffLvlEst[i] = InitAsymCmpCoeffs2_GCFilters(p->gC_n, sR,
			  p->lvEst_b2, p->lvEst_c2, &p->theCFs->bandwidthMode)) == NULL) {
				NotifyError(wxT("%s: Could not initialise asymmetric compressive ")
				  wxT("level estimation filter, channel %d."), funcName, i);
				return(FALSE);
			}
			SetAsymCmpCoeffs2_GCFilters(p->aCFCoeffLvlEst[i], fr2LvlEst);
			cInfo->eF = CFRateFromF_CFList(p->theCFs, p->theCFs->frequency[i]) /
			  eRBrate1kHz - 1.0;
			cInfo->b2Val = p->gC_b2[0] + p->gC_b2[1] * cInfo->eF;
			cInfo->c2Val = p->gC_c2[0] + p->gC_c2[1] * cInfo->eF;
			if ((p->aCFCoeffSigPath[i] = InitAsymCmpCoeffs2_GCFilters(p->gC_n, sR,
			  cInfo->b2Val, cInfo->c2Val, &p->theCFs->bandwidthMode)) == NULL) {
				NotifyError(wxT("%s: Could not initialise asymmetric compressive ")
				  wxT("signal path filter, channel %d."), funcName, i);
				return(FALSE);
			}
			for (j = 0, pp = p->gC_frat; j < BM_GC_DYN_FRATVAL_EXPRS; j++, pp +=
			  BM_GC_DYN_FRATVAL_ARRAY_N)
				cInfo->fratVal[j] = *pp + *(pp + 1) * cInfo->eF;
		}
		CmprsGCFreqResp_BasilarM_GammaChirp_Dyn(data);
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data),
		  wxT("Frequency (Hz)"));
		SetInfoChannelLabels_SignalData(_OutSig_EarObject(data), p->theCFs->
		  frequency);
		SetInfoCFArray_SignalData(_OutSig_EarObject(data), p->theCFs->
		  frequency);
		p->expDecayVal = exp(-1.0 / (p->lvEst_DecayHL * sR / 1000.0) * LN_2);
		p->lvlLinRef = pow(10.0, (p->lvEst_RefdB - p->lvEst_RMStoSPLdB) / 20.0);
		p->lvlLinMinLim = pow(10.0, -p->lvEst_RMStoSPLdB / 20.0);
		p->updateProcessVariablesFlag = FALSE;
		p->theCFs->updateFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0, cInfo = p->genChanInfo; i < p->numChannels; i++, cInfo++) {
			for (j = 0; j < BM_GC_DYN_LVLLIN_SIZE; j++) {
				cInfo->lvlLinPrev[j] = 0.0;
				cInfo->lvlLinNow[j] = 0.0;
			}
			ResetAsymCmpCoeffs2State_GCFilters(p->aCFCoeffLvlEst[i]);
			ResetAsymCmpCoeffs2State_GCFilters(p->aCFCoeffSigPath[i]);
		}
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_BasilarM_GammaChirp_Dyn(void)
{
	int		i;
	BMDGammaCPtr	p = bMDGammaCPtr;

	if (p->fp) {
		fclose(p->fp);
		p->fp = NULL;
	}
	if (p->genChanInfo) {
		free(p->genChanInfo);
		p->genChanInfo = NULL;
	}
	if (p->pGCoeffs) {
		for (i = 0; i < p->numChannels; i++)
			FreePGammaChirpCoeffs_GCFilters(&p->pGCoeffs[i]);
		free(p->pGCoeffs);
		p->pGCoeffs = NULL;
	}
	if (p->aCFCoeffLvlEst) {
		for (i = 0; i < p->numChannels; i++)
			FreeAsymCmpCoeffs2_GCFilters(&p->aCFCoeffLvlEst[i]);
		free(p->aCFCoeffLvlEst);
		p->aCFCoeffLvlEst = NULL;
	}
	if (p->aCFCoeffSigPath) {
		for (i = 0; i < p->numChannels; i++)
			FreeAsymCmpCoeffs2_GCFilters(&p->aCFCoeffSigPath[i]);
		free(p->aCFCoeffSigPath);
		p->aCFCoeffSigPath = NULL;
	}
	p->updateProcessVariablesFlag = TRUE;

}

/****************************** RunModel **************************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if
 * required.
 * Stimulus generation only sets the output signal, the input signal is not used.
 * With repeated calls the Signal memory is only allocated once, then re-used.
 * The use of 'nchLvlEst' is cross-channel so even in threaded mode, the base genChanInfo
 * must always be used to access the appropriate savedPGOut data.
 * The 'aCFilterOutIndex' value should be the same for all channels.
 */

BOOLN
RunModel_BasilarM_GammaChirp_Dyn(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_BasilarM_GammaChirp_Dyn");
	register ChanData	 *dataPtr, workVar;
	uShort	totalChannels;
	BOOLN	debug;
	int		chan;
	Float	lvlLinTtl, fratVal;
	ChanLen	i;
	ChanData	*dataStart;
	BMGCDGenChanInfo	*cInfo, *cInfoStart;
	AsymCmpCoeffs2Ptr	*aCFCoeffSigPath, *aCFCoeffSigPathStart;
	SignalDataPtr	inSignal, outSignal;
	BMDGammaCPtr	p = bMDGammaCPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_BasilarM_GammaChirp_Dyn(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		inSignal = _InSig_EarObject(data, 0);
		SetProcessName_EarObject(data, wxT("Dynamic Compressive Gammachirp Filtering"));
		if (!CheckRamp_SignalData(inSignal)) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
		totalChannels = p->theCFs->numChannels * inSignal->numChannels;
		if (!InitOutSignal_EarObject(data, totalChannels, inSignal->length,
		  inSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_BasilarM_GammaChirp_Dyn(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	if (p->outputMode == BM_GC_DYN_OUTPUTMODE_DYNA_COMP)
		InitOutDataFromInSignal_EarObject(data);
	else
		PassiveGCFilter_GCFilters(data, p->pGCoeffs);
	if (p->outputMode != BM_GC_DYN_OUTPUTMODE_PASSIVE) {
#		if DEBUG
			for (chan = _OutSig_EarObject(data)->offset; chan <
			  _OutSig_EarObject(data)->numChannels; chan++)
				SetAsymCmpCoeffs2_GCFilters(p->aCFCoeffSigPath[chan], p->genChanInfo[
				chan].fp1);
#		endif
		outSignal = _OutSig_EarObject(data);
		if ((debug = (!data->threadRunFlag && (p->diagMode !=
		  GENERAL_DIAGNOSTIC_OFF_MODE))) == TRUE) {
			DSAM_fprintf(p->fp, wxT("Time(s)"));
			for (chan = outSignal->offset; chan < outSignal->numChannels; chan++)
				DSAM_fprintf(p->fp, wxT("\tHP-AF[%d]\tcGC[%d]"), chan, chan);
			DSAM_fprintf(p->fp, wxT("\n"));
		}
		cInfoStart = p->genChanInfo + outSignal->offset;
		aCFCoeffSigPathStart = p->aCFCoeffSigPath + outSignal->offset;
		for (i = 0, dataStart = outSignal->channel[outSignal->offset]; i <
		  data->outSignal->length; i++, dataStart++) {
			if (p->gC_ctrl == BM_GC_DYN_GC_CTRL_FIXED)
				for (chan = outSignal->offset, cInfo = cInfoStart;chan <
				  outSignal->numChannels; chan++, cInfo++)
					cInfo->lvldB = p->gC_gainRefdB;
			else {
				/* Level estimation path */
				for (chan = outSignal->offset, cInfo = cInfoStart; chan <
				  outSignal->numChannels; chan++, cInfo++) {
					ACFilterBank_GCFilters(p->aCFCoeffLvlEst + chan, data, cInfo->nchLvlEst,
					  cInfo->nchLvlEst + 1, i);
					if ((cInfo->lvlLinNow[0] = *(cInfo->lvlEstChan + i)) < 0.0)
						cInfo->lvlLinNow[0] = 0.0;
					if (cInfo->lvlLinNow[0] < (workVar = cInfo->lvlLinPrev[0] * p->expDecayVal))
						cInfo->lvlLinNow[0] = workVar;
					if ((cInfo->lvlLinNow[1] = *p->aCFCoeffLvlEst[cInfo->nchLvlEst]->y) < 0.0)
						cInfo->lvlLinNow[1] = 0;
					if (cInfo->lvlLinNow[1] < (workVar = cInfo->lvlLinPrev[1] * p->expDecayVal))
						cInfo->lvlLinNow[1] = workVar;
					cInfo->lvlLinPrev[0] = cInfo->lvlLinNow[0];
					cInfo->lvlLinPrev[1] = cInfo->lvlLinNow[1];
					lvlLinTtl = p->lvlLinRef * (p->lvEst_Weight * pow(cInfo->lvlLinNow[0] /
					  p->lvlLinRef, p->lvEst_Pwr[0]) + (1.0 - p->lvEst_Weight) *
		          	  pow(cInfo->lvlLinNow[1] / p->lvlLinRef, p->lvEst_Pwr[1]));
		          	if (lvlLinTtl < p->lvlLinMinLim)
		          		lvlLinTtl = p->lvlLinMinLim;
		          	cInfo->lvldB = 20.0 * log10(lvlLinTtl) + p->lvEst_RMStoSPLdB;
				}
			}
			for (chan = outSignal->offset, cInfo = cInfoStart, aCFCoeffSigPath =
			  aCFCoeffSigPathStart; chan < outSignal->numChannels; chan++, cInfo++,
			  aCFCoeffSigPath++) {
				fratVal = cInfo->fratVal[0] + cInfo->fratVal[1] * cInfo->lvldB;
				SetAsymCmpCoeffs2_GCFilters(*aCFCoeffSigPath, fratVal * cInfo->fp1);
			}
			ACFilterBank_GCFilters(p->aCFCoeffSigPath, data, outSignal->offset,
			  outSignal->numChannels, i);
			for (chan = outSignal->offset, cInfo = cInfoStart, dataPtr = dataStart,
			  aCFCoeffSigPath = aCFCoeffSigPathStart; chan < outSignal->numChannels;
			  chan++, cInfo++, dataPtr += outSignal->length, aCFCoeffSigPath++)
				*dataPtr = *((*aCFCoeffSigPath)->y) * cInfo->gainFactor;
			if (debug) {
				DSAM_fprintf(p->fp, wxT("%g"), i * outSignal->dt);
				for (chan = outSignal->offset, cInfo = cInfoStart, dataPtr = dataStart;
				  chan < outSignal->numChannels; chan++, cInfo++, dataPtr += outSignal->length)
					DSAM_fprintf(p->fp, wxT("\t%g\t%g"), cInfo->lvldB, *dataPtr);
				DSAM_fprintf(p->fp, wxT("\n"));
			}

		}
		if (debug && p->fp)
			CloseDiagnostics_NSpecLists(&p->fp);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

#endif /* HAVE_FFTW3 */

