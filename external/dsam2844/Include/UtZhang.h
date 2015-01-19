/**********************
 *
 * File:		UtZhang.h
 * Purpose:		Incorporates the Zhang et al. AN code utilities
 * Comments:	This code was revised from the ARLO matlab code.
 * Author:		Edited by L. P. O'Mard
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

#ifndef _UTZHANG_H
#define _UTZHANG_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define Broad_ALL		0x80
#define Linear_ALL		0x40
#define NonLinear_ALL		0x20

#define FeedBack_NL		(NonLinear_ALL|0x01)
#define FeedForward_NL		(NonLinear_ALL|0x02)
#define Broad_Linear		(Broad_ALL|Linear_ALL|0x01)
#define Sharp_Linear		(Linear_ALL|0x02)
#define Broad_Linear_High	(Broad_ALL|Linear_ALL|0x03)

#define MAX_ORDER 10

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define UT_ZHANG_GAIN(MPA)	((MPA)? 1e-6: 1.0)

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UT_ZHANG_SPECIES_HUMAN,
	UT_ZHANG_SPECIES_CAT,
	UT_ZHANG_SPECIES_UNIVERSAL,
	UT_ZHANG_SPECIES_NULL

} BasilarMSpeciesSpecifier;

typedef struct TLowPass {
	/*//input one signal to the filter and get the output */
	Float (*Run)(struct TLowPass *p, Float x);
	/*//input multiple signal to the filter and get the multiple output */
	void (*Run2)(struct TLowPass *p,const Float *in, Float *out,
	  const int length);

	/*/time-domain resolution,cut-off frequency,gain, filter order */
	Float tdres, Fc, gain;
	int Order;
	/*/ parameters used in calculation */
	Float c1LP,c2LP,hc[MAX_ORDER],hcl[MAX_ORDER];
} TLowPass, *TLowPassPtr;

typedef struct TNonLinear {
	Float	(* Run)(struct TNonLinear *p, Float x);
	void	(* Run2)(struct TNonLinear *p, const Float *in, Float *out,
			  const int length);
	/*/For OHC Boltzman */
	Float	p_corner,p_slope,p_strength,x0,s0,x1,s1,shift;
	Float	Acp,Bcp,Ccp;
	/*/For AfterOHCNL */
	Float	dc,minR,A,TauMax,TauMin; /*/s0 also used by AfterOHCNL */
	/*/For IHC nonlinear function */
	Float	A0,B,C,D;
	/*/For IHCPPI nonlinear */
	Float	psl,pst,p1,p2;

} TNonLinear, *TNonLinearPtr;

typedef struct THairCell {
	Float	(* Run)(struct THairCell *p,Float x);
	void	(* Run2)(struct THairCell* p,const Float *in, Float *out,
			  const int length);
	TLowPass hclp;
	TNonLinear hcnl;
	/*/Boltzman Like NonLinear */
	/* void (*setOHC_NL)(THairCell*,Float,Float,Float,Float,Float,Float,Float,Float); */
	/* void (*setIHC_NL)(); */
} THairCell, *THairCellPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	init_boltzman(TNonLinear* p,Float _corner, Float _slope,
		  Float _strength, Float _x0, Float _s0, Float _x1, Float _s1,
		  Float _asym);

void	InitLowPass_Utility_Zhang(TLowPass* res, Float _tdres, Float _Fc,
		  Float _gain,int _LPorder);

NameSpecifier *	SpeciesList_Utility_Zhang(int index);

Float	RunAfterOhcNL_Utility_Zhang(TNonLinear* p, Float x);

void	RunAfterOhcNL2_Utility_Zhang(TNonLinear* p, const Float *in,
		  Float *out, const int length);

Float	RunBoltzman(TNonLinear *p, Float x);

Float	RunHairCell(THairCell *p, Float x);

Float	RunLowPass_Utility_Zhang(TLowPass *p, Float x);

void	RunLowPass2_Utility_Zhang(TLowPass *p,const Float *in,
		  Float *out, const int length);

__END_DECLS

#endif
