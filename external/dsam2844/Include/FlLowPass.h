/**********************
 *
 * File:		FlLowPass.h
 * Purpose:		This is an implementation of a 1st order Butterworth low-pass
 *				filter.  The filter is used to approximate the effects of
 *				dendrite filtering.
 * Comments:	It uses the filter IIR filter from the UtFilter module.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		03 Dec 1996
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

#ifndef	_FLLOWPASS_H
#define _FLLOWPASS_H	1

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define FILTER_LOW_PASS_NUM_PARS			3
#define FILTER_LOW_PASS_MOD_NAME			wxT("FILT_LOWPASS")

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	FILTER_LOW_PASS_MODE,
	FILTER_LOW_PASS_CUTOFFFREQUENCY,
	FILTER_LOW_PASS_SIGNALMULTIPLIER

} LowPassFParSpecifier;

typedef	enum {

	FILTER_LOW_PASS_MODE_NORMAL,
	FILTER_LOW_PASS_MODE_SCALED,
	FILTER_LOW_PASS_MODE_NULL

} LowPassFModeSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	modeFlag, cutOffFrequencyFlag, signalMultiplierFlag;
	BOOLN	updateProcessVariablesFlag;
	int		mode;
	Float	cutOffFrequency;
	Float	signalMultiplier;	/* potential mV */

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	ContButt1CoeffsPtr	*coefficients;
	NameSpecifier *modeList;

} LowPassF, *LowPassFPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	LowPassFPtr	lowPassFPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_Filter_LowPass(void);

BOOLN	Free_Filter_LowPass(void);

void	FreeProcessVariables_Filter_LowPass(void);

UniParListPtr	GetUniParListPtr_Filter_LowPass(void);

BOOLN	Init_Filter_LowPass(ParameterSpecifier parSpec);

BOOLN	InitModeList_Filter_LowPass(void);

BOOLN	InitProcessVariables_Filter_LowPass(EarObjectPtr data);

BOOLN	PrintPars_Filter_LowPass(void);

BOOLN	RunProcess_Filter_LowPass(EarObjectPtr data);

BOOLN	SetCutOffFrequency_Filter_LowPass(Float theCutOffFrequency);

BOOLN	SetMode_Filter_LowPass(WChar *theMode);

BOOLN	InitModule_Filter_LowPass(ModulePtr theModule);

BOOLN	SetParsPointer_Filter_LowPass(ModulePtr theModule);

BOOLN	SetSignalMultiplier_Filter_LowPass(Float theUpperCutOffFreq);

BOOLN	SetUniParList_Filter_LowPass(void);

__END_DECLS

#endif
