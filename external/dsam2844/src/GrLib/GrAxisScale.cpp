/**********************
 *
 * File:		GrAxisScale.cpp
 * Purpose: 	Axis scale class for organising scales in a rational manner.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		12 Feb 2002
 * Updated:		
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <math.h>
#include <float.h>

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
 	   #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GeCommon.h"
#include "GrAxisScale.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/****************************** Destructor ************************************/

/****************************** Set *******************************************/

bool
AxisScale::Set(const wxChar *numberFormat, double minVal, double maxVal, int minPos,
  int maxPos, int theNumTicks, bool theAutoScale)
{
	static const wxChar *funcName = wxT("AxisScale::Set");

	if (minVal > maxVal) {
		wxLogError(wxT("%s: minmum value (%g) is greater than maximum value ")
		  wxT("(%g)\n"), funcName, minVal, maxVal);
		return(false);
	}
	if (minPos > maxPos) {
		wxLogError(wxT("%s: minmum position (%d) is greater than maximum ")
		  wxT("position (%d)\n"), funcName, minPos, maxPos);
		return(false);
	}
	settingsChanged = false;
 	if (fabs(maxVal - minVal) <= 0.0) {
		minValue = minVal - 1.0;
		maxValue = maxVal + 1.0;
	} else {
		minValue = minVal;
		maxValue = maxVal;
	}
	minPosition = minPos;
	maxPosition = maxPos;
	tickOffset = 0;
	dataExponent = (int) floor(log10(MAXIMUM(fabs(minValue), fabs(maxValue))) +
	  0.5);
	if ((dataExponent >= INT_MAX) || (dataExponent <= INT_MIN)) {
		wxLogError(wxT("%s: Maximum representable display magnitude exceeded\n"),
		  funcName);
		return(false);
	}
	sigDigits = AXIS_SCALE_DEFAULT_SIG_DIGITS;
	decPlaces = AXIS_SCALE_DEFAULT_DEC_PLACES;
	SetExponent(dataExponent - sigDigits, AXIS_SCALE_DELTA_EXPONENT);
	autoScale = theAutoScale;
	if (autoScale) {
		numTicks = AXIS_SCALE_DEFAULT_NUM_TICKS;
	} else {
		numTicks = theNumTicks;
		if (!ParseNumberFormat(numberFormat)) {
			wxLogError(wxT("%s: Failed to recognise number format '%s'"),
			  funcName, numberFormat);
			return(FALSE);
		}
	}
	CalculateScales();
	outputFormat.Printf(wxT("%%.%df"), decPlaces);
	return(true);

}

/****************************** ParseNumberFormat *****************************/

/*
 * This routine parses the number format and sets the number of significant
 * numbers, decimal places and exponent.
 * It returns FALSE if it fails in any way.
 */

bool
AxisScale::ParseNumberFormat(const wxString &format)
{
	static const wxChar *funcName = wxT("AxisScale::ParseNumberFormat");
	int	p1, p2;

	if (format.empty()) {
		wxLogError(wxT("%s: Format string not set.\n"), funcName);
		return(FALSE);
	}
	if (format.compare(wxT("auto")) == 0)
		return(TRUE);
	sigDigits = 0;
	decPlaces = 0;
	p1 = (int) format.find('.');
	p2 = (int) format.find('e', true);
	if (p1 > 0)
		sigDigits = p1;
	else if (p2 > 0)
		sigDigits = p2;
	else
		sigDigits = (int) format.length();
	if ((p1 > 0) && (p2 > 0))
		decPlaces = p2 - p1 - 1;
	if (p2 > 0) {
		long	exponent;
		wxString	strExp = format.substr(p2 + 1);
		strExp.ToLong(&exponent);
		SetExponent((int) exponent, 1);
	} else {
		SetExponent(dataExponent - sigDigits, AXIS_SCALE_DELTA_EXPONENT);
		settingsChanged = !autoScale;
	}
	return(TRUE);

}

/****************************** SetExponent ***********************************/

/*
 * This sets the exponent with respect to the deltaExponent argument value.
 */

void
AxisScale::SetExponent(int requestedExp, int deltaExp)
{
	exponent = requestedExp;
	while (exponent % deltaExp)
		exponent += SIGN(exponent);
}

/****************************** CalculateScales *******************************/

/*
 * This determines the axis scalers.
 */

bool
AxisScale::CalculateScales(void)
{
	double	baseScale;

	roundingScaler = pow(10.0, decPlaces);
	baseScale = (maxValue - minValue) / (numTicks - 1);
	while (fabs(RoundedValue(baseScale * pow(10.0, -exponent))) <
	  DSAM_EPSILON) {
	  	settingsChanged = !autoScale;
		exponent -= AXIS_SCALE_DELTA_EXPONENT;
	}
	powerScale = pow(10.0, -exponent);
	positionScale = (maxPosition - minPosition) / (maxValue - minValue) /
	  powerScale;
	valueScale = RoundedValue(baseScale * powerScale);
	minValueScaled = minValue * powerScale;
	maxValueScaled = maxValue * powerScale;
	minValueScaledRounded = RoundedValue(minValueScaled);
	if (minValueScaledRounded < minValueScaled)
		tickOffset++;
	while (GetTickValue(numTicks - 1) > maxValueScaled) {
	  	settingsChanged = !autoScale;
		numTicks--;
	}
	while ((maxValueScaled - GetTickValue(numTicks - 1)) > valueScale) {
	  	settingsChanged = !autoScale;
		numTicks++;
	}
	return(TRUE);

}

/****************************** RoundedValue **********************************/

/*
 * This function returns the number rounded to the specified number of 
 * decimal places, using the respective scalers.
 */

double
AxisScale::RoundedValue(double value)
{
	return(floor(value * roundingScaler + 0.5) / roundingScaler);

}

/****************************** GetTickValue **********************************/

/*
 * This function returns the number represented by a tick point.
 */

double
AxisScale::GetTickValue(int i)
{
	return(minValueScaledRounded + valueScale * (i + tickOffset));

}

/****************************** GetTickPosition *******************************/

/*
 * This function returns the number represented by a value.
 */

int
AxisScale::GetTickPosition(double tickValue)
{
	return((int) (minPosition + positionScale * (tickValue - minValueScaled)));

}

/****************************** GetFormatString *******************************/

/*
 * This function returns the format string for the current settings.
 */

wxString
AxisScale::GetFormatString(wxChar formatChar)
{
	int		i;
	wxString	format;

	for (i = 0; i < sigDigits; i++)
		format += formatChar;
	if (decPlaces)
		format += '.';
	for (i = 0; i < decPlaces; i++)
		format += formatChar;
	format.Printf(wxT("%se%d"), format.c_str(), exponent);
	return(format);

}
