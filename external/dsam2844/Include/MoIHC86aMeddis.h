/******************
 *
 * File:		MoIHC86aMeddis.h
 * Purpose:		This module contains the routines for the Meddis 86a Hair cell
 *				model module.
 * Comments:	It is expects input from a IHC receptor potential model module.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1996
 * Updated:		05 Sep 1996
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

#ifndef	_MOIHC86AMEDDIS_H
#define _MOIHC86AMEDDIS_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define IHC_MEDDIS86A_NUM_PARS	8
#define	MEDDIS86A_MAX_DT		0.0001		/* Magic number for model */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IHC_MEDDIS86A_MAXFREEPOOL_M,
	IHC_MEDDIS86A_REPLENISHRATE_Y,
	IHC_MEDDIS86A_LOSSRATE_L,
	IHC_MEDDIS86A_REPROCESSRATE_X,
	IHC_MEDDIS86A_RECOVERYRATE_R,
	IHC_MEDDIS86A_PERMEABILITYPH_H,
	IHC_MEDDIS86A_PERMEABILITYPZ_Z,
	IHC_MEDDIS86A_FIRINGRATE_H2

} HairCell3ParSpecifier;

typedef struct {

	Float	cleftC;			/* Quantity of transmitter in the cleft */
	Float	reservoirQ;		/* Quantity of transmitter in the free pool */
	Float	reprocessedW;	/* Quantity of transm. in the reproc. store */

} HairCellVars3, *HairCellVars3Ptr;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;

	Float	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	Float	replenishRate_y;	/* Replenishment rate (units per second). */
	Float	lossRate_l;			/* Loss rate (units per second). */
	Float	reprocessRate_x;	/* Reprocessing rate (units per second). */
	Float	recoveryRate_r;		/* Recovery rate (units per second). */
	Float	permeabilityPH_h;	/* Permeability constant. */
	Float	permeabilityPZ_z;	/* Permeability constant. */
	Float	firingRate_h2;		/* Permeability constant. */

	/* Private members */
	UniParListPtr	parList;
	Float	ymdt, ydt, xdt, rdt, zdt, l_Plus_rdt, h2dt;
	HairCellVars3	*hCChannels;

} HairCell3, *HairCell3Ptr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	HairCell3Ptr	hairCell3Ptr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHC_Meddis86a(EarObjectPtr data);

BOOLN	Free_IHC_Meddis86a(void);

void	FreeProcessVariables_IHC_Meddis86a(void);

UniParListPtr	GetUniParListPtr_IHC_Meddis86a(void);

BOOLN	Init_IHC_Meddis86a(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHC_Meddis86a(EarObjectPtr data);

BOOLN	PrintPars_IHC_Meddis86a(void);

BOOLN	RunModel_IHC_Meddis86a(EarObjectPtr data);

BOOLN	SetFiringRate_IHC_Meddis86a(Float theSetFiringRate);

BOOLN	SetLossRate_IHC_Meddis86a(Float theLossRate);

BOOLN	SetMaxFreePool_IHC_Meddis86a(Float theMaxFreePool);

BOOLN	InitModule_IHC_Meddis86a(ModulePtr theModule);

BOOLN	SetParsPointer_IHC_Meddis86a(ModulePtr theModule);

BOOLN	SetPermeabilityPH_IHC_Meddis86a(Float thePermeabilityPH);

BOOLN	SetPermeabilityPZ_IHC_Meddis86a(Float thePermeabilityPZ);

BOOLN	SetReplenishRate_IHC_Meddis86a(Float theReplenishRate);

BOOLN	SetReprocessRate_IHC_Meddis86a(Float theReprocessRate);

BOOLN	SetRecoveryRate_IHC_Meddis86a(Float theRecoveryRate);

BOOLN	SetUniParList_IHC_Meddis86a(void);

__END_DECLS

#endif
