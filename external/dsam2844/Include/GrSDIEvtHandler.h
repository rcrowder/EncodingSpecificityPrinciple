/**********************
 *
 * File:		GrSDIEvtHandler.h
 * Purpose: 	Event handler class for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		13 Nov 2002
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

#ifndef _GRSDIEVTHANDLER_H
#define _GRSDIEVTHANDLER_H 1

#ifdef __GNUG__
// #pragma interface
#endif

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

/********************************** Pre-references ****************************/

class ModuleParDialog;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIEvtHandler ************************************/

/*
 * All shape event behaviour is routed through this handler, so we don't
 * have to derive from each shape class. We plug this in to each shape.
 */

class SDIEvtHandler: public wxShapeEvtHandler
{

  public:
	int		processType;
	wxString	label;
	DatumPtr	pc;
	ModuleParDialog	*dialog;

	SDIEvtHandler(wxShapeEvtHandler *prev = NULL, wxShape *shape = NULL,
	  const wxString& lab = wxT(""), int theProcessType = -1);
	~SDIEvtHandler(void);

	bool	EditInstruction(void);
	void	FreeInstruction(void);
	int		GetProcessType(void)	{ return processType; }
	bool	InitInstruction(void);
	void	ProcessProperties(double x, double y);
	void	ResetLabel(void);
	bool	SetSelectedShape(wxClientDC &dc);

	void	OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
	void	OnLeftDoubleClick(double x, double y, int keys = 0,
			  int attachment = 0);
	void	OnRightClick(double x, double y, int keys = 0, int attachment = 0);
	void	OnBeginDragRight(double x, double y, int keys = 0, int attachment =
			  0);
	void	OnDragRight(bool draw, double x, double y, int keys = 0,
			  int attachment = 0);
	void	OnEndDragRight(double x, double y, int keys = 0, int attachment =
			  0);
	void	OnEndSize(double x, double y);
	void	OnMovePost(wxDC& dc, double x, double y, double oldX, double oldY,
			  bool display = TRUE);

	void	SetDialog(ModuleParDialog *dlg) { dialog = dlg; }

};

#endif
