/**********************
 *
 * File:		GrLines.cpp
 * Purpose: 	Graphical line drawing handling class.
 * Comments:	26 Nov: support for drawing lines of filled rectangles added.
 *				26-10-97 LPO: Added yNormalisationMode.
 *				14-10-99 LPO: The 'chanSpace' member field is real to avoid
 *				rounding errors when resizing and the y-axis placings.
 * Author:		L.P.O'Mard
 * Created:		03 Dec 1995
 * Updated:		26 Oct 1997
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

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <limits.h>

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/msgdlg.h>
//#		include <wx/wx_cmdlg.h>
#		include <wx/dc.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GrUtils.h"
#include "GrLines.h"
#include "GrBrushes.h"
#include "DiSignalDisp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** GrLines Methods *******************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * It assumes that the signal has been correctly initialised.
 */

GrLines::GrLines(void)
{
	signal = NULL;
	minYRecord = NULL;
	offset = 0;
	length = 0;
	yNormalisationMode = GRAPH_LINE_YNORM_MIDDLE_MODE;
	redrawFlag = TRUE;
	greyScaleMode = FALSE;
	channelStep = -1;
	numDisplayedLines = 1;
	maxY = -(minY = FLT_MAX);
	yOffset = 0;
	yMagnification = 1.0;
	xResolution = 1.0;
	rect.SetWidth(-1);

}
	
/****************************** Destructor ************************************/

GrLines::~GrLines(void)
{
	free(minYRecord);
	
}

/****************************** Set *******************************************/

/*
 * This routine resets the memory allocation and unsets the redraw flags.
 */

void
GrLines::Set(SignalDataPtr theSignal, ChanLen theOffset, ChanLen theLength)
{
	SetSignal(theSignal);
	SetOffset(theOffset);
	SetLength(theLength);
	redrawFlag = TRUE;

}

/****************************** SetGreyScaleMode ******************************/
/*
 * This routine sets the GreyScaleMode.
 */

void
GrLines::SetGreyScaleMode(bool status)
{
        if (greyScaleMode == status)
                return;
        greyScaleMode = status;
        redrawFlag = TRUE;

}

/****************************** GetLineLabel **********************************/

/*
 * This routine returns a label for a specified line.
 * It uses the local channel step;
 */

WChar *
GrLines::GetLineLabel(int theLine)
{
	static wxChar	stringNum[MAXLINE];
	int		line = theLine * channelStep;

	DSAM_snprintf(stringNum, MAXLINE, wxT("%.0f"), signal->info.chanLabel[
	  line]);
	return(stringNum);

}

/****************************** SetChannelStep ********************************/

/*
 * This method sets the y channel step and the associated numDisplayedLines
 * parameters.
 */

void
GrLines::SetChannelStep(int theChannelStep)
{
	channelStep = theChannelStep;
	if (signal->numChannels >= channelStep)
		numDisplayedLines = signal->numChannels / channelStep;
	else {
		numDisplayedLines = signal->numChannels;
		channelStep = 1;
	}
	redrawFlag = TRUE;

}

/****************************** SetLength *************************************/

/*
 * This method sets the length parameter for the lines.
 */

void
GrLines::SetLength(ChanLen theLength)
{
	if (theLength == length)
		return;
	length = theLength;
	if (minYRecord)
		free(minYRecord);
	if ((minYRecord = (wxCoord *) calloc(length, sizeof(wxCoord))) == NULL) {
		NotifyError(wxT("GrLines::GrLines: out of memory for minYRecord."));
		exit(1);
	}
	redrawFlag = TRUE;

}

/****************************** SetSignal *************************************/

/*
 * This method sets the signal the lines.
 */

void
GrLines::SetSignal(SignalDataPtr theSignal)
{
	if (theSignal == signal)
		return;
	signal = theSignal;
	redrawFlag = TRUE;

}

/****************************** YRange ****************************************/

/*
 * This routine returns the y-range of the data or a large value if the minimum
 * and maximum values are two close together.
 */

double
GrLines::GetYRange(void)
{
	double yRange = maxY - minY;

	if (yRange > DSAM_EPSILON)
		return(yRange);
	return(HUGE_VAL);
}

/****************************** Rescale ***************************************/

/*
 * This routine re-scales the line space values, if required.
 * This routine only has meaning in the line graph mode.
 * The 'SetYSpacing' routine must always be called because the channel step
 * could have been changed.
 */

void
GrLines::Rescale(wxRect& newRect)
{
	rect = newRect;
	chanSpace = (double) rect.GetHeight() / numDisplayedLines;
	channelScale = chanSpace / GetYRange();
	if (greyScaleMode)
		yOffset = (wxCoord) (rect.GetBottom() - chanSpace);
	else if (yNormalisationMode == GRAPH_LINE_YNORM_MIDDLE_MODE)
		yOffset = (wxCoord) (rect.GetBottom() - chanSpace / 2.0);
	else
		yOffset = rect.GetBottom();
	redrawFlag = TRUE;

}

/****************************** CalcMaxMinLimits ******************************/

/*
 * This method calculates the minY and MaxY members of each line, and the
 * overall minimum and maximum limits, using the specified signal.
 */

void
GrLines::CalcMaxMinLimits(void)
{
	int		j;
	ChanLen	i;
	ChanData	*p;

	maxY = -(minY = DBL_MAX);
	for (j = 0; j < signal->numChannels; j++) {
		p = signal->channel[j] + offset;
		for (i = 0; i < length; i++, p++) {
			if (*p > maxY)
				maxY = *p;
			if (*p < minY)
				minY = *p;
		}
	}

}

/****************************** DrawLines *************************************/

/*
 * This routine draws the signal graph lines using the DC DrawLine routine.
 * In the GreyScale mode the DC DrawRectangle routine is used:
 * The x and theYOffset points describe the top left hand corner of the
 * rectangles, which are drawn using the graph brushes.
 * it uses the minYRecord to prevent overdrawing lines.
 */

#define	X_COORD(X)	((wxCoord) ((X) + xOffset))
#define	Y_COORD(Y)	((wxCoord) (yChan - ((Y) - yNormalOffset) * yScale))

void
GrLines::DrawLines(wxDC& dc, double theXOffset, double theYOffset)
{
	int		chan;
	double	xScale, yScale, x, deltaX, yNormalOffset, yChan;
	wxCoord	xCoord, yCoord, lastXCoord, lastYCoord, *minYPtr, xOffset;
	ChanLen	i, xIndex;
	ChanData	*p;

	if (greyScaleMode)
		dc.SetPen(*wxTRANSPARENT_PEN);
	else
		dc.SetPen(*wxBLACK_PEN);
	xScale = (double) rect.GetWidth() / (length - ((greyScaleMode)? 0: 1));
	yScale = (greyScaleMode)? (numGreyScales - 1) / GetYRange():
	  yMagnification * channelScale;
	if (xScale >= 1.0)
		xIndex = 1;
	else
		xIndex = (xResolution / xScale > 1.0)? (int) (xResolution / xScale): 1;
	deltaX = xScale * xIndex;
	yNormalOffset = (yNormalisationMode == GRAPH_LINE_YNORM_MIDDLE_MODE)?
	  (maxY + minY) / 2.0: minY;

	xOffset = (wxCoord) (theXOffset + rect.GetLeft());
	yChan = theYOffset + yOffset;
	for (i = 0, minYPtr = minYRecord; i < length; i++)
		*minYPtr++ = Y_COORD(minY * 0.99);		
	for (chan = 0; chan < signal->numChannels; chan += channelStep) {
		p = signal->channel[chan] + offset;
		if (greyScaleMode) {
			for (i = 0, x = 0; i < length; i += xIndex, p += xIndex, x +=
			  deltaX) {
				dc.SetBrush(*greyBrushes->GetBrush((int) floor((*p - minY) *
				  yScale + 0.5)));
				dc.DrawRectangle(X_COORD(x), (wxCoord) yChan, (wxCoord) deltaX,
				  (wxCoord) chanSpace);
			}
		} else {
			lastXCoord = X_COORD(0.0);
			lastYCoord = Y_COORD(*p);
			for (i = xIndex, p += xIndex, x = deltaX, minYPtr = minYRecord; i <
			  length; i += xIndex, p += xIndex, x += deltaX, minYPtr +=
			  xIndex) {
		  		xCoord = X_COORD(x);
				yCoord = Y_COORD(*p);
				if (yCoord < *minYPtr) {
					*minYPtr = yCoord;
					dc.DrawLine(lastXCoord, lastYCoord, xCoord, yCoord);
				}
				lastXCoord = xCoord;
				lastYCoord = yCoord;
			}
		}
		yChan -= chanSpace;
	}
	dc.SetPen(wxNullPen);
	dc.SetBrush(wxNullBrush);
	redrawFlag = FALSE;

}

#undef X_COORD
#undef Y_COORD
