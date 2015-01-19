/**********************
 *
 * File:		UtIonChanList.h
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
 *				structure. This allows the 'SetBaseMaxConducance' routine
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

#ifndef _UTIONCHANLIST_H
#include "UtDynaList.h"

#define _UTIONCHANLIST_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ICLIST_NUM_PARS				9
#define ICLIST_NUM_CONSTANT_PARS	7

#define ICLIST_IC_NUM_PARS			35
#define ICLIST_IC_NUM_CONSTANT_PARS	7

#define	E_OVER_R					11.8e3
#define	ABSOLUTE_ZERO_DEGREES		-273.0
#define	ICLIST_NUM_GATES			2
#define	ICLIST_MAX_GATES			3
#define ICLIST_ROTHMAN_PHI			0.85

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define HHUXLEY_ALL_ZERO_PARS(A, B, C, D, E, F, G, H, I, J, K) ((fabs(A) + \
		  fabs(B) + fabs(C) + fabs(D) + fabs(E) + fabs(F) + fabs(G) + \
		  fabs(H) + fabs(I) + fabs(J) + fabs(K)) <= DSAM_EPSILON)

#define HHUXLEY_TF(A, T)	(pow(A, ((T) - 22) / 10))

#define SET_IC_GATE_ARRAY(GATE)	((GATE).ptr = (GATE).array)

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/*
 * Ion channel modes.
 */

typedef enum {

	ICLIST_BOLTZMANN_MODE,
	ICLIST_FILE_MODE,
	ICLIST_HHUXLEY_MODE,
	ICLIST_ROTHMAN_MODE,
	ICLIST_NULL

} ICModeSpecifier;

typedef enum {

	ICLIST_ACTIVATION_MODE_STANDARD,
	ICLIST_ACTIVATION_MODE_ROTHMAN_KHT,
	ICLIST_ACTIVATION_MODE_1_GATE,
	ICLIST_ACTIVATION_MODE_3_GATE,
	ICLIST_ACTIVATION_MODE_NULL

} ICActivationModeSpecifier;

typedef enum {

	ICLIST_PRINTTABLESMODE,
	ICLIST_BASELEAKAGECOND,
	ICLIST_LEAKAGE_COND_Q10,
	ICLIST_LEAKAGEPOT,
	ICLIST_NUMCHANNELS,
	ICLIST_MIN_VOLT,
	ICLIST_MAX_VOLT,
	ICLIST_DV,
	ICLIST_TEMPERATURE

} ICListParSpecifier;

typedef enum {

	ICLIST_IC_DESCRIPTION,
	ICLIST_IC_MODE,
	ICLIST_IC_ENABLED,
	ICLIST_IC_EQUILIBRIUM_POT,
	ICLIST_IC_CONDUCTANCE,
	ICLIST_IC_CONDUCTANCE_Q10,
	ICLIST_IC_ACTIVATION_EXPONENT,
	ICLIST_IC_NUM_GATES,
	ICLIST_IC_V_HALF,
	ICLIST_IC_Z,
	ICLIST_IC_TAU,
	ICLIST_IC_FILE_NAME,
	ICLIST_IC_ACIVATION_MODE,
	ICLIST_IC_FUNC1_A,
	ICLIST_IC_FUNC1_B,
	ICLIST_IC_FUNC1_C,
	ICLIST_IC_FUNC1_D,
	ICLIST_IC_FUNC1_E,
	ICLIST_IC_FUNC1_F,
	ICLIST_IC_FUNC1_G,
	ICLIST_IC_FUNC1_H,
	ICLIST_IC_FUNC1_I,
	ICLIST_IC_FUNC1_J,
	ICLIST_IC_FUNC1_K,
	ICLIST_IC_FUNC2_A,
	ICLIST_IC_FUNC2_B,
	ICLIST_IC_FUNC2_C,
	ICLIST_IC_FUNC2_D,
	ICLIST_IC_FUNC2_E,
	ICLIST_IC_FUNC2_F,
	ICLIST_IC_FUNC2_G,
	ICLIST_IC_FUNC2_H,
	ICLIST_IC_FUNC2_I,
	ICLIST_IC_FUNC2_J,
	ICLIST_IC_FUNC2_K

} IonChannelParSpecifier;

typedef struct {

	Float	yY, ty;
	Float	zZ, tz;
	Float	xX, tx;

} ICTableEntry, *ICTableEntryPtr;

typedef struct {

	Float	array[ICLIST_MAX_GATES];
	Float	*ptr;

} ICGateArray, *ICGateArrayPtr;

typedef	struct {

	ICGateArray	halfMaxV;
	ICGateArray	zZ;
	ICGateArray	tau;

} ICBoltzmannPars, *ICBoltzmannParsPtr;

typedef	struct {

	ICGateArray	func1A, func1B, func1C, func1D, func1E, func1F, func1G, func1H;
	ICGateArray	func1I, func1J, func1K;
	ICGateArray	func2A, func2B, func2C, func2D, func2E, func2F, func2G, func2H;
	ICGateArray	func2I, func2J, func2K;

} ICHHuxleyPars, *ICHHuxleyParsPtr;

typedef struct {

	BOOLN	updateFlag;
	ICModeSpecifier		mode;
	BOOLN	enabled;
	WChar	description[MAXLINE];
	int		numTableEntries;
	Float	activationExponent;
	Float	equilibriumPot;
	Float	temperature;
	Float	baseMaxConductance, maxConductance, conductanceQ10;
	Float	minVoltage;
	Float	maxVoltage;
	Float	dV;
	int		activationMode;
	int		numGates;
	ICBoltzmannPars	boltzmann;
	ICHHuxleyPars	hHuxley;
	WChar			fileName[MAX_FILE_PATH];
	Float			(* PowFunc)(Float, Float);
	ICTableEntry	*table;
	UniParListPtr	parList;

} IonChannel, *IonChannelPtr;

typedef struct IonChanList {

	BOOLN	updateFlag;
	BOOLN	useTemperatureCalcFlag;
	int		printTablesMode;
	Float	temperature;
	Float	baseLeakageCond, leakageCond, leakageCondQ10;
	Float	leakagePot;
	int		numChannels;
	Float	minVoltage;
	Float	maxVoltage;
	Float	dV;
	DynaListPtr	ionChannels;

	/* Private members */
	NameSpecifier	*printTablesModeList;
	UniParListPtr	parList;
	IonChannelPtr	currentIC;			/* Used by universal parameter list. */
	int		oldNumChannels;
	int		numTableEntries;
	int		oldNumTableEntries;
	WChar	diagFileName[MAX_FILE_PATH];

} IonChanList, *IonChanListPtr;

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckInit_IonChanList(IonChanListPtr theICList,
		  const WChar *callingFunction);

BOOLN	CheckInitIC_IonChanList(IonChannelPtr theIC,
		  const WChar *callingFunction);

BOOLN	CheckPars_IonChanList(IonChanListPtr theICList);

BOOLN	Free_IonChanList(IonChanListPtr *theICList);

void	FreeIonChannel_IonChanList(IonChannelPtr *theIC);

void	FreeIonChannels_IonChanList(IonChanListPtr theICList);

IonChanListPtr	GenerateDefault_IonChanList(void);

ICTableEntryPtr	GetTableEntry_IonChanList(IonChannelPtr theIC, Float voltage);

void	GenerateBoltzmann_IonChanList(IonChannelPtr theIC);

void	GenerateHHuxley_IonChanList(IonChannelPtr theIC);

IonChanListPtr	Init_IonChanList(const WChar *callingFunctionName);

IonChannelPtr	InitIonChannel_IonChanList(const WChar *callingFunctionName,
				  int numTableEntries);

void	PrintIonChannelPars_IonChanList(IonChannelPtr theIC);

BOOLN	PrintPars_IonChanList(IonChanListPtr theICList);

void	PrintTables_IonChanList(IonChanListPtr theICList);

BOOLN	ReadVoltageTable_IonChanList(IonChannelPtr theIC, FILE *fp);

IonChanListPtr	ReadPars_IonChanList(FILE *fp);

BOOLN	ReadICGeneralPars_IonChanList(FILE **fp, ICModeSpecifier mode,
		  WChar *fileName, WChar *description, WChar *enabled,
		  Float *equilibriumPot, Float *baseMaxConductance,
		  Float *activationExponent);

BOOLN	ResetIonChannel_IonChanList(IonChanListPtr theICs, IonChannelPtr theIC);

Float	HHuxleyAlpha_IonChanList(Float a, Float b, Float c, Float d,
		  Float e, Float f, Float g, Float h, Float i, Float j, Float k,
		  Float mV, Float mDV, Float temperature);

Float	HHuxleyBeta_IonChanList(Float a, Float b, Float c, Float d,
		  Float e, Float f, Float g, Float h, Float i, Float j, Float k,
		  Float mV, Float mDV, Float temperature);

BOOLN	SetBaseLeakageCond_IonChanList(IonChanListPtr theICs,
		  Float baseLeakageCond);

BOOLN	PrepareIonChannels_IonChanList(IonChanListPtr theICs);

BOOLN	SetGeneralUniParList_IonChanList(IonChanListPtr theICs);

BOOLN	SetGeneralUniParListMode_IonChanList(IonChanListPtr theICs);

BOOLN	SetGeneratedPars_IonChanList(IonChanListPtr theICs);

BOOLN	SetICActivationExponent_IonChanList(IonChannelPtr theIC,
		  Float theActivationExponent);

BOOLN	SetICActivationMode_IonChanList(IonChannelPtr theIC,
		  WChar * theActivationMode);

BOOLN	SetICBaseMaxConductance_IonChanList(IonChannelPtr theIC,
		  Float theBaseMaxConductance);

BOOLN	SetICConductanceQ10_IonChanList(IonChannelPtr theIC,
		  Float theConductanceQ10);

BOOLN	SetICDescription_IonChanList(IonChannelPtr theIC,
		  const WChar *theDescription);

BOOLN	SetICEnabled_IonChanList(IonChannelPtr theIC, WChar *theICEnabled);

BOOLN	SetICEquilibriumPot_IonChanList(IonChannelPtr theIC,
		  Float theEquilibriumPot);

BOOLN	SetICFileName_IonChanList(IonChannelPtr theIC, WChar *fileName);

BOOLN	SetICGeneralPars_IonChanList(IonChannelPtr theIC, ICModeSpecifier mode,
		  const WChar *description, WChar *enabled, Float equilibriumPot,
		  Float baseMaxConductance, Float activationExponent);

void	SetICGeneralParsFromICList_IonChanList(IonChannelPtr theIC,
		  IonChanListPtr theICs);

BOOLN	SetICBoltzmannHalfMaxV_IonChanList(IonChannelPtr theIC, int index,
		  Float theHalfMaxV);

BOOLN	SetICBoltzmannZ_IonChanList(IonChannelPtr theIC, int index,
		  Float theZ);

BOOLN	SetICBoltzmannTau_IonChanList(IonChannelPtr theIC, int index,
		  Float theTau);

BOOLN	SetICMode_IonChanList(IonChannelPtr theIC, WChar *modeName);

void	SetICPowFunc_IonChanList(IonChannelPtr theIC);

BOOLN	SetICHHFunc1A_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1B_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1C_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1D_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1E_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1F_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1G_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1H_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1I_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1J_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc1K_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2A_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2B_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2C_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2D_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2E_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2F_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2G_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2H_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2I_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2J_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICHHFunc2K_IonChanList(IonChannelPtr theIC, int index, Float value);

BOOLN	SetICNumGates_IonChanList(IonChannelPtr theIC, int theNumGates);

void	SetICPowFunc_IonChanList(IonChannelPtr theIC);

BOOLN	SetIonChannelUniParList_IonChanList(IonChanListPtr theICs,
		  IonChannelPtr theIC);

BOOLN	SetIonChannelUniParListMode_IonChanList(IonChannelPtr theIC);

BOOLN	SetLeakageCondQ10_IonChanList(IonChanListPtr theICs,
		  Float theLeakageCondQ10);

BOOLN	SetLeakagePot_IonChanList(IonChanListPtr theICs, Float leakagePot);

BOOLN	SetMaxVoltage_IonChanList(IonChanListPtr theICs, Float theMaxVoltage);

BOOLN	SetMinVoltage_IonChanList(IonChanListPtr theICs, Float theMinVoltage);

BOOLN	SetNumChannels_IonChanList(IonChanListPtr theICs, int numChannels);

BOOLN	SetNumGates_IonChanList(IonChannelPtr theIC, int numGates);

BOOLN	SetPrintTablesMode_IonChanList(IonChanListPtr theICs, WChar *modeName);

BOOLN	SetTemperature_IonChanList(IonChanListPtr theICs,
		  Float theTemperature);

BOOLN	SetVoltageStep_IonChanList(IonChanListPtr theICs,
		  Float theVoltageStep);

__END_DECLS

#endif
