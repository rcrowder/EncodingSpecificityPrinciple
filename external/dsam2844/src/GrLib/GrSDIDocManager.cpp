/**********************
 *
 * File:		GrSDIDocManager.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GrSDIDocManager.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIDocManager, wxDocManager)

BEGIN_EVENT_TABLE(SDIDocManager, wxDocManager)
    EVT_MENU(wxID_SAVEAS, SDIDocManager::OnFileSaveAs)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIDocManager::SDIDocManager(long flags, bool initialize): wxDocManager(flags,
  initialize)
{

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/****************************** OnFileSaveAs **********************************/

/*
 * A replacement for the default 'OnFileSaveAs'.
 * The conditional "#if" allows the use of multiple filters for the platforms
 * that allow it.
 */

void
SDIDocManager::OnFileSaveAs(wxCommandEvent& event)
{
#	if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
	unsigned int	i;
	wxString		descrBuf;
	wxDocTemplate	*docTemplate, *temp;

	docTemplate = GetCurrentDocument()->GetDocumentTemplate();
	wxString	oldFileFilter = docTemplate->GetFileFilter();
	for (i = 0; i < m_templates.GetCount(); i++) {
		temp = (wxDocTemplate *) (m_templates.Item(i)->GetData());
		if (temp->IsVisible()) {
		// add a '|' to separate this filter from the previous one
		if ( !descrBuf.IsEmpty() )
    		descrBuf << wxT('|');
		descrBuf << temp->GetDescription() << wxT(" (") << temp->GetFileFilter(
		  ) << wxT(") |") << temp->GetFileFilter();
		}
	}
	docTemplate->SetFileFilter(descrBuf);
	docTemplate->SetDirectory(GetLastDirectory());
	wxDocManager::OnFileSaveAs(event);
	docTemplate->SetFileFilter(oldFileFilter);
#	else
	wxDocManager::OnFileSaveAs(event);
#	endif

}

