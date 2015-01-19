/**********************
 *
 * File:		MoBMDRNL.h
 * Purpose:		The DRNL Composite filter is used to model the response of the
 *				basilar membrane.
 * Comments:	This is an implementation of a digital filter using the
 *				filters.c module.
 *				28-05-97 LPO: Corrected linearFLPCutOffScale parameter - it
 *				wasn't being used, and now a negative or zero value disables
 *				the filter altogether.
 *				06-06-97 LPO: added outputScale parameter.
 *				21-12-98 LPO: Implemented new parameters for filter bank.
 *				These parameters use the broken stick compression - see
 *				UtFilters.
 *				12-01-99 LPO: Implemented universal parameters list.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Jan 1999
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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

#ifndef _MOBMDRNL_H
#define _MOBMDRNL_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BM_DRNL_NUM_PARS			13
#define BM_DRNL_NUM_SUB_PROCESSES	1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BM_DRNL_LINEARF

} BMDRNLSubProcessSpecifier;

typedef enum {

	BM_DRNL_NONLINGTCASCADE,
	BM_DRNL_NONLINLPCASCADE,
	BM_DRNL_NONLINBWIDTH,
	BM_DRNL_NONLINSCALEG,
	BM_DRNL_COMPRSCALEA,
	BM_DRNL_COMPRSCALEB,
	BM_DRNL_COMPREXPONENT,
	BM_DRNL_LINGTCASCADE,
	BM_DRNL_LINLPCASCADE,
	BM_DRNL_LINCF,
	BM_DRNL_LINBWIDTH,
	BM_DRNL_LINSCALEG,
	BM_DRNL_THECFS

} BMDRNLParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		nonLinGTCascade;
	int		nonLinLPCascade;
	ParArrayPtr	nonLinBwidth;
	ParArrayPtr	nonLinScaleG;
	ParArrayPtr	comprScaleA;
	ParArrayPtr	comprScaleB;
	Float	comprExponent;
	int		linGTCascade;
	int		linLPCascade;
	ParArrayPtr	linCF;
	ParArrayPtr	linBwidth;
	ParArrayPtr	linScaleG;
	CFListPtr	theCFs;

	/* Private members */
	NameSpecifier	*fitFuncModeList;
	UniParListPtr	parList;
	int		numChannels;
	Float	*compressionA;
	Float	*compressionB;
	GammaToneCoeffsPtr	*nonLinearGT1;
	GammaToneCoeffsPtr	*nonLinearGT2;
	GammaToneCoeffsPtr	*linearGT;
	ContButtCoeffsPtr	*nonLinearLP;
	ContButtCoeffsPtr	*linearLP;
	EarObjectPtr	linearF;			/* Extra signal for linear filter. */

} BMDRNL, *BMDRNLPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMDRNLPtr	bMDRNLPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

void	ApplyScale_BasilarM_DRNL(EarObjectPtr data, SignalDataPtr signal,
		  ParArrayPtr p);

BOOLN	CheckData_BasilarM_DRNL(EarObjectPtr data);

BOOLN	Free_BasilarM_DRNL(void);

BOOLN	FreeProcessVariables_BasilarM_DRNL(void);

CFListPtr	GetCFListPtr_BasilarM_DRNL(void);

Float	GetFitFuncValue_BasilarM_DRNL(ParArrayPtr p, Float linCF);

Float	GetNonLinBandwidth_BasilarM_DRNL(BandwidthModePtr modePtr,
		  Float theCF);

int	GetNumFitFuncPars_BasilarM_DRNL(int mode);

UniParListPtr	GetUniParListPtr_BasilarM_DRNL(void);

BOOLN	InitProcessVariables_BasilarM_DRNL(EarObjectPtr data);

BOOLN	Init_BasilarM_DRNL(ParameterSpecifier parSpec);

BOOLN	PrintPars_BasilarM_DRNL(void);

BOOLN	RunModel_BasilarM_DRNL(EarObjectPtr data);

BOOLN	SetCFList_BasilarM_DRNL(CFListPtr theCFList);

BOOLN	SetComprExponent_BasilarM_DRNL(Float theComprExponent);

BOOLN	SetComprScaleA_BasilarM_DRNL(ParArrayPtr theComprScaleA);

BOOLN	SetComprScaleB_BasilarM_DRNL(ParArrayPtr theComprScaleB);

BOOLN	SetLinBwidth_BasilarM_DRNL(ParArrayPtr theLinBwidth);

BOOLN	SetLinCF_BasilarM_DRNL(ParArrayPtr theLinCF);

BOOLN	SetLinGTCascade_BasilarM_DRNL(int theLinGTCascade);

BOOLN	SetLinLPCascade_BasilarM_DRNL(int theLinLPCascade);

BOOLN	SetLinScaleG_BasilarM_DRNL(ParArrayPtr theLinScaleG);

BOOLN	InitModule_BasilarM_DRNL(ModulePtr theModule);

BOOLN	SetNonLinBwidth_BasilarM_DRNL(ParArrayPtr theNonLinBwidth);

BOOLN	SetNonLinGTCascade_BasilarM_DRNL(int theNonLinGTCascade);

BOOLN	SetNonLinLPCascade_BasilarM_DRNL(int theNonLinLPCascade);

BOOLN	SetNonLinScaleG_BasilarM_DRNL(ParArrayPtr theNonLinScaleG);

BOOLN	SetParsPointer_BasilarM_DRNL(ModulePtr theModule);

BOOLN	SetUniParList_BasilarM_DRNL(void);

__END_DECLS

#endif
