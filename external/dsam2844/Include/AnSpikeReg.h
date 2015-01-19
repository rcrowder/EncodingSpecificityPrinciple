/**********************
 *
 * File:		AnSpikeReg.h
 * Purpose:		This module carries out a spike regularity analysis,
 *				calculating the mean, standard deviation and covariance
 *				measures for a spike train.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				See Hewitt M. J. & Meddis R. (1993) "Regularity of cochlear
 *				nucleus stellate cells: A computational Modeling study",
 *				J. of the Acoust. Soc. Am, 93, pp 3390-3399.
 *				If the standard deviation results are only valid if the
 *				covariance measure is greater than 0.  This enables the case
 *				when there are less than two counts to be marked.
 *				10-1-97: LPO - added dead-time correction for
 *				covariance CV' = S.D. / (mean - dead time) - see Rothman J. S.
 *				Young E. D. and Manis P. B. "Convergence of Auditory Nerve
 *				Fibers in the Ventral Cochlear Nucleus: Implications of a
 *				Computational Model" J. of NeuroPhysiology, 70:2562-2583.
 *				04-01-05: LPO: The 'countEarObj' EarObject does need to be
 *				registered as a subprocess for the thread processing because
 *				access to the channels is controlled by the main output process
 *				channel access.
 * Author:		L. P. O'Mard
 * Created:		01 Apr 1996
 * Updated:		20 Feb 1997
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

#ifndef _ANSPIKEREG_H
#define _ANSPIKEREG_H 1

#include "UtSpikeList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_SPIKEREGULARITY_MOD_NAME		wxT("ANA_SPIKEREGULARITY")
#define ANALYSIS_SPIKEREGULARITY_NUM_PARS		7
#define SPIKE_REG_NUM_ACCUMULATORS				3

enum {
	SPIKE_REG_COUNT = 0,
	SPIKE_REG_SUM,
	SPIKE_REG_SUMSQRS
};

#define	SPIKE_REG_MIN_SPIKES_FOR_STATISTICS		2.0

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE,
	ANALYSIS_SPIKEREGULARITY_EVENTTHRESHOLD,
	ANALYSIS_SPIKEREGULARITY_WINDOWWIDTH,
	ANALYSIS_SPIKEREGULARITY_TIMEOFFSET,
	ANALYSIS_SPIKEREGULARITY_TIMERANGE,
	ANALYSIS_SPIKEREGULARITY_DEADTIME,
	ANALYSIS_SPIKEREGULARITY_COUNTTHRESHOLD

} SpikeRegParSpecifier;

typedef enum {

	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_COUNT,
	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_COVARIANCE,
	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_MEAN,
	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_REGULARITY,
	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_STANDARD_DEV,
	ANALYSIS_SPIKEREGULARITY_OUTPUTMODE_NULL

} SpikeRegOutputModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;
	BOOLN	updateProcessVariablesFlag;

	int		outputMode;
	Float	eventThreshold;
	Float	windowWidth;
	Float	timeOffset;
	Float	timeRange;
	Float	deadTime;
	Float	countThreshold;

	/* Private members */
	NameSpecifier	*outputModeList;
	UniParListPtr	parList;
	Float	dt;
	ChanLen	*spikeTimeHistIndex;
	EarObjectPtr	countEarObj;
	SpikeListSpecPtr	spikeListSpec;

} SpikeReg, *SpikeRegPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SpikeRegPtr	spikeRegPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_SpikeRegularity(EarObjectPtr data);

BOOLN	CheckData_Analysis_SpikeRegularity(EarObjectPtr data);

BOOLN	Free_Analysis_SpikeRegularity(void);

void	FreeProcessVariables_Analysis_SpikeRegularity(void);

UniParListPtr	GetUniParListPtr_Analysis_SpikeRegularity(void);

BOOLN	Init_Analysis_SpikeRegularity(ParameterSpecifier parSpec);

BOOLN	InitOutputModeList_Analysis_SpikeRegularity(void);

BOOLN	InitProcessVariables_Analysis_SpikeRegularity(EarObjectPtr data);

BOOLN	PrintPars_Analysis_SpikeRegularity(void);

void	ResetProcess_Analysis_SpikeRegularity(EarObjectPtr data);

void	ResetStatistics_Analysis_SpikeRegularity(EarObjectPtr data);

BOOLN	SetCountThreshold_Analysis_SpikeRegularity(Float theCountThreshold);

BOOLN	SetDeadTime_Analysis_SpikeRegularity(Float theDeadTime);

BOOLN	SetWindowWidth_Analysis_SpikeRegularity(Float theWindowWidth);

BOOLN	SetEventThreshold_Analysis_SpikeRegularity(Float theEventThreshold);

BOOLN	InitModule_Analysis_SpikeRegularity(ModulePtr theModule);

BOOLN	SetOutputMode_Analysis_SpikeRegularity(WChar * theOutputMode);

BOOLN	SetParsPointer_Analysis_SpikeRegularity(ModulePtr theModule);

BOOLN	SetTimeOffset_Analysis_SpikeRegularity(Float theTimeOffset);

BOOLN	SetTimeRange_Analysis_SpikeRegularity(Float theTimeRange);

BOOLN	SetUniParList_Analysis_SpikeRegularity(void);

__END_DECLS

#endif
