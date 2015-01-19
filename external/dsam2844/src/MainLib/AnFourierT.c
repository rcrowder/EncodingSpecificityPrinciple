/**********************
 *
 * File:		AnFourierT.c
 * Purpose:		This module runs a basic Fourier Analysis.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		18-01-94
 * Updated:
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "GeModuleMgr.h"
#include "UtCmplxM.h"
#include "UtFFT.h"
#include "UtString.h"
#include "AnFourierT.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FourierTPtr	fourierTPtr = NULL;

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_Analysis_FourierT(void)
{
	if (fourierTPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_FourierT();
	if (fourierTPtr->parList)
		FreeList_UniParMgr(&fourierTPtr->parList);
	if (fourierTPtr->parSpec == GLOBAL) {
		free(fourierTPtr);
		fourierTPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOutputModeList ****************************/

/*
 * This function initialises the 'outputMode' list array
 */

BOOLN
InitOutputModeList_Analysis_FourierT(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("MODULUS"),	ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE },
			{ wxT("PHASE"),		ANALYSIS_FOURIERT_PHASE_OUTPUTMODE },
			{ wxT("COMPLEX"),	ANALYSIS_FOURIERT_COMPLEX_OUTPUTMODE },
			{ wxT("DB_SPL"),	ANALYSIS_FOURIERT_DB_SPL_OUTPUTMODE },
			{ NULL,				ANALYSIS_FOURIERT_OUTPUTMODE_NULL },
		};
	fourierTPtr->outputModeList = modeList;
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
Init_Analysis_FourierT(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_FourierT");

	if (parSpec == GLOBAL) {
		if (fourierTPtr != NULL)
			free(fourierTPtr);
		if ((fourierTPtr = (FourierTPtr) malloc(sizeof(FourierT))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fourierTPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	fourierTPtr->parSpec = parSpec;
	fourierTPtr->updateProcessVariablesFlag = TRUE;
	fourierTPtr->outputMode = ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE;

	InitOutputModeList_Analysis_FourierT();
	if (!SetUniParList_Analysis_FourierT()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_FourierT();
		return(FALSE);
	}
	fourierTPtr->fTLength = 0;
	fourierTPtr->numThreads = 0;
#	if HAVE_FFTW3
		fourierTPtr->plan = NULL;
#	endif /* HAVE_FFTW3 */
	fourierTPtr->fT = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_FourierT(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_FourierT");
	UniParPtr	pars;

	if ((fourierTPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_FOURIERT_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = fourierTPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_FOURIERT_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode: 'modulus', 'phase', 'complex' or 'dB_SPL' ")
	  wxT("(approximation)."),
	  UNIPAR_NAME_SPEC,
	  &fourierTPtr->outputMode, fourierTPtr->outputModeList,
	  (void * (*)) SetOutputMode_Analysis_FourierT);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_FourierT(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_FourierT");

	if (fourierTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (fourierTPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(fourierTPtr->parList);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_Analysis_FourierT(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_Analysis_FourierT");
	int		specifier;

	if (fourierTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
		fourierTPtr->outputModeList)) == ANALYSIS_FOURIERT_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fourierTPtr->outputMode = specifier;
	return(TRUE);

}

//****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_FourierT(void)
{
	DPrint(wxT("Fourier Transform Analysis Module Parameters:-\n"));
	DPrint(wxT("\tOutput mode = %s \n"),
	  fourierTPtr->outputModeList[fourierTPtr->outputMode].name);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_FourierT(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_FourierT");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	fourierTPtr = (FourierTPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_FourierT(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_FourierT");

	if (!SetParsPointer_Analysis_FourierT(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_FourierT(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = fourierTPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_FourierT;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_FourierT;
	theModule->PrintPars = PrintPars_Analysis_FourierT;
	theModule->ResetProcess = ResetProcess_Analysis_FourierT;
	theModule->RunProcess = Calc_Analysis_FourierT;
	theModule->SetParsPointer = SetParsPointer_Analysis_FourierT;
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
CheckData_Analysis_FourierT(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_FourierT");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Analysis_FourierT(EarObjectPtr data)
{
	ResetOutSignal_EarObject(data);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the exponential table.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 * This routine assumes that calloc sets all of the InterSIHSpikeSpecPtr
 * pointers to NULL.
 * If using in-place FFTs under FFTW, the length must be the fftLength * 2 + 2.
 */

BOOLN
InitProcessVariables_Analysis_FourierT(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_Analysis_FourierT");
	int		i;
	FourierTPtr	p = fourierTPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_FourierT();
		p->fTLength = Length_FFT(_InSig_EarObject(data, 0)->length);
		p->numThreads = data->numThreads;
		if ((p->fT = (ComplxPtr *) calloc(p->numThreads, sizeof(
		  ComplxPtr))) == NULL) {
			NotifyError(wxT("%s: Couldn't allocate memory for complex data ")
			  wxT("pointer array."), funcName);
			return(FALSE);
		}
#		if HAVE_FFTW3
		p->arrayLen = (p->fTLength << 1) + 2;
#		else
		p->arrayLen = p->fTLength;
#		endif
		for (i = 0; i < p->numThreads; i++) {
			if ((p->fT[i] = (Complx *) CMPLX_MALLOC(p->arrayLen * sizeof(
			  Complx))) == NULL) {
				NotifyError(wxT("%s: Couldn't allocate memory for complex ")
				  wxT("data array (%d)."), funcName, i);
				return(FALSE);
			}
		}
#		if HAVE_FFTW3
			p->plan = DSAM_FFTW_NAME(plan_dft_r2c_1d)(p->fTLength, (Float *) p->fT[0],
			  p->fT[0], FFTW_ESTIMATE);
#		endif
		p->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Analysis_FourierT(void)
{
	int		i;

#	if HAVE_FFTW3
		if (fourierTPtr->plan) {
			DSAM_FFTW_NAME(destroy_plan)(fourierTPtr->plan);
			fourierTPtr->plan = NULL;
		}
#	endif
	if (fourierTPtr->fT) {
		for (i = 0; i < fourierTPtr->numThreads; i++) {
			CMPLX_FREE(fourierTPtr->fT[i]);
		}
		free(fourierTPtr->fT);
		fourierTPtr->fT = NULL;
	}

}

/****************************** Calc ******************************************/

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
 * The outSignal->dt field is set to the dF value, however to calculate the
 * length of the channels, this needs to be the same as the input signal dt.
 * the call to 'SetSamplingInterval_SignalData' before 'InitOutSignal_EarObject'
 * ensures that the signal is not needlessly recreated.
 */

BOOLN
Calc_Analysis_FourierT(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_FourierT");
	register	ChanData	 *inPtr, *outPtr, modulus;
	int		chan, outChan;
	Float	dF;
	ChanLen	i;
	Complx	*fT;
	SignalDataPtr	outSignal;
	FourierTPtr	p = fourierTPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_FourierT(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Fourier Transform: modulus"));
		p->numOutChans = (p->outputMode ==
		  ANALYSIS_FOURIERT_COMPLEX_OUTPUTMODE)? 2: 1;
		if (_OutSig_EarObject(data))
			SetSamplingInterval_SignalData(_OutSig_EarObject(data),
			  _InSig_EarObject(data, 0)->dt);
		if (!InitOutSignal_EarObject(data, (uShort) (_InSig_EarObject(data, 0)->
		  numChannels * p->numOutChans), _InSig_EarObject(data, 0)->length,
		  _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Couldn't initialse output signal."), funcName);
			return(FALSE);
		}
		ResetProcess_Analysis_FourierT(data);
		if (!InitProcessVariables_Analysis_FourierT(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		dF = 1.0 / (_InSig_EarObject(data, 0)->dt * p->fTLength);
		SetSamplingInterval_SignalData(_OutSig_EarObject(data), dF);
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoSampleTitle_SignalData(_OutSig_EarObject(data), wxT("Frequency ")
		  wxT("(Hz)"));
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data), wxT(
		  "Arbitrary Amplitude"));
		SetStaticTimeFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), 0.0);
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), (uShort) (
		  _InSig_EarObject(data, 0)->interleaveLevel * p->numOutChans));
		p->dBSPLFactor = SQRT_2 / _InSig_EarObject(data, 0)->length;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset * p->numOutChans; chan < outSignal->numChannels; chan +=
	  p->numOutChans) {
		outChan = chan * p->numOutChans;
		fT = p->fT[data->threadIndex];
		CMPLX_PTR_RE(fT) = CMPLX_PTR_IM(fT) = 0.0;
		inPtr = _InSig_EarObject(data, 0)->channel[outChan] + 1;
#		if HAVE_FFTW3
		{	/*     Braces required by MSVC Studio2005 */
			Float	*fTIn = (Float *) fT++;
			for (i = 1; i < outSignal->length; i++)
				*fTIn++ = *inPtr++;
			for (; i < p->arrayLen; i++)
				*fTIn++ = 0.0;
		}
#		else
			for (i = 1, fT++; i < outSignal->length; i++, fT++) {
				CMPLX_PTR_IM(fT) = 0.0;
				CMPLX_PTR_RE(fT) = *inPtr++;
			}
			for (; i < p->fTLength; i++, fT++) {
				CMPLX_PTR_IM(fT) = 0.0;
				CMPLX_PTR_RE(fT) = 0.0;
			}
#		endif
		outPtr = outSignal->channel[outChan];

		fT = p->fT[data->threadIndex];
#		if HAVE_FFTW3
			DSAM_FFTW_NAME(execute_dft_r2c)(p->plan, (Float *) fT, fT);
#		else
			CalcComplex_FFT(fT, p->fTLength, FORWARD_FT);
#		endif

		switch (p->outputMode) {
		case ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE:
			for (i = 0; i < outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) CMPLX_MODULUS(*fT);
			break;
		case ANALYSIS_FOURIERT_PHASE_OUTPUTMODE:
			for (i = 0; i < outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) atan2(CMPLX_PTR_IM(fT), CMPLX_PTR_RE(fT));
			break;
		case ANALYSIS_FOURIERT_COMPLEX_OUTPUTMODE:
			for (i = 0; i < outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) CMPLX_PTR_RE(fT);
			fT = p->fT[data->threadIndex];
			outPtr = outSignal->channel[outChan + 1];
			for (i = 0; i < outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) CMPLX_PTR_IM(fT);
			break;
		case ANALYSIS_FOURIERT_DB_SPL_OUTPUTMODE:
			for (i = 0; i < outSignal->length; i++, fT++) {
				modulus = CMPLX_MODULUS(*fT);
				*outPtr++ = (ChanData) DB_SPL(((modulus < DBL_EPSILON)? 0.0002: modulus *
				  p->dBSPLFactor));
			}
			break;
		default:
			;
		}

	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

