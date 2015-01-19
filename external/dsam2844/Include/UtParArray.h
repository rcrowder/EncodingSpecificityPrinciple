/**********************
 *
 * File:		UtParArray.h
 * Purpose:		This Module contains the parameter array structure, where the
 *				length of the array is defined by the mode.
 * Comments:	Originally created for use in the MoDRNL filter.
 * 				Any parameter with a zero number of parameter values will be assumed
 * 				to consist of a variable list of floats.  The size of the array is
 * 				defined dynamically according to the highest index.
 * Author:		L. P. O'Mard
 * Created:		01 Sep 2000
 * Updated:
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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

#ifndef UTPARARRAY_H
#define	UTPARARRAY_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PARARRAY_NUM_PARS			2

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PARARRAY_MODE,
	PARARRAY_PARAMETER,
	PARARRAY_NULL

} ParArrayParSpecifier;

typedef struct _ParArray {

	BOOLN	updateFlag;
	WChar	name[MAXLINE];
	int		mode;
	int		numParams;
	int		varNumParams;
	Float	*params;
	NameSpecifier	*modeList;

	int (* GetNumPars)(int);
	BOOLN	(* CheckPars)(struct _ParArray *p, SignalDataPtr);

	UniParListPtr	parList;
	WChar	desc[PARARRAY_NUM_PARS][MAXLINE];
	WChar	abbr[PARARRAY_NUM_PARS][MAXLINE];

} ParArray, *ParArrayPtr;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckInit_ParArray(ParArrayPtr parArray, const WChar *callingFunction);

BOOLN	CheckPars_ParArray(ParArrayPtr parArray, SignalDataPtr signal);

void	Free_ParArray(ParArrayPtr *parArray);

ParArrayPtr	Init_ParArray(WChar *name, NameSpecifier *modeList,
			  int (* GetNumPars)(int), BOOLN (* CheckPars)(ParArrayPtr,
			  SignalDataPtr));

void	PrintPars_ParArray(ParArrayPtr parArray);

BOOLN	SetIndividualPar_ParArray(ParArrayPtr parArray, int theIndex,
		  Float parValue);

BOOLN	SetMode_ParArray(ParArrayPtr parArray, const WChar *modeName);

BOOLN	SetUniParList_ParArray(ParArrayPtr parArray);

__END_DECLS

#endif
