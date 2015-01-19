/**********************
 *
 * File:		GrDialogInfo.cpp
 * Purpose:		This is a dialog for handling the information for dialogs and
 *				also has a pointer to the dialog.
 * Comments:	14-01-99 LPO: This was revised from the ModuleInfo routine in
 *				the GrSimModuleDiag module.  It is used to store the data
 *				pointer for the dialog, but the dialog does not have to be
 *				created (taking up memory).
 * Author:		L. P. O'Mard
 * Created:		14 Jan 1999
 * Updated:		
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

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
 	   #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/object.h>
#		include <wx/list.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GeCommon.h"
#include "GrDialogInfo.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/******************************************************************************/
/****************************** DialogInfo Methods ****************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

DialogInfo::DialogInfo(wxObject *thePtr, int theSpecifier)
{
	ptr = thePtr;
	specifier = theSpecifier;

}

/****************************** Destructor ************************************/

DialogInfo::~DialogInfo(void)
{

}

