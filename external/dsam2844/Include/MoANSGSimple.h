/******************
 *
 * File:		MoANSGSimple.h
 * Purpose:		This module contains the model for the hair cell post-synaptic
 *				firing
 * Comments:	24-02-97 LPO: Amended to allow multiple fibres.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
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

#ifndef	_MOANSGSIMPLE_H
#define _MOANSGSIMPLE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define ANSPIKEGEN_SIMPLE_NUM_PARS	8
#define PS_REFRACTORY_PERIOD		1.0E-3		/* Default value in seconds. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_SIMPLE_DIAGNOSTICMODE,
	ANSPIKEGEN_SIMPLE_OUTPUTMODE,
	ANSPIKEGEN_SIMPLE_RANSEED,
	ANSPIKEGEN_SIMPLE_NUMFIBRES,
	ANSPIKEGEN_SIMPLE_PULSEDURATIONCOEFF,
	ANSPIKEGEN_SIMPLE_PULSEMAGNITUDE,
	ANSPIKEGEN_SIMPLE_REFRACTORYPERIOD,
	ANSPIKEGEN_SIMPLE_DISTRIBUTION

} SimpleSGParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		outputMode;
	long	ranSeed;			/* seed for the random number generator. */
	int		numFibres;
	Float	pulseDurationCoeff;	/* Defines duration applied to each pulse (s). */
	Float	pulseMagnitude;		/* Magnitude for each pulse (nA?). */
	Float	refractoryPeriod;	/* The time during which spikes cannot occur */
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

} SimpleSG, *SimpleSGPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	SimpleSGPtr		simpleSGPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_ANSpikeGen_Simple(EarObjectPtr data);

BOOLN	Init_ANSpikeGen_Simple(ParameterSpecifier parSpec);

BOOLN	InitModule_ANSpikeGen_Simple(ModulePtr theModule);

BOOLN	Free_ANSpikeGen_Simple(void);

void	FreeProcessVariables_ANSpikeGen_Simple(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Simple(void);

BOOLN	InitProcessVariables_ANSpikeGen_Simple(EarObjectPtr data);

BOOLN	PrintPars_ANSpikeGen_Simple(void);

void	ResetProcess_ANSpikeGen_Simple(EarObjectPtr data);

BOOLN	RunModel_ANSpikeGen_Simple(EarObjectPtr data);

BOOLN	SetDiagnosticMode_ANSpikeGen_Simple(WChar * theDiagnosticMode);

BOOLN	SetDistribution_ANSpikeGen_Simple(ParArrayPtr theDistribution);

BOOLN	SetNumFibres_ANSpikeGen_Simple(int theNumFibres);

BOOLN	SetOutputMode_ANSpikeGen_Simple(WChar * theOutputMode);

BOOLN	SetParsPointer_ANSpikeGen_Simple(ModulePtr theModule);

BOOLN	SetPulseDurationCoeff_ANSpikeGen_Simple(Float thePulseDurationCoeff);

BOOLN	SetPulseMagnitude_ANSpikeGen_Simple(Float thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Simple(long theRanSeed);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Simple(Float theRefractoryPeriod);

BOOLN	SetUniParList_ANSpikeGen_Simple(void);

__END_DECLS

#endif
