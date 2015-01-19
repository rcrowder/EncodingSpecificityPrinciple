/**********************
 *
 * File:		UtTranspose.h
 * File:		UtTranspose.c
 * Purpose:		This module takes an input signal and converts the n channels
 *				of m samples to m channels of n samples.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 *				02-03-05 LPO: This module is not being thread enabled as the
 *				number of input channels defines the channel processing.
 * Author:		L. P. O'Mard
 * Created:		13 Jul 2004
 * Updated:
 * Copyright:	(c) 2005, 2010 Lowel P. O'Mard
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

#ifndef _UTTRANSPOSE_H
#define _UTTRANSPOSE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_TRANSPOSE_NUM_PARS			1
#define	UTILITY_TRANSPOSE_MOD_NAME			wxT("UTIL_TRANSPOSE")

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_TRANSPOSE_MODE

} TransposeParSpecifier;

typedef enum {

	UTILITY_TRANSPOSE_STANDARD_MODE,
	UTILITY_TRANSPOSE_FIRST_CHANNEL_MODE,
	UTILITY_TRANSPOSE_MODE_NULL

} TransposeModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;

	/* Private members */
	UniParListPtr	parList;

} Transpose, *TransposePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	TransposePtr	transposePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Utility_Transpose(EarObjectPtr data);

BOOLN	Free_Utility_Transpose(void);

UniParListPtr	GetUniParListPtr_Utility_Transpose(void);

NameSpecifier *	ModeList_Utility_Transpose(int index);

BOOLN	InitModule_Utility_Transpose(ModulePtr theModule);

BOOLN	Init_Utility_Transpose(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_Transpose(void);

BOOLN	Process_Utility_Transpose(EarObjectPtr data);

BOOLN	SetMode_Utility_Transpose(WChar * theMode);

BOOLN	SetParsPointer_Utility_Transpose(ModulePtr theModule);

BOOLN	SetUniParList_Utility_Transpose(void);

__END_DECLS

#endif
