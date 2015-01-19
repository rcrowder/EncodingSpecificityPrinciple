/**********************
 *
 * File:		UtRefractory.h
 * Purpose:		Applies a refractory effect adjustment for spike probabilities.
 *				R. Meddis and M. J. Hewitt (1991)"Virtual pitch and phase
 *				sensitivity of a computer modle of the auditory periphery. 1:
 *				pitch identification" Journal of the Acoustical Society of
 *				America, 89, pp 2866-2882.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard, revised from code by M. J. Hewitt
 * Created:		26 Jun 1996
 * Updated:
 * Copyright:	(c) 1996, 2010 Lowel P. O'Mard
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

#ifndef _UTREFRACTORY_H
#define _UTREFRACTORY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_REFRACTORYADJUST_NUM_PARS			1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_REFRACTORYADJUST_REFRACTORYPERIOD

} RefractAdjParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	refractoryPeriod;

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	Float	**lastOutput;
	ChanLen	refractoryPeriodIndex;

} RefractAdj, *RefractAdjPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	RefractAdjPtr	refractAdjPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_RefractoryAdjust(EarObjectPtr data);

BOOLN	Free_Utility_RefractoryAdjust(void);

void	FreeProcessVariables_Utility_RefractoryAdjust(void);

UniParListPtr	GetUniParListPtr_Utility_RefractoryAdjust(void);

BOOLN	Init_Utility_RefractoryAdjust(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Utility_RefractoryAdjust(EarObjectPtr data);

BOOLN	PrintPars_Utility_RefractoryAdjust(void);

BOOLN	Process_Utility_RefractoryAdjust(EarObjectPtr data);

BOOLN	InitModule_Utility_RefractoryAdjust(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_RefractoryAdjust(ModulePtr theModule);

BOOLN	SetRefractoryPeriod_Utility_RefractoryAdjust(
		  Float theRefractoryPeriod);

BOOLN	SetUniParList_Utility_RefractoryAdjust(void);

__END_DECLS

#endif
