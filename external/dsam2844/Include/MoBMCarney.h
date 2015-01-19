/**********************
 *
 * File:		MoBMCarney.h
 * Purpose:		Laurel H. Carney basilar membrane module: Carney L. H. (1993)
 *				"A model for the responses of low-frequency auditory-nerve
 *				fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				21-03-97 LPO: Corrected binaural processing bug.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		12 Mar 1996
 * Updated:		21 Mar 1997
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

#ifndef _MOBMCARNEY_H
#define _MOBMCARNEY_H 1

#include "UtCmplxM.h"
#include "UtBandwidth.h"
#include "UtCFList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BASILARM_CARNEY_NUM_PARS			6

/*
 * Use original fit for Tl (latency vs. CF in msec) from Carney & Yin '88
 *and then correct by .75 cyles to go from PEAK delay to ONSET delay
 */

#define	BM_CARNEY_SS0	6.0e-3		/* mm */
#define	BM_CARNEY_CC0	1.1e-3		/* ms */
#define	BM_CARNEY_SS1	2.2e-3		/* mm */
#define	BM_CARNEY_CC1	1.1e-3		/* ms */

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define	BM_CARNEY_A(C, TAU0)	(1.0 / (1.0 + (C) * ((TAU0) * 3.0 / 2.0)))

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BASILARM_CARNEY_CASCADE,
	BASILARM_CARNEY_CUTOFFFREQUENCY,
	BASILARM_CARNEY_HCOPERATINGPOINT,
	BASILARM_CARNEY_ASYMMETRICALBIAS,
	BASILARM_CARNEY_MAXHCVOLTAGE,
	BASILARM_CARNEY_CFLIST

} BMCarneyParSpecifier;

typedef struct {

	Float	x;
	Float	tau0;
	Complex	*fLast;
	Float	oHCLast;
	Float	oHCTempLast;

} CarneyGTCoeffs, *CarneyGTCoeffsPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;

	int		cascade;
	Float	cutOffFrequency;
	Float	hCOperatingPoint;
	Float	asymmetricalBias;
	Float	maxHCVoltage;
	CFListPtr	cFList;

	/* Private members */
	UniParListPtr	parList;
	int		numChannels, numComplexCoeffs, numThreads;
	Float	c, aA, c1LP, c2LP, pix2xDt;
	ComplexPtr *f;
	CarneyGTCoeffsPtr	*coefficients;

} BMCarney, *BMCarneyPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMCarneyPtr	bMCarneyPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_BasilarM_Carney(EarObjectPtr data);

BOOLN	Free_BasilarM_Carney(void);

void	FreeCarneyGTCoeffs_BasilarM_Carney(CarneyGTCoeffsPtr *p);

BOOLN	FreeProcessVariables_BasilarM_Carney(void);

CFListPtr	GetCFListPtr_BasilarM_Carney(void);

UniParListPtr	GetUniParListPtr_BasilarM_Carney(void);

BOOLN	Init_BasilarM_Carney(ParameterSpecifier parSpec);

CarneyGTCoeffsPtr	InitCarneyGTCoeffs_BasilarM_Carney(int cascade, Float cF);

BOOLN	InitProcessVariables_BasilarM_Carney(EarObjectPtr data);

BOOLN	PrintPars_BasilarM_Carney(void);

BOOLN	RunModel_BasilarM_Carney(EarObjectPtr data);

BOOLN	SetAsymmetricalBias_BasilarM_Carney(Float theAsymmetricalBias);

BOOLN	SetCFList_BasilarM_Carney(CFListPtr theCFList);

BOOLN	SetCutOffFrequency_BasilarM_Carney(Float theCutOffFrequency);

BOOLN	SetMaxHCVoltage_BasilarM_Carney(Float theMaxHCVoltage);

BOOLN	SetHCOperatingPoint_BasilarM_Carney(Float theHCOperatingPoint);

BOOLN	SetCascade_BasilarM_Carney(int theCascade);

BOOLN	InitModule_BasilarM_Carney(ModulePtr theModule);

BOOLN	SetParsPointer_BasilarM_Carney(ModulePtr theModule);

BOOLN	SetUniParList_BasilarM_Carney(void);

__END_DECLS

#endif
