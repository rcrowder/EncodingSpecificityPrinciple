/**********************
 *
 * File:		MoNCArleKim.h
 * Purpose:		This module implements Arle & Kim's version of the MacGregor
 *				Model.
 * Comments:	The changes implemented in "Neural Modeling of instrinsic and
 *				spike-disWCharge properties of cochlear nucleus neurons",
 *				J. E. Erle and D. O. Kim, Biological Cybernetics, Springer
 *				Verlag 1991. - have been added to this module version of the
 *				McGregor model.
 * Author:		Trevor Shackleton and M. J. Hewitt. (Revised L. P. O'Mard).
 * Created:		12 Jul 1993
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

#ifndef	_MONCARLEKIM_H
#define _MONCARLEKIM_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define NEURON_ARLEKIM_NUM_PARS	12
#define	ARLEKIM_DE_LIMIT		0.7	/* This limits the value of G/tm => dE. */

/*
 * ns = nano siemens - unit of conductance.
 * The default values are for the Stellate model.
 */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	NEURON_ARLEKIM_MEMBRANETCONST_TM,
	NEURON_ARLEKIM_KDECAYTCONST_TGK,
	NEURON_ARLEKIM_THRESHOLDTCONST_TTH,
	NEURON_ARLEKIM_ACCOMCONST_C,
	NEURON_ARLEKIM_DELAYEDRECTKCOND_B,
	NEURON_ARLEKIM_RESTINGTHRESHOLD_TH0,
	NEURON_ARLEKIM_ACTIONPOTENTIAL,
	NEURON_ARLEKIM_NONLINEARVCONST_VNL,
	NEURON_ARLEKIM_KREVERSALPOTEN_EK,
	NEURON_ARLEKIM_BREVERSALPOTEN_EB,
	NEURON_ARLEKIM_KRESTINGCOND_GK,
	NEURON_ARLEKIM_BRESTINGCOND_GB

} ArleKimParSpecifier;

typedef struct {

	int		lastSpikeState;
	Float	potential_V;
	Float	kConductance_Gk;
	Float	bConductance_Gb;
	Float	threshold_Th;

} ArleKimState, *ArleKimStatePtr;

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
	Float	nonLinearVConst_Vnl;/* Non-linear voltage constant (mV). */
	Float	kReversalPoten_Ek;	/* Reversal pot. of the K conductance (mV). */
	Float	bReversalPoten_Eb;	/* Rev. pot. of all other conductances (mV) */
	Float	kRestingCond_gk;	/* Resting potassium conductance (nS). */
	Float	bRestingCond_gb;	/* Resting component for all other conds.(nS).*/

	/* Private members */
	UniParListPtr	parList;
	Float	totalConductance_G, restingPotential_Er, bEquilibriumPot_Vb;
	Float	kEquilibriumPot_Vk, tmOverDt, tGkOverDt, tThOverDt, bOverDt;
	ArleKimState	*state;

} ArleKim, *ArleKimPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	ArleKimPtr		arleKimPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Neuron_ArleKim(EarObjectPtr data);

BOOLN	Free_Neuron_ArleKim(void);

void	FreeProcessVariables_Neuron_ArleKim(void);

void *	GetPotentialResponse_Neuron_ArleKim(void *potentialPtr);

UniParListPtr	GetUniParListPtr_Neuron_ArleKim(void);

BOOLN	Init_Neuron_ArleKim(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Neuron_ArleKim(EarObjectPtr data);

BOOLN	PrintPars_Neuron_ArleKim(void);

BOOLN	RunModel_Neuron_ArleKim(EarObjectPtr data);

BOOLN	SetAccomConst_Neuron_ArleKim(Float theAccomConst);

BOOLN	SetActionPotential_Neuron_ArleKim(Float theActionPotential);

BOOLN	SetBRestingCond_Neuron_ArleKim(Float theBRestingCond);

BOOLN	SetBReversalPoten_Neuron_ArleKim(Float theBReversalPoten);

BOOLN	SetCellRestingPot_Neuron_ArleKim(Float theCellRestingPot);

BOOLN	SetDelayedRectKCond_Neuron_ArleKim(Float theDelayedRectKCond);

BOOLN	SetKDecayTConst_Neuron_ArleKim(Float theKDecayTConst);

BOOLN	SetKRestingCond_Neuron_ArleKim(Float theKRestingCond);

BOOLN	SetKReversalPoten_Neuron_ArleKim(Float theKReversalPoten);

BOOLN	SetMembraneTConst_Neuron_ArleKim(Float theMembraneTConst);

BOOLN	InitModule_Neuron_ArleKim(ModulePtr theModule);

BOOLN	SetNonLinearVConst_Neuron_ArleKim(Float theNonLinearVConst);

BOOLN	SetParsPointer_Neuron_ArleKim(ModulePtr theModule);

BOOLN	SetRestingThreshold_Neuron_ArleKim(Float theRestingThreshold);

BOOLN	SetThresholdTConst_Neuron_ArleKim(Float theThresholdTConst);

BOOLN	SetUniParList_Neuron_ArleKim(void);

__END_DECLS

#endif
