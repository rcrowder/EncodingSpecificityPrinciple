/**********************
 *
 * File:		GrLines.h
 * Purpose: 	Graphical line drawing handling class.
 * Comments:	26 Nov: support for drawing lines of filled rectangles added.
 *				26-10-97 LPO: Added yNormalisationMode.
 *				14-10-99 LPO: The 'chanSpace' member field is real to avoid
 *				rounding errors when resizing and the y-axis placings.
 * Author:		L.P.O'Mard
 * Created:		03 Dec 1995
 * Updated:		26 Oct 1997
 * Copyright:	(c) 1997, 2010 Lowel P. O'Mard
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

#ifndef _GRLINES_H
#define _GRLINES_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define		LINE_LABEL_STRING	6	/* String representation of line label. */

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** GrLines ***********************************/

class GrLines
{
	bool	redrawFlag, greyScaleMode;
	int		numDisplayedLines, channelStep, yNormalisationMode;
	int		numGreyScales;
	wxCoord	yOffset, *minYRecord;
	double	chanSpace, yMagnification, channelScale, minY, maxY, xResolution;
	ChanLen	offset, length;
	wxRect	rect;
	SignalDataPtr	signal;
	
  public:
	GrLines(void);
	~GrLines(void);

	void	CalcMaxMinLimits(void);
	void	DrawLines(wxDC& dc, double theXOffset, double theYOffset);
	double	GetChannelScale(void)		{ return channelScale; }
	double	GetChannelSpace(void)		{ return chanSpace; }
	wxChar *	GetLineLabel(int theLine);
	ChanLen	GetLength(void)				{ return length; }
	double	GetMaxY(void)				{ return(maxY); }
	double	GetMinY(void)				{ return(minY); }
	int		GetNumDisplayedLines(void)	{ return numDisplayedLines; }
	int		GetNumLines(void)			{ return signal->numChannels; }
	SignalDataPtr	GetSignalPtr(void)	{ return signal; }
	double	GetYRange(void);
	void	Rescale(wxRect& newRect);
	void	Set(SignalDataPtr theSignal, ChanLen theOffset, ChanLen theLength);
	void	SetChannelStep(int theChannelStep);
	void	SetGreyScaleMode(bool status);
	void	SetLength(ChanLen theLength);
	void	SetNumGreyScales(int theNumGS)		{ numGreyScales = theNumGS; }
	void	SetOffset(ChanLen theOffset)		{ offset = theOffset; }
	void	SetXResolution(double theXRes)		{ xResolution = theXRes; }
	void	SetSignal(SignalDataPtr theSignal);
	void	SetYLimits(double theMinY, double theMaxY)	{ minY = theMinY;
			  maxY = theMaxY; }
	void	SetYMagnification(double theYMagnification) { yMagnification = 
			  theYMagnification; }
	void	SetYNormalisationMode(int theYNormMode) { yNormalisationMode =
			  theYNormMode; }

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern NameSpecifier yNormModeList_Line[];

#endif
