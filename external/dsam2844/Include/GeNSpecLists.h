/**********************
 *
 * File:		GeNSpecLists.h
 * Purpose:		This module contains the common specifier lists used by various
 *				modules.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		26 Nov 1997
 * Updated:
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

#ifndef _GENSPECLISTS_H
#define _GENSPECLISTS_H 1

#include "UtNameSpecs.h"
#include "UtRandom.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef	enum {

	GENERAL_BOOLEAN_OFF = 0,
	GENERAL_BOOLEAN_ON,
	GENERAL_BOOLEAN_NULL

} GeneralBooleanSpecifier;

typedef	enum {

	GENERAL_DIAGNOSTIC_OFF_MODE,
	GENERAL_DIAGNOSTIC_SCREEN_MODE,
	GENERAL_DIAGNOSTIC_ERROR_MODE,
	GENERAL_DIAGNOSTIC_FILE_MODE,
	GENERAL_DIAGNOSTIC_MODE_NULL

} GeneralDiagnosticModeSpecifier;

typedef enum {

	GENERAL_FIT_FUNC_EXP1_MODE,
	GENERAL_FIT_FUNC_LINEAR1_MODE,
	GENERAL_FIT_FUNC_LOG1_MODE,
	GENERAL_FIT_FUNC_LOG2_MODE,
	GENERAL_FIT_FUNC_POLY1_MODE,
	GENERAL_FIT_FUNC_NULL

} GeneralFitFuncModeSpecifier;

typedef enum {

	GENERAL_PHASE_RANDOM,
	GENERAL_PHASE_SINE,
	GENERAL_PHASE_COSINE,
	GENERAL_PHASE_ALTERNATING,
	GENERAL_PHASE_SCHROEDER,
	GENERAL_PHASE_PLACK_AND_WHITE,
	GENERAL_PHASE_USER,
	GENERAL_PHASE_NULL

} GeneralPhaseModeSpecifier;

typedef enum {

	GENERAL_EAR_LEFT,
	GENERAL_EAR_RIGHT,
	GENERAL_EAR_BOTH,
	GENERAL_EAR_NULL

} GeneralEarModeSpecifier;

typedef enum {

	GENERAL_SPACINGMODE_ERB,
	GENERAL_SPACINGMODE_LINEAR,
	GENERAL_SPACINGMODE_OCTAVE,
	GENERAL_SPACINGMODE_NULL

} GeneralSpacingModeSpecifier;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DSAM_API NameSpecifier *	BooleanList_NSpecLists(int index);

void			CloseDiagnostics_NSpecLists(FILE **fp);

NameSpecifier *	DiagModeList_NSpecLists(int index);

NameSpecifier * EarModeList_NSpecLists(int index);

NameSpecifier *	FitFuncModeList_NSpecLists(int index);

int		GetNumListEntries_NSpecLists(NameSpecifierPtr list);

int		IdentifyDiag_NSpecLists(WChar *mode, NameSpecifierPtr list);

NameSpecifier *	InitNameList_NSpecLists(NameSpecifierPtr prototypeList,
				  WChar *textPtr);

BOOLN	OpenDiagnostics_NSpecLists(FILE **fp, NameSpecifierPtr list, int mode);

NameSpecifier *	PhaseModeList_NSpecLists(int index);

void	SetPhaseArray_NSpecLists(Float *phase, long *ranSeed,
		  RandParsPtr randPars, int phaseMode, Float phaseVariable,
		  int lowestHarmonic, int numHarmonics);

NameSpecifier *	SpacingModeList_NSpecLists(int index);

__END_DECLS

#endif
