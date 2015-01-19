/**********************
 *
 * File:		UtSimScript.c
 * Purpose:		This is the source file for the simulation script reading
 *				module.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				21-08-97 LPO: Removed rampInterval parameter - use new ramp
 *				module.
 *				26-11-97 LPO: Using new GeNSpecLists module for 'operationMode'
 *				parameter.
 *				01-05-98 LPO: The GetInSignalRef_EarObject routine is now used
 *				to ensure that the input signals are correctly transferred to
 *				the first process, and that the numInputs count is updated
 *				At the moment the input signals to a simulation script
 *				are always re-connected each time the process is run.  This is
 *				because to add the connection to the supplier/customer lists
 *				the process would need the previous process' EarObjectPtr.
 *				24-09-1998 LPO: Introduced the parameter file mode.  If it is
 *				'on' then parameters for the simulation will be read from a
 *				file with the same name as the simulation script, but
 *				with the ".spf" extension.  A simulation parameter file (SPF)
 *				file will be created if one does not already exist.
 *				30-09-98 LPO: I have introduced the uniParLockFlag field which
 *				is set to disallow command line changes of the universal
 *				parameters when a uniParList file is being used.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for
 *				modules with no parameters.
 *				02-02-99 LPO: The 'SetUniParValue_' has been moved to the
 *				UtDatum module.
 *				25-05-99 LPO: Introduced the 'parsFilePath' field.  All modules
 *				now read their parameter files using a path relative to the
 *				path of the simulation script file.
 *				03-06-99 LPO: Introduced 'SetUniParValue_' routine, so that it
 *				can be called from 'GeModuleMgr' module.
 *				23-08-99 LPO: The 'NotifyError_' routine was not calling the
 *				CheckInitErrorsFile_Common" routine so the
 *				'GetDSAMPtr_Common()->errorsFile' field was not set and was
 *				causing a crash.
 * Author:		L. P. O'mard
 * Created:		19 Jan 1996
 * Updated:		23 Aug 1999
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeModuleReg.h"
#include "GeNSpecLists.h"

#include "FiParFile.h"

#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtString.h"
#include "UtSimScript.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SimScriptPtr	simScriptPtr = NULL;
BOOLN	(* ReadXMLSimFile_SimScript)(WChar *fileName) = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_Utility_SimScript(void)
{
	if (simScriptPtr == NULL)
		return(FALSE);
	FreeSimulation_Utility_SimScript();
	if (simScriptPtr->parFilePathModeList)
		free(simScriptPtr->parFilePathModeList);
	if (simScriptPtr->labelBList)
		FreeList_Utility_DynaBList(&simScriptPtr->labelBList);
	if (simScriptPtr->parList)
		FreeList_UniParMgr(&simScriptPtr->parList);
	if (simScriptPtr->parSpec == GLOBAL) {
		free(simScriptPtr);
		simScriptPtr = NULL;
	}
	return(TRUE);

}

/****************************** ParFilePathModePrototypeList ******************/

/*
 * This function returns a pointer to a prototype name list.
 */

DSAM_API NameSpecifierPtr
ParFilePathModePrototypeList_Utility_SimScript(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("RELATIVE"),	UTILITY_SIMSCRIPT_PARFILEPATHMODE_RELATIVE },
			{ NO_FILE,			UTILITY_SIMSCRIPT_PARFILEPATHMODE_PATH },
			{ NULL,				UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL },
		};
	return(modeList);

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
Init_Utility_SimScript(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_SimScript");

	if (parSpec == GLOBAL) {
		if (simScriptPtr != NULL)
			Free_Utility_SimScript();
		if ((simScriptPtr = (SimScriptPtr) malloc(sizeof(SimScript))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (simScriptPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	simScriptPtr->parSpec = parSpec;
	simScriptPtr->simulation = NULL;
	simScriptPtr->operationMode = GENERAL_BOOLEAN_OFF;
	simScriptPtr->parFilePathMode = UTILITY_SIMSCRIPT_PARFILEPATHMODE_RELATIVE;

	if ((simScriptPtr->parFilePathModeList = InitNameList_NSpecLists(
	  ParFilePathModePrototypeList_Utility_SimScript(),
	  simScriptPtr->parsFilePath)) == NULL)
		return(FALSE);
	if (!SetUniParList_Utility_SimScript()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_SimScript();
		return(FALSE);
	}
	simScriptPtr->symList = NULL;
	simScriptPtr->simFileType = UTILITY_SIMSCRIPT_UNKNOWN_FILE;
	DSAM_strcpy(simScriptPtr->parsFilePath, wxT("No path"));
	simScriptPtr->lineNumber = 0;
	simScriptPtr->simPtr = NULL;
	simScriptPtr->subSimList = NULL;
	simScriptPtr->labelBList = NULL;
	simScriptPtr->labelBListPtr = &simScriptPtr->labelBList;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_SimScript(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_SimScript");
	UniParPtr	pars;

	if ((simScriptPtr->parList = InitList_UniParMgr(UNIPAR_SET_SIMSPEC,
	  UTILITY_SIMSCRIPT_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = simScriptPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_SIMSCRIPT_OPERATIONMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostics operation mode ('ON' or 'OFF')."),
	  UNIPAR_BOOL,
	  &simScriptPtr->operationMode, NULL,
	  (void * (*)) SetOperationMode_Utility_SimScript);
	SetPar_UniParMgr(&pars[UTILITY_SIMSCRIPT_PARFILEPATHMODE],
	  wxT("PAR_FILE_PATH_MODE"),
	  wxT("Parameter file path mode ('relative', 'none' or <file path>)."),
	  UNIPAR_NAME_SPEC_WITH_FPATH,
	  &simScriptPtr->parFilePathMode, simScriptPtr->parFilePathModeList,
	  (void * (*)) SetParFilePathMode_Utility_SimScript);
	SetPar_UniParMgr(&pars[UTILITY_SIMSCRIPT_SIMULATION], wxT("SIMULATION"),
	  wxT("Simulation script."),
	  UNIPAR_SIMSCRIPT,
	  &simScriptPtr->simulation, simScriptPtr->simFileName,
	  (void * (*)) SetSimulation_Utility_SimScript);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_SimScript(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (simScriptPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(simScriptPtr->parList);

}

/****************************** SetSimulation *********************************/

/*
 * This function sets the module's simulation parameter.
 * Additional checks should be added as required.
 */

DSAM_API BOOLN
SetSimulation_Utility_SimScript(DatumPtr theSimulation)
{
	static const WChar	*funcName = wxT("SetSimulation_Utility_SimScript");
	BOOLN	ok = TRUE;

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!InitSimulation_Utility_SimScript(theSimulation)) {
		NotifyError(wxT("%s: Simulation not correctly Initialised."), funcName);
		return(FALSE);
	}
	if (!GetDSAMPtr_Common()->usingGUIFlag && simScriptPtr->labelBList)
		FreeList_Utility_DynaBList(&simScriptPtr->labelBList);
	simScriptPtr->simulation = theSimulation;
	return(ok);

}

/****************************** SetOperationMode ******************************/

/*
 * This function sets the module's operationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOperationMode_Utility_SimScript(const WChar *theOperationMode)
{
	static const WChar	*funcName = wxT("SetOperationMode_Utility_SimScript");
	int		specifier;

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOperationMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal operationMode name (%s)."), funcName,
		  theOperationMode);
		return(FALSE);
	}
	simScriptPtr->operationMode = specifier;
	return(TRUE);

}

/****************************** SetParFilePathMode ****************************/

/*
 * This function sets the module's parFilePathMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetParFilePathMode_Utility_SimScript(const WChar *theParFilePathMode)
{
	static const WChar	*funcName = wxT("SetParFilePathMode_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	simScriptPtr->parFilePathMode = Identify_NameSpecifier(theParFilePathMode,
	  simScriptPtr->parFilePathModeList);
	switch (simScriptPtr->parFilePathMode) {
	case UTILITY_SIMSCRIPT_PARFILEPATHMODE_PATH:
	case UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL:
		simScriptPtr->parFilePathMode = UTILITY_SIMSCRIPT_PARFILEPATHMODE_PATH;
		DSAM_strncpy(simScriptPtr->parsFilePath, theParFilePathMode,
		  MAX_FILE_PATH);
		break;
	default:
		break;
	}
	return(TRUE);

}

/****************************** SetUniParValue ********************************/

/*
 * This function sets universal parameter value for the module.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUniParValue_Utility_SimScript(WChar *parName, WChar *parValue)
{
	static const WChar	*funcName = wxT("SetUniParValue_Utility_SimScript");
	UniParPtr	par;

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((par = FindUniPar_UniParMgr(&simScriptPtr->parList, parName,
	  UNIPAR_SEARCH_ABBR)) == NULL) {
		NotifyError(wxT("%s: Could not find parameter '%s'."), funcName,
		  parName);
		return(FALSE);
	}
	return(SetParValue_UniParMgr(&simScriptPtr->parList, par->index, parValue));

}

/****************************** SetSimUniParValue *****************************/

/*
 * This function sets universal parameter values for the simulation.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSimUniParValue_Utility_SimScript(const WChar *parName, const WChar *parValue)
{
	static const WChar	*funcName = wxT("SetSimUniParValue_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!SetUniParValue_Utility_Datum(simScriptPtr->simulation, parName,
	  parValue)) {
		NotifyError(wxT("%s: Could not set '%s' value to '%s'."), funcName,
		  parName, parValue);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetControlParValue ****************************/

/*
 * This function sets the control parameter values for the simulation.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetControlParValue_Utility_SimScript(const WChar *label, const WChar *value,
  BOOLN diagsOn)
{
	static const WChar	*funcName = wxT("SetControlParValue_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!SetControlParValue_Utility_Datum(simScriptPtr->simulation, label,
	  value, diagsOn)) {
		if (diagsOn)
			NotifyError(wxT("%s: Could not set control labelled '%s' value to ")
			  wxT("'%s'."), funcName, label, value);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetSimFileType ********************************/

/*
 * This function sets the module's simParFileFlag field.
 */

BOOLN
SetSimFileType_Utility_SimScript(SimFileTypeSpecifier simFileType)
{
	static const WChar	*funcName = wxT("SetSimFileType_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	simScriptPtr->simFileType = simFileType;
	return(TRUE);

}

/****************************** SetSimFileName ********************************/

/*
 * This function sets the module's simFileName field.
 */

BOOLN
SetSimFileName_Utility_SimScript(WChar * simFileName)
{
	static const WChar	*funcName = wxT("SetSimFileName_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(simScriptPtr->simFileName, simFileName, MAX_FILE_PATH);
	return(TRUE);

}

/****************************** SetLabelBListPtr ******************************/

/*
 * This function sets the module's simFileName field.
 */

BOOLN
SetLabelBListPtr_Utility_SimScript(DynaBListPtr *labelBListPtr)
{
	static const WChar	*funcName = wxT("SetLabelBList_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	simScriptPtr->labelBListPtr = labelBListPtr;
	return(TRUE);

}

/****************************** SetParsFilePath *******************************/

/*
 * This function sets the module's parsFilePath field.
 */

DSAM_API BOOLN
SetParsFilePath_Utility_SimScript(WChar * parsFilePath)
{
	static const WChar	*funcName = wxT("SetParsFilePath_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(simScriptPtr->parsFilePath, parsFilePath, MAX_FILE_PATH);
	return(TRUE);

}

/****************************** PrintSimParFile *******************************/

/*
 * This routine prints the simulation script and the universal parameters.
 */

BOOLN
PrintSimParFile_Utility_SimScript(void)
{
	static WChar *funcName = wxT("PrintSimParFile_Utility_SimScript(");
	int		i;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(SIMSCRIPT_SIMPARFILE_VISUAL_SEPARATION_LINE);
	DPrint(wxT("## Simulation parameter file (SPF) written using DSAM version ")
	  wxT("%s.\n"), GetDSAMPtr_Common()->version);
	DPrint(SIMSCRIPT_SIMPARFILE_VISUAL_SEPARATION_LINE wxT("\n"));

	DPrint(wxT("##--------------- Simulation script ----------------------##")
	  wxT("\n"));
	for (i = 0; i < UTILITY_SIMSCRIPT_NUM_PARS; i++)
		if (simScriptPtr->parList->pars[i].index !=
		  UTILITY_SIMSCRIPT_SIMULATION)
			PrintPar_UniParMgr(&simScriptPtr->parList->pars[i], wxT(""),
			  wxT("\t"));
	DPrint(wxT("\n"));
	PrintInstructions_Utility_Datum(localSimScriptPtr->simulation, wxT(""),
	  DATUM_INITIAL_INDENT_LEVEL, wxT(""));
	DPrint(wxT("\n"));
	PrintParListModules_Utility_Datum(localSimScriptPtr->simulation, wxT(""));
	DPrint(SIMSCRIPT_SIMPARFILE_VISUAL_SEPARATION_LINE);
	DPrint(wxT("%s Simulation parameter file divider.\n"),
	  SIMSCRIPT_SIMPARFILE_DIVIDER);
	DPrint(SIMSCRIPT_SIMPARFILE_VISUAL_SEPARATION_LINE wxT("\n"));
	simScriptPtr = localSimScriptPtr;
	return(TRUE);

}

/************************ ReadSimParFile **************************************/

/*
 * Read a list of universal parameters from a simulation parameter file.
 */

BOOLN
ReadSimParFile_Utility_SimScript(FILE *fp)
{
	static const WChar	*funcName = wxT("ReadSimParFile_Utility_SimScript");
	BOOLN	ok = TRUE, foundDivider = FALSE;
	WChar	parName[MAXLINE], parValue[MAX_FILE_PATH];
	DatumPtr	simulation;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (localSimScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	FreeSimulation_Utility_SimScript();
	Init_ParFile();
	if ((simulation = Read_Utility_SimScript(fp)) == NULL)
		ok = FALSE;
	if (ok && !SetSimulation_Utility_SimScript(simulation)) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in ")
		  wxT("simulation parameter file."), funcName);
		ok = FALSE;
	}
	SetEmptyLineMessage_ParFile(FALSE);
	while (ok && !foundDivider && GetPars_ParFile(fp, wxT("%s %s"), parName,
	  parValue))
		if (DSAM_strcmp(parName, SIMSCRIPT_SIMPARFILE_DIVIDER) == 0)
			foundDivider = TRUE;
		else if (!SetUniParValue_Utility_Datum(localSimScriptPtr->simulation,
		  parName, parValue)) {
			NotifyError(wxT("%s: Could not set '%s' parameter."), funcName,
			  parName);
			ok = FALSE;
		}
	SetEmptyLineMessage_ParFile(TRUE);
	Free_ParFile();
	simScriptPtr = localSimScriptPtr;
	if (!ok) {
		NotifyError(wxT("%s: Invalid parameters, in simulation parameter file ")
		  wxT("'%s'."), funcName, simScriptPtr->simFileName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the GetDSAMPtr_Common()->parsFile file pointer.
 */

BOOLN
PrintPars_Utility_SimScript(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Simulation Script Utility Module Parameters:-\n"));
	DPrint(wxT("\tSimulation instruction list:-\n"));
	PrintInstructions_Utility_Datum(simScriptPtr->simulation, wxT(""),
	  DATUM_INITIAL_INDENT_LEVEL, wxT("\t"));
	DPrint(wxT("\tDiagnostics operation mode = %s,\n"),
	  BooleanList_NSpecLists(simScriptPtr->operationMode)->name);
	DPrint(wxT("\tParameter file path mode: %s \n"),
	  simScriptPtr->parFilePathModeList[simScriptPtr->parFilePathMode].name);
	return(TRUE);

}

/****************************** GetFilePath ***********************************/

/*
 * This routine returns the given filePath, depending upon whether GUI mode is
 * used.
 */

WChar *
GetFilePath_Utility_SimScript(WChar *filePath)
{
	static WChar guiFilePath[MAX_FILE_PATH];

	if (!GetDSAMPtr_Common()->usingGUIFlag)
		return(filePath);
	Snprintf_Utility_String(guiFilePath, MAX_FILE_PATH, wxT("%s/%s"),
	  simScriptPtr->parsFilePath, simScriptPtr->simFileName);
	return(guiFilePath);

}

/****************************** ReadSimScript *********************************/

/*
 * This program reads a simulation script from a file.
 * It uses the parser to read both the module parameters and the simulation.
 * This routine is special in the way that it treats the file path.  For this
 * module, all file paths for modules needs to be stored.
 * It returns FALSE if it fails in any way.
 */

BOOLN
ReadSimScript_Utility_SimScript(FILE *fp)
{
	static const WChar	*funcName = wxT("ReadSimScript_Utility_SimScript");
	DatumPtr	simulation;

	Init_ParFile();
	simulation = Read_Utility_SimScript(fp);
	Free_ParFile();
	if (!simulation) {
		NotifyError(wxT("%s: Parser could not read simulation."), funcName);
		return(FALSE);
	}
	if (!SetSimulation_Utility_SimScript(simulation)) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in ")
		  wxT("simulation script file."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** GetSimFileType **********************************/

/*
 * This routine returns the pointer to a simulation file format.
 * It also sets the simScriptPtr->simFileType field.
 */

DSAM_API SimFileTypeSpecifier
GetSimFileType_Utility_SimScript(WChar *suffix)
{
	static NameSpecifier simFileTypeList[] = {

		{wxT("SIM"),	UTILITY_SIMSCRIPT_SIM_FILE},
		{wxT("SPF"),	UTILITY_SIMSCRIPT_SPF_FILE},
		{wxT("XML"),	UTILITY_SIMSCRIPT_XML_FILE},
		{NULL, 			UTILITY_SIMSCRIPT_UNKNOWN_FILE}

	};

	return((SimFileTypeSpecifier) Identify_NameSpecifier(suffix,
	  simFileTypeList));

}

/****************************** ReadPars **************************************/

/*
 * This function sets the simulation for an EarObject, according to the
 * file name extension.
 * It sets the 'simParFileFlag' argument pointer accordingly.
 * If using the GUI, then the file path and file name are set by the GUI code.
 */

BOOLN
ReadPars_Utility_SimScript(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_Utility_SimScript");
	BOOLN	ok = TRUE;
	WChar	*filePath;
	FILE	*fp = NULL;
	SimScriptPtr	p = simScriptPtr;

	if (!p) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((p->simFileType == UTILITY_SIMSCRIPT_UNKNOWN_FILE) || p->simulation)
		p->simFileType = GetSimFileType_Utility_SimScript(
		  GetSuffix_Utility_String(fileName));
	filePath = GetParsFileFPath_Common(fileName);
	if (!GetDSAMPtr_Common()->usingGUIFlag)
		FindFilePathAndName_Common(filePath, p->parsFilePath, p->simFileName);
	if (((p->simFileType == UTILITY_SIMSCRIPT_SIM_FILE) || (p->simFileType ==
	  UTILITY_SIMSCRIPT_SPF_FILE)) && (fp = fopen(ConvUTF8_Utility_String(
	  filePath), "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  GetFilePath_Utility_SimScript(filePath));
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName,
	  GetFilePath_Utility_SimScript(filePath));
	switch (p->simFileType) {
	case UTILITY_SIMSCRIPT_SIM_FILE:
		if (!ReadSimScript_Utility_SimScript(fp)) {
			NotifyError(wxT("%s: Could not read simulation script from\nfile ")
			  wxT("'%s'."), funcName, GetFilePath_Utility_SimScript(filePath));
			ok = FALSE;
		}
		break;
	case UTILITY_SIMSCRIPT_SPF_FILE:
		if (!ReadSimParFile_Utility_SimScript(fp)) {
			NotifyError(wxT("%s: Could not read simulation parameter file ")
			  wxT("from\n file '%s'."), funcName, GetFilePath_Utility_SimScript(
			  filePath));
			ok = FALSE;
		}
		break;
	case UTILITY_SIMSCRIPT_XML_FILE:
		if (ReadXMLSimFile_SimScript)
			ok = (* ReadXMLSimFile_SimScript)(filePath);
		else  {
			NotifyError(wxT("%s: XML support not available."), funcName);
			ok = FALSE;
		}
		break;
	default:
		NotifyError(wxT("%s: Unknown simulation file type '%s'."), funcName,
		  GetFilePath_Utility_SimScript(filePath));
		ok = FALSE;
		break;
	}
	if (fp)
		fclose(fp);
	return(ok);

}

/****************************** GetSimulation *********************************/

/*
 * This routine returns a pointer to the module's simulation instruction list.
 * It returns NULL if it fails in any way.
 */

DatumPtr
GetSimulation_Utility_SimScript(void)
{
	static const WChar	*funcName = wxT("GetSimulation_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	return(simScriptPtr->simulation);

}

/****************************** GetLabelBList *********************************/

/*
 * This routine returns a pointer to the module's label binary list for use by
 * other modules.
 */

DynaBListPtr
GetLabelBList_Utility_SimScript(void)
{
	static const WChar	*funcName = wxT("GetLabelBList_Utility_SimScript");

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	return(simScriptPtr->labelBList);

}

/****************************** FreeSimulation ********************************/

/*
 * This routine frees a simulation, its symbols and resets the parameters
 * associated with it.
 * It returns FALSE if it fails in any way.
 */

BOOLN
FreeSimulation_Utility_SimScript(void)
{
	static WChar *funcName = wxT("FreeSimulation_Utility_SimScript(");
	SimScriptPtr		localSimScriptPtr = simScriptPtr;

	if (simScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!localSimScriptPtr->simulation)
		return(TRUE);
	FreeInstructions_Utility_Datum(&localSimScriptPtr->simulation);
	FreeSymbols_Utility_SSSymbols(&localSimScriptPtr->symList);
	localSimScriptPtr->lineNumber = 0;
	simScriptPtr = localSimScriptPtr;
	return(TRUE);

}

/****************************** Read ******************************************/

/*
 * This routine reads the simulation script from a file.
 * It returns a pointer to a list of instructions, which must be free'd by the
 * calling program.
 */

DatumPtr
Read_Utility_SimScript(FILE *fp)
{
	static const WChar	*funcName = wxT("Read_Utility_SimScript");
	int	yyparse(void);
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (localSimScriptPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised, the 'Init_...' function ")
		  wxT("must\nbe called."), funcName);
		return(NULL);
	}
	FreeSimulation_Utility_SimScript();
	localSimScriptPtr->fp = fp;
	localSimScriptPtr->simPtr = &localSimScriptPtr->simulation;
	if (!localSimScriptPtr->symList)
		InitKeyWords_Utility_SSSymbols(&simScriptPtr->symList);
	if (yyparse() != 0)
		FreeSimulation_Utility_SimScript();
	simScriptPtr = localSimScriptPtr;
	return(simScriptPtr->simulation);

}

/****************************** NotifyError ***********************************/

/*
 * This is the NotifyError routine for this module.  It also returns the
 * line number at which the error occured.
 */

void
NotifyError_Utility_SimScript(WChar *format, ...)
{
	WChar	msg[LONG_STRING];
	va_list	args;
#	if DSAM_USE_UNICODE
	WChar	newFormat[LONG_STRING];
#	endif

#	if DSAM_USE_UNICODE
	ConvIOFormat_Utility_String(newFormat, format, LONG_STRING);
	format = newFormat;
#	endif
	va_start(args, format);
	DSAM_vsnprintf(msg, LONG_STRING, format, args);
	va_end(args);
	NotifyError(wxT("SimScript Parser: %s [line %d]\n"), msg, simScriptPtr->
	  lineNumber + 1);

} /* NotifyError */

/****************************** InstallProcessInst ****************************/

/*
 * This routine initialises the EarObjects in a simulation.
 * It is called by the parser routine.
 * A return value of -1 means that parsing should continue.
 */

DatumPtr
InstallProcessInst_Utility_SimScripts(WChar *moduleName)
{
	static WChar *funcName = wxT("InstallProcessInst_Utility_SimScripts");
	DatumPtr	p;

	if (!GetRegEntry_ModuleReg(moduleName)) {
		NotifyError_Utility_SimScript(wxT("%s: process module '%s' not known."),
		  funcName, moduleName);
		return(NULL);
	}
	if ((p = InstallInst_Utility_Datum(simScriptPtr->simPtr, PROCESS)) ==
	  NULL) {
		NotifyError_Utility_SimScript(wxT("%s: process module '%s' not known."),
		  funcName, moduleName);
		return(NULL);
	}
	p->u.proc.moduleName = InitString_Utility_String(moduleName);
	return(p);

}

/****************************** InitialiseEarObjects **************************/

/*
 * This routine initialises the EarObjects in a simulation.
 * It is called by the parser routine.
 * A return value of -1 means that parsing should continue.
 */

BOOLN
InitialiseEarObjects_Utility_SimScript(void)
{
	static WChar *funcName = wxT("InitialiseEarObjects_Utility_SimScript");
	BOOLN	ok = TRUE;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (!InitialiseEarObjects_Utility_Datum(*simScriptPtr->simPtr,
	  simScriptPtr->labelBListPtr)) {
		NotifyError(wxT("%s: Could not initialise EarObjects."), funcName);
		ok = FALSE;
	}
	simScriptPtr = localSimScriptPtr;
	return(ok);

}
	
/****************************** GetSimScriptSimulation ************************/

/*
 * This function returns the pointer to a simulation of a process node.
 * It checks that the process is the SIMSCRIPT_MODULE process.
 * It is used as a wrapper.
 * It exits if there is an error, as this would be a programming error anyway.
 */

DatumPtr *
GetSimScriptSimulation_Utility_SimScript(DatumPtr pc)
{
	static const WChar	*funcName = wxT(
	  "GetSimScriptSimulation_Utility_SimScript");

	if ((pc->type != PROCESS) || (pc->data->module->specifier !=
	  SIMSCRIPT_MODULE)) {
		NotifyError(wxT("%s: Not a 'SIMSCRIPT_MODULE' process."), funcName);
		exit(1);
	}
	return (&((SimScriptPtr) pc->data->module->parsPtr)->simulation);

}

/****************************** SetProcessSimPtr ******************************/

/*
 * This routine sets the simPtr pointer to the simulation for a process.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
SetProcessSimPtr_Utility_SimScript(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("SetProcessSimPtr_Utility_SimScript");
	SimScriptPtr	p;

	if (!data || (data->module->specifier != SIMSCRIPT_MODULE)) {
		NotifyError(wxT("%s: Not a 'SIMSCRIPT_MODULE' process."), funcName);
		return(FALSE);
	}
	p = (SimScriptPtr) data->module->parsPtr;
	p->simPtr = &p->simulation;
	return (TRUE);

}

/****************************** GetPtr ****************************************/

/*
 * This function returns the global simScriptPtr pointer.
 */

SimScriptPtr
GetPtr_Utility_SimScript(void)
{
	static const WChar	*funcName = wxT("SetProcessSimPtr_Utility_SimScript");

	if (!simScriptPtr) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(NULL);
	}
	return (simScriptPtr);

}

/****************************** SetReadXMLSimFile *****************************/

/*
 * This function sets the function for reading XML files.
 */

DSAM_API void
SetReadXMLSimFile_Utility_SimScript(BOOLN (* Func)(WChar *))
{
	ReadXMLSimFile_SimScript = Func;

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_SimScript(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_SimScript");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	simScriptPtr = (SimScriptPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_SimScript(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_SimScript");

	if (!SetParsPointer_Utility_SimScript(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_SimScript(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = simScriptPtr;
	theModule->Free = Free_Utility_SimScript;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_SimScript;
	theModule->PrintPars = PrintPars_Utility_SimScript;
	theModule->RunProcess = Process_Utility_SimScript;
	theModule->SetParsPointer = SetParsPointer_Utility_SimScript;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * If an input signal is available, then it is set as the input signal, other-
 * wise it will not be used.
 */

BOOLN
CheckData_Utility_SimScript(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_SimScript");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitSimulation **********************************/

/*
 * This routine initialises the simulation.
 * It sets the parameters for any sub-simulation. These may be reset for the
 * sub-simulation script if it is read from a file.
 */

BOOLN
InitSimulation_Utility_SimScript(DatumPtr simulation)
{
	static const WChar *funcName = wxT("InitSimulation_Utility_SimScript");
	WChar	*oldParsFilePath = GetDSAMPtr_Common()->parsFilePath;
	DatumPtr	pc;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (localSimScriptPtr->parFilePathMode ==
	  UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL) {
		DSAM_strncpy(localSimScriptPtr->parsFilePath, localSimScriptPtr->
		  parFilePathModeList[UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL].name,
		  MAX_FILE_PATH);
	}
	for (pc = localSimScriptPtr->simulation; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS) {
			pc->data->module->onFlag = pc->onFlag;
			switch (pc->data->module->specifier) {
			case SIMSCRIPT_MODULE:
				SET_PARS_POINTER(pc->data);
				SetOperationMode_Utility_SimScript(BooleanList_NSpecLists(
				  localSimScriptPtr->operationMode)->name);
				SetParFilePathMode_Utility_SimScript(localSimScriptPtr->
				  parFilePathModeList[localSimScriptPtr->parFilePathMode].name);
				SetParsFilePath_Utility_SimScript(localSimScriptPtr->
				  parsFilePath);
				SetSimFileName_Utility_SimScript(pc->u.proc.parFile);
				simScriptPtr->simFileType = localSimScriptPtr->simFileType;
				SetLabelBListPtr_Utility_SimScript(localSimScriptPtr->
				  labelBListPtr);
				SetSimulation_Utility_SimScript(
				  *GetSimScriptSimulation_Utility_SimScript(pc));
				break;
			case ANA_SAI_MODULE:
				SetPar_ModuleMgr(pc->data, wxT("STROBE_PAR_FILE"), NO_FILE);
				break;
			default:
				break;
			}
		}
	if (localSimScriptPtr->simFileType == UTILITY_SIMSCRIPT_SIM_FILE) {
		SetParsFilePath_Common(localSimScriptPtr->parsFilePath);
		if (!InitialiseModules_Utility_Datum(simulation)) {
			NotifyError(wxT("%s: Could not initialise modules."), funcName);
			return(FALSE);
		}
		SetParsFilePath_Common(oldParsFilePath);
	}
	simScriptPtr = localSimScriptPtr;
	return(TRUE);

}

/****************************** Process ***************************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if
 * required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used.
 * The 'localSimScriptPtr' is required because with nested usage, the global
 * simScriptPtr will be changed for other instances of this module (cf LUT:558).
 * The first process must be manually disconnected and connected to the
 * 'data' EarObject's signals.
 */

BOOLN
Process_Utility_SimScript(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_SimScript");
	WChar	*oldParsFilePath = GetDSAMPtr_Common()->parsFilePath;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_SimScript(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Simulation script"));
		TempInputConnection_EarObject(data, GetFirstProcess_Utility_Datum(
		  simScriptPtr->simulation), data->numInSignals);
		if (!CheckParLists_Utility_Datum(localSimScriptPtr->simulation)) {
			NotifyError(wxT("%s: Universal parameter lists check failed."),
			  funcName);
			return(FALSE);
		}
		if (data->updateProcessFlag)
			ResetSimulation_Utility_Datum(localSimScriptPtr->simulation);
		if (localSimScriptPtr->operationMode == GENERAL_BOOLEAN_ON)
			PrintParsModules_Utility_Datum(localSimScriptPtr->simulation);
		SetParsFilePath_Common(localSimScriptPtr->parsFilePath);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	if (!Execute_Utility_Datum(localSimScriptPtr->simulation, NULL, 0)) {
		NotifyError(wxT("%s: Could not execute simulation modules."), funcName);
		SetParsFilePath_Common(oldParsFilePath);
		return(FALSE);
	}
	SetParsFilePath_Common(oldParsFilePath);
	data->outSignal = *GetLastProcess_Utility_Datum(localSimScriptPtr->
	  simulation)->outSignalPtr;
	simScriptPtr = localSimScriptPtr;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
