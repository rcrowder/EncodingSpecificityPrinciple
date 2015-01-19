/**********************
 *
 * File:		UtConvMonaural.c
 * Purpose:		This module converts a binaural signal to a monaural signal.
 * Comments:	Written using ModuleProducer version 1.10 (Jan  3 1997).
 * Author:		L. P. O'Mard
 * Convd:		5 Mar 1997
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
#include "UtConvMonaural.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CMonauralPtr	cMonauralPtr = NULL;

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
Free_Utility_ConvMonaural(void)
{
	if (cMonauralPtr == NULL)
		return(FALSE);
	if (cMonauralPtr->parList)
		FreeList_UniParMgr(&cMonauralPtr->parList);
	if (cMonauralPtr->parSpec == GLOBAL) {
		free(cMonauralPtr);
		cMonauralPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This function initialises the 'mode' list array
 */

BOOLN
InitModeList_Utility_ConvMonaural(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("ADD"),	UTILITY_CONVMONAURAL_MODE_ADD },
			{ wxT("LEFT"),	UTILITY_CONVMONAURAL_MODE_LEFT },
			{ wxT("RIGHT"),	UTILITY_CONVMONAURAL_MODE_RIGHT },
			{ NULL,			UTILITY_CONVMONAURAL_MODE_NULL },
		};
	cMonauralPtr->modeList = modeList;
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
Init_Utility_ConvMonaural(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_ConvMonaural");

	if (parSpec == GLOBAL) {
		if (cMonauralPtr != NULL)
			Free_Utility_ConvMonaural();
		if ((cMonauralPtr = (CMonauralPtr) malloc(sizeof(CMonaural))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (cMonauralPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	cMonauralPtr->parSpec = parSpec;
	cMonauralPtr->mode = UTILITY_CONVMONAURAL_MODE_ADD;

	InitModeList_Utility_ConvMonaural();
	if (!SetUniParList_Utility_ConvMonaural()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_ConvMonaural();
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
SetUniParList_Utility_ConvMonaural(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_ConvMonaural");
	UniParPtr	pars;

	if ((cMonauralPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_CONVMONAURAL_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = cMonauralPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_CONVMONAURAL_MODE], wxT("MODE"),
	  wxT("Monaural output mode ('Add', 'left' or 'right')."),
	  UNIPAR_NAME_SPEC,
	  &cMonauralPtr->mode, cMonauralPtr->modeList,
	  (void * (*)) SetMode_Utility_ConvMonaural);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_ConvMonaural(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_ConvMonaural");

	if (cMonauralPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (cMonauralPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. ")
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(cMonauralPtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_ConvMonaural(WChar * theMode)
{
	static const WChar	*funcName = wxT("SetMode_Utility_ConvMonaural");
	int		specifier;

	if (cMonauralPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
		cMonauralPtr->modeList)) == UTILITY_CONVMONAURAL_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMonauralPtr->mode = specifier;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_ConvMonaural(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_ConvMonaural");

	if (cMonauralPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Convert to Monaural Utility Module Parameters:-\n"));
	DPrint(wxT("\tMode = %s.\n"), cMonauralPtr->modeList[cMonauralPtr->mode].name);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_ConvMonaural(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_ConvMonaural");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	cMonauralPtr = (CMonauralPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_ConvMonaural(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_ConvMonaural");

	if (!SetParsPointer_Utility_ConvMonaural(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_ConvMonaural(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = cMonauralPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_ConvMonaural;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_ConvMonaural;
	theModule->PrintPars = PrintPars_Utility_ConvMonaural;
	theModule->RunProcess = Process_Utility_ConvMonaural;
	theModule->SetParsPointer = SetParsPointer_Utility_ConvMonaural;
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
CheckData_Utility_ConvMonaural(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_ConvMonaural");

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
 * It can be called repeatedly with different parameter values if required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used
 * Two loops are used in the main channel process loop so that reseting the
 * original contents to zero is not necessary.
 */

BOOLN
Process_Utility_ConvMonaural(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_ConvMonaural");
	register	ChanData	 *inPtr, *outPtr;
	int		i, outChan, inChan;
	SignalDataPtr	inSignal, outSignal;
	ChanLen	j;
	CMonauralPtr	p = cMonauralPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_ConvMonaural(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Convert binaural -> monaural ")
		  wxT("utility"));
		if (!InitOutSignal_EarObject(data, (uShort) (_InSig_EarObject(data, 0)->
		  numChannels / _InSig_EarObject(data, 0)->interleaveLevel), _InSig_EarObject(data, 0)->
		  length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), 1);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	for (outChan = outSignal->offset; outChan < outSignal->numChannels;
	  outChan++) {
		inChan = outChan * inSignal->interleaveLevel;
		outPtr = outSignal->channel[outChan];
		if ((p->mode == UTILITY_CONVMONAURAL_MODE_ADD) || (p->mode == UTILITY_CONVMONAURAL_MODE_LEFT))
			for (i = 0; i < inSignal->interleaveLevel; i++) {
				inPtr = inSignal->channel[inChan];
				outPtr = outSignal->channel[outChan];
				for (j = 0; j < inSignal->length; j++)
					*(outPtr++) = *(inPtr++);
			}
		if ((p->mode == UTILITY_CONVMONAURAL_MODE_ADD) || (p->mode == UTILITY_CONVMONAURAL_MODE_RIGHT))
			for (i = 1; i < inSignal->interleaveLevel; i++) {
				inPtr = inSignal->channel[inChan + i];
				outPtr = outSignal->channel[outChan];
				if (p->mode == UTILITY_CONVMONAURAL_MODE_ADD)
					for (j = 0; j < inSignal->length; j++)
						*(outPtr++) += *(inPtr++);
				else
					for (j = 0; j < inSignal->length; j++)
						*(outPtr++) = *(inPtr++);
			}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

