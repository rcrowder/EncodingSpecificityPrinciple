/**********************
 *
 * File:		GrUtils.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <string.h>
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
#		include <wx/wx.h>
#	endif

// Any files included regardless of precompiled headers
#endif /* GRAPHICS_SUPPORT */

#include "GeCommon.h"
#include "GrUtils.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Box Methods ***********************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

Box::Box(double theLeft, double theRight, double theTop, double theBottom)
{
	Reset(theLeft, theRight, theTop, theBottom);

}

/****************************** Destructor ************************************/

/****************************** Reset *****************************************/

void
Box::Reset(double theLeft, double theRight, double theTop, double theBottom)
{
	left = theLeft;
	right = theRight;
	top = theTop;
	bottom = theBottom;

}

/******************************************************************************/
/****************************** Misc Routines *********************************/
/******************************************************************************/

/****************************** Hash ******************************************/

/*
 * This function returns a hash value for a string.
 */

uInt
Hash_Utils(const wxChar *s)
{
	uInt	hashValue;
	
	for (hashValue = 0; *s != '\0'; s++)
		hashValue = *s + 31 * hashValue;
	return hashValue % GRUTILS_HASHSIZE;

}

/****************************** FileSelector **********************************/

/*
 * This function returns the path to a file.
 * It assumes that a zero-length file path should be set to NULL so that the
 * 'wxFileSelector' routine selects the current directory.
 */

wxString
FileSelector_Utils(wxChar *message, wxChar *defaultPath,
  wxChar *defaultFileName, wxChar *defaultExtension, wxChar *wildcard,
  int flags, wxWindow *parent)
{
	wxChar *path, *fileName;

	path = ((defaultPath == NULL) || (defaultPath[0] == '\0'))? NULL:
	  defaultPath;
	fileName = ((defaultFileName == NULL) || (defaultFileName[0] == '\0'))?
	  NULL: defaultFileName;
	return(wxFileSelector( message,  path, 
	  fileName,  defaultExtension,  wildcard, flags,
	  parent));

}
