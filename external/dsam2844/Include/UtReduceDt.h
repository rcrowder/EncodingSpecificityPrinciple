/**********************
 *
 * File:		UtReduceDt.h
 * Purpose:		This routine produces an output signal whose sampling rate is
 *				increased by reducing the sampling interval of the input signal
 *				by a specified denominator.
 * Comments:	Written using ModuleProducer version 1.8.
 *				The length of the signal will be increased accordingly.
 *				A new output signal is created, if required, or the old output
 *				signal is overwritten.
 * Author:
 * Created:		Dec 21 1995
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

#ifndef _UTREDUCEDT_H
#define _UTREDUCEDT_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_REDUCEDT_NUM_PARS			1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_REDUCEDT_DENOMINATOR

} ReduceDtParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		denominator;

	/* Private members */
	UniParListPtr	parList;

} ReduceDt, *ReduceDtPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ReduceDtPtr	reduceDtPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_ReduceDt(EarObjectPtr data);

BOOLN	Free_Utility_ReduceDt(void);

UniParListPtr	GetUniParListPtr_Utility_ReduceDt(void);

BOOLN	Init_Utility_ReduceDt(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_ReduceDt(void);

BOOLN	Process_Utility_ReduceDt(EarObjectPtr data);

void	ResetProcess_Utility_ReduceDt(EarObjectPtr data);

BOOLN	SetDenominator_Utility_ReduceDt(int theDenominator);

BOOLN	InitModule_Utility_ReduceDt(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_ReduceDt(ModulePtr theModule);

BOOLN	SetUniParList_Utility_ReduceDt(void);

__END_DECLS

#endif
