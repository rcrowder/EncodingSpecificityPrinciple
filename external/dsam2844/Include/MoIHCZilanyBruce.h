/**********************
 *
 * File:		MoIHCZilanyBruce.h
 * Purpose:		This module is the Zilany and Bruce revision of the Carney
 * 				Synapse.
 *				It was revised from code provided by I. C. Bruce
 * 				(zbcatmodel2007v2.tar.gz).
 *				Zilany, M. S. A. and Bruce, I. C. (2006). "Modeling auditory-
 * 				nerve responses for high sound pressure levels in the normal
 * 				and impaired auditory periphery," Journal of the Acoustical
 * 				Society of America 120(3):1446-1466.
 * 				Zilany, M. S. A. and Bruce, I. C. (2007). "Representation of
 * 				the vowel /eh/ in normal and impaired auditory nerve fibers:
 * 				Model predictions of responses in cats," Journal of the
 * 				Acoustical Society of America 122(1):402-417.
 * Comments:	Written using ModuleProducer version 1.6.0 (Jul  9 2008).
 * Author:		Revised by L. P. O'Mard
 * Created:		11 Jul 2008
 * Updated:
 * Copyright:	(c) 2010 Lowel P. O'Mard
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

#ifndef _MOIHCZILANYBRUCE_H
#define _MOIHCZILANYBRUCE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHC_ZILANYBRUCE_MOD_NAME			wxT("IHC_ZILANYBRUCE")
#define IHC_ZILANYBRUCE_NUM_PARS			6

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHC_ZILANYBRUCE_SPONT,
	IHC_ZILANYBRUCE_ASS,
	IHC_ZILANYBRUCE_TAUST,
	IHC_ZILANYBRUCE_TAUR,
	IHC_ZILANYBRUCE_AR_AST,
	IHC_ZILANYBRUCE_PIMAX

} ZBHCParSpecifier;

typedef struct {

	Float	synstrength;
	Float	synslope;
	Float	slopeOverStrength;
	Float	CI, CL, PG, CG, VL, PL, VI;

} ZBSynapse, *ZBSynapsePtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	spont;
	Float	Ass;
	Float	tauST;
	Float	tauR;
	Float	Ar_Ast;
	Float	PImax;

	/* Private members */
	UniParListPtr	parList;
	ZBSynapse	*synapse;

} ZBHC, *ZBHCPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ZBHCPtr	zBHCPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IHC_ZilanyBruce(EarObjectPtr data);

BOOLN	FreeProcessVariables_IHC_ZilanyBruce(void);

BOOLN	Free_IHC_ZilanyBruce(void);

UniParListPtr	GetUniParListPtr_IHC_ZilanyBruce(void);

BOOLN	InitModule_IHC_ZilanyBruce(ModulePtr theModule);

BOOLN	InitProcessVariables_IHC_ZilanyBruce(EarObjectPtr data);

BOOLN	Init_IHC_ZilanyBruce(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHC_ZilanyBruce(void);

BOOLN	RunModel_IHC_ZilanyBruce(EarObjectPtr data);

BOOLN	SetAr_Ast_IHC_ZilanyBruce(Float theAr_Ast);

BOOLN	SetAss_IHC_ZilanyBruce(Float theAss);

BOOLN	SetPImax_IHC_ZilanyBruce(Float thePImax);

BOOLN	SetParsPointer_IHC_ZilanyBruce(ModulePtr theModule);

BOOLN	SetSpont_IHC_ZilanyBruce(Float theSpont);

BOOLN	SetTauR_IHC_ZilanyBruce(Float theTauR);

BOOLN	SetTauST_IHC_ZilanyBruce(Float theTauST);

BOOLN	SetUniParList_IHC_ZilanyBruce(void);

__END_DECLS

#endif
