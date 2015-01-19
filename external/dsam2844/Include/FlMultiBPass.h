/**********************
 *
 * File:		FlMultiBPass.h
 * Purpose:		This is an implementation of an outer-/middle-ear pre-emphasis
 *				filter using the filters.c module.
 * Comments:	Written using ModuleProducer version 1.2.2 (Nov 11 1998).
 *				A parallel, multiple band-pass filter is used to model the
 *				effects of the outer- and middle-ear on an input signal.
 *				The first data EarObject in the BPass is the process EarObject.
 *				12-11-98 LPO: I have changed this module to use arrays rather
 *				than a data structure.  This change makes it easier to do the
 *				universal parameter implementation.
 * Author:		L. P. O'Mard
 * Created:		20 Jul 1998
 * Updated:		12 Nov 1998
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

#ifndef _FLMULTIBPASS_H
#define _FLMULTIBPASS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define MULTIBPASS_NUM_PARS			5

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	MULTIBPASS_NUMFILTERS,
	MULTIBPASS_CASCADE,
	MULTIBPASS_GAIN,
	MULTIBPASS_LOWERCUTOFFFREQ,
	MULTIBPASS_UPPERCUFOFFFREQ

} MultiBPassFParSpecifier;

typedef struct {

	BandPassCoeffsPtr	*coefficients;
	EarObjectPtr		data;

} BPassPars, *BPassParsPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		numFilters;
	int		*cascade;
	Float	*gain;
	Float	*upperCutOffFreq;
	Float	*lowerCutOffFreq;

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	BPassPars	*bPassPars;

} MultiBPassF, *MultiBPassFPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	MultiBPassFPtr	multiBPassFPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	AllocNumFilters_Filter_MultiBPass(int numFilters);

BOOLN	CheckData_Filter_MultiBPass(EarObjectPtr data);

BOOLN	Free_Filter_MultiBPass(void);

void	FreeProcessVariables_Filter_MultiBPass(void);

UniParListPtr	GetUniParListPtr_Filter_MultiBPass(void);

BOOLN	Init_Filter_MultiBPass(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Filter_MultiBPass(EarObjectPtr data);

BOOLN	PrintPars_Filter_MultiBPass(void);

BOOLN	RunModel_Filter_MultiBPass(EarObjectPtr data);

BOOLN	SetDefaultNumFiltersArrays_Filter_MultiBPass(void);

BOOLN	SetIndividualLowerCutOffFreq_Filter_MultiBPass(int theIndex,
		  Float theLowerCutOffFreq);

BOOLN	SetIndividualCascade_Filter_MultiBPass(int theIndex, int theCascade);

BOOLN	SetIndividualGain_Filter_MultiBPass(int theIndex,
		  Float theGain);

BOOLN	SetIndividualUpperCutOffFreq_Filter_MultiBPass(int theIndex,
		  Float theUpperCutOffFreq);

BOOLN	SetLowerCutOffFreq_Filter_MultiBPass(Float *theLowerCutOffFreq);

BOOLN	InitModule_Filter_MultiBPass(ModulePtr theModule);

BOOLN	SetNumFilters_Filter_MultiBPass(int theNumFilters);

BOOLN	SetCascade_Filter_MultiBPass(int *theCascade);

BOOLN	SetParsPointer_Filter_MultiBPass(ModulePtr theModule);

BOOLN	SetGain_Filter_MultiBPass(Float *theGain);

BOOLN	SetUniParList_Filter_MultiBPass(void);

BOOLN	SetUpperCutOffFreq_Filter_MultiBPass(Float *theUpperCutOffFreq);

__END_DECLS

#endif
