/**********************
 *
 * File:		MoHCRPMeddis.h
 * Purpose:		This module contains the model for the hair cell receptor
 *				potential.
 * Comments:	07-05-98 LPO: This model has now changed to a new version.  It
 *				uses the Meddis 86 permeability function with the low-pass
 *				filter.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
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

#ifndef	_MOHCRPMEDDIS_H
#define _MOHCRPMEDDIS_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define IHCRP_MEDDIS_NUM_PARS			4

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IHCRP_MEDDIS_PERMCONST_A,
	IHCRP_MEDDIS_PERMCONST_B,
	IHCRP_MEDDIS_RELEASERATE_G,
	IHCRP_MEDDIS_MTIMECONST_TM

} MeddisRPParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;

	Float	permConst_A;		/* For K+ probability: base Amplitude (uPa). */
	Float	permConst_B;		/* For K+ probability: amplitude scale (/uPa) */
	Float	releaseRate_g;		/* Release rate (units per second). */
	Float	mTimeConst_tm;		/* Membrane time constant, s. */

	/* Private members */
	UniParListPtr	parList;
	Float	*lastOutput, dtOverTm;

} MeddisRP, *MeddisRPPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	MeddisRPPtr	meddisRPPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHCRP_Meddis(EarObjectPtr data);

BOOLN	Free_IHCRP_Meddis(void);

void	FreeProcessVariables_IHCRP_Meddis(void);

UniParListPtr	GetUniParListPtr_IHCRP_Meddis(void);

BOOLN	Init_IHCRP_Meddis(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHCRP_Meddis(EarObjectPtr data);

BOOLN	PrintPars_IHCRP_Meddis(void);

BOOLN	RunModel_IHCRP_Meddis(EarObjectPtr data);

BOOLN	SetMTimeConstTm_IHCRP_Meddis(Float theMTimeConstTm);

BOOLN	SetPermConstA_IHCRP_Meddis(Float thePermConstA);

BOOLN	SetPermConstB_IHCRP_Meddis(Float thePermConstB);

BOOLN	InitModule_IHCRP_Meddis(ModulePtr theModule);

BOOLN	SetParsPointer_IHCRP_Meddis(ModulePtr theModule);

BOOLN	SetReleaseRate_IHCRP_Meddis(Float theReleaseRate);

BOOLN	SetUniParList_IHCRP_Meddis(void);

__END_DECLS

#endif
