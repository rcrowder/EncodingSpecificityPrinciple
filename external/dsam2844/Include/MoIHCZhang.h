/**********************
 *
 * File:		MoIHCZhang.h
 * Purpose:		Incorporates the Zhang et al. 'hppi' and 'syn' IHC synapse code.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		19 Aug 2002
 * Updated:
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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

#ifndef _MOIHCZHANG_H
#define _MOIHCZHANG_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHC_ZHANG_NUM_PARS			10

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHC_ZHANG_SPONT,
	IHC_ZHANG_AASS,
	IHC_ZHANG_TAUST,
	IHC_ZHANG_TAUR,
	IHC_ZHANG_AAROVERAST,
	IHC_ZHANG_PIMAX,
	IHC_ZHANG_KS,
	IHC_ZHANG_VS1,
	IHC_ZHANG_VS2,
	IHC_ZHANG_VS3

} ZhangHCParSpecifier;

typedef struct TSynapse {

  Float 	(*Run)(struct TSynapse *p,Float x);
  void		(*Run2)(struct TSynapse *p, const Float *in, Float *out,
  			  const int length);
  Float	cf,tdres;
  Float	spont;
  Float	PTS, Ass, Ar_over_Ast, Pimax, tauR, tauST;
  Float	Prest,PPIlast,PL,PG,CIrest,CIlast,CLrest,CLlast,CG,VI,VL;
  Float	Vsat;

} TSynapse, *TSynapsePtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	spont;
	Float	aAss;
	Float	tauST;
	Float	tauR;
	Float	aAROverAST;
	Float	pIMax;
	Float	ks;
	Float	vs1;
	Float	vs2;
	Float	vs3;

	/* Private members */
	UniParListPtr	parList;
	TNonLinear	*iHCPPI;
	TSynapse	*synapse;

} ZhangHC, *ZhangHCPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ZhangHCPtr	zhangHCPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IHC_Zhang(EarObjectPtr data);

BOOLN	FreeProcessVariables_IHC_Zhang(void);

BOOLN	Free_IHC_Zhang(void);

UniParListPtr	GetUniParListPtr_IHC_Zhang(void);

BOOLN	InitModule_IHC_Zhang(ModulePtr theModule);

BOOLN	InitProcessVariables_IHC_Zhang(EarObjectPtr data);

void	InitSynapse_IHC_Zhang(TSynapse *pthis);

BOOLN	Init_IHC_Zhang(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHC_Zhang(void);

BOOLN	RunModel_IHC_Zhang(EarObjectPtr data);

void	RunSynapseDynamic_IHC_Zhang(TSynapse *pthis, const Float *in,
		  Float *out, const int length);

BOOLN	SetAAROverAST_IHC_Zhang(Float theAAROverAST);

BOOLN	SetAAss_IHC_Zhang(Float theAAss);

BOOLN	SetKs_IHC_Zhang(Float theKs);

BOOLN	SetPIMax_IHC_Zhang(Float thePIMax);

BOOLN	SetParsPointer_IHC_Zhang(ModulePtr theModule);

BOOLN	SetSpont_IHC_Zhang(Float theSpont);

BOOLN	SetTauR_IHC_Zhang(Float theTauR);

BOOLN	SetTauST_IHC_Zhang(Float theTauST);

BOOLN	SetUniParList_IHC_Zhang(void);

BOOLN	SetVs1_IHC_Zhang(Float theVs1);

BOOLN	SetVs2_IHC_Zhang(Float theVs2);

BOOLN	SetVs3_IHC_Zhang(Float theVs3);

__END_DECLS

#endif
