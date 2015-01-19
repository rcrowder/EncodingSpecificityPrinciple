/**********************
 *
 * File:		MoIHC86Meddis.h
 * Purpose:		This module contains the model Meddis 86 hair cell module.
 * Comments:	The parameters A and B have been changed so that the hair cell
 *				responds correctly for  Intensity levels in dB (SPL).
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
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

#ifndef	_MOIHC86MEDDIS_H
#define _MOIHC86MEDDIS_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define MEDDIS86_NUM_PARS			10
#define MEDDIS86_MOD_NAME			wxT("IHC_Meddis86")
#define	MEDDIS86_MAX_DT	0.0001		/* Magic number for model */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	MEDDIS86_DIAGMODE,
	MEDDIS86_PERM_CONST_A,
	MEDDIS86_PERM_CONST_B,
	MEDDIS86_RELEASE_RATE_G,
	MEDDIS86_REPLENISH_RATE_Y,
	MEDDIS86_LOSS_RATE_L,
	MEDDIS86_REPROCESS_RATE_X,
	MEDDIS86_RECOVERY_RATE_R,
	MEDDIS86_MAX_FREE_POOL_M,
	MEDDIS86_FIRING_RATE_H

} Meddis86ParSpecifier;

typedef struct {

		Float	cleftC;			/* Quantity of transmitter in the cleft */
		Float	reservoirQ;		/* Quantity of transmitter in the free pool */
		Float	reprocessedW;	/* Quantity of transm. in the reproc. store */

} HairCellVars, *HairCellVarsPtr;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;

	int		diagMode;
	Float	permConst_A;		/* Permeability constant (?). */
	Float	permConst_B;		/* Units per second */
	Float	releaseRate_g;		/* Release rate (units per second). */
	Float	replenishRate_y;	/* Replenishment rate (units per second). */
	Float	lossRate_l;			/* Loss rate (units per second). */
	Float	reprocessRate_x;	/* Reprocessing rate (units per second). */
	Float	recoveryRate_r;		/* Recovery rate (units per second). */
	Float	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	Float	firingRate_h;		/* Firing rate (spikes per second). */

	/* Private members */
	NameSpecifier	*diagModeList;
	UniParListPtr	parList;
	HairCellVars	*hCChannels;
	WChar		diagFileName[MAX_FILE_PATH];
	FILE		*fp;
	Float	dt, ymdt, xdt, ydt, l_Plus_rdt, rdt, gdt, hdt;

} HairCell, *HairCellPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	HairCellPtr		hairCellPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHC_Meddis86(EarObjectPtr data);

BOOLN	Init_IHC_Meddis86(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHC_Meddis86(EarObjectPtr data);

BOOLN	Free_IHC_Meddis86(void);

void	FreeProcessVariables_IHC_Meddis86(void);

UniParListPtr	GetUniParListPtr_IHC_Meddis86(void);

BOOLN	PrintPars_IHC_Meddis86(void);

BOOLN	RunModel_IHC_Meddis86(EarObjectPtr data);

BOOLN	SetDiagMode_IHC_Meddis86(WChar * theDiagMode);

BOOLN	SetFiringRate_IHC_Meddis86(Float theSetFiringRate);

BOOLN	SetLossRate_IHC_Meddis86(Float theLossRate);

BOOLN	SetMaxFreePool_IHC_Meddis86(Float theMaxFreePool);

BOOLN	InitModule_IHC_Meddis86(ModulePtr theModule);

BOOLN	SetParsPointer_IHC_Meddis86(ModulePtr theModule);

BOOLN	SetPermConstA_IHC_Meddis86(Float thePermConstA);

BOOLN	SetPermConstB_IHC_Meddis86(Float thePermConstB);

BOOLN	SetRecoveryRate_IHC_Meddis86(Float theRecoveryRate);

BOOLN	SetReleaseRate_IHC_Meddis86(Float theReleaseRate);

BOOLN	SetReplenishRate_IHC_Meddis86(Float theReplenishRate);

BOOLN	SetReprocessRate_IHC_Meddis86(Float theReprocessRate);

BOOLN	SetUniParList_IHC_Meddis86(void);

__END_DECLS

#endif
