/**********************
 *
 * File:		UtCollateSignals.h
 * Purpose:		This utility produces and output signal with a number of samples
 *				equal to the sum of the input signals.  All input signals
 *				will need to have the same number of channels, and this will define
 *				the number of output channels.
 * Comments:	Written using ModuleProducer version 1.5.0 (May  2 2007).
 * Author:		L. P. O'Mard
 * Created:		02 May 2007
 * Updated:
 * Copyright:	(c) 2007, 2010 Lowel P. O'Mard
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

#ifndef _UTCOLLATESIGNALS_H
#define _UTCOLLATESIGNALS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_COLLATESIGNALS_NUM_PARS			3
#define	UTILITY_COLLATESIGNALS_MOD_NAME			wxT("Util_CollateSignals")

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_COLLATESIGNALS_SUMMATIONMODE,
	UTILITY_COLLATESIGNALS_LABELMODE,
	UTILITY_COLLATESIGNALS_LABELS

} CollateSigsParSpecifier;

typedef enum {

	UTILITY_COLLATESIGNALS_SUMMATIONMODE_AVERAGE,
	UTILITY_COLLATESIGNALS_SUMMATIONMODE_SUM,
	UTILITY_COLLATESIGNALS_SUMMATIONMODE_NULL

} UtilitySummationModeSpecifier;

typedef enum {

	UTILITY_COLLATESIGNALS_LABELMODE_CHAN_INDEX,
	UTILITY_COLLATESIGNALS_LABELMODE_INPUT_LABELS,
	UTILITY_COLLATESIGNALS_LABELMODE_USER,
	UTILITY_COLLATESIGNALS_LABELMODE_NULL

} UtilityLabelModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		summationMode;
	int		labelMode;
	Float	*labels;

	/* Private members */
	NameSpecifier	*summationModeList;
	NameSpecifier	*labelModeList;
	UniParListPtr	parList;
	int		length;

} CollateSigs, *CollateSigsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CollateSigsPtr	collateSigsPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Utility_CollateSignals(EarObjectPtr data);

BOOLN	Free_Utility_CollateSignals(void);

UniParListPtr	GetUniParListPtr_Utility_CollateSignals(void);

BOOLN	InitLabelModeList_Utility_CollateSignals(void);

BOOLN	InitModule_Utility_CollateSignals(ModulePtr theModule);

BOOLN	InitSummationModeList_Utility_CollateSignals(void);

BOOLN	Init_Utility_CollateSignals(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_CollateSignals(void);

BOOLN	Process_Utility_CollateSignals(EarObjectPtr data);

void	SetEnabledState_Utility_CollateSignals(void);

BOOLN	SetIndividualLabel_Utility_CollateSignals(int theIndex, Float theLabel);

BOOLN	SetLabelMode_Utility_CollateSignals(WChar * theLabelMode);

BOOLN	SetLabels_Utility_CollateSignals(Float *theLabels);

BOOLN	SetParsPointer_Utility_CollateSignals(ModulePtr theModule);

BOOLN	SetSummationMode_Utility_CollateSignals(WChar * theSummationMode);

BOOLN	SetUniParList_Utility_CollateSignals(void);

__END_DECLS

#endif
