/**********************
 *
 * File:		GrBrushes.cpp
 * Purpose: 	Routines for the globally used brushes.
 * Comments:	A pen is also set for the outline of the filled shapes.
 * Author:		L.P.O'Mard
 * Created:		26 Nov 1996
 * Updated:		
 * Copyright:	(c) 1996, 2010 Lowel P. O'Mard
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

#include <stdlib.h>

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/log.h>
#		include <wx/dc.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GrBrushes.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

// Must initialise these (in OnInit), not statically
GrBrushes	*greyBrushes;

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

GrBrushes::GrBrushes(void)
{
	int		i;

	for (i = 0; i < MAX_BRUSHES; i++)
		brush[i] = new wxBrush();

}

/****************************** Destructor ***********************************/

GrBrushes::~GrBrushes(void)
{
	int		i;

	for (i = 0; i < MAX_BRUSHES; i++)
		delete brush[i];

}

/****************************** SetGreyScales *********************************/

/*
 * This method sets grey scales for each brush.
 */

void
GrBrushes::SetGreyScales(int theNumGreyScales)
{
	int		i, index;
	float	redScale, blueScale, greenScale;

	if (theNumGreyScales > MAX_BRUSHES) {
		wxLogError(_T("GrBrushes::SetGreyScales: Illegal no. of scales ")
		  wxT("given."));
		exit(1);
	}
	redScale = BRUSHES_GREYSCALE_MAX_RED / (theNumGreyScales - 1.0);
	blueScale = BRUSHES_GREYSCALE_MAX_BLUE / (theNumGreyScales - 1.0);
	greenScale = BRUSHES_GREYSCALE_MAX_GREEN / (theNumGreyScales - 1.0);
	numBrushes = theNumGreyScales;
	for (i = 0; i < numBrushes; i++) {
		index = numBrushes - 1 - i;
		brush[index]->SetStyle(wxSOLID);
		brush[index]->SetColour((int) (i * redScale), (int) (i * blueScale),
		  (int) (i * greenScale));
	}

}

/****************************** GetBrush **************************************/

/*
 * This method returns a pointer to a brush.
 */

wxBrush *
GrBrushes::GetBrush(int theBrush)
{
	if ((theBrush > numBrushes) || (theBrush < 0)) {
		wxLogError(_T("GrBrushes::GetBrush: Illegal brush number specified."));
		exit(1);
	}
	return(brush[theBrush]);

}

/******************************************************************************/
/*************************** Function Declarations ****************************/
/******************************************************************************/

