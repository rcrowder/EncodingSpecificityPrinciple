/**********************
 *
 * File:		GrSDIFrame.h
 * Purpose: 	Frame class for Simulation Design Interface .
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

#ifndef _GRSDIFRAME_H
#define _GRSDIFRAME_H 1

#ifdef __GNUG__
// #pragma interface
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	SDIFRAME_ID_TOOLBAR				500
#define	SDIFRAME_MAX_ZOOM				200
#define	SDIFRAME_MIN_ZOOM				10
#define	SDIFRAME_INC_ZOOM				10
#define SDIFRAME_STATUS_BAR_FIELDS		2
#define SDIFRAME_SIM_STATUS_FIELD		1

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	SDIFRAME_CUT = 0x1000,
	SDIFRAME_ABOUT,
	SDIFRAME_ADD_SHAPE,
	SDIFRAME_ADD_LINE,
	SDIFRAME_ADD_REPEAT_LINE,
	SDIFRAME_EDIT_PROCESS,
	SDIFRAME_SET_RESET,

	SDIFRAME_EDIT_MAIN_PARS,

	SDIFRAME_EDIT_MENU_ENABLE,
	SDIFRAME_EDIT_MENU_READ_PAR_FILE,
	SDIFRAME_EDIT_MENU_WRITE_PAR_FILE,
	SDIFRAME_EDIT_MENU_PROPERTIES,
	
	SDIFRAME_EXECUTE,
	SDIFRAME_STOP_SIMULATION,

	SDIFRAME_DIAG_WINDOW,
	SDIFRAME_CHANGE_BACKGROUND_COLOUR,
	SDIFRAME_ZOOM_COMBOBOX,

	SDIFRAME_SIM_THREAD_DISPLAY_EVENT

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

class ModuleParDialog;
class SDICanvas;
class EditorToolPalette;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIFrame *****************************************/

class SDIFrame: public wxDocParentFrame
{
	DECLARE_CLASS(SDIFrame)

	ModuleParDialog	*mainParDialog;
	DynaListPtr		dialogList;

  protected:
	wxComboBox	*myZoomComboBox;

  public:
	wxMenu *editMenu;

	SDICanvas	*canvas;
	wxToolBar	*myToolBar;
	EditorToolPalette *palette;

	SDIFrame(wxDocManager *manager, wxFrame *parent, const wxString& title,
	  const wxPoint& pos = wxDefaultPosition, const wxSize& size =
	  wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
	~SDIFrame(void);

	#if USE_GENERIC_TBAR
    virtual wxToolBar *	OnCreateToolBar(long style, wxWindowID id, const 
						  wxString& name );
	#endif // USE_GENERIC_TBAR

	SDICanvas *	CreateCanvas(wxView *view, wxFrame *parent);
	void	AddToDialogList(ModuleParDialog *dialog);
	bool	CheckChangedValues(void);
	void	DeleteFromDialogList(ModuleParDialog *dialog);
	void	DeleteMainParDialog(void);
	bool	LoadSimulation(void);
	void	OnAbout(wxCommandEvent& event);
	void	OnCloseWindow(wxCloseEvent& event);
	void	OnDiagWindow(wxCommandEvent& event);
	void	OnExecute(wxCommandEvent& event);
	void	OnEditMainPars(wxCommandEvent& event);
	void	OnEditProcess(wxCommandEvent& event);
	void	OnHelp(wxCommandEvent& event);
	void	OnQuit(wxCommandEvent& event);
	void	OnSimThreadEvent(wxCommandEvent& event);
	void	OnStopSimulation(wxCommandEvent& event);
	void	OnViewSimPars(wxCommandEvent& event);
	void	OnSize(wxSizeEvent& event);
	void	CreateToolbar(void);
	void	SetMainParDialog(ModuleParDialog *dlg) { mainParDialog = dlg; }
	void	UpdateMainParDialog(void);

	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

int		CmpDialogs_SDIFrame(void *diagNode1, void *diagNode2);

#endif
