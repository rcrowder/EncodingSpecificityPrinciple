/**********************
 *
 * File:		AnCrossCorr.h
 * Purpose:		This routine generates a cross-correlation function from
 *				the EarObject's input signal i.e.
 *				channel[i] * channel[i + 1]...etc.
 * Comments:	Written using ModuleProducer version 1.9.
 *				It expects a signal to be a 2N multi-channel signal, i.e. it
 *				assumes that an interleaved signal only has an
 *				interleaveLevel = 2.
 *				It overwrites previous data if the output signal has already
 *				been initialised.
 *				The function is calculated backwards from the time specified.
 *				An exponential decay function is included.
 *				The same binning as for the input signal is used.
 *				The "SignalInfo" continuity needs some work, the CF should be
 *				set automatically.
 *				02-09-97 LPO: Corrected the checking of the cross-correlation
 *				bounds in "CheckData_...".
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 * Author:		L. P. O'Mard & E. A. Lopez-Poveda
 * Created:		14 Feb 1996
 * Updated:		30 Jun 1998
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

#ifndef _ANCROSSCORR_H
#define _ANCROSSCORR_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_CCF_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_CCF_TIMEOFFSET,
	ANALYSIS_CCF_TIMECONSTANT,
	ANALYSIS_CCF_PERIOD

} CrossCorrParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	timeOffset;
	Float	timeConstant;
	Float	period;

	/* Private members */
	Float	*exponentDt;
	UniParListPtr	parList;
	ChanLen	periodIndex, timeOffsetIndex;

} CrossCorr, *CrossCorrPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CrossCorrPtr	crossCorrPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_CCF(EarObjectPtr data);

BOOLN	CheckData_Analysis_CCF(EarObjectPtr data);

BOOLN	Free_Analysis_CCF(void);

void	FreeProcessVariables_Analysis_CCF(void);

UniParListPtr	GetUniParListPtr_Analysis_CCF(void);

BOOLN	Init_Analysis_CCF(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Analysis_CCF(EarObjectPtr data);

BOOLN	PrintPars_Analysis_CCF(void);

BOOLN	InitModule_Analysis_CCF(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_CCF(ModulePtr theModule);

BOOLN	SetPeriod_Analysis_CCF(Float thePeriod);

BOOLN	SetTimeConstant_Analysis_CCF(Float theTimeConstant);

BOOLN	SetTimeOffset_Analysis_CCF(Float theTimeOffset);

BOOLN	SetUniParList_Analysis_CCF(void);

__END_DECLS

#endif
