/**********************
 *
 * File:		StFMTone.c
 * Purpose:		Frequency modulated pure tone signal generation paradigm.
 * Comments:	Written using ModuleProducer version 1.5.
 * Author:		Almudena
 * Created:		Nov 28 1995
 * Updated:		12 Mar 1997
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
#include "StFMTone.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FMTonePtr	fMTonePtr = NULL;

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
Free_PureTone_FM(void)
{
	if (fMTonePtr == NULL)
		return(FALSE);
	if (fMTonePtr->parList)
		FreeList_UniParMgr(&fMTonePtr->parList);
	if (fMTonePtr->parSpec == GLOBAL) {
		free(fMTonePtr);
		fMTonePtr = NULL;
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
Init_PureTone_FM(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_PureTone_FM");

	if (parSpec == GLOBAL) {
		if (fMTonePtr != NULL)
			Free_PureTone_FM();
		if ((fMTonePtr = (FMTonePtr) malloc(sizeof(FMTone))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fMTonePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	fMTonePtr->parSpec = parSpec;
	fMTonePtr->frequency = 1000.0;
	fMTonePtr->intensity = DEFAULT_INTENSITY;
	fMTonePtr->duration = 0.1;
	fMTonePtr->dt = DEFAULT_DT;
	fMTonePtr->phase = 0.0;
	fMTonePtr->modulationDepth = 80.0;
	fMTonePtr->modulationFrequency = 2000.0;
	fMTonePtr->modulationPhase = 0.0;

	if (!SetUniParList_PureTone_FM()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PureTone_FM();
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
SetUniParList_PureTone_FM(void)
{
	static const WChar *funcName = wxT("SetUniParList_PureTone_FM");
	UniParPtr	pars;

	if ((fMTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_FM_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = fMTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_FM_FREQUENCY], wxT("FREQUENCY"),
	  wxT("Frequency (Hz)."),
	  UNIPAR_REAL,
	  &fMTonePtr->frequency, NULL,
	  (void * (*)) SetFrequency_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &fMTonePtr->intensity, NULL,
	  (void * (*)) SetIntensity_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_PHASE], wxT("PHASE"),
	  wxT("Phase (degrees)."),
	  UNIPAR_REAL,
	  &fMTonePtr->phase, NULL,
	  (void * (*)) SetPhase_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_MODULATIONDEPTH], wxT("MOD_DEPTH"),
	  wxT("Modulation depth (%)."),
	  UNIPAR_REAL,
	  &fMTonePtr->modulationDepth, NULL,
	  (void * (*)) SetModulationDepth_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_MODULATIONFREQUENCY], wxT("MOD_FREQ"),
	  wxT("Modulation frequency (Hz)."),
	  UNIPAR_REAL,
	  &fMTonePtr->modulationFrequency, NULL,
	  (void * (*)) SetModulationFrequency_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_MODULATIONPHASE], wxT("MOD_PHASE"),
	  wxT("Modulation phase (degrees)."),
	  UNIPAR_REAL,
	  &fMTonePtr->modulationPhase, NULL,
	  (void * (*)) SetModulationPhase_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &fMTonePtr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &fMTonePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_FM);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_FM(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (fMTonePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(fMTonePtr->parList);

}

/****************************** SetFrequency **********************************/

/*
 * This function sets the module's frequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFrequency_PureTone_FM(Float theFrequency)
{
	static const WChar	*funcName = wxT("SetFrequency_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->frequency = theFrequency;
	return(TRUE);

}

/****************************** SetIntensity **********************************/

/*
 * This function sets the module's intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIntensity_PureTone_FM(Float theIntensity)
{
	static const WChar	*funcName = wxT("SetIntensity_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->intensity = theIntensity;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_PureTone_FM(Float theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_PureTone_FM(Float theSamplingInterval)
{
	static const WChar	*funcName = wxT("SetSamplingInterval_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetPhase **************************************/

/*
 * This function sets the module's phase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPhase_PureTone_FM(Float thePhase)
{
	static const WChar	*funcName = wxT("SetPhase_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->phase = thePhase;
	return(TRUE);

}

/****************************** SetModulationDepth ****************************/

/*
 * This function sets the module's modulationDepth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationDepth_PureTone_FM(Float theModulationDepth)
{
	static const WChar	*funcName = wxT("SetModulationDepth_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->modulationDepth = theModulationDepth;
	return(TRUE);

}

/****************************** SetModulationFrequency ************************/

/*
 * This function sets the module's modulationFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationFrequency_PureTone_FM(Float theModulationFrequency)
{
	static const WChar	*funcName = wxT("SetModulationFrequency_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->modulationFrequency = theModulationFrequency;
	return(TRUE);

}

/****************************** SetModulationPhase ****************************/

/*
 * This function sets the module's modulationPhase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationPhase_PureTone_FM(Float theModulationPhase)
{
	static const WChar	*funcName = wxT("SetModulationPhase_PureTone_FM");

	if (fMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->modulationPhase = theModulationPhase;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_PureTone_FM(void)
{
	DPrint(wxT("Frequeny Modulated PureTone Module Parameters:-\n"));
	DPrint(wxT("\tFrequency = %g Hz,"), fMTonePtr->frequency);
	DPrint(wxT("\tIntensity = %g dB SPL\n"), fMTonePtr->intensity);
	DPrint(wxT("\tDuration = %g ms,"),
	  MSEC(fMTonePtr->duration));
	DPrint(wxT("\tsamplingInterval = %g ms\n"),
	  MSEC(fMTonePtr->dt));
	DPrint(wxT("\tPhase = %g degrees,"), fMTonePtr->phase);
	DPrint(wxT("\tModulation depth = %g %%\n"),
	  fMTonePtr->modulationDepth);
	DPrint(wxT("\tModulation frequency/phase = %g Hz / %g degrees\n"),
	  fMTonePtr->modulationFrequency, fMTonePtr->modulationPhase);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_PureTone_FM(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PureTone_FM");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	fMTonePtr = (FMTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_FM(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PureTone_FM");

	if (!SetParsPointer_PureTone_FM(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PureTone_FM(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = fMTonePtr;
	theModule->Free = Free_PureTone_FM;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_FM;
	theModule->PrintPars = PrintPars_PureTone_FM;
	theModule->RunProcess = GenerateSignal_PureTone_FM;
	theModule->SetParsPointer = SetParsPointer_PureTone_FM;
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
CheckData_PureTone_FM(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_PureTone_FM");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** GenerateSignal ********************************/

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
GenerateSignal_PureTone_FM(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("GenerateSignal_PureTone_FM");
	ChanLen		i, t;
	register	Float		amplitude, modulationIndex;
	register	ChanData	 *dataPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_PureTone_FM(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Frequency Modulated Pure Tone ")
		  wxT("stimulus"));
		if ( !InitOutSignal_EarObject(data, 1, (ChanLen) floor(fMTonePtr->
		  duration / fMTonePtr->dt + 0.5), fMTonePtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	modulationIndex = fMTonePtr->modulationDepth / 100.0 * fMTonePtr->
	  frequency / fMTonePtr->modulationFrequency;
	amplitude = RMS_AMP(fMTonePtr->intensity) * SQRT_2;
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->length;
	   i++, t++)
		*(dataPtr++) = amplitude * sin(PIx2 * fMTonePtr->frequency *
		  t * _OutSig_EarObject(data)->dt - modulationIndex * cos(2 * PI *
		  fMTonePtr->modulationFrequency * t * _OutSig_EarObject(data)->dt +
		  DEGREES_TO_RADS(fMTonePtr->modulationPhase)) +
		    DEGREES_TO_RADS(fMTonePtr->phase));

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

