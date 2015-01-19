/*******
 *
 * File:		MoIHCCooke.h
 * Purpose: 	Header file for the Cooke 1991 hair cell.
 * Comments:
 * Authors:		M. Cooke, L.P.O'Mard.
 * Created:		21 May 1995
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

#ifndef	_MOHCCOOKE_H
#define _MOHCCOOKE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define IHC_COOKE91_NUM_PARS			5

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IHC_COOKE91_CRAWFORDCONST,
	IHC_COOKE91_RELEASEFRACTION,
	IHC_COOKE91_REFILLFRACTION,
	IHC_COOKE91_SPONTRATE,
	IHC_COOKE91_MAXSPIKERATE

} CookeHCParSpecifier;

typedef struct {

	Float	vimm;
	Float	vrel;
	Float	crel;
	Float	vres;
	Float	cimm;

} CookeHCVars, *CookeHCVarsPtr;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;

	Float	crawfordConst;		/* Crawford and Fettiplace c Value */
	Float	releaseFraction;	/* Release fraction. */
	Float	refillFraction;		/* Replenishment fraction. */
	Float	spontRate;			/* desired spontaneous firing rate. */
	Float	maxSpikeRate;		/* maximum possible firing rate. */

	/* Private members */
	UniParListPtr	parList;
	Float	vmin, k, l, rateScale;
	CookeHCVars	*hCChannels;

} CookeHC, *CookeHCPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	CookeHCPtr	cookeHCPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHC_Cooke91(EarObjectPtr data);

BOOLN	Free_IHC_Cooke91(void);

void	FreeProcessVariables_IHC_Cooke91(void);

UniParListPtr	GetUniParListPtr_IHC_Cooke91(void);

BOOLN	Init_IHC_Cooke91(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHC_Cooke91(EarObjectPtr data);

BOOLN	PrintPars_IHC_Cooke91(void);

BOOLN	RunModel_IHC_Cooke91(EarObjectPtr data);

BOOLN	SetCrawfordConst_IHC_Cooke91(Float theCrawfordConst);

BOOLN	SetMaxSpikeRate_IHC_Cooke91(Float theMaxSpikeRate);

BOOLN	InitModule_IHC_Cooke91(ModulePtr theModule);

BOOLN	SetParsPointer_IHC_Cooke91(ModulePtr theModule);

BOOLN	SetRefillFraction_IHC_Cooke91(Float theRefillFraction);

BOOLN	SetReleaseFraction_IHC_Cooke91(Float theReleaseFraction);

BOOLN	SetSpontRate_IHC_Cooke91(Float theRecoveryRate);

BOOLN	SetUniParList_IHC_Cooke91(void);

__END_DECLS

#endif
