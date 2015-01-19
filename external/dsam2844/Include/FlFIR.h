/**********************
 *
 * File:		FlFIR.h
 * Purpose:		This routine uses the Remez exchange algorithm to calculate a
 *				Finite Impulse Response (FIR) filter which is then run as a
 *				process.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		L. P. O'Mard.
 * Created:		04 Dec 2000
 * Updated:
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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

#ifndef _FLFIR_H
#define _FLFIR_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define FILTER_FIR_NUM_PARS			7

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	FILTER_FIR_DIAGNOSTICMODE,
	FILTER_FIR_TYPE,
	FILTER_FIR_NUMTAPS,
	FILTER_FIR_NUMBANDS,
	FILTER_FIR_BANDFREQS,
	FILTER_FIR_DESIRED,
	FILTER_FIR_WEIGHTS

} FIRParSpecifier;

typedef enum {

	FILTER_FIR_ARBITRARY_TYPE,
	FILTER_FIR_BANDPASS_TYPE,
	FILTER_FIR_DIFFERENTIATOR_TYPE,
	FILTER_FIR_HILBERT_TYPE,
	FILTER_FIR_USER_TYPE,
	FILTER_FIR_TYPE_NULL

} FilterTypeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		type;
	int		numTaps;
	int		numBands;
	Float	*bandFreqs, *desired, *weights;

	/* Private members */
	NameSpecifier	*typeList;
	UniParListPtr	parList;

	FIRCoeffsPtr	coeffs;

} FIR, *FIRPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FIRPtr	fIRPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	AllocNumBands_Filter_FIR(int numBands);

BOOLN	CheckData_Filter_FIR(EarObjectPtr data);

void	FreeProcessVariables_Filter_FIR(void);

BOOLN	Free_Filter_FIR(void);

int		GetType_Filter_FIR(void);

UniParListPtr	GetUniParListPtr_Filter_FIR(void);

BOOLN	InitProcessVariables_Filter_FIR(EarObjectPtr data);

BOOLN	InitTypeList_Filter_FIR(void);

BOOLN	Init_Filter_FIR(ParameterSpecifier parSpec);

BOOLN	PrintPars_Filter_FIR(void);

BOOLN	RunProcess_Filter_FIR(EarObjectPtr data);

BOOLN	SetBandFreqs_Filter_FIR(Float *theBandFreqs);

BOOLN	SetDesired_Filter_FIR(Float *theDesired);

BOOLN	SetDiagnosticMode_Filter_FIR(WChar * theDiagnosticMode);

BOOLN	SetIndividualBand_Filter_FIR(int theIndex, Float theBand);

BOOLN	SetIndividualDesired_Filter_FIR(int theIndex, Float theDesired);

BOOLN	SetIndividualWeight_Filter_FIR(int theIndex, Float theWeight);

BOOLN	InitModule_Filter_FIR(ModulePtr theModule);

BOOLN	SetNumBands_Filter_FIR(int theNumBands);

BOOLN	SetNumTaps_Filter_FIR(int theNumTaps);

BOOLN	SetParsPointer_Filter_FIR(ModulePtr theModule);

BOOLN	SetType_Filter_FIR(WChar * theType);

BOOLN	SetUniParList_Filter_FIR(void);

BOOLN	SetWeights_Filter_FIR(Float *theWeights);

__END_DECLS

#endif
