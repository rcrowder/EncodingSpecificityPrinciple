/**********************
 *
 * File:		UtIterRipple.c
 * Purpose:		This routine transforms a signal using the Iterated Ripple
 *				algorithm as described in: "A time domain description for the
 *				pitch strength of iterated rippled noide", W.A. Yost, R.
 * 				Patterson, S. Sheft. J.Acoust. Soc. Am. 999(2), February 1996
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				It generates two classes of Iterated Ripple signal:
 * 				IRSO Iterated Rippled Signal ADD-ORIGINAL
 * 				IRSS Iterated Rippled Signal ADD-SAME
 * Authors:		Almudena Eustaquio-Martin, L. P. O'Mard
 * Created:		04-JUN-1996.
 * Updated:
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
#include "UtString.h"
#include "UtIterRipple.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IterRipplePtr	iterRipplePtr = NULL;

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
Free_Utility_IteratedRipple(void)
{
	if (iterRipplePtr == NULL)
		return(FALSE);
	if (iterRipplePtr->parList)
		FreeList_UniParMgr(&iterRipplePtr->parList);
	if (iterRipplePtr->parSpec == GLOBAL) {
		free(iterRipplePtr);
		iterRipplePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Utility_IteratedRipple(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("IRSO"),	ITERRIPPLE_IRSO_MODE },
					{ wxT("IRSS"),	ITERRIPPLE_IRSS_MODE },
					{ NULL,			ITERRIPPLE_NULL }
				};
	iterRipplePtr->modeList = modeList;
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
Init_Utility_IteratedRipple(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_IteratedRipple");

	if (parSpec == GLOBAL) {
		if (iterRipplePtr != NULL)
			Free_Utility_IteratedRipple();
		if ((iterRipplePtr = (IterRipplePtr) malloc(sizeof(IterRipple))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (iterRipplePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	iterRipplePtr->parSpec = parSpec;
	iterRipplePtr->numIterations = 2;
	iterRipplePtr->mode = ITERRIPPLE_IRSO_MODE;
	iterRipplePtr->delay = 0.0;
	iterRipplePtr->signalMultiplier = 1.0;

	InitModeList_Utility_IteratedRipple();
	if (!SetUniParList_Utility_IteratedRipple()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_IteratedRipple();
		return(FALSE);
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
SetUniParList_Utility_IteratedRipple(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_IteratedRipple");
	UniParPtr	pars;

	if ((iterRipplePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_ITERATEDRIPPLE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = iterRipplePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_ITERATEDRIPPLE_NUMITERATIONS], wxT("NUM_IT"),
	  wxT("Number of iterations."),
	  UNIPAR_INT,
	  &iterRipplePtr->numIterations, NULL,
	  (void * (*)) SetNumIterations_Utility_IteratedRipple);
	SetPar_UniParMgr(&pars[UTILITY_ITERATEDRIPPLE_MODE], wxT("MODE"),
	  wxT("Ripple signal mode: 'IRSO' - add original, 'IRSS' - add same."),
	  UNIPAR_NAME_SPEC,
	  &iterRipplePtr->mode, iterRipplePtr->modeList,
	  (void * (*)) SetMode_Utility_IteratedRipple);
	SetPar_UniParMgr(&pars[UTILITY_ITERATEDRIPPLE_DELAY], wxT("DELAY"),
	  wxT("Delay (s)."),
	  UNIPAR_REAL,
	  &iterRipplePtr->delay, NULL,
	  (void * (*)) SetDelay_Utility_IteratedRipple);
	SetPar_UniParMgr(&pars[UTILITY_ITERATEDRIPPLE_SIGNALMULTIPLIER],
	  wxT("GAIN"),
	  wxT("Gain (scalar)."),
	  UNIPAR_REAL,
	  &iterRipplePtr->signalMultiplier, NULL,
	  (void * (*)) SetSignalMultiplier_Utility_IteratedRipple);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_IteratedRipple(void)
{
	static const WChar	*funcName = wxT(
	  "GetUniParListPtr_Utility_IteratedRipple");

	if (iterRipplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (iterRipplePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(iterRipplePtr->parList);

}

/****************************** SetNumIterations ******************************/

/*
 * This function sets the module's numIterations parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIterations_Utility_IteratedRipple(int theNumIterations)
{
	static const WChar	*funcName = wxT(
	  "SetNumIterations_Utility_IteratedRipple");

	if (iterRipplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (theNumIterations <= 0) {
		NotifyError(wxT("%s: The number of iterations must be bigger the 0 ")
		  wxT("(%d)."), funcName, theNumIterations);
		return(FALSE);
	}
	iterRipplePtr->numIterations = theNumIterations;
	return(TRUE);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_IteratedRipple(WChar *theMode)
{
	static const WChar	*funcName = wxT("SetMode_Utility_IteratedRipple");
	int		specifier;

	if (iterRipplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
	  iterRipplePtr->modeList)) == ITERRIPPLE_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	iterRipplePtr->mode = specifier;
	return(TRUE);

}

/****************************** SetDelay **************************************/

/*
 * This function sets the module's delay parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDelay_Utility_IteratedRipple(Float theDelay)
{
	static const WChar	*funcName = wxT("SetDelay_Utility_IteratedRipple");

	if (iterRipplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iterRipplePtr->delay = theDelay;
	return(TRUE);

}

/****************************** SetSignalMultiplier ***************************/

/*
 * This function sets the module's signalMultiplier parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSignalMultiplier_Utility_IteratedRipple(Float theSignalMultiplier)
{
	static const WChar *funcName = wxT(
	  "SetSignalMultiplier_Utility_IteratedRipple");

	if (iterRipplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((theSignalMultiplier < -1.0) || (theSignalMultiplier > 1.0)) {
		NotifyError(wxT("%s: Value must be between (-1, +1) (%d)."), funcName,
		  theSignalMultiplier);
		return(FALSE);
	}
	iterRipplePtr->signalMultiplier = theSignalMultiplier;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_IteratedRipple(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_IteratedRipple");

	if (iterRipplePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Iterated Ripple Utility Module Parameters:-\n"));
	DPrint(wxT("\tNumber of iterations = %d,"),
	  iterRipplePtr->numIterations);
	DPrint(wxT("\tNetwork mode = %s,"),
	  iterRipplePtr->modeList[iterRipplePtr->mode].name);
	DPrint(wxT("\tDelay = %g (ms),\n"), MSEC(iterRipplePtr->delay));
	DPrint(wxT("\tSignal multiplier = %g (units).\n"),
	  iterRipplePtr->signalMultiplier);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_IteratedRipple(ModulePtr theModule)
{
	static const WChar	*funcName = wxT(
	  "SetParsPointer_Utility_IteratedRipple");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	iterRipplePtr = (IterRipplePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_IteratedRipple(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_IteratedRipple");

	if (!SetParsPointer_Utility_IteratedRipple(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_IteratedRipple(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = iterRipplePtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_IteratedRipple;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_IteratedRipple;
	theModule->PrintPars = PrintPars_Utility_IteratedRipple;
	theModule->RunProcess = Process_Utility_IteratedRipple;
	theModule->SetParsPointer = SetParsPointer_Utility_IteratedRipple;
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
CheckData_Utility_IteratedRipple(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_IteratedRipple");
	Float signalDuration;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
	if (iterRipplePtr->delay > signalDuration)	{
		NotifyError(wxT("%s: Delay (%g ms) is longer than the signal duration ")
		  wxT("(%g ms)"), funcName, MSEC(iterRipplePtr->delay), MSEC(
		  signalDuration));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
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
 * The 'Add_SignalData' routine is not used here as it would not have been easy
 * to make it thread-safe, as the input signal does not change no. of channels.
 */

BOOLN
Process_Utility_IteratedRipple(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_IteratedRipple");
	register	ChanData	 *inPtr, *outPtr;
	int		j, chan;
	ChanLen		i, samplesDelay;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_IteratedRipple(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Iterated Ripple Utility process."));
		if (!InitOutTypeFromInSignal_EarObject(data, 0)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	InitOutDataFromInSignal_EarObject(data);
	outSignal = _OutSig_EarObject(data);
	switch (iterRipplePtr->mode) {
	case ITERRIPPLE_IRSO_MODE:
		for (j = 0; j < iterRipplePtr->numIterations; j++)	{
			Delay_SignalData(outSignal, iterRipplePtr->delay);
			for (chan = outSignal->offset; chan < outSignal->numChannels;
			  chan++) {
			  	inPtr = _InSig_EarObject(data, 0)->channel[chan];
				outPtr = outSignal->channel[chan];
				for (i = 0; i < outSignal->length ; i++) {
					*outPtr *= iterRipplePtr->signalMultiplier;
					*outPtr++ += *inPtr++;
				}
			}
		}
		break;
	case ITERRIPPLE_IRSS_MODE:
		samplesDelay = (ChanLen) ( iterRipplePtr->delay / outSignal->dt);
		for (j = 0; j < iterRipplePtr->numIterations; j++)	{
			for (chan = outSignal->offset; chan < outSignal->numChannels;
			  chan++) {
				outPtr = outSignal->channel[chan] + outSignal->length -
				  samplesDelay - 1;
				for (i = 0; i < outSignal->length - samplesDelay; i++, outPtr--)
					*(outPtr + samplesDelay) += *outPtr *
					  iterRipplePtr->signalMultiplier;
			}
		}
		break;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

