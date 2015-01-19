/**********************
 *
 * File:		MoANSGBinom.h
 * Purpose:		Uses spike probabilities to produce the output from a number of
 *				fibres, using a binomial approximation.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				This is an approximation, so it should only be used when theree
 *				are a large number of fibres.  If in doubt, test against the
 *				output from the "Simple" spike generation module.
 *				It uses the refractory adjustment model module.
 * Author:		L. P. O'Mard, revised from code by M. J. Hewitt
 * Created:		25 Jun 1996
 * Updated:		15 Sep 1996
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

#ifndef _MOANSGBINOM_H
#define _MOANSGBINOM_H 1

#include "UtRandom.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANSPIKEGEN_BINOM_NUM_PARS			7
#define ANSPIKEGEN_BINOM_NUM_SUB_PROCESSES	1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_REFRACTADJDATA

} AnSpikeGenBinomSubProcessSpecifier;

typedef enum {

	ANSPIKEGEN_BINOMIAL_DIAGNOSTICMODE,
	ANSPIKEGEN_BINOM_NUMFIBRES,
	ANSPIKEGEN_BINOM_RANSEED,
	ANSPIKEGEN_BINOM_PULSEDURATION,
	ANSPIKEGEN_BINOM_PULSEMAGNITUDE,
	ANSPIKEGEN_BINOM_REFRACTORYPERIOD,
	ANSPIKEGEN_BINOM_DISTRIBUTION

} BinomialSGParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		numFibres;
	long	ranSeed;
	Float	pulseDuration;
	Float	pulseMagnitude;
	Float	refractoryPeriod;
	ParArrayPtr	distribution;

	/* Private members */
	NameSpecifier	*diagnosticModeList;
	UniParListPtr	parList;
	WChar			diagFileName[MAX_FILE_PATH];
	FILE	*fp;
	Float	*lastOutput;
	ChanLen	*remainingPulseIndex, pulseDurationIndex;
	EarObjectPtr	refractAdjData;
	ANSGDistPtr	aNDist;

} BinomialSG, *BinomialSGPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BinomialSGPtr	binomialSGPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_ANSpikeGen_Binomial(EarObjectPtr data);

BOOLN	Free_ANSpikeGen_Binomial(void);

void	FreeProcessVariables_ANSpikeGen_Binomial(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Binomial(void);

BOOLN	Init_ANSpikeGen_Binomial(ParameterSpecifier parSpec);

BOOLN	InitModule_ANSpikeGen_Binomial(ModulePtr theModule);

BOOLN	InitProcessVariables_ANSpikeGen_Binomial(EarObjectPtr data);

BOOLN	PrintPars_ANSpikeGen_Binomial(void);

BOOLN	RunModel_ANSpikeGen_Binomial(EarObjectPtr data);

BOOLN	SetDiagnosticMode_ANSpikeGen_Binomial(WChar * theDiagnosticMode);

BOOLN	SetDistribution_ANSpikeGen_Binomial(ParArrayPtr theDistribution);

BOOLN	SetNumFibres_ANSpikeGen_Binomial(int theNumFibres);

BOOLN	SetParsPointer_ANSpikeGen_Binomial(ModulePtr theModule);

BOOLN	SetPulseDuration_ANSpikeGen_Binomial(Float thePulseDuration);

BOOLN	SetPulseMagnitude_ANSpikeGen_Binomial(Float thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Binomial(long theRanSeed);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Binomial(Float theRefractoryPeriod);

BOOLN	SetUniParList_ANSpikeGen_Binomial(void);

__END_DECLS

#endif
