/**********************
 *
 * File:		UtMathOp.h
 * Purpose:		This utility carries out simple mathematical operations.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 * Author:		L.  P. O'Mard
 * Created:		08 Jul 2002
 * Updated:
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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

#ifndef _UTMATHOP_H
#define _UTMATHOP_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_MATHOP_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_MATHOP_OPERATORMODE,
	UTILITY_MATHOP_OPERAND

} MathOpParSpecifier;

typedef enum {

	UTILITY_MATHOP_OPERATORMODE_ADD,
	UTILITY_MATHOP_OPERATORMODE_ABSOLUTE,
	UTILITY_MATHOP_OPERATORMODE_OFFSET,
	UTILITY_MATHOP_OPERATORMODE_SCALE,
	UTILITY_MATHOP_OPERATORMODE_SQR,
	UTILITY_MATHOP_OPERATORMODE_SUBTRACT,
	UTILITY_MATHOP_OPERATORMODE_NULL

} UtilityOperatorModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		operatorMode;
	Float	operand;

	/* Private members */
	NameSpecifier	*operatorModeList;
	UniParListPtr	parList;

} MathOp, *MathOpPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	MathOpPtr	mathOpPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Utility_MathOp(EarObjectPtr data);

BOOLN	Free_Utility_MathOp(void);

UniParListPtr	GetUniParListPtr_Utility_MathOp(void);

BOOLN	InitModule_Utility_MathOp(ModulePtr theModule);

BOOLN	InitOperatorModeList_Utility_MathOp(void);

BOOLN	Init_Utility_MathOp(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_MathOp(void);

BOOLN	Process_Utility_MathOp(EarObjectPtr data);

BOOLN	SetOperand_Utility_MathOp(Float theOperand);

BOOLN	SetOperatorMode_Utility_MathOp(WChar * theOperatorMode);

BOOLN	SetParsPointer_Utility_MathOp(ModulePtr theModule);

BOOLN	SetUniParList_Utility_MathOp(void);

__END_DECLS

#endif
