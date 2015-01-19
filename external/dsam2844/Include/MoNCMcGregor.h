/******************
 *
 * File:		MoNCMcGregor.h
 * Purpose:		This module implements  the MacGregor Model.
 * Comments:	It was revised from code written by Trevor Shackleton and
 *				M. J. Hewitt.
 * Authors:		L. P. O'Mard.
 * Created:		Oct 1993
 * Updated:		24 May 1996
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

#ifndef	_MONCMCGREGOR_H
#define _MONCMCGREGOR_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define NEURON_MCGREGOR_NUM_PARS	9
#define	MCGREGOR_DE_LIMIT			0.7	/* -limits the value of G/tm => dE. */

/*
 * ns = nano siemens - unit of conductance.
 * The default values are for the Stellate model.
 */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	NEURON_MCGREGOR_MEMBRANETCONST_TM,
	NEURON_MCGREGOR_KDECAYTCONST_TGK,
	NEURON_MCGREGOR_THRESHOLDTCONST_TTH,
	NEURON_MCGREGOR_ACCOMCONST_C,
	NEURON_MCGREGOR_DELAYEDRECTKCOND_B,
	NEURON_MCGREGOR_RESTINGTHRESHOLD_TH0,
	NEURON_MCGREGOR_ACTIONPOTENTIAL,
	NEURON_MCGREGOR_KEQUILIBRIUMPOT_EK,
	NEURON_MCGREGOR_CELLRESTINGPOT_ER

} McGregorParSpecifier;

typedef struct {

	int		lastSpikeState;
	Float	potential_E;
	Float	kConductance_Gk;
	Float	threshold_Th;

} McGregorState, *McGregorStatePtr;

typedef struct  {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;

	Float	membraneTConst_Tm;	/* Membrane time constant (s). */
	Float	kDecayTConst_TGk;	/* Potassium conductance decay constant (s). */
	Float	thresholdTConst_TTh;/* Threshold rise time constant (s). */
	Float	accomConst_c;		/* Accomdation constant (dimensionless). */
	Float	delayedRectKCond_b;	/* Delayed Rectifier K Cond. strength (ns). */
	Float	restingThreshold_Th0;/* Resting threshold of cell (mV). */
	Float	actionPotential;	/* Action potential of spike (mv). */
	Float	kEquilibriumPot_Ek;	/* Reversal pot. of the K conductance (mV). */
	Float	cellRestingPot_Er;	/* Resting potential for K conductance (mV). */

	/* Private members */
	UniParListPtr	parList;
	McGregorState *state;
	Float	condDecay, threshDecay, bOverDt, dtOverTm;

} McGregor, *McGregorPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	McGregorPtr	mcGregorPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_Neuron_McGregor(void);

void	FreeProcessVariables_Neuron_McGregor(void);

void *	GetPotentialResponse_Neuron_McGregor(void *potentialPtr);

UniParListPtr	GetUniParListPtr_Neuron_McGregor(void);

BOOLN	Init_Neuron_McGregor(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Neuron_McGregor(EarObjectPtr data);

BOOLN	PrintPars_Neuron_McGregor(void);

BOOLN	RunModel_Neuron_McGregor(EarObjectPtr data);

BOOLN	SetAccomConst_Neuron_McGregor(Float theAccomConst);

BOOLN	SetActionPotential_Neuron_McGregor(Float theActionPotential);

BOOLN	SetDelayedRectKCond_Neuron_McGregor(Float theDelayedRectKCond);

BOOLN	SetKDecayTConst_Neuron_McGregor(Float theKDecayTConst);

BOOLN	SetKEquilibriumPot_Neuron_McGregor(Float theKEquilibriumPot);

BOOLN	SetCellRestingPot_Neuron_McGregor(Float theCellRestingPot);

BOOLN	SetMembraneTConst_Neuron_McGregor(Float theMembraneTConst);

BOOLN	InitModule_Neuron_McGregor(ModulePtr theModule);

BOOLN	SetParsPointer_Neuron_McGregor(ModulePtr theModule);

BOOLN	SetRestingThreshold_Neuron_McGregor(Float theRestingThreshold);

BOOLN	SetThresholdTConst_Neuron_McGregor(Float theThresholdTConst);

BOOLN	SetUniParList_Neuron_McGregor(void);

__END_DECLS

#endif
