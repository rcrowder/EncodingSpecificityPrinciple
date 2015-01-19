/**********************
 *
 * File:		UtMathOp.c
 * Purpose:		This utility carries out simple mathematical operations.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 * Author:		L.  P. O'Mard
 * Created:		08 Jul 2002
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
#include "UtMathOp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

MathOpPtr	mathOpPtr = NULL;

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
Free_Utility_MathOp(void)
{
	if (mathOpPtr == NULL)
		return(FALSE);
	if (mathOpPtr->parList)
		FreeList_UniParMgr(&mathOpPtr->parList);
	if (mathOpPtr->parSpec == GLOBAL) {
		free(mathOpPtr);
		mathOpPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOperatorModeList **************************/

/*
 * This function initialises the 'operatorMode' list array
 */

BOOLN
InitOperatorModeList_Utility_MathOp(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("ADD"),		UTILITY_MATHOP_OPERATORMODE_ADD },
			{ wxT("ABSOLUTE"),	UTILITY_MATHOP_OPERATORMODE_ABSOLUTE },
			{ wxT("OFFSET"),	UTILITY_MATHOP_OPERATORMODE_OFFSET },
			{ wxT("SCALE"),		UTILITY_MATHOP_OPERATORMODE_SCALE },
			{ wxT("SQR"),		UTILITY_MATHOP_OPERATORMODE_SQR },
			{ wxT("SUBTRACT"),	UTILITY_MATHOP_OPERATORMODE_SUBTRACT },
			{ NULL,				UTILITY_MATHOP_OPERATORMODE_SUBTRACT },
		};
	mathOpPtr->operatorModeList = modeList;
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
Init_Utility_MathOp(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_MathOp");

	if (parSpec == GLOBAL) {
		if (mathOpPtr != NULL)
			Free_Utility_MathOp();
		if ((mathOpPtr = (MathOpPtr) malloc(sizeof(MathOp))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (mathOpPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	mathOpPtr->parSpec = parSpec;
	mathOpPtr->operatorMode = UTILITY_MATHOP_OPERATORMODE_OFFSET;
	mathOpPtr->operand = 0.0;

	InitOperatorModeList_Utility_MathOp();
	if (!SetUniParList_Utility_MathOp()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_MathOp();
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
SetUniParList_Utility_MathOp(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_MathOp");
	UniParPtr	pars;

	if ((mathOpPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_MATHOP_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = mathOpPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_MATHOP_OPERATORMODE], wxT("OPERATOR"),
	  wxT("Mathematical operator ('add', 'modulus', 'offset', 'scale', 'sqr' ")
	  wxT("or 'subtract')."),
	  UNIPAR_NAME_SPEC,
	  &mathOpPtr->operatorMode, mathOpPtr->operatorModeList,
	  (void * (*)) SetOperatorMode_Utility_MathOp);
	SetPar_UniParMgr(&pars[UTILITY_MATHOP_OPERAND], wxT("OPERAND"),
	  wxT("Operand (only used in 'scale' and 'offset' modes at present)."),
	  UNIPAR_REAL,
	  &mathOpPtr->operand, NULL,
	  (void * (*)) SetOperand_Utility_MathOp);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_MathOp(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_MathOp");

	if (mathOpPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (mathOpPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(mathOpPtr->parList);

}

/****************************** SetOperatorMode *******************************/

/*
 * This function sets the module's operatorMode parameter.
 * It returns TRUE if the OPERATORMODE is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOperatorMode_Utility_MathOp(WChar * theOperatorMode)
{
	static const WChar	*funcName = wxT("SetOperatorMode_Utility_MathOp");
	int		specifier;

	if (mathOpPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOperatorMode,
		mathOpPtr->operatorModeList)) == UTILITY_MATHOP_OPERATORMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOperatorMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mathOpPtr->operatorMode = specifier;
	mathOpPtr->parList->pars[UTILITY_MATHOP_OPERAND].enabled = ((mathOpPtr->
	  operatorMode == UTILITY_MATHOP_OPERATORMODE_OFFSET) || (mathOpPtr->
	  operatorMode == UTILITY_MATHOP_OPERATORMODE_SCALE));
	mathOpPtr->parList->updateFlag = TRUE;
	return(TRUE);

}

/****************************** SetOperand ************************************/

/*
 * This function sets the module's operand parameter.
 * It returns TRUE if the OPERATORMODE is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOperand_Utility_MathOp(Float theOperand)
{
	static const WChar	*funcName = wxT("SetOperand_Utility_MathOp");

	if (mathOpPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mathOpPtr->operand = theOperand;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_MathOp(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_MathOp");

	if (mathOpPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Mathematical Operation Utility Module Parameters:-\n"));
	DPrint(wxT("\tOperatorMode = %s, "), mathOpPtr->operatorModeList[mathOpPtr->
	  operatorMode].name);
	if (mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_SCALE)
		DPrint(wxT("\tOperand = %g (units).\n"), mathOpPtr->operand);
	else
		DPrint(wxT("\n"));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_MathOp(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_MathOp");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	mathOpPtr = (MathOpPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_MathOp(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_MathOp");

	if (!SetParsPointer_Utility_MathOp(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_MathOp(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = mathOpPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Utility_MathOp;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_MathOp;
	theModule->PrintPars = PrintPars_Utility_MathOp;
	theModule->RunProcess = Process_Utility_MathOp;
	theModule->SetParsPointer = SetParsPointer_Utility_MathOp;
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
CheckData_Utility_MathOp(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_MathOp");
	SignalDataPtr	inSignal[2];

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);

	if ((mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_ADD) ||
	  (mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_SUBTRACT)) {
		if (data->numInSignals != 2) {
			NotifyError(wxT("%s: Process must be receive 2 inputs in '%s' ")
			  wxT("mode."), funcName, mathOpPtr->operatorModeList[mathOpPtr->
			  operatorMode].name);
			return(FALSE);
		}
		inSignal[0] = _InSig_EarObject(data, 0);
		inSignal[1] = _InSig_EarObject(data, 1);
		if (!CheckPars_SignalData(inSignal[1])) {
			NotifyError(wxT("%s: Input signals not correctly set."), funcName);
			return(FALSE);
		}
		if (!SameType_SignalData(inSignal[0], inSignal[1])) {
			NotifyError(wxT("%s: Input signals are not the same."), funcName);
			return(FALSE);
		}
		if (inSignal[0]->interleaveLevel != inSignal[1]->interleaveLevel) {
			NotifyError(wxT("%s: Input signals do not have the same ")
			  wxT("interleave level."), funcName);
			return(FALSE);
		}
	}
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
Process_Utility_MathOp(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_MathOp");
	register ChanData	 *inPtr1, *inPtr2 = NULL, *outPtr;
	int		chan;
	ChanLen	i;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_MathOp(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Mathematical operation module ")
		  wxT("process"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr1 = _InSig_EarObject(data, 0)->channel[chan];
		if ((mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_ADD) ||
		  (mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_SUBTRACT))
			inPtr2 = _InSig_EarObject(data, 1)->channel[chan];
		outPtr = outSignal->channel[chan];
		switch (mathOpPtr->operatorMode) {
		case UTILITY_MATHOP_OPERATORMODE_ADD:
			for (i = 0; i < outSignal->length; i++)
				*outPtr++ = *inPtr1++ + *inPtr2++;
			break;
		case UTILITY_MATHOP_OPERATORMODE_ABSOLUTE:
			for (i = 0; i < outSignal->length; i++)
				*outPtr++ = fabs(*inPtr1++);
			break;
		case UTILITY_MATHOP_OPERATORMODE_OFFSET:
			for (i = 0; i < outSignal->length; i++)
				*outPtr++ = *inPtr1++ + mathOpPtr->operand;
			break;
		case UTILITY_MATHOP_OPERATORMODE_SCALE:
			for (i = 0; i < outSignal->length; i++)
				*outPtr++ = *inPtr1++ * mathOpPtr->operand;
			break;
		case UTILITY_MATHOP_OPERATORMODE_SQR:
			for (i = 0; i < outSignal->length; i++, inPtr1++)
				*outPtr++ = SQR(*inPtr1);
			break;
		case UTILITY_MATHOP_OPERATORMODE_SUBTRACT:
			for (i = 0; i < outSignal->length; i++)
				*outPtr++ = *inPtr1++ - *inPtr2++;
			break;
		default:
			;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

