/**********************
 *
 * File:		GrSDIPalette.h
 * Purpose: 	Simulation Process palette.
 * Comments:	Revised from Julian Smart's Ogledit/palette.h
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

#ifndef _GRSDIPALETTE_H
#define _GRSDIPALETTE_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/toolbar.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/*
 * This enum is used with the ModuleClassSpecifier in GeModuleMgr.h.
 */

enum {

	PALETTE_ARROW = 1000

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*
 * Object editor tool palette
 *
 */

class EditorToolPalette: public wxToolBar
{
  public:
	int currentlySelected;

	EditorToolPalette(wxWindow *parent, const wxPoint& pos = wxDefaultPosition,
	const wxSize& size = wxDefaultSize,
	long style = wxTB_VERTICAL);

	void	Set(void);
	bool	OnLeftClick(int toolIndex, bool toggled);
	void	OnMouseEnter(int toolIndex);
	void	SetSize(int x, int y, int width, int height, int sizeFlags =
	 		 wxSIZE_AUTO);

};

#endif
    // _OGLSAMPLE_PALETTE_H_
