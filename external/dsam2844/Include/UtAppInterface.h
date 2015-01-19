/**********************
 *
 * File:		UtAppInterface.h
 * Purpose:		This Module contains the interface routines for DSAM
 *				applications.
 * Comments:	The 'PostInitFunc' routine is run at the end of the
 *				'InitProcessVariables' routine.
 *				17 May 2006: LPO: ToDO: This code module needs to be moved into
 *				ExtMainApp.cpp.
 * Author:		L. P. O'Mard
 * Created:		15 Mar 2000
 * Updated:
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

#ifndef _UTAPPINTERFACE_H
#define	_UTAPPINTERFACE_H 1

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define DEFAULT_ERRORS_MODE			wxT("screen")
#define	APP_INT_NUM_PARS			6		/* Number of main parameters. */
#define	APP_MAX_AUTHORS				5
#define	APP_MAX_HELP_BOOKS			4
#define APP_PARLIST_NAME			wxT("app_specific")
#define APP_DEFAULT_NUM_THREADS		-1

#if defined(USE_GUI) || defined(LIBRARY_COMPILE) || defined(EXTENSIONS_SUPPORT)
#	define	MAIN_ARGS	void
#	define ARGC		0
#	define ARGV		NULL
#	if defined(INTERFACE_DLL)
		int MainSimulation(void)	{ return 0; } // Dummy function for DLL projects
#	endif
#else
#	define MainSimulation	main
#	define	MAIN_ARGS	int argc, WChar **argv
#	define ARGC		argc
#	define ARGV		argv
#endif /* GRAPHICS_SUPPORT */

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	APP_INT_DIAGNOSTICMODE,
	APP_INT_SIMULATIONFILE,
	APP_INT_SEGMENTMODE,
	APP_INT_THREADMODE,
	APP_INT_NUMTHREADS,
	APP_INT_PARLIST

} AppInteraceParSpecifier;

typedef enum {

	APP_INT_PARAMETERS_LIST_MODE,
	APP_INT_CFLIST_LIST_MODE,
	APP_INT_LIST_NULL

} AppInterfaceListSpecifier;

typedef enum {

	APP_INT_THREAD_MODE_PROCESS,
	APP_INT_THREAD_MODE_CHANNEL_CHAIN,
	APP_INT_THREAD_MODE_NULL

} AppInterfaceThreadModeSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	simulationFileFlag, useParComsFlag, checkMainInit, listParsAndExit;
	BOOLN	listCFListAndExit, readAppParFileFlag, printUsageFlag;
	BOOLN	appParFileFlag, canLoadSimulationFlag, simulationFinishedFlag;
	BOOLN	numThreadsFlag, threadModeFlag;
	BOOLN	updateProcessVariablesFlag;
	WChar	appName[MAX_FILE_PATH];
	WChar	appHelpBooks[APP_MAX_HELP_BOOKS][MAXLINE];
	WChar	appParFile[MAX_FILE_PATH];
	WChar	appVersion[MAXLINE];
	WChar	compiledDSAMVersion[MAXLINE];
	WChar	title[MAX_FILE_PATH];
	WChar	authors[APP_MAX_AUTHORS][MAXLINE];
	WChar	simulationFile[MAX_FILE_PATH];
	WChar	segmentMode[SMALL_STRING];
	WChar	diagMode[MAX_FILE_PATH];
	WChar	installDir[MAX_FILE_PATH];
	WChar	workingDirectory[MAX_FILE_PATH];
	WChar	**argv;
	int		argc;
	int		initialCommand;
	int		segmentModeSpecifier;
	int		diagModeSpecifier;
	int		maxUserModules;
	int		numHelpBooks;
	int		numThreads;
	int		threadMode;
	EarObjectPtr	audModel;

	/* Private members */
	BOOLN	canFreePtrFlag;
	NameSpecifier	*listingModeList;
	NameSpecifier	*diagModeList;
	NameSpecifier	*threadModeList;
	UniParListPtr	appParList;
	UniParListPtr	parList;
	int		simFileType;
	BOOLN	(* FreeAppProcessVars)(void);
	BOOLN	(* Init)(void);
	BOOLN	(* OnExecute)(void);
	void	(* OnExit)(void);
	void	(* PrintExtMainAppUsage)(void);
	void	(* PrintUsage)(void);
	int		(* ProcessOptions)(int , WChar **, int *);
	BOOLN	(* RegisterUserModules)(void);
	BOOLN	(* PostInitFunc)(void);
	BOOLN	(* SetUniParList)(UniParListPtr *);

} AppInterface, *AppInterfacePtr;

typedef struct stat *StatPtr;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern WChar **argv;
extern int	argc;

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

extern AppInterfacePtr	appInterfacePtr;

__END_DECLS

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DSAM_API BOOLN	AddAppHelpBook_AppInterface(const WChar *bookName);

DSAM_API BOOLN	Free_AppInterface(void);

DSAM_API void	FreeSim_AppInterface(void);

DSAM_API EarObjectPtr	GetDataFileInProcess_AppInterface(void);

WChar *	GetFilePath_AppInterface(WChar *filePath);

DSAM_API AppInterfacePtr	GetPtr_AppInterface(void);

DSAM_API DatumPtr *	GetSimPtr_AppInterface(void);

DSAM_API EarObjectPtr	GetSimProcess_AppInterface(void);

DSAM_API DatumPtr	GetSimulation_AppInterface(void);

UniParPtr	GetUniParPtr_AppInterface(WChar *parName);

DSAM_API BOOLN	Init_AppInterface(ParameterSpecifier parSpec);

DSAM_API BOOLN	InitProcessVariables_AppInterface(BOOLN (* Init)(void), int theArgc,
		  WChar **theArgv);

BOOLN	InitSimulation_AppInterface(void);

BOOLN	InitThreadModeList_AppInterface(void);

void	ListCFListAndExit_AppInterface(void);

DSAM_API BOOLN	ListParameters_AppInterface(void);

void	ListParsAndExit_AppInterface(void);

DSAM_API BOOLN	OnExecute_AppInterface(void);

DSAM_API void	OnExit_AppInterface(void);

void	ParseParSpecifiers_AppInterface(WChar *parName, WChar *appName,
		  WChar *subProcess);

void	PrintPars_AppInterface(void);

BOOLN	ProcessParComs_AppInterface(void);

BOOLN	ProcessOptions_AppInterface(void);

DSAM_API BOOLN	PrintSimPars_AppInterface(void);

void	PrintUsage_AppInterface(void);

BOOLN	ReadPars_AppInterface(WChar *parFileName);

BOOLN	ReadProgParFile_AppInterface(void);

DSAM_API void	ResetCommandArgFlags_AppInterface(void);

DSAM_API BOOLN	ResetSim_AppInterface(void);

DSAM_API BOOLN	RunSim_AppInterface(void);

BOOLN	SetAppFreeProcessVars_AppInterface(BOOLN (* FreeAppProcessVars)(void));

BOOLN	SetAppParFile_AppInterface(WChar *fileName);

DSAM_API BOOLN	SetAppName_AppInterface(const WChar *appName);

DSAM_API BOOLN	SetAppPrintUsage_AppInterface(void (* PrintUsage)(void));

DSAM_API BOOLN	SetAppProcessOptions_AppInterface(int (* ProcessOptions)(int, WChar **,
				  int *));

DSAM_API BOOLN	SetAppRegisterUserModules_AppInterface(
				  BOOLN	(* RegisterUserModules)(void));

DSAM_API BOOLN	SetAppPostInitFunc_AppInterface(BOOLN (* PostInitFunc)(void));

DSAM_API BOOLN	SetAppSetUniParList_AppInterface(BOOLN (* SetUniParList)(UniParListPtr
				  *parList));

DSAM_API BOOLN	SetAppVersion_AppInterface(const WChar *appVersion);

DSAM_API void	SetArgcAndArgV_AppInterface(int theArgc, WChar **theArgv);

DSAM_API BOOLN	SetCanFreePtrFlag_AppInterface(BOOLN status);

DSAM_API BOOLN	SetCompiledDSAMVersion_AppInterface(const WChar *compiledDSAMVersion);

DSAM_API BOOLN	SetDiagMode_AppInterface(WChar *theDiagMode);

DSAM_API BOOLN	SetInstallDir_AppInterface(const WChar *theInstallDir);

BOOLN	SetMaxUserModules_AppInterface(int maxUserModules);

BOOLN	SetNumThreads_AppInterface(int theNumThreads);

DSAM_API BOOLN	SetOnExecute_AppInterface(BOOLN (* OnExecute)(void));

DSAM_API BOOLN	SetOnExit_AppInterface(void (* OnExit)(void));

DSAM_API BOOLN	SetWorkingDirectory_AppInterface(WChar * workingDirectory);

DSAM_API BOOLN	SetProgramParValue_AppInterface(const WChar *parName,
				  WChar *parValue, BOOLN readSPF);

BOOLN	SetRealArraySimPar_AppInterface(WChar *parName, int index, Float value);

BOOLN	SetRealSimPar_AppInterface(WChar *parName, Float value);

BOOLN	SetSegmentMode_AppInterface(WChar *theSegmentMode);

DSAM_API BOOLN	SetSimFileType_AppInterface(int simFileType);

BOOLN	SetSimPar_AppInterface(WChar *parName, WChar *value);

DSAM_API BOOLN	SetSimulationFile_AppInterface(WChar *theSimulationFile);

DSAM_API void	SetSimulationFileFlag_AppInterface(BOOLN theSimulationFileFlag);

BOOLN	SetThreadMode_AppInterface(WChar * theThreadMode);

BOOLN	SetTitle_AppInterface(const WChar *title);

BOOLN	SetUniParList_AppInterface(void);

__END_DECLS

#endif
