/**********************
 *
 * File:		UtConvMonaural.h
 * Purpose:		This module converts a binaural signal to a monaural signal.
 * Comments:	Written using ModuleProducer version 1.10 (Jan  3 1997).
 * Author:		L. P. O'Mard
 * Convd:		5 Mar 1997
 * Updated:
 * Copyright:	(c) 1997, 2010 Lowel P. O'Mard
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

#ifndef _UTCONVMONAURAL_H
#define _UTCONVMONAURAL_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_CONVMONAURAL_NUM_PARS			1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_CONVMONAURAL_MODE

} CMonauralParSpecifier;

typedef enum {

	UTILITY_CONVMONAURAL_MODE_ADD,
	UTILITY_CONVMONAURAL_MODE_LEFT,
	UTILITY_CONVMONAURAL_MODE_RIGHT,
	UTILITY_CONVMONAURAL_MODE_NULL

} UtilityConvMonauralModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;

} CMonaural, *CMonauralPtr;

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

BOOLN	Free_Utility_ConvMonaural(void);

UniParListPtr	GetUniParListPtr_Utility_ConvMonaural(void);

BOOLN	InitModeList_Utility_ConvMonaural(void);

BOOLN	CheckData_Utility_ConvMonaural(EarObjectPtr data);

BOOLN	InitModule_Utility_ConvMonaural(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_ConvMonaural(ModulePtr theModule);

BOOLN	Init_Utility_ConvMonaural(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_ConvMonaural(void);

BOOLN	Process_Utility_ConvMonaural(EarObjectPtr data);

BOOLN	SetMode_Utility_ConvMonaural(WChar * theMode);

BOOLN	SetParsPointer_Utility_ConvMonaural(ModulePtr theModule);

BOOLN	SetUniParList_Utility_ConvMonaural(void);

__END_DECLS

#endif
