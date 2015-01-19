/**********************
 *
 * File:		GrSDICanvas.h
 * Purpose: 	Canvas clase for Simulation Design Interface .
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		11 Nov 2002
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

#ifndef _GRSDICANVAS_H
#define _GRSDICANVAS_H 1

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

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

class SDICanvas: public wxShapeCanvas
{
// DECLARE_DYNAMIC_CLASS(wxShapeCanvas)
  protected:
  public:
	wxView	*view;
  	wxWindow	*parent;

	SDICanvas(wxView *view, wxWindow *theParent = NULL, wxWindowID id = -1,
	  const wxPoint& pos = wxDefaultPosition, const wxSize& size =
	  wxDefaultSize, long style = wxRETAINED);
	~SDICanvas(void);

	void	EditProcessMenu(double x = 0, double y = 0);
	wxClassInfo *	GetClassInfo(int classSpecifier);
	void	OnMouseEvent(wxMouseEvent& event);
	void	OnPaint(wxPaintEvent& event);

	virtual void OnLeftClick(double x, double y, int keys = 0);
	virtual void OnRightClick(double x, double y, int keys = 0);

	virtual void OnDragLeft(bool draw, double x, double y, int keys=0); // Erase if draw false
	virtual void OnBeginDragLeft(double x, double y, int keys=0);
	virtual void OnEndDragLeft(double x, double y, int keys=0);

	virtual void OnDragRight(bool draw, double x, double y, int keys=0); // Erase if draw false
	virtual void OnBeginDragRight(double x, double y, int keys=0);
	virtual void OnEndDragRight(double x, double y, int keys=0);

	DECLARE_EVENT_TABLE()
};

#endif
