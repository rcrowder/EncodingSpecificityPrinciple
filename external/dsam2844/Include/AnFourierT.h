/**********************
 *
 * File:		AnFourierT.h
 * Purpose:		This module runs a basic Fourier Analysis.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		18-01-94
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

#ifndef	_ANFOURIERT_H
#define _ANFOURIERT_H	1

#include "UtCmplxM.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define ANALYSIS_FOURIERT_NUM_PARS			1

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_FOURIERT_OUTPUTMODE

} FourierTParSpecifier;

typedef enum {

	ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE,
	ANALYSIS_FOURIERT_PHASE_OUTPUTMODE,
	ANALYSIS_FOURIERT_COMPLEX_OUTPUTMODE,
	ANALYSIS_FOURIERT_DB_SPL_OUTPUTMODE,
	ANALYSIS_FOURIERT_OUTPUTMODE_NULL

} AnalysisOutputModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;
	BOOLN	updateProcessVariablesFlag;

	int		outputMode;

	/* Private members */
	NameSpecifier	*outputModeList;
	UniParListPtr	parList;
	int		numThreads;
	int		numOutChans;
	Float	dBSPLFactor;
	ChanLen	fTLength;
	ChanLen	arrayLen;
	ComplxPtr	*fT;
#	if HAVE_FFTW3
	DSAM_FFTW_NAME(plan)	plan;
#	endif

} FourierT, *FourierTPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FourierTPtr	fourierTPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_FourierT(EarObjectPtr data);

BOOLN	CheckData_Analysis_FourierT(EarObjectPtr data);

BOOLN	Free_Analysis_FourierT(void);

void	FreeProcessVariables_Analysis_FourierT(void);

UniParListPtr	GetUniParListPtr_Analysis_FourierT(void);

BOOLN	InitOutputModeList_Analysis_FourierT(void);

BOOLN	Init_Analysis_FourierT(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Analysis_FourierT(EarObjectPtr data);

BOOLN	PrintPars_Analysis_FourierT(void);

void	ResetProcess_Analysis_FourierT(EarObjectPtr data);

BOOLN	InitModule_Analysis_FourierT(ModulePtr theModule);

BOOLN	SetOutputMode_Analysis_FourierT(WChar * theOutputMode);

BOOLN	SetParsPointer_Analysis_FourierT(ModulePtr theModule);

BOOLN	SetUniParList_Analysis_FourierT(void);

__END_DECLS

#endif
