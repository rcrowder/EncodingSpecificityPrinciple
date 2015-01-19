/**********************
 *
 * File:		UtAppInterface.c
 * Purpose:		This Module contains the interface routines for DSAM
 *				application's.
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <locale.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeNSpecLists.h"
#include "GeModuleMgr.h"
#include "GeModuleReg.h"

#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtOptions.h"
#include "UtString.h"

#include "FiParFile.h"
#include "FiDataFile.h"

#include "UtAppInterface.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AppInterfacePtr	appInterfacePtr = NULL;

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 * The "FreeAll_EarObject" call is used here when the main
 * This routine will return without doing anything if the 'canFreePtrFlag' is
 * not set.
 * This is to allow it to be placed in the 'MainSimulation' routine of programs
 * but it will not do anything when the GUI is calling 'MainSimulation' routine.
 * This is because the GUI interface calls it when the GUI exits.
 * This routine also calls the routine to free the nullModule global variable.
 */

DSAM_API BOOLN
Free_AppInterface(void)
{
	if (!appInterfacePtr)
		return(FALSE);
	if (!appInterfacePtr->canFreePtrFlag)
		return(TRUE);
	GetDSAMPtr_Common()->appInitialisedFlag = FALSE;
	FreeUserModuleList_ModuleReg();
	if (appInterfacePtr->diagModeList)
		free(appInterfacePtr->diagModeList);
	if (appInterfacePtr->parList)
		FreeList_UniParMgr(&appInterfacePtr->parList);
	if (appInterfacePtr->appParList)
		FreeList_UniParMgr(&appInterfacePtr->appParList);
	if (appInterfacePtr->audModel)
		Free_EarObject(&appInterfacePtr->audModel);
	if (appInterfacePtr->FreeAppProcessVars)
		(* appInterfacePtr->FreeAppProcessVars)();
	if (appInterfacePtr->parSpec == GLOBAL) {
		free(appInterfacePtr);
		appInterfacePtr = NULL;
	}
	FreeSoundFormatLists_DataFile();
	CloseFiles();
	return(TRUE);

}

/****************************** InitListingModeList ***************************/

/*
 * This function initialises the 'listingMode' list array
 */

BOOLN
InitListingModeList_AppInterface(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("PARAMETERS"),	APP_INT_PARAMETERS_LIST_MODE },
			{ wxT("CFINFO"),		APP_INT_CFLIST_LIST_MODE },
			{ NULL,					APP_INT_LIST_NULL }
		};
	appInterfacePtr->listingModeList = modeList;
	return(TRUE);

}

/****************************** InitThreadModeList ****************************/

/*
 * This function initialises the 'ThreadMode' list array
 */

BOOLN
InitThreadModeList_AppInterface(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("PROCESS"),			APP_INT_THREAD_MODE_PROCESS },
			{ wxT("CHANNEL_CHAIN"),		APP_INT_THREAD_MODE_CHANNEL_CHAIN },
			{ NULL,						APP_INT_THREAD_MODE_NULL }
		};
	appInterfacePtr->threadModeList = modeList;
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This function initialises the module by setting module's parameter
 * pointer structure.
 * The GLOBAL option is for hard programming - it sets the module's
 * pointer to the global parameter structure and initialises the
 * parameters. The LOCAL option is for generic programming - it
 * initialises the parameter structure currently pointed to by the
 * module's parameter pointer.
 */

BOOLN
Init_AppInterface(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_AppInterface");
	int		i;

	if (parSpec == GLOBAL) {
		if (appInterfacePtr != NULL)
			Free_AppInterface();
		if ((appInterfacePtr = (AppInterfacePtr) malloc(sizeof(
		  AppInterface))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (!appInterfacePtr) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	appInterfacePtr->parSpec = parSpec;
	appInterfacePtr->updateProcessVariablesFlag = TRUE;
	appInterfacePtr->simulationFileFlag = FALSE;
	appInterfacePtr->useParComsFlag = TRUE;
	appInterfacePtr->checkMainInit = TRUE;
	appInterfacePtr->canLoadSimulationFlag = TRUE;
	appInterfacePtr->numThreadsFlag = TRUE;
	appInterfacePtr->threadModeFlag = TRUE;
	appInterfacePtr->listParsAndExit = FALSE;
	appInterfacePtr->listCFListAndExit = FALSE;
	appInterfacePtr->appParFileFlag = FALSE;
	appInterfacePtr->readAppParFileFlag = FALSE;
	appInterfacePtr->printUsageFlag = FALSE;
	appInterfacePtr->simulationFinishedFlag = TRUE;
	DSAM_strcpy(appInterfacePtr->appName, wxT("No Name"));
	DSAM_strcpy(appInterfacePtr->appParFile, NO_FILE);
	DSAM_strcpy(appInterfacePtr->appVersion, wxT("?.?.?"));
	DSAM_strcpy(appInterfacePtr->compiledDSAMVersion, wxT("?.?.?"));
	DSAM_strcpy(appInterfacePtr->title, wxT("No title"));
	for (i = 0; i < APP_MAX_AUTHORS; i++)
		appInterfacePtr->authors[i][0] = '\0';
	DSAM_strcpy(appInterfacePtr->simulationFile, NO_FILE);
	DSAM_strcpy(appInterfacePtr->diagMode, DEFAULT_FILE_NAME);
	DSAM_strcpy(appInterfacePtr->installDir, wxT("."));
	appInterfacePtr->argv = NULL;
	appInterfacePtr->argc = 0;
	appInterfacePtr->initialCommand = 0;
	appInterfacePtr->numThreads = APP_DEFAULT_NUM_THREADS;
	appInterfacePtr->threadMode = APP_INT_THREAD_MODE_PROCESS;
	appInterfacePtr->segmentModeSpecifier = GENERAL_BOOLEAN_OFF;
	appInterfacePtr->diagModeSpecifier = GENERAL_DIAGNOSTIC_OFF_MODE;
	appInterfacePtr->maxUserModules = -1;
	appInterfacePtr->numHelpBooks = 0;
	appInterfacePtr->audModel = NULL;

	InitListingModeList_AppInterface();
	InitThreadModeList_AppInterface();
	if ((appInterfacePtr->diagModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), appInterfacePtr->diagMode)) == NULL)
		return(FALSE);
	if (!SetUniParList_AppInterface()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_AppInterface();
		return(FALSE);
	}
	appInterfacePtr->appParList = NULL;
	appInterfacePtr->simFileType = UTILITY_SIMSCRIPT_UNKNOWN_FILE;
	appInterfacePtr->canFreePtrFlag = TRUE;
	appInterfacePtr->OnExit = NULL;
	appInterfacePtr->FreeAppProcessVars = NULL;
	appInterfacePtr->Init = NULL;
	appInterfacePtr->OnExecute = NULL;
	appInterfacePtr->PrintExtMainAppUsage = NULL;
	appInterfacePtr->PrintUsage = NULL;
	appInterfacePtr->ProcessOptions = NULL;
	appInterfacePtr->RegisterUserModules = NULL;
	appInterfacePtr->PostInitFunc = NULL;
	appInterfacePtr->SetUniParList = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the programs universal parameter
 * list. This list provides universal access to the program's
 * parameters.  It needs to be called from an initialise routine.
 */

BOOLN
SetUniParList_AppInterface(void)
{
	static const WChar *funcName = wxT("SetUniParList_AppInterface");
	UniParPtr	pars;

	if ((appInterfacePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  APP_INT_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = appInterfacePtr->parList->pars;
	SetPar_UniParMgr(&pars[APP_INT_DIAGNOSTICMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostics mode specifier ('off', 'screen' or <filename>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &appInterfacePtr->diagModeSpecifier, appInterfacePtr->diagModeList,
	  (void * (*)) SetDiagMode_AppInterface);
	SetPar_UniParMgr(&pars[APP_INT_SIMULATIONFILE], wxT("SIM_FILE"),
	  wxT("Simulation file."),
	  UNIPAR_FILE_NAME,
	  &appInterfacePtr->simulationFile, (WChar *) wxT("Sim. Par File (*.spf)|")
	    wxT("*.spf|Sim. script (*.sim)|*.sim|All files (*.*)|*.*"),
	  (void * (*)) SetSimulationFile_AppInterface);
	SetPar_UniParMgr(&pars[APP_INT_SEGMENTMODE], wxT("SEGMENT_MODE"),
	  wxT("Segmented or frame-base processing mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &appInterfacePtr->segmentModeSpecifier, NULL,
	  (void * (*)) SetSegmentMode_AppInterface);
	SetPar_UniParMgr(&pars[APP_INT_THREADMODE], wxT("THREAD_MODE"),
	  wxT("Thread mode ('process' or 'equi_channel')."),
	  UNIPAR_NAME_SPEC,
	  &appInterfacePtr->threadMode, appInterfacePtr->threadModeList,
	  (void * (*)) SetThreadMode_AppInterface);
	SetPar_UniParMgr(&pars[APP_INT_NUMTHREADS], wxT("NUM_THREADS"),
	  wxT("No. of processing threads for simulation (-ve defaults to no. ")
	  wxT("CPU's)."),
	  UNIPAR_INT,
	  &appInterfacePtr->numThreads, NULL,
	  (void * (*)) SetNumThreads_AppInterface);
	SetPar_UniParMgr(&pars[APP_INT_PARLIST], wxT("PAR_LIST"),
	  wxT("App. Specific Pars."),
	  UNIPAR_PARLIST,
	  &appInterfacePtr->appParList, NULL,
	  (void * (*)) SetUniParList_AppInterface);
	return(TRUE);

}

/****************************** SetDiagMode ***********************************/

/*
 * This functions sets the diagnostic mode.
 * It returns false if it fails in any way.
 */

DSAM_API BOOLN
SetDiagMode_AppInterface(WChar *theDiagMode)
{
	static const WChar	*funcName = wxT("SetDiagMode_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->diagModeSpecifier = IdentifyDiag_NSpecLists(theDiagMode,
	  appInterfacePtr->diagModeList);
	SetParsFile_Common(appInterfacePtr->diagModeList[
	  appInterfacePtr->diagModeSpecifier].name, OVERWRITE);
	return(TRUE);

}

/****************************** SetSimulationFileFlag *************************/

/*
 * This routine sets the simulationFileFlag which signals whether a simulation
 * file needs to be loaded.
 */

DSAM_API void
SetSimulationFileFlag_AppInterface(BOOLN theSimulationFileFlag)
{
	appInterfacePtr->simulationFileFlag = theSimulationFileFlag;

}

/****************************** SetSimulationFile *****************************/

/*
 * This functions sets the simulation file name.
 * It returns false if it fails in any way.
 */

DSAM_API BOOLN
SetSimulationFile_AppInterface(WChar *theSimulationFile)
{
	static const WChar	*funcName = wxT("SetSimulationFile_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (*theSimulationFile == '\0') {
		NotifyError(wxT("%s: Illegal zero length name."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(appInterfacePtr->simulationFile, theSimulationFile,
	  MAX_FILE_PATH);
	appInterfacePtr->simulationFileFlag = TRUE;
	appInterfacePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetSegmentMode ********************************/

/*
 * This functions sets the segment processing mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetSegmentMode_AppInterface(WChar *theSegmentMode)
{
	static const WChar	*funcName = wxT("SetSegmentMode_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if ((appInterfacePtr->segmentModeSpecifier = Identify_NameSpecifier(
	  theSegmentMode, BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal segment processing mode (%s): must be ")
		  wxT("'on' or 'off'."), funcName, theSegmentMode);
		return(FALSE);
	}
	DSAM_strcpy(appInterfacePtr->segmentMode, theSegmentMode);
	SetSegmentedMode(appInterfacePtr->segmentModeSpecifier);
	return(TRUE);

}

/****************************** SetThreadMode **************************/

/*
 * This function sets the module's threadMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetThreadMode_AppInterface(WChar * theThreadMode)
{
	static const WChar	*funcName = wxT("SetThreadMode_AppInterface");
	int		specifier;

	if (appInterfacePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theThreadMode,
		appInterfacePtr->threadModeList)) == APP_INT_THREAD_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theThreadMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	appInterfacePtr->threadModeFlag = TRUE;
	appInterfacePtr->threadMode = specifier;
	return(TRUE);

}

/****************************** SetNumThreads *********************************/

/*
 * This functions sets the application's number of processing threads to use for
 * the simulation.
 * It returns false if it fails in any way.
 */

BOOLN
SetNumThreads_AppInterface(int numThreads)
{
	static const WChar	*funcName = wxT("SetNumThreads_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->numThreads = numThreads;
	return(TRUE);

}

/****************************** SetMaxUserModules *****************************/

/*
 * This functions sets the application's final pars routine.
 * These are parameters which set by the application interace, after the
 * simulation is initialised.
 * It returns false if it fails in any way.
 */

BOOLN
SetMaxUserModules_AppInterface(int maxUserModules)
{
	static const WChar	*funcName = wxT("SetMaxUserModules_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->maxUserModules = maxUserModules;
	return(TRUE);

}

/****************************** SetInstallDir *********************************/

/*
 * This functions sets the installation directory.
 * It returns false if it fails in any way.
 */

DSAM_API BOOLN
SetInstallDir_AppInterface(const WChar *theInstallDir)
{
	static const WChar	*funcName = wxT("SetInstallDir_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	DSAM_strncpy(appInterfacePtr->installDir, theInstallDir, MAX_FILE_PATH);
	return(TRUE);

}

/****************************** SetAppParFile *********************************/

/*
 * This functions sets the application's parameter file name'.
 * It returns false if it fails in any way.
 */

BOOLN
SetAppParFile_AppInterface(WChar *fileName)
{
	static const WChar	*funcName = wxT("SetAppParFile_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if ((fileName == NULL) || (*fileName == '\0')) {
		NotifyError(wxT("%s: illegal file name."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(appInterfacePtr->appParFile, fileName, MAX_FILE_PATH);
	appInterfacePtr->appParFileFlag = TRUE;
	return(TRUE);

}

/****************************** SetAppSetUniParList ***************************/

/*
 * This functions sets the application's SetUniParList routine which is used
 * to set the applications parameters.
 */

DSAM_API BOOLN
SetAppSetUniParList_AppInterface(BOOLN (* SetUniParList)(UniParListPtr
  *parList))
{
	static const WChar	*funcName = wxT("SetAppSetUniParList_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->SetUniParList = SetUniParList;
	return(TRUE);

}

/****************************** SetAppFreeAppProcessVars **********************/

/*
 * This functions sets the application's FreeAppProcessVars routine which is
 * used free allocated memory for the applications process.
 */

BOOLN
SetAppFreeProcessVars_AppInterface(BOOLN (* FreeAppProcessVars)(void))
{
	static const WChar	*funcName = wxT("SetAppFreeProcessVars_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->FreeAppProcessVars = FreeAppProcessVars;
	return(TRUE);

}

/****************************** SetAppPrintUsage ******************************/

/*
 * This functions sets the application's print usage routine.
 * It returns false if it fails in any way.
 */

DSAM_API BOOLN
SetAppPrintUsage_AppInterface(void (* PrintUsage)(void))
{
	static const WChar	*funcName = wxT("SetAppPrintUsage_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->PrintUsage = PrintUsage;
	return(TRUE);

}

/****************************** SetAppProcessOptions **************************/

/*
 * This functions sets the application's process options routine.
 * It returns false if it fails in any way.
 */

DSAM_API BOOLN
SetAppProcessOptions_AppInterface(int (* ProcessOptions)(int, WChar **, int *))
{
	static const WChar	*funcName = wxT("SetAppProcessOptions_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->ProcessOptions = ProcessOptions;
	return(TRUE);

}

/****************************** SetAppPostInitFunc ****************************/

/*
 * This functions sets the application's final pars routine.
 * These are parameters which set by the application interace, after the
 * simulation is initialised.
 * It returns false if it fails in any way.
 */

DSAM_API BOOLN
SetAppPostInitFunc_AppInterface(BOOLN (* PostInitFunc)(void))
{
	static const WChar	*funcName = wxT("SetAppPostInitFunc_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->PostInitFunc = PostInitFunc;
	return(TRUE);

}

/****************************** SetAppRegisterUserModules *********************/

/*
 * This functions sets the application's RegisterUserModules routine.
 * This routine registers the users modules so that they can be used by the
 * simulation script interface.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
SetAppRegisterUserModules_AppInterface(BOOLN (* RegisterUserModules)(void))
{
	static const WChar	*funcName = wxT(
	  "SetAppRegisterUserModules_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->RegisterUserModules = RegisterUserModules;
	return(TRUE);

}

/****************************** SetOnExecute **********************************/

/*
 * This functions sets the application's OnExecute routine.
 * This routine is used by the application to hold it's execution routine.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
SetOnExecute_AppInterface(BOOLN (* OnExecute)(void))
{
	static const WChar	*funcName = wxT("SetOnExecute_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->OnExecute = OnExecute;
	return(TRUE);

}

/****************************** SetOnExit *************************************/

/*
 * This functions sets the application's OnExit routine.
 * This routine is used by the application to hold it's exit routine.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
SetOnExit_AppInterface(void (* OnExit)(void))
{
	static const WChar	*funcName = wxT("SetOnExit_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->OnExit = OnExit;
	return(TRUE);

}

/****************************** PrintInitialDiagnostics ***********************/

/*
 * This routine prints the initial diagnostics for the program.
 */

void
PrintPars_AppInterface(void)
{
	static const WChar *funcName = wxT("PrintPars_AppInterface");
	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return;
	}
	DPrint(wxT("Simulation script application interface settings:\n"));
	DPrint(wxT("Diagnostics specifier is set to '%s'.\n"),
	  appInterfacePtr->diagModeList[appInterfacePtr->diagModeSpecifier].name);
	DPrint(wxT("This simulation is run from the file %s.\n"),
	  GetFilePath_AppInterface(appInterfacePtr->simulationFile));
	DPrint(wxT("Thread mode '%' used with '%d' threads.\n"), appInterfacePtr->
	  threadModeList[appInterfacePtr->threadMode].name,
	  appInterfacePtr->numThreads);
	DPrint(wxT("\n"));

}

/****************************** PrintUsage ************************************/

/* * This routine prints the usage diagnostics.
 */

void
PrintUsage_AppInterface(void)
{
	static const WChar *funcName = wxT("PrintUsage_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return;
	}
	fprintf_Utility_String(stderr, wxT("\n")
	   wxT("Usage: %s [options] [parameter1 value1 parameter2 value2 ...]\n")
	   wxT("\t-d <state>    \t: Diagnostics mode ('off', 'screen' or filename).\n")
	   wxT("\t-h            \t: Produce this help message.\n")
#	  if USE_EXTENSIONS_LIBRARY
	   wxT("\t-I <x>        \t: User server ID 'x' when running in server mode.\n")
#	  endif
	   wxT("\t-l <list>     \t: List options: 'parameters', 'cfinfo'.\n")
	   wxT("\t-P <file name>\t: Use this main parameter file\n")
	   wxT("\t-s <file name>\t: Use this simulation file (*.spf or *.sim)\n")
#	  if USE_EXTENSIONS_LIBRARY
	   wxT("\t-S            \t: Run AMS in server mode.\n")
#	  endif
	  ,
	  appInterfacePtr->appName);

}

/****************************** ProcessParComs ********************************/

/*
 * This routine processes parameter-value command pairs, passed as command-line
 * arguments.
 * These parameters are only used once, then the 'useParComsFlag' is set to
 * FALSE. If a new set commands is given (as in the server/client mode) then
 * this flag needs to be set to TRUE.
 */

BOOLN
ProcessParComs_AppInterface(void)
{
	static const WChar *funcName = wxT("ProcessParComs_AppInterface");
	int		i;
	DatumPtr	simulation = NULL;
	AppInterfacePtr	p = appInterfacePtr;

	if (!p) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (!p->useParComsFlag)
		return(TRUE);

	if (p->audModel && (simulation = GetSimulation_ModuleMgr(p->audModel)) ==
	  NULL) {
		NotifyError(wxT("%s: No simulation has been initialised."), funcName);
		return(FALSE);
	}
	if (p->argc && ((p->argc - p->initialCommand) % 2 != 0)) {
		NotifyError(wxT("%s: parameter values must be in <name> <value> ")
		  wxT("pairs."), funcName);
		return(FALSE);
	}
	for (i = p->initialCommand; i < p->argc; i += 2) {
		if (SetProgramParValue_AppInterface(p->argv[i], p->argv[i + 1],
		  FALSE) || (simulation && SetPar_ModuleMgr(p->audModel, p->argv[i],
		    p->argv[i + 1])) || !simulation)
			continue;
		NotifyError(wxT("%s: Could not set '%s' parameter to '%s'."), funcName,
		  p->argv[i], p->argv[i + 1]);
		return(FALSE);
	}
	p->useParComsFlag = (!p->audModel);
	return(TRUE);

}

/****************************** ProcessOptions ********************************/

/*
 * This routine processes any command-line options.
 * The '@' options is to be ignored.  It is used by other modules, i.e. myApp.
 * This routine should only change flags and/or.  This allows it to be run
 * more than once.
 */

BOOLN
ProcessOptions_AppInterface(void)
{
	static const WChar *funcName = wxT("ProcessOptions_AppInterface");
	BOOLN	optionFound = FALSE, ok = TRUE;
	int		optInd = 1, optSub = 0;
	WChar	c, *argument;

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	while ((c = Process_Options(appInterfacePtr->argc, appInterfacePtr->argv,
	  &optInd, &optSub, &argument, wxT("@#:d:hl:P:R:s:Sp:")))) {
		optionFound = TRUE;
		switch (c) {
		case '@':
		case '#':
			break;
		case 'd':
			ok = SetProgramParValue_AppInterface(appInterfacePtr->parList->pars[
			  APP_INT_DIAGNOSTICMODE].abbr, argument, FALSE);
			break;
		case 'h':
			appInterfacePtr->printUsageFlag = TRUE;
			break;
		case 'l':
			switch (Identify_NameSpecifier(argument,
			  appInterfacePtr->listingModeList)) {
			case APP_INT_PARAMETERS_LIST_MODE:
				appInterfacePtr->listParsAndExit = TRUE;
				break;
			case APP_INT_CFLIST_LIST_MODE:
				appInterfacePtr->listCFListAndExit = TRUE;
				break;
			default:
				DSAM_fprintf(stderr, wxT("Unknown list mode (%s).\n"),
				  argument);
				appInterfacePtr->printUsageFlag = TRUE;
			}
			break;
		case 'P':
			DSAM_strncpy(appInterfacePtr->appParFile, argument, MAX_FILE_PATH);
			appInterfacePtr->readAppParFileFlag = TRUE;
			break;
		case 's':
			MarkIgnore_Options(appInterfacePtr->argc, appInterfacePtr->argv,
			  wxT("-s"), OPTIONS_WITH_ARG);
			if (!SetSimulationFile_AppInterface(argument))
				ok = FALSE;
			break;
		default:
			if (appInterfacePtr->ProcessOptions && (* appInterfacePtr->
			  ProcessOptions)(appInterfacePtr->argc, appInterfacePtr->argv,
			  &optInd)) {
				optSub = 0;
				break;
			}
			appInterfacePtr->printUsageFlag = TRUE;
		} /* switch */
	}
	appInterfacePtr->initialCommand = (optionFound)? optInd: 1;
	return(ok);

}

/****************************** GetFilePath ***********************************/

/*
 * This routine returns the given filePath, depending upon whether GUI mode is
 * used.
 * This was introduced to conform to the use of streams in the wxWindows
 * document/view code.
 */

WChar *
GetFilePath_AppInterface(WChar *filePath)
{
	static WChar guiFilePath[MAX_FILE_PATH];

	if (!GetDSAMPtr_Common()->usingExtFlag)
		return(filePath);
	Snprintf_Utility_String(guiFilePath, MAX_FILE_PATH, wxT("%s/%s"),
	  appInterfacePtr->workingDirectory, appInterfacePtr->simulationFile);
	return(guiFilePath);

}

/************************ ParseParSpecifiers **********************************/

/*
 * This routine parses the parameter specifiers.
 * It expects the strings spaces passed as arguments to have sufficient space.
 */

void
ParseParSpecifiers_AppInterface(WChar *parName, WChar *appName,
  WChar *subProcess)
{
	WChar	*p;

	*appName = *subProcess = '\0';
	if ((p = DSAM_strchr(parName, UNIPAR_NAME_SEPARATOR)) == NULL)
		return;
	*p = '\0';
	DSAM_strncpy(appName, p + 1, MAXLINE);
	if ((p = DSAM_strchr(appName, UNIPAR_NAME_SEPARATOR)) == NULL)
		return;
	*p = '\0';
	DSAM_strncpy(subProcess, p + 1, MAXLINE);

}

/************************ SetProgramParValue **********************************/

/*
 * This function sets a program parameter.
 * A copy of the 'parList' must be taken, as the argument 'parList' it can be
 * changed by the 'FindUniPar_UniParMgr' routine.
 */

DSAM_API BOOLN
SetProgramParValue_AppInterface(const WChar *parName, WChar *parValue, BOOLN readSPF)
{
	static const WChar *funcName = wxT("SetProgramParValue_AppInterface");
	BOOLN	ok = TRUE, creatorApp = TRUE;
	WChar	parNameCopy[MAXLINE], appName[MAXLINE], subProcess[MAXLINE];
	UniParPtr	par;
	UniParListPtr	parList;

	DSAM_strncpy(parNameCopy, parName, MAXLINE);
	ParseParSpecifiers_AppInterface(parNameCopy, appName, subProcess);
	if (*appName)  {
		creatorApp = (StrNCmpNoCase_Utility_String(appInterfacePtr->appName,
		  appName) == 0);
		if (!creatorApp && !readSPF)
			return(FALSE);
	}
	parList = appInterfacePtr->parList;
	if (*subProcess && (*subProcess != '0')) {
		if ((par = FindUniPar_UniParMgr(&parList, subProcess,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			NotifyError(wxT("%s: Unknown sub-process '%s' for application."),
			  funcName, subProcess);
			return(FALSE);
		} else
			parList = *(par->valuePtr.parList.list);
	}
	if ((par = FindUniPar_UniParMgr(&parList, parNameCopy,
	  UNIPAR_SEARCH_ABBR)) == NULL) {
		if (creatorApp)
			ok = FALSE;
	} else if (((parList != appInterfacePtr->parList) || (par->index !=
	  APP_INT_SIMULATIONFILE) || (appInterfacePtr->audModel &&
	 (GetSimFileType_ModuleMgr(appInterfacePtr->audModel) ==
	 UTILITY_SIMSCRIPT_SPF_FILE))) && !SetParValue_UniParMgr(&parList,
	 par->index, parValue))
		ok = FALSE;
	return(ok);

}

/****************************** GetSimulation *********************************/

/*
 * This function returns a pointer to the application interface's audModel
 * simulation.
 */

DSAM_API DatumPtr
GetSimulation_AppInterface(void)
{
	static const WChar *funcName = wxT("GetSimulation_AppInterface");
	DatumPtr	simulation;

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(NULL);
	}
	if (!appInterfacePtr->audModel) {
		NotifyError(wxT("%s: Simulation EarObject nod initialised."), funcName);
		return(NULL);
	}
	if ((simulation = GetSimulation_ModuleMgr(appInterfacePtr->audModel)) ==
	   NULL) {
		NotifyError(wxT("%s: Simulation not initialised."), funcName);
		return(NULL);
	}
	return(simulation);

}

/****************************** GetSimPtr *************************************/

/*
 * This function returns a pointer to the application interface's audModel
 * simulation pointer.
 */

DSAM_API DatumPtr *
GetSimPtr_AppInterface(void)
{
	static const WChar *funcName = wxT("GetSimPtr_AppInterface");
	DatumPtr	*simPtr;

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(NULL);
	}
	if (!appInterfacePtr->audModel) {
		NotifyError(wxT("%s: Simulation EarObject nod initialised."), funcName);
		return(NULL);
	}
	if ((simPtr = GetSimPtr_ModuleMgr(appInterfacePtr->audModel)) == NULL) {
		NotifyError(wxT("%s: Simulation not initialised."), funcName);
		return(NULL);
	}
	return(simPtr);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters using the specified file
 * pointer.
 * It is meant to be read from within the Main Program ReadPars routine.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_AppInterface(WChar *parFileName)
{
	static const WChar *funcName = wxT("ReadPars_AppInterface");
	BOOLN	ok = TRUE;
	WChar	*filePath;
	WChar	parName[MAXLINE], appName[MAXLINE], subProcess[MAXLINE];
	WChar	parValue[MAX_FILE_PATH];
	FILE	*fp;
	UniParPtr	par;
	UniParListPtr	tempParList;

	filePath = GetParsFileFPath_Common(parFileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  parFileName);
		return(FALSE);
	}
	Init_ParFile();
	SetEmptyLineMessage_ParFile(FALSE);
	while (GetPars_ParFile(fp, wxT("%s %s"), parName, parValue)) {
		ParseParSpecifiers_AppInterface(parName, appName, subProcess);
		tempParList = appInterfacePtr->parList;
		if (*subProcess) {
			if ((par = FindUniPar_UniParMgr(&tempParList, subProcess,
			  UNIPAR_SEARCH_ABBR)) == NULL) {
				NotifyError(wxT("%s: Unknown sub-process '%s' for ")
				  wxT("application."), funcName, subProcess);
				ok = FALSE;
				break;
			} else
				tempParList = *(par->valuePtr.parList.list);
		}
		if ((par = FindUniPar_UniParMgr(&tempParList, parName,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			NotifyError(wxT("%s: Unknown parameter '%s' for application."),
			  funcName, parName);
			ok = FALSE;
		} else {
			if (!SetParValue_UniParMgr(&tempParList, par->index, parValue))
				ok = FALSE;
		}

	}
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Could not set parameters for application."),
		  funcName);
		return(FALSE);
	}
	return(TRUE);

}

/************************ ReadProgParFile *************************************/

/*
 * This routine reads the parameters, saved in the simulation parameter file,
 * which control the main program parameters.  These parameters are found
 * after the 'SIMSCRIPT_SIMPARFILE_DIVIDER' string, if one exists.
 * The 'filePath' variable must have storage space here, as setting one of the
 * parameter values may overwrite the 'static' storage for the
 * 'GetParsFileFPath_DSAM' routine.
 * This routine currently ignores any characters after and including a period,
 * "." in the parameter name.
 * It returns FALSE if it fails in any way.
 */

BOOLN
ReadProgParFile_AppInterface(void)
{
	static const WChar	*funcName = wxT("ReadProgParFile_AppInterface");
	static BOOLN	readProgParFileFlag = FALSE;
	BOOLN	ok = TRUE, foundDivider = FALSE;
	WChar	parName[MAXLINE], parValue[MAX_FILE_PATH], oldSPF[MAX_FILE_PATH];
	FILE	*fp;

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (readProgParFileFlag)
		return(TRUE);
	readProgParFileFlag = TRUE;
	if ((fp = DSAM_fopen(appInterfacePtr->simulationFile,
	  "r")) == NULL) {
		NotifyError(wxT("%s: Could not open '%s' parameter file."), funcName,
		  GetFilePath_AppInterface(appInterfacePtr->simulationFile));
		readProgParFileFlag = FALSE;
		return(FALSE);
	}
	DSAM_strcpy(oldSPF, appInterfacePtr->simulationFile);
	Init_ParFile();
	SetEmptyLineMessage_ParFile(FALSE);
	while (!foundDivider && GetPars_ParFile(fp, wxT("%s %s"), parName,
	  parValue))
		if (DSAM_strcmp(parName, SIMSCRIPT_SIMPARFILE_DIVIDER) == 0)
			foundDivider = TRUE;
	if (!foundDivider) {
		Free_ParFile();
		readProgParFileFlag = FALSE;
		return(TRUE);
	}
	while (ok && GetPars_ParFile(fp, wxT("%s %s"), parName, parValue))
		ok = SetProgramParValue_AppInterface(parName, parValue, TRUE);
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	readProgParFileFlag = FALSE;
	if (!ok && (DSAM_strcmp(parName, SIMSCRIPT_SIMPARFILE_SDI_DIVIDER) != 0)) {
		NotifyError(wxT("%s: Invalid parameters in file '%s', program ")
		  wxT("parameter  section (%s)."), funcName, GetFilePath_AppInterface(
		  appInterfacePtr->simulationFile), parName);
		return(FALSE);
	}
	DSAM_strcpy(appInterfacePtr->simulationFile, oldSPF);
	return(TRUE);

}

/****************************** InitSimFromSimScript **************************/

/*
 * This routine Initialises the simulation from a simulation script.
 */

BOOLN
InitSimFromSimScript_AppInterface(void)
{
	static const WChar *funcName = wxT("InitSimFromSimScript_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (!appInterfacePtr->audModel) {
		FreeSim_AppInterface();
		if ((appInterfacePtr->audModel = Init_EarObject(wxT(
		  "Util_SimScript"))) == NULL) {
			NotifyError(wxT("%s: Could not initialise process."), funcName);
			FreeSim_AppInterface();
			appInterfacePtr->checkMainInit = TRUE;
			return(FALSE);
		}

	}
	return(TRUE);

}

/****************************** FreeSim ***************************************/

/*
 * This routine resets the simulation.
 */

DSAM_API void
FreeSim_AppInterface(void)
{
	Free_EarObject(&appInterfacePtr->audModel);
	ResetStepCount_Utility_Datum();
	appInterfacePtr->updateProcessVariablesFlag = TRUE;

}

/****************************** SetSimFileType ********************************/

/*
 * This function sets the module's simParFileFlag field.
 */

DSAM_API BOOLN
SetSimFileType_AppInterface(int simFileType)
{
	static const WChar	*funcName = wxT("SetSimFileType_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->simFileType = simFileType;
	return(TRUE);

}

/****************************** SetWorkingDirectory ***************************/

/*
 * This function sets the module's workingDirectory field.
 */

DSAM_API BOOLN
SetWorkingDirectory_AppInterface(WChar * workingDirectory)
{
	static const WChar	*funcName = wxT("SetWorkingDirectory_AppInterface");

	if (appInterfacePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(appInterfacePtr->workingDirectory, workingDirectory,
	  MAX_FILE_PATH);
	return(TRUE);

}

/****************************** InitSimulation ********************************/

/*
 * This routine Initialises the simulation.
 */

BOOLN
InitSimulation_AppInterface(void)
{
	static const WChar *funcName = wxT("InitSimulation_AppInterface");
	BOOLN	ok = TRUE;

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (!appInterfacePtr->audModel) {
		FreeSim_AppInterface();
		if ((appInterfacePtr->audModel = Init_EarObject(wxT(
		  "Util_SimScript"))) == NULL) {
			NotifyError(wxT("%s: Could not initialise process."), funcName);
			ok = FALSE;
		}
		if (GetDSAMPtr_Common()->usingGUIFlag) {
			SET_PARS_POINTER(appInterfacePtr->audModel);
			SetSimFileType_Utility_SimScript(appInterfacePtr->simFileType);
			SetParsFilePath_Utility_SimScript(appInterfacePtr->
			  workingDirectory);
			SetSimFileName_Utility_SimScript(appInterfacePtr->simulationFile);
		}
	}
	if (ok && !ReadPars_ModuleMgr(appInterfacePtr->audModel, appInterfacePtr->
	  simulationFile))
		ok = FALSE;
	if (!ok) {
		FreeSim_AppInterface();
		return(FALSE);
	}

	SetTitle_AppInterface(appInterfacePtr->simulationFile);
	if (GetDSAMPtr_Common()->usingGUIFlag)
		PrintPars_AppInterface();
	return(TRUE);

}

/****************************** SetAppName ************************************/

/*
 * This routine sets the global appName variable.
 */

DSAM_API BOOLN
SetAppName_AppInterface(const WChar *appName)
{
	static const WChar *funcName = wxT("SetAppName_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}

	DSAM_strncpy(appInterfacePtr->appName, appName, MAX_FILE_PATH);
	return(TRUE);

}

/****************************** SetAppVersion *********************************/

/*
 * This routine sets the appVersion variable.
 */

DSAM_API BOOLN
SetAppVersion_AppInterface(const WChar *appVersion)
{
	static const WChar *funcName = wxT("SetAppVersion_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}

	DSAM_strcpy(appInterfacePtr->appVersion, appVersion);
	return(TRUE);

}

/****************************** SetCompiledDSAMVersion ************************/

/*
 * This routine sets the version of DSAM that the application was compiled
 * with, as compared to the dynamic library that is being used.
 */

DSAM_API BOOLN
SetCompiledDSAMVersion_AppInterface(const WChar *compiledDSAMVersion)
{
	static const WChar *funcName = wxT("SetCompiledDSAMVersion_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}

	DSAM_strcpy(appInterfacePtr->compiledDSAMVersion, compiledDSAMVersion);
	return(TRUE);

}

/****************************** SetTitle **************************************/

/*
 * This routine sets the title that is used in the simulation manager window.
 */

BOOLN
SetTitle_AppInterface(const WChar *title)
{
	static const WChar *funcName = wxT("SetTitle_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	DSAM_strncpy(appInterfacePtr->title, title, MAX_FILE_PATH);
	return(TRUE);

}

/****************************** AddAppHelpBook ********************************/

/*
 * This function adds a help book to the application's help book list.
 * It returns FALSE if it fails in any way
 */

DSAM_API BOOLN
AddAppHelpBook_AppInterface(const WChar *bookName)
{
	static const WChar *funcName = wxT("AddAppHelpBook_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (*bookName == '\0') {
		NotifyError(wxT("%s: Book name not set.\n"), funcName);
		return(FALSE);
	}
	if ((appInterfacePtr->numHelpBooks - 1) == APP_MAX_HELP_BOOKS) {
		NotifyError(wxT("%s: Maximum number of help books added (%d)."),
		  funcName, APP_MAX_HELP_BOOKS);
		return(FALSE);
	}
	DSAM_strcpy(appInterfacePtr->appHelpBooks[appInterfacePtr->numHelpBooks++],
	  bookName);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This routine carries out general initialisation tasks for the application
 * interface.
 */

BOOLN
SetPars_AppInterface(WChar *diagMode, WChar *simulationFile, WChar *segmentMode)
{
	static const WChar *funcName = wxT("SetPars_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (!SetDiagMode_AppInterface(diagMode)) {
		NotifyError(wxT("%s: Could not set diagnostic mode."), funcName);
		return(FALSE);
	}
	if (!SetSimulationFile_AppInterface(simulationFile)) {
		NotifyError(wxT("%s: Could not set simulation file mode."), funcName);
		return(FALSE);
	}
	if (!SetSegmentMode_AppInterface(segmentMode)) {
		NotifyError(wxT("%s: Could not set segment processing mode."),
		  funcName);
		return(FALSE);
	}
	appInterfacePtr->checkMainInit = FALSE;
	return(TRUE);

}

/****************************** ListParsAndExit *******************************/

/*
 * This routine prints the simulation parameters and exits.
 */

void
ListParsAndExit_AppInterface(void)
{
	SetUsingGUIStatus(FALSE);
	SetDPrintFunc(DPrintStandard);
	SetParsFile_Common(wxT("screen"), OVERWRITE);
	ListParameters_AppInterface();
	exit(0);

}

/****************************** ListParameters ********************************/

/*
 * This routine prints the simulation and main program parameters.
 */

DSAM_API BOOLN
ListParameters_AppInterface(void)
{
	static const WChar *funcName = wxT("ListParameters_AppInterface");
	WChar	suffix[MAXLINE];

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (!PrintSimParFile_ModuleMgr(appInterfacePtr->audModel)) {
		NotifyError(wxT("%s: Could not print simulation parameter file."),
		  funcName);
		return(FALSE);
	}
	Snprintf_Utility_String(suffix, MAXLINE, wxT(".%s%s"), appInterfacePtr->
	  appName, UNIPAR_TOP_PARENT_LABEL);
	PrintPars_UniParMgr(appInterfacePtr->parList, wxT(""), suffix);
	return(TRUE);

}

/****************************** ListCFListAndExit *****************************/

/*
 * This routine prints the simulation parameters and exits.
 */

void
ListCFListAndExit_AppInterface(void)
{
	static const WChar *funcName = wxT("ListCFListAndExit_AppInterface");
	CFListPtr	theBMCFs;
	DatumPtr	simulation, bMDatumPtr;

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		exit(1);
	}
	if ((simulation = GetSimulation_ModuleMgr(appInterfacePtr->audModel)) ==
	   NULL) {
		NotifyError(wxT("%s: Simulation not initialised."), funcName);
		exit(1);
	}
	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
	SetParsFile_Common(wxT("screen"), OVERWRITE);
	if (((bMDatumPtr = FindModuleProcessInst_Utility_Datum(simulation,
	  wxT("BM_"))) == NULL) || ((theBMCFs = *GetUniParPtr_ModuleMgr(
	  bMDatumPtr->data, wxT("cflist"))->valuePtr.cFPtr) == NULL)) {
		NotifyError(wxT("%s: Could not list CFList."), funcName);
		exit(1);
	}
	PrintList_CFList(theBMCFs);
	exit(0);

}

/****************************** ResetCommandArgFlags **************************/

/*
 * This routine sets the appInterfacePtr command arg flags so that the
 * InitProcessVariables routine knows when to process the options or parameter
 * commands.
 */

DSAM_API void
ResetCommandArgFlags_AppInterface(void)
{
	static const WChar *funcName = wxT("ResetCommandArgFlags_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		exit(1);
	}
	appInterfacePtr->checkMainInit = TRUE;
	appInterfacePtr->useParComsFlag = TRUE;

}

/****************************** SetArgcAndArgV ********************************/

/*
 * This routine sets the appInterfacePtr argc and argv parameters.
 * It creates its own copy of the values.
 * The global variables are also set.
 */

DSAM_API void
SetArgcAndArgV_AppInterface(int theArgc, WChar **theArgv)
{
	static const WChar *funcName = wxT("SetArgcAndArgV_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		exit(1);
	}
	appInterfacePtr->argc = theArgc;
	appInterfacePtr->argv = theArgv;

}

/****************************** InitProcessVariables **************************/

/*
 * This function initialises the simulation process.
 * This routine is a special one, in that it actually initialises the
 * application interface too.
 */

DSAM_API BOOLN
InitProcessVariables_AppInterface(BOOLN (* Init)(void), int theArgc,
  WChar **theArgv)
{
	static const WChar *funcName = wxT("InitProcessVariables_AppInterface");
	AppInterfacePtr	p;

	if (!appInterfacePtr && !Init_AppInterface(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise the application interface."),
		  funcName);
		exit(1);
	}
	p = appInterfacePtr;
	setlocale(LC_ALL, "");
	if (Init) {
		if (!GetDSAMPtr_Common()->appInitialisedFlag && !(* Init)())
			return(FALSE);
		if (!GetDSAMPtr_Common()->appInitialisedFlag) {
			p->Init = Init;
			if (p->RegisterUserModules) {
				if (!InitUserModuleList_ModuleReg(p->maxUserModules)) {
					NotifyError(wxT("%s: Could not initialise user module ")
					  wxT("list."), funcName);
					return(FALSE);
				}
				if (!(* p->RegisterUserModules)()) {
					NotifyError(wxT("%s: Failed to register user modules."),
					  funcName);
					return(FALSE);
				}
			}
			if (p->SetUniParList)
				(* p->SetUniParList)(&p->appParList);
			GetDSAMPtr_Common()->appInitialisedFlag = TRUE;
			if (GetDSAMPtr_Common()->usingExtFlag)
				return(FALSE);
		}
	}

	SetArgcAndArgV_AppInterface(theArgc, theArgv);
	if (p->checkMainInit) {
		ProcessOptions_AppInterface();
		if (p->printUsageFlag) {
			PrintUsage_AppInterface();
			if (p->PrintUsage)
				(* p->PrintUsage)();
			exit(0);
		}
		DPrint(wxT("Starting %s Application version %s [DSAM Version: %s ")
		  wxT("(dynamic),\n%s (compiled)]...\n"), p->appName, p->appVersion,
		  GetDSAMPtr_Common()->version, p->compiledDSAMVersion);
	}
	if (p->updateProcessVariablesFlag) {
		if (p->readAppParFileFlag) {
			if (!ReadPars_AppInterface(p->appParFile)) {
				NotifyError(wxT("%s: Failed to set application parameters."),
				  funcName);
				return(FALSE);
			}
			p->readAppParFileFlag = FALSE;
		}
		if ((p->canLoadSimulationFlag || p->listParsAndExit ||
		  p->listCFListAndExit) && p->simulationFileFlag) {
			if (!InitSimulation_AppInterface()) {
				NotifyError(wxT("%s: Could not Initialise simulation."),
				  funcName);
				return(FALSE);
			}
			SetParsFilePath_Common(GetParsFilePath_ModuleMgr(p->audModel));
			if ((GetSimFileType_ModuleMgr(p->audModel) ==
			  UTILITY_SIMSCRIPT_SPF_FILE) && !ReadProgParFile_AppInterface()) {
				NotifyError(wxT("%s: Could not read the program settings in\n")
				  wxT("file '%s'."), funcName, GetFilePath_AppInterface(
				  p->simulationFile));
				return(FALSE);
			}
			if (!ProcessParComs_AppInterface())
				return(FALSE);
		}
		if (p->PostInitFunc && !(* p->PostInitFunc)()) {
			NotifyError(wxT("%s: Failed to run post initialisation function."),
			  funcName);
			return(FALSE);
		}

		if (p->listParsAndExit)
			ListParsAndExit_AppInterface();
		if (p->listCFListAndExit)
			ListCFListAndExit_AppInterface();

		p->updateProcessVariablesFlag = FALSE;

	}
	p->checkMainInit = FALSE;
	if (!ProcessParComs_AppInterface())
		return(FALSE);
	return(TRUE);

}

/****************************** SetCanFreePtrFlag *****************************/

/*
 * This routine set the structure's 'canFreePtrFlag' which is used by the
 * "Free_" to decide whether it does anything when called.
 */

DSAM_API BOOLN
SetCanFreePtrFlag_AppInterface(BOOLN status)
{
	if (!appInterfacePtr) {
		return(FALSE);
	}
	appInterfacePtr->canFreePtrFlag = status;
	return(TRUE);

}

/****************************** GetPtr ****************************************/

/*
 * This function returns the application interface pointer.
 * This routine is useful for the VC++ compile.
 */

DSAM_API AppInterfacePtr
GetPtr_AppInterface(void)
{
	static const WChar *funcName = wxT("GetPtr_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(NULL);
	}
	return(appInterfacePtr);

}

/****************************** GetUniParPtr **********************************/

/*
 * This function returns a universal parameter for the main application
 * simulation.
 * It returns NULL if it fails in any way.
 */

UniParPtr
GetUniParPtr_AppInterface(WChar *parName)
{
	static const WChar *funcName = wxT("GetUniParPtr_AppInterface");
	UniParPtr	par;

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(NULL);
	}
	if ((par = GetUniParPtr_ModuleMgr(appInterfacePtr->audModel, parName)) ==
	  NULL) {
		NotifyError(wxT("%s: Could not find simulation parameter '%s'."),
		  funcName, parName);
		return(NULL);
	}
	return(par);

}

/****************************** PrintSimPars **********************************/

/*
 * This function prints the parameters for the main auditory model simulation.
 * It returns NULL if it fails in any way.
 */

DSAM_API BOOLN
PrintSimPars_AppInterface(void)
{
	static const WChar *funcName = wxT("PrintSimPars_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	return(PrintPars_ModuleMgr(appInterfacePtr->audModel));

}

/****************************** ResetSim **************************************/

/*
 * This routine resets the processes in the main auditory model simulation.
 */

DSAM_API BOOLN
ResetSim_AppInterface(void)
{
	static const WChar *funcName = wxT("ResetSim_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	ResetProcess_EarObject(appInterfacePtr->audModel);
	return(TRUE);

}

/****************************** RunSim ****************************************/

/*
 * This routine runs the main auditory model simulation.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
RunSim_AppInterface(void)
{
	static const WChar *funcName = wxT("RunSim_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	if (!appInterfacePtr->audModel) {
		NotifyError(wxT("%s: No simulation set."), funcName);
		return(FALSE);
	}
	if (!RunProcess_ModuleMgr(appInterfacePtr->audModel)) {
		NotifyError(wxT("%s: Could not run main auditory model simulation ")
		  wxT("'%s'."), funcName, appInterfacePtr->simulationFile);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GetSimProcess *********************************/

/*
 * This routine returns the main auditory model simulation process EarObject.
 */

DSAM_API EarObjectPtr
GetSimProcess_AppInterface(void)
{
	static const WChar *funcName = wxT("GetSimProcess_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(NULL);
	}
	if (!appInterfacePtr->audModel) {
		NotifyError(wxT("%s: Simulation EarObject not initialised."),
		  funcName);
		return(NULL);
	}
	return(appInterfacePtr->audModel);

}

/****************************** SetSimPar *************************************/

/*
 * This function sets a parameter in the main auditory model simulation
 * process EarObject.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetSimPar_AppInterface(WChar *parName, WChar *value)
{
	static const WChar *funcName = wxT("SetSimPar_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	return(SetPar_ModuleMgr(appInterfacePtr->audModel, parName, value));

}

/****************************** SetRealSimPar *********************************/

/*
 * This function sets a real parameter in the main auditory model simulation
 * process EarObject.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealSimPar_AppInterface(WChar *parName, Float value)
{
	static const WChar *funcName = wxT("SetRealSimPar_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	return(SetRealPar_ModuleMgr(appInterfacePtr->audModel, parName, value));

}

/****************************** SetRealArraySimPar ****************************/

/*
 * This function sets a real array parameter element in the main auditory model
 * simulation process EarObject.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealArraySimPar_AppInterface(WChar *parName, int index, Float value)
{
	static const WChar *funcName = wxT("SetRealArraySimPar_AppInterface");

	if (!appInterfacePtr) {
		NotifyError(wxT("%s: Application interface not initialised."),
		  funcName);
		return(FALSE);
	}
	return(SetRealArrayPar_ModuleMgr(appInterfacePtr->audModel, parName, index,
	  value));

}

/****************************** OnExit ***************************************/

/*
 * This routine calls the application's exit routine, if on exists.
 */

DSAM_API void
OnExit_AppInterface(void)
{
	if (appInterfacePtr->OnExit)
		(* appInterfacePtr->OnExit)();
}

/****************************** OnExecute *************************************/

/*
 * This routine calls the application's OnExecute routine if one exists.
 * This routine is used when the application interface needs to do various
 * other things, i.e. under the GUI.
 */

DSAM_API BOOLN
OnExecute_AppInterface(void)
{
	BOOLN	ok;

	if (!appInterfacePtr->OnExecute)
		return(FALSE);
	appInterfacePtr->simulationFinishedFlag = FALSE;
	ok = (* appInterfacePtr->OnExecute)();
	return(ok);

}

/****************************** GetDataFileInProcess **************************/

/*
 * This routine returns a pointer to the a 'DataFile_In' process at the
 * beginning of a simulation.  If it is not there, then it returns NULL.
 */

DSAM_API EarObjectPtr
GetDataFileInProcess_AppInterface(void)
{
	FILE	*savedErrorsFilePtr = GetDSAMPtr_Common()->errorsFile;
	EarObjectPtr	process;

	SetErrorsFile_Common(wxT("off"), OVERWRITE);
	process = GetFirstProcess_Utility_Datum(GetSimulation_AppInterface());
	GetDSAMPtr_Common()->errorsFile = savedErrorsFilePtr;
	if (!process)
		return(NULL);
	if (StrCmpNoCase_Utility_String(process->module->name, wxT(
	  "DataFile_In")) != 0)
		return(NULL);
	return(process);

}

