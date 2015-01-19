/**********************
 *
 * File:		AnSAC.h
 * Purpose:		This module calculates a shuffled autocorrelogram from the
 * 				input signal channels: Dries H. G. Louage, Marcel van der
 * 				Heijden, and Philip X. Joris, (2004) "Temporal Properties of
 * 				Responses to Broadband Noise in the Auditory Nerve",
 * 				J. Neurophysiol. 91:2051-2065.
 * Comments:	Written using ModuleProducer version 1.5.0 (Jan 17 2007).
 * Author:		L. P. O'Mard
 * Created:		17 Jan 2007
 * Updated:
 * Copyright:	(c) 2007, 2010 Lowel P. O'Mard
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

#ifndef _ANSAC_H
#define _ANSAC_H 1

#include "UtSpikeList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_SAC_MOD_NAME			wxT("ANA_SAC")
#define ANALYSIS_SAC_NUM_PARS			5

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_SAC_NORMALISATION,
	ANALYSIS_SAC_ORDER,
	ANALYSIS_SAC_EVENTTHRESHOLD,
	ANALYSIS_SAC_MAXINTERVAL,
	ANALYSIS_SAC_BINWIDTH

} SACParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	BOOLN	normalisation;
	int		order;
	Float	eventThreshold;
	Float	maxInterval;
	Float	binWidth;

	/* Private members */
	UniParListPtr	parList;
	SpikeListSpecPtr	spikeListSpec;
	int		maxSpikes;
	Float	lastNormalisationFactor;
	ChanLen	maxIntervalIndex;

} SAC, *SACPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SACPtr	sACPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	Calc_Analysis_SAC(EarObjectPtr data);

BOOLN	CheckData_Analysis_SAC(EarObjectPtr data);

BOOLN	FreeProcessVariables_Analysis_SAC(void);

BOOLN	Free_Analysis_SAC(void);

UniParListPtr	GetUniParListPtr_Analysis_SAC(void);

BOOLN	InitModule_Analysis_SAC(ModulePtr theModule);

BOOLN	InitProcessVariables_Analysis_SAC(EarObjectPtr data);

BOOLN	Init_Analysis_SAC(ParameterSpecifier parSpec);

BOOLN	PrintPars_Analysis_SAC(void);

void	ResetProcess_Analysis_SAC(EarObjectPtr data);

BOOLN	SetBinWidth_Analysis_SAC(Float theBinWidth);

BOOLN	SetEventThreshold_Analysis_SAC(Float theEventThreshold);

BOOLN	SetMaxInterval_Analysis_SAC(Float theMaxInterval);

BOOLN	SetNormalisation_Analysis_SAC(WChar * theNormalisation);

BOOLN	SetOrder_Analysis_SAC(int theOrder);

BOOLN	SetParsPointer_Analysis_SAC(ModulePtr theModule);

BOOLN	SetUniParList_Analysis_SAC(void);

__END_DECLS

#endif
