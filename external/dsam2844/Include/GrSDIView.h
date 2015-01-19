/**********************
 *
 * File:		GrSDIView.h
 * Purpose: 	Implements view functionality for Simulation Design Interface
 * Comments:	Revised from Julian Smart's Ogledit/view.h
 * Author:		L.P.O'Mard
 * Created:		04 Nov 2002
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

#ifndef _GRSDIVIEW_H
#define _GRSDIVIEW_H 1

#ifdef __GNUG__
// #pragma interface "GrSDIView.h"
#endif

#include "GrSDIDoc.h"
#include <wx/ogl/ogl.h>

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

class SDIView: public wxView
{
	DECLARE_DYNAMIC_CLASS(SDIView)

  public:
	SDIFrame *frame;
	SDICanvas *canvas;

	SDIView(void) { canvas = NULL; frame = NULL; };
	~SDIView(void) {};

	SDICanvas *	CreateCanvas(wxView *view, wxFrame *parent);
	void	EditCtrlProperties(void);
	wxShape	*FindSelectedShape(void);
	bool	OnCreate(wxDocument *doc, long flags);
	void	OnDraw(wxDC *dc);
	void	OnUpdate(wxView *sender, wxObject *hint = NULL);
	bool	OnClose(bool deleteWindow = TRUE);

	void	ProcessListDialog(void);

	//  void OnMenuCommand(int cmd);

	void	OnCut(wxCommandEvent& event);
	void	OnChangeBackgroundColour(wxCommandEvent& event);
	void	OnEditEnable(wxCommandEvent& WXUNUSED(event));
	void	OnEditProperties(wxCommandEvent& event);
	void	OnReadParFile(wxCommandEvent& event);
	void	OnSetProcessLabel(wxCommandEvent& event);
	void	OnZoomSel(wxCommandEvent& event);
	void	OnWriteParFile(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif
