/**********************
 *
 * File:		FlZBMiddleEar.h
 * Purpose:		This is the Zilany and Bruce middle-ear filter module.
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
 * Comments:	Written using ModuleProducer version 1.6.0 (Mar  3 2008).
 *				This first revision follows the existing code with as little
 *				change as possible.
 * 				It uses an EarObject to allocate the memory for the mey arrays,
 *				but note that the mey array should not be set up as a sub-process
 *				for threaded operation.
 * Author:		Revised by L. P. O'Mard
 * Created:		03 Mar 2008
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

#ifndef _FLZBMIDDLEEAR_H
#define _FLZBMIDDLEEAR_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define FILTER_ZBMIDDLEEAR_MOD_NAME		wxT("FILT_ZILANYBRUCEME")
#define FILTER_ZBMIDDLEEAR_NUM_PARS				2
#define ZB_ME_NUM_STATE_VECTORS					3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	FILTER_ZBMIDDLEEAR_GAINMAX,
	FILTER_ZBMIDDLEEAR_PREWARPINGFREQ

} FLZBMEarParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	gainMax;
	Float	preWarpingFreq;

	/* Private members */
	UniParListPtr	parList;
	Float	m11, m12;
	Float	m21, m22, m23, m24, m25, m26;
	Float	m31, m32, m33, m34, m35, m36;
	Float	*lastInput;
	EarObjectPtr	mey;

} FLZBMEar, *FLZBMEarPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FLZBMEarPtr	fLZBMEarPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Filter_ZBMiddleEar(EarObjectPtr data);

BOOLN	FreeProcessVariables_Filter_ZBMiddleEar(void);

BOOLN	Free_Filter_ZBMiddleEar(void);

UniParListPtr	GetUniParListPtr_Filter_ZBMiddleEar(void);

BOOLN	InitModule_Filter_ZBMiddleEar(ModulePtr theModule);

BOOLN	InitProcessVariables_Filter_ZBMiddleEar(EarObjectPtr data);

BOOLN	Init_Filter_ZBMiddleEar(ParameterSpecifier parSpec);

BOOLN	PrintPars_Filter_ZBMiddleEar(void);

BOOLN	RunModel_Filter_ZBMiddleEar(EarObjectPtr data);

BOOLN	SetGainMax_Filter_ZBMiddleEar(Float theGainMax);

BOOLN	SetParsPointer_Filter_ZBMiddleEar(ModulePtr theModule);

BOOLN	SetPreWarpingFreq_Filter_ZBMiddleEar(Float thePreWarpingFreq);

BOOLN	SetUniParList_Filter_ZBMiddleEar(void);

__END_DECLS

#endif
