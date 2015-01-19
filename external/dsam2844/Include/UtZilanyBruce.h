/**********************
 *
 * File:		UtZilanyBruce.h
 * Purpose:		This is the Zilany and Bruce BM filter utilities module.
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
 * Comments:	This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		05 Mar 2008
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

#ifndef _UTZILANYBRUCE_H
#define _UTZILANYBRUCE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ZB_GC_C1,
	ZB_GC_C2

} ZBGCTypeSpecifier;

/*
 * Both the tau and gain fields are variable, dependent upon the input.
 */

typedef struct {

	int		order;
	int		grdelay;
	Float	phase;
	Float	delta_phase;
	Float	cF;
	Float	tau;
	Float	gain;
	Float	bmTaumin;
	Float	bmTaumax;
	Float	TauWBMax;
	Float	TauWBMin;
	Float	ratiobm;
	Float	lasttmpgain;
	Complex	*fl;
	Complex	*f;

} ZBWBGTCoeffs, *ZBWBGTCoeffsPtr;

typedef struct {

	ZBGCTypeSpecifier		type;
	int		half_order_pole;
	Float	ipw;
	Float	pzero;
	Float	CF;
	Float	sigma0;
	Float	ipb;
	Float	rpa;
	Float	fs_bilinear;
	Float	gain_norm;
	Float	initphase;
	Float	input[12][4];
	Float	output[12][4];
	Complex	p[11];

} ZBGCCoeffs, *ZBGCCoeffsPtr;

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

Float	Boltzman_Utility_ZilanyBruce(Float x, Float asym, Float s0, Float s1,
		  Float x1);

Float	ChirpFilt_Utility_ZilanyBruce(Float x, ZBGCCoeffsPtr p, Float rsigma_fcohc);

Float	delay_cat_Utility_ZilanyBruce(Float cf);

void	FreeZBWBGTCoeffs_Utility_ZilanyBruce(ZBWBGTCoeffsPtr *p);

Float	gain_groupdelay_Utility_ZilanyBruce(Float binwidth,Float centerfreq,
		  Float cf, Float tau, int *grdelay);

ZBWBGTCoeffsPtr	InitZBWBGTCoeffs_Utility_ZilanyBruce(int order, Float dt,
				  Float cF, Float CAgain, Float cohc);

void	ResetZBWBGTCoeffs_Utility_ZilanyBruce(ZBWBGTCoeffsPtr p);

void	ResetZBGCCoeffs_Utility_ZilanyBruce(ZBGCCoeffsPtr p, Float binwidth,
		  Float cf, Float taumax, ZBGCTypeSpecifier type);

void	Set_tau_Utility_ZilanyBruce(ZBWBGTCoeffsPtr p, Float tau);

Float	NLafterohc_Utility_ZilanyBruce(Float x,Float taumin, Float taumax,
		  Float asym);

Float	NLogarithm_Utility_ZilanyBruce(Float x, Float slope, Float asym);

Float	WbGammaTone_Utility_ZilanyBruce(Float x, ZBWBGTCoeffsPtr p, Float dt);

__END_DECLS

#endif
