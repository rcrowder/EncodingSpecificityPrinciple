/**********************
 *
 * File:		UtIterRipple.h
 * Purpose:		This routine transforms a signal using the Iterated Ripple
 *				algorithm as described in: "A time domain description for the
 *				pitch strength of iterated rippled noide", W.A. Yost, R.
 * 				Patterson, S. Sheft. J.Acoust. Soc. Am. 999(2), February 1996
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				It generates two classes of Iterated Ripple signal:
 * 				IRSO Iterated Rippled Signal ADD-ORIGINAL
 * 				IRSS Iterated Rippled Signal ADD-SAME
 * Authors:		Almudena Eustaquio-Martin, L. P. O'Mard
 * Created:		04-JUN-1996.
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

#ifndef _UTITERRIPPLE_H
#define _UTITERRIPPLE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_ITERATEDRIPPLE_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_ITERATEDRIPPLE_NUMITERATIONS,
	UTILITY_ITERATEDRIPPLE_MODE,
	UTILITY_ITERATEDRIPPLE_DELAY,
	UTILITY_ITERATEDRIPPLE_SIGNALMULTIPLIER

} IterRippleParSpecifier;

typedef	enum {

	ITERRIPPLE_IRSO_MODE,
	ITERRIPPLE_IRSS_MODE,
	ITERRIPPLE_NULL

} IterRippleModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		numIterations;
	int		mode;
	Float	delay;
	Float	signalMultiplier;

	/* Private members */
	NameSpecifier *modeList;
	UniParListPtr	parList;

} IterRipple, *IterRipplePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	IterRipplePtr	iterRipplePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_IteratedRipple(EarObjectPtr data);

BOOLN	Free_Utility_IteratedRipple(void);

UniParListPtr	GetUniParListPtr_Utility_IteratedRipple(void);

BOOLN	Init_Utility_IteratedRipple(ParameterSpecifier parSpec);

BOOLN	InitModeList_Utility_IteratedRipple(void);

BOOLN	PrintPars_Utility_IteratedRipple(void);

BOOLN	Process_Utility_IteratedRipple(EarObjectPtr data);

BOOLN	SetDelay_Utility_IteratedRipple(Float theDelay);

BOOLN	SetSignalMultiplier_Utility_IteratedRipple(Float theSignalMultiplier);

BOOLN	SetMode_Utility_IteratedRipple(WChar *theMode);

BOOLN	InitModule_Utility_IteratedRipple(ModulePtr theModule);

BOOLN	SetNumIterations_Utility_IteratedRipple(int theNumIterations);

BOOLN	SetParsPointer_Utility_IteratedRipple(ModulePtr theModule);

BOOLN	SetUniParList_Utility_IteratedRipple(void);

__END_DECLS

#endif
