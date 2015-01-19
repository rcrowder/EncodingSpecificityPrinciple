/**********************
 *
 * File:		TrSetDBSPL.h
 * Purpose:		This program calculates a signal's current intensity then
 *				adjusts it to a specified intensity.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 *				The intensity is calculated starting from an offset position.
 *				23-04-98 LPO: This routine will only work once, until reset.
 *				This means that it will only work for the second segment in
 *				segmented mode.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 1997
 * Updated:		23 Apr 1998
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

#ifndef _TRSETDBSPL_H
#define _TRSETDBSPL_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define SETDBSPL_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	SETDBSPL_TIMEOFFSET,
	SETDBSPL_INTENSITY

} SetDBSPLParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	timeOffset;
	Float	intensity;

	/* Private members */
	UniParListPtr	parList;
	Float	scale;

} SetDBSPL, *SetDBSPLPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SetDBSPLPtr	setDBSPLPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Transform_SetDBSPL(EarObjectPtr data);

BOOLN	Free_Transform_SetDBSPL(void);

BOOLN	FreeProcessVariables_Transform_SetDBSPL(void);

UniParListPtr	GetUniParListPtr_Transform_SetDBSPL(void);

BOOLN	Init_Transform_SetDBSPL(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Transform_SetDBSPL(EarObjectPtr data);

BOOLN	PrintPars_Transform_SetDBSPL(void);

BOOLN	Process_Transform_SetDBSPL(EarObjectPtr data);

BOOLN	SetIntensity_Transform_SetDBSPL(Float theIntensity);

BOOLN	InitModule_Transform_SetDBSPL(ModulePtr theModule);

BOOLN	SetParsPointer_Transform_SetDBSPL(ModulePtr theModule);

BOOLN	SetTimeOffset_Transform_SetDBSPL(Float theTimeOffset);

BOOLN	SetUniParList_Transform_SetDBSPL(void);

__END_DECLS

#endif
