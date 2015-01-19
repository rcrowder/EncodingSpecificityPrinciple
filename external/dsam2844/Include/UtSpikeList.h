/**********************
 *
 * File:		UtSpikeList.h
 * Purpose:		This utility module contains the routines for producing a spike
 *				train list from a signal.
 * Comments:	This implementation uses a linked list to record the spike
 *				times.  This list can be re-used, or it can retain the previous
 *				values.
 *				28-04-98 LPO: Correct segmented mode bug by adding the timeIndex
 *				value and also the riseDetected and lastValue arrays to the
 *				SpikeListSpec structure.
 * Author:		L. P. O'Mard
 * Created:		01 Apr 1996
 * Updated:		28 Apr 1998
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

#ifndef _UTSPIKELIST_H
#define _UTSPIKELIST_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct SpikeSpec {

	int		number;
	ChanLen	timeIndex;
	struct SpikeSpec *next;

} SpikeSpec, *SpikeSpecPtr;

typedef struct {

	BOOLN			*riseDetected;
	uShort			numChannels;
	Float			*lastValue;
	ChanLen			*timeIndex;
	ChanLen			*lastSpikeTimeIndex;
	SpikeSpecPtr	*head;
	SpikeSpecPtr	*tail;
	SpikeSpecPtr	*current;

} SpikeListSpec, *SpikeListSpecPtr;

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

void	Free_SpikeList(SpikeSpecPtr *head);

void	FreeListSpec_SpikeList(SpikeListSpecPtr *p);

BOOLN	GenerateList_SpikeList(SpikeListSpecPtr listSpec, Float eventThreshold,
		  EarObjectPtr process);

SpikeListSpecPtr	InitListSpec_SpikeList(int numChannels);

BOOLN	InsertSpikeSpec_SpikeList(SpikeListSpecPtr listSpec, uShort channel,
		  ChanLen timeIndex);

BOOLN	ResetListSpec_SpikeList(SpikeListSpecPtr listSpec, int offset,
		  int numChannels);

void	SetTimeContinuity_SpikeList(SpikeListSpecPtr listSpec, int offset,
		  int numChannels, ChanLen signalLength);

__END_DECLS

#endif
