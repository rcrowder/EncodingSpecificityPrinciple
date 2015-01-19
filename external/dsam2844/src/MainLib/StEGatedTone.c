/**********************
 *
 * File:		StEGatedTone.c
 * Purpose:		This module generates exponential damped or ramped sinusoids.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 * Author:		L. P. O'Mard
 * Created:		14 Dec 1997
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
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtString.h"
#include "StEGatedTone.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

EGatedTonePtr	eGatedTonePtr = NULL;

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
Free_Stimulus_ExpGatedTone(void)
{
	if (eGatedTonePtr == NULL)
		return(FALSE);
	if (eGatedTonePtr->parList)
		FreeList_UniParMgr(&eGatedTonePtr->parList);
	if (eGatedTonePtr->parSpec == GLOBAL) {
		free(eGatedTonePtr);
		eGatedTonePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitTypeModeList ******************************/

/*
 * This routine intialises the Type Mode list array.
 */

BOOLN
InitTypeModeList_Stimulus_ExpGatedTone(void)
{
	static NameSpecifier	typeModeList[] = {

					{wxT("RAMPED"),	EGATED_TONE_RAMPED_MODE },
					{wxT("DAMPED"),	EGATED_TONE_DAMPED_MODE },
					{NULL,			EGATED_TONE_MODE_NULL }

				};
	eGatedTonePtr->typeModeList = typeModeList;
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
Init_Stimulus_ExpGatedTone(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Stimulus_ExpGatedTone");

	if (parSpec == GLOBAL) {
		if (eGatedTonePtr != NULL)
			Free_Stimulus_ExpGatedTone();
		if ((eGatedTonePtr = (EGatedTonePtr) malloc(sizeof(EGatedTone))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (eGatedTonePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	eGatedTonePtr->parSpec = parSpec;
	eGatedTonePtr->typeMode = EGATED_TONE_RAMPED_MODE;
	eGatedTonePtr->floorMode = GENERAL_BOOLEAN_OFF;
	eGatedTonePtr->carrierFrequency = 1000.0;
	eGatedTonePtr->amplitude = 10000.0;
	eGatedTonePtr->phase = 0.0;
	eGatedTonePtr->beginPeriodDuration = 0.0;
	eGatedTonePtr->repetitionRate = 40.0;
	eGatedTonePtr->halfLife = 5e-3;
	eGatedTonePtr->floor = 0.0;
	eGatedTonePtr->duration = 0.1;
	eGatedTonePtr->dt = 0.1e-3;

	InitTypeModeList_Stimulus_ExpGatedTone();
	if (!SetUniParList_Stimulus_ExpGatedTone()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Stimulus_ExpGatedTone();
		return(FALSE);
	}
	eGatedTonePtr->cycleTimer = 0.0;
	return(TRUE);

}


/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_Stimulus_ExpGatedTone(void)
{
	static const WChar *funcName = wxT("SetUniParList_Stimulus_ExpGatedTone");
	UniParPtr	pars;

	if ((eGatedTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  EGATED_TONE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_TYPEMODE], wxT("T_MODE"),
	  wxT("Type mode ('ramped' or 'damped')."),
	  UNIPAR_NAME_SPEC,
	  &eGatedTonePtr->typeMode, eGatedTonePtr->typeModeList,
	  (void * (*)) SetTypeMode_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_FLOORMODE], wxT("F_MODE"),
	  wxT("Floor mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &eGatedTonePtr->floorMode, NULL,
	  (void * (*)) SetFloorMode_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_CARRIERFREQUENCY], wxT("FREQUENCY"),
	  wxT("Carrier frequency (Hz)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->carrierFrequency, NULL,
	  (void * (*)) SetCarrierFrequency_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_AMPLITUDE], wxT("AMPLITUDE"),
	  wxT("Amplitude (uPa)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->amplitude, NULL,
	  (void * (*)) SetAmplitude_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_PHASE], wxT("PHASE"),
	  wxT("Phase (degrees)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->phase, NULL,
	  (void * (*)) SetPhase_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_BEGINPERIODDURATION], wxT("SILENCE"),
	  wxT("Begin period duration - silence (s)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->beginPeriodDuration, NULL,
	  (void * (*)) SetBeginPeriodDuration_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_REPETITIONRATE], wxT("R_RATE"),
	  wxT("Repetition rate (Hz)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->repetitionRate, NULL,
	  (void * (*)) SetRepetitionRate_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_HALFLIFE], wxT("HALF_LIFE"),
	  wxT("Half life (s)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->halfLife, NULL,
	  (void * (*)) SetHalfLife_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_FLOOR], wxT("FLOOR"),
	  wxT("Floor value (uPa)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->floor, NULL,
	  (void * (*)) SetFloor_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->duration, NULL,
	  (void * (*)) SetDuration_Stimulus_ExpGatedTone);
	SetPar_UniParMgr(&pars[EGATED_TONE_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &eGatedTonePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_Stimulus_ExpGatedTone);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Stimulus_ExpGatedTone(void)
{
	static const WChar *funcName = wxT(
	  "GetUniParListPtr_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (eGatedTonePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(eGatedTonePtr->parList);

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the module's typeMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTypeMode_Stimulus_ExpGatedTone(WChar *theTypeMode)
{
	static const WChar	*funcName = wxT("SetTypeMode_Stimulus_ExpGatedTone");
	int		specifier;

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTypeMode,
	  eGatedTonePtr->typeModeList)) == EGATED_TONE_MODE_NULL) {
		NotifyError(wxT("%s: Illegal type mode (%s)."), funcName,
		  theTypeMode);
		return(FALSE);
	}
	eGatedTonePtr->typeMode = specifier;
	return(TRUE);

}

/****************************** SetFloorMode **********************************/

/*
 * This function sets the module's floorMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFloorMode_Stimulus_ExpGatedTone(WChar *theFloorMode)
{
	static const WChar	*funcName = wxT("SetFloorMode_Stimulus_ExpGatedTone");
	int		specifier;

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theFloorMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  theFloorMode);
		return(FALSE);
	}
	eGatedTonePtr->floorMode = specifier;
	return(TRUE);

}

/****************************** SetCarrierFrequency ***************************/

/*
 * This function sets the module's carrierFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCarrierFrequency_Stimulus_ExpGatedTone(Float theCarrierFrequency)
{
	static const WChar	*funcName = wxT(
	  "SetCarrierFrequency_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->carrierFrequency = theCarrierFrequency;
	return(TRUE);

}

/****************************** SetAmplitude **********************************/

/*
 * This function sets the module's amplitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAmplitude_Stimulus_ExpGatedTone(Float theAmplitude)
{
	static const WChar	*funcName = wxT("SetAmplitude_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->amplitude = theAmplitude;
	return(TRUE);

}

/****************************** SetPhase **************************************/

/*
 * This function sets the module's phase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPhase_Stimulus_ExpGatedTone(Float thePhase)
{
	static const WChar	*funcName = wxT("SetPhase_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->phase = thePhase;
	return(TRUE);

}

/****************************** SetBeginPeriodDuration ************************/

/*
 * This function sets the module's beginPeriodDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBeginPeriodDuration_Stimulus_ExpGatedTone(Float theBeginPeriodDuration)
{
	static const WChar	*funcName =
	  wxT("SetBeginPeriodDuration_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->beginPeriodDuration = theBeginPeriodDuration;
	return(TRUE);

}

/****************************** SetRepetitionRate *****************************/

/*
 * This function sets the module's repetitionRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRepetitionRate_Stimulus_ExpGatedTone(Float theRepetitionRate)
{
	static const WChar	*funcName = wxT(
	  "SetRepetitionRate_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->repetitionRate = theRepetitionRate;
	return(TRUE);

}

/****************************** SetHalfLife ***********************************/

/*
 * This function sets the module's halfLife parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetHalfLife_Stimulus_ExpGatedTone(Float theHalfLife)
{
	static const WChar	*funcName = wxT("SetHalfLife_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->halfLife = theHalfLife;
	return(TRUE);

}

/****************************** SetFloor **************************************/

/*
 * This function sets the module's floor parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFloor_Stimulus_ExpGatedTone(Float theFloor)
{
	static const WChar	*funcName = wxT("SetFloor_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->floor = theFloor;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_Stimulus_ExpGatedTone(Float theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_Stimulus_ExpGatedTone(Float theSamplingInterval)
{
	static const WChar	*funcName = wxT(
	  "SetSamplingInterval_Stimulus_ExpGatedTone");

	if (eGatedTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Stimulus_ExpGatedTone(void)
{
	DPrint(wxT("Exponentially Gated Pure Tone Module Parameters:-\n"));
	DPrint(wxT("\tType mode = %s"), eGatedTonePtr->typeModeList[
	  eGatedTonePtr->typeMode].name);
	DPrint(wxT("\tCarrier frequency = %g Hz,\n"), eGatedTonePtr->
	  carrierFrequency);
	DPrint(wxT("\tAmplitude (maximum) = %g uPa,"), eGatedTonePtr->amplitude);
	DPrint(wxT("\tPhase = %g (degrees),\n"), eGatedTonePtr->phase);
	DPrint(wxT("\tRepetition rate = %g Hz,"), eGatedTonePtr->repetitionRate);
	DPrint(wxT("\tHalf Life = %g ms,"), MSEC(eGatedTonePtr->halfLife));
	DPrint(wxT("\tFloor mode: %s,\n"), BooleanList_NSpecLists(
	  eGatedTonePtr->floorMode)->name);
	if (eGatedTonePtr->floorMode == GENERAL_BOOLEAN_ON)
		DPrint(wxT("\tFloor = %g uPa,"), eGatedTonePtr->floor);
	DPrint(wxT("\tBegin period duration = %g ms\n"),
	  MSEC(eGatedTonePtr->beginPeriodDuration));
	DPrint(wxT("\tDuration = %g ms,"), MSEC(eGatedTonePtr->duration));
	DPrint(wxT("\tSampling interval = %g ms\n"), MSEC(eGatedTonePtr->dt));
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Stimulus_ExpGatedTone(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Stimulus_ExpGatedTone");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	eGatedTonePtr = (EGatedTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Stimulus_ExpGatedTone(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Stimulus_ExpGatedTone");

	if (!SetParsPointer_Stimulus_ExpGatedTone(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Stimulus_ExpGatedTone(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = eGatedTonePtr;
	theModule->Free = Free_Stimulus_ExpGatedTone;
	theModule->GetUniParListPtr = GetUniParListPtr_Stimulus_ExpGatedTone;
	theModule->PrintPars = PrintPars_Stimulus_ExpGatedTone;
	theModule->RunProcess = GenerateSignal_Stimulus_ExpGatedTone;
	theModule->SetParsPointer = SetParsPointer_Stimulus_ExpGatedTone;
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
GenerateSignal_Stimulus_ExpGatedTone(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("GenerateSignal_Stimulus_ExpGatedTone");
	register ChanData	 *outPtr;
	register Float		time, cycle, expFactor, phaseRads, cyclePeriod;
	ChanLen		i, t;
	EGatedTonePtr	p = eGatedTonePtr;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Exponentially Gated Pure Tone")
		  wxT(" Module Process"));
		if ( !InitOutSignal_EarObject(data, EGATED_TONE_NUM_CHANNELS,
		  (ChanLen) floor(p->duration / p->dt + 0.5), p->dt)) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	cyclePeriod = 1.0 / p->repetitionRate;
	expFactor = -LN_2 / p->halfLife;
	phaseRads = DEGREES_TO_RADS(p->phase);
	if (data->timeIndex == PROCESS_START_TIME) {
		p->cycleTimer = 0.0;
		p->nextCycle = cyclePeriod;
	}
	outPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->length;
	  i++, t++, outPtr++) {
		time = t * p->dt;
	  	if (time < p->beginPeriodDuration) {
			*outPtr = 0.0;
	  		continue;
		}
		cycle = (p->typeMode == EGATED_TONE_RAMPED_MODE)? cyclePeriod -
		  p->cycleTimer: p->cycleTimer;
		*outPtr = p->amplitude * sin(p->carrierFrequency * p->cycleTimer *
		  PIx2 + phaseRads) * exp(cycle * expFactor);
		if (p->floorMode && (*outPtr < p->floor))
			*outPtr = p->floor;
		if (time >= p->nextCycle) {
			p->cycleTimer = 0.0;
			p->nextCycle += cyclePeriod;
		} else
			p->cycleTimer += p->dt;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

