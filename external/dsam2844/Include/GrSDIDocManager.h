/**********************
 *
 * File:		GrSDIDocManager.h
 * Purpose: 	Simulation Design Interface base document manager class.
 * Comments:	This class allows me to define the "SaveAs" behaviour, etc.
 * Author:		L.P.O'Mard
 * Created:		10 Mar 2004
 * Updated:		
 * Copyright:	(c) 2004, 2010 Lowel P. O'Mard
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

#ifndef _GRSDIDOCMANAGER_H
#define _GRSDIDOCMANAGER_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/docview.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIDocManager ************************************/

class SDIDocManager: public wxDocManager
{
	DECLARE_DYNAMIC_CLASS(SDIDocManager)

  private:

  public:
	SDIDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = TRUE);

    void OnFileSaveAs(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

};

#endif
