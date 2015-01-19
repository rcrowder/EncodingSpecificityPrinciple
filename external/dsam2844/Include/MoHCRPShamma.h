/**********************
 *
 * File:		MoHCRPShamma.h
 * Purpose:		This module contains the model for the Shamme hair cell receptor
 *				potential: Shamm, S. A. Chadwick R. S. Wilbur W. J. Morrish
 *				K. A. and Rinzel J.(1986) "A biophysical model oc cochlear
 *				processing: Intensity dependence of pure tone responses",
 *				J. Acoust. Soc. Am. [80], pp 133-145.
 * Comments:	The program uses a mixture of "hard" and "generic" programming:
 *				The test module is used in "hard" programming mode for testing
 *				purposes.
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:	    13 Sep 1996
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

#ifndef	_MOHCRPSHAMMA_H
#define _MOHCRPSHAMMA_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define IHCRP_SHAMMA_MOD_NAME			wxT("IHCRP_SHAMMA")
#define IHCRP_SHAMMA_NUM_PARS			12

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IHCRP_SHAMMA_ENDOCOCHLEARPOT_ET,
	IHCRP_SHAMMA_REVERSALPOT_EK,
	IHCRP_SHAMMA_REVERSALPOTCORRECTION,
	IHCRP_SHAMMA_TOTALCAPACITANCE_C,
	IHCRP_SHAMMA_RESTINGCONDUCTANCE_G0,
	IHCRP_SHAMMA_KCONDUCTANCE_GK,
	IHCRP_SHAMMA_MAXMCONDUCTANCE_GMAX,
	IHCRP_SHAMMA_BETA,
	IHCRP_SHAMMA_GAMMA,
	IHCRP_SHAMMA_CILIATIMECONST_TC,
	IHCRP_SHAMMA_CILIACOUPLINGGAIN_C,
	IHCRP_SHAMMA_REFERENCEPOT

} ShammaParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;

	Float	endocochlearPot_Et;	/* Endocochlear potential, mV. */
	Float	reversalPot_Ek;		/* Reversal potential, mV. */
	Float	reversalPotCorrection;	/* Rp/(Rt+Rp), mV. */
	Float	totalCapacitance_C;	/* Total capacitance C = Ca + Cb, pF. */
	Float	restingConductance_G0;	/* Resting Conductance, G0. */
	Float	kConductance_Gk;	/* Potassium conductance, S (Seimens). */
	Float	maxMConductance_Gmax;	/* Maximum mechanical conductance, S. */
	Float	beta;				/* beta = exp(-G1/RT), dimensionless. */
	Float	gamma;				/* gamma = Z1/RT, u/m. */
	Float	ciliaTimeConst_tc;	/* BM/cilia displacement time constant, s. */
	Float	ciliaCouplingGain_C;/* Cilia coupling gain. */
	Float	referencePot;		/* Reference potential */

	/* Private members */
	UniParListPtr	parList;
	Float	*lastCiliaDisplacement_u, *lastInput, *lastOutput;
	Float	leakageConductance_Ga, dtOverC, gkEpk, dtOverTc, cGain, max_u;

} Shamma, *ShammaPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	ShammaPtr	shammaPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_IHCRP_Shamma(void);

void	FreeProcessVariables_IHCRP_Shamma(void);

UniParListPtr	GetUniParListPtr_IHCRP_Shamma(void);

BOOLN	Init_IHCRP_Shamma(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHCRP_Shamma(EarObjectPtr data);

BOOLN	PrintPars_IHCRP_Shamma(void);

BOOLN	RunModel_IHCRP_Shamma(EarObjectPtr data);

BOOLN	SetBeta_IHCRP_Shamma(Float theBeta);

BOOLN	SetCiliaCouplingGain_IHCRP_Shamma(Float theCiliaCouplingGain);

BOOLN	SetCiliaTimeConstTc_IHCRP_Shamma(Float theCiliaTimeConstTc);

BOOLN	SetEndocochlearPot_IHCRP_Shamma(Float theEndocochlearPot);

BOOLN	SetGamma_IHCRP_Shamma(Float theGamma);

BOOLN	SetKConductance_IHCRP_Shamma(Float theKConductance);

BOOLN	SetMaxMConductance_IHCRP_Shamma(Float theMaxMConductance);

BOOLN	InitModule_IHCRP_Shamma(ModulePtr theModule);

BOOLN	SetParsPointer_IHCRP_Shamma(ModulePtr theModule);

BOOLN	SetReferencePot_IHCRP_Shamma(Float theReferencePot);

BOOLN	SetRestingConductance_IHCRP_Shamma(Float theRestingConductance);

BOOLN	SetReversalPot_IHCRP_Shamma(Float theReversalPot);

BOOLN	SetReversalPotCorrection_IHCRP_Shamma(Float
		  theReversalPotCorrection);

BOOLN	SetTotalCapacitance_IHCRP_Shamma(Float theTotalCapacitance);

BOOLN	SetUniParList_IHCRP_Shamma(void);

__END_DECLS

#endif
