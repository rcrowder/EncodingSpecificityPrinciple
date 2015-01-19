/**********************
 *
 * File:		MoHCRPZhang.h
 * Purpose:		Incorporates the Zhang et al. 'ihc' receptor potential code.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		01 Aug 2002
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

#ifndef _MOHCRPZHANG_H
#define _MOHCRPZHANG_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHCRP_ZHANG_NUM_PARS			6

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHCRP_ZHANG_A0,
	IHCRP_ZHANG_B,
	IHCRP_ZHANG_C,
	IHCRP_ZHANG_D,
	IHCRP_ZHANG_CUT,
	IHCRP_ZHANG_K

} IHCRPZhangParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	a0;
	Float	b;
	Float	c;
	Float	d;
	Float	cut;
	int		k;

	/* Private members */
	UniParListPtr	parList;
	THairCell *hCRP;

} IHCRPZhang, *IHCRPZhangPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	IHCRPZhangPtr	iHCRPZhangPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IHCRP_Zhang(EarObjectPtr data);

BOOLN	FreeProcessVariables_IHCRP_Zhang(void);

BOOLN	Free_IHCRP_Zhang(void);

UniParListPtr	GetUniParListPtr_IHCRP_Zhang(void);

BOOLN	InitModule_IHCRP_Zhang(ModulePtr theModule);

BOOLN	InitProcessVariables_IHCRP_Zhang(EarObjectPtr data);

BOOLN	Init_IHCRP_Zhang(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHCRP_Zhang(void);

BOOLN	RunModel_IHCRP_Zhang(EarObjectPtr data);

BOOLN	SetA0_IHCRP_Zhang(Float theA0);

BOOLN	SetB_IHCRP_Zhang(Float theB);

BOOLN	SetC_IHCRP_Zhang(Float theC);

BOOLN	SetCut_IHCRP_Zhang(Float theCut);

BOOLN	SetD_IHCRP_Zhang(Float theD);

BOOLN	SetK_IHCRP_Zhang(int theK);

BOOLN	SetParsPointer_IHCRP_Zhang(ModulePtr theModule);

BOOLN	SetUniParList_IHCRP_Zhang(void);

__END_DECLS

#endif
