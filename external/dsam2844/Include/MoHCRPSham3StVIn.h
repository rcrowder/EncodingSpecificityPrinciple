/**********************
 *
 * File:		MoHCRPSham3StVIn.h
 * Purpose:		This module contains a revised model for the Shamme hair cell
 *				receptor potential: Shamm, S. A. Chadwick R. S. Wilbur W. J.
 *				Morrish K. A. and Rinzel J.(1986) "A biophysical model of
 *				cochlear processing: Intensity dependence of pure tone
 *				responses", J. Acoust. Soc. Am. [80], pp 133-145.
 * Comments:	Written using ModuleProducer version 1.3.2 (Mar 27 2001).
 * Author:		C. J. Sumner
 * Created:		20 Aug 2001
 * Updated:
 * Copyright:	(c) 2001, 2010 Lowel P. O'Mard
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

#ifndef _MOHCRPSHAM3STVIN_H
#define _MOHCRPSHAM3STVIN_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHCRP_SHAMMA3STATEVELIN_NUM_PARS	14
#define IHCRP_SHAMMA3STATEVELIN_MOD_NAME	wxT("IHCRP_SHAMMA3STATEVELIN")

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHCRP_SHAMMA3STATEVELIN_ENDOCOCHLEARPOT_ET,
	IHCRP_SHAMMA3STATEVELIN_REVERSALPOT_EK,
	IHCRP_SHAMMA3STATEVELIN_REVERSALPOTCORRECTION,
	IHCRP_SHAMMA3STATEVELIN_TOTALCAPACITANCE_C,
	IHCRP_SHAMMA3STATEVELIN_RESTINGCONDUCTANCE_G0,
	IHCRP_SHAMMA3STATEVELIN_KCONDUCTANCE_GK,
	IHCRP_SHAMMA3STATEVELIN_MAXMCONDUCTANCE_GMAX,
	IHCRP_SHAMMA3STATEVELIN_CILIATIMECONST_TC,
	IHCRP_SHAMMA3STATEVELIN_CILIACOUPLINGGAIN_C,
	IHCRP_SHAMMA3STATEVELIN_REFERENCEPOT,
	IHCRP_SHAMMA3STATEVELIN_SENSITIVITY_S0,
	IHCRP_SHAMMA3STATEVELIN_SENSITIVITY_S1,
	IHCRP_SHAMMA3STATEVELIN_OFFSET_U0,
	IHCRP_SHAMMA3STATEVELIN_OFFSET_U1

} Sham3StVInParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	endocochlearPot_Et;
	Float	reversalPot_Ek;
	Float	reversalPotCorrection;
	Float	totalCapacitance_C;
	Float	restingConductance_G0;
	Float	kConductance_Gk;
	Float	maxMConductance_Gmax;
	Float	ciliaTimeConst_tc;
	Float	ciliaCouplingGain_C;
	Float	referencePot;
	Float	sensitivity_s0;
	Float	sensitivity_s1;
	Float	offset_u0;
	Float	offset_u1;

	/* Private members */
	UniParListPtr	parList;
	Float	*lastCiliaDisplacement_u;
	Float	*lastOutput;
	Float	dtOverC, gkEpk, dtOverTc, cGaindt, leakageConductance_Ga;

} Sham3StVIn, *Sham3StVInPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	Sham3StVInPtr	sham3StVInPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IHCRP_Shamma3StateVelIn(EarObjectPtr data);

BOOLN	FreeProcessVariables_IHCRP_Shamma3StateVelIn(void);

BOOLN	Free_IHCRP_Shamma3StateVelIn(void);

UniParListPtr	GetUniParListPtr_IHCRP_Shamma3StateVelIn(void);

BOOLN	InitModule_IHCRP_Shamma3StateVelIn(ModulePtr theModule);

BOOLN	InitProcessVariables_IHCRP_Shamma3StateVelIn(EarObjectPtr data);

BOOLN	Init_IHCRP_Shamma3StateVelIn(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHCRP_Shamma3StateVelIn(void);

BOOLN	RunModel_IHCRP_Shamma3StateVelIn(EarObjectPtr data);

BOOLN	SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn(
		  Float theCiliaCouplingGain_C);

BOOLN	SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn(
		  Float theCiliaTimeConst_tc);

BOOLN	SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn(
		  Float theEndocochlearPot_Et);

BOOLN	SetKConductance_Gk_IHCRP_Shamma3StateVelIn(Float theKConductance_Gk);

BOOLN	SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn(
		  Float theMaxMConductance_Gmax);

BOOLN	SetOffset_u0_IHCRP_Shamma3StateVelIn(Float theOffset_u0);

BOOLN	SetOffset_u1_IHCRP_Shamma3StateVelIn(Float theOffset_u1);

BOOLN	SetParsPointer_IHCRP_Shamma3StateVelIn(ModulePtr theModule);

BOOLN	SetReferencePot_IHCRP_Shamma3StateVelIn(Float theReferencePot);

BOOLN	SetRestingConductance_G0_IHCRP_Shamma3StateVelIn(
		  Float theRestingConductance_G0);

BOOLN	SetReversalPotCorrection_IHCRP_Shamma3StateVelIn(
		  Float theReversalPotCorrection);

BOOLN	SetReversalPot_Ek_IHCRP_Shamma3StateVelIn(Float theReversalPot_Ek);

BOOLN	SetSensitivity_s0_IHCRP_Shamma3StateVelIn(Float theSensitivity_s0);

BOOLN	SetSensitivity_s1_IHCRP_Shamma3StateVelIn(Float theSensitivity_s1);

BOOLN	SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn(
		  Float theTotalCapacitance_C);

BOOLN	SetUniParList_IHCRP_Shamma3StateVelIn(void);

__END_DECLS

#endif
