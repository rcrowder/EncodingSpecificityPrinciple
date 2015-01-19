/**********************
 *
 * File:		ExtMainApp.h
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
 
#ifndef _EXTMAINAPP
#define _EXTMAINAPP 1

/* sccsid[] = "%W% %G%" */

#include <wx/filename.h>
#include "UtSSSymbols.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define MAINAPP_PARAMETER_STR_DELIMITERS	wxT(" ,\t")
#define MAINAPP_QUOTE						'"'
#define MAINAPP_SPACE_SUBST					'\1'
#define	MAINAPP_NUM_BASE_ARGUMENTS			3

#if !defined(WIN32)
#	define MAINAPP_MAIN_SIM_INIT MainSimulation
#else
#	define MAINAPP_MAIN_SIM_INIT NULL
#endif

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/
/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

class SimThread;
class RunThreadedProc;
class DSAMXMLDocument;
class IPCClient;

/*************************** MainApp ******************************************/

class DSAMEXT_API MainApp {

	bool	initOk, serverFlag, superServerFlag, diagsOn;
	bool	threadedSimExecutionFlag;
	wxChar	**argv;
	int		argc;
	int		(* ExternalMain)(void);
	int		(* ExternalRunSimulation)(void);
	SymbolPtr	symList;
	IPCClient	*myClient;
	wxFileName	simFileName;

  public:
	wxChar	serverHost[MAXLINE];
	int		serverPort;
	SimThread	*simThread;
	DSAMXMLDocument	*doc;
	RunThreadedProc	*runThreadedProc;
	wxCriticalSection	mainCritSect;

  	MainApp(int theArgc = 0, wxChar **theArgv = NULL, int (* TheExternalMain)(
	  void) = NULL, int (* TheExternalRunSimulation)(void) = NULL);
  	virtual ~MainApp(void);

	virtual void	InitXMLDocument(void);
	virtual EarObjectPtr	GetSimProcess(void);
	virtual bool	InitRun(void);
	virtual int		Main(void);
	virtual	bool	RunSimulation(void);
	virtual bool	ResetSimulation(void);
	virtual void	SetRunIndicators(bool on)	{ on = true; }

	// has the initialization been successful? (implicit test)
	operator bool() const { return initOk; }

	bool	CheckInitialisation(void);
	void	CheckOptions(void);
	bool	CreateClient(wxChar * serverHost, uShort serverPort);
	void	DeleteSimThread(void);
	void	FreeArgStrings(void);
	int		GetArgc(void)	{ return argc; }
	wxChar **	GetArgv(void)	{ return argv; }
	IPCClient *	GetClient(void)	{ return myClient; }
	int		GetServerFlag(void)	{ return(serverFlag); }
	int		GetServerPort(void)	{ return(serverPort); }
	wxFileName &	GetSimFileName(void)	{ return simFileName; }
	SymbolPtr GetSymList(void)	{ return symList; }
	bool	InitArgv(int theArgc);
	bool	InitCommandLineArgs(int theArgc, wxChar **theArgv);
	bool	InitMain(bool loadSimulationFlag = false);
	bool	LoadXMLDocument(void);
	bool	ProtectQuotedStr(wxChar *str);
	void	RemoveCommands(int offset, const wxChar *prefix);
	wxChar *	RestoreQuotedStr(wxChar *str);
	int		RunIPCMode(void);
	int		RunServer(void);
	int		RunClient(void);
	void	SetArgc(int theArgc)	{ argc = theArgc; }
	void	SetArgv(wxChar **theArgv)	{ argv = theArgv; }
	bool	SetArgvString(int index, const wxChar *string, size_t size);
	void	SetInitStatus(bool status)	{ initOk = status; }
	int		SetParameterOptionArgs(int indexStart,
			  const wxChar *parameterOptions, bool countOnly);
	void	SetSimulationFile(wxFileName &fileName);
	void	SetThreadPars(void);
	void	StartSimThread(wxThreadKind kind = wxTHREAD_DETACHED);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern int	MainSimulation(void);
DSAMEXT_API extern MainApp	*dSAMMainApp;
DSAMEXT_API extern int		(* MainSimulation_MainApp)(void);

/******************************************************************************/
/*************************** Early subroutine declarations ********************/
/******************************************************************************/

DSAMEXT_API int		DSAMStart_MainApp(int (* MainSimulation)(void), int argc,
					  char **argv);
DSAMEXT_API int		DSAMStart_MainApp(int (* MainSimulation)(void), int argc,
					  wxChar **argv);

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

#if defined(EXTENSIONS_SUPPORT) && !defined(USE_GUI)
#	if !defined(LIBRARY_COMPILE)
#		if defined(WIN32)
			int main(int argc, wxChar **argv) { return(
			  DSAMStart_MainApp(MainSimulation, argc, argv)); }
#		else
			int main(int argc, char **argv) { return(
			  DSAMStart_MainApp(MainSimulation, argc, argv)); }
#		endif
#	endif

#endif /* defined(EXTENSIONS_SUPPORT) && !defined(GRAPHICS_SUPPORT) */

void	DPrintSysLog_MainApp(const wxChar *format, va_list args);

void	PrintUsage_MainApp(void);

BOOLN	OnExecute_MainApp(void);

BOOLN	ReadXMLSimFile_MainApp(wxChar *fileName);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

