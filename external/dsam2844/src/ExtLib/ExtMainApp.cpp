/**********************
 *
 * File:		ExtMainApp.cpp
 * Purpose:		This routine contains the main handling code running simulations
 *				using the extensions library.
 * Comments:	This routine contains the main routine for when the non-GUI mode
 *				is used.
 * Author:		L. P. O'Mard
 * Created:		12 Dec 2003
 * Updated:
 * Copyright:	(c) 2003, 2010 Lowel P. O'Mard
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

#include <ExtCommon.h>

#include <wx/app.h>
#include <wx/socket.h>
#include <wx/tokenzr.h>

#include "GeCommon.h"
#if USE_FFTW3_THREADS
#	include <fftw3.h>
#endif /* USE_FFTW3_THREADS */
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeNSpecLists.h"
#include "GeModuleMgr.h"
#include "UtString.h"
#include "UtOptions.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "ExtSimThread.h"
#include "ExtIPCUtils.h"
#include "ExtIPCClient.h"
#include "ExtSocket.h"
#include "ExtSocketServer.h"
#include "ExtIPCServer.h"
#include "ExtRunThreadedProc.h"
#include "ExtXMLDocument.h"
#include "ExtMainApp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

DSAMEXT_API MainApp *dSAMMainApp = NULL;
DSAMEXT_API int (* MainSimulation_MainApp)(void) = MAINAPP_MAIN_SIM_INIT;

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * This is the main routine for using the extensions library, and is called by
 * the "main" subroutine in the header file.
 */

MainApp::MainApp(int theArgc, wxChar **theArgv, int(*TheExternalMain)(void),
		int(*TheExternalRunSimulation)(void)) {
	static const WChar *funcName = wxT("MainApp::MainApp");
	initOk = true;
	argc = 0;
	argv = NULL;
	if (!InitCommandLineArgs(theArgc, theArgv)) {
		NotifyError(wxT("%s: Could not initialise command-line arguments.\n"),
				funcName);
		return;
	}
	ExternalMain = TheExternalMain;
	threadedSimExecutionFlag = true; /* should be set by command-line */
	ExternalRunSimulation =
			(TheExternalRunSimulation) ?
					TheExternalRunSimulation : TheExternalMain;
	SetReadXMLSimFile_Utility_SimScript(ReadXMLSimFile_MainApp);
	serverFlag = false;
	superServerFlag = false;
	diagsOn = false;
	DSAM_strcpy(serverHost, wxT("localhost"));
	serverPort = EXTIPCUTILS_DEFAULT_SERVER_PORT;
	myClient = NULL;
	simThread = NULL;
	dSAMMainApp = this;
	SetUsingExtStatus(TRUE);
	symList = NULL;
	InitKeyWords_Utility_SSSymbols(&symList);
	if (ExternalMain)
		initOk = InitRun();
	if (initOk)
		CheckOptions();
#	if USE_FFTW3_THREADS
	if (!DSAM_FFTW_NAME(init_threads)()) {
		NotifyError(wxT("%s: Could not initialise FFTW3 threading."), funcName);
		return;
	}
	DSAM_FFTW_NAME(plan_with_nthreads)((GetPtr_AppInterface()->numThreads < 1)? 1:
			GetPtr_AppInterface()->numThreads);
#	endif /* USE_FFTW3_THREADS */
	runThreadedProc = new RunThreadedProc();

}

/****************************** Destructor ************************************/

//
// This routine does not call wxUninitialize() because there are errors with
// trying to re-call wxInitialize() - so it needs to be only called.
//
MainApp::~MainApp(void) {
	DeleteSimThread();
	if (myClient)
		delete myClient;
	if (runThreadedProc)
		delete runThreadedProc;
#	if USE_FFTW3_THREADS
	DSAM_FFTW_NAME(cleanup_threads)();
#	endif /* USE_FFTW3_THREADS */
	FreeArgStrings();
	FreeSymbols_Utility_SSSymbols(&symList);
	SetCanFreePtrFlag_AppInterface(TRUE);
	if (!initOk)
		CloseFiles();
	Free_AppInterface();
	FreeAll_EarObject();
	FreeNull_ModuleMgr();
	dSAMMainApp = NULL;

}

/****************************** InitCommandLineArgs ***************************/

/*
 * This routine prepares space and initialises the command line arguments.
 * It is no longer necessary to convert the arguments from muiltbye->to wide char
 * format, as this is now done in DSAMStart_MainApp if necessary.
 */

bool MainApp::InitCommandLineArgs(int theArgc, wxChar **theArgv) {
	int i;

	argc = theArgc;
	if (!InitArgv(argc)) {
		NotifyError(wxT("%s: Could not initialise argv pointer list.\n"));
		return false;
	}
	for (i = 0; i < argc; i++)
		SetArgvString(i, theArgv[i], DSAM_strlen(theArgv[i]));
	return (true);

}

/****************************** Main ******************************************/

/*
 */

int MainApp::Main(void) {
	static const wxChar *funcName = wxT("MainApp::Main");
	bool ok = false;

	if (!initOk) {
		NotifyWarning(wxT("%s: Program not using application interface or not ")
		wxT("correctly initialised.\n"), funcName);
		return (false);
	}
	if (serverFlag)
		return (RunIPCMode());
	if (!InitMain(TRUE)) {
		NotifyError(wxT("%s: Could not initialise the main program."),
				funcName);
		return (false);
	}
	if (!threadedSimExecutionFlag)
		return (RunSimulation());
	StartSimThread(wxTHREAD_JOINABLE);
	ok = CXX_BOOL(simThread->Wait());
	delete simThread;
	return (ok);

}

/****************************** RunIPCMode ************************************/

/*
 * This function runs in IPC mode if there is server on the same host and
 * port, then run the client mode.
 * Note that the diagnostics are set to off by default.  Only the -d option
 * can turn them on.
 */

int MainApp::RunIPCMode(void) {
	if (diagsOn)
		SetDiagMode(COMMON_DIALOG_DIAG_MODE);
	else
		SetDiagMode(COMMON_OFF_DIAG_MODE);
	InitMain(FALSE);
	if (!CreateClient(serverHost, serverPort))
		return (RunServer());
	return (RunClient());

}

/****************************** RunClient ************************************/

/*
 * This function runs the simulation in client mode.
 * The function deals with piped signals by the server setting up for piped
 * signals and the client then asking if an input (piped) signal is required.
 */

int MainApp::RunClient(void) {
	static const wxChar *funcName = wxT("MainApp::RunClient");

	if (!GetClient()->SendArguments(GetArgc(), GetArgv())) {
		NotifyError(wxT("%s: Could not initialise remote simulation."),
				funcName);
		return (false);
	}
	if (!GetClient()->InitSimFromFile(GetPtr_AppInterface()->simulationFile)) {
		NotifyError(wxT("%s: Could not initialise remote simulation."),
				funcName);
		return (false);
	}
#	if DSAM_DEBUG
	for (int i = 0; i < GetArgc(); i++)
	printf("%s: %2d: %s\n", funcName, i, GetArgv()[i]);
#	endif /* DSAM_DEBUG */
// 	if (inputData) {
// 		if (!InitInputEarObject(length)) {
// 			NotifyError(wxT("%s: Could not initialise input process."),
// 			  funcName);
// 			return(false);
// 		}
// 		if (!GetClient()->GetIPCUtils()->InitOutProcess()) {
// 			NotifyError(wxT("%s: Could not initialise output process for input ")
// 			  wxT("data."), funcName);
// 			return(false);
// 		}
// 		GetClient()->GetIPCUtils()->ConnectToOutProcess(inputProcess);
// 		if (!GetClient()->SendInputProcess()) {
// 			NotifyError(wxT("%s: Could not send the input data."), funcName);
// 			return(false);
// 		}
// 	}
	if (!GetClient()->RunSimulation()) {
		NotifyError(wxT("%s: Could not run remote simulation."), funcName);
		return (false);
	}
	GetClient()->GetAllOutputFiles();
	return (true);

	return (0);

}

/****************************** RunServer ************************************/

/*
 * This function runs in server mode.
 * Note that the diagnostics are set to off by default.  Only the -d option
 * can turn them on.
 */

int MainApp::RunServer(void) {
	static const wxChar *funcName = wxT("MainApp::RunServer");

	SetOnExecute_AppInterface(OnExecute_MainApp);
	SetDPrintFunc(DPrintSysLog_MainApp);
	if (diagsOn)
		SetDiagMode(COMMON_DIALOG_DIAG_MODE);
	else
		SetDiagMode(COMMON_OFF_DIAG_MODE);
	InitMain(FALSE);
	IPCServer *server = new IPCServer(wxT(""), serverPort, superServerFlag);
	if (!server->Ok()) {
		NotifyError(wxT("%s: Could not start server on port %u.\n"), funcName,
				serverPort);
		return (false);
	}
	for (;;) {
		SocketBase *socket = server->InitConnection();
		if (!socket) {
			NotifyError(wxT("%s: Connection failed."), funcName);
			break;
		}
		while (socket->IsConnected()) {
			while (server->ProcessInput())
				;
			delete socket;
			if (superServerFlag)
				return (0);
			break;
		}

	}
	return (0);

}

/****************************** CheckOptions **********************************/

/*
 * This routine checks for the program call options.
 * It sets the ones only to be used here to be ignored by other routines.
 * Two instances of the "-S" flag set the super server flag indicating that the
 * server is to be run by xinetd/inetd.
 */

void MainApp::CheckOptions(void) {
	int optInd = 1, optSub = 0;
	wxChar c, *argument;

	while ((c = Process_Options(argc, argv, &optInd, &optSub, &argument,
			wxT("R:SI:d:"))))
		switch (c) {
		case 'R':
			DSAM_strcpy(serverHost, argument);
			MarkIgnore_Options(argc, argv, wxT("-R"), OPTIONS_WITH_ARG);
			break;
		case 'S':
			if (!serverFlag)
				serverFlag = TRUE;
			else
				superServerFlag = TRUE;
			MarkIgnore_Options(argc, argv, wxT("-S"), OPTIONS_NO_ARG);
			break;
		case 'I':
			serverPort = DSAM_atoi(argument);
			MarkIgnore_Options(argc, argv, wxT("-I"), OPTIONS_WITH_ARG);
			break;
		case 'd':
			SetDiagMode_AppInterface(argument);
			switch (GetPtr_AppInterface()->diagModeSpecifier) {
			case GENERAL_DIAGNOSTIC_SCREEN_MODE:
			case GENERAL_DIAGNOSTIC_ERROR_MODE:
				diagsOn = true;
				break;
			default:
				;
			} /* switch */
			break;
		default:
			;
		} /* switch */

}

/****************************** InitArgv **************************************/

/*
 * This allocates the memory for the argv pointer.
 */

bool MainApp::InitArgv(int theArgc) {
	static const wxChar *funcName = wxT("MainApp::InitArgv");

	if (!theArgc)
		return (true);
	argc = theArgc;
	if ((argv = (wxChar **) calloc(argc, sizeof(wxChar *))) == NULL) {
		NotifyError(wxT("%s: Out of memory for argv[%d] array."), funcName,
				argc);
		return (false);
	}
	return (true);

}

/****************************** SetArgvString *********************************/

bool MainApp::SetArgvString(int index, const wxChar *string, size_t size) {
	static const wxChar *funcName = wxT("MainApp::SetArgvString");

	if (argv[index])
		free(argv[index]);
	if ((argv[index] = (wxChar *) calloc(size + 1, sizeof(wxChar))) == NULL) {
		NotifyError(wxT("%s: Out of memory for argv[%d]."), funcName, index);
		return (FALSE);
	}
	DSAM_strcpy(argv[index], string);
	return (TRUE);

}

/****************************** ProtectQuotedStr ******************************/

/*
 * This function replaces characters between quotes so the strings are reqarded
 * as non-delimited.
 * The speech marks are also replaced with spaces.
 * It returns FALSE if it enounters an error.
 */

/* ToDo: implement this using wxString. */

bool MainApp::ProtectQuotedStr(wxChar *str) {
	static const wxChar *funcName = wxT("MainApp::ProtectQuotedStr");
	bool quotesOn = FALSE;
	wxChar *p;

	for (p = str; *p != '\0'; p++) {
		if (*p == MAINAPP_QUOTE) {
			quotesOn = (!quotesOn);
			*p = ' ';
		} else if (quotesOn && (*p == ' '))
			*p = MAINAPP_SPACE_SUBST;
	}
	if (quotesOn) {
		NotifyError(wxT("%s: Incomplete quoted string in parameters."),
				funcName);
		return (FALSE);
	}
	return (TRUE);

}

/****************************** RestoreQuotedStr ******************************/

/*
 * This routine returns the spaces to a protected quoted string.
 */

wxChar *
MainApp::RestoreQuotedStr(wxChar *str) {
	wxChar *p;

	for (p = str; *p != '\0'; p++)
		if (*p == MAINAPP_SPACE_SUBST)
			*p = ' ';
	return (str);

}

/****************************** SetParameterOptionArgs ************************/

/*
 * This function extracts the parameter options from a string.
 * In the count mode it only counts the parameter options, it does not add it to
 * the global argv list.
 * It returns the number of parameters, or a negative number if an error occurs.
 */

int MainApp::SetParameterOptionArgs(int indexStart,
		const wxChar *parameterOptions, bool countOnly) {
	static const wxChar *funcName = wxT("MainApp::SetParameterOptionArgs");
	wxString token, workStr = parameterOptions;
	int count = 0;

	if (!ProtectQuotedStr((wxChar *) workStr.c_str())) {
		NotifyError(wxT("%s; Could note protect quoted parameters."), funcName);
		return (-1);
	}
	wxStringTokenizer tkz(workStr, MAINAPP_PARAMETER_STR_DELIMITERS);
	while (tkz.HasMoreTokens()) {
		token = tkz.GetNextToken();
		if (token.length() < 1)
			continue;
		if (!countOnly)
			SetArgvString(indexStart + count,
					RestoreQuotedStr((wxChar *) token.c_str()), token.length());
		count++;
	}
	return (count);

}

/****************************** RemoveCommands ********************************/

/*
 * This routine removes the commands with a set prefix from the argv list by
 * setting the first character to zero.
 * It assumes that the argv list has a valid list of command pairs after the
 * offset index;
 */

void MainApp::RemoveCommands(int offset, const wxChar *prefix) {
	int i;

	for (i = offset; i < argc; i++)
		if (StrNCmpNoCase_Utility_String(argv[i], prefix) == 0)
			*argv[i] = *argv[i + 1] = '\0';

}

/****************************** FreeArgStrings ********************************/

/*
 * This routine frees the argument strings.
 */

void MainApp::FreeArgStrings(void) {
	int i;

	if (argv) {
		for (i = 0; i < argc; i++)
			if (argv[i])
				free(argv[i]);
		free(argv);
	}

}

/****************************** InitRun ***************************************/

/*
 * This function runs the simulation in the first initialisation run
 * during which the application interface is initialised.
 * With correct usage of the 'InitProcessVariables_AppInterface' routine
 * nothing after the program should be run.
 */

bool MainApp::InitRun(void) {
	static const wxChar *funcName = wxT("MainApp::InitRun");

	wxCriticalSectionLocker locker(mainCritSect);

	if (GetDSAMPtr_Common()->appInitialisedFlag) {
		NotifyWarning(wxT("%s: Attempted to re-initialise application."),
				funcName);
		return (TRUE);
	}
	(*ExternalMain)();
	SetCanFreePtrFlag_AppInterface(FALSE);
	return (CXX_BOOL(GetDSAMPtr_Common()->appInitialisedFlag));

}

/****************************** InitMain **************************************/

/*
 * This routine carries out the main initialisation of the application
 * interface.
 */

bool MainApp::InitMain(bool loadSimulationFlag) {
	static const wxChar *funcName = wxT("MainApp::InitMain");

	ResetStepCount_Utility_Datum();
	if (!GetPtr_AppInterface() || !GetPtr_AppInterface()->Init)
		return (true);
	GetPtr_AppInterface()->PrintExtMainAppUsage = PrintUsage_MainApp;
	ResetCommandArgFlags_AppInterface();
	GetPtr_AppInterface()->canLoadSimulationFlag = loadSimulationFlag;
	if (!InitProcessVariables_AppInterface(NULL, argc, argv)) {
		NotifyError(wxT("%s: Could not initialise process variables."),
				funcName);
		return (false);
	}
	GetPtr_AppInterface()->canLoadSimulationFlag = TRUE;
	if (runThreadedProc) {
		runThreadedProc->SetNumThreads(GetPtr_AppInterface()->numThreads);
		runThreadedProc->SetThreadMode(GetPtr_AppInterface()->threadMode);
	}
	return (true);

}

/****************************** InitXMLDocument *******************************/

/*
 * This initialises the
 */

void MainApp::InitXMLDocument(void) {
	doc = new DSAMXMLDocument;

}

/****************************** LoadXMLDocument *******************************/

/*
 * This routine checks that the simulation run has been initialised.
 */

bool MainApp::LoadXMLDocument(void) {
	static const wxChar *funcName = wxT("MainApp::LoadXMLDocument");
	bool ok = true;

	InitXMLDocument();
	if (!doc->Load(simFileName.GetFullPath())) {
		NotifyError(wxT("%s: Could not load XML file '%s' (Error: %s)."),
				funcName, simFileName.GetFullPath().c_str());
		ok = false;
	}
	if (ok && !doc->Translate()) {
		NotifyError(wxT("%s: Could not translate XML document."), funcName);
		ok = false;
	}
	if (ok) {
		GetPtr_AppInterface()->audModel = doc->GetSimProcess();
		SetSimulationFileFlag_AppInterface(FALSE);
	}
	delete doc;
	return (ok);

}

/****************************** CheckInitialisation ***************************/

/*
 * This routine checks that the simulation run has been initialised.
 */

bool MainApp::CheckInitialisation(void) {
	static const wxChar *funcName = wxT("MainApp::CheckInitialisation");

	if (!GetPtr_AppInterface()) {
		NotifyError(wxT("%s: Application interface not initialised."),
				funcName);
		return (false);
	}
#	if DSAM_DEBUG
	for (int i = 0; i < argc; i++)
	printf(wxT("%s: %2d: %s\n"), funcName, i, argv[i]);
#	endif /* DSAM_DEBUG */
	if (!InitProcessVariables_AppInterface(NULL, argc, argv)) {
		NotifyError(wxT("%s: Could not initialise process variables."),
				funcName);
		return (false);
	}
	return (true);

}

/****************************** StartSimThread ********************************/

/*
 * This routine starts the simulation thread.
 */

void MainApp::StartSimThread(wxThreadKind kind) {
	static const wxChar *funcName = wxT("MainApp::StartSimThread");

	mainCritSect.Enter();
	SwitchGUILocking_Common(TRUE);
	mainCritSect.Leave();
	simThread = new SimThread(kind);
	if (simThread->Create() != wxTHREAD_NO_ERROR)
		wxLogFatalError(wxT("%s: Can't create simulation thread!"), funcName);

	if (simThread->Run() != wxTHREAD_NO_ERROR)
		wxLogError(wxT("%s: Cannot start simulation thread."), funcName);

}

/****************************** DeleteSimThread *******************************/

/*
 * This routine deletes the simulation thread.
 * It first turns off the GUI output so that locking problems with exit
 * diagnostics do not occur.
 * It also waits until the simThread pointer is set to NULL.
 */

void MainApp::DeleteSimThread(void) {
	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
	if (simThread) {
		SetInterruptRequestStatus_Common(TRUE);
		simThread->SuspendDiagnostics();
		simThread->Delete();
		delete simThread;
	}

}

/****************************** ResetSimulation *******************************/

bool MainApp::ResetSimulation(void) {
	static const wxChar *funcName = wxT("MainApp::ResetSimulation");

	if (simThread) {
		wxLogWarning(wxT("%s: Running simulation not yet terminated!"),
				funcName);
		return (false);
	}
	SwitchGUILocking_Common(false);
	if (!CheckInitialisation())
		return (false);
	return (true);

}

/****************************** RunSimulation *********************************/

bool MainApp::RunSimulation(void) {
	static const wxChar *funcName = wxT("MainApp::RunSimulation");

	SetInterruptRequestStatus_Common(FALSE);
	if (!ExternalRunSimulation) {
		NotifyError(wxT("%s: External run simulation function not set."),
				funcName);
		return (false);
	}
	return (CXX_BOOL((* ExternalRunSimulation)()));

}

/****************************** GetSimProcess *********************************/

EarObjectPtr MainApp::GetSimProcess(void) {
	static const wxChar *funcName = wxT("MainApp::GetSimProcess");
	EarObjectPtr audModel;

	if ((audModel = GetSimProcess_AppInterface()) == NULL) {
		NotifyError(wxT("%s: simulation did not run successfully."), funcName);
		return (NULL);
	}
	return (audModel);

}

/******************************************************************************/
/****************************** SetSimulationFile *****************************/
/******************************************************************************/

/*
 * Sets the application interface file and name.
 */

void MainApp::SetSimulationFile(wxFileName &fileName) {
	simFileName = fileName;
	SetWorkingDirectory_AppInterface((wxChar *) fileName.GetPath().c_str());
	SetSimFileType_AppInterface(
			GetSimFileType_Utility_SimScript(
					(wxChar *) fileName.GetExt().c_str()));
	SetSimulationFile_AppInterface((WChar *) fileName.GetFullPath().c_str());

}

/******************************************************************************/
/****************************** CreateClient **********************************/
/******************************************************************************/

/*
 */

bool MainApp::CreateClient(wxChar * serverHost, uShort serverPort) {
	static const wxChar *funcName = wxT("MainApp::CreateClient");

	if (!myClient)
		myClient = new IPCClient(serverHost, serverPort);
	if (!myClient->Ok()) {
		if (diagsOn)
			NotifyError(wxT("%s: Failed to connect to %s:%u\n"), funcName,
					serverHost, serverPort);
		delete myClient;
		myClient = NULL;
		return (false);
	}
	if (diagsOn)
		NotifyWarning(wxT("%s: Connected to %s:%u.\n"), funcName, serverHost,
				serverPort);
	return (true);

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

/****************************** DSAMStart ************************************/

DSAMEXT_API int DSAMStart_MainApp(int(*MainSimulation)(void), int argc,
		wxChar **argv) {
	wxInitializer initializer;

	if (!initializer) {
		NotifyError(wxT("main: Failed to initialize the wxWidgets library, ")
		wxT("aborting."));
		return (false);
	}
	MainApp mainApp(argc, argv, MainSimulation);
	return (mainApp.Main());

}

#if DSAM_USE_UNICODE && !defined(WIN32)

static struct InitData {
	InitData() {
		nInitCount = 0;

#if wxUSE_UNICODE
		argc = 0;
		// argv = NULL; -- not even really needed
#endif // wxUSE_UNICODE
	}

	// critical section protecting this struct
	wxCRIT_SECT_DECLARE_MEMBER(csInit);

	// number of times wxInitialize() was called minus the number of times
	// wxUninitialize() was
	size_t nInitCount;

#if wxUSE_UNICODE
	int argc;

	// if we receive the command line arguments as ASCII and have to convert
	// them to Unicode ourselves (this is the case under Unix but not Windows,
	// for example), we remember the converted argv here because we'll have to
	// free it when doing cleanup to avoid memory leaks
	wchar_t **argv;
#endif // wxUSE_UNICODE
	DECLARE_NO_COPY_CLASS(InitData)
} gs_initData;

static void ConvertArgsToUnicode(int argc, char **argv) {
	gs_initData.argv = new wchar_t *[argc + 1];
	int wargc = 0;
	for (int i = 0; i < argc; i++) {
		wxWCharBuffer buf(wxConvLocal.cMB2WX(argv[i]));
		if (!buf) {
			wxLogWarning(
					_("Command line argument %d couldn't be converted to Unicode and will be ignored."),
					i);
		} else { // converted ok
			gs_initData.argv[wargc++] = wxStrdup(buf);
		}
	}

	gs_initData.argc = wargc;
	gs_initData.argv[wargc] = NULL;
}

static void FreeConvertedArgs() {
	if (gs_initData.argv) {
		for (int i = 0; i < gs_initData.argc; i++) {
			free(gs_initData.argv[i]);
		}

		delete[] gs_initData.argv;
		gs_initData.argv = NULL;
		gs_initData.argc = 0;
	}
}

DSAMEXT_API int DSAMStart_MainApp(int(*MainSimulation)(void), int argc,
		char **argv) {
	int myReturn;

	ConvertArgsToUnicode(argc, argv);
	myReturn = DSAMStart_MainApp(MainSimulation, gs_initData.argc,
			gs_initData.argv);
	FreeConvertedArgs();
	return (myReturn);

}
#endif // DSAM_USE_UNICODE && !defined(WIN32)
/****************************** ReadXMLSimFile ********************************/

/*
 * This function loads an XML file.  It is called from here because the XML
 * library code used is in C++.
 */

BOOLN ReadXMLSimFile_MainApp(wxChar *theFileName) {
	static const wxChar *funcName = wxT("ReadXMLSimFile_MainApp");

	wxFileName fileName((wxChar *) theFileName);
	dSAMMainApp->SetSimulationFile(fileName);
	if (!dSAMMainApp->LoadXMLDocument()) {
		NotifyError(wxT("%s: Could not load XML Document."), funcName);
		return (FALSE);
	}
	return (TRUE);

}

/****************************** Onexecute *************************************/

/*
 * This routine starts an execution thread.
 * It expects to be called by the operating routine's "OnExecute" routine.
 * So it cannot be a member function.
 */

BOOLN OnExecute_MainApp(void) {
	dSAMMainApp->StartSimThread();
	return (TRUE);

}

/****************************** PrintUsage ************************************/

/*
 * This routine prints the usage for the options used by myApp.
 * It expects to be called by the operating routine's "PrintUsage" routine.
 * So it cannot be a member function.
 */

void PrintUsage_MainApp(void) {
	DSAM_fprintf(
			stderr,
			wxT("\n")
			wxT("\t-S         : Run in server mode (add another -S with (x)inetd).\n")
			wxT("\t-I <x>     : Run using server ID <x>.\n"));

}

/*************************** DPrintSysLog *************************************/

/*
 * This routine prints out a diagnostic message to the system log.
 */

void DPrintSysLog_MainApp(const wxChar *format, va_list args) {
#	if DSAM_USE_UNICODE
	wxChar src[LONG_STRING];
	vswprintf(src, LONG_STRING, format, args);
	syslog(LOG_INFO, "%s", ConvUTF8_Utility_String(src));
#	else
	vsyslog(LOG_INFO, format, args);
#	endif

}

