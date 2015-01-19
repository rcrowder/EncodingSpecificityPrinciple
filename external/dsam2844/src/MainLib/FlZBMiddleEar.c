/**********************
 *
 * File:		FlZBMiddleEar.c
 * Purpose:		This is the Zilany and Bruce middle-ear filter module.
 *				It was revised from code provided by I. C. Bruce
 * 				(zbcatmodel2007v2.tar.gz).
 *				Zilany, M. S. A. and Bruce, I. C. (2006). "Modeling auditory-
 * 				nerve responses for high sound pressure levels in the normal
 * 				and impaired auditory periphery," Journal of the Acoustical
 * 				Society of America 120(3):1446-1466.
 * 				Zilany, M. S. A. and Bruce, I. C. (2007). "Representation of
 * 				the vowel /eh/ in normal and impaired auditory nerve fibers:
 * 				Model predictions of responses in cats," Journal of the
 * 				Acoustical Society of America 122(1):402-417.
 * Comments:	Written using ModuleProducer version 1.6.0 (Mar  3 2008).
 *				This first revision follows the existing code with as little
 *				change as possible.
 * 				It uses an EarObject to allocate the memory for the mey arrays,
 *				but note that the mey array should not be set up as a sub-process
 *				for threaded operation.
 * Author:		Revised by L. P. O'Mard
 * Created:		03 Mar 2008
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
#include "FlZBMiddleEar.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FLZBMEarPtr	fLZBMEarPtr = NULL;

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
Free_Filter_ZBMiddleEar(void)
{
	/* static const WChar	*funcName = wxT("Free_Filter_ZBMiddleEar"); */

	if (fLZBMEarPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Filter_ZBMiddleEar();
	if (fLZBMEarPtr->parList)
		FreeList_UniParMgr(&fLZBMEarPtr->parList);
	if (fLZBMEarPtr->parSpec == GLOBAL) {
		free(fLZBMEarPtr);
		fLZBMEarPtr = NULL;
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
Init_Filter_ZBMiddleEar(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Filter_ZBMiddleEar");

	if (parSpec == GLOBAL) {
		if (fLZBMEarPtr != NULL)
			Free_Filter_ZBMiddleEar();
		if ((fLZBMEarPtr = (FLZBMEarPtr) malloc(sizeof(FLZBMEar))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fLZBMEarPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	fLZBMEarPtr->parSpec = parSpec;
	fLZBMEarPtr->updateProcessVariablesFlag = TRUE;
	fLZBMEarPtr->gainMax = 41.1405;
	fLZBMEarPtr->preWarpingFreq = 1.0e3;

	if (!SetUniParList_Filter_ZBMiddleEar()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Filter_ZBMiddleEar();
		return(FALSE);
	}
	fLZBMEarPtr->lastInput = NULL;
	fLZBMEarPtr->mey = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Filter_ZBMiddleEar(void)
{
	static const WChar	*funcName = wxT("SetUniParList_Filter_ZBMiddleEar");
	UniParPtr	pars;

	if ((fLZBMEarPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  FILTER_ZBMIDDLEEAR_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = fLZBMEarPtr->parList->pars;
	SetPar_UniParMgr(&pars[FILTER_ZBMIDDLEEAR_GAINMAX], wxT("GAIN_MAX"),
	  wxT("Maximum middle ear gain (?)."),
	  UNIPAR_REAL,
	  &fLZBMEarPtr->gainMax, NULL,
	  (void * (*)) SetGainMax_Filter_ZBMiddleEar);
	SetPar_UniParMgr(&pars[FILTER_ZBMIDDLEEAR_PREWARPINGFREQ], wxT("PREWARPING_FREQ"),
	  wxT("Pre-warping frequency (Hz)."),
	  UNIPAR_REAL,
	  &fLZBMEarPtr->preWarpingFreq, NULL,
	  (void * (*)) SetPreWarpingFreq_Filter_ZBMiddleEar);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Filter_ZBMiddleEar(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Filter_ZBMiddleEar");

	if (fLZBMEarPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (fLZBMEarPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(fLZBMEarPtr->parList);

}

/****************************** SetGainMax ************************************/

/*
 * This function sets the module's gainMax parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGainMax_Filter_ZBMiddleEar(Float theGainMax)
{
	static const WChar	*funcName = wxT("SetGainMax_Filter_ZBMiddleEar");

	if (fLZBMEarPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fLZBMEarPtr->updateProcessVariablesFlag = TRUE;
	fLZBMEarPtr->gainMax = theGainMax;
	return(TRUE);

}

/****************************** SetPreWarpingFreq *****************************/

/*
 * This function sets the module's preWarpingFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPreWarpingFreq_Filter_ZBMiddleEar(Float thePreWarpingFreq)
{
	static const WChar	*funcName = wxT(
	  "SetPreWarpingFreq_Filter_ZBMiddleEar");

	if (fLZBMEarPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fLZBMEarPtr->updateProcessVariablesFlag = TRUE;
	fLZBMEarPtr->preWarpingFreq = thePreWarpingFreq;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Filter_ZBMiddleEar(void)
{

	DPrint(wxT("Zilany & Bruce (JASA 2006, 2007) Middle-ear Filter ")
			wxT("Module Parameters:-\n"));
	DPrint(wxT("\tgainMax = %g ??\n"), fLZBMEarPtr->gainMax);
	DPrint(wxT("\tpreWarpingFreq = %g ??\n"), fLZBMEarPtr->preWarpingFreq);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Filter_ZBMiddleEar(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Filter_ZBMiddleEar");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	fLZBMEarPtr = (FLZBMEarPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Filter_ZBMiddleEar(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Filter_ZBMiddleEar");

	if (!SetParsPointer_Filter_ZBMiddleEar(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Filter_ZBMiddleEar(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = fLZBMEarPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Filter_ZBMiddleEar;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_ZBMiddleEar;
	theModule->PrintPars = PrintPars_Filter_ZBMiddleEar;
	theModule->RunProcess = RunModel_Filter_ZBMiddleEar;
	theModule->SetParsPointer = SetParsPointer_Filter_ZBMiddleEar;
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
CheckData_Filter_ZBMiddleEar(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Filter_ZBMiddleEar");

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
InitProcessVariables_Filter_ZBMiddleEar(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_Filter_ZBMiddleEar");
	int		chan;
	Float	c, cSqrd;
	FLZBMEarPtr	p = fLZBMEarPtr;
	SignalDataPtr	outSignal;

  	outSignal = _OutSig_EarObject(data);
	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_ZBMiddleEar();
		if ((p->lastInput = (Float *) calloc(outSignal->numChannels,
		  sizeof(Float))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for 'lastOutput'."),
			  funcName);
		 	return(FALSE);
		}
		p->mey = Init_EarObject(wxT("NULL"));
		if (!InitOutSignal_EarObject(p->mey, outSignal->numChannels *
		  ZB_ME_NUM_STATE_VECTORS, outSignal->length, outSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise 'y' memory."), funcName);
			return(FALSE);
		}
		c  = PIx2 * p->preWarpingFreq / tan(PIx2 / 2.0 * p->preWarpingFreq *
		  _InSig_EarObject(data, 0)->dt);
		cSqrd = c * c;
		p->m11 = c / (c + 693.48);
		p->m12 = (693.48 - c) / c;

		p->m21 = 1 / (cSqrd + 11053 * c + 1.163e8);
		p->m22 = -2 * cSqrd + 2.326e8;
		p->m23 = cSqrd - 11053 * c + 1.163e8;
		p->m24 = cSqrd + 1356.3 * c + 7.4417e8;
		p->m25 = -2 * cSqrd + 14.8834e8;
		p->m26 = cSqrd - 1356.3 * c + 7.4417e8;

		p->m31 = 1 / (cSqrd + 4620 * c + 909059944);
		p->m32 = -2 * cSqrd + 2 * 909059944;
		p->m33 = cSqrd - 4620 * c + 909059944;
		p->m34 = 5.7585e5 * c + 7.1665e7;
		p->m35 = 14.333e7;
		p->m36 = 7.1665e7 - 5.7585e5 * c;
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetOutSignal_EarObject(p->mey);
		for (chan = 0; chan < outSignal->numChannels; chan++)
			p->lastInput[chan] = 0.0;
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Filter_ZBMiddleEar(void)
{
	FLZBMEarPtr	p = fLZBMEarPtr;

	if (p->lastInput) {
		free(p->lastInput);
		p->lastInput = NULL;
	}
	Free_EarObject(&p->mey);
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
RunModel_Filter_ZBMiddleEar(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_Filter_ZBMiddleEar");
	register ChanData	 *inPtr, *outPtr, *mey1, *mey2, *mey3;
	int		chan, n_1, n_2;
	ChanLen	n;
	SignalDataPtr	inSignal, outSignal, mey;
	FLZBMEarPtr	p = fLZBMEarPtr;

	inSignal = _InSig_EarObject(data, 0);
	if (!data->threadRunFlag) {
		if (!CheckData_Filter_ZBMiddleEar(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Zilany & Bruce (JASA 2006, 2007) ")
				wxT("middle-ear filter process"));
		if (!InitOutSignal_EarObject(data, inSignal->numChannels, inSignal->length,
		  inSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Filter_ZBMiddleEar(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	mey = _OutSig_EarObject(p->mey);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = inSignal->channel[chan];
		outPtr = outSignal->channel[chan];
		mey1 = mey->channel[chan * ZB_ME_NUM_STATE_VECTORS];
		mey2 = mey->channel[chan * ZB_ME_NUM_STATE_VECTORS + 1];
		mey3 = mey->channel[chan * ZB_ME_NUM_STATE_VECTORS + 2];

		for (n = 0; n < outSignal->length; n++, inPtr++, mey1++, mey2++, mey3++) {
			if (n == 0) {
				n_1 = outSignal->length - 1;
				n_2 = n_1 - 1;
				*mey1 = p->m11 * (-p->m12 * *(mey1 + n_1) + *inPtr - p->lastInput[chan]);
			} else {
				n_1 = -1;
				n_2 = (n == 1)? outSignal->length - 2: -2;
				*mey1 = p->m11 * (-p->m12 * *(mey1 + n_1) + *inPtr - *(inPtr - 1));
			}
			*mey2 = p->m21 * (-p->m22 * *(mey2 + n_1) - p->m23 * *(mey2 + n_2) + p->m24 *
					*mey1 + p->m25 * *(mey1 + n_1) + p->m26 * *(mey1 + n_2));
			*mey3  = p->m31 * (-p->m32 * *(mey3 + n_1) - p->m33 * *(mey3 + n_2) + p->m34 *
					*mey2 + p->m35 * *(mey2 + n_1) + p->m36 * *(mey2 + n_2));
			*outPtr++ = *mey3 / p->gainMax;
		}
		p->lastInput[chan] = *(inPtr - 1);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

