/**********************
 *
 * File:		MoHCRPCarney.c
 * Purpose:		Laurel H. Carney IHC receptor potential module: Carney L. H.
 *				(1993) "A model for the responses of low-frequency
 *				auditory-nerve fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				It expects to be used in conjuction with a BM module.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		15 Mar 1996
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
#include "MoHCRPCarney.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CarneyRPPtr	carneyRPPtr = NULL;

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
Free_IHCRP_Carney(void)
{
	if (carneyRPPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHCRP_Carney();
	if (carneyRPPtr->parList)
		FreeList_UniParMgr(&carneyRPPtr->parList);
	if (carneyRPPtr->parSpec == GLOBAL) {
		free(carneyRPPtr);
		carneyRPPtr = NULL;
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
Init_IHCRP_Carney(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IHCRP_Carney");

	if (parSpec == GLOBAL) {
		if (carneyRPPtr != NULL)
			Free_IHCRP_Carney();
		if ((carneyRPPtr = (CarneyRPPtr) malloc(sizeof(CarneyRP))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (carneyRPPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	carneyRPPtr->parSpec = parSpec;
	carneyRPPtr->updateProcessVariablesFlag = TRUE;
	carneyRPPtr->cutOffFrequency = 1100.0;
	carneyRPPtr->hCOperatingPoint = 1000.0;
	carneyRPPtr->asymmetricalBias = 0.462;
	carneyRPPtr->maxHCVoltage = 10.0;
	carneyRPPtr->referencePot = 0.0;
	carneyRPPtr->waveDelayCoeff = 0.00813;
	carneyRPPtr->waveDelayLength = 0.00649;

	if (!SetUniParList_IHCRP_Carney()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHCRP_Carney();
		return(FALSE);
	}
	carneyRPPtr->numChannels = 0;
	carneyRPPtr->coefficients = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHCRP_Carney(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHCRP_Carney");
	UniParPtr	pars;

	if ((carneyRPPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_CARNEY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = carneyRPPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_CARNEY_CUTOFFFREQUENCY], wxT("FC"),
	  wxT("Cut-off frequency for OHC low-pass filter, Fc (Hz)."),
	  UNIPAR_REAL,
	  &carneyRPPtr->cutOffFrequency, NULL,
	  (void * (*)) SetCutOffFrequency_IHCRP_Carney);
	SetPar_UniParMgr(&pars[IHCRP_CARNEY_HCOPERATINGPOINT], wxT("P_D_IHC"),
	  wxT("Operating point of OHC (feedback) non-linearity, P_Dihc (uPa)."),
	  UNIPAR_REAL,
	  &carneyRPPtr->hCOperatingPoint, NULL,
	  (void * (*)) SetHCOperatingPoint_IHCRP_Carney);
	SetPar_UniParMgr(&pars[IHCRP_CARNEY_ASYMMETRICALBIAS], wxT("P0"),
	  wxT("Asymmetrical bias for OHC non-linearity, P0 (rad)"),
	  UNIPAR_REAL,
	  &carneyRPPtr->asymmetricalBias, NULL,
	  (void * (*)) SetAsymmetricalBias_IHCRP_Carney);
	SetPar_UniParMgr(&pars[IHCRP_CARNEY_MAXHCVOLTAGE], wxT("V_MAX"),
	  wxT("Maximum depolarising hair cell voltage, Vmax (V)."),
	  UNIPAR_REAL,
	  &carneyRPPtr->maxHCVoltage, NULL,
	  (void * (*)) SetMaxHCVoltage_IHCRP_Carney);
	SetPar_UniParMgr(&pars[IHCRP_CARNEY_WAVEDELAYCOEFF], wxT("A_D"),
	  wxT("Travelling wave delay coefficient, A_D (s)."),
	  UNIPAR_REAL,
	  &carneyRPPtr->waveDelayCoeff, NULL,
	  (void * (*)) SetWaveDelayCoeff_IHCRP_Carney);
	SetPar_UniParMgr(&pars[IHCRP_CARNEY_WAVEDELAYLENGTH], wxT("A_L"),
	  wxT("Travelling wave delay length constant, A_L (m)."),
	  UNIPAR_REAL,
	  &carneyRPPtr->waveDelayLength, NULL,
	  (void * (*)) SetWaveDelayLength_IHCRP_Carney);
	SetPar_UniParMgr(&pars[IHCRP_CARNEY_REFERENCEPOT], wxT("V_REF"),
	  wxT("Reference potential (V)."),
	  UNIPAR_REAL,
	  &carneyRPPtr->referencePot, NULL,
	  (void * (*)) SetReferencePot_IHCRP_Carney);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHCRP_Carney(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHCRP_Carney");

	if (carneyRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (carneyRPPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(carneyRPPtr->parList);

}

/****************************** SetCutOffFrequency ****************************/

/*
 * This function sets the module's cutOffFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCutOffFrequency_IHCRP_Carney(Float theCutOffFrequency)
{
	static const WChar	*funcName = wxT("SetCutOffFrequency_IHCRP_Carney");

	if (carneyRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyRPPtr->cutOffFrequency = theCutOffFrequency;
	return(TRUE);

}

/****************************** SetHCOperatingPoint ***************************/

/*
 * This function sets the module's HCOperatingPoint parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetHCOperatingPoint_IHCRP_Carney(Float theHCOperatingPoint)
{
	static const WChar	*funcName = wxT("SetHCOperatingPoint_IHCRP_Carney");

	if (carneyRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyRPPtr->hCOperatingPoint = theHCOperatingPoint;
	return(TRUE);

}

/****************************** SetAsymmetricalBias ***************************/

/*
 * This function sets the module's asymmetricalBias parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAsymmetricalBias_IHCRP_Carney(Float theAsymmetricalBias)
{
	static const WChar	*funcName = wxT("SetAsymmetricalBias_IHCRP_Carney");

	if (carneyRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyRPPtr->asymmetricalBias = theAsymmetricalBias;
	return(TRUE);

}

/****************************** SetMaxHCVoltage ******************************/

/*
 * This function sets the module's maxHCVoltage parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxHCVoltage_IHCRP_Carney(Float theMaxHCVoltage)
{
	static const WChar	*funcName = wxT("SetMaxHCVoltage_IHCRP_Carney");

	if (carneyRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyRPPtr->maxHCVoltage = theMaxHCVoltage;
	return(TRUE);

}

/********************************* SetReferencePot ****************************/

/*
 * This function sets the module's reference Potential, Eref, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReferencePot_IHCRP_Carney(Float theReferencePot)
{
	static const WChar	 *funcName = wxT("SetCiliaTimeConstTc_IHCRP_Carney");
	if (carneyRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	carneyRPPtr->referencePot = theReferencePot;
	carneyRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetWaveDelayCoeff *****************************/

/*
 * This function sets the module's waveDelayCoeff parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetWaveDelayCoeff_IHCRP_Carney(Float theWaveDelayCoeff)
{
	static const WChar	*funcName = wxT("SetWaveDelayCoeff_IHCRP_Carney");

	if (carneyRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyRPPtr->waveDelayCoeff = theWaveDelayCoeff;
	return(TRUE);

}

/****************************** SetWaveDelayLength ****************************/

/*
 * This function sets the module's waveDelayLength parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetWaveDelayLength_IHCRP_Carney(Float theWaveDelayLength)
{
	static const WChar	*funcName = wxT("SetWaveDelayLength_IHCRP_Carney");

	if (carneyRPPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyRPPtr->waveDelayLength = theWaveDelayLength;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_IHCRP_Carney(void)
{
	DPrint(wxT("Carney IHC Receptor Rotential Module Parameters:-\n"));
	DPrint(wxT("\tLow-pass cut-off frequency, Fc = %g (Hz),\n"),
	  carneyRPPtr->cutOffFrequency);
	DPrint(wxT("\tIHC operating point, P_Dihc = %g (uPa),\n"),
	  carneyRPPtr->hCOperatingPoint);
	DPrint(wxT("\tAsymmetrical bias (OHC non-linearity), P0 = %g (rad.)\n"),
	  carneyRPPtr->asymmetricalBias);
	DPrint(wxT("\tMax, HC voltage, Vmax = %g V,\n"), carneyRPPtr->maxHCVoltage);
	DPrint(wxT("\tWave delay coefficient, A_D = %g (ms),\n"), MSEC(carneyRPPtr->
	  waveDelayCoeff));
	DPrint(wxT("\tWave delay length, A_L = %g (mm).\n"), carneyRPPtr->
	  waveDelayLength * 1.0e3);
	DPrint(wxT("\tReference potential = %g V\n"), carneyRPPtr->referencePot);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHCRP_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHCRP_Carney");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	carneyRPPtr = (CarneyRPPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHCRP_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHCRP_Carney");

	if (!SetParsPointer_IHCRP_Carney(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_Carney(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = carneyRPPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_IHCRP_Carney;
	theModule->GetUniParListPtr = GetUniParListPtr_IHCRP_Carney;
	theModule->PrintPars = PrintPars_IHCRP_Carney;
	theModule->RunProcess = RunModel_IHCRP_Carney;
	theModule->SetParsPointer = SetParsPointer_IHCRP_Carney;
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
CheckData_IHCRP_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IHCRP_Carney");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_IHCRP_Carney(void)
{
	int		i;

	if (carneyRPPtr->coefficients) {
		for (i = 0; i < carneyRPPtr->numChannels; i++)
    		FreeCarneyRPCoeffs_IHCRP_Carney(&carneyRPPtr->coefficients[i]);
		free(carneyRPPtr->coefficients);
		carneyRPPtr->coefficients = NULL;
	}
	carneyRPPtr->updateProcessVariablesFlag = TRUE;
	return(FALSE);

}

/**************************** FreeCarneyRPCoeffs ******************************/

/*
 * This routine frees the space allocated for the coefficients.
 */

void
FreeCarneyRPCoeffs_IHCRP_Carney(CarneyRPCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	if ((*p)->lastOutputSection)
		free((*p)->lastOutputSection);
	if ((*p)->lastOutputStore)
		free((*p)->lastOutputStore);
	free(*p);
	*p = NULL;

}

/**************************** InitCarneyRPCoeffs ******************************/

/*
 * This routine initialises the Carney IHC receptor potential coefficients,
 * and prepares space for the appropriate arrays.
 * Parameter 'x' is the position of cf unit; from Liberman's map,
 * (cf. UtBandwidth.h).
 * At least one sample from the last signal process must be saved.
 */

CarneyRPCoeffsPtr
InitCarneyRPCoeffs_IHCRP_Carney(Float cF, Float dt)
{
	static const WChar *funcName = wxT("InitCarneyRPCoeffs_IHCRP_Carney");
	Float	delay;
	CarneyRPCoeffsPtr p;

	if ((p = (CarneyRPCoeffsPtr) malloc(sizeof(CarneyRPCoeffs))) == NULL) {
		NotifyError(wxT("%s: Out of memory for coefficient."), funcName);
		return(NULL);
	}
	p->x = BANDWIDTH_CARNEY_X(cF);
	delay = carneyRPPtr->waveDelayCoeff * exp(-p->x /
	  carneyRPPtr->waveDelayLength) - 1.0 / cF;
	p->numLastSamples = (delay < dt)? 1: (ChanLen) (delay / dt);
	if ((p->lastOutputSection = (ChanData *) calloc(p->numLastSamples,
	  sizeof(ChanData))) == NULL) {
		NotifyError(wxT("%s: Out of memory for 'lastOutputSection' array (%u ")
		  wxT("elements)."), funcName, p->numLastSamples);
		FreeCarneyRPCoeffs_IHCRP_Carney(&p);
		return(NULL);
	}
	if ((p->lastOutputStore = (ChanData *) calloc(p->numLastSamples,
	  sizeof(ChanData))) == NULL) {
		NotifyError(wxT("%s: Out of memory for 'lastOutputStore' array (%u ")
		  wxT("elements)."), funcName, p->numLastSamples);
		FreeCarneyRPCoeffs_IHCRP_Carney(&p);
		return(NULL);
	}
	p->waveLast = 0.0;
	return(p);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_IHCRP_Carney(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_IHCRP_Carney");
	int		i, cFIndex;
	Float	*ptr1, *ptr2, dt = _OutSig_EarObject(data)->dt;
	ChanLen	j;
	CarneyRPPtr	p = carneyRPPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_IHCRP_Carney();
			p->numChannels = _OutSig_EarObject(data)->numChannels;
			if ((p->coefficients = (CarneyRPCoeffsPtr *) calloc(
			  p->numChannels, sizeof(CarneyRPCoeffs))) == NULL) {
		 		NotifyError(wxT("%s: Out of memory for coefficients array."),
		 		  funcName);
		 		return(FALSE);
			}
			SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
			CopyInfo_SignalData(_OutSig_EarObject(data), _InSig_EarObject(data, 0));
			for (i = 0; i < p->numChannels; i++) {
				cFIndex = i / _OutSig_EarObject(data)->interleaveLevel;
				if ((p->coefficients[i] = InitCarneyRPCoeffs_IHCRP_Carney(
				  _OutSig_EarObject(data)->info.cFArray[cFIndex], dt)) == NULL) {
					NotifyError(wxT("%s: Out of memory for coefficient (%d)."),
					  funcName, i);
					FreeProcessVariables_IHCRP_Carney();
					return(FALSE);
				}
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			cFIndex = i / _OutSig_EarObject(data)->interleaveLevel;
			p->coefficients[i]->x = BANDWIDTH_CARNEY_X(
			  _OutSig_EarObject(data)->info.cFArray[cFIndex]);
			ptr1 = p->coefficients[i]->lastOutputSection;
			ptr2 = p->coefficients[i]->lastOutputStore;
			for (j = 0; j < p->coefficients[i]->numLastSamples; j++)
				*ptr1++ = *ptr2++ = 0.0;
			p->coefficients[i]->waveLast = 0.0;
			p->coefficients[i]->waveTempLast = 0.0;
			p->coefficients[i]->iHCLast = 0.0;
			p->coefficients[i]->iHCTempLast = 0.0;
		}

	}
	for (i = 0; i < carneyRPPtr->numChannels; i++)
		if (carneyRPPtr->coefficients[i]->numLastSamples >=
		  _OutSig_EarObject(data)->length) {
			NotifyError(wxT("%s: Signal too short for wave delay (%g ms)."),
			  funcName, carneyRPPtr->coefficients[i]->numLastSamples * dt);
			FreeProcessVariables_IHCRP_Carney();
			return(FALSE);
		}
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
 * The reference potential, is not remembered for previous runs, and as added
 * after all processing has been done, because otherwise it causes problems
 * with the low-pass filtering effect.
 */

BOOLN
RunModel_IHCRP_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_IHCRP_Carney");
	register	ChanData	 *inPtr, *outPtr, waveNow, iHCTemp;
	register	ChanData	 temp;
	int		chan;
	Float	c;
	ChanLen	i;
	CarneyRPPtr	p = carneyRPPtr;
	SignalDataPtr	outSignal;
	CarneyRPCoeffsPtr	cC;

	if (!data->threadRunFlag) {
		if (!CheckData_IHCRP_Carney(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Carney hair cell receptor ")
		  wxT("potential"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_IHCRP_Carney(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		/* Main Processing. */
		c = 2.0 / _InSig_EarObject(data, 0)->dt;
		p->aA = p->maxHCVoltage / (1.0 + tanh(p->asymmetricalBias));
		p->c1LP = (c - PIx2 * p->cutOffFrequency) / (c + PIx2 *
		  p->cutOffFrequency);
		p->c2LP = PIx2 * p->cutOffFrequency / (PIx2 * p->cutOffFrequency + c);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		cC = *(p->coefficients + chan);
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		for(i = 0; i < outSignal->length; i++, outPtr++) {
			temp = 0.746 * *inPtr++ / p->hCOperatingPoint;
			waveNow = p->aA * (tanh(temp - p->asymmetricalBias) +
			  tanh(p->asymmetricalBias));
			*outPtr = p->c1LP * cC->waveTempLast + p->c2LP * (waveNow +
			  cC->waveLast);
			cC->waveTempLast = *outPtr;
			cC->waveLast = waveNow;
		}
		/* lowpass filter the IHC voltage once more  */
 		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++, outPtr++) {
			iHCTemp = *outPtr;
			*outPtr = p->c1LP * cC->iHCLast + p->c2LP * (*outPtr +
			  cC->iHCTempLast);
			cC->iHCTempLast = iHCTemp;
			cC->iHCLast = *outPtr;
		}
		/* DELAY THE WAVEFORM */
		inPtr = outSignal->channel[chan] + outSignal->length - cC->
		  numLastSamples;
		outPtr = cC->lastOutputStore;
		for(i = 0; i < cC->numLastSamples; i++)
			*outPtr++ = *inPtr++;

		outPtr = outSignal->channel[chan] + outSignal->length - 1;
		for(i = cC->numLastSamples; i < outSignal->length; i++, outPtr--)
			*outPtr = *(outPtr - cC->numLastSamples);

		inPtr = cC->lastOutputSection;
		outPtr = outSignal->channel[chan];
		for(i = 0; i < cC->numLastSamples; i++)
			*outPtr++ = *inPtr++;

		inPtr = cC->lastOutputStore;
		outPtr = cC->lastOutputSection;
		for(i = 0; i < cC->numLastSamples; i++)
			*outPtr++ = *inPtr++;

		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++)
			*outPtr++ += carneyRPPtr->referencePot;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

