/**********************
 *
 * File:		GrDiagFrame.h
 * Purpose: 	Frame class for diagnostic output.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		21 Nov 2002
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

#ifndef _GRDIAGFRAME_H
#define _GRDIAGFRAME_H 1

#ifdef __GNUG__
// #pragma interface
#endif

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

class DiagFrame: public wxFrame
{

  public:
	wxMenu *fileMenu;
	wxTextCtrl	*diagnosticsText;
	wxScrolledWindow   *scrolledWindow;

	DiagFrame(wxFrame *parent, const wxString& title, const wxPoint& pos =
	  wxDefaultPosition, const wxSize& size = wxDefaultSize, long style =
	  wxDEFAULT_FRAME_STYLE | wxVSCROLL);

	void	Clear(void)		{ diagnosticsText->Clear(); }
	void	OnCloseWindow(wxCloseEvent& event);
	void	OnSize(wxSizeEvent& event);


	DECLARE_DYNAMIC_CLASS(DiagFrame)
	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

#endif
