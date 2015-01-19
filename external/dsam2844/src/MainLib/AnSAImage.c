/**********************
 *
 * File:		AnSAImage.c
 * Purpose:		This module implements the AIM stabilised auditory image (SAI)
 *				process.
 *				See Patterson R. D. and Allerhand M. H. (1995) "Time-domain
 *				modeling of peripheral auditory processing: A modular
 *				Architecture and a software platform", J. Acoust. Soc. Am. 98,
 *				1890-1894.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 *				This module uses the UtStrobe utility module.
 *				29-10-97 LPO: Introduced 'dataBuffer' EarObject for the
 *				correct treatment during segment processing.  The previous data
 *				is assumed to be the negative width plus zero.
 *				04-11-97 LPO: Introduced the correct 'strobe is time zero'
 *				behaviour.
 *				04-11-97 LPO: Improved buffer algorithm so the signal segments
 *				less than the SAI length can be used.
 *				18-11-97 LPO: Introduced the inputDecayRate parameter.
 *				11-12-97 LPO: Allowed the "delayed" strobe modes to be processed
 *				correctly, by interpreting delays as an additional shift using
 *				the postive and negative widths.
 *				28-05-98 LPO: Put in check for zero image decay half-life.
 *				09-06-98 LPO: The strobe information is now output as part of
 *				the diagnostics.
 *				12-06-98 LPO: Corrected numLastSamples problem, which was
 *				causing strobes to be processed passed the end of the strobe
 *				data.
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 *				08-10-98 LPO: The initialisation and free'ing of the 'Strobe'
 *				utility module is now done at the same time as the SAI module.
 *				This was necessary for the UniPar implementation.
 * Author:		L. P. O'Mard
 * Created:		12 Oct 1997
 * Updated:		08 Oct 1998
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtString.h"
#include "AnSAImage.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SAImagePtr	sAImagePtr = NULL;

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
Free_Analysis_SAI(void)
{
	if (sAImagePtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_SAI();
	Free_EarObject(&sAImagePtr->strobeData);
	if (sAImagePtr->diagnosticModeList)
		free(sAImagePtr->diagnosticModeList);
	if (sAImagePtr->parList)
		FreeList_UniParMgr(&sAImagePtr->parList);
	if (sAImagePtr->parSpec == GLOBAL) {
		free(sAImagePtr);
		sAImagePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitIntegrationModeList ***********************/

/*
 * This function initialises the 'integrationMode' list array
 */

BOOLN
InitIntegrationModeList_Analysis_SAI(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("STI"),	SAI_INTEGRATION_MODE_STI },
			{ wxT("AC"),	SAI_INTEGRATION_MODE_AC },
			{ NULL,			SAI_INTEGRATION_MODE_NULL },
		};
	sAImagePtr->integrationModeList = modeList;
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
Init_Analysis_SAI(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_SAI");

	if (parSpec == GLOBAL) {
		if (sAImagePtr != NULL)
			Free_Analysis_SAI();
		if ((sAImagePtr = (SAImagePtr) malloc(sizeof(SAImage))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (sAImagePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	sAImagePtr->parSpec = parSpec;
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	sAImagePtr->diagnosticMode = GENERAL_BOOLEAN_OFF;
	sAImagePtr->integrationMode = SAI_INTEGRATION_MODE_STI;
	sAImagePtr->strobeSpecification[0] = '\0';
	sAImagePtr->positiveWidth = 5.0e-3;
	sAImagePtr->negativeWidth = -35.0e-3;
	sAImagePtr->inputDecayRate = 2.5e+3;
	sAImagePtr->imageDecayHalfLife = 30e-3;

	if ((sAImagePtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), sAImagePtr->diagnosticString)) == NULL)
		return(FALSE);
	InitIntegrationModeList_Analysis_SAI();
	if ((sAImagePtr->strobeData = Init_EarObject(wxT("Util_Strobe"))) == NULL) {
		NotifyError(wxT("%s: Could not initialise strobe data EarObject"),
		  funcName);
		return(FALSE);
	}
	if (!SetUniParList_Analysis_SAI()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_SAI();
		return(FALSE);
	}
	DSAM_strcpy(sAImagePtr->diagnosticString, DEFAULT_FILE_NAME);
	sAImagePtr->strobeInSignalIndex = -1;
	sAImagePtr->numThreads = 0;
	sAImagePtr->inputDecay = NULL;
	sAImagePtr->fp = NULL;
	sAImagePtr->decayCount = NULL;
	sAImagePtr->inputCount = NULL;
	sAImagePtr->dataBuffer = NULL;
	sAImagePtr->strobeDataBuffer = NULL;
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_Analysis_SAI(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_SAI");
	UniParPtr	pars;

	if ((sAImagePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  SAI_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = sAImagePtr->parList->pars;
	SetPar_UniParMgr(&pars[SAI_DIAGNOSTIC_MODE], wxT("DIAGNOSTICS"),
	  wxT("Diagnostic mode ('off', 'screen', 'error' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &sAImagePtr->diagnosticMode, sAImagePtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_INTEGRATION_MODE], wxT("INT_MODE"),
	  wxT("Integration mode: 'STI' - stabilised temporal integration, 'AC' - ")
	  wxT("autocorrelation."),
	  UNIPAR_NAME_SPEC,
	  &sAImagePtr->integrationMode, sAImagePtr->integrationModeList,
	  (void * (*)) SetIntegrationMode_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_STROBE_SPECIFICATION], wxT("STROBE_PAR_FILE"),
	  wxT("Strobe module parameter file name."),
	  UNIPAR_MODULE,
	  &sAImagePtr->strobeSpecification, GetUniParListPtr_ModuleMgr(sAImagePtr->
	  strobeData),
	  (void * (*)) SetStrobeSpecification_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_NEGATIVE_WIDTH], wxT("NWIDTH"),
	  wxT("Negative width of auditory image (s)."),
	  UNIPAR_REAL,
	  &sAImagePtr->negativeWidth, NULL,
	  (void * (*)) SetNegativeWidth_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_POSITIVE_WIDTH], wxT("PWIDTH"),
	  wxT("Positive width of auditory image (s)."),
	  UNIPAR_REAL,
	  &sAImagePtr->positiveWidth, NULL,
	  (void * (*)) SetPositiveWidth_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_INPUT_DECAY_RATE], wxT("NAP_DECAY"),
	  wxT("Neural activity pattern (input) decay rate (%/s)"),
	  UNIPAR_REAL,
	  &sAImagePtr->inputDecayRate, NULL,
	  (void * (*)) SetInputDecayRate_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_IMAGE_DECAY_HALF_LIFE], wxT("IMAGE_DECAY"),
	  wxT("Auditory image decay half-life (s)."),
	  UNIPAR_REAL,
	  &sAImagePtr->imageDecayHalfLife, NULL,
	  (void * (*)) SetImageDecayHalfLife_Analysis_SAI);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Analysis_SAI(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_Analysis_SAI");

	if (sAImagePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (sAImagePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(sAImagePtr->parList);

}

/****************************** SetDiagnosticMode ****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_Analysis_SAI(WChar *theDiagnosticMode)
{
	static const WChar	*funcName = wxT("SetDiagnosticMode_Analysis_SAI");

	if (sAImagePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	sAImagePtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  sAImagePtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetIntegrationMode ****************************/

/*
 * This function sets the module's integrationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIntegrationMode_Analysis_SAI(WChar * theIntegrationMode)
{
	static const WChar	*funcName = wxT("SetIntegrationMode_Analysis_SAI");
	int		specifier;

	if (sAImagePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theIntegrationMode,
		sAImagePtr->integrationModeList)) == SAI_INTEGRATION_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName,
		  theIntegrationMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sAImagePtr->integrationMode = specifier;
	return(TRUE);

}

/****************************** SetStrobeSpecification ************************/

/*
 * This function sets the module's strobeSpecification parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStrobeSpecification_Analysis_SAI(WChar *theStrobeSpecification)
{
	static const WChar	*funcName = wxT("SetStrobeSpecification_Analysis_SAI");
	BOOLN	ok;
	ParFilePtr	oldPtr = parFile;

	if (sAImagePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (DSAM_strcmp(theStrobeSpecification, NO_FILE) != 0) {
		parFile = NULL;
		ok = ReadPars_ModuleMgr(sAImagePtr->strobeData, theStrobeSpecification);
		parFile = oldPtr;
		if (!ok) {
			NotifyError(wxT("%s: Could not read strobe utility module ")
			  wxT("parameters."), funcName);
			return(FALSE);
		}
	}
	DSAM_strncpy(sAImagePtr->strobeSpecification, theStrobeSpecification,
	  MAX_FILE_PATH);
	return(TRUE);

}

/****************************** SetPositiveWidth ******************************/

/*
 * This function sets the module's positiveWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPositiveWidth_Analysis_SAI(Float thePositiveWidth)
{
	static const WChar	*funcName = wxT("SetPositiveWidth_Analysis_SAI");

	if (sAImagePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sAImagePtr->positiveWidth = thePositiveWidth;
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetNegativeWidth ******************************/

/*
 * This function sets the module's negativeWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNegativeWidth_Analysis_SAI(Float theNegativeWidth)
{
	static const WChar	*funcName = wxT("SetNegativeWidth_Analysis_SAI");

	if (sAImagePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNegativeWidth > 0.0) {
		NotifyError(wxT("%s: Illegal negative width (%g ms)."), funcName,
		  MSEC(theNegativeWidth));
		return(FALSE);
	}
	sAImagePtr->negativeWidth = theNegativeWidth;
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetInputDecayRate *****************************/

/*
 * This function sets the module's inputDecayRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInputDecayRate_Analysis_SAI(Float theInputDecayRate)
{
	static const WChar	*funcName = wxT("SetInputDecayRate_Analysis_SAI");

	if (sAImagePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sAImagePtr->inputDecayRate = theInputDecayRate;
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetImageDecayHalfLife *************************/

/*
 * This function sets the module's imageDecayHalfLife parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetImageDecayHalfLife_Analysis_SAI(Float theImageDecayHalfLife)
{
	static const WChar	*funcName = wxT("SetImageDecayHalfLife_Analysis_SAI");

	if (sAImagePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theImageDecayHalfLife <= 0.0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g)."), funcName,
		  theImageDecayHalfLife);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sAImagePtr->imageDecayHalfLife = theImageDecayHalfLife;
	return(TRUE);

}

/****************************** SetDelay **************************************/

/*
 * This function sets the delay for the modules local instance of the
 * strobe utility module.
 */

BOOLN
SetDelay_Analysis_SAI(Float theDelay)
{
	return(SetRealPar_ModuleMgr(sAImagePtr->strobeData, wxT("strobe_lag"),
	  theDelay));

}

/****************************** SetDelayTimeout *******************************/

/*
 * This function sets the delay timeout for the modules local instance of the
 * strobe utility module.
 */

BOOLN
SetDelayTimeout_Analysis_SAI(Float theDelayTimeout)
{
	return(SetRealPar_ModuleMgr(sAImagePtr->strobeData, wxT("timeout"),
	  theDelayTimeout));

}

/****************************** SetThresholdDecayRate *************************/

/*
 * This function sets the threshold decay rate for the modules local instance
 * of the strobe utility module.
 */

BOOLN
SetThresholdDecayRate_Analysis_SAI(Float theThresholdDecayRate)
{
	return(SetRealPar_ModuleMgr(sAImagePtr->strobeData, wxT("threshold_decay"),
	  theThresholdDecayRate));

}

/****************************** SetThreshold **********************************/

/*
 * This function sets thethreshold for the modules local instance of the
 * strobe utility module.
 */

BOOLN
SetThreshold_Analysis_SAI(Float theThreshold)
{
	return(SetRealPar_ModuleMgr(sAImagePtr->strobeData, wxT("threshold"),
	  theThreshold));

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the type mode for the modules local instance of the
 * strobe utility module.
 */

BOOLN
SetTypeMode_Analysis_SAI(WChar *theTypeMode)
{
	return(SetPar_ModuleMgr(sAImagePtr->strobeData, wxT("criterion"),
	  theTypeMode));

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_SAI(void)
{
	DPrint(wxT("Stabilised Auditory Image Analysis Module Parameters:-\n"));
	DPrint(wxT("\tStrobeSpecification (%s):\n"), sAImagePtr->
	  strobeSpecification);
	SetDiagnosticsPrefix(wxT("\t"));
	PrintPars_ModuleMgr(sAImagePtr->strobeData);
	SetDiagnosticsPrefix(NULL);
	DPrint(wxT("\tIntegration mode = %s,\n"), sAImagePtr->integrationModeList[
	  sAImagePtr->integrationMode].name);
	DPrint(wxT("\tNegative width = %g ms,"), MSEC(sAImagePtr->negativeWidth));
	DPrint(wxT("\tPositive width = %g ms,\n"), MSEC(sAImagePtr->positiveWidth));
	DPrint(wxT("\tInput decay rate = %g %/ms,"), sAImagePtr->inputDecayRate /
	  MSEC(1.0));
	DPrint(wxT("\tImage decay half-life = %g ms,\n"),
	  MSEC(sAImagePtr->imageDecayHalfLife));
	DPrint(wxT("\tDiagnostic mode: %s.\n"), sAImagePtr->diagnosticModeList[
	  sAImagePtr->diagnosticMode].name);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_SAI(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_SAI");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	sAImagePtr = (SAImagePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_SAI(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_SAI");

	if (!SetParsPointer_Analysis_SAI(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_SAI(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = sAImagePtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_Analysis_SAI;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_SAI;
	theModule->PrintPars = PrintPars_Analysis_SAI;
	theModule->ResetProcess = ResetProcess_Analysis_SAI;
	theModule->RunProcess = Process_Analysis_SAI;
	theModule->SetParsPointer = SetParsPointer_Analysis_SAI;
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
CheckData_Analysis_SAI(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_SAI");
	Float	width, strobeDelay;
	int		strobeType;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	strobeType = *GetUniParPtr_ModuleMgr(sAImagePtr->strobeData, wxT(
	  "criterion"))->valuePtr.nameList.specifier;
	if (strobeType == STROBE_USER_MODE) {
		if (data->numInSignals < 2) {
			NotifyError(wxT("%s: In strobe 'USER' mode two input EarObjects ")
			  wxT("are\nrequired (%d)"), funcName, data->numInSignals);
			return(FALSE);
		}
		if (!CheckInit_SignalData(_InSig_EarObject(data, 1),
		  wxT("CheckData_Analysis_SAI"))) {
			NotifyError(wxT("%s: Second EarObject required for USER mode not ")
			  wxT("initialised."), funcName);
			return(FALSE);
		}
		if (!SameType_SignalData(_InSig_EarObject(data, 0), _InSig_EarObject(
		  data, 1))) {
			NotifyError(wxT("%s: Input signals are not the same."), funcName);
			return(FALSE);
		}
	}
	width = sAImagePtr->positiveWidth - sAImagePtr->negativeWidth +
	  _InSig_EarObject(data, 0)->dt;
	if (width <= 0.0) {
		NotifyError(wxT("%s: Illegal frame width (%g -> %g ms)."), funcName,
		  MSEC(sAImagePtr->negativeWidth), MSEC(sAImagePtr->positiveWidth));
		return(FALSE);
	}
	strobeDelay = *GetUniParPtr_ModuleMgr(sAImagePtr->strobeData,
	  wxT("strobe_lag"))->valuePtr.r;
	if ((strobeType == STROBE_PEAK_SHADOW_POSITIVE_MODE) && (sAImagePtr->
	  positiveWidth < strobeDelay)) {
		NotifyError(wxT("%s: The positive width (%g ms) must be less than ")
		  wxT("strobe delay (%g ms)."), funcName, MSEC(sAImagePtr->
		  positiveWidth), MSEC(strobeDelay));
		return(FALSE);
	}
	return(TRUE);

}

/****************************OutputStrobeData *********************************/

/*
 * This routine outputs the strobe data to the specified file stream.
 * It expects the strobe data to have been initialised correctly.
 */

void
OutputStrobeData_Analysis_SAI(void)
{
	/* static const WChar *funcName = wxT("OutputStrobeData_Analysis_SAI"); */
	int		chan;
	ChanLen	i, t;
	SignalDataPtr	signal;

	signal = _OutSig_EarObject(sAImagePtr->strobeData);
	DSAM_fprintf(sAImagePtr->fp, wxT("Time (s)"));
	for (chan = signal->offset; chan < signal->numChannels; chan++)
		DSAM_fprintf(sAImagePtr->fp, wxT("\t[%d]"), chan);
	DSAM_fprintf(sAImagePtr->fp, wxT("\n"));
	for (i = 0, t = signal->timeIndex; i < signal->length; i++, t++) {
		DSAM_fprintf(sAImagePtr->fp, wxT("%g"), t * signal->dt +
		  signal->outputTimeOffset);
		for (chan = signal->offset; chan < signal->numChannels; chan++)
			DSAM_fprintf(sAImagePtr->fp, wxT("\t%g"), signal->channel[chan][i]);
		DSAM_fprintf(sAImagePtr->fp, wxT("\n"));
	}

}

/**************************** InitInputDecayArray *****************************/

/*
 * This function initialises the input decay array for the process variables.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 * This function assumes that the inputDecayRate is the percentage decay per
 * second.
 * The number of samples per second = 1 / dt.
 * The decay values scale the input by 1.0 at the strobe point, e.g. in the SAI
 * the values fall to the left and rise to the right of 0 s, when the
 * inputDecayRate > 0.0.
 * This is the opposite sign to that in AIM because in AIM the decay array is
 * applied from the end of the channel, backwards.
 */

BOOLN
InitInputDecayArray_Analysis_SAI(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitInputDecayArray_Analysis_SAI");
	Float	decayPerSample, *decayPtr, totalDecay;
	ChanLen	i;

	if ((sAImagePtr->inputDecay = (Float *) calloc(_OutSig_EarObject(data)->
	  length, sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Could not initialise input decay array"),
		  funcName);
		return(FALSE);
	}
	decayPerSample = -sAImagePtr->inputDecayRate / 100.0 * _OutSig_EarObject(
	  data)->dt;
	totalDecay = 1.0 + sAImagePtr->zeroIndex * decayPerSample;
	decayPtr = sAImagePtr->inputDecay;
	for (i = 0; i < _OutSig_EarObject(data)->length; i++) {
		if (totalDecay >= 1.0)
			*decayPtr++ = 1.0;
		else if (totalDecay <= 0.0)
			*decayPtr++ = 0.0;
		else
			*decayPtr++ = totalDecay;
		totalDecay -= decayPerSample;
	}
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Analysis_SAI(EarObjectPtr data)
{
	int		i;
	SAImagePtr	p = sAImagePtr;

	ResetOutSignal_EarObject(data);
	SetNumWindowFrames_SignalData(_OutSig_EarObject(data), 0);
	ResetOutSignal_EarObject(p->dataBuffer);
	ResetOutSignal_EarObject(p->strobeDataBuffer);
	if (data->threadRunFlag)
		p->inputCount[data->threadIndex] = 0;
	else {
		for (i = 0; i < p->numThreads; i++)
			p->inputCount[i] = 0;
	}
	for (i = _OutSig_EarObject(data)->offset; i < _OutSig_EarObject(data)->
	  numChannels; i++)
		p->decayCount[i] = 0;

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_Analysis_SAI(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Analysis_SAI");
	SAImagePtr	p = sAImagePtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_SAI();
		OpenDiagnostics_NSpecLists(&p->fp, p->diagnosticModeList,
		  p->diagnosticMode);
		if (!InitSubProcessList_EarObject(data, SAI_NUM_SUB_PROCESSES)) {
			NotifyError(wxT("%s: Could not initialise %d sub-process list for ")
			  wxT("process."), funcName, SAI_NUM_SUB_PROCESSES);
			return(FALSE);
		}
		data->subProcessList[SAI_STROBEDATA] = p->strobeData;
		if ((p->dataBuffer = Init_EarObject(wxT("NULL"))) == NULL) {
			NotifyError(wxT("%s: Could not initialise previous data EarObject"),
			  funcName);
			return(FALSE);
		}
		if ((p->strobeDataBuffer = Init_EarObject(wxT("NULL"))) == NULL) {
			NotifyError(wxT("%s: Could not initialise previous strobe data ")
			  wxT("EarObject"), funcName);
			return(FALSE);
		}
		p->zeroIndex = (ChanLen) floor(-p->negativeWidth / _InSig_EarObject(
		  data, 0)->dt + 0.5);
		p->positiveWidthIndex = _OutSig_EarObject(data)->length - p->zeroIndex;
		if (!InitInputDecayArray_Analysis_SAI(data)) {
			NotifyError(wxT("%s: failed in to initialise input decay array"),
			  funcName);
			return(FALSE);
		}
		if ((p->decayCount = (ChanLen *) calloc(_OutSig_EarObject(data)->
		  numChannels, sizeof(ChanLen))) == NULL) {
			NotifyError(wxT("%s: Could not initialise decayCount array"),
			  funcName);
			return(FALSE);
		}
		if ((p->inputCount = (ChanLen *) calloc(data->numThreads, sizeof(
		  ChanLen))) == NULL) {
			NotifyError(wxT("%s: Out of memory for inputCount array."),
			  funcName);
			return(FALSE);
		}
		p->strobeInSignalIndex = (*GetUniParPtr_ModuleMgr(p->strobeData,
		  wxT("criterion"))->valuePtr.nameList.specifier == STROBE_USER_MODE)?
		  1: 0;
		if (!InitOutSignal_EarObject(p->dataBuffer, _OutSig_EarObject(data)->
		  numChannels, _OutSig_EarObject(data)->length, _OutSig_EarObject(
		  data)->dt)) {
			NotifyError(wxT("%s: Cannot initialise channels for previous ")
			  wxT("data."), funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(p->strobeDataBuffer, _OutSig_EarObject(
		  data)->numChannels, _OutSig_EarObject(data)->length,
		  _OutSig_EarObject(data)->dt)) {
			NotifyError(wxT("%s: Cannot initialise channels for previous ")
			  wxT("strobe data."), funcName);
			return(FALSE);
		}
		p->numThreads = data->numThreads;
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetProcess_Analysis_SAI(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Analysis_SAI(void)
{
	Free_EarObject(&sAImagePtr->dataBuffer);
	Free_EarObject(&sAImagePtr->strobeDataBuffer);
	if (sAImagePtr->inputDecay) {
		free(sAImagePtr->inputDecay);
		sAImagePtr->inputDecay = NULL;
	}
	if (sAImagePtr->fp) {
		fclose(sAImagePtr->fp);
		sAImagePtr->fp = NULL;
	}
	if (sAImagePtr->decayCount) {
		free(sAImagePtr->decayCount);
		sAImagePtr->decayCount = NULL;
	}
	if (sAImagePtr->inputCount) {
		free(sAImagePtr->inputCount);
		sAImagePtr->inputCount = NULL;
	}
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** PushBufferData **********************************/

/*
 * This routine pushes the input and strobe data into the previous data
 * buffers.
 */

void
PushBufferData_Analysis_SAI(EarObjectPtr data, ChanLen frameLength)
{
	/* static const WChar	*funcName = wxT("PushBufferData_Analysis_SAI"); */
	register ChanData	*inPtr, *outPtr, *inStrobePtr, *outStrobePtr;
	int		chan;
	ChanLen	i, shiftLength, inputCount;
	SAImagePtr	p = sAImagePtr;

	inputCount = *(p->inputCount + data->threadIndex);
	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(
	  data)->numChannels; chan++) {
		if (frameLength == _OutSig_EarObject(data)->length) {
			inPtr = _InSig_EarObject(data, 0)->channel[chan] + inputCount;
			inStrobePtr = _OutSig_EarObject(p->strobeData)->channel[chan] +
			  inputCount;
			shiftLength = frameLength;
		} else {
			inPtr = _OutSig_EarObject(p->dataBuffer)->channel[chan] +
			  frameLength;
			inStrobePtr = _OutSig_EarObject(p->strobeDataBuffer)->channel[
			  chan] + frameLength;
			shiftLength = _OutSig_EarObject(data)->length - frameLength;
		}
		outPtr = _OutSig_EarObject(p->dataBuffer)->channel[chan];
		outStrobePtr = _OutSig_EarObject(p->strobeDataBuffer)->channel[chan];
		for (i = 0; i < shiftLength; i++) {
			*outPtr++ = *inPtr++;
			*outStrobePtr++ = *inStrobePtr++;
		}
		if (frameLength < _OutSig_EarObject(data)->length) {
			inPtr = _InSig_EarObject(data, 0)->channel[chan] + inputCount;
			inStrobePtr = _OutSig_EarObject(p->strobeData)->channel[chan] +
			  inputCount;
			for (i = 0; i < frameLength; i++) {
				*outPtr++ = *inPtr++;
				*outStrobePtr++ = *inStrobePtr++;
			}
		}
	}

}

/**************************** DecayImage **************************************/

/*
 * This routine applies a decay to the image buffer, dependent upon the
 * 'decayCount' for each channel.
 */

void
DecayImage_Analysis_SAI(EarObjectPtr data, int chan)
{
	register ChanData	*dataPtr, scale;
	ChanLen		i;

	if (!sAImagePtr->decayCount[chan])
		return;
	scale = exp(-LN_2 / sAImagePtr->imageDecayHalfLife * sAImagePtr->decayCount[
	  chan] * _InSig_EarObject(data, 0)->dt);
	dataPtr = _OutSig_EarObject(data)->channel[chan];
	for (i = 0; i < _OutSig_EarObject(data)->length; i++)
		*dataPtr++ *= scale;
	sAImagePtr->decayCount[chan] = 0;

}

/**************************** ProcessFrameSection *****************************/

/*
 * This routine carries out the SAI image process.
 * The values in the data buffer have already been scaled, but all values
 * used from the input signal must be scaled.
 * The routine decays the SAI data according to the duration of the input
 * signal duration.
 * It only applies the scale when a full frame has been processed.
 */

void
ProcessFrameSection_Analysis_SAI(EarObjectPtr data, ChanData **strobeStatePtrs,
  ChanData **dataPtrs, ChanLen strobeOffset, ChanLen frameOffset,
  ChanLen sectionLength)
{
	register ChanData	*inPtr, *outPtr, *strobeStatePtr, scaler;
	int		chan;
	Float	*inputDecayPtr;
	ChanLen	i, j, sectionEnd, inputCount;
	SAImagePtr	p = sAImagePtr;

	if (sectionLength == 0)
		return;
	inputCount = *(p->inputCount + data->threadIndex);
	sectionEnd = frameOffset + sectionLength;
	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(
	  data)->numChannels; chan++) {
		strobeStatePtr = strobeStatePtrs[chan] + strobeOffset;
		for (i = frameOffset; i < sectionEnd; i++, strobeStatePtr++) {
			if (*strobeStatePtr > 0.0) {
				DecayImage_Analysis_SAI(data, chan);
				inPtr = _OutSig_EarObject(p->dataBuffer)->channel[chan] + i;
				outPtr = _OutSig_EarObject(data)->channel[chan];
				inputDecayPtr = p->inputDecay;
				switch (p->integrationMode) {
				case SAI_INTEGRATION_MODE_STI:
					for (j = i; j < _OutSig_EarObject(data)->length; j++)
						*outPtr++ += *inPtr++ * *inputDecayPtr++;
					inPtr = _InSig_EarObject(data, 0)->channel[chan] +
					  inputCount;
					for (j = 0; j < i; j++)
						*outPtr++ += *inPtr++ * *inputDecayPtr++;
					break;
				case SAI_INTEGRATION_MODE_AC:
					scaler = *(dataPtrs[chan] + strobeOffset + i - frameOffset);
					for (j = i; j < _OutSig_EarObject(data)->length; j++)
						*outPtr++ += *inPtr++ * *inputDecayPtr++ * scaler;
					inPtr = _InSig_EarObject(data, 0)->channel[chan] +
					  inputCount;
					for (j = 0; j < i; j++)
						*outPtr++ += *inPtr++ * *inputDecayPtr++ * scaler;
					break;
				default:
					;
				}
			}
			p->decayCount[chan]++;
		}
	}

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
 * With repeated calls the Signal memory is only allocated once, then re-used.
 * The signalData process is "Manually" connected to the appropriate input
 * signal.
 * The width of the frame must be "(positive - negative) / dt + 1".  The
 * addtional 1 is necessary because of the zero.  Thank you God, for helping me
 * to find that.
 */

BOOLN
Process_Analysis_SAI(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Analysis_SAI");
	BOOLN	endOfData;
	int		chan;
	Float	dt;
	ChanLen	frameLength, positiveWidthIndex, negativeWidthIndex, *inputCountPtr;
	SAImagePtr	p = sAImagePtr;
	SignalDataPtr	inSignal, outSignal;
	EarObjectPtr	strobeData;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_SAI(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Stabilised Auditory Image Module ")
		  wxT("process"));
		dt = _InSig_EarObject(data, 0)->dt;
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels,  (ChanLen) floor((p->positiveWidth - p->negativeWidth) /
		  dt + 1.5), dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		SetStaticTimeFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), p->
		  negativeWidth);
		if (!InitProcessVariables_Analysis_SAI(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		TempInputConnection_EarObject(data, p->strobeData, 1);
		 p->strobeData->inSignal[0] = data->inSignal[p->strobeInSignalIndex];
		if (p->diagnosticMode)
			OutputStrobeData_Analysis_SAI();
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	strobeData = data->subProcessList[SAI_STROBEDATA];
	if (!RunProcessStandard_ModuleMgr(strobeData)) {
		NotifyError(wxT("%s: Could not process strobe data ."), funcName);
		return(FALSE);
	}
	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	inputCountPtr = p->inputCount + data->threadIndex;
	positiveWidthIndex = p->positiveWidthIndex - strobePtr->numLastSamples;
	negativeWidthIndex = p->zeroIndex + strobePtr->numLastSamples;
	for (frameLength = outSignal->length, endOfData = FALSE; !endOfData; ) {
		if (*inputCountPtr + outSignal->length > inSignal->length) {
			endOfData = TRUE;
			frameLength = inSignal->length - *inputCountPtr;
			if (frameLength < positiveWidthIndex) {
				positiveWidthIndex = frameLength;
				negativeWidthIndex = 0;
			} else if ((frameLength - positiveWidthIndex) < negativeWidthIndex)
				negativeWidthIndex = frameLength - positiveWidthIndex;
		}
		ProcessFrameSection_Analysis_SAI(data, _OutSig_EarObject(p->
		  strobeDataBuffer)->channel, _OutSig_EarObject(p->dataBuffer)->channel,
		  p->zeroIndex + strobePtr->numLastSamples, 0, positiveWidthIndex);
		ProcessFrameSection_Analysis_SAI(data, _OutSig_EarObject(strobeData)->
		  channel, inSignal->channel, *inputCountPtr, positiveWidthIndex,
		  negativeWidthIndex);
		PushBufferData_Analysis_SAI(data, frameLength);
		*inputCountPtr = (endOfData)? 0: *inputCountPtr + frameLength;
	}
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++)
		DecayImage_Analysis_SAI(data, chan);
	outSignal->numWindowFrames++;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
