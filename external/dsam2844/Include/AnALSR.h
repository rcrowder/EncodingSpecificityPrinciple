/**********************
 *
 * File:		AnALSR.h
 * Purpose:		This proces module calculates the average localised
 *				synchronised rate (ALSR).
 *				Yound E. D. and Sachs M. B. (1979) "Representation of steady-
 *				state vowels in the temporal aspects of the discharge patterns
 *				of populations of auditory-nerve fibers", J. Acoust. Soc. Am,
 *				Vol 66, pages 1381-1403.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 *				It expects multi-channel input from a period histogram
 * Author:		L. P. O'Mard
 * Created:		22 Jan 2002
 * Updated:
 * Copyright:	(c) 2010 Lowel P. O'Mard
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

#ifndef _ANALSR_H
#define _ANALSR_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_ALSR_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_ALSR_LOWERAVELIMIT,
	ANALYSIS_ALSR_UPPERAVELIMIT,
	ANALYSIS_ALSR_NORMALISE

} ALSRParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	lowerAveLimit;
	Float	upperAveLimit;
	Float	normalise;

	/* Private members */
	UniParListPtr	parList;
	EarObjectPtr	modulusFT;

} ALSR, *ALSRPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ALSRPtr	aLSRPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	Calc_Analysis_ALSR(EarObjectPtr data);

BOOLN	CheckData_Analysis_ALSR(EarObjectPtr data);

BOOLN	FreeProcessVariables_Analysis_ALSR(void);

BOOLN	Free_Analysis_ALSR(void);

UniParListPtr	GetUniParListPtr_Analysis_ALSR(void);

BOOLN	InitModule_Analysis_ALSR(ModulePtr theModule);

BOOLN	InitProcessVariables_Analysis_ALSR(EarObjectPtr data);

BOOLN	Init_Analysis_ALSR(ParameterSpecifier parSpec);

BOOLN	PrintPars_Analysis_ALSR(void);

void	ResetProcess_Analysis_ALSR(EarObjectPtr data);

BOOLN	SetLowerAveLimit_Analysis_ALSR(Float theLowerAveLimit);

BOOLN	SetNormalise_Analysis_ALSR(Float theNormalise);

BOOLN	SetParsPointer_Analysis_ALSR(ModulePtr theModule);

BOOLN	SetUniParList_Analysis_ALSR(void);

BOOLN	SetUpperAveLimit_Analysis_ALSR(Float theUpperAveLimit);

__END_DECLS

#endif
