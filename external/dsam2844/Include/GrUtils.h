/**********************
 *
 * File:		GrUtils.h
 * Purpose: 	Miscelaneous utilities.
 * Comments:	14-05-99 LPO: Added the 'FileSelector_' routine to cope with the
 *				deficiencies (!) of the 'wxFileSelector' routine's 'path'
 *				variable treatment.
 * Author:		L.P.O'Mard
 * Created:		6 Jan 1995
 * Updated:		14 May 199
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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

#ifndef _GRUTILS_H
#define _GRUTILS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	GRUTILS_HASHSIZE		UINT_MAX

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** Box ***************************************/

class Box
{

  public:
	double	left, right, top, bottom;

	Box(double theLeft = 0.0, double theRight = 0.0, double theTop = 0.0,
	  double theBottom = 0.0);

	void	 Reset(double theLeft, double theRight, double theTop,
			  double theBottom);
	void	Offset(double x, double y) {bottom += y; top += y; left += x;
			  right += x;}
	void	ShiftVertical(double shift) {bottom += shift; top += shift;}
	void	ShiftHorizontal(double shift) {left += shift; right += shift;}
	double	Height(void) {return (bottom - top);}
	double	Width(void) {return (right - left);}

};

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

wxString	FileSelector_Utils(wxChar *message, wxChar *defaultPath,
			  wxChar *defaultFileName, wxChar *defaultExtension, 
			  wxChar *wildcard, int flags, wxWindow *parent);

uInt	Hash_Utils(const wxChar *s);


#endif

