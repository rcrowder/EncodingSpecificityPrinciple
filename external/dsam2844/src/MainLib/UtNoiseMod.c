/**********************
 *
 * File:		UtNoiseMod.c
 * Purpose:		Amplitude modulates an input signal with a noise masker.
 * Comments:	Written using ModuleProducer version 1.6.1 (Nov 10 2008).
 * Author:		L. P. O'Mard
 * Created:		17 Nov 2008
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
#include "UtFFT.h"
#include "UtBandwidth.h"
#include "UtNoiseMod.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

NAmpModPtr	nAmpModPtr = NULL;

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
Free_Utility_AmpMod_Noise(void)
{
	/* static const WChar	*funcName = wxT("Free_Utility_AmpMod_Noise"); */

	if (nAmpModPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Utility_AmpMod_Noise();
	if (nAmpModPtr->parList)
		FreeList_UniParMgr(&nAmpModPtr->parList);
	if (nAmpModPtr->parSpec == GLOBAL) {
		free(nAmpModPtr);
		nAmpModPtr = NULL;
	}
	return(TRUE);

}

/****************************** BandwidthModeList *****************************/

/*
 * This function initialises the 'bandwidthMode' list array
 */

NameSpecifier *
BandwidthModeList_Utility_AmpMod_Noise(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("HZ"),	UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_HZ },
			{ wxT("ERB"),	UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_ERB },
			{ 0, 			UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_NULL },
		};
	return(&modeList[index]);

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
Init_Utility_AmpMod_Noise(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_AmpMod_Noise");

	if (parSpec == GLOBAL) {
		if (nAmpModPtr != NULL)
			Free_Utility_AmpMod_Noise();
		if ((nAmpModPtr = (NAmpModPtr) malloc(sizeof(NAmpMod))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (nAmpModPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	nAmpModPtr->parSpec = parSpec;
	nAmpModPtr->updateProcessVariablesFlag = TRUE;
	nAmpModPtr->bandwidthMode = UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_ERB;
	nAmpModPtr->bandwidth = 1.0;
	nAmpModPtr->ranSeed = -1;

	if (!SetUniParList_Utility_AmpMod_Noise()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_AmpMod_Noise();
		return(FALSE);
	}
	nAmpModPtr->fTInv = NULL;
	nAmpModPtr->kUpp = NULL;
	nAmpModPtr->normFactor = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_AmpMod_Noise(void)
{
	static const WChar	*funcName = wxT("SetUniParList_Utility_AmpMod_Noise");
	UniParPtr	pars;

	if ((nAmpModPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_AMPMOD_NOISE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = nAmpModPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_AMPMOD_NOISE_BANDWIDTHMODE], wxT("BW_MODE"),
	  wxT("Noise bandwidth mode ('Hz', or 'ERB')."),
	  UNIPAR_NAME_SPEC,
	  &nAmpModPtr->bandwidthMode, BandwidthModeList_Utility_AmpMod_Noise(0),
	  (void * (*)) SetBandwidthMode_Utility_AmpMod_Noise);
	SetPar_UniParMgr(&pars[UTILITY_AMPMOD_NOISE_BANDWIDTH], wxT("BANDWIDTH"),
	  wxT("Bandwidth ('Hz' or 'ERB scaler')."),
	  UNIPAR_REAL,
	  &nAmpModPtr->bandwidth, NULL,
	  (void * (*)) SetBandwidth_Utility_AmpMod_Noise);
	SetPar_UniParMgr(&pars[UTILITY_AMPMOD_NOISE_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 produces a different seed each run)."),
	  UNIPAR_LONG,
	  &nAmpModPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_Utility_AmpMod_Noise);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_AmpMod_Noise(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_AmpMod_Noise");

	if (nAmpModPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (nAmpModPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(nAmpModPtr->parList);

}

/****************************** SetBandwidthMode ******************************/

/*
 * This function sets the module's bandwidthMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBandwidthMode_Utility_AmpMod_Noise(WChar * theBandwidthMode)
{
	static const WChar	*funcName = wxT(
	  "SetBandwidthMode_Utility_AmpMod_Noise");
	int		specifier;

	if (nAmpModPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theBandwidthMode,
		BandwidthModeList_Utility_AmpMod_Noise(0))) ==
		  UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theBandwidthMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	nAmpModPtr->updateProcessVariablesFlag = TRUE;
	nAmpModPtr->bandwidthMode = specifier;
	return(TRUE);

}

/****************************** SetBandwidth **********************************/

/*
 * This function sets the module's bandwidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBandwidth_Utility_AmpMod_Noise(Float theBandwidth)
{
	static const WChar	*funcName = wxT("SetBandwidth_Utility_AmpMod_Noise");

	if (nAmpModPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	nAmpModPtr->updateProcessVariablesFlag = TRUE;
	nAmpModPtr->bandwidth = theBandwidth;
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_Utility_AmpMod_Noise(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_Utility_AmpMod_Noise");

	if (nAmpModPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	nAmpModPtr->updateProcessVariablesFlag = TRUE;
	nAmpModPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_AmpMod_Noise(void)
{

	DPrint(wxT("?? Utility Module Parameters:-\n"));
	DPrint(wxT("\tbandwidthMode = %s \n"), BandwidthModeList_Utility_AmpMod_Noise(
	  nAmpModPtr->bandwidthMode)->name);
	DPrint(wxT("\tbandwidth = %g ??\n"), nAmpModPtr->bandwidth);
	DPrint(wxT("\tranSeed = %ld ??\n"), nAmpModPtr->ranSeed);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_AmpMod_Noise(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_AmpMod_Noise");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	nAmpModPtr = (NAmpModPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_AmpMod_Noise(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_AmpMod_Noise");

	if (!SetParsPointer_Utility_AmpMod_Noise(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_AmpMod_Noise(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = nAmpModPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_AmpMod_Noise;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_AmpMod_Noise;
	theModule->PrintPars = PrintPars_Utility_AmpMod_Noise;
	theModule->RunProcess = Process_Utility_AmpMod_Noise;
	theModule->SetParsPointer = SetParsPointer_Utility_AmpMod_Noise;
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
CheckData_Utility_AmpMod_Noise(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_AmpMod_Noise");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((nAmpModPtr->bandwidthMode == UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_HZ) &&
	  (nAmpModPtr->bandwidth <= 1.0)) {
		NotifyError(wxT("%s: Bandwidth must be greater than 1.0 in 'Hz' ")
		  wxT("bandwidth mode."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_Utility_AmpMod_Noise(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_Utility_AmpMod_Noise");
	int		i, cFIndex;
	NAmpModPtr	p = nAmpModPtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Utility_AmpMod_Noise();
		if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
			return(FALSE);
		if ((p->kUpp = (int *) calloc(outSignal->numChannels,
		  sizeof(int))) == NULL) {
			NotifyError(wxT("%s: Out of memory for kUpp array (%d)"), funcName,
			  outSignal->numChannels);
			return(FALSE);
		}
		if ((p->normFactor = (Float *) calloc(outSignal->numChannels,
		  sizeof(Float))) == NULL) {
			NotifyError(wxT("%s: Out of memory for normFactor array (%d)"), funcName,
			  outSignal->numChannels);
			return(FALSE);
		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		CopyInfo_SignalData(_OutSig_EarObject(data), _InSig_EarObject(data, 0));
		if ((p->fTInv = InitArray_FFT(outSignal->length, TRUE, 1)) == NULL) {
			NotifyError(wxT("%s: Out of memory for fT fft structure."), funcName);
			return(FALSE);
		}
		p->fTInv->plan[0] = DSAM_FFTW_NAME(plan_dft_c2r_1d)(p->fTInv->fftLen,
		  (Complx *) p->fTInv->data, p->fTInv->data, FFTW_ESTIMATE);
		for (i = 0; i < outSignal->numChannels; i++) {
			cFIndex = i / outSignal->interleaveLevel;
			p->kUpp[i] = (int) floor(0.5 * ((p->bandwidthMode ==
			  UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_HZ)? p->bandwidth:
			  ERBFromF_Bandwidth(outSignal->info.cFArray[cFIndex]) *
			  p->bandwidth) * outSignal->dt * outSignal->length + 0.5);
			p->normFactor[i] = 1.0 / sqrt((Float) p->kUpp[i]); // after amultiplying with normFactor each noiseband has 0dB output level
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		/*** Put reset (to zero ?) code here ***/
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Utility_AmpMod_Noise(void)
{
	if (nAmpModPtr->kUpp) {
		free(nAmpModPtr->kUpp);
		nAmpModPtr->kUpp = NULL;
	}
	if (nAmpModPtr->normFactor) {
		free(nAmpModPtr->normFactor);
		nAmpModPtr->normFactor = NULL;
	}
	FreeArray_FFT(&nAmpModPtr->fTInv);
	return(TRUE);

}

/****************************** Process ***************************************/

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
Process_Utility_AmpMod_Noise(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_AmpMod_Noise");
	register ChanData	 *inPtr, *outPtr, *fftPtr, sineFactor;
	int		chan, cFIndex;
	ChanLen	i;
	RandParsPtr		randParsPtr;
	SignalDataPtr	inSignal, outSignal;
	NAmpModPtr	p = nAmpModPtr;

	inSignal = _InSig_EarObject(data, 0);
	if (!data->threadRunFlag) {
		if (!CheckData_Utility_AmpMod_Noise(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Noise Amplitude Modulation ")
		  wxT("Module process."));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels, inSignal->length,
		  inSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Utility_AmpMod_Noise(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan];
		outPtr = outSignal->channel[chan];
		randParsPtr = &data->randPars[chan];
		CreateNoiseBand_FFT(p->fTInv, 0, randParsPtr, 1, p->kUpp[chan]);
		fftPtr = p->fTInv->data;
		if (p->bandwidthMode == UTILITY_AMPMOD_NOISE_BANDWIDTHMODE_HZ)
			for (i = 0; i < data->outSignal->length; i++)
				*outPtr++ = *inPtr++ * *fftPtr++ * p->normFactor[chan];
		else {
			cFIndex = chan / outSignal->interleaveLevel;
			sineFactor = outSignal->dt * outSignal->info.cFArray[cFIndex];
			for (i = 0; i < data->outSignal->length; i++)
				*outPtr++ = *inPtr++ * *fftPtr++ * sin(i * sineFactor * PIx2) *
				  p->normFactor[chan];
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
