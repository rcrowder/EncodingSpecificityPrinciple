/*******************
 *
 * File:		MoBMCooke.c
 * Purpose: 	An implementation of Martin Cooke's model of the response of the
 *				Basilar Membrane.
 * Comments:	This version is an almost completely faithful reproduction of
 *				supplied code.
 *				26-01-97 LPO: named changed from MoBM0Cooke -> MoBMCooke.
 *				11-11-98 LPO: Implemented 'outputMode' as 'NameSpecifier'.
 * Author:		M.Cooke/L.P.O'Mard
 * Created:		1 May 1995
 * Updated:		11 Nov 1998
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtFilters.h"
#include "UtString.h"
#include "MoBMCooke.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

BM0CookePtr	bM0CookePtr = NULL;

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/****************************** InitOutputModeList ****************************/

/*
 * This function initialises the 'outputMode' list array
 */

BOOLN
InitOutputModeList_BasilarM_Cooke(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("BM"),	BM_COOKE_OUTPUTMODE_BM_DETAIL },
			{ wxT("POW"),	BM_COOKE_OUTPUTMODE_POWER_SPEC },
			{ wxT("AMP"),	BM_COOKE_OUTPUTMODE_AMP_ENVELOPE },
			{ NULL,			BM_COOKE_OUTPUTMODE_BM_DETAIL },
		};
	bM0CookePtr->outputModeList = modeList;
	return(TRUE);

}

/********************************* Init ***************************************/

/*
 * This function initialises the module by setting module's parameter pointer
 * structure.
 * The GLOBAL option is for hard programming - it sets the module's pointer to
 * the global parameter structure and initialises the parameters.
 * The LOCAL option is for generic programming - it initialises the parameter
 * structure currently pointed to by the module's parameter pointer.
 */

BOOLN
Init_BasilarM_Cooke(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_BasilarM_Cooke");

	if (parSpec == GLOBAL) {
		if (bM0CookePtr != NULL)
			Free_BasilarM_Cooke();
		if ((bM0CookePtr = (BM0CookePtr) malloc(sizeof(BM0Cooke))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bM0CookePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bM0CookePtr->parSpec = parSpec;
	bM0CookePtr->updateProcessVariablesFlag = TRUE;
	bM0CookePtr->outputMode = BM_COOKE_OUTPUTMODE_BM_DETAIL;
	bM0CookePtr->broadeningCoeff = 1.019;
	if ((bM0CookePtr->theCFs = GenerateDefault_CFList(
	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS,
	  CFLIST_DEFAULT_LOW_FREQ, CFLIST_DEFAULT_HIGH_FREQ,
	  CFLIST_DEFAULT_BW_MODE_NAME, CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {
		NotifyError(wxT("%s: could not set default CFList."), funcName);
		return(FALSE);
	}

	InitOutputModeList_BasilarM_Cooke();
	if (!SetUniParList_BasilarM_Cooke()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_Cooke();
		return(FALSE);
	}
	bM0CookePtr->numChannels = 0;
	bM0CookePtr->coefficients = NULL;
	bM0CookePtr->sine = NULL;
	bM0CookePtr->cosine = NULL;
	bM0CookePtr->cosine = NULL;
	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface requires that a non-void value be returned.
 */

BOOLN
Free_BasilarM_Cooke(void)
{
	if (bM0CookePtr == NULL)
		return(TRUE);
	FreeProcessVariables_BasilarM_Cooke();
	Free_CFList(&bM0CookePtr->theCFs);
	if (bM0CookePtr->parList)
		FreeList_UniParMgr(&bM0CookePtr->parList);
	if (bM0CookePtr->parSpec == GLOBAL) {
		free(bM0CookePtr);
		bM0CookePtr = NULL;
	}
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_Cooke(void)
{
	static const WChar *funcName = wxT("SetUniParList_BasilarM_Cooke");
	UniParPtr	pars;

	if ((bM0CookePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BM_COOKE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bM0CookePtr->parList->pars;
	SetPar_UniParMgr(&pars[BM_COOKE_BROADENINGCOEFF], wxT("BWI_FACTOR"),
	  wxT("ERB Bandwidth correction factor."),
	  UNIPAR_REAL,
	  &bM0CookePtr->broadeningCoeff, NULL,
	  (void * (*)) SetBroadeningCoeff_BasilarM_Cooke);
	SetPar_UniParMgr(&pars[BM_COOKE_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode ('bm', 'pow', 'amp')."),
	  UNIPAR_NAME_SPEC,
	  &bM0CookePtr->outputMode, bM0CookePtr->outputModeList,
	  (void * (*)) SetOutputMode_BasilarM_Cooke);
	SetPar_UniParMgr(&pars[BM_COOKE_THECFS], wxT("CFLIST"),
	  wxT(""),
	  UNIPAR_CFLIST,
	  &bM0CookePtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_Cooke);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_Cooke(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_BasilarM_Cooke");

	if (bM0CookePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bM0CookePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bM0CookePtr->parList);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_BasilarM_Cooke(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_BasilarM_Cooke");
	int		specifier;

	if (bM0CookePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
		bM0CookePtr->outputModeList)) == BM_COOKE_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bM0CookePtr->outputMode = specifier;
	return(TRUE);

}


/********************************* SetBroadeningCoeff *************************/

/*
 * This function sets the module's broadeningCoeff parameter.
 *It first checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBroadeningCoeff_BasilarM_Cooke(Float theBroadeningCoeff)
{
	static const WChar *funcName = wxT("SetBroadeningCoeff_BasilarM_Cooke");

	if (bM0CookePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	bM0CookePtr->broadeningCoeff = theBroadeningCoeff;
	bM0CookePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetCFList **********************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_Cooke(CFListPtr theCFList)
{
	static const WChar *funcName = wxT("SetCFList_BasilarM_Cooke");

	if (bM0CookePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not initialised ")
		  wxT("correctly set."), funcName);
		return(FALSE);
	}
	if (bM0CookePtr->theCFs != NULL)
		Free_CFList(&bM0CookePtr->theCFs);
	bM0CookePtr->theCFs = theCFList;
	bM0CookePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetBandwidths ******************************/

/*
 * This function sets the band width mode for the gamma tone filters.
 * The band width mode defines the function for calculating the gamma tone
 * filter band width 3 dB down.
 * No checks are made on the correct length for the bandwidth array.
 */

BOOLN
SetBandwidths_BasilarM_Cooke(WChar *theBandwidthMode,
  Float *theBandwidths)
{
	static const WChar *funcName = wxT("SetBandwidths_BasilarM_Cooke");

	if (bM0CookePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(bM0CookePtr->theCFs, theBandwidthMode,
	  theBandwidths)) {
		NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
		return(FALSE);
	}
	bM0CookePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 *
 */

CFListPtr
GetCFListPtr_BasilarM_Cooke(void)
{
	static const WChar *funcName = wxT("GetCFListPtr_BasilarM_Cooke");

	if (bM0CookePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bM0CookePtr->theCFs == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly ")
		  wxT("set.  NULL returned."), funcName);
		return(NULL);
	}
	return(bM0CookePtr->theCFs);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_BasilarM_Cooke(void)
{
	DPrint(wxT("Cooke (0) Basilar Membrane Filter Module Parameters:-\n"));
	PrintPars_CFList(bM0CookePtr->theCFs);
	DPrint(wxT("\tOutput mode = %s.\n"), bM0CookePtr->outputModeList[
	  bM0CookePtr->outputMode].name);
	DPrint(wxT("\tBroadening coefficient = %g.\n"), bM0CookePtr->
	  broadeningCoeff);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_Cooke(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_BasilarM_Cooke");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bM0CookePtr = (BM0CookePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_Cooke(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_Cooke");

	if (!SetParsPointer_BasilarM_Cooke(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_Cooke(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = bM0CookePtr;
	/*theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;*/
	theModule->Free = Free_BasilarM_Cooke;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_Cooke;
	theModule->PrintPars = PrintPars_BasilarM_Cooke;
	theModule->RunProcess = RunModel_BasilarM_Cooke;
	theModule->SetParsPointer = SetParsPointer_BasilarM_Cooke;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the various tables and coeffients for the Cooke 0
 * filter.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_BasilarM_Cooke(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_BasilarM_Cooke");

	int		i;
	unsigned long	j;
	Float	twoPiDt, phi, bandwidth;
	CookeCoeffsPtr	c;
	BM0CookePtr	p = bM0CookePtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || p->theCFs->
	  updateFlag || (data->timeIndex == PROCESS_START_TIME)) {
		twoPiDt = PIx2 * _OutSig_EarObject(data)->dt;
		if (p->updateProcessVariablesFlag || data->updateProcessFlag || p->
		  theCFs->updateFlag) {
			FreeProcessVariables_BasilarM_Cooke();
			if ((p->coefficients = (CookeCoeffsPtr) calloc(p->theCFs->
			  numChannels, sizeof(CookeCoeffs))) == NULL) {
				NotifyError(wxT("%s: Out of memory for coeffients."), funcName);
				return(FALSE);
			}
			p->intSampleRate = (unsigned long) ceil(1.0 / _OutSig_EarObject(
			  data)->dt);
			if ((p->sine = (Float *) calloc(p->intSampleRate, sizeof(
			  Float))) == NULL) {
				NotifyError(wxT("%s: Out of memory for sine table."), funcName);
				return(FALSE);
			}
			if ((p->cosine = (Float *) calloc(p->intSampleRate, sizeof(
			  Float))) == NULL) {
				NotifyError(wxT("%s: Out of memory for cosine table."),
				  funcName);
				return(FALSE);
			}
			for (j = 0; j < p->intSampleRate; j++) {
				phi = (Float) (j * twoPiDt);
				p->sine[j] = sin(phi);
				p->cosine[j] = cos(phi);
			}
			c = p->coefficients;
			for (i = 0; i < p->theCFs->numChannels; i++) {
				bandwidth = p->theCFs->bandwidth[i] *
				  p->broadeningCoeff;
				c[i].z = exp(-twoPiDt * bandwidth);
				c[i].gain = SQR(SQR(2 * PI * bandwidth * _OutSig_EarObject(
				  data)->dt)) / 3.0;
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
		}
		c = p->coefficients;
		for (i = 0; i < p->theCFs->numChannels; i++) {
			c[i].p0 = c[i].p1 = c[i].p2 = c[i].p3 = c[i].p4 = 0.0;
			c[i].q0 = c[i].q1 = c[i].q2 = c[i].q3 = c[i].q4 = 0.0;
			c[i].u0 = 0.0;
			c[i].v0 = 0.0;
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
FreeProcessVariables_BasilarM_Cooke(void)
{
	if (bM0CookePtr->coefficients == NULL)
		return;
	free(bM0CookePtr->coefficients);
	bM0CookePtr->coefficients = NULL;
	free(bM0CookePtr->sine);
	bM0CookePtr->sine = NULL;
	free(bM0CookePtr->cosine);
	bM0CookePtr->cosine = NULL;

}

/**************************** RunModel ****************************************/

/*
 * This routine filters the input signal and puts the result into the output
 * signal.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * The appropriate coefficients are calculated at the beginning, then the
 * memory used is released before the program returns.
 */

BOOLN
RunModel_BasilarM_Cooke(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_BasilarM_Cooke");
	uShort	totalChannels;
	int		chan, cFIndex;
	unsigned long	tablePtr;
	ChanLen	i;
	Float	zz, pow, cosPtr;
	register Float	*inPtr, *outPtr;
	CookeCoeffsPtr	c;
	SignalDataPtr	outSignal;
	BM0CookePtr	p = bM0CookePtr;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}
		if (!CheckInSignal_EarObject(data, funcName))
			return(FALSE);

		/* Initialise Variables and coefficients */

		SetProcessName_EarObject(data, wxT("Cooke gammatone basilar membrane ")
		  wxT("filtering"));
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
		totalChannels = p->theCFs->numChannels * _InSig_EarObject(data, 0)->
		  numChannels;
		if (!InitOutTypeFromInSignal_EarObject(data, totalChannels)) {
			NotifyError(wxT("%s: Could not initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_BasilarM_Cooke(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	InitOutDataFromInSignal_EarObject(data);
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		cFIndex = chan / outSignal->interleaveLevel;
		c = p->coefficients + cFIndex;
		outPtr = inPtr = outSignal->channel[chan];
		for (i = 0, cosPtr = 0.0; i < outSignal->length; i++, inPtr++,
		  outPtr++) {
			tablePtr = (unsigned long) cosPtr;
			zz = c->z;
			c->p0 = *inPtr * p->cosine[tablePtr] + zz * (4.0 * c->p1 - zz *
			  (6.0 * c->p2 - zz * (4.0 * c->p3 - zz * c->p4)));
			c->q0 = -*inPtr * p->sine[tablePtr] + zz * (4.0 * c->q1 - zz *
			  (6.0 * c->q2 - zz * (4.0 * c->q3 - zz * c->q4)));
			c->u0 = zz * (c->p1 + zz * (4.0 * c->p2 + zz * c->p3));
			c->v0 = zz * (c->q1 + zz * (4.0 * c->q2 + zz * c->q3));
			pow = SQR(c->u0) + SQR(c->v0);
			switch (p->outputMode) {
			case BMCOOKE0_BM_DETAIL:
				*outPtr = (c->u0 * p->cosine[tablePtr] - c->v0 * p->sine[
				  tablePtr]) * c->gain;
				break;
			case BMCOOKE0_POWER_SPEC:
				*outPtr = pow;
				break;
			case BMCOOKE0_AMP_ENVELOPE:
				*outPtr = sqrt(pow) * c->gain;
				break;
			default:
				break;
			}
			c->p4 = c->p3; c->p3 = c->p2; c->p2 = c->p1; c->p1 = c->p0;
			c->q4 = c->q3; c->q3 = c->q2; c->q2 = c->q1; c->q1 = c->q0;
			cosPtr += p->theCFs->frequency[cFIndex];
			while (cosPtr >= p->intSampleRate)
				cosPtr -= p->intSampleRate;
		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
