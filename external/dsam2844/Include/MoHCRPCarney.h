/**********************
 *
 * File:		MoHCRPCarney.h
 * Purpose:		Laurel H. Carney IHC receptor potential module: Carney L. H.
 *				(1993) "A model for the responses of low-frequency
 *				auditory-nerve fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				It expects to be used in conjuction with a BM module.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Copyright:	(c) 1996, 2010 Lowel P. O'Mard
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

#ifndef _MOHCRPCARNEY_H
#define _MOHCRPCARNEY_H 1

#include "UtBandwidth.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHCRP_CARNEY_NUM_PARS			7

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHCRP_CARNEY_CUTOFFFREQUENCY,
	IHCRP_CARNEY_HCOPERATINGPOINT,
	IHCRP_CARNEY_ASYMMETRICALBIAS,
	IHCRP_CARNEY_MAXHCVOLTAGE,
	IHCRP_CARNEY_WAVEDELAYCOEFF,
	IHCRP_CARNEY_WAVEDELAYLENGTH,
	IHCRP_CARNEY_REFERENCEPOT

} CarneyRPParSpecifier;

typedef struct {

	Float	x;
	ChanLen	numLastSamples;
	Float	waveLast;
	Float	waveTempLast;
	Float	iHCLast;
	Float	iHCTempLast;
	ChanData	*lastOutputSection, *lastOutputStore;

} CarneyRPCoeffs, *CarneyRPCoeffsPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;

	Float	cutOffFrequency;
	Float	hCOperatingPoint;
	Float	asymmetricalBias;
	Float	maxHCVoltage;
	Float	waveDelayCoeff;
	Float	waveDelayLength;
	Float	referencePot;		/* Reference potential */

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	Float	aA,c1LP, c2LP;
	CarneyRPCoeffsPtr	*coefficients;

} CarneyRP, *CarneyRPPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CarneyRPPtr	carneyRPPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHCRP_Carney(EarObjectPtr data);

BOOLN	Free_IHCRP_Carney(void);

void	FreeCarneyRPCoeffs_IHCRP_Carney(CarneyRPCoeffsPtr *p);

BOOLN	FreeProcessVariables_IHCRP_Carney(void);

UniParListPtr	GetUniParListPtr_IHCRP_Carney(void);

BOOLN	Init_IHCRP_Carney(ParameterSpecifier parSpec);

CarneyRPCoeffsPtr	InitCarneyRPCoeffs_IHCRP_Carney(Float cF, Float dt);

BOOLN	InitProcessVariables_IHCRP_Carney(EarObjectPtr data);

BOOLN	PrintPars_IHCRP_Carney(void);

BOOLN	RunModel_IHCRP_Carney(EarObjectPtr data);

BOOLN	SetAsymmetricalBias_IHCRP_Carney(Float theAsymmetricalBias);

BOOLN	SetCutOffFrequency_IHCRP_Carney(Float theCutOffFrequency);

BOOLN	SetHCOperatingPoint_IHCRP_Carney(Float theHCOperatingPoint);

BOOLN	SetMaxHCVoltage_IHCRP_Carney(Float theMaxHCVoltage);

BOOLN	InitModule_IHCRP_Carney(ModulePtr theModule);

BOOLN	SetParsPointer_IHCRP_Carney(ModulePtr theModule);

BOOLN	SetReferencePot_IHCRP_Carney(Float theReferencePot);

BOOLN	SetUniParList_IHCRP_Carney(void);

BOOLN	SetWaveDelayCoeff_IHCRP_Carney(Float theWaveDelayCoeff);

BOOLN	SetWaveDelayLength_IHCRP_Carney(Float theWaveDelayLength);

__END_DECLS

#endif
