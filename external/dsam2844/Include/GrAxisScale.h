/**********************
 *
 * File:		GrAxisScale.h
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

#ifndef _GRAXISSCALE_H
#define _GRAXISSCALE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define AXIS_SCALE_DEFAULT_SIG_DIGITS	1
#define AXIS_SCALE_DEFAULT_DEC_PLACES	0
#define AXIS_SCALE_DEFAULT_NUM_TICKS	5
#define AXIS_SCALE_DELTA_EXPONENT		3

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** AxisScale *********************************/

class AxisScale
{
	bool	settingsChanged, autoScale;
	int		exponent, dataExponent, decPlaces, numTicks, minPosition;
	int		tickOffset, sigDigits, maxPosition;
	double	valueScale, positionScale, roundingScaler, minValue, maxValue;
	double	minValueScaled, minValueScaledRounded, minTickValue, powerScale;
	double	maxValueScaled;
	wxString	outputFormat;

  public:

	bool	CalculateScales(void);
	double	GetDecPlaces(void)		{ return decPlaces; }
	int		GetExponent(void)		{ return exponent; }
	wxString	GetFormatString(wxChar formatChar);
	int		GetNumTicks(void)		{ return numTicks; }
	bool	GetSettingsChanged()	{ return settingsChanged; }
	wxChar *	GetOutputFormatString(void)	{ return (wxChar *) outputFormat.
				  GetData(); }
	double	GetTickValue(int i);
	int		GetTickPosition(double tickValue);
	double	GetValueScale(void)			{ return valueScale; }
	bool	ParseNumberFormat(const wxString &numberFormat);
	double	RoundedValue(double value);
	bool	Set(const wxChar *numberFormat, double minVal, double maxVal, int minPos,
			  int maxPos, int theNumTicks, bool theAutoScale);
	void	SetExponent(int requestedExp, int deltaExp);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

#endif

