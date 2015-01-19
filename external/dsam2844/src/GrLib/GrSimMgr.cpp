/**********************
 *
 * File:		GrSimMgr.cpp
 * Purpose:		Graphical Display Simulation Manager
 * Comments:	23-06-98 LPO: Introduced manager window resizing:
 *				MyFrame::OnSize
 *				13-09-98: LPO Added Server mode so that it is no longer
 *				necessary to press the "Go" button.
 *				28-09-98: LPO: Moved the textWindow declaration to above the
 *				menu bar, so that diagnostics can be sent to the text window
 *				when the menu bar is being set up.  Otherwise the program will
 *				crash.
 *				30-09-98 LPO: Some headers appear both in the source file and
 *				header file.  This is because they are needed in the header
 *				file for compilation, but automake needs them in the source
 *				file for generating dependancy information.
 *				05-01-99 LPO: Implemented "MyApp::RunSimulation" routine.
 *				29-01-99 LPO: The 'RunSimulation' routine now always uses the
 *				'ResetGUIDialogs()' routine.  After the first unless this
 *				routine is used all other messages are sent to the console.
 *				16-04-99 LPO: Moved the 'myApp' declaration to DSAM.h to solve
 *				some linking problems with shared libraries on IRIX 6.2.
 * Author:		L. P. O'Mard
 * Created:		06 Jan 1995
 * Updated:		16 Apr 1999
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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

// Any files included regardless of precompiled headers
#include <wx/image.h>
#include <wx/config.h>
#include <wx/thread.h>
#include <wx/dynarray.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
#include <wx/docview.h>

#if !wxUSE_CONSTRAINTS
#	error "You must set wxUSE_CONSTRAINTS to 1 in setup.h!"
#endif

#if !wxUSE_THREADS
#	error "You must enable thread support!"
#endif // wxUSE_THREADS

#if !wxUSE_STREAMS || !wxUSE_ZIPSTREAM || !wxUSE_ZLIB
       #error "You must enable streams. zipstream amd zlib support!"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#	error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtDatum.h"
#include "GeModuleMgr.h"
#include "GeModuleReg.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtAppInterface.h"
#include "UtString.h"

#include "GrSDIPalette.h"
#include "GrSDIFrame.h"
#include "GrSDIDiagram.h"
#include "GrSDIDoc.h"
#include "GrSDIView.h"
#include "GrSDIDocManager.h"
#include "GrDiagFrame.h"
#include "ExtIPCUtils.h"
#include "ExtSocket.h"
#include "ExtSocketServer.h"
#include "GrIPCServer.h"
#include "DiSignalDisp.h"
#include "GrCanvas.h"
#include "GrPrintDisp.h"
#include "GrFonts.h"
#include "ExtSimThread.h"
#include "GrMainApp.h"
#include "GrBrushes.h"
#include "GrSimMgr.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

int		grSimMgrPleaseLink = 0;

/* Create a new application object: this macro will allow wxWindows to
 * create the application object during program execution (it's better than
 * using a static object for many reasons) and also declares the accessor
 * function wxGetApp() which will return the reference of the right type
 * (i.e. MyApp and not wxApp)
 */

#if !defined(WIN32)
	IMPLEMENT_APP(MyApp)
#else
	DSAM_IMPLEMENT_APP_NO_MAIN(MyApp)
#endif

#ifdef wx_mac
void CreateApp(void)
{
}
#endif // wx_mac

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

BEGIN_EVENT_TABLE(MyApp, wxApp)
	EVT_SOCKET(IPC_APP_SERVER_ID,	MyApp::OnServerEvent)
	EVT_SOCKET(IPC_APP_SOCKET_ID,	MyApp::OnSocketEvent)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** MyApp Methods *********************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * "Testing of ressources"
 * I'm not quite sure why this is here, but it is in the example hello.cpp, so
 * I have put it in.
 */

MyApp::MyApp(void): help(wxHF_DEFAULTSTYLE | wxHF_OPENFILES)
{
	SetUsingGUIStatus(TRUE);
	displayDefaultX = 0;
	displayDefaultY = 0;
	frame = NULL;
	iPCServer = NULL;
	grMainApp = NULL;
	dataInstallDir = DSAM_DATA_INSTALL_DIR;
	icon = NULL;
	myDocManager = NULL;
	diagFrame = NULL;
	fileMenu = editMenu = viewMenu = programMenu = NULL;
	helpCount = 0;
	audModelLoadedFlag = false;
	SetDPrintFunc(DPrint_MyApp);
	SetNotifyFunc(Notify_MyApp);

}

/****************************** OnExit ****************************************/

/*.
 */

int
MyApp::OnExit(void)
{
	wxOGLCleanUp();
	delete printData;

	if (iPCServer)
		delete iPCServer;
	if (myDocManager)
		delete myDocManager;

	delete wxConfigBase::Set((wxConfigBase *) NULL);
	delete grMainApp;
	return(0);

}

/****************************** OnInit ****************************************/

/*
 * The `main program' equivalent, creating the windows and returning the
 * main frame.
 * The first call to 'ResetGUIDialogs' ensures that the initial diagnostics
 * are sent to the GUI support routines.
 */

bool
MyApp::OnInit(void)
{
	wxOGLInitialize();
	/* static const char *funcName = "MyApp::OnInit"; */

	// Setup the printing global variables.
    printData = new wxPrintData;
#	if defined(__WXGTK__) || defined(__WXMOTIF__)
//		*printData = *wxThePrintSetupData;
#	endif

	SwitchGUILocking_Common(FALSE);

	// Set brushes
	greyBrushes = new GrBrushes();
	
	// Set fonts
	Init_Fonts();

	// Image handlers
	wxInitAllImageHandlers();

	// Required for advanced HTML help
	wxFileSystem::AddHandler(new wxZipFSHandler);

	// Get config setup
	SetVendorName(wxT("CNBH"));

	// Get config setup
	pConfig = wxConfigBase::Get();

	grMainApp = new GrMainApp(argc, argv);

	if (grMainApp->GetServerFlag()) {
		if (!InitServer())
			return(FALSE);
	}

	InitHelp();

	// Create a document manager
	myDocManager = new SDIDocManager;
	// Create templates relating drawing documents to their views
	(void) new wxDocTemplate(myDocManager, wxT("Simulation Script"), wxT("*.")
	  SDI_DOCUMENT_XML_FILE_EXT, grMainApp->GetSimFileName().GetCwd(),
	  SDI_DOCUMENT_XML_FILE_EXT, wxT("Simulation Design"), wxT(
	  "Simulation view"),CLASSINFO(SDIDocument), CLASSINFO(SDIView));
	(void) new wxDocTemplate(myDocManager, wxT("Simulation Parameter File"),
	  wxT("*.") SDI_DOCUMENT_SPF_FILE_EXT, grMainApp->GetSimFileName().GetCwd(),
	  SDI_DOCUMENT_SPF_FILE_EXT, wxT("Simulation Design"), wxT("Simulation ")
	  wxT("view"), CLASSINFO(SDIDocument), CLASSINFO(SDIView));
	(void) new wxDocTemplate(myDocManager, wxT("Simulation Script"), wxT("*.")
	  SDI_DOCUMENT_SIM_FILE_EXT, grMainApp->GetSimFileName().GetCwd(),
	  SDI_DOCUMENT_SIM_FILE_EXT, wxT("Simulation Design"), wxT(
	  "Simulation view"), CLASSINFO(SDIDocument), CLASSINFO(SDIView));
	(void) new wxDocTemplate(myDocManager, wxT("Simulation Script"), wxT("*.")
	  SDI_DOCUMENT_ALL_FILES_EXT, grMainApp->GetSimFileName().GetCwd(),
	  SDI_DOCUMENT_XML_FILE_EXT, wxT("Simulation Design"), wxT(
	  "Simulation view"),CLASSINFO(SDIDocument), CLASSINFO(SDIView));
	myDocManager->SetMaxDocsOpen(1);

	// Get frame position and size
	pConfig->SetPath(SIM_MANAGER_REG_MAIN_FRAME);
	int		x = pConfig->Read(wxT("x"), (long int) 0);
	int		y = pConfig->Read(wxT("y"), (long int) 0);
	int		w = pConfig->Read(wxT("w"), (long int) SIM_MANAGER_DEFAULT_WIDTH);
	int		h = pConfig->Read(wxT("h"), (long int) SIM_MANAGER_DEFAULT_HEIGHT);

	// Create the main frame window
	frame = new SDIFrame(myDocManager, (wxFrame *) NULL, wxT(
	  "Simulation Manager"), wxPoint(x, y), wxSize(w, h),
	  wxDEFAULT_FRAME_STYLE);

	frame->SetMenuBar(CreateMenuBar());
	frame->editMenu = editMenu;
	myDocManager->FileHistoryUseMenu(fileMenu);
	myDocManager->FileHistoryLoad(*pConfig);

	frame->canvas = frame->CreateCanvas(NULL, frame);

	if (GetPtr_AppInterface())
		SetAppName(GetPtr_AppInterface()->appName);
	else
		SetAppName(wxT("DSAM_App"));

	// Set up process lists for dialogs
	CreateProcessLists();

	if (GetPtr_AppInterface()->simulationFileFlag)
		CreateDocument(GetPtr_AppInterface()->simulationFile);
	else
		myDocManager->CreateDocument(wxT(""), wxDOC_NEW);

    // Show frame and tell the application that it's our main window

	frame->Show(TRUE);
   //SetTopWindow(frame);

	// Essential - return the main frame window
	return(TRUE);

}

/****************************** InitHelp **************************************/

/*
 * This routine initialises the help.
 */

void
MyApp::InitHelp(void)
{
	help.UseConfig(wxConfig::Get());
	// help.SetTempDir(wxT("."));  -- causes crashes on solaris
	if (GetPtr_AppInterface()) {
		AddHelpBook(SIM_MANAGER_REG_APP_HELP_PATH, wxString((wxChar *)
		  GetPtr_AppInterface()->installDir) + wxT("/") + SIM_MANAGER_HELP_DIR,
		  wxString((wxChar *) GetPtr_AppInterface()->appName).Upper() + wxT(
		  "Help"));
		AddHelpBook(SIM_MANAGER_REG_DSAM_HELP_PATH, wxString(
		  DSAM_DATA_INSTALL_DIR) + wxT("/") + SIM_MANAGER_HELP_DIR,
		  DSAM_PACKAGE);
	}

}

/****************************** CreateMenus ***********************************/

/*
 * This routine creates the application menus
 */

wxMenuBar *
MyApp::CreateMenuBar(void)
{
	fileMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);
	fileMenu->Append(wxID_NEW, wxT("&New\tCtrl-N"), wxT("Design new ")
	  wxT("simulation."));
	fileMenu->Append(wxID_OPEN, wxT("&Open...\tCtrl-O"), wxT("Load Simulation ")
	  wxT("from file."));
//	fileMenu->Append(wxID_REVERT, wxT("&Reload\tCtrl-R"), wxT("Reload ")
//	  wxT("Simulation."));
	fileMenu->Append(wxID_CLOSE, wxT("&Close\tCtrl-W"));
 	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_SAVE,  wxT("&Save...\tCtrl-S"),  wxT("Save ")
	  wxT("simulation parameter file."));
	fileMenu->Append(wxID_SAVEAS,  wxT("Save as..."),  wxT("Save simulation ")
	  wxT("parameter file..."));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, wxT("E&xit\tCtrl-Q"), wxT("Exit from ")
	  wxT("program."));
	myDocManager->FileHistoryUseMenu(fileMenu);

	editMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);
	editMenu->Append(wxID_UNDO, wxT("&Undo\tCtrl-Z"));
	editMenu->Append(wxID_REDO, wxT("&Redo\tCtrl-Y"));
	editMenu->AppendSeparator();
	editMenu->Append(SDIFRAME_CUT, wxT("&Cut\tCtrl-X"));
	editMenu->AppendSeparator();
	editMenu->Append(SDIFRAME_EDIT_MAIN_PARS, wxT("&Preferences...")
	  wxT("\tCtrl-M"), wxT("Edit main program preferences"));

	viewMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);
	viewMenu->Append(MYFRAME_ID_VIEW_SIM_PARS, wxT("&Simulation parameters"),
	  wxT("List simulation parameters (to diagnostic window)"));

	programMenu = new wxMenu;
	programMenu->Append(SDIFRAME_EXECUTE, wxT("&Execute\tCtrl-G"), wxT(
	  "Execute simulation"));
	programMenu->Append(SDIFRAME_STOP_SIMULATION, wxT("S&top ")
	  wxT("simulation\tCtrl-C"), wxT("Stop simulation execution."));

	windowsMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);
	windowsMenu->Append(SDIFRAME_DIAG_WINDOW, wxT("Diagnostic Window"),
	  wxT("Open diagnostics window."));

	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(SDIFRAME_ABOUT, wxT("&About...\tCtrl-A"),
	  wxT("Show about dialog"));
	helpMenu->Append(wxID_HELP, wxT("&Help Topics...\tCtrl-H"));
	if (!helpCount)
		helpMenu->Enable(wxID_HELP, FALSE);

	// Disable unusable menus
	if (!GetPtr_AppInterface() || !GetPtr_AppInterface()->Init ||
	  !GetPtr_AppInterface()->audModel) {
		EnableSimParMenuOptions(FALSE);
	}
	programMenu->Enable(SDIFRAME_STOP_SIMULATION, FALSE);

	// Make a menubar
	wxMenuBar *menuBar = new wxMenuBar((long) (wxMB_DOCKABLE));

	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(editMenu, wxT("&Edit"));
	menuBar->Append(viewMenu, wxT("&View"));
	menuBar->Append(programMenu, wxT("&Program"));
	menuBar->Append(windowsMenu, wxT("&Windows"));
	menuBar->Append(helpMenu, wxT("&Help"));
	return(menuBar);

}

/****************************** GetProcessList ********************************/

// This routine returns a pointer to a process list.

wxArrayString *
MyApp::GetProcessList(int classSpecifier)
{
	static const char *funcName = "MyApp::GetProcessList";

	switch (classSpecifier) {
	case ANALYSIS_MODULE_CLASS:
		return(&wxGetApp().anaList);
	case CONTROL_MODULE_CLASS:
		return(&wxGetApp().ctrlList);
	case DISPLAY_MODULE_CLASS:
		return(&wxGetApp().displayList);
	case FILTER_MODULE_CLASS:
		return(&wxGetApp().filtList);
	case IO_MODULE_CLASS:
		return(&wxGetApp().ioList);
	case MODEL_MODULE_CLASS:
		return(&wxGetApp().modelsList);
	case TRANSFORM_MODULE_CLASS:
		return(&wxGetApp().transList);
	case USER_MODULE_CLASS:
		return(&wxGetApp().userList);
	case UTILITY_MODULE_CLASS:
		return(&wxGetApp().utilList);
	default:
		NotifyError(wxT("%s: Unknown process type (%d).\n"), funcName,
		  classSpecifier);
		return(NULL);
	}

}

/****************************** CreateProcessLists ****************************/

// This routine creates the lists of processes for the dialog windows.
// The 

void
MyApp::CreateProcessLists(void)
{
	SymbolPtr	p, symList = NULL;
	ModRegEntryPtr	modReg;

	for (modReg = LibraryList_ModuleReg(0); modReg->name; modReg++)
		GetProcessList(modReg->classSpecifier)->Add((wxChar *) modReg->name);
	for (modReg = UserList_ModuleReg(0); modReg && modReg->name;
	  modReg++)
		userList.Add((wxChar *) modReg->name);
	InitKeyWords_Utility_SSSymbols(&symList);
	for (p = symList; p; p = p->next) {
		if ((p->type == STOP) || (p->type == BEGIN))
			continue;
		ctrlList.Add((wxChar *) p->name);
	}
	FreeSymbols_Utility_SSSymbols(&symList);

}

/****************************** EnableSimParMenuOptions ***********************/

// Intercept menu commands

void
MyApp::EnableSimParMenuOptions(bool on)
{
	/*
	**** to be removed ?***
	fileMenu->Enable(MYFRAME_ID_SAVE_SIM_PARS, on);
	editMenu->Enable(MYFRAME_ID_EDIT_SIM_PARS, on);
	viewMenu->Enable(MYFRAME_ID_VIEW_SIM_PARS, on);
	*/
	programMenu->Enable(SDIFRAME_EXECUTE, on);

}

/****************************** InitServer ************************************/

/*
 * This routine sets up the general parameters for the server mode.
 */

bool
MyApp::InitServer(void)
{
	wxIPV4address	addr;

	iPCServer = new GrIPCServer(wxT(""), grMainApp->GetServerPort());
	if (iPCServer->Ok())
		iPCServer->SetNotification(this);
	return(TRUE);
			
}
			
/****************************** ExitMain **************************************/

/*
 * This saves the configuration and closes all DSAM interfaces.
 */

void
MyApp::ExitMain(void)
{
	if (GetPtr_AppInterface())
		SaveConfiguration(GetPtr_AppInterface()->parList);

	// save the control's values to the config
	if ( pConfig != NULL ) {
		// save the frame position
		pConfig->SetPath(SIM_MANAGER_REG_MAIN_FRAME);
		int		x, y, w, h;
		frame->GetSize(&w, &h);
		frame->GetPosition(&x, &y);
		pConfig->Write(wxT("x"), (long) x);
		pConfig->Write(wxT("y"), (long) y);
		pConfig->Write(wxT("w"), (long) w);
		pConfig->Write(wxT("h"), (long) h);
		myDocManager->FileHistorySave(*pConfig);
	}

}

/****************************** CreatDocument *********************************/

/*
 */

void
MyApp::CreateDocument(const wxString& fileName)
{
	wxDocument *doc = myDocManager->CreateDocument(fileName, wxDOC_SILENT);
	myDocManager->CreateView(doc);

}

/****************************** GetDefaultDisplayPos **************************/

/*
 * This routine sets the argument variables to the next default display
 * position.
 */

void
MyApp::GetDefaultDisplayPos(int *x, int *y)
{
	*x = displayDefaultX;
	*y = displayDefaultX;
	displayDefaultX += 15;
	displayDefaultY += 10;

}

/****************************** SetConfiguration ******************************/

/*
 * This attempts to set the initial configuration from the saved configuration
 */

void
MyApp::SetConfiguration(UniParListPtr parList)
{
	int		i, j;
	wxString	indexedName, parValue, indexedParValue;
	UniParPtr	p;

	if (!parList)
		return;

	wxConfigBase *pConfig = wxConfigBase::Get();
 	pConfig->SetPath(SIM_MANAGER_REG_MAIN_PARS);

	for (i = 0; i < parList->numPars; i++) {
		p = &parList->pars[i];
		switch (p->type) {
		case UNIPAR_PARLIST:
			if (p->valuePtr.parList.process)
				SET_PARS_POINTER(*p->valuePtr.parList.process);
			SetConfiguration(*p->valuePtr.parList.list);
			break;
		case UNIPAR_INT_ARRAY:
		case UNIPAR_REAL_ARRAY:
		case UNIPAR_STRING_ARRAY:
		case UNIPAR_NAME_SPEC_ARRAY:
			for (j = 0; j < *p->valuePtr.array.numElements; j++) {
				indexedName.Printf(wxT("%s.%d"), p->abbr, j);
				parValue = pConfig->Read(indexedName, wxT(""));
				if (parValue.Len() != 0) {
					indexedParValue.Printf(wxT("%d:%s"), j, parValue.c_str());
					SetParValue_UniParMgr(&parList, i, (wxChar *) 
					  indexedParValue.c_str());
				}
			}
			break;
		default:
			parValue = pConfig->Read((wxChar *) p->abbr, wxT(""));
			if (parValue.Len() != 0)
				SetParValue_UniParMgr(&parList, i, (wxChar *) parValue.c_str());
		}
	}
	SetSimulationFileFlag_AppInterface(FALSE); //Don't use sim file name

}

/****************************** SaveConfiguration *****************************/

/*
 * This routine saves the configuration
 */

void
MyApp::SaveConfiguration(UniParListPtr parList, const wxString& processSuffix)
{
	int		i, j, oldIndex;
	UniParPtr	p;
	wxString	name, value;

	if (!parList)
		return;

	wxConfigBase *pConfig = wxConfigBase::Get();
 	pConfig->SetPath(SIM_MANAGER_REG_MAIN_PARS);

	for (i = 0; i < parList->numPars; i++) {
		p = &parList->pars[i];
		name = FormatPar_UniParMgr(p, (wxChar *) processSuffix.c_str());
		switch (p->type) {
		case UNIPAR_PARLIST:
			if (p->valuePtr.parList.process)
				SET_PARS_POINTER(*p->valuePtr.parList.process);
			SaveConfiguration(*p->valuePtr.parList.list, wxT(".") + wxString(
			  (wxChar *) p->abbr));
			break;
		case UNIPAR_INT_ARRAY:
		case UNIPAR_REAL_ARRAY:
		case UNIPAR_STRING_ARRAY:
		case UNIPAR_NAME_SPEC_ARRAY: {
			oldIndex = p->valuePtr.array.index;
			for (j = 0; j < *p->valuePtr.array.numElements; j++) {
				oldIndex = p->valuePtr.array.index;
				p->valuePtr.array.index = j;
				value.Printf(wxT("%d:%s"), j, GetParString_UniParMgr(p));
				pConfig->Write(name, value);
			}
			p->valuePtr.array.index = oldIndex;
			break; }
		default:
			;
		}
	}

}

/****************************** OpenDiagWindow ********************************/

void
MyApp::OpenDiagWindow(void)
{
	if (diagFrame)
		return;

	diagFrame = new DiagFrame(frame, wxT("DSAM Diagnostics"));
	diagFrame->Show(TRUE);

}

/****************************** CloseDiagWindow *******************************/

void
MyApp::CloseDiagWindow(void)
{
	delete diagFrame;
	diagFrame = NULL;

}

/****************************** AddHelpBook ***********************************/

void
MyApp::AddHelpBook(const wxString& path, const wxString& defaultPath,
  const wxString& fileName)
{
	wxString	helpFilePath;
	wxFileName	helpFile;

	pConfig->SetPath(SIM_MANAGER_REG_PATHS);
	helpFilePath = pConfig->Read(path, wxT(""));
	if (GetPtr_AppInterface()) {
		if (!helpFilePath.Len())
			helpFilePath = defaultPath;
		helpFile = wxFileName(helpFilePath, fileName, wxT("zip"));
	} else
		helpFile = wxFileName(defaultPath, wxT("DSAMApp.zip"));
	if (helpFile.FileExists() && help.AddBook(helpFile))
		 helpCount++;

}

/****************************** OnServerEvent *********************************/

/*
 * Server socket event handler routine.
 */

void
MyApp::OnServerEvent(wxSocketEvent& event)
{
	static const char *funcName = "MyApp::OnServerEvent";
	wxString	salutation;

	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
	if (event.GetSocketEvent() != wxSOCKET_CONNECTION) {
		NotifyError(wxT("%s: Unexpected socket event."), funcName);
		return;
	}
	SocketBase *sock = iPCServer->InitConnection(false);
	if (!sock) {
		NotifyError(wxT("%s: Couldn't initialise connection.\n"));
		return;
	}
	sock->SetEventHandler(*this, IPC_APP_SOCKET_ID);
	sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	sock->Notify(TRUE);
	SetDiagMode(COMMON_DIALOG_DIAG_MODE);

}

/****************************** OnSocketEvent *********************************/

/*
 * Socket event handler routine.
 */

void
MyApp::OnSocketEvent(wxSocketEvent& event)
{
	wxSocketBase *sock = event.GetSocket();

	SetDiagMode(COMMON_DIALOG_DIAG_MODE);
	switch (event.GetSocketEvent()) {
	case wxSOCKET_INPUT: {
		iPCServer->ProcessInput();
		break; }
	case wxSOCKET_LOST:
		sock->Destroy();
		break;
	case wxSOCKET_CONNECTION:
		break;
	default:
		;
	}
	SetDiagMode(COMMON_DIALOG_DIAG_MODE);

}

/****************************** OnExit ****************************************/

/*
 * This routine posts an exit event.
 * It expects to be called by the operating routine's "OnExit" routine.
 * So it cannot be a member function.
 */

void
OnExit_MyApp(void)
{
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_EXIT);

	wxPostEvent(wxGetApp().GetFrame(), event);

}

/****************************** OnExecute *************************************/

/*
 * This routine posts an execute event..
 * It expects to be called by the operating routine's "RunSimulation" routine.
 * So it cannot be a member function.
 */

BOOLN
OnExecute_MyApp(void)
{
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, SDIFRAME_EXECUTE);

	wxPostEvent(wxGetApp().GetFrame(), event);
	return(TRUE);

}

/*************************** EmptyDiagWinBuffer *******************************/

/*
 * This routine prints the print buffer, then resets its counter to zero.
 * It expects the count 'c' to point beyond the last character.
 */

void
EmptyDiagWinBuffer_MyApp(wxChar *s, size_t *c)
{
	*(s + *c) = '\0';
	*(wxGetApp().GetDiagFrame()->diagnosticsText) << s;
	*c = 0;

}

/*************************** DPrint *******************************************/

/*
 * This routine prints out a diagnostic message, preceded by a bell sound.
 * It is used in the same way as the printf statement.
 * There are different versions for the different compile options.
 * It assumes that all real values are 'double'.
 * It does not check that the format string is less than SMALL_STRING
 * characters.
 * The 'buffer' string is used to retain control of the formatting, as WxWin
 * does not yet have sufficient controls on the number output using the '<<'
 * operator.
 */
 
void
DPrint_MyApp(const wxChar *format, va_list args)
{
	if (!wxGetApp().GetDiagFrame())
		return;
	if (GetDSAMPtr_Common()->lockGUIFlag)
		wxMutexGuiEnter();
	DPrintBuffer_Common(format, args, EmptyDiagWinBuffer_MyApp);
	if (GetDSAMPtr_Common()->lockGUIFlag)
		wxMutexGuiLeave();
	
}

/***************************** Notify *****************************************/

/*
 * A message window dialog is opened when this routine is called.
 * The '#ifdef' compile options are required because it seems that the
 * 'wxMessageBox' is thread safe under windows, but not under Linux.
 */

void
Notify_MyApp(const wxChar *message, CommonDiagSpecifier type)
{
	const wxChar	*heading;
	wxString	mesg;
	long	style = wxOK;

	if (!GetDSAMPtr_Common()->notificationCount) {
		if (type == COMMON_GENERAL_DIAGNOSTIC_WITH_CANCEL)
			style |= wxCANCEL;
		if ((wxMessageBox(message, DiagnosticTitle(type),
		  style) == wxCANCEL) && wxGetApp().GetGrMainApp()->simThread) {
			wxGetApp().GetGrMainApp()->DeleteSimThread();
			wxLogWarning(wxT("Simulation terminated by user."));
		}
			
		if ((type != COMMON_GENERAL_DIAGNOSTIC_WITH_CANCEL) ||
		  GetDSAMPtr_Common()->interruptRequestedFlag)
			SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
		heading = wxT("\nDiagnostics:-\n");
	} else
		heading = wxT("");
	mesg.Printf(wxT("%s%s\n"), heading, message);
	DPrint((wxChar *) message);

}
