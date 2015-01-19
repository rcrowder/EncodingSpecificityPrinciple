/**********************
 *
 * File:		UtCompress.c
 * Purpose:		Compresses an input signal using log or power compression.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard revised from AIM code
 * Created:		28 Jun 1996
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
#include "UtCompress.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CompressionPtr	compressionPtr = NULL;

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
Free_Utility_Compression(void)
{
	if (compressionPtr == NULL)
		return(FALSE);
	if (compressionPtr->parList)
		FreeList_UniParMgr(&compressionPtr->parList);
	if (compressionPtr->parSpec == GLOBAL) {
		free(compressionPtr);
		compressionPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Utility_Compression(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("LOG"),			COMPRESS_LOG_MODE },
					{ wxT("POWER"),			COMPRESS_POWER_MODE },
					{ NULL,					COMPRESS_NULL }

				};
	compressionPtr->modeList = modeList;
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
Init_Utility_Compression(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_Compression");

	if (parSpec == GLOBAL) {
		if (compressionPtr != NULL)
			Free_Utility_Compression();
		if ((compressionPtr = (CompressionPtr) malloc(sizeof(Compression))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (compressionPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	compressionPtr->parSpec = parSpec;
	compressionPtr->mode = COMPRESS_LOG_MODE;
	compressionPtr->signalMultiplier = 1.0;
	compressionPtr->powerExponent = 1.0;
	compressionPtr->minResponse = 0.0;

	InitModeList_Utility_Compression();
	if (!SetUniParList_Utility_Compression()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_Compression();
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
SetUniParList_Utility_Compression(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_Compression");
	UniParPtr	pars;

	if ((compressionPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_COMPRESSION_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = compressionPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_COMPRESSION_MODE], wxT("MODE"),
	  wxT("Compression mode ('log' or 'power')."),
	  UNIPAR_NAME_SPEC,
	  &compressionPtr->mode, compressionPtr->modeList,
	  (void * (*)) SetMode_Utility_Compression);
	SetPar_UniParMgr(&pars[UTILITY_COMPRESSION_SIGNALMULTIPLIER], wxT(
	  "MULTIPLIER"),
	  wxT("Signal multiplier (arbitrary units)."),
	  UNIPAR_REAL,
	  &compressionPtr->signalMultiplier, NULL,
	  (void * (*)) SetSignalMultiplier_Utility_Compression);
	SetPar_UniParMgr(&pars[UTILITY_COMPRESSION_POWEREXPONENT], wxT("EXPONENT"),
	  wxT("Power exponent ('power' mode only)."),
	  UNIPAR_REAL,
	  &compressionPtr->powerExponent, NULL,
	  (void * (*)) SetPowerExponent_Utility_Compression);
	SetPar_UniParMgr(&pars[UTILITY_COMPRESSION_MINRESPONSE], wxT(
	  "MIN_RESPONSE"),
	  wxT("Minimum response from module (arbitrary units)."),
	  UNIPAR_REAL,
	  &compressionPtr->minResponse, NULL,
	  (void * (*)) SetMinResponse_Utility_Compression);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_Compression(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_Compression");

	if (compressionPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (compressionPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(compressionPtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_Compression(WChar *theMode)
{
	static const WChar	*funcName = wxT("SetMode_Utility_Compression");
	int		specifier;

	if (compressionPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
	  compressionPtr->modeList)) == COMPRESS_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	compressionPtr->mode = specifier;
	switch (compressionPtr->mode) {
	case COMPRESS_LOG_MODE:
		compressionPtr->parList->pars[
		  UTILITY_COMPRESSION_POWEREXPONENT].enabled = FALSE;
		compressionPtr->parList->pars[
		  UTILITY_COMPRESSION_MINRESPONSE].enabled = TRUE;
		break;
	case COMPRESS_POWER_MODE:
		compressionPtr->parList->pars[
		  UTILITY_COMPRESSION_POWEREXPONENT].enabled = TRUE;
		compressionPtr->parList->pars[
		  UTILITY_COMPRESSION_MINRESPONSE].enabled = FALSE;
		break;
	default:
		;
	}
	compressionPtr->parList->updateFlag = TRUE;
	return(TRUE);

}

/****************************** SetSignalMultiplier ***************************/

/*
 * This function sets the module's signalMultiplier parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSignalMultiplier_Utility_Compression(Float theSignalMultiplier)
{
	static const WChar	*funcName = wxT(
	  "SetSignalMultiplier_Utility_Compression");

	if (compressionPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	compressionPtr->signalMultiplier = theSignalMultiplier;
	return(TRUE);

}

/****************************** SetPowerExponent ******************************/

/*
 * This function sets the module's powerExponent parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPowerExponent_Utility_Compression(Float thePowerExponent)
{
	static const WChar	*funcName = wxT("SetPowerExponent_Utility_Compression");

	if (compressionPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((thePowerExponent > 1.0) || (thePowerExponent < 0.0)) {
		NotifyError(wxT("%s: Value must be between 0 and 1 (%g)."),
		  thePowerExponent);
		return(FALSE);
	}
	compressionPtr->powerExponent = thePowerExponent;
	return(TRUE);

}

/****************************** SetMinResponse ********************************/

/*
 * This function sets the module's minResponse parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMinResponse_Utility_Compression(Float theMinResponse)
{
	static const WChar	*funcName = wxT("SetMinResponse_Utility_Compression");

	if (compressionPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theMinResponse < 0.0) {
		NotifyError(wxT("%s: This value must be greater then zero (%d)."),
		  funcName, theMinResponse);
		return(FALSE);
	}
	compressionPtr->minResponse = theMinResponse;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_Compression(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_Compression");

	if (compressionPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Compression Utility Module Parameters:-\n"));
	DPrint(wxT("\tMode = %s,"),
	  compressionPtr->modeList[compressionPtr->mode].name);
	DPrint(wxT("\tSignal multiplier = %g (units)\n"),
	  compressionPtr->signalMultiplier);
	DPrint(wxT("\tPower exponent = %g,"), compressionPtr->powerExponent);
	DPrint(wxT("\tMinimum response = %g (units).\n"), compressionPtr->
	  minResponse);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_Compression(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_Compression");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	compressionPtr = (CompressionPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Compression(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_Compression");

	if (!SetParsPointer_Utility_Compression(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_Compression(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = compressionPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_Compression;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_Compression;
	theModule->PrintPars = PrintPars_Utility_Compression;
	theModule->RunProcess = Process_Utility_Compression;
	theModule->SetParsPointer = SetParsPointer_Utility_Compression;
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
CheckData_Utility_Compression(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_Compression");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
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
 */

BOOLN
Process_Utility_Compression(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_Compression");
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	ChanLen	i;
	SignalDataPtr	outSignal;
	CompressionPtr p = compressionPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_Compression(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Compression Utility Module"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		p->minInput = pow(10, p->minResponse);
		p->scale = p->signalMultiplier / p->powerExponent;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		switch (p->mode) {
		case COMPRESS_LOG_MODE:
			for (i = 0; i < _InSig_EarObject(data, 0)->length; i++) {
				*outPtr = (p->minInput > *inPtr)? p->minResponse: log10(*inPtr);
				 *outPtr++ *= p->signalMultiplier;
				 inPtr++;
			}
			break;
		case COMPRESS_POWER_MODE:
			for (i = 0; i < _InSig_EarObject(data, 0)->length; i++)
				*outPtr++ = pow(fabs(*inPtr++),
				  p->powerExponent) * p->scale;
			break;
		default:
			;
		}
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

