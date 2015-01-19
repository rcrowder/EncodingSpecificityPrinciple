/**********************
 *
 * File:		GrSDIFrame.cpp
 * Purpose: 	Frame clase for Simulation Design Interface .
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef USE_WX_OGL

#include "ExtCommon.h"
#include <wx/docview.h>

// define this to use XPMs everywhere (by default, BMPs are used under Win)
#ifdef __WXMSW__
    #define USE_XPM_BITMAPS 0
#else
    #define USE_XPM_BITMAPS 1
#endif

#if USE_GENERIC_TBAR
    #if !wxUSE_TOOLBAR_SIMPLE
        #error wxToolBarSimple is not compiled in, set wxUSE_TOOLBAR_SIMPLE \
               to 1 in setup.h and recompile the library.
    #else
        #include <wx/tbarsmpl.h>
    #endif
#endif // USE_GENERIC_TBAR

#if USE_XPM_BITMAPS && defined(__WXMSW__) && !wxUSE_XPM_IN_MSW
    #error You need to enable XPM support to use XPM bitmaps with toolbar!
#endif // USE_XPM_BITMAPS

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtDynaList.h"
#include "UtAppInterface.h"

#include "GrIPCServer.h"
#include "GrSimMgr.h"
#include "GrSDIPalette.h"
#include "GrSDICanvas.h"
#include "GrModParDialog.h"
#include "DiSignalDisp.h"
#include "GrDiagFrame.h"
#include "GrDisplayS.h"
#include "GrCanvas.h"
#include "GrMainApp.h"
#include "GrSDIFrame.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

#if USE_XPM_BITMAPS
#	include "Bitmaps/dsam.xpm"
#	include "Bitmaps/new.xpm"
#	include "Bitmaps/open.xpm"
#	include "Bitmaps/save.xpm"
#	include "Bitmaps/cut.xpm"
#	include "Bitmaps/print.xpm"
#	include "Bitmaps/play.xpm"
#	include "Bitmaps/stop.xpm"
#	include "Bitmaps/help.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

IMPLEMENT_CLASS(SDIFrame, wxDocParentFrame)

BEGIN_EVENT_TABLE(SDIFrame, wxDocParentFrame)
	EVT_MENU(SDIFRAME_ABOUT, SDIFrame::OnAbout)
	EVT_MENU(SDIFRAME_DIAG_WINDOW, SDIFrame::OnDiagWindow)
	EVT_MENU(SDIFRAME_EDIT_MAIN_PARS, SDIFrame::OnEditMainPars)
	EVT_MENU(wxID_HELP, SDIFrame::OnHelp)
	EVT_MENU(MYFRAME_ID_VIEW_SIM_PARS, SDIFrame::OnViewSimPars)
	EVT_MENU(SDIFRAME_SIM_THREAD_DISPLAY_EVENT, SDIFrame::OnSimThreadEvent)
	EVT_MENU(SDIFRAME_STOP_SIMULATION, SDIFrame::OnStopSimulation)
	EVT_MENU(SDIFRAME_EXECUTE, SDIFrame::OnExecute)
	EVT_SIZE(SDIFrame::OnSize)
	EVT_CLOSE(SDIFrame::OnCloseWindow)
END_EVENT_TABLE()

/******************************************************************************/
/*************************** Constructor **************************************/
/******************************************************************************/

SDIFrame::SDIFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
  const wxPoint& pos, const wxSize& size, long type): wxDocParentFrame(manager,
  frame, -1, title, pos, size, type)
{
	SetIcon(wxICON((wxGetApp().icon)? *wxGetApp().icon: dsam));

	canvas = NULL;
	mainParDialog = NULL;
	myToolBar = NULL;
	dialogList = NULL;
	myZoomComboBox = NULL;

#	ifdef MPI_SUPPORT
	static const wxChar *funcName = wxT("SDIFrame::MyFrame");
	int		argc = 1;

	if ((initStringPtrs = (wxChar **) calloc(argc, sizeof(wxChar *))) == NULL) {
		NotifyError(wxT("%s: Out of memory for %d initialisation strings."),
		 funcName, argc);
		 exit(1);
	}
	initStringPtrs[0] = (wxChar *) funcName;
	MPI_Init( &argc, &initStringPtrs );
#	endif

	CreateToolbar();

	palette = new EditorToolPalette(this, wxPoint(0, 0), wxSize(-1,-1),
	  wxTB_VERTICAL);
	Centre(wxBOTH);

	CreateStatusBar(SDIFRAME_STATUS_BAR_FIELDS);

}

/******************************************************************************/
/*************************** Desstructor **************************************/
/******************************************************************************/

SDIFrame::~SDIFrame(void)
{
#	ifdef MPI_SUPPORT
	int		i, myRank, numProcesses, ok = TRUE;

	MPI_Comm_rank( MPI_COMM_WORLD, &myRank);
	MPI_Group_size( MPI_COMM_WORLD, &numProcesses);
	for (i = 0; i < numProcesses; i++)
		if (i != myRank)
			MPI_Send(&ok, 1, MPI_INT, i, MASTER_EXIT_TAG, MPI_COMM_WORLD);

	free(initStringPtrs);
	MPI_Finalize();
#	endif

	DeleteMainParDialog();
	wxGetApp().ExitMain();

}

/******************************************************************************/
/*************************** CreateCanvas *************************************/
/******************************************************************************/

// Creates a canvas. Called by OnInit as a child of the main window

SDICanvas *
SDIFrame::CreateCanvas(wxView *view, wxFrame *parent)
{
	int width, height;
	parent->GetClientSize(&width, &height);

	// Non-retained canvas
	SDICanvas *canvas = new SDICanvas(view, parent, -1, wxPoint(0, 0),
	  wxSize(width, height), 0);
	canvas->SetCursor(wxCursor(wxCURSOR_HAND));

	// Give it scrollbars
	canvas->SetScrollbars(20, 20, 50, 50);

	return canvas;

}

/****************************** DeleteMainParDialog ***************************/

void
SDIFrame::DeleteMainParDialog(void)
{
	if (!mainParDialog)
		return;
	delete mainParDialog;
	mainParDialog = NULL;

}

/****************************** UpdateMainParDialog ***************************/

/*
 * This routine checks if main dialog window is open, and if so it refreshes
 * the parameters in the dialog.
 */

void
SDIFrame::UpdateMainParDialog(void)
{
	if (!mainParDialog)
		return;


}

/****************************** LoadSimulation ********************************/

/*
 * This routine sets the simulation file and attempts to load it.
 */

bool
SDIFrame::LoadSimulation(void)
{
	ResetGUIDialogs();
	if (wxGetApp().GetDiagFrame())
		wxGetApp().GetDiagFrame()->Clear();
	if (!wxGetApp().GetGrMainApp()->ResetSimulation())
		return(FALSE);
	if (mainParDialog)
		mainParDialog->parListInfoList->UpdateAllControlValues();
	return(TRUE);

}

/****************************** AddToDialogList *******************************/

/*
 * This routine adds a dialog to the open dialog list.
 */

void
SDIFrame::AddToDialogList(ModuleParDialog *dialog)
{
	Append_Utility_DynaList(&dialogList, dialog);

}

/****************************** DeleteFromDialogList **************************/

/*
 * This routine adds a dialog to the open dialog list.
 */

void
SDIFrame::DeleteFromDialogList(ModuleParDialog *dialog)
{
	DynaListPtr	p = FindElement_Utility_DynaList(dialogList,
	  CmpDialogs_SDIFrame, dialog);
	if (!p)
		wxLogError(wxT("SDIFrame::DeleteFromDialogList: Could not find ")
		  wxT("dialog.\n"));
	Remove_Utility_DynaList(&dialogList, p);

}

/****************************** CheckChangedValues ****************************/

/*
 * This routine checks for changed values in the open dialogs.
 */

bool
SDIFrame::CheckChangedValues(void)
{
	bool	ok = TRUE;
	DynaListPtr	p = dialogList;

	while (p) {
		if (!((ModuleParDialog *) p->data)->CheckChangedValues())
			ok = FALSE;
		p = p->next;
	}
	return(ok);

}

/****************************** CreateToolbar *********************************/

void
SDIFrame::CreateToolbar(void)
{
	// delete and recreate the toolbar
	wxToolBarBase *toolBar = GetToolBar();
	delete toolBar;

	SetToolBar(NULL);

	toolBar = CreateToolBar((long) (wxNO_BORDER | wxTB_FLAT | wxTB_DOCKABLE |
	  wxTB_HORIZONTAL), SDIFRAME_ID_TOOLBAR);
	toolBar->SetMargins( 4, 4 );

	// Set up toolbar
	#if USE_XPM_BITMAPS
		wxBitmap newBitmap = wxBitmap(new_xpm);
		wxBitmap openBitmap = wxBitmap(open_xpm);
		wxBitmap saveBitmap = wxBitmap(save_xpm);
		wxBitmap cutBitmap = wxBitmap(cut_xpm);
		wxBitmap printBitmap = wxBitmap(print_xpm);
		wxBitmap playBitmap = wxBitmap(play_xpm);
		wxBitmap stopBitmap = wxBitmap(stop_xpm);
		wxBitmap helpBitmap = wxBitmap(help_xpm);
	#else // !USE_XPM_BITMAPS
		wxBitmap newBitmap = wxBITMAP(new);
		wxBitmap openBitmap = wxBITMAP(open);
		wxBitmap saveBitmap = wxBITMAP(save);
		wxBitmap cutBitmap = wxBITMAP(cut);
		wxBitmap printBitmap = wxBITMAP(print);
		wxBitmap playBitmap = wxBITMAP(play);
		wxBitmap stopBitmap = wxBITMAP(stop);
		wxBitmap helpBitmap = wxBITMAP(help);
	#endif // USE_XPM_BITMAPS/!USE_XPM_BITMAPS

	#ifdef __WXMSW__
		int width = 24;
	#else
		int width = 16;
	#endif

	int currentX = 5;

	toolBar->AddTool(wxID_NEW, newBitmap, wxNullBitmap, FALSE, currentX, -1,
	  (wxObject *) NULL, wxT("New simulation"));
	currentX += width + 5;
	toolBar->AddTool(wxID_OPEN, openBitmap, wxNullBitmap, FALSE, currentX, -1,
	  (wxObject *) NULL, wxT("Open simulation"));
	currentX += width + 5;
	toolBar->AddTool(wxID_SAVE, saveBitmap, wxNullBitmap, TRUE, currentX, -1,
	  (wxObject *) NULL, wxT("Save simulation"));
	currentX += width + 5;
	toolBar->AddTool(SDIFRAME_CUT, cutBitmap, wxNullBitmap, FALSE, currentX, -1,
	  (wxObject *) NULL, wxT("Cut element"));
	currentX += width + 5;
	toolBar->AddTool(wxID_PRINT, printBitmap, wxNullBitmap, FALSE, currentX, -1,
	  (wxObject *) NULL, wxT("Print simulation design"));
	currentX += width + 5;
	toolBar->AddSeparator();
	toolBar->AddTool(SDIFRAME_EXECUTE, playBitmap, wxNullBitmap, FALSE,
	  currentX, -1, (wxObject *) NULL, wxT("Play simulation"));
	currentX += width + 5;
	toolBar->AddTool(SDIFRAME_STOP_SIMULATION, stopBitmap, wxNullBitmap, FALSE,
	  currentX, -1, (wxObject *) NULL, wxT("Stop simulation"));
	currentX += width + 5;
	toolBar->AddSeparator();

	// Create a combobox for zooming
	int zoomW = 60;
	int zoomH = 18;
	int i;
	int numStrings = (SDIFRAME_MAX_ZOOM - SDIFRAME_MIN_ZOOM) /
	SDIFRAME_INC_ZOOM + 1;
	wxString *zoomStrings = new wxString[numStrings];
	for (i = 0; i < numStrings; i ++)
		zoomStrings[numStrings - i - 1].Printf(wxT("%d%%"), (i *
		  SDIFRAME_INC_ZOOM + SDIFRAME_MIN_ZOOM));

	myZoomComboBox = new wxComboBox(toolBar, SDIFRAME_ZOOM_COMBOBOX,
	wxEmptyString, wxPoint(currentX, 1), wxSize(zoomW, zoomH), numStrings,
	zoomStrings);
	delete[] zoomStrings;
	currentX += zoomW + 10;

//#ifdef __WXGTK__
	toolBar->AddControl(myZoomComboBox);
//#endif

	toolBar->AddSeparator();
	toolBar->AddTool(wxID_HELP, helpBitmap, wxNullBitmap, FALSE, currentX, -1,
	  (wxObject *) NULL, wxT("Help button"));

	// after adding the buttons to the toolbar, must call Realize() to reflect
	// the changes
	toolBar->Realize();

	toolBar->SetRows(1);

}


/****************************** OnCreateToolBar *******************************/

#if USE_GENERIC_TBAR

wxToolBar *
SDIFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	return (wxToolBar *)new wxToolBarSimple(this, id, wxDefaultPosition,
	  wxDefaultSize, style, name);

}

#endif // USE_GENERIC_TBAR

/******************************************************************************/
/****************************** Call backs ************************************/
/******************************************************************************/

/****************************** OnExecute *************************************/

void
SDIFrame::OnExecute(wxCommandEvent& WXUNUSED(event))
{
	static const wxChar *funcName = wxT("SDIFrame::OnExecute");

	if (wxGetApp().GetGrMainApp()->simThread) {
		wxLogWarning(wxT("%s: Running simulation not yet terminated!"),
		  funcName);
		return;
	}

	if (wxGetApp().GetDiagFrame())
		wxGetApp().GetDiagFrame()->Clear();
	ResetGUIDialogs();
	if (GetPtr_AppInterface()->Init) {
		if (!CheckChangedValues())
			return;
		if (!GetPtr_AppInterface()->audModel) {
			NotifyError(wxT("%s: Simulation not initialised."), funcName);
			return;
		}
	}
	if (mainParDialog) {
		if (!mainParDialog->CheckChangedValues())
			return;
		mainParDialog->parListInfoList->UpdateAllControlValues();
		mainParDialog->cancelBtn->Enable(FALSE);
	}
	ResetGUIDialogs();
	wxGetApp().GetGrMainApp()->StartSimThread();
	wxGetApp().programMenu->Enable(SDIFRAME_STOP_SIMULATION, TRUE);

}

/****************************** OnStopSimulation ******************************/

void
SDIFrame::OnStopSimulation(wxCommandEvent& WXUNUSED(event))
{

	if (!wxGetApp().GetGrMainApp()->simThread)
		return;
	wxGetApp().GetGrMainApp()->DeleteSimThread();
	wxLogWarning(wxT("Simulation terminated by user."));

}

/****************************** OnQuit ****************************************/

void
SDIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();

}

/****************************** OnHelp ****************************************/

void
SDIFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetHelpController()->DisplayContents();

}

/****************************** OnAbout ***************************************/

void
SDIFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	int		i;
	wxString title, authors, message, dsamVersion;

	if (GetPtr_AppInterface()) {
		title.sprintf(wxT("About %s %s"), GetPtr_AppInterface()->appName,
		  GetPtr_AppInterface()->appVersion);
		dsamVersion = GetPtr_AppInterface()->compiledDSAMVersion;
		for (i = 0, authors = wxT("Authors: "); i < APP_MAX_AUTHORS; i++)
			if (GetPtr_AppInterface()->authors[i][0] != '\0')
				authors += GetPtr_AppInterface()->authors[i];
	} else {
		title.sprintf(wxT("About Application"));
		dsamVersion = DSAM_VERSION;
	}
	message.sprintf(wxT("%s")
	  wxT("DSAM version: %s (dynamic), compiled with %s\n")
	  wxT("Author, Dr. Lowel P. O'Mard (with God's support)\n")
	  wxT("(c) 2001 Centre for the Neural Basis of Hearing (CNBH)\n"),
	  authors.GetData(), GetDSAMPtr_Common()->version, dsamVersion.
	  GetData());
	wxMessageBox(message, title, wxOK | wxICON_INFORMATION, this);

}

/****************************** OnEditMainPars ********************************/

/*
 * This routine creates the run paraneter dialog.
 * If the GetPtr_AppInterface()->updateProcessFlag is set, then on
 * 'OK' the simulation parameter windows and display windows will be closed.
 */

void
SDIFrame::OnEditMainPars(wxCommandEvent& WXUNUSED(event))
{
	if (!GetPtr_AppInterface()->parList)
		return;

	if (mainParDialog)
		return;
	mainParDialog = new ModuleParDialog(this, wxT("Preferences"), NULL,
	  GetPtr_AppInterface()->parList, NULL, 300, 300, 500, 500,
	  wxDEFAULT_DIALOG_STYLE);
	mainParDialog->SetNotebookSelection();
	mainParDialog->Show(TRUE);
	wxGetApp().GetFrame()->AddToDialogList(mainParDialog);

}

/****************************** OnViewSimPars *********************************/

void
SDIFrame::OnViewSimPars(wxCommandEvent& WXUNUSED(event))
{
	FILE	*oldParsFile = GetDSAMPtr_Common()->parsFile;

	SetParsFile_Common(wxT("screen"), OVERWRITE);
	ListParameters_AppInterface();
	GetDSAMPtr_Common()->parsFile = oldParsFile;

}

/****************************** OnDiagWindow *********************************/

void
SDIFrame::OnDiagWindow(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().OpenDiagWindow();

}

/****************************** OnSimThreadEvent ******************************/

void
SDIFrame::OnSimThreadEvent(wxCommandEvent& event)
{
 	switch (event.GetInt()) {
	case MYAPP_THREAD_DRAW_GRAPH: {
		SignalDispPtr	signalDispPtr = (SignalDispPtr) event.GetClientData();
		DisplayS 	*display;

		if (!signalDispPtr->display) {
			signalDispPtr->initialisationFlag = TRUE;
			display = new DisplayS(this, signalDispPtr);
			display->canvas->InitGraph();
			display->Show(TRUE);
			signalDispPtr->initialisationFlag = FALSE;
		} else {
			display = (DisplayS *) signalDispPtr->display;
			((wxCriticalSection *) signalDispPtr->critSect)->Enter();
			display->canvas->InitGraph();
			display->canvas->RedrawGraph();
			((wxCriticalSection *) signalDispPtr->critSect)->Leave();
		}
		signalDispPtr->drawCompletedFlag = TRUE;
		break; }
	default:
		;
	}

}

/*************************** OnEditProcess ************************************/

void
SDIFrame::OnEditProcess(wxCommandEvent& event)
{
	wxMenu menu(wxT("Edit Process"));

	menu.Append(SDIFRAME_CUT, wxT("&Cut"));
	menu.AppendSeparator();
	menu.Append(SDIFRAME_EDIT_PROCESS, wxT("&Properties"));

	PopupMenu(&menu, 0, 0);

}

/*************************** OnSize *******************************************/

void
SDIFrame::OnSize(wxSizeEvent& event)
{
	int		canvasX, canvasY, canvasW, canvasH;
	wxSize	size = GetClientSize();

	if (!canvas) {
		event.Skip();
		return;
	}
	if (palette) {
		int paletteX = 0;
		int paletteY = 0;
		int paletteW = 30;
		int paletteH = size.GetHeight();
		canvasX = paletteX + paletteW;
		canvasY = 0;
		canvasW = size.GetWidth() - paletteW;
		canvasH = size.GetHeight();

		palette->SetSize(paletteX, paletteY, paletteW, paletteH);
	}
	if (myToolBar) {
		myToolBar->SetSize(-1, size.y);
		myToolBar->Move(0, 0);

	}
	canvas->SetSize(canvasX, canvasY, canvasW, canvasH);

}

/******************************************************************************/
/*************************** OnCloseWindow ************************************/
/******************************************************************************/

void
SDIFrame::OnCloseWindow(wxCloseEvent& event)
{
	wxDocParentFrame::OnCloseWindow(event);
	if (!event.GetVeto())
		wxOGLCleanUp();

}

/****************************** CmpDialogs ************************************/

/*
 * This function compares dialogs for a match
 */

int
CmpDialogs_SDIFrame(void *diagNode1, void *diagNode2)
{
	ModuleParDialog	*diag1 = (ModuleParDialog * ) diagNode1;
	ModuleParDialog	*diag2 = (ModuleParDialog * ) diagNode2;

	return (diag1->pc != diag2->pc);

}

#endif /* HAVE_WX_OGL_H */
