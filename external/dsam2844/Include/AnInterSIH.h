/**********************
 *
 * File:		AnInterSIH.h
 * Purpose:		This routine generates an inter-spike interval histogram from
 *				the EarObject's input signal.
 *				It counts all events over the specified "eventthreshold".
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				It adds to previous data if the output signal has already been
 *				initialised and the data->updateProcessFlag is set to FALSE.
 *				Otherwise it will overwrite the old signal or create a new
 *				signal as required.  The data->updateProcessFlag facility is
 *				useful for repeated runs.
 *				This implementation uses a linked list to record the spike
 *				times.  This list can be re-used, or it can retain the previous
 *				values.  It is not free'd until the module is free'd.
 * Author:		L. P. O'Mard
 * Created:		19 Mar 1996
 * Updated:
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

#ifndef _ANINTERSIH_H
#define _ANINTERSIH_H 1

#include "UtSpikeList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_ISIH_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_ISIH_ORDER,
	ANALYSIS_ISIH_EVENTTHRESHOLD,
	ANALYSIS_ISIH_MAXINTERVAL

} InterSIHParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		order;
	Float	eventThreshold;
	Float	maxInterval;

	/* Private members */
	UniParListPtr	parList;
	SpikeListSpecPtr	spikeListSpec;
	int		maxSpikes;
	ChanLen	maxIntervalIndex;

} InterSIH, *InterSIHPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	InterSIHPtr	interSIHPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_ISIH(EarObjectPtr data);

BOOLN	CheckData_Analysis_ISIH(EarObjectPtr data);

BOOLN	Free_Analysis_ISIH(void);

void	FreeProcessVariables_Analysis_ISIH(void);

UniParListPtr	GetUniParListPtr_Analysis_ISIH(void);

BOOLN	Init_Analysis_ISIH(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Analysis_ISIH(EarObjectPtr data);

BOOLN	PrintPars_Analysis_ISIH(void);

void	ResetProcess_Analysis_ISIH(EarObjectPtr data);

BOOLN	SetEventThreshold_Analysis_ISIH(Float theEventThreshold);

BOOLN	SetMaxInterval_Analysis_ISIH(Float theMaxInterval);

BOOLN	InitModule_Analysis_ISIH(ModulePtr theModule);

BOOLN	SetOrder_Analysis_ISIH(int theOrder);

BOOLN	SetParsPointer_Analysis_ISIH(ModulePtr theModule);

BOOLN	SetUniParList_Analysis_ISIH(void);

__END_DECLS

#endif
