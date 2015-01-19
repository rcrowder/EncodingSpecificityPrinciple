/**********************
 *
 * File:		MoANSGCarney.h
 * Purpose:		Laurel H. Carney auditory nerve spike generation module:
 *				Carney L. H. (1993) "A model for the responses of low-frequency
 *				auditory-nerve fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				24-02-97 LPO: Amended to allow multiple fibres.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		7 May 1996
 * Updated:		24 Feb 1997
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

#ifndef _MOANSGCARNEY_H
#define _MOANSGCARNEY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANSPIKEGEN_CARNEY_NUM_PARS					14

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_CARNEY_DIAGNOSTICMODE,
	ANSPIKEGEN_CARNEY_INPUTMODE,
	ANSPIKEGEN_CARNEY_OUTPUTMODE,
	ANSPIKEGEN_CARNEY_RANSEED,
	ANSPIKEGEN_CARNEY_NUMFIBRES,
	ANSPIKEGEN_CARNEY_PULSEDURATIONCOEFF,
	ANSPIKEGEN_CARNEY_PULSEMAGNITUDE,
	ANSPIKEGEN_CARNEY_REFRACTORYPERIOD,
	ANSPIKEGEN_CARNEY_MAXTHRESHOLD,
	ANSPIKEGEN_CARNEY_DISCHARGECOEFFC0,
	ANSPIKEGEN_CARNEY_DISCHARGECOEFFC1,
	ANSPIKEGEN_CARNEY_DISCHARGETCONSTS0,
	ANSPIKEGEN_CARNEY_DISCHARGETCONSTS1,
	ANSPIKEGEN_CARNEY_DISTRIBUTION

} CarneySGParSpecifier;

typedef enum {

	ANSPIKEGEN_CARNEY_INPUTMODE_CORRECTED,
	ANSPIKEGEN_CARNEY_INPUTMODE_ORIGINAL,
	ANSPIKEGEN_CARNEY_INPUTMODE_NULL

} ANSpikeGenInputModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		inputMode;
	int		outputMode;
	long	ranSeed;
	int		numFibres;
	Float	pulseDurationCoeff;
	Float	pulseMagnitude;
	Float	refractoryPeriod;
	Float	maxThreshold;
	Float	dischargeCoeffC0;
	Float	dischargeCoeffC1;
	Float	dischargeTConstS0;
	Float	dischargeTConstS1;
	ParArrayPtr	distribution;

	/* Private members */
	NameSpecifier	*diagnosticModeList;
	UniParListPtr	parList;
	WChar			diagFileName[MAX_FILE_PATH];
	ChanLen	pulseDurationIndex;
	ChanLen	refractoryPeriodIndex;
	FILE	*fp;
	Float	dt;
	ChanData	*pulse;
	ChanLen	**pulseIndex;
	ChanLen	**timerIndex;
	ANSGDistPtr	aNDist;

} CarneySG, *CarneySGPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CarneySGPtr	carneySGPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	Free_ANSpikeGen_Carney(void);

void	FreeProcessVariables_ANSpikeGen_Carney(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Carney(void);

BOOLN	InitProcessVariables_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	Init_ANSpikeGen_Carney(ParameterSpecifier parSpec);

BOOLN	PrintPars_ANSpikeGen_Carney(void);

void	ResetProcess_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	RunModel_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	SetDiagnosticMode_ANSpikeGen_Carney(WChar * theDiagnosticMode);

BOOLN	SetDischargeCoeffC0_ANSpikeGen_Carney(Float theDischargeCoeffC0);

BOOLN	SetDischargeCoeffC1_ANSpikeGen_Carney(Float theDischargeCoeffC1);

BOOLN	SetDischargeTConstS0_ANSpikeGen_Carney(Float theDischargeTConstS0);

BOOLN	SetDischargeTConstS1_ANSpikeGen_Carney(Float theDischargeTConstS1);

BOOLN	SetDistribution_ANSpikeGen_Carney(ParArrayPtr theDistribution);

BOOLN	SetInputMode_ANSpikeGen_Carney(WChar * theInputMode);

BOOLN	SetMaxThreshold_ANSpikeGen_Carney(Float theMaxThreshold);

BOOLN	InitModule_ANSpikeGen_Carney(ModulePtr theModule);

BOOLN	SetNumFibres_ANSpikeGen_Carney(int theNumFibres);

BOOLN	SetOutputMode_ANSpikeGen_Carney(WChar * theOutputMode);

BOOLN	SetParsPointer_ANSpikeGen_Carney(ModulePtr theModule);

BOOLN	SetPulseDurationCoeff_ANSpikeGen_Carney(Float thePulseDurationCoeff);

BOOLN	SetPulseMagnitude_ANSpikeGen_Carney(Float thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Carney(long theRanSeed);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Carney(Float theRefractoryPeriod);

BOOLN	SetTimeToPeak_ANSpikeGen_Carney(Float theTimeToPeak);

BOOLN	SetUniParList_ANSpikeGen_Carney(void);

__END_DECLS

#endif
