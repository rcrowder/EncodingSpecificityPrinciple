/**********************
 *
 * File:		GrSDIPalette.cpp
 * Purpose: 	Simulation Process palette.
 * Comments:	Revised from Julian Smart's Ogledit/Palette.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef USE_WX_OGL

#ifdef __GNUG__
// #pragma implementation
#endif

#include "ExtCommon.h"
#include "ExtCommon.h"
//#include <wx/toolbar.h>

#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GrIPCServer.h"
#include "GrSimMgr.h"
//#include "GrSDIDoc.h"
#include "GrSDIPalette.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

// Include pixmaps
#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "Bitmaps/arrow.xpm"
#include "Bitmaps/analysis_tool.xpm"
#include "Bitmaps/control_tool.xpm"
#include "Bitmaps/display_tool.xpm"
#include "Bitmaps/filter_tool.xpm"
#include "Bitmaps/io_tool.xpm"
#include "Bitmaps/model_tool.xpm"
#include "Bitmaps/transform_tool.xpm"
#include "Bitmaps/user_tool.xpm"
#include "Bitmaps/utility_tool.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/*
 * Object editor tool palette
 *
 */

EditorToolPalette::EditorToolPalette(wxWindow* parent, const wxPoint& pos,
  const wxSize& size, long style): wxToolBar(parent, -1, pos, size,
  style)
{
	currentlySelected = -1;
	Set();

	#if 1 // ndef __WXGTK__
	SetMaxRowsCols(1000, 1);
	#endif
}

/******************************************************************************/
/*************************** Set **********************************************/
/******************************************************************************/

void
EditorToolPalette::Set(void)
{
	// Load palette bitmaps
	#ifdef __WXMSW__
	wxBitmap PaletteAnaTool(wxT("ANALYSIS_TOOL"));
	wxBitmap PaletteCtrlTool(wxT("CONTROL_TOOL"));
	wxBitmap PaletteDisplayTool(wxT("DISPLAY_TOOL"));
	wxBitmap PaletteFiltTool(wxT("FILTER_TOOL"));
	wxBitmap PaletteIOTool(wxT("IO_TOOL"));
	wxBitmap PaletteArrow(wxT("ARROWTOOL"));
	wxBitmap PaletteModelTool(wxT("MODEL_TOOL"));
	wxBitmap PaletteTransTool(wxT("TRANSFORM_TOOL"));
	wxBitmap PaletteUserTool(wxT("USER_TOOL"));
	wxBitmap PaletteUtilTool(wxT("UTILITY_TOOL"));
	#elif defined(__WXGTK__) || defined(__WXMOTIF__)
	wxBitmap PaletteAnaTool(analysis_tool_xpm);
	wxBitmap PaletteCtrlTool(control_tool_xpm);
	wxBitmap PaletteDisplayTool(display_tool_xpm);
	wxBitmap PaletteFiltTool(filter_tool_xpm);
	wxBitmap PaletteIOTool(io_tool_xpm);
	wxBitmap PaletteModelTool(model_tool_xpm);
	wxBitmap PaletteTransTool(transform_tool_xpm);
	wxBitmap PaletteUserTool(user_tool_xpm);
	wxBitmap PaletteUtilTool(utility_tool_xpm);
	wxBitmap PaletteArrow(arrow_xpm);
	#endif

	SetMargins(2, 2);
	SetToolBitmapSize(wxSize(22, 22));

	AddTool(PALETTE_ARROW, PaletteArrow, wxNullBitmap, TRUE, 0, -1, NULL,
	  wxT("Pointer"));
	AddTool(ANALYSIS_MODULE_CLASS, PaletteAnaTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, wxT("Analysis Process"));
	AddTool(CONTROL_MODULE_CLASS, PaletteCtrlTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, wxT("Control Process"));
	AddTool(DISPLAY_MODULE_CLASS, PaletteDisplayTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, wxT("Display Process"));
	AddTool(FILTER_MODULE_CLASS, PaletteFiltTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, wxT("Filter Process"));
	AddTool(IO_MODULE_CLASS, PaletteIOTool, wxNullBitmap, TRUE, 0, -1, NULL,
	  wxT("Input/Output Process"));
	AddTool(MODEL_MODULE_CLASS, PaletteModelTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, wxT("Model Process"));
	AddTool(TRANSFORM_MODULE_CLASS, PaletteTransTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, wxT("Transform Process"));
	AddTool(USER_MODULE_CLASS, PaletteUserTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, wxT("User defined Process"));
	AddTool(UTILITY_MODULE_CLASS, PaletteUtilTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, wxT("Utility Process"));

	Realize();

	ToggleTool(PALETTE_ARROW, TRUE);
	currentlySelected = PALETTE_ARROW;

}

/******************************************************************************/
/*************************** OnLeftClick **************************************/
/******************************************************************************/

bool
EditorToolPalette::OnLeftClick(int toolIndex, bool toggled)
{
	// BEGIN mutual exclusivity code
	if (toggled && (currentlySelected != -1) && (toolIndex !=
	  currentlySelected))
		ToggleTool(currentlySelected, FALSE);

	if (toggled)
		currentlySelected = toolIndex;
	else if (currentlySelected == toolIndex)
		currentlySelected = -1;
	//  END mutual exclusivity code

	return TRUE;
}

/******************************************************************************/
/*************************** OnMouseEnter *************************************/
/******************************************************************************/

void
EditorToolPalette::OnMouseEnter(int toolIndex)
{
}

/******************************************************************************/
/*************************** SetSize ******************************************/
/******************************************************************************/

void
EditorToolPalette::SetSize(int x, int y, int width, int height, int sizeFlags)
{
	wxToolBar::SetSize(x, y, width, height, sizeFlags);
}

#endif /* USE_WX_OGL */
