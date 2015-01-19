/**********************
 *
 * File:		MoANSGMeddis02.h
 * Purpose:		Auditory nerve spike generation program, based on vessicle
 *				release with a refractory time an recovery period.
 * Comments:	Written using ModuleProducer version 1.4.0 (Oct 10 2002).
 * Author:		L. P. O'Mard from a matlab routine from R. Meddis
 * Created:		08 Nov 2002
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

#ifndef _MOANSGMEDDIS02_H
#define _MOANSGMEDDIS02_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANSPIKEGEN_MEDDIS02_NUM_PARS			9

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_MEDDIS02_DIAGNOSTICMODE,
	ANSPIKEGEN_MEDDIS02_OUTPUTMODE,
	ANSPIKEGEN_MEDDIS02_RANSEED,
	ANSPIKEGEN_MEDDIS02_NUMFIBRES,
	ANSPIKEGEN_MEDDIS02_PULSEDURATION_COEFF,
	ANSPIKEGEN_MEDDIS02_PULSEMAGNITUDE,
	ANSPIKEGEN_MEDDIS02_REFRACTORYPERIOD,
	ANSPIKEGEN_MEDDIS02_RECOVERYTAU,
	ANSPIKEGEN_MEDDIS02_DISTRIBUTION


} Meddis02SGParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		outputMode;
	long	ranSeed;
	int		numFibres;
	Float	pulseDurationCoeff;
	Float	pulseMagnitude;
	Float	refractoryPeriod;
	Float	recoveryTau;
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

} Meddis02SG, *Meddis02SGPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	Meddis02SGPtr	meddis02SGPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_ANSpikeGen_Meddis02(EarObjectPtr data);

BOOLN	FreeProcessVariables_ANSpikeGen_Meddis02(void);

BOOLN	Free_ANSpikeGen_Meddis02(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Meddis02(void);

BOOLN	InitModule_ANSpikeGen_Meddis02(ModulePtr theModule);

BOOLN	InitProcessVariables_ANSpikeGen_Meddis02(EarObjectPtr data);

BOOLN	Init_ANSpikeGen_Meddis02(ParameterSpecifier parSpec);

BOOLN	PrintPars_ANSpikeGen_Meddis02(void);

void	ResetProcess_ANSpikeGen_Meddis02(EarObjectPtr data);

BOOLN	RunModel_ANSpikeGen_Meddis02(EarObjectPtr data);

BOOLN	SetDiagnosticMode_ANSpikeGen_Meddis02(WChar * theDiagnosticMode);

BOOLN	SetDistribution_ANSpikeGen_Meddis02(ParArrayPtr theDistribution);

BOOLN	SetNumFibres_ANSpikeGen_Meddis02(int theNumFibres);

BOOLN	SetOutputMode_ANSpikeGen_Meddis02(WChar * theOutputMode);

BOOLN	SetParsPointer_ANSpikeGen_Meddis02(ModulePtr theModule);

BOOLN	SetPulseDurationCoeff_ANSpikeGen_Meddis02(Float thePulseDurationCoeff);

BOOLN	SetPulseMagnitude_ANSpikeGen_Meddis02(Float thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Meddis02(long theRanSeed);

BOOLN	SetRecoveryTau_ANSpikeGen_Meddis02(Float theRecoveryTau);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Meddis02(Float theRefractoryPeriod);

BOOLN	SetUniParList_ANSpikeGen_Meddis02(void);

__END_DECLS

#endif
