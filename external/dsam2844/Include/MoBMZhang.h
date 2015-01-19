/**********************
 *
 * File:		MoBMZhang.h
 * Purpose:		Incorporates the Zhang et al. BM code
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		13 Jun 2002
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

#ifndef _MOBMZHANG_H
#define _MOBMZHANG_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BASILARM_ZHANG_NUM_PARS			17
#define BASILARM_ZHANG_ABS_DB			20.0

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BASILARM_ZHANG_MODEL,
	BASILARM_ZHANG_SPECIES,
	BASILARM_ZHANG_MICROPAINPUT,
	BASILARM_ZHANG_NBORDER,
	BASILARM_ZHANG_WBORDER,
	BASILARM_ZHANG_CORNERCP,
	BASILARM_ZHANG_SLOPECP,
	BASILARM_ZHANG_STRENGHCP,
	BASILARM_ZHANG_X0CP,
	BASILARM_ZHANG_S0CP,
	BASILARM_ZHANG_X1CP,
	BASILARM_ZHANG_S1CP,
	BASILARM_ZHANG_SHIFTCP,
	BASILARM_ZHANG_CUTCP,
	BASILARM_ZHANG_KCP,
	BASILARM_ZHANG_R0,
	BASILARM_ZHANG_CFLIST

} BMZhangParSpecifier;

typedef enum {

	BASILARM_ZHANG_MODEL_FEED_FORWARD_NL,
	BASILARM_ZHANG_MODEL_FEED_BACK_NL,
	BASILARM_ZHANG_MODEL_SHARP_LINEAR,
	BASILARM_ZHANG_MODEL_BROAD_LINEAR,
	BASILARM_ZHANG_MODEL_BROAD_LINEAR_HIGH,
	BASILARM_ZHANG_MODEL_NULL

} BasilarMModelSpecifier;

typedef struct TGammaTone {
	/*//input one signal to the filter and get the output */
	Float (*Run)(struct TGammaTone *p, Float x);
	/*//input multiple signal to the filter and get the multiple output */
	void (*Run2)(struct TGammaTone *p,const Float *in, Float *out,
	  const int length);

	Float phase;
	/* Cutoff Freq(tau), Shift Freq, ... */
	Float tdres,tau;
	Float F_shift,delta_phase;
	Float gain,c1LP,c2LP;
	Complex gtf[MAX_ORDER],gtfl[MAX_ORDER];
	int Order;

	/*// Set the tau of the gammatone filter, this is useful for time-varying
	 * filter */
	void (*SetTau)(struct TGammaTone *p, Float _tau);

} TGammaTone, *TGammaTonePtr;

typedef struct { /* class of basilar membrane */

  /* Float (*run)(TBasilarMembrane *p, Float x); */
  /* void (*run2)(TBasilarMembrane *p, const Float *in, Float *out,
   * const int length); */

  int bmmodel; /* determine if the bm is broad_linear, sharp_linear or other */
  Float tdres;
  int bmorder,wborder;

  Float tau,TauMax,TauMin;
  Float TauWB,TauWBMin;
  Float A,B;
  /* --------Model -------------- */
  TGammaTone bmfilter; /*/NonLinear Filter */
  TGammaTone gfagain; /*/Linear Filter */
  TGammaTone wbfilter; /*/Control Path filter */
  THairCell ohc;
  TNonLinear afterohc;

} TBasilarMembrane, *TBasilarMembranePtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		model;
	int		species;
	int		microPaInput;
	int		nbOrder;
	int		wbOrder;
	Float	cornerCP;
	Float	slopeCP;
	Float	strenghCP;
	Float	x0CP;
	Float	s0CP;
	Float	x1CP;
	Float	s1CP;
	Float	shiftCP;
	Float	cutCP;
	int		kCP;
	Float	r0;
	CFListPtr	cFList;

	/* Private members */
	NameSpecifier	*modelList;
	UniParListPtr	parList;
	int		numChannels;
	TBasilarMembrane *bM;

} BMZhang, *BMZhangPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMZhangPtr	bMZhangPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_BasilarM_Zhang(EarObjectPtr data);

BOOLN	FreeProcessVariables_BasilarM_Zhang(void);

BOOLN	Free_BasilarM_Zhang(void);

CFListPtr	GetCFListPtr_BasilarM_Zhang(void);

UniParListPtr	GetUniParListPtr_BasilarM_Zhang(void);

BOOLN	InitBasilarMembrane_BasilarM_Zhang(TBasilarMembranePtr bm,
		  int model, int species, Float tdres, Float cf);

BOOLN	InitModelList_BasilarM_Zhang(void);

BOOLN	InitModule_BasilarM_Zhang(ModulePtr theModule);

BOOLN	InitProcessVariables_BasilarM_Zhang(EarObjectPtr data);

BOOLN	Init_BasilarM_Zhang(ParameterSpecifier parSpec);

BOOLN	PrintPars_BasilarM_Zhang(void);

void	Run2BasilarMembrane_BasilarM_Zhang(TBasilarMembrane *bm,
		  const Float *in, Float *out, const int length);

BOOLN	RunModel_BasilarM_Zhang(EarObjectPtr data);

BOOLN	SetCFList_BasilarM_Zhang(CFListPtr theCFList);

BOOLN	SetCornerCP_BasilarM_Zhang(Float theCornerCP);

BOOLN	SetCutCP_BasilarM_Zhang(Float theCutCP);

BOOLN	SetKCP_BasilarM_Zhang(int theKCP);

BOOLN	SetMicroPaInput_BasilarM_Zhang(WChar * theMicroPaInput);

BOOLN	SetModel_BasilarM_Zhang(WChar * theModel);

BOOLN	SetNbOrder_BasilarM_Zhang(int theNbOrder);

BOOLN	SetParsPointer_BasilarM_Zhang(ModulePtr theModule);

BOOLN	SetR0_BasilarM_Zhang(Float theR0);

BOOLN	SetS0CP_BasilarM_Zhang(Float theS0CP);

BOOLN	SetS1CP_BasilarM_Zhang(Float theS1CP);

BOOLN	SetShiftCP_BasilarM_Zhang(Float theShiftCP);

BOOLN	SetSlopeCP_BasilarM_Zhang(Float theSlopeCP);

BOOLN	SetSpecies_BasilarM_Zhang(WChar * theSpecies);

BOOLN	SetStrenghCP_BasilarM_Zhang(Float theStrenghCP);

BOOLN	SetUniParList_BasilarM_Zhang(void);

BOOLN	SetWbOrder_BasilarM_Zhang(int theWbOrder);

BOOLN	SetX0CP_BasilarM_Zhang(Float theX0CP);

BOOLN	SetX1CP_BasilarM_Zhang(Float theX1CP);

__END_DECLS

#endif
