/**********************
 *
 * File:		UtSpikeList.c
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtSpikeList.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/**************************** Free ********************************************/

/*
 * This routine free's the spike specifications in the list.
 */

void
Free_SpikeList(SpikeSpecPtr *head)
{
	SpikeSpecPtr	p, temp;

	for (p = *head; p != NULL; ) {
		temp = p;
		p = p->next;
		free(temp);
	}
	*head = NULL;
}

/**************************** FreeListSpec ************************************/

/*
 * This routine free's the memory allocated for the spike list pointer to by
 * the 'head' pointers, and deallocates the 'head', 'tail' and 'current'
 * pointer arrays.
 */

void
FreeListSpec_SpikeList(SpikeListSpecPtr *p)
{
	int		i;

	if (*p == NULL)
		return;
	if ((*p)->head) {
		for (i = 0; i < (*p)->numChannels; i++)
			Free_SpikeList(&(*p)->head[i]);
		free((*p)->head);
	}
	if ((*p)->tail)
		free((*p)->tail);
	if ((*p)->current)
		free((*p)->current);
	if ((*p)->riseDetected)
		free((*p)->riseDetected);
	if ((*p)->lastValue)
		free((*p)->lastValue);
	if ((*p)->timeIndex)
		free((*p)->timeIndex);
	if ((*p)->lastSpikeTimeIndex)
		free((*p)->lastSpikeTimeIndex);
	free(*p);
	*p = NULL;

}

/**************************** InitListSpec ************************************/

/*
 * This intialises the head, tail and current pointers for a list.
 */

SpikeListSpecPtr
InitListSpec_SpikeList(int numChannels)
{
	static const WChar *funcName = wxT("InitListSpec_SpikeList");
	BOOLN	ok = TRUE;
	SpikeListSpecPtr p;

	if (numChannels < 1) {
		NotifyError(wxT("%s: Illegal number of channels (%d)."), funcName,
		  numChannels);
		return(NULL);
	}
	if ((p = (SpikeListSpecPtr) malloc(sizeof(SpikeListSpec))) == NULL) {
		NotifyError(wxT("%s: Out of memory SpikeListSpecPtr."), funcName);
		return(NULL);
	}
	p->numChannels = numChannels;
	if ((p->head = (SpikeSpecPtr *) calloc(p->numChannels,
	  sizeof(SpikeSpecPtr))) == NULL) {
		NotifyError(wxT("%s: Out of memory for %d 'head' pointers."), funcName,
		  p->numChannels);
		ok = FALSE;
	}
	if ((p->tail = (SpikeSpecPtr *) calloc(p->numChannels,
	  sizeof(SpikeSpecPtr))) == NULL) {
		NotifyError(wxT("%s: Out of memory for %d 'tail' pointers."), funcName,
		  p->numChannels);
		ok = FALSE;
	}
	if ((p->current = (SpikeSpecPtr *) calloc(p->numChannels,
	  sizeof(SpikeSpecPtr))) == NULL) {
		NotifyError(wxT("%s: Out of memory for %d 'current' pointers. "),
		  funcName, p->numChannels);
		ok = FALSE;
	}
	if ((p->riseDetected = (BOOLN *) calloc(p->numChannels, sizeof(BOOLN))) ==
	 NULL) {
		NotifyError(wxT("%s: Out of memory for the 'riseDetected[%d]' array. "),
		  funcName, p->numChannels);
		ok = FALSE;
	}
	if ((p->lastValue = (Float *) calloc(p->numChannels, sizeof(Float))) ==
	 NULL) {
		NotifyError(wxT("%s: Out of memory for 'lastValue[%d]' array. "),
		  funcName, p->numChannels);
		ok = FALSE;
	}
	if ((p->timeIndex = (ChanLen *) calloc(p->numChannels, sizeof(ChanLen))) ==
	 NULL) {
		NotifyError(wxT("%s: Out of memory for 'timeIndex[%d]' array. "),
		  funcName, p->numChannels);
		ok = FALSE;
	}
	if ((p->lastSpikeTimeIndex = (ChanLen *) calloc(p->numChannels,
	  sizeof(ChanLen))) == NULL) {
		NotifyError(wxT("%s: Out of memory for 'lastSpikeTimeIndex[%d]' array. "),
		  funcName, p->numChannels);
		ok = FALSE;
	}
	if (!ok)
		FreeListSpec_SpikeList(&p);
	return(p);

}

/**************************** InsertSpikeSpec *********************************/

/*
 * This routine inserts a spike specification into the spike specification list
 * It will re-use the nodes in an existing list if one exists.
 * The list can be 'reset' by setting the 'current' pointer back to the
 * head of the list.
 * The spike specification is always placed at the first node, if the list is
 * not initialised, or at the next node after the current position.
 * It returns FALSE if it fails in any way.
 * It assumes that the list specification has been correctly initialised,
 * And that there are the correct number of channels.
 */

BOOLN
InsertSpikeSpec_SpikeList(SpikeListSpecPtr listSpec, uShort channel,
  ChanLen timeIndex)
{
	static const WChar *funcName = wxT("InsertSpikeSpec_SpikeSpec");
	SpikeSpecPtr	p;

	if (!listSpec->head[channel] || (listSpec->current[channel] &&
	  !listSpec->current[channel]->next)) {
		if ((p = (SpikeSpecPtr) malloc(sizeof(SpikeSpec))) == NULL) {
			NotifyError(wxT("%s: Out of memory for spike specification."),
			  funcName);
			return(FALSE);
		}
		if (listSpec->head[channel] == NULL) {
			listSpec->head[channel] = listSpec->tail[channel] = p;
			p->number = 1;
		} else {
			p->number = listSpec->tail[channel]->number + 1;
			listSpec->tail[channel] = listSpec->tail[channel]->next = p;
		}
		p->next = NULL;
		listSpec->current[channel] = p;
	} else {
		p = (listSpec->current[channel])? listSpec->current[channel]->next:
		  listSpec->head[channel];
		listSpec->current[channel] = p;
	}
	p->timeIndex = timeIndex;
	return(TRUE);

}

/**************************** ResetListSpec ***********************************/

/*
 * This routine resets the lists in the list specification by setting the
 * current pointer back to the head of the list, it is not set to NULL.
 * This is carried out at the start of the "GenerateList" routine.
 * The offset and numChannels arguments are dependent upon what section of the
 * signal is being worked on.  They are passed explicitly rather than using a
 * pointer to the signal, to help to make this clear, especially for thread
 * processing.
 */

BOOLN
ResetListSpec_SpikeList(SpikeListSpecPtr listSpec, int offset, int numChannels)
{
	static const WChar *funcName = wxT("ResetListSpec_SpikeList");
	int		chan;

	if (listSpec == NULL) {
		NotifyError(wxT("%s: Attempt to reset NULL list specification."),
		  funcName);
		return(FALSE);
	}
	for (chan = offset; chan < numChannels; chan++)
		listSpec->timeIndex[chan] = PROCESS_START_TIME;
	return(TRUE);

}

/**************************** SetTimeContinuity ********************************/

/*
 * This routine sets the time continuity for the spike list generation.
 * It assumes that the spike list has been correctly initialised.
 * The offset and numChannels arguments are dependent upon what section of the
 * signal is being worked on.  They are passed explicitly rather than using a
 * pointer to the signal, to help to make this clear, especially for thread
 * processing.
 */

void
SetTimeContinuity_SpikeList(SpikeListSpecPtr listSpec, int offset,
  int numChannels, ChanLen signalLength)
{
	int		chan;

	for (chan = offset; chan < numChannels; chan++)
		listSpec->timeIndex[chan] += signalLength;

}

/**************************** GenerateList ************************************/

/*
 * This routine produces a spike list for each channel of an input signal.
 * It uses an EarObject so that it can utilise the channel offsets and channel
 * count when used in threaded processing mode.
 * It assumes that the 'head' and 'current' pointer arrays have been correctly
 * initialised.
 * No allowances are made for flat-topped spikes.
 */

BOOLN
GenerateList_SpikeList(SpikeListSpecPtr listSpec, Float eventThreshold,
  EarObjectPtr process)
{
	static WChar	*funcName = wxT("GenerateList_SpikeList");
	register	BOOLN		*riseDetected;
	register	ChanData	*inPtr, *lastValue;
	uShort		chan;
	ChanLen	i, startTime, *timeIndex;
	SignalDataPtr	signal = _InSig_EarObject(process, 0);

	if (listSpec == NULL) {
		NotifyError(wxT("%s: Attempt to use NULL list specification."),
		  funcName);
		return(FALSE);
	}
	for (chan = _OutSig_EarObject(process)->offset; chan <
	  _OutSig_EarObject(process)->numChannels; chan++) {
		inPtr = signal->channel[chan];
		riseDetected = listSpec->riseDetected + chan;
		lastValue = listSpec->lastValue + chan;
		timeIndex = listSpec->timeIndex + chan;
		if (*timeIndex == PROCESS_START_TIME) {
			startTime = 1;
			*riseDetected = FALSE;
			*lastValue = *(inPtr++);
			listSpec->lastSpikeTimeIndex[chan] = 0;
		} else {
			startTime = 0;
			if (listSpec->current[chan])
				listSpec->lastSpikeTimeIndex[chan] = listSpec->current[chan]->timeIndex;
		}
		listSpec->current[chan] = NULL;
		for (i = startTime; i < signal->length; i++) {
			if (!*riseDetected)
				*riseDetected = (*inPtr > *lastValue);
			else {
				if (*inPtr < *lastValue) {
					*riseDetected = FALSE;
					if (*lastValue > eventThreshold) {
						if (!InsertSpikeSpec_SpikeList(listSpec, chan,
						  *timeIndex + i - 1)) {
							NotifyError(wxT("%s: Could not insert spike ")
							  wxT("specification."), funcName);
							return(FALSE);
						}
					}
				}
			}
			*lastValue = *(inPtr++);
		}
	}
	return(TRUE);

}

/**************************** PrintList ************************************/

/*
 * This routine prints the spike list for testing purposes.
 * It assumes that the spike list has been correctly initialised.
 */

void
PrintList_SpikeList(SpikeListSpecPtr listSpec, int offset, int numChannels)
{
	static const WChar	*funcName = wxT("PrintList_SpikeList");
	int		chan;
	SpikeSpecPtr	p;

	if ((numChannels > listSpec->numChannels) || (offset >
			listSpec->numChannels) || (offset < 0)) {
		NotifyError(wxT("%s: Illegal offset (%d) or numChannels (%d)."),
		  offset, numChannels);
		return;
	}
	for (chan = offset; chan < numChannels; chan++) {
		if (!listSpec->current[chan])
			continue;
		p = listSpec->head[chan];
		while (p != listSpec->current[chan]->next) {
			DPrint(wxT("Channel[%2d], No. [%2d] = %lu\n"), chan, p->number,
			  p->timeIndex);
			p = p->next;
		}
	}
	DPrint(wxT("\n"));
}

/**************************** FPrintList ************************************/

/*
 * This routine prints the spike list to a file for testing purposes.
 * This allows the files to be written in a thread-safe way.
 * It assumes that the spike list has been correctly initialised.
 */

void
FPrintList_SpikeList(SpikeListSpecPtr listSpec, int offset, int numChannels)
{
	static const WChar	*funcName = wxT("FPrintList_SpikeList");
	WChar 	fileName[MAXLINE];
	int		chan;
	SpikeSpecPtr	p;
	FILE	*fp;

	if ((numChannels > listSpec->numChannels) || (offset >
			listSpec->numChannels) || (offset < 0)) {
		NotifyError(wxT("%s: Illegal offset (%d) or numChannels (%d)."),
		  offset, numChannels);
		return;
	}
	DSAM_snprintf(fileName, MAXLINE, wxT("SpikeList%d-%d.txt"), offset,
	  numChannels);
	if ((fp = DSAM_fopen(fileName, "w")) == NULL) {
		NotifyError(wxT("%s: could not open file '%s'."), fileName);
		return;
	}
	for (chan = offset; chan < numChannels; chan++) {
		if (!listSpec->current[chan])
			continue;
		p = listSpec->head[chan];
		while (p != listSpec->current[chan]->next) {
			DSAM_fprintf(fp, wxT("Channel[%2d], No. [%2d] = %lu\n"), chan,
			  p->number, p->timeIndex);
			p = p->next;
		}
	}
	DSAM_fprintf(fp, wxT("\n"));
	fclose(fp);

}
