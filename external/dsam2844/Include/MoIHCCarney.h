/**********************
 *
 * File:		MoIHCCarney.h
 * Purpose:		Laurel H. Carney IHC synapse module: Carney L. H. (1993)
 *				"A model for the responses of low-frequency auditory-nerve
 *				fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		30 April 1996
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

#ifndef _MOIHCCARNEY_H
#define _MOIHCCARNEY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHC_CARNEY_NUM_PARS						10
#define	CARNEY_IHC_RESTING_LOCAL_CONC_FACTOR	2
#define	CARNEY_IHC_RESTING_GLOBAL_CONC_FACTOR	3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHC_CARNEY_MAXHCVOLTAGE,
	IHC_CARNEY_RESTINGRELEASERATE,
	IHC_CARNEY_RESTINGPERM,
	IHC_CARNEY_MAXGLOBALPERM,
	IHC_CARNEY_MAXLOCALPERM,
	IHC_CARNEY_MAXIMMEDIATEPERM,
	IHC_CARNEY_MAXLOCALVOLUME,
	IHC_CARNEY_MINLOCALVOLUME,
	IHC_CARNEY_MAXIMMEDIATEVOLUME,
	IHC_CARNEY_MINIMMEDIATEVOLUME

} CarneyHCParSpecifier;

typedef struct {

	Float	vI;
	Float	vL;
	Float	cI;
	Float	cL;

} CarneyHCVars, *CarneyHCVarsPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;

	Float	maxHCVoltage;		/* Vmax */
	Float	restingReleaseRate;	/* R0 */
	Float	restingPerm;		/* Prest */
	Float	maxGlobalPerm;		/* PGmax */
	Float	maxLocalPerm;		/* PLmax */
	Float	maxImmediatePerm;	/* PImax */
	Float	maxLocalVolume;		/* VLmax */
	Float	minLocalVolume;		/* VLmin */
	Float	maxImmediateVolume;	/* VImax */
	Float	minImmediateVolume;	/* VImin */

	/* Private members */
	UniParListPtr	parList;
	Float	dt, cG, pIMaxMinusPrest, pLMaxMinusPrest, pGMaxMinusPrest;
	CarneyHCVars	*hCChannels;

} CarneyHC, *CarneyHCPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CarneyHCPtr	carneyHCPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHC_Carney(EarObjectPtr data);

BOOLN	Free_IHC_Carney(void);

void	FreeProcessVariables_IHC_Carney(void);

UniParListPtr	GetUniParListPtr_IHC_Carney(void);

BOOLN	Init_IHC_Carney(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHC_Carney(EarObjectPtr data);

BOOLN	PrintPars_IHC_Carney(void);

BOOLN	RunModel_IHC_Carney(EarObjectPtr data);

BOOLN	SetMaxGlobalPerm_IHC_Carney(Float theMaxGlobalPerm);

BOOLN	SetMaxHCVoltage_IHC_Carney(Float theMaxHCVoltage);

BOOLN	SetMaxImmediatePerm_IHC_Carney(Float theMaxImmediatePerm);

BOOLN	SetMaxImmediateVolume_IHC_Carney(Float theMaxImmediateVolume);

BOOLN	SetMaxLocalPerm_IHC_Carney(Float theMaxLocalPerm);

BOOLN	SetMaxLocalVolume_IHC_Carney(Float theMaxLocalVolume);

BOOLN	SetMinImmediateVolume_IHC_Carney(Float theMinImmediateVolume);

BOOLN	SetMinLocalVolume_IHC_Carney(Float theMinLocalVolume);

BOOLN	InitModule_IHC_Carney(ModulePtr theModule);

BOOLN	SetParsPointer_IHC_Carney(ModulePtr theModule);

BOOLN	SetRestingPerm_IHC_Carney(Float theRestingPerm);

BOOLN	SetRestingReleaseRate_IHC_Carney(Float theRestingReleaseRate);

BOOLN	SetUniParList_IHC_Carney(void);

__END_DECLS

#endif
