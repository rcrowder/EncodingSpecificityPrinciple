/**********************
 *
 * File:		UtIonChanList.c
 * Purpose:		This module generates the ion channel tables list, reading the
 *				tables from file or generating them as required.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 13 1996).
 *				13-01-97: LPO: Introduce HHuxley Channel list mode.
 *				26-11-97 LPO: Using new GeNSpecLists module for 'mode'
 *				parameter.
 *				18-11-98 LPO: Introduced the 'SetParentICList_IonChanList' and
 *				the 'parentPtr' so that a ICList can always set the pointer
 *				which is pointing to it.
 *				08-12-98 LPO: Separated the IC channel generation routines into
 *				GetPars... and Generate... routines.
 *				14-12-98 LPO: Converted the 'InitTable_' so that it initialises
 *				the table for a single channel.  This means that it can be used
 *				by the 'ReadIonChannel_' routine as well as the others.
 *				03-02-99 LPO: Introduced the 'mode' field for the 'IonChannel'
 *				structure. This allows the 'SetICBaseMaxConducance' routine
 *				called from the GUI to set the correct maxConductance.  I also
 *				had to add the 'temperature' field too.
 *				Changed the 'InitTable_' routine to 'SetIonChannel'.
 * Author:		L. P. O'Mard
 * Created:		13 Oct 1996
 * Updated:		03 Feb 1999
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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
#include <string.h>
#include <float.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeNSpecLists.h"
#include "GeUniParMgr.h"
#include "FiParFile.h"
#include "UtNameSpecs.h"
#include "UtDynaList.h"
#include "UtString.h"
#include "UtIonChanList.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

NameSpecifier iCListModeList[] = {

		{ wxT("BOLTZMANN"),	ICLIST_BOLTZMANN_MODE },
		{ wxT("FILE"),		ICLIST_FILE_MODE },
		{ wxT("HHUXLEY"),	ICLIST_HHUXLEY_MODE },
		{ wxT("ROTHMAN"),	ICLIST_ROTHMAN_MODE },
		{ 0,				ICLIST_NULL }

	};

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** FreeIonChannel ********************************/

/*
 * This function releases of the memory allocated for an ion channels members.
 * The structure itself was not allocated, so it does not require deallocation.
 */

void
FreeIonChannel_IonChanList(IonChannelPtr *theIC)
{
	if (!*theIC)
		return;
	free((*theIC)->table);
	FreeList_UniParMgr(&(*theIC)->parList);
	free(*theIC);
	*theIC = NULL;

}

/****************************** FreeIonChannels *******************************/

/*
 * This function releases the memory allocated for all ion channels.
 */

void
FreeIonChannels_IonChanList(IonChanListPtr theICList)
{
	IonChannelPtr theIC;

	if (!theICList)
		return;
	while (theICList->ionChannels) {
		theIC = (IonChannelPtr) Pull_Utility_DynaList(&theICList->ionChannels);
		FreeIonChannel_IonChanList(&theIC);
	}

}

/****************************** Free ******************************************/

/*
 * This function releases the memory allocated for the ion channel list.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_IonChanList(IonChanListPtr *theICList)
{
	if (*theICList == NULL)
		return (FALSE);
	FreeIonChannels_IonChanList(*theICList);
	if ((*theICList)->printTablesModeList)
		free((*theICList)->printTablesModeList);
	FreeList_UniParMgr(&(*theICList)->parList);
	free(*theICList);
	*theICList = NULL;
	return (TRUE);

}

/****************************** Init ******************************************/

/*
 * This function allocates memory for the ion channel list structure.
 * The function returns a pointer to the prepared structure.
 */

IonChanListPtr
Init_IonChanList(const WChar *callingFunctionName)
{
	static const WChar *funcName = wxT("Init_IonChanList");
	IonChanListPtr theICList;

	if ((theICList = (IonChanListPtr) malloc(sizeof(IonChanList))) == NULL) {
		NotifyError(wxT("%s: Out of memory (called by %s)."), funcName,
		  callingFunctionName);
		return (NULL);
	}
	theICList->updateFlag = TRUE;
	theICList->useTemperatureCalcFlag = TRUE;
	theICList->numChannels = 0;
	theICList->ionChannels = NULL;
	theICList->printTablesMode = 0;
	theICList->minVoltage = 0.0;
	theICList->maxVoltage = 0.0;
	theICList->dV = 0.0;
	DSAM_strcpy(theICList->diagFileName, DEFAULT_FILE_NAME);

	if ((theICList->printTablesModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), theICList->diagFileName)) == NULL) {
		free(theICList);
		return (NULL);
	}
	theICList->parList = NULL;
	theICList->currentIC = NULL;
	theICList->oldNumChannels = 0;
	theICList->oldNumTableEntries = 0;
	return (theICList);

}

/****************************** InitICBoltzmannPars ***************************/

/*
 * This routine initialises the Boltzmann parameters.
 * This is a structural initialisation for setting up the pointers to the
 * gate arrays, so that they can be passed to the universal parameter list
 */

BOOLN
InitICBoltzmannPars_IonChanList(ICBoltzmannParsPtr p)
{
	static const WChar *funcName = wxT("InitICBoltzmannPars_IonChanList");

	if (p == NULL) {
		NotifyError(wxT("%s: Pointer not initialised."), funcName);
		return (FALSE);
	}
	SET_IC_GATE_ARRAY(p->halfMaxV);
	SET_IC_GATE_ARRAY(p->zZ);
	SET_IC_GATE_ARRAY(p->tau);
	return (TRUE);

}

/****************************** InitICHHuxleyPars *******************************/

/*
 * This routine initialises the new HH parameter formulism.
 * This is a structural initialisation for setting up the pointers to the
 * gate arrays, so that they can be passed to the universal parameter list
 */

BOOLN
InitICHHuxleyPars_IonChanList(ICHHuxleyParsPtr p)
{
	static const WChar *funcName = wxT("InitHHuxleyPars_IonChanList");

	if (p == NULL) {
		NotifyError(wxT("%s: Pointer not initialised."), funcName);
		return (FALSE);
	}
	SET_IC_GATE_ARRAY(p->func1A);
	SET_IC_GATE_ARRAY(p->func1B);
	SET_IC_GATE_ARRAY(p->func1C);
	SET_IC_GATE_ARRAY(p->func1D);
	SET_IC_GATE_ARRAY(p->func1E);
	SET_IC_GATE_ARRAY(p->func1F);
	SET_IC_GATE_ARRAY(p->func1G);
	SET_IC_GATE_ARRAY(p->func1H);
	SET_IC_GATE_ARRAY(p->func1I);
	SET_IC_GATE_ARRAY(p->func1J);
	SET_IC_GATE_ARRAY(p->func1K);
	SET_IC_GATE_ARRAY(p->func2A);
	SET_IC_GATE_ARRAY(p->func2B);
	SET_IC_GATE_ARRAY(p->func2C);
	SET_IC_GATE_ARRAY(p->func2D);
	SET_IC_GATE_ARRAY(p->func2E);
	SET_IC_GATE_ARRAY(p->func2F);
	SET_IC_GATE_ARRAY(p->func2G);
	SET_IC_GATE_ARRAY(p->func2H);
	SET_IC_GATE_ARRAY(p->func2I);
	SET_IC_GATE_ARRAY(p->func2J);
	SET_IC_GATE_ARRAY(p->func2K);
	return (TRUE);

}

/****************************** ActivationModeList *****************************/

/*
 * This function initialises the 'bandwidthMode' list array
 */

NameSpecifier *
ActivationModeList_IonChanList(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("STANDARD"),		ICLIST_ACTIVATION_MODE_STANDARD },
			{ wxT("ROTHMAN_KHT"),	ICLIST_ACTIVATION_MODE_ROTHMAN_KHT },
			{ wxT("ONE_GATE"),		ICLIST_ACTIVATION_MODE_1_GATE },
			{ wxT("THREE_GATE"),	ICLIST_ACTIVATION_MODE_3_GATE },
			{ 0,					ICLIST_ACTIVATION_MODE_NULL }, };
	return (&modeList[index]);

}

/****************************** InitIonChannel ********************************/

/*
 * This function allocates memory for the ion channel structure.
 * The function returns a pointer to the prepared structure.
 * The 'calloc' function is used so that all the values are set to zero.
 */

IonChannelPtr
InitIonChannel_IonChanList(const WChar *callingFunctionName,
 int numTableEntries)
{
	static const WChar *funcName = wxT("InitIonChannel_IonChanList");
	IonChannelPtr theIC;

	if ((theIC = (IonChannelPtr) calloc(1, sizeof(IonChannel))) == NULL) {
		NotifyError(wxT("%s: Out of memory (called by %s)."), funcName,
		  callingFunctionName);
		return (NULL);
	}
	theIC->updateFlag = TRUE;
	theIC->mode = ICLIST_NULL;
	theIC->enabled = GENERAL_BOOLEAN_ON;
	theIC->description[0] = '\0';
	theIC->numTableEntries = numTableEntries;
	theIC->activationMode = ICLIST_ACTIVATION_MODE_STANDARD;
	theIC->activationExponent = 0.0;
	theIC->equilibriumPot = 0.0;
	theIC->temperature = 0.0;
	theIC->baseMaxConductance = 0.0;
	theIC->maxConductance = 0.0;
	theIC->conductanceQ10 = 0.0;
	theIC->minVoltage = 0.0;
	theIC->maxVoltage = 0.0;
	theIC->dV = 0.0;
	theIC->numGates = 0;
	InitICBoltzmannPars_IonChanList(&theIC->boltzmann);
	InitICHHuxleyPars_IonChanList(&theIC->hHuxley);
	DSAM_strcpy(theIC->fileName, DEFAULT_FILE_NAME);
	theIC->PowFunc = DSAM_POW;
	theIC->parList = NULL;
	if ((theIC->table = (ICTableEntry *) calloc(theIC->numTableEntries,
	  sizeof(ICTableEntry))) == NULL) {
		NotifyError(wxT("%s: Out of memory for table entries (%d)."), funcName,
		  theIC->numTableEntries);
		FreeIonChannel_IonChanList(&theIC);
		return (NULL);
	}
	return (theIC);

}

/********************************* SetGeneralUniParListMode *******************/

/*
 * This routine enables and disables the respective parameters for each ICList
 * mode.
 * It assumes that the parameter list has been correctly initialised.
 */

BOOLN
SetGeneralUniParListMode_IonChanList(IonChanListPtr theICs)
{
	static const WChar *funcName = wxT("SetGeneralUniParListMode_IonChanList");
	int i;

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	for (i = ICLIST_NUM_CONSTANT_PARS; i < CFLIST_NUM_PARS; i++)
		theICs->parList->pars[i].enabled = FALSE;
	if (theICs->useTemperatureCalcFlag) {
		theICs->parList->pars[ICLIST_TEMPERATURE].enabled = TRUE;
		theICs->parList->pars[ICLIST_LEAKAGE_COND_Q10].enabled = TRUE;
	}
	return (TRUE);

}

/********************************* SetGeneralUniParList ***********************/

/*
 * This routine initialises and sets the IonChanList's universal parameter list.
 * This list provides universal access to the IonChanList's parameters.
 */

BOOLN
SetGeneralUniParList_IonChanList(IonChanListPtr theICs)
{
	static const WChar *funcName = wxT("SetGeneralUniParList_IonChanList");
	UniParPtr pars;

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	FreeList_UniParMgr(&theICs->parList);
	if ((theICs->parList = InitList_UniParMgr(UNIPAR_SET_ICLIST,
	  ICLIST_NUM_PARS, theICs)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return (FALSE);
	}
	pars = theICs->parList->pars;
	SetPar_UniParMgr(&pars[ICLIST_PRINTTABLESMODE], wxT("TABLES_MODE"), wxT(
	  "Print ion channel tables mode ('off', 'screen' or filename)."),
	  UNIPAR_NAME_SPEC_WITH_FILE, &theICs->printTablesMode,
	  theICs->printTablesModeList,
	  (void *(*)) SetPrintTablesMode_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_BASELEAKAGECOND], wxT("LEAKAGE_COND"), wxT(
	  "Base leakage conductance (S)."), UNIPAR_REAL,
	  &theICs->baseLeakageCond, NULL,
	  (void *(*)) SetBaseLeakageCond_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_LEAKAGE_COND_Q10], wxT("LEAKAGE_C_Q10"),
	  wxT("Leakage conductance Q10."), UNIPAR_REAL,
	  &theICs->leakageCondQ10, NULL,
	  (void *(*)) SetLeakageCondQ10_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_LEAKAGEPOT], wxT("LEAKAGE_POT"), wxT(
	  "Leakage equilibrium potential (V)."), UNIPAR_REAL,
	  &theICs->leakagePot, NULL, (void *(*)) SetLeakagePot_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_NUMCHANNELS], wxT("NUM_CHANNELS"), wxT(
	  "Number of ion channels."), UNIPAR_INT, &theICs->numChannels, NULL,
	  (void *(*)) SetNumChannels_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_MIN_VOLT], wxT("MIN_VOLT"), wxT(
	  "Minimum voltage for tables (V)."), UNIPAR_REAL,
	  &theICs->minVoltage, NULL, (void *(*)) SetMinVoltage_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_MAX_VOLT], wxT("MAX_VOLT"), wxT(
	  "Maximum voltage for tables (V)."), UNIPAR_REAL,
	  &theICs->maxVoltage, NULL, (void *(*)) SetMaxVoltage_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_DV], wxT("DV"), wxT(
	  "Voltage step for tables (V)."), UNIPAR_REAL, &theICs->dV, NULL,
	  (void *(*)) SetVoltageStep_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_TEMPERATURE], wxT("TEMPERATURE"), wxT(
	  "Operating temperature (degrees C)."), UNIPAR_REAL,
	  &theICs->temperature, NULL, (void *(*)) SetTemperature_IonChanList);
	return (TRUE);

}

/********************************* SetIonChannelUniParListMode ****************/

/*
 * This routine enables and disables the respective parameters for each
 * Ion channel mode.
 * It assumes that the parameter list has been correctly initialised.
 */

BOOLN
SetIonChannelUniParListMode_IonChanList(IonChannelPtr theIC)
{
	static const WChar *funcName = wxT(
	  "SetIonChannelUniParListMode_IonChanList");
	int i;

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	for (i = ICLIST_IC_NUM_GATES + 1; i < ICLIST_IC_NUM_PARS; i++)
		theIC->parList->pars[i].enabled = FALSE;
	switch (theIC->mode) {
	case ICLIST_BOLTZMANN_MODE:
		theIC->parList->pars[ICLIST_IC_CONDUCTANCE_Q10].enabled = TRUE;
		theIC->parList->pars[ICLIST_IC_V_HALF].enabled = TRUE;
		theIC->parList->pars[ICLIST_IC_Z].enabled = TRUE;
		theIC->parList->pars[ICLIST_IC_TAU].enabled = TRUE;
		break;
	case ICLIST_FILE_MODE:
		theIC->parList->pars[ICLIST_IC_CONDUCTANCE_Q10].enabled = TRUE;
		theIC->parList->pars[ICLIST_IC_FILE_NAME].enabled = TRUE;
		break;
	case ICLIST_HHUXLEY_MODE:
	case ICLIST_ROTHMAN_MODE:
		theIC->parList->pars[ICLIST_IC_CONDUCTANCE_Q10].enabled = TRUE;
		for (i = ICLIST_IC_FUNC1_A; i <= ICLIST_IC_FUNC2_K; i++)
			theIC->parList->pars[i].enabled = TRUE;
		break;
	default:
		NotifyError(wxT("%s: Unknown ion channel mode (%d).\n"), funcName,
		  theIC->mode);
		return (FALSE);
	}
	return (TRUE);

}

/********************************* SetIonChannelUniParList ********************/

/*
 * This routine initialises and sets the IonChannel's universal parameter list.
 * This list provides universal access to the IonChanList's parameters.
 */

BOOLN
SetIonChannelUniParList_IonChanList(IonChanListPtr theICs,
		IonChannelPtr theIC)
{
	static const WChar *funcName = wxT("SetIonChannelUniParList_IonChanList");
	UniParPtr pars;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return (FALSE);
	FreeList_UniParMgr(&theIC->parList);
	if ((theIC->parList = InitList_UniParMgr(UNIPAR_SET_IC, ICLIST_IC_NUM_PARS,
	  theICs)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return (FALSE);
	}
	pars = theIC->parList->pars;
	SetPar_UniParMgr(&pars[ICLIST_IC_DESCRIPTION], wxT("DESCRIPTION"), wxT(
	  "Description."), UNIPAR_STRING, &theIC->description, NULL,
	  (void *(*)) SetICDescription_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_MODE], wxT("MODE"), wxT(
	  "Mode option ('file', 'hHuxley' or 'boltzmann')."),
	  UNIPAR_NAME_SPEC, &theIC->mode, iCListModeList,
	  (void *(*)) SetICMode_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ENABLED], wxT("ENABLED"), wxT(
	  "Ion channel enabled status ('on' or 'off)."), UNIPAR_BOOL,
	  &theIC->enabled, NULL, (void *(*)) SetICEnabled_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_EQUILIBRIUM_POT], wxT("EQUIL_POT"), wxT(
	  "Equilibrium potential (V)."), UNIPAR_REAL, &theIC->equilibriumPot,
	  NULL, (void *(*)) SetICEquilibriumPot_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_CONDUCTANCE], wxT("BASE_MAX_COND"), wxT(
	  "Base maximum conductance (S)."), UNIPAR_REAL,
	  &theIC->baseMaxConductance, NULL,
	  (void *(*)) SetICBaseMaxConductance_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_CONDUCTANCE_Q10], wxT("COND_Q10"), wxT(
	  "Conductance Q10."), UNIPAR_REAL, &theIC->conductanceQ10, NULL,
	  (void *(*)) SetICConductanceQ10_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ACTIVATION_EXPONENT], wxT("ACTIVATION"),
	  wxT("Activation exponent (real)."), UNIPAR_REAL,
	  &theIC->activationExponent, NULL,
	  (void *(*)) SetICActivationExponent_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_NUM_GATES], wxT("NUM_GATES"), wxT(
	  "Number of channel gates (int)."), UNIPAR_INT, &theIC->numGates,
	  NULL, (void *(*)) SetICNumGates_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_V_HALF], wxT("V_HALF"), wxT(
	  "Voltage at half maximum values (V)"), UNIPAR_REAL_ARRAY,
	  &theIC->boltzmann.halfMaxV.ptr, &theIC->numGates,
	  (void *(*)) SetICBoltzmannHalfMaxV_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_Z], wxT("Z"), wxT(
	  "'Z' Constant values (unit)."), UNIPAR_REAL_ARRAY,
	  &theIC->boltzmann.zZ.ptr, &theIC->numGates,
	  (void *(*)) SetICBoltzmannZ_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_TAU], wxT("TAU"), wxT(
	  "Time constants, tau (s)"), UNIPAR_REAL_ARRAY,
	  &theIC->boltzmann.tau.ptr, &theIC->numGates,
	  (void *(*)) SetICBoltzmannTau_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FILE_NAME], wxT("FILENAME"), wxT(
	  "Ion channel file name."), UNIPAR_FILE_NAME, &theIC->fileName,
	  (WChar *) wxT("*.par"), (void *(*)) SetICFileName_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_ACIVATION_MODE], wxT("ACTIVATION_MODE"),
	  wxT("Sets the ion channel activation function ('standard' or 'rothman_kht')."),
	  UNIPAR_NAME_SPEC, &theIC->activationMode,
	  ActivationModeList_IonChanList(0),
	  (void *(*)) SetICActivationMode_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_A], wxT("FUNC1_A"),
	  wxT("'a' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1A.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1A_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_B], wxT("FUNC1_B"),
	  wxT("'b' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1B.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1B_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_C], wxT("FUNC1_C"),
	  wxT("'c' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1C.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1C_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_D], wxT("FUNC1_D"),
	  wxT("'d' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1D.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1D_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_E], wxT("FUNC1_E"),
	  wxT("'e' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1E.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1E_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_F], wxT("FUNC1_F"),
	  wxT("'f' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1F.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1F_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_G], wxT("FUNC1_G"),
	  wxT("'g' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1G.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1G_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_H], wxT("FUNC1_H"),
	  wxT("'h' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1H.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1H_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_I], wxT("FUNC1_I"),
	  wxT("'i' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1I.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1I_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_J], wxT("FUNC1_J"),
	  wxT("'j' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1J.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1J_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC1_K], wxT("FUNC1_K"),
	  wxT("'k' parameter values for first activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func1K.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc1K_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_A], wxT("FUNC2_A"),
	  wxT("'a' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2A.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2A_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_B], wxT("FUNC2_B"),
	  wxT("'b' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2B.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2B_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_C], wxT("FUNC2_C"),
	  wxT("'c' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2C.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2C_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_D], wxT("FUNC2_D"),
	  wxT("'d' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2D.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2D_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_E], wxT("FUNC2_E"),
	  wxT("'e' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2E.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2E_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_F], wxT("FUNC2_F"),
	  wxT("'f' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2F.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2F_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_G], wxT("FUNC2_G"),
	  wxT("'g' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2G.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2G_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_H], wxT("FUNC2_H"),
	  wxT("'h' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2H.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2H_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_I], wxT("FUNC2_I"),
	  wxT("'i' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2I.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2I_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_J], wxT("FUNC2_J"),
	  wxT("'j' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2J.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2J_IonChanList);
	SetPar_UniParMgr(&pars[ICLIST_IC_FUNC2_K], wxT("FUNC2_K"),
	  wxT("'k' parameter values for second activation/inactivation function."),
	  UNIPAR_REAL_ARRAY, &theIC->hHuxley.func2K.ptr,
	  &theIC->numGates, (void *(*)) SetICHHFunc2K_IonChanList);
	return (TRUE);

}

/****************************** CheckPars *************************************/

/*
 * This routine checks that the necessary parameters for a ion channel list
 *.have been correctly initialised.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_IonChanList(IonChanListPtr theICList)
{
	static const WChar *funcName = wxT("CheckPars_IonChanList");
	BOOLN ok;

	ok = TRUE;
	if (!CheckInit_IonChanList(theICList, funcName))
		return (FALSE);
	if (theICList->ionChannels == NULL) {
		NotifyError(wxT("%s: ionChannels array not set."), funcName);
		ok = FALSE;
	}
	if (!theICList->numChannels <= 0) {
		NotifyError(wxT("%s: Number of Channels not correctly set."), funcName);
		ok = FALSE;
	}
	return (ok);

}

/****************************** PrintIonChannelPars ***************************/

/*
 * This routine prints all the ion channel parameters.
 * It assumes that the structure has been correctly initialised.
 */

void
PrintIonChannelPars_IonChanList(IonChannelPtr theIC)
{
	static const WChar *funcName = wxT("PrintIonChannelPars_IonChanList");
	int i;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return;
	DPrint(wxT("\t\t<---- Ion channel %s mode: %s ---->\n"),
	  iCListModeList[theIC->mode].name, theIC->description);
	DPrint(wxT("\t\tEnabled status: %s,"), BooleanList_NSpecLists(
	  theIC->enabled)->name);
	DPrint(wxT("\tEquilibrium Potential: %g (mV),\n"), MILLI(
	  theIC->equilibriumPot));
	switch (theIC->mode) {
	case ICLIST_BOLTZMANN_MODE: {
		ICBoltzmannParsPtr p = &theIC->boltzmann;
		DPrint(wxT("\t\tMax. conductance: %.3g S,"), theIC->maxConductance);
		DPrint(wxT("\tConductance Q10: %g,\n"), theIC->conductanceQ10);
		DPrint(wxT("\t\t\t%s\t%s\t%s\n"), wxT("V_1/2 (mV)"), wxT("Z (units)"),
		  wxT("tau (ms)"));
		for (i = 0; i < ICLIST_NUM_GATES; i++)
			DPrint(wxT("\t\t\t%g\t\t%g\t\t%g\n"), MILLI(p->halfMaxV.array[i]),
			  p->zZ.array[i], MILLI(p->tau.array[i]));
		break;
	}
	case ICLIST_FILE_MODE:
		DPrint(wxT("\t\tIonchannel table file: %s,\n"), theIC->fileName);
		DPrint(wxT("\t\tMax. conductance: %.3g S,\n"), theIC->maxConductance);
		break;
	case ICLIST_HHUXLEY_MODE:
	case ICLIST_ROTHMAN_MODE: {
		ICHHuxleyParsPtr p = &theIC->hHuxley;
		DPrint(wxT("\t\tBase max. conductance: %.3g (%.3g @ %g oC.) ")
		  wxT("(S),\n"), theIC->baseMaxConductance, theIC->maxConductance,
		  theIC->temperature);
		DPrint(wxT("\t\tConductance Q10: %g,\n"), theIC->conductanceQ10);
		DPrint(wxT("\t\t%6s%5s%5s%6s%5s%5s%6s%6s\n"), wxT("a"), wxT("b"), wxT(
		  "c"), wxT("d"), wxT("e"), wxT("f"), wxT("g"), wxT("h"));
		for (i = 0; i < theIC->numGates; i++) {
			DPrint(wxT("\t\t%6g%5g%5g%6g%5g%5g%6g       %s\n"),
			  p->func1A.array[i], p->func1B.array[i], p->func1C.array[i],
			  p->func1D.array[i], p->func1E.array[i], p->func1F.array[i],
			  p->func1G.array[i], (theIC->mode == ICLIST_HHUXLEY_MODE)?
			  wxT("Alpha") : wxT("Act."));
			DPrint(wxT("\t\t%6g%5g%5g%6g%5g%5g%6g%6g %s\n"),
			  p->func2A.array[i], p->func2B.array[i], p->func2C.array[i],
			  p->func2D.array[i], p->func2E.array[i], p->func2F.array[i],
			  p->func2G.array[i], p->func2H.array[i], (theIC->mode ==
			  ICLIST_HHUXLEY_MODE) ? wxT("Beta") : wxT("Tau"));
		}
		break;
	}
	default:
		;
	}
	DPrint(wxT("\t\tActivation exponent: %g,"), theIC->activationExponent);
	DPrint(wxT("\tNo. of table entries: %d\n"), theIC->numTableEntries);

}

/****************************** PrintTables ***********************************/

/*
 * This routine prints all the ion channel tables and parameters.
 * It assumes that the structure has been correctly initialised.
 * It expects there to be no more than three gates, i.e. y, z, and x.
 */

void
PrintTables_IonChanList(IonChanListPtr theICList)
{
	static const WChar *funcName = wxT("PrintTables_IonChanList");
	WChar	labelStr[MAXLINE];
	int	 i, j;
	FILE *fp = NULL, *savedFp;
	DynaListPtr node;
	ICTableEntryPtr e;
	IonChannelPtr theIC;
	static const WChar	*gateLabel = wxT("yzx");

	if (!CheckInit_IonChanList(theICList, funcName))
		return;
	OpenDiagnostics_NSpecLists(&fp, theICList->printTablesModeList,
	  theICList->printTablesMode);
	savedFp = GetDSAMPtr_Common()->parsFile;
	GetDSAMPtr_Common()->parsFile = fp;
	for (node = theICList->ionChannels; node != NULL; node = node->next) {
		theIC = (IonChannelPtr) node->data;
		PrintIonChannelPars_IonChanList(theIC);
		DPrint(wxT("\t\t\t%7s"), wxT("   V   "));
		for (i = 0; i < theIC->numGates; i++) {
			Snprintf_Utility_String(labelStr, MAXLINE, wxT("   %c   "),
			  DSAM_toupper(gateLabel[i]));
			DPrint(wxT("\t%s"), labelStr);
			Snprintf_Utility_String(labelStr, MAXLINE, wxT(" tau_%c "),
			  gateLabel[i]);
			DPrint(wxT("\t%s"), labelStr);
		}
		DPrint(wxT("\n"));
		DPrint(wxT("\t\t\t%7s"), wxT(" (0-1) "), wxT("  (ms) "));
		for (i = 0; i < theIC->numGates; i++)
			DPrint(wxT("\t%7s\t%7s"), wxT(" (0-1) "), wxT("  (ms) "));
		DPrint(wxT("\n"));
		for (i = 0; i < theIC->numTableEntries; i++) {
			e = &theIC->table[i];
			DPrint(wxT("\t\t\t%7g"), MILLI(theIC->minVoltage + i * theIC->dV));
			for (j = 0; j < theIC->numGates; j++)
				switch (j) {
				case 0:
					DPrint(wxT("\t%7.4f\t%7.4g"), e->yY, MSEC(e->ty));
				break;
				case 1:
					DPrint(wxT("\t%7.4f\t%7.4g"), e->zZ, MSEC(e->tz));
					break;
				case 2:
					DPrint(wxT("\t%7.4f\t%7.4g"), e->xX, MSEC(e->tx));
					break;
				}
			DPrint(wxT("\n"));
		}
	}
	GetDSAMPtr_Common()->parsFile = savedFp;
	CloseDiagnostics_NSpecLists(&fp);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the ion channel list parameters.
 */

BOOLN
PrintPars_IonChanList(IonChanListPtr theICs)
{
	static const WChar *funcName = wxT("PrintPars_IonChanList");
	DynaListPtr node;

	if (!CheckInit_IonChanList(theICs, funcName)) {
		NotifyError(wxT("%s: Ion channel list parameters not been correctly ")
		  wxT("set."), funcName);
		return (FALSE);
	}
	DPrint(wxT("\t\tIon channel list structure parameters:-\n"));
	DPrint(wxT("\t\tPrint tables mode: %s,\n"),
	  theICs->printTablesModeList[theICs->printTablesMode].name);
	if (theICs->useTemperatureCalcFlag) {
		DPrint(wxT("\t\tBase leakage conductance = %.3g (%.3g @ %g oC.) (S),")
		  wxT("\n"), theICs->baseLeakageCond, theICs->leakageCond, theICs->
		  temperature);
		DPrint(wxT("\t\tLeakage conductance Q10 = %g,\n"),
		  theICs-> leakageCondQ10);
	} else
		DPrint(wxT("\t\tLeakage conductance = %.3g S,\n"), theICs->leakageCond);
	DPrint(wxT("\t\tLeakage potential = %g (mV),\n"),
	  MILLI(theICs-> leakagePot));
	DPrint(wxT("\t\tNo. ion channels = %d\n"), theICs->numChannels);
	DPrint(wxT("\t\tMin./max voltage: %g / %g (mV),"), MILLI(
	  theICs-> minVoltage), MILLI(theICs->maxVoltage));
	DPrint(wxT("\tVoltage step: %g (mV),\n"), MILLI(theICs->dV));
	if (theICs->printTablesMode)
		PrintTables_IonChanList(theICs);
	else
		for (node = theICs->ionChannels; node != NULL; node = node->next)
	  PrintIonChannelPars_IonChanList((IonChannelPtr) node->data);
	return (TRUE);

}

/****************************** ReadVoltageTable ******************************/

/*
 * This function reads an Ion channel format file.
 * It returns a pointer to the data structure, or NULL if it fails in any way.
 */

BOOLN
ReadVoltageTable_IonChanList(IonChannelPtr theIC, FILE *fp)
{
	static const WChar *funcName = wxT("ReadVoltageTable_IonChanList");
	BOOLN ok = TRUE;
	int i;
	Float v, dV;

	for (i = 0; ok && (i < theIC->numTableEntries); i++) {
		ok = GetPars_ParFile(fp, wxT("%lf %lf %lf %lf %lf"), &v,
		  &theIC->table[i].yY, &theIC->table[i].ty, &theIC->table[i].zZ,
		  &theIC->table[i].tz);

		if (ok && ((fabs(theIC->table[i].ty) < DSAM_EPSILON) || (fabs(
		  theIC->table[i].ty) < DSAM_EPSILON))) {
			NotifyError(wxT("%s: The tau values must be greater than zero ")
			  wxT("(entry %d)."), funcName, i);
			ok = FALSE;
		}
		if (ok)
			switch (i) {
			case 0:
				if (fabs(theIC->minVoltage - v) > DSAM_EPSILON) {
					NotifyError(wxT("%s: Incorrect minimum voltage for cell ")
					  wxT("(%g mV)."), funcName, MILLI(v));
					ok = FALSE;
				}
				break;
			case 1:
				dV = v - theIC->minVoltage;
				if (fabs(theIC->dV - dV) > DSAM_EPSILON) {
					NotifyError(wxT("%s: Incorrect voltage step for cell (%g ")
					  wxT("mV)."), funcName, MILLI(dV));
					ok = FALSE;
				}
				break;
			default:
				if (fabs(theIC->minVoltage + i * theIC->dV - v) > DSAM_EPSILON) {
					NotifyError(wxT("%s: Table entry voltage out of sequence: ")
					  wxT("entry %d = %g mV."), funcName, i, MILLI(v));
					ok = FALSE;
				}
			}
	}
	fclose(fp);
	if (!ok)
	NotifyError(wxT("%s: Failed to read table: insufficient entries for\n")
	  wxT("voltage range specification."), funcName);
	return (ok);

}

/****************************** HHuxleyAlpha **********************************/

/*
 * This function calculates the alpha parameter for the HHuxley et al. tables.
 * It returns 1.0e3 if all the unified equation parameters are zero, this will
 * mean the activation and activation time constants will be 1.0.
 * It assumes that alpha is continuous about any singularities that occur in
 * the equation, i.e. mV = -f or mV = -k.
 */

Float
HHuxleyAlpha_IonChanList(Float a, Float b, Float c,
  Float d, Float e, Float f, Float g, Float h,
  Float i, Float j, Float k, Float mV, Float mDV,
  Float temperature)
{
	Float previous, next;

	if (HHUXLEY_ALL_ZERO_PARS(a, b, c, d, e, f, g, h, i, j, k))
		return (1.0e-3);
	if ((fabs(mV + f) > DSAM_EPSILON) && (fabs(mV + k)
	  > DSAM_EPSILON))
		return (a * HHUXLEY_TF(b, temperature) * (c * mV + d)
		  / (1 + e * exp((mV + f) / g)) + h * HHUXLEY_TF(
		  i, temperature) / (1 + j * exp(mV + k)));
	else {
		previous = HHuxleyAlpha_IonChanList(a, b, c, d, e, f,
		  g, h, i, j, k, mV - mDV, mDV, temperature);
		next = HHuxleyAlpha_IonChanList(a, b, c, d, e, f, g, h,
		  i, j, k, mV + mDV, mDV, temperature);
		return ((previous + next) / 2.0);
	}
}

/****************************** HHuxleyBeta ***********************************/

/*
 * This function calculates the beta parameter for the HHuxley et al. tables.
 * It returns 0.0 if all the unified equation parameters are zero, this will
 * mean the activation and activation time constants will be 1.0.
 * It assumes that beta is continuous about any singularities that occur in
 * the equation, i.e. mV = -j.
 */

Float
HHuxleyBeta_IonChanList(Float a, Float b, Float c,
  Float d, Float e, Float f, Float g, Float h,
  Float i, Float j, Float k, Float mV, Float mDV,
  Float temperature)
{
	Float previous, next;

	if (HHUXLEY_ALL_ZERO_PARS(a, b, c, d, e, f, g, h, i, j, k))
		return (0.0);
	if (fabs(mV + j) > DSAM_EPSILON)
		return (a * HHUXLEY_TF(b, temperature) * exp((mV + c)
		  / d) + e * HHUXLEY_TF(f, temperature) * (g * mV
		  + h) / (1 + i * exp((mV + j) / k)));
	else {
		previous = HHuxleyBeta_IonChanList(a, b, c, d, e, f, g,
		  h, i, j, k, mV - mDV, mDV, temperature);
		next = HHuxleyBeta_IonChanList(a, b, c, d, e, f, g, h,
		  i, j, k, mV + mDV, mDV, temperature);
		return ((previous + next) / 2.0);
	}
}

/****************************** SetTableEntry *********************************/

/*
 * Discrete variable elements are used to reduce the number of operations done
 * in the main cell processing loop, i.e. it removes the need for a loop for the
 * gates.  This is okay at present as the maximum number of gates is three and
 * at present only one ion channel has three out of the four used (for the
 * Rothman and Manis model).
 * No error checking is done.  It expects the index to be in a valid range.
 */

void
SetTableEntry_IonChanList(ICTableEntryPtr p, int index,
  Float activation, Float tau)
{
	switch (index) {
	case 0:
		p->yY = activation;
		p->ty = tau;
		break;
	case 1:
		p->zZ = activation;
		p->tz = tau;
		break;
	case 2:
		p->xX = activation;
		p->tx = tau;
		break;
	} /* switch */

}

/****************************** GenerateHHuxley *******************************/

/*
 * This function generates a list of ion channels in an ICList data structure,
 * using the HHuxley et al. equations (Rothman J. S. , Young E. D. and Manis P.
 * B. (1993) " Convergence of Auditory Nerve Fibres Onto Bushy Cells in the
 * Ventral Cochlear Nucleus: Implications of a Computational Model", Journal of
 * Neurophysiology, (70), No. 6, pp 2562-2583.
 * It expects the data structure to be passed to the routine as an argument.
 * The function returns TRUE if it succeeds, otherwise it returns FALSE.
 */

void
GenerateHHuxley_IonChanList(IonChannelPtr theIC)
{
	/* static const WChar	*funcName = wxT("GenerateHHuxley_IonChanList"); */
	int j, k;
	Float mV, mDV, alpha, beta, activation, tau;
	ICHHuxleyParsPtr p;

	p = &theIC->hHuxley;
	mDV = MILLI(theIC->dV);
	for (j = 0, mV = MILLI(theIC->minVoltage); j < theIC->numTableEntries;
	  j++, mV += mDV)
		for (k = 0; k < ICLIST_NUM_GATES; k++) {
			alpha = HHuxleyAlpha_IonChanList(
			  p->func1A.array[k], p->func1B.array[k],
			  p->func1C.array[k], p->func1D.array[k],
			  p->func1E.array[k], p->func1F.array[k],
			  p->func1G.array[k], p->func1H.array[k],
			  p->func1I.array[k], p->func1J.array[k],
			  p->func1K.array[k], mV, mDV,
			  theIC->temperature);
			beta = HHuxleyBeta_IonChanList(p->func2A.array[k],
			  p->func2B.array[k], p->func2C.array[k],
			  p->func2D.array[k], p->func2E.array[k],
			  p->func2F.array[k], p->func2G.array[k],
			  p->func2H.array[k], p->func2I.array[k],
			  p->func2J.array[k], p->func2K.array[k], mV,
			  mDV, theIC->temperature);
			activation = alpha / (alpha + beta);
			tau = 0.001 / (alpha + beta);
			SetTableEntry_IonChanList(&theIC->table[j], k, activation, tau);
		}
	theIC->updateFlag = FALSE;

}

/****************************** GenerateBoltzmann *****************************/

/*
 * This function generates a list of ion channels in an ICList data structure,
 * using the Manis & Marx equations (Manis P. B. and Marx S. O. (1991) "Outward
 * currents in isolated ventral cochlear
 * nucleus neurons", Journal of Neuroscience, (11), pp 2865-2880.
 * It expects the data structure to be passed to the routine as an argument.
 * This ion-channel mode does not use the baseMaxConductance field for the
 * ion-channel structure, but it does read it in, then sets the maxConductance
 * field from it.  This is so that there can be more uniformity in the universal
 * parameters implementation.
 * The function returns TRUE if it succeeds, otherwise it returns FALSE.
 */

void
GenerateBoltzmann_IonChanList(IonChannelPtr theIC)
{
	/* static const WChar	*funcName = wxT("GenerateBoltzmann_IonChanList"); */
	int j, k;
	Float v, activation, tau, kelvinTemp;
	ICBoltzmannParsPtr p;

	kelvinTemp = theIC->temperature - ABSOLUTE_ZERO_DEGREES;
	p = &theIC->boltzmann;
	for (j = 0, v = theIC->minVoltage; j < theIC->numTableEntries; j++, v +=
	  theIC->dV)
		for (k = 0; k < theIC->numGates; k++) {
			if (p->zZ.array[k] != 0.0) {
				activation = 1.0 / (1.0 + exp(-(v - p->halfMaxV.array[k])
				  * p->zZ.array[k] * E_OVER_R / kelvinTemp));
				tau = p->tau.array[k] / HHUXLEY_TF(theIC->conductanceQ10,
				  theIC->temperature);
			} else {
				activation = 1.0;
				tau = 1.0;
			}
			SetTableEntry_IonChanList(&theIC->table[j], k, activation, tau);
		}
	theIC->updateFlag = FALSE;

}

/****************************** ReadICGeneralPars *****************************/

/*
 * This routine reads the general parameters for an IC structure.
 * It assumes that the IonChannel structure and the IonChanList structure
 * have been correctly initialised.
 * This routine was created so that it could also be used by the
 * 'ResetIonChannel_' routine.
 * If '*fp' is NULL then the filename will not be from the file stream.
 */

BOOLN
ReadICGeneralPars_IonChanList(FILE **fp, ICModeSpecifier mode, WChar *fileName,
  WChar *description, WChar *enabled, Float *equilibriumPot,
  Float *baseMaxConductance, Float *activationExponent)
{
	static const WChar	*funcName = wxT("ReadICGeneralPars_IonChanList");
	BOOLN	ok = TRUE;
	WChar	*filePath;

	if (mode == ICLIST_FILE_MODE) {
		if (*fp && !GetPars_ParFile(*fp, wxT("%s"), fileName)) {
			NotifyError(wxT("%s: Could not read ion channel file '%s'."),
			  funcName, fileName);
			return(FALSE);
		}
		filePath = GetParsFileFPath_Common(fileName);
		if ((*fp = DSAM_fopen(filePath, "r")) == NULL) {
			NotifyError(wxT("%s: Could not open ion channel file '%s'."),
			  funcName, filePath);
			return(FALSE);
		}
	}
	if (!GetPars_ParFile(*fp, wxT("%s"), description))
		ok = FALSE;
	if (!GetPars_ParFile(*fp, wxT("%s"), enabled))
		ok = FALSE;
	if (!GetPars_ParFile(*fp, wxT("%lf"), equilibriumPot))
		ok = FALSE;
	if (!GetPars_ParFile(*fp, wxT("%lf"), baseMaxConductance))
		ok = FALSE;
	if (!GetPars_ParFile(*fp, wxT("%lf"), activationExponent))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Could not read general parameters."), funcName);
	return(ok);

}

/****************************** SetICGeneralParsFromICList ********************/

/*
 * This routine sets the general parameters for an IC structure which are
 * inherited from the IC list structure.
 * It assumes that the IonChannel structure and the IonChanList structure
 * have been correctly initialised.
 */

void
SetICGeneralParsFromICList_IonChanList(
		IonChannelPtr theIC, IonChanListPtr theICs)
{
	theIC->temperature = theICs->temperature;
	theIC->minVoltage = theICs->minVoltage;
	theIC->maxVoltage = theICs->maxVoltage;
	theIC->dV = theICs->dV;
	theIC->numTableEntries = theICs->numTableEntries;

}

/****************************** SetICGeneralPars ******************************/

/*
 * This routine sets the general parameters for an IC structure.
 * It assumes that the IonChannel structure and the IonChanList structure
 * have been correctly initialised.
 */

BOOLN
SetICGeneralPars_IonChanList(IonChannelPtr theIC, ICModeSpecifier mode,
  const WChar *description, WChar *enabled, Float equilibriumPot,
  Float baseMaxConductance, Float activationExponent)
{
	static const WChar	*funcName = wxT("SetICGeneralPars_IonChanList");
	BOOLN	ok = TRUE;

	if (!SetICDescription_IonChanList(theIC, description))
		ok = FALSE;
	if (!SetICEnabled_IonChanList(theIC, enabled))
		ok = FALSE;
	if (!SetICEquilibriumPot_IonChanList(theIC, equilibriumPot))
		ok = FALSE;
	if (!SetICBaseMaxConductance_IonChanList(theIC, baseMaxConductance))
		ok = FALSE;
	if (!SetICActivationExponent_IonChanList(theIC, activationExponent))
		ok = FALSE;
	if (!ok) {
		NotifyError(wxT("%s: Could not set general parameters."), funcName);
		return(FALSE);
	}
	theIC->mode = mode;
	return(TRUE);

}

/****************************** SetGeneratedPars ******************************/

/*
 * This routine sets the generated/calculated parameters for the ion channel
 * list.
 */

BOOLN
SetGeneratedPars_IonChanList(IonChanListPtr theICs)
{
	static const WChar	*funcName = wxT("SetGeneratedPars_IonChanList");
	DynaListPtr		node;
	IonChannelPtr	iC;

	if (theICs->dV <= DSAM_EPSILON) {
		NotifyError(wxT("%s: the voltage step must be greater than zero."),
		  funcName);
		return(FALSE);
	}
	if ((theICs->numTableEntries = (int) ceil((theICs->maxVoltage -
	  theICs->minVoltage) / theICs->dV) + 1) <= 0) {
		NotifyError(wxT("%s: Cannot define table from voltage range\n%g -> %g ")
		  wxT("mV and step size %g mV.\n"), funcName, theICs->minVoltage,
		  theICs->maxVoltage, theICs->dV);
		return(FALSE);
	}
	if (theICs->useTemperatureCalcFlag)
		theICs->leakageCond = theICs->baseLeakageCond *
		  HHUXLEY_TF(theICs->leakageCondQ10, theICs->temperature);
	for (node = theICs->ionChannels; node; node = node->next) {
		iC = (IonChannelPtr) node->data;
		iC->temperature = theICs->temperature;
		iC->maxConductance = iC->baseMaxConductance * HHUXLEY_TF(
		  iC->conductanceQ10, iC->temperature);
	}

	return(TRUE);

}

/****************************** GenerateDefault *******************************/

/*
 * This function returns a default ICList structure.
 * It is used to create a structure whose parameters can be set later.
 */

IonChanListPtr
GenerateDefault_IonChanList(void)
{
	static const WChar	*funcName = wxT("GenerateDefault_IonChanList");
	int		i, j;
	IonChanListPtr	theICs = NULL;
	IonChannelPtr	theIC;
	DynaListPtr		node;
	ICBoltzmannParsPtr	p;
	struct iC {
		WChar *	desc;
		Float	equilPot;
		Float	baseMax;
		Float	actExp;
		Float	condQ10;
		Float	currPars[2][3];
	} iCs[] = {
		{ wxT("Na_Conner"), 0.055, 300e-9, 1.0, 2.5, {{-0.0191, 3.7, 0.10e-3},
		  {-0.053, -3.6, 0.50e-3}}},
		{ wxT("K+_FastHiT"), -0.073, 54e-9, 1.0, 2.5, {{-0.015, 4, 1.61e-3},
		  {0.0, 0.0, 0.0}}},
		{ wxT("K+_SlowLoT"), -0.073, 18e-9, 1.0, 2.5, {{-0.028, 4.6, 10.0e-3},
		  {0.0, 0.0, 0.0}}},
		{ wxT("K+_BoltzmannTr"), -0.073, 24e-9, 1.0, 2.5, {{-0.0302, 0.6,
		  1.0e-3}, {-0.0612, -4.9, 3.0e-3}}}
	};

	if ((theICs = Init_IonChanList((WChar *) funcName)) == NULL) {
		NotifyError(wxT("%s: Out of memory for ion channel list structure."),
		  funcName);
		return(NULL);
	}
	SetNumChannels_IonChanList(theICs, 4);
	SetPrintTablesMode_IonChanList(theICs, wxT("off"));
	SetBaseLeakageCond_IonChanList(theICs, 0.150e-9);
	SetLeakagePot_IonChanList(theICs, -0.01);
	SetTemperature_IonChanList(theICs, 22.0);
	SetLeakageCondQ10_IonChanList(theICs, 2.0);
	SetMinVoltage_IonChanList(theICs, -0.120);
	SetMaxVoltage_IonChanList(theICs, 0.055);
	SetVoltageStep_IonChanList(theICs, 0.001);

	SetGeneratedPars_IonChanList(theICs);
	if (!PrepareIonChannels_IonChanList(theICs)) {
		NotifyError(wxT("%s: Failed to prepare the ion channels."), funcName);
		Free_IonChanList(&theICs);
		return(NULL);
	}
	for (node = theICs->ionChannels, i = 0; node; node = node->next, i++) {
		theIC = (IonChannelPtr) node->data;
		if (!SetICGeneralPars_IonChanList(theIC, ICLIST_BOLTZMANN_MODE,
		  iCs[i].desc, wxT("on"), iCs[i].equilPot, iCs[i].baseMax, iCs[i].
		  actExp)) {
			NotifyError(wxT("%s: Could not set general parameters."), funcName);
			Free_IonChanList(&theICs);
			return(NULL);
		}
		theIC->conductanceQ10 = iCs[i].condQ10;
		p = &theIC->boltzmann;
		SetICNumGates_IonChanList(theIC, ICLIST_NUM_GATES);
		for (j = 0; j < theIC->numGates; j++) {
			p->halfMaxV.array[j] = iCs[i].currPars[j][0];
			p->zZ.array[j] = iCs[i].currPars[j][1];
			p->tau.array[j] = iCs[i].currPars[j][2];
		}
		SetICPowFunc_IonChanList(theIC);
		SetIonChannelUniParListMode_IonChanList(theIC);
		GenerateBoltzmann_IonChanList(theIC);
	}
	if (!SetGeneralUniParList_IonChanList(theICs)) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IonChanList(&theICs);
		return(NULL);
	}
	return(theICs);

}

/****************************** GenerateRothman ******************************/

/*
 * This function generates a list of ion channels in an ICList data structure,
 * using the Rothman and Manis equations (Rothman J. S. and Manis P. B. (2003)
 * "The Roles Potassium Currents Play in Regulating the Electrical Activity
 * of Ventral Cochlear Nucleus Neurons", J. Neurophysiol 89, 3097-3113.)
 * It expects the data structure to be passed to the routine as an argument.
 */

void
GenerateRothman_IonChanList(IonChannelPtr theIC)
{
	/* static const WChar	*funcName = wxT("GenerateRothman_IonChanList"); */
	int j, k;
	Float mV, mDV, tau, activation, tauQ10;
	ICHHuxleyParsPtr p;

	p = &theIC->hHuxley;
	tauQ10 = HHUXLEY_TF(theIC->conductanceQ10 + 1, theIC->temperature);
	mDV = MILLI(theIC->dV);
	for (j = 0, mV = MILLI(theIC->minVoltage); j < theIC->numTableEntries;
	  j++, mV += mDV)
		for (k = 0; k < theIC->numGates; k++) {
			activation = (p->func1A.array[k] - p->func1B.array[k]) / pow(
			  p->func1C.array[k] + exp((mV + p->func1D.array[k]) /
			  p->func1E.array[k]), 1.0 / p->func1F.array[k]) +
			  p->func1G.array[k];
			tau = (p->func2A.array[k] / (p->func2B.array[k] * exp((mV +
			  p->func2C.array[k]) / p->func2D.array[k]) + p->func2E.array[k] *
			  exp(-(mV + p->func2F.array[k]) / p->func2G.array[k])) +
			  p->func2H.array[k]) * 1e-3 / tauQ10;
			SetTableEntry_IonChanList(&theIC->table[j], k, activation, tau);
		}
	theIC->updateFlag = FALSE;

}

/****************************** PrepareIonChannels ****************************/

/*
 * This adds a default ion channel to an IC List structure.
 * It is used to create an ion channel structure whose parameters can be set
 * later.
 * If the number of channels is reduced, then the channel at the top of the
 * list is removed.
 */

BOOLN
PrepareIonChannels_IonChanList(IonChanListPtr theICs)
{
	static const WChar *funcName = wxT("PrepareIonChannels_IonChanList");
	int i;
	IonChannelPtr theIC;
	DynaListPtr node, previous;

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	if (!SetGeneratedPars_IonChanList(theICs)) {
		NotifyError(wxT("%s: Ion channel parameters must be set first."),
		  funcName);
		return (FALSE);
	}
	if (!theICs->ionChannels)
		theICs->oldNumTableEntries = theICs->numTableEntries;
	if (theICs->numChannels != theICs->oldNumChannels) {
		if (theICs->numChannels < theICs->oldNumChannels) {
			node = GetLastInst_Utility_DynaList(theICs->ionChannels);
			for (i = theICs->numChannels; i < theICs->oldNumChannels; i++) {
				theIC = node->data;
				previous = node->previous;
				Remove_Utility_DynaList(&theICs->ionChannels, node);
				FreeIonChannel_IonChanList(&theIC);
				node = previous;
			}
		} else {
			for (i = theICs->oldNumChannels; i < theICs->numChannels; i++) {
				if ((theIC = InitIonChannel_IonChanList(funcName, theICs->
				  numTableEntries)) == NULL)
					return (FALSE);
				SetICGeneralParsFromICList_IonChanList(theIC, theICs);
				if (!Append_Utility_DynaList(&theICs->ionChannels, theIC)) {
					NotifyError(wxT("%s: Could not add ion channel [%d] to ")
					  wxT("list."), funcName, i);
					FreeIonChannel_IonChanList(&theIC);
					return (FALSE);
				}
				if (!SetIonChannelUniParList_IonChanList(theICs, theIC)) {
					NotifyError(wxT("%s: Could not initialise ion channel ")
					  wxT("'%s' parameter list."), funcName,
					  theIC->description);
					return (FALSE);
				}
			}
		}
		theICs->oldNumChannels = theICs->numChannels;
	}
	if (theICs->numTableEntries != theICs->oldNumTableEntries) {
		for (node = theICs->ionChannels; node; node = node->next) {
			theIC = (IonChannelPtr) node->data;
			theIC->table = (ICTableEntry *) realloc(theIC->table,
			  theICs->numTableEntries * sizeof(ICTableEntry));
		}
		theICs->oldNumTableEntries = theICs->numTableEntries;
	}
	return (TRUE);

}

/****************************** Pow1Func **************************************/

/*
 * This function is a fast implementation of x^1.
 * The second argument is there for compatibility with the standard 'pow'
 * function, but it is not used.
 */

Float
Pow1Func_IonChanList(Float x, Float dummy)
{
	return (x);

}

/****************************** Pow2Func **************************************/

/*
 * This function is a fast implementation of x^2.
 * The second argument is there for compatibility with the standard 'pow'
 * function, but it is not used.
 */

Float
Pow2Func_IonChanList(Float x, Float dummy)
{
	return (x * x);

}

/****************************** Pow3Func **************************************/

/*
 * This function is a fast implementation of x^3.
 * The second argument is there for compatibility with the standard 'pow'
 * function, but it is not used.
 */

Float
Pow3Func_IonChanList(Float x, Float dummy)
{
	return (x * x * x);

}

/****************************** Pow3Func **************************************/

/*
 * This function is a fast implementation of x^3.
 * The second argument is there for compatibility with the standard 'pow'
 * function, but it is not used.
 */

Float
Pow4Func_IonChanList(Float x, Float dummy)
{
	return (x * x * x * x);

}

/****************************** SetICPowFunc **********************************/

/*
 * This routine sets the ion channels Pow function.
 * If the activation is an integer, then the more efficient X * x ,etc. formas
 * are used.
 */

void
SetICPowFunc_IonChanList(IonChannelPtr theIC)
{
	int intActivation;

	if (fabs(theIC->activationExponent) != (intActivation = (int) floor(fabs(
	  theIC->activationExponent) + 0.5))) {
		theIC->PowFunc = DSAM_POW;
		return;
	}
	switch (intActivation) {
	case 2:
		theIC->PowFunc = Pow2Func_IonChanList;
		break;
	case 3:
		theIC->PowFunc = Pow3Func_IonChanList;
		break;
	case 1:
		theIC->PowFunc = Pow1Func_IonChanList;
		break;
	default:
		theIC->PowFunc = DSAM_POW;
	}

}

/****************************** ResetIonChannel *******************************/

/*
 * This routine resets an ion channel for an ion channel list.
 * It returns false if it fails in any way.
 */

BOOLN
ResetIonChannel_IonChanList(IonChanListPtr theICs, IonChannelPtr theIC)
{
	static const WChar *funcName = wxT("ResetIonChannel_IonChanList");
	BOOLN ok = TRUE;
	WChar enabled[SMALL_STRING], description[MAXLINE];
	Float equilibriumPot, baseMaxConductance, activationExponent;
	FILE *fp = NULL;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return (FALSE);
	SetICGeneralParsFromICList_IonChanList(theIC, theICs);
	switch (theIC->mode) {
	case ICLIST_BOLTZMANN_MODE:
		GenerateBoltzmann_IonChanList(theIC);
		break;
	case ICLIST_FILE_MODE:
		Init_ParFile();
		if (!ReadICGeneralPars_IonChanList(&fp, theIC->mode, theIC->fileName,
		  description, enabled, &equilibriumPot, &baseMaxConductance,
		  &activationExponent)) {
			NotifyError(wxT("%s: Could not read general parameters."),
			  funcName);
			ok = FALSE;
		}
		if (ok && !SetICGeneralPars_IonChanList(theIC, theIC->mode,
		  description, enabled, equilibriumPot, baseMaxConductance,
		  activationExponent)) {
			NotifyError(wxT("%s: Could not set general parameters."), funcName);
			FreeIonChannel_IonChanList(&theIC);
			ok = FALSE;
		}
		SetICGeneralParsFromICList_IonChanList(theIC, theICs);
		if (ok)
			ok = ReadVoltageTable_IonChanList(theIC, fp);
		Free_ParFile();
		break;
	case ICLIST_HHUXLEY_MODE:
		GenerateHHuxley_IonChanList(theIC);
		break;
	case ICLIST_ROTHMAN_MODE:
		GenerateRothman_IonChanList(theIC);
		break;
	default:
		NotifyError(wxT("%s: Ion channel mode not implemented (%d)."),
		  funcName, theIC->mode);
		ok = FALSE;
	} /* Switch */
	SetICPowFunc_IonChanList(theIC);
	return (ok);

}

/****************************** CheckInit *************************************/

/*
 * This routine checks whether or not an ion channel list.has been initialised.
 */

BOOLN
CheckInit_IonChanList(IonChanListPtr theICList,
		const WChar *callingFunction)
{
	static const WChar *funcName = wxT("CheckInit_IonChanList");

	if (theICList == NULL) {
		NotifyError(wxT("%s: ICList not set in %s."), funcName,
		  callingFunction);
		return (FALSE);
	}
	return (TRUE);

}

/****************************** CheckInitIC ***********************************/

/*
 * This routine checks whether or not an ion channel has been initialised.
 */

BOOLN
CheckInitIC_IonChanList(IonChannelPtr theIC, const WChar *callingFunction)
{
	static const WChar *funcName = wxT("CheckInitIC_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised in %s."), funcName,
		  callingFunction);
		return (FALSE);
	}
	return (TRUE);

}

/****************************** FindVoltageIndex ******************************/

/*
 * This function returns a pointer to an entry in a table, corresponding to a
 * specified voltage.
 * There will be a fatal error if the voltage is outside the bounds of the
 * table.
 * The "floor(..)" function is used for the rounding, because in biological
 * systems sub-threshold quantities do not fire, and leak away.
 */

ICTableEntryPtr
GetTableEntry_IonChanList(IonChannelPtr theIC, Float voltage)
{
	static const WChar *funcName = wxT("GetTableEntry_IonChanList");
	int index;

	index = (int) floor((voltage - theIC->minVoltage) / theIC->dV + 0.5);
	if ((index < 0) || (index >= theIC->numTableEntries)) {
		NotifyError(wxT("%s: Voltage is outside the table bounds for\nchannel ")
		  wxT("'%s' (%g mV)."), funcName, theIC->description, MILLI(voltage));
		return (NULL);
	}
	return (&theIC->table[index]);

}

/********************************* SetNumChannels *****************************/

/*
 * This routine sets the numChannels field of an IonChanList.
 */

BOOLN
SetNumChannels_IonChanList(IonChanListPtr theICs, int numChannels)
{
	static const WChar *funcName = wxT("SetNumChannels_IonChanList");
	DynaListPtr node;

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	if (numChannels < 1) {
		NotifyError(wxT("%s: Insufficient channels (%d)."), funcName,
		  numChannels);
		return (FALSE);
	}
	theICs->numChannels = numChannels;
	theICs->updateFlag = TRUE;
	if (theICs->ionChannels)
		for (node = theICs->ionChannels; node; node = node->next)
			((IonChannelPtr) node->data)->description[0] = '\0';
	return (TRUE);

}

/********************************* SetTemperature *****************************/

/*
 * This routine sets the temperature field of an IonChanList.
 */

BOOLN
SetTemperature_IonChanList(IonChanListPtr theICs, Float theTemperature)
{
	static const WChar *funcName = wxT("SetTemperature_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	theICs->temperature = theTemperature;
	theICs->updateFlag = TRUE;
	return (TRUE);

}

/********************************* SetLeakageCondQ10 **************************/

/*
 * This routine sets the leakageCondQ10 field of an IonChanList.
 */

BOOLN
SetLeakageCondQ10_IonChanList(IonChanListPtr theICs, Float theLeakageCondQ10)
{
	static const WChar *funcName = wxT("SetLeakageCondQ10_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	theICs->leakageCondQ10 = theLeakageCondQ10;
	return (TRUE);

}

/********************************* SetPrintTablesMode *************************/

/*
 * This routine sets the Print Tables Mode of an IonChanList.
 */

BOOLN
SetPrintTablesMode_IonChanList(IonChanListPtr theICs, WChar *modeName)
{
	static const WChar *funcName = wxT("SetPrintTablesMode_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	theICs->printTablesMode = IdentifyDiag_NSpecLists(modeName,
	  theICs->printTablesModeList);
	return (TRUE);

}

/********************************* SetBaseLeakageCond *************************/

/*
 * This routine sets the baseLeakageCond field of an IonChanList.
 */

BOOLN
SetBaseLeakageCond_IonChanList(IonChanListPtr theICs, Float baseLeakageCond)
{
	static const WChar *funcName = wxT("SetBaseLeakageCond_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	theICs->leakageCond = theICs->baseLeakageCond = baseLeakageCond;
	return (TRUE);

}

/********************************* SetLeakagePot ******************************/

/*
 * This routine sets the leakagePot field of an IonChanList.
 */

BOOLN
SetLeakagePot_IonChanList(IonChanListPtr theICs, Float leakagePot)
{
	static const WChar *funcName = wxT("SetBaseLeakageCond_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	theICs->leakagePot = leakagePot;
	return (TRUE);

}

/********************************* SetMinVoltage ******************************/

/*
 * This routine sets the minimum voltage parameter for the ion channel list.
 */

BOOLN
SetMinVoltage_IonChanList(IonChanListPtr theICs, Float theMinVoltage)
{
	static const WChar *funcName = wxT("SetMinVoltage_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	theICs->minVoltage = theMinVoltage;
	theICs->updateFlag = TRUE;
	return (TRUE);

}

/********************************* SetMaxVoltage ******************************/

/*
 * This routine sets the maximum voltage parameter for the ion channel list.
 */

BOOLN
SetMaxVoltage_IonChanList(IonChanListPtr theICs, Float theMaxVoltage)
{
	static const WChar *funcName = wxT("SetMaxVoltage_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	theICs->maxVoltage = theMaxVoltage;
	theICs->updateFlag = TRUE;
	return (TRUE);

}

/********************************* SetVoltageStep *****************************/

/*
 * This routine sets the voltage step parameter for the ion channel list.
 */

BOOLN
SetVoltageStep_IonChanList(IonChanListPtr theICs, Float theVoltageStep)
{
	static const WChar *funcName = wxT("SetVoltageStep_IonChanList");

	if (!CheckInit_IonChanList(theICs, funcName))
		return (FALSE);
	theICs->dV = theVoltageStep;
	theICs->updateFlag = TRUE;
	return (TRUE);

}

/****************************** SetICEnabled **********************************/

/*
 * This function sets the module's operationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetICEnabled_IonChanList(IonChannelPtr theIC, WChar *theICEnabled)
{
	static const WChar *funcName = wxT("SetICEnabled_IonChanList(");
	int specifier;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return (FALSE);
	if ((specifier = Identify_NameSpecifier(theICEnabled,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal status name (%s)."), funcName,
		  theICEnabled);
		return (FALSE);
	}
	theIC->enabled = specifier;
	return (TRUE);

}

/********************************* SetICMode **********************************/

/*
 * This routine sets the diagnostic mode of an IonChanList.
 */

BOOLN
SetICMode_IonChanList(IonChannelPtr theIC, WChar *modeName)
{
	static const WChar *funcName = wxT("SetICMode_IonChanList");
	int mode;

	if (!CheckInitIC_IonChanList(theIC, funcName))
		return (FALSE);
	if ((mode = Identify_NameSpecifier(modeName, iCListModeList)) ==
	  ICLIST_NULL) {
		NotifyError(wxT("%s: Unknown ion channel mode (%s)."), funcName,
		  modeName);
		return (FALSE);
	}
	theIC->mode = (ICModeSpecifier) mode;
	if (theIC->parList) {
		SetIonChannelUniParListMode_IonChanList(theIC);
		theIC->parList->updateFlag = TRUE;
	}
	theIC->updateFlag = TRUE;
	return (TRUE);

}

/********************************* SetICDescription ***************************/

/*
 * This routine sets the specifier ion channel's description parameter.
 */

BOOLN
SetICDescription_IonChanList(IonChannelPtr theIC, const WChar *theDescription)
{
	static const WChar *funcName = wxT("SetICDescription_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	DSAM_strncpy(theIC->description, theDescription, MAXLINE);
	return (TRUE);

}

/********************************* SetICEquilibriumPot ************************/

/*
 * This routine sets the specifier ion channel's EquilibriumPot parameter.
 */

BOOLN
SetICEquilibriumPot_IonChanList(IonChannelPtr theIC, Float theEquilibriumPot)
{
	static const WChar *funcName = wxT("SetICEquilibriumPot_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->equilibriumPot = theEquilibriumPot;
	return (TRUE);

}

/********************************* SetICBaseMaxConductance ********************/

/*
 * This routine sets the specifier ion channel's baseMaxConductance parameter.
 */

BOOLN
SetICBaseMaxConductance_IonChanList(IonChannelPtr theIC,
  Float theBaseMaxConductance)
{
	static const WChar *funcName = wxT("SetICBaseMaxConductance_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->baseMaxConductance = theBaseMaxConductance;
	switch (theIC->mode) {
	case ICLIST_BOLTZMANN_MODE:
	case ICLIST_HHUXLEY_MODE:
	case ICLIST_ROTHMAN_MODE:
		theIC->maxConductance = theIC->baseMaxConductance *
		  HHUXLEY_TF(theIC->conductanceQ10, theIC->temperature);
	default:
		theIC->maxConductance = theIC->baseMaxConductance;
	}
	return (TRUE);

}

/********************************* SetICConductanceQ10 ************************/

/*
 * This routine sets the specifier ion channel's conductanceQ10 parameter.
 */

BOOLN
SetICConductanceQ10_IonChanList(IonChannelPtr theIC,
  Float theConductanceQ10)
{
	static const WChar *funcName = wxT("SetICConductanceQ10_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->conductanceQ10 = theConductanceQ10;
	return (TRUE);

}

/********************************* SetICActivationExponent ********************/

/*
 * This routine sets the specifier ion channel's activationExponent parameter.
 */

BOOLN
SetICActivationExponent_IonChanList(IonChannelPtr theIC,
  Float theActivationExponent)
{
	static const WChar *funcName = wxT("SetICActivationExponent_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->activationExponent = theActivationExponent;
	return(TRUE);

}

/********************************* SetICBoltzmannHalfMaxV *********************/

/*
 * This routine sets the specifier ion channel's halfMaxV array element.
 */

BOOLN
SetICBoltzmannHalfMaxV_IonChanList(IonChannelPtr theIC, int index,
  Float theHalfMaxV)
{
	static const WChar *funcName = wxT("SetICBoltzmannHalfMaxV_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->boltzmann.halfMaxV.array[index] = theHalfMaxV;
	return(TRUE);

}

/********************************* SetICBoltzmannZ ****************************/

/*
 * This routine sets the specifier ion channel's zZ array element.
 */

BOOLN
SetICBoltzmannZ_IonChanList(IonChannelPtr theIC, int index, Float theZ)
{
	static const WChar *funcName = wxT("SetICBoltzmannZ_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->boltzmann.zZ.array[index] = theZ;
	return(TRUE);

}

/********************************* SetICBoltzmannTau **************************/

/*
 * This routine sets the specifier ion channel's tau array element.
 */

BOOLN
SetICBoltzmannTau_IonChanList(IonChannelPtr theIC, int index, Float theTau)
{
	static const WChar *funcName = wxT("SetICBoltzmannTau_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return(FALSE);
	}
	theIC->boltzmann.tau.array[index] = theTau;
	return(TRUE);

}

/****************************** SetICActivationMode ******************************/

/*
 * This function sets the module's activationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetICActivationMode_IonChanList(IonChannelPtr theIC, WChar * theActivationMode)
{
	static const WChar *funcName = wxT("SetActivationMode_IonChanList");
	int specifier;

	if (!theIC) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return (FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theActivationMode,
	  ActivationModeList_IonChanList(0))) == ICLIST_ACTIVATION_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theActivationMode);
		return (FALSE);
	}
	theIC->activationMode = specifier;
	return (TRUE);

}

/********************************* SetICNumGates *****************************/

/*
 * This routine sets the specifier ion channel's AlphaG array element.
 */

BOOLN
SetICNumGates_IonChanList(IonChannelPtr theIC, int theNumGates)
{
	static const WChar *funcName = wxT("SetICNumGates_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	if (theNumGates < 1) {
		NotifyError(wxT("%s: The number of gates must be greater than ")
		  wxT("zero (%d)"), funcName, theNumGates);
		return (FALSE);
	}
	theIC->numGates = theNumGates;
	return (TRUE);

}

/********************************* SetICHHFunc1A *************************/

/*
 * This routine sets the specifier ion channel's ActA array element.
 */

BOOLN
SetICHHFunc1A_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1A_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func1A.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1B *************************/

/*
 * This routine sets the specifier ion channel's ActB array element.
 */

BOOLN
SetICHHFunc1B_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1B_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func1B.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1C *************************/

/*
 * This routine sets the specifier ion channel's ActC array element.
 */

BOOLN
SetICHHFunc1C_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1C_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func1C.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1D *************************/

/*
 * This routine sets the specifier ion channel's ActD array element.
 */

BOOLN
SetICHHFunc1D_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1D_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func1D.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1E *************************/

/*
 * This routine sets the specifier ion channel's ActE array element.
 */

BOOLN
SetICHHFunc1E_IonChanList(IonChannelPtr theIC, int index,
		Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1E_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func1E.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1F *************************/

/*
 * This routine sets the specifier ion channel's ActF array element.
 */

BOOLN
SetICHHFunc1F_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1F_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func1F.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1G *************************/

/*
 * This routine sets the specifier ion channel's ActG array element.
 */

BOOLN
SetICHHFunc1G_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1G_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func1G.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1H *************************/

/*
 * This routine sets the specifier ion channel's tauH array element.
 */

BOOLN
SetICHHFunc1H_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1H_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2H.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1I *************************/

/*
 * This routine sets the specifier ion channel's tauI array element.
 */

BOOLN
SetICHHFunc1I_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1I_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2I.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1J *************************/

/*
 * This routine sets the specifier ion channel's tauJ array element.
 */

BOOLN
SetICHHFunc1J_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1J_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2J.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc1K *************************/

/*
 * This routine sets the specifier ion channel's tauK array element.
 */

BOOLN
SetICHHFunc1K_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc1K_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2K.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2A *************************/

/*
 * This routine sets the specifier ion channel's tauA array element.
 */

BOOLN
SetICHHFunc2A_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2A_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2A.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2B *************************/

/*
 * This routine sets the specifier ion channeActlphaB array element.
 */

BOOLN
SetICHHFunc2B_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2B_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2B.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2C *************************/

/*
 * This routine sets the specifier ion channel's tauC array element.
 */

BOOLN
SetICHHFunc2C_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2C_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2C.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2D *************************/

/*
 * This routine sets the specifier ion channel's tauD array element.
 */

BOOLN
SetICHHFunc2D_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2D_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2D.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2E *************************/

/*
 * This routine sets the specifier ion channel's tauE array element.
 */

BOOLN
SetICHHFunc2E_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2E_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2E.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2F *************************/

/*
 * This routine sets the specifier ion channel's tauF array element.
 */

BOOLN
SetICHHFunc2F_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2F_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2F.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2G *************************/

/*
 * This routine sets the specifier ion channel's tauG array element.
 */

BOOLN
SetICHHFunc2G_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2G_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2G.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2H *************************/

/*
 * This routine sets the specifier ion channel's tauH array element.
 */

BOOLN
SetICHHFunc2H_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2H_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2H.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2I *************************/

/*
 * This routine sets the specifier ion channel's tauI array element.
 */

BOOLN
SetICHHFunc2I_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2I_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2I.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2J *************************/

/*
 * This routine sets the specifier ion channel's tauJ array element.
 */

BOOLN
SetICHHFunc2J_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2J_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2J.array[index] = value;
	return (TRUE);

}

/********************************* SetICHHFunc2K *************************/

/*
 * This routine sets the specifier ion channel's tauK array element.
 */

BOOLN
SetICHHFunc2K_IonChanList(IonChannelPtr theIC, int index, Float value)
{
	static const WChar *funcName = wxT("SetICHHFunc2K_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	theIC->hHuxley.func2K.array[index] = value;
	return (TRUE);

}

/********************************* SetICFileName ******************************/

/*
 * This routine sets the specifier ion channel table file name.
 */

BOOLN
SetICFileName_IonChanList(IonChannelPtr theIC, WChar *fileName)
{
	static const WChar *funcName = wxT("SetICFileName_IonChanList");

	if (!theIC) {
		NotifyError(wxT("%s: Ion channel not initialised."), funcName);
		return (FALSE);
	}
	DSAM_strncpy(theIC->fileName, fileName, MAX_FILE_PATH);
	theIC->updateFlag = TRUE;
	if (theIC->parList)
		theIC->parList->updateFlag = TRUE;
	return (TRUE);

}

