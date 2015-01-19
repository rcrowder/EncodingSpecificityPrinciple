/**********************
 *
 * File:		TrGate.h
 * Purpose:		This is the signal ramp/Damp module transformation module.
 *				At changes the start or end envelope of a signal.
 *				Transformation modules do not create a signal, but change an
 *				existing one.  In order to preserve the processing line, the
 *				output signal is just pointed to the input signal.
 * Comments:	Written using ModuleProducer version 1.11 (Apr  9 1997).
 * 				Valid ramp functions are defined as a function which varies
 *				from 0 to 1 as a function of the ramp interval.
 *				The functions used must be defined in the format given below
 *				Impulse signals, such as clicks, should not be ramped: if a
 *				warning message is given, in this case ignore it.
 *				If a signal is ramped, then the ramp flag will be set.
 *				In segmented mode, this routine will only work for the first
 *				signal section.
 *				i.e. if the ramp interval has expired, then all subsequent
 *				segments will be marked as ramped.
 *				A duration = -1 denotes to the end of the signal.
 *				17-11-97 LPO: Changed name from TrRampOrDamp to TrGate and also
 *				introduced the Raised_Cos ramp mode.  This should be the ramp
 *				that we use by default.
 *				07-05-98 LPO: Corrected time offset usage in relative mode.
 * Author:		L. P. O'Mard
 * Created:		20 Aug 1997
 * Updated:		07 May 1998
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

#ifndef _TRGATE_H
#define _TRGATE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define GATE_NUM_PARS			6

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	GATE_POSITION_MODE,
	GATE_OPERATION_MODE,
	GATE_TYPE_MODE,
	GATE_TIME_OFFSET,
	GATE_DURATION,
	GATE_SLOPE_PARAMETER

} GateParSpecifier;

typedef enum {

	GATE_RAMP_OPERATION_MODE,
	GATE_DAMP_OPERATION_MODE,
	GATE_NULL_OPERATION_MODE

} GateOperationModeSpecifier;

typedef enum {

	GATE_ABSOLUTE_POSITION_MODE,
	GATE_RELATIVE_POSITION_MODE,
	GATE_NULL_POSITION_MODE

} GatePositionModeSpecifier;

typedef enum {

	GATE_LINEAR_TYPE_MODE,
	GATE_SINE_TYPE_MODE,
	GATE_RAISED_COS_TYPE_MODE,
	GATE_EXP_DECAY_TYPE_MODE,
	GATE_NULL_TYPE_MODE

} GateTypeModeSpecifier;

typedef enum {

	GATE_STANDARD_PROCESS_MODE,
	GATE_RAMP_PROCESS_MODE,
	GATE_DAMP_PROCESS_MODE,
	GATE_NULL_PROCESS_MODE

} GateProcessModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		positionMode;
	int		operationMode;
	int		typeMode;
	Float	timeOffset;
	Float	duration;
	Float	slopeParameter;

	/* Private members */
	GateProcessModeSpecifier	processMode;
	UniParListPtr	parList;
	NameSpecifier *positionModeList;
	NameSpecifier *operationModeList;
	NameSpecifier *typeModeList;
	NameSpecifier *processModeList;
	ChanLen	intervalIndex, offsetIndex;

} Gate, *GatePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	GatePtr	gatePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Transform_Gate(EarObjectPtr data);

void	Damp_Transform_Gate(EarObjectPtr data, ChanLen offsetIndex,
		  ChanLen intervalIndex);

BOOLN	Free_Transform_Gate(void);

UniParListPtr	GetUniParListPtr_Transform_Gate(void);

BOOLN	Init_Transform_Gate(ParameterSpecifier parSpec);

BOOLN	InitOperationModeList_Transform_Gate(void);

BOOLN	InitPositionModeList_Transform_Gate(void);

BOOLN	InitProcessModeList_Transform_Gate(void);

BOOLN	InitTypeModeList_Transform_Gate(void);

BOOLN	PrintPars_Transform_Gate(void);

BOOLN	Process_Transform_Gate(EarObjectPtr data);

void	Ramp_Transform_Gate(EarObjectPtr data, ChanLen offsetIndex,
		  ChanLen intervalIndex);

Float	GateFunction_Transform_Gate(ChanLen step, ChanLen intervalIndex,
		  Float dt);

BOOLN	InitModule_Transform_Gate(ModulePtr theModule);

BOOLN	SetDefaulEnabledPars_Transform_Gate(void);

BOOLN	SetOperationMode_Transform_Gate(WChar *theOperationMode);

BOOLN	SetParsPointer_Transform_Gate(ModulePtr theModule);

BOOLN	SetPositionMode_Transform_Gate(WChar *thePositionMode);

BOOLN	SetSlopeParameter_Transform_Gate(Float theSlopeParameter);

BOOLN	SetDuration_Transform_Gate(Float theDuration);

BOOLN	SetTimeOffset_Transform_Gate(Float theTimeOffset);

BOOLN	SetTypeMode_Transform_Gate(WChar *theTypeMode);

BOOLN	SetUniParList_Transform_Gate(void);

__END_DECLS

#endif
