/**********************
 *
 * File:		MoBMCooke.h
 * Purpose: 	An implementation of Martin Cooke's model of the response of the
 *				Basilar Membrane.
 * Comments:	This version is an almost completely faithful reproduction of
 *				supplied code.
 *				26-01-97 LPO: named changed from MoBM0Cooke -> MoBMCooke.
 *				11-11-98 LPO: Implemented 'outputMode' as 'NameSpecifier'.
 * Author:		M.Cooke/L.P.O'Mard
 * Created:		1 May 1995
 * Updated:		11 Nov 1998
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

#ifndef	_MOBMCOOKE_H
#define _MOBMCOOKE_H	1

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant definitions *****************************/
/******************************************************************************/

#define BM_COOKE_NUM_PARS			3

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BM_COOKE_BROADENINGCOEFF,
	BM_COOKE_OUTPUTMODE,
	BM_COOKE_THECFS

} BM0CookeParSpecifier;

typedef enum {

	BM_COOKE_OUTPUTMODE_BM_DETAIL,
	BM_COOKE_OUTPUTMODE_POWER_SPEC,
	BM_COOKE_OUTPUTMODE_AMP_ENVELOPE,
	BM_COOKE_OUTPUTMODE_NULL

} BasilarMOutputModeSpecifier;

typedef enum {

	BMCOOKE0_BM_DETAIL,
	BMCOOKE0_POWER_SPEC,
	BMCOOKE0_AMP_ENVELOPE,
	BMCOOKE0_NULL

} OutputModeSpecifier;

typedef struct {

	Float	z;
	Float	p0, p1, p2, p3, p4;
	Float	q0, q1, q2, q3, q4;
	Float	u0;
	Float	v0;
	Float	gain;

} CookeCoeffs, *CookeCoeffsPtr;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN		updateProcessVariablesFlag;
	int			outputMode;
	Float		broadeningCoeff;	/* Bandwidth correction for filter. */
	CFListPtr	theCFs;		/* Pointer to centre frequency structure. */

	/* Private members */
	NameSpecifier	*outputModeList;
	UniParListPtr	parList;
	int				numChannels;
	unsigned long	intSampleRate;
	CookeCoeffsPtr	coefficients;
	Float			*sine;
	Float			*cosine;

} BM0Cooke, *BM0CookePtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	BM0CookePtr	bM0CookePtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

CFListPtr	GetCFListPtr_BasilarM_Cooke(void);

UniParListPtr	GetUniParListPtr_BasilarM_Cooke(void);

BOOLN	Init_BasilarM_Cooke(ParameterSpecifier parSpec);

BOOLN	InitOutputModeList_BasilarM_Cooke(void);

BOOLN	InitProcessVariables_BasilarM_Cooke(EarObjectPtr data);

BOOLN	Free_BasilarM_Cooke(void);

void	FreeProcessVariables_BasilarM_Cooke(void);

BOOLN	PrintPars_BasilarM_Cooke(void);

BOOLN	RunModel_BasilarM_Cooke(EarObjectPtr data);

BOOLN	SetCFList_BasilarM_Cooke(CFListPtr theCFList);

BOOLN	InitModule_BasilarM_Cooke(ModulePtr theModule);

BOOLN	SetOutputMode_BasilarM_Cooke(WChar *theOutputMode);

BOOLN	SetParsPointer_BasilarM_Cooke(ModulePtr theModule);

BOOLN	SetBroadeningCoeff_BasilarM_Cooke(Float theBroadeningCoeff);

BOOLN	SetUniParList_BasilarM_Cooke(void);

__END_DECLS

#endif
