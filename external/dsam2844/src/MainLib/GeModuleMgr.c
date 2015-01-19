/******************
 *
 * File:		GeModuleMgr.c
 * Purpose:		This module contains the module managment routines.
 * Comments:	These routines carry out various functions as freeing
 *				all module memory space, and so on.
 *				17-04-98 LPO: Added the NULL_MODULE_NAME string constant.
 *				22-04-98 LPO: Changed all the stimulus module names so that they
 *				start with "Stim_".
 *				10-12-98 LPO: Changed the old 'NullFunction_...' to
 *				'TrueFunction_...' which alwaysreturns a TRUE value.  I have
 *				created a new 'NullFunction_...' which always returns the
 *				correct NULL (zero) value.
 *				05-03-99 LPO: The 'SetParsPointer_' routine now checks for a
 *				NULL module passed as an argument.
 *				18-03-99 LPO: I have added the 'LinkGlueRoutine'.  This routine
 *				is not meant to be used, but it is to access global variables
 *				of modules that wouldn't otherwise be linked.
 *				20-04-99 LPO: The "Free_ModuleMgr" routine was not setting the
 *				appropriate module parameters pointer using the
 *				"SetParsPointer_" routine.
 *				27-04-99 LPO:  The universal parameter lists have allowed me
 *				to introduce the 'ReadPar_', 'PrintPar_' and 'RunProcess'
 *				'SetPar_' commands.  These commands replace the previous
 *				generic programming interface.  It will also mean that the
 *				ModuleMgr module can be automatically generated.
 *				10-05-99 LPO: Introduced the 'ReadSimParFile_' routine.  There
 *				is only one module which uses this routine, but since it may
 *				be used quite alot in programs, it is best to put it in.
 *				02-06-99 LPO: In line with my drive to remove the 'DoFun'
 *				interface I have created the 'SetRealPar_', 'GetCFListPtr_' and
 *				'GetSimulation_' routines.  These routines will only work with
 *				modules that have these functions.
 *				22-10-99 LPO: The global 'nullModule' is initialised by the
 *				first call to 'Init_'.  Only one instance is ever initialised.
 * Authors:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		02 Jun 1999
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
#include <string.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtString.h"
#include "UtOptions.h"
#include "UtSimScript.h"
#include "FiParFile.h"

#include "GeModuleMgr.h"
#include "GeModuleReg.h"

/******************************************************************************/
/************************** Global Variables **********************************/
/******************************************************************************/

ModuleRefPtr	moduleList = NULL;
ModulePtr	nullModule = NULL;
BOOLN	(* TestDestroy_ModuleMgr)(void) = NULL;
DSAM_API BOOLN	(* RunProcess_ModuleMgr)(EarObjectPtr) = RunProcessStandard_ModuleMgr;

/******************************************************************************/
/************************** Subroutines and functions *************************/
/******************************************************************************/

/************************** NoFunction ****************************************/

/*
 * This routine is set as the default for all non-assigned functions.
 * It will print a message it an attempt is made to use it.
 */

void *
NoFunction_ModuleMgr(void)
{
	static const WChar *funcName = wxT("NoFunction_ModuleMgr");

	NotifyError(wxT("%s: Attempted to use a function which does not\nexist for ")
	  wxT("module."), funcName );
	return(NULL);

}

/************************** NullFunction **************************************/

/*
 * This routine is set as the NULL default for non-assigned functions.
 * It always returns zero.
 */

void *
NullFunction_ModuleMgr(void)
{
	return(NULL);

}

/************************** TrueFunction **************************************/

/*
 * This routine is set as the default for non-assigned functions.
 * It always returns TRUE.
 */

void *
TrueFunction_ModuleMgr(void)
{

	return((void *) TRUE);

}

/************************** SetDefault ****************************************/

/*
 * This routine sets all of the function pointers to the default function.
 */

void
SetDefault_ModuleMgr(ModulePtr module, void *(* DefaultFunc)(void))
{
	module->CheckData = (BOOLN (*)(EarObjectPtr)) DefaultFunc;
	module->Free = (BOOLN (*)(void)) TrueFunction_ModuleMgr;
	module->GetData = (void * (*)(void *)) DefaultFunc;
	module->GetPotentialResponse = (Float (*)(Float)) DefaultFunc;
	module->GetUniParListPtr = (UniParListPtr (*)(void)) NullFunction_ModuleMgr;
	module->PrintPars = (BOOLN (*)(void)) DefaultFunc;
#	ifdef _PAMASTER1_H
		module->QueueCommand = (BOOLN (*)(void *, int, TypeSpecifier, WChar *,
		  CommandSpecifier, ScopeSpecifier)) DefaultFunc;
		module->SendQueuedCommands = (BOOLN (*)(void)) DefaultFunc;
#	endif
	module->ReadSignal = (BOOLN (*)(WChar *, EarObjectPtr)) DefaultFunc;
	module->ResetProcess = (void (*)(EarObjectPtr)) DefaultFunc;
	module->RunProcess = (BOOLN (*)(EarObjectPtr)) DefaultFunc;
	module->InitModule = (BOOLN (*)(ModulePtr)) DefaultFunc;
	module->SetParsPointer = (BOOLN (*)(ModulePtr)) DefaultFunc;

}

/************************** Init **********************************************/

/*
 * This function initialises and sets a module structure.
 * It allocates the necessary memory for the structure and connects the
 * necessary routines.
 * The stored module name is converted to upper case for (later) comparison
 * purposes.
 * This routine first checks if the global 'nullModule' has been initialised,
 * and initialises it if necessary.
 */

DSAM_API ModulePtr
Init_ModuleMgr(const WChar *theModuleName)
{
	static const WChar *funcName = wxT("Init_ModuleMgr");
	static ModuleHandle handleNumber = 0;
	ModulePtr	theModule;
	ModRegEntryPtr	modRegEntryPtr;

	if ((modRegEntryPtr = GetRegEntry_ModuleReg(theModuleName)) == NULL) {
		NotifyError(wxT("%s: Unknown module '%s'."), funcName, theModuleName);
		return(NULL);
	}
	if (modRegEntryPtr->specifier == NULL_MODULE) {
		if (nullModule)
			return(nullModule);
	} else {
		if (!nullModule)
			nullModule = Init_ModuleMgr(wxT("NULL"));
	}

	if ((theModule = (ModulePtr) (malloc(sizeof (Module)))) == NULL) {
		NotifyError(wxT("%s: Could not allocate memory."), funcName);
		return(NULL);
	}
	theModule->specifier = modRegEntryPtr->specifier;
	theModule->classSpecifier = modRegEntryPtr->classSpecifier;
	theModule->onFlag = TRUE;
	theModule->threadMode = MODULE_THREAD_MODE_NONE;
	theModule->parsPtr = NULL;
	ToUpper_Utility_String(theModule->name, theModuleName);
	SetDefault_ModuleMgr(theModule, NoFunction_ModuleMgr);
	(* modRegEntryPtr->InitModule)(theModule);
	theModule->handle = handleNumber++;		/* Unique handle for each module. */
	if (!AddModuleRef_ModuleMgr(&moduleList, theModule)) {
		NotifyError(wxT("%s: Could not register new module."), funcName);
		Free_ModuleMgr(&theModule);
		return(NULL);
	}
	return(theModule);

}

/**************************** CreateModuleRef *********************************/

/*
 * This function returns a pointer to a module node.
 * It returns NULL if it fails.
 */

ModuleRefPtr
CreateModuleRef_ModuleMgr(ModulePtr theModule)
{
	static const WChar *funcName = wxT("CreateModuleRef_ModuleMgr");
	ModuleRef	*newNode;

	if ((newNode = (ModuleRef *) malloc(sizeof (ModuleRef))) == NULL) {
		NotifyError(wxT("%s: Out of memory."), funcName);
		return(NULL);
	}
	newNode->module = theModule;
	newNode->next = NULL;
	return(newNode);

}

/**************************** AddModuleRef ************************************/

/*
 * This routine adds a module reference to a list.
 * It places the module handles in ascending order.
 */

BOOLN
AddModuleRef_ModuleMgr(ModuleRefPtr *theList, ModulePtr theModule)
{
	static const WChar *funcName = wxT("AddModuleRef_ModuleMgr");
	ModuleRefPtr	temp, p;

    if (*theList == NULL) {
    	if ((*theList = CreateModuleRef_ModuleMgr(theModule)) == NULL) {
    		NotifyError(wxT("%s: Could not create new node."), funcName);
    		return(FALSE);
    	}
		return(TRUE);
	}
	for (p = *theList; (p->next != NULL) && (p->module->handle <
	  theModule->handle); p = p->next)
		;
	if (p->module->handle == theModule->handle) {
		NotifyWarning(wxT("%s: Module already in list."), funcName);
		return(TRUE);
	}
	if ((temp = CreateModuleRef_ModuleMgr(theModule)) == NULL) {
		NotifyError(wxT("%s: Could not create temp node."), funcName);
		return(FALSE);
	}
	if (p->module->handle > theModule->handle) {
		temp->module = theModule;
		temp->next = p;
		if (p == *theList)	/* Test for head of list. */
			*theList = temp;
	} else
		p->next = temp;
	return(TRUE);

}

/**************************** FreeModuleRef ***********************************/

/*
 * This routine removes a ModuleRef from a list.
 */

void
FreeModuleRef_ModuleMgr(ModuleRefPtr *theList, ModuleHandle theHandle)
{
	ModuleRefPtr	p, lastRef;

    if (*theList == NULL)
    	return;
	for (p = lastRef = *theList; (p != NULL) && (p->module->handle !=
	  theHandle); p = p->next)
		lastRef = p;
	if (p->module->handle == theHandle) {
		if (p == *theList)	/* Test for head of list. */
			*theList = p->next;
		else
			lastRef->next = p->next;
		free(p);
	}

}

/************************** Free **********************************************/

/*
 * This routine frees the memory, allocated for module structures.
 * It calls the explicit module freeing routine.  The default NullFunction is
 * used if another has not been assigned.
 * It also unregisters the module from the main list.
 * This routine will not delete the global 'nullModule'.
 */

DSAM_API void
Free_ModuleMgr(ModulePtr *theModule)
{
	if ((*theModule == NULL) || (*theModule == nullModule))
		return;
	(* (*theModule)->SetParsPointer)(*theModule);
	FreeModuleRef_ModuleMgr(&moduleList, (*theModule)->handle);
	(* (*theModule)->Free)();
	free(*theModule);
	*theModule = NULL;

}

/**************************** FreeAll *****************************************/

/*
 * This routine frees the memory for all of the initialised modules from
 * the main register list.
 * It does not, however, set the pointer variables to NULL - watch this.
 */

void
FreeAll_ModuleMgr(void)
{
	while (moduleList != NULL)
		FreeModuleRef_ModuleMgr(&moduleList, moduleList->module->handle);

}

/************************** RunModel...Null ***********************************/

/*
 * This function connects the input signal to the output signal, without
 * doing any processing.
 * If the 'data->module->onFlag' is set to FALSE, this means a module has been
 * disabled, and any previous data needs to be Free'd.
 */

BOOLN
RunModel_ModuleMgr_Null(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_ModuleMgr_Null");

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, NULL_MODULE_PROCESS_NAME);
		if (!data->inSignal || !CheckPars_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not set correctly."), funcName);
			return(FALSE);
		}
		if (!data->module->onFlag)
			FreeOutSignal_EarObject(data);
		data->localOutSignalFlag = FALSE;
		_OutSig_EarObject(data) = _InSig_EarObject(data, 0);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
 	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

/*************************** LinkGlueRoutine **********************************/

/*
 * This routine has references to modules which are not properly linked
 * otherwise.
 */

void
LinkGlueRoutine_ModuleMgr(void)
{
	optionsPleaseLink++;

}

/*************************** CheckData ****************************************/

/*
 * This function checks the correct initialisation of a data object.
 * It returns FALSE if it fails in any way.
 */

BOOLN
CheckData_ModuleMgr(EarObjectPtr data, const WChar *callingFunction)
{
	static const WChar *funcName = wxT("CheckData_ModuleMgr");

	if (data == NULL) {
		NotifyError(wxT("%s: %s: Process EarObject not initialised."), funcName,
		  callingFunction);
		return(FALSE);
	}
	if (data->module == NULL) {
		NotifyError(wxT("%s: %s: Process EarObject  module not initialised."),
		  funcName, callingFunction);
		return(FALSE);
	}
	return(TRUE);
}

/**************************** Enable ******************************************/

/*
 * This routine sets the onFlag for an EarObject structure.
 * On re-enabling a process the _OutSig_EarObject(data) pointer must be set to NULL
 * because it will have been set manually by the 'RunModel_ModuleMgr_NulL'
 * routine.
 */

DSAM_API BOOLN
Enable_ModuleMgr(EarObjectPtr data, BOOLN on)
{
	static const WChar *funcName = wxT("Enable_ModuleMgr");

	if (!data) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!data->module) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}

	if (on) {
		data->module->onFlag = TRUE;
		_OutSig_EarObject(data) = NULL;
	} else {
		data->module->onFlag = FALSE;
		FreeOutSignal_EarObject(data);
	}
	return(TRUE);

}

/*************************** GetData ******************************************/

/*
 * This function returns general data from a model, as defined by the module's
 * specification.
 */

void *
GetData_ModuleMgr(EarObjectPtr data, void *inArg)
{
	static const WChar *funcName = wxT("GetData_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	SET_PARS_POINTER(data);
	return((* data->module->GetData)(inArg));

}

/*************************** GetSimulation ************************************/

/*
 * This function returns the simulation pointer from a module.
 * It only works for routines that can return a simulation.
 */

DSAM_API DatumPtr
GetSimulation_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GetSimulation_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	SET_PARS_POINTER(data);
	return((data->module->specifier != SIMSCRIPT_MODULE)? NULL: ((SimScriptPtr)
	  data->module->parsPtr)->simulation);

}

/*************************** GetSimPtr ****************************************/

/*
 * This function returns the simulation pointer pointer from a module.
 * It only works for routines that can return a simulation.
 */

DSAM_API DatumPtr *
GetSimPtr_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GetSimPtr_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	SET_PARS_POINTER(data);
	return((data->module->specifier != SIMSCRIPT_MODULE)? NULL: ((SimScriptPtr)
	  data->module->parsPtr)->simPtr);

}

/*************************** GetProcess ***************************************/

/*
 * This function returns pointer to specified process in a simulation, or just
 * the EarObject's pointer otherwise.
 */

EarObjectPtr
GetProcess_ModuleMgr(EarObjectPtr data, WChar *processSpecifier)
{
	static const WChar *funcName = wxT("GetProcess_ModuleMgr");
	EarObjectPtr	process;

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	if (data->module->specifier != SIMSCRIPT_MODULE)
		return(data);
	SET_PARS_POINTER(data);
	if ((process = FindProcess_Utility_Datum(((SimScriptPtr) data->module->
	  parsPtr)->simulation, processSpecifier)) != NULL)
		return(process);

	NotifyError(wxT("%s: Could not find process."), funcName);
	return(NULL);

}

/*************************** GetLabelledProcess *******************************/

/*
 * This function returns pointer to labelled process in a simulation.
 */

EarObjectPtr
GetLabelledProcess_ModuleMgr(EarObjectPtr data, WChar *label)
{
	static const WChar *funcName = wxT("GetLabelledProcess_ModuleMgr");
	EarObjectPtr	process;

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	if (data->module->specifier != SIMSCRIPT_MODULE)
		return(NULL);
	SET_PARS_POINTER(data);
	if ((process = FindLabelledProcess_Utility_Datum(((SimScriptPtr) data->
	  module->parsPtr)->simulation, label)) != NULL)
		return(process);

	NotifyError(wxT("%s: Could not find process."), funcName);
	return(NULL);

}

/*************************** GetUniParPtr *************************************/

/*
 * This function returns the specified parameter pointer from a module
 */

DSAM_API UniParPtr
GetUniParPtr_ModuleMgr(EarObjectPtr data, const WChar *parName)
{
	static const WChar *funcName = wxT("GetUniParPtr_ModuleMgr");
	UniParPtr	par;
	UniParListPtr	parList;

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	SET_PARS_POINTER(data);
	parList = (* data->module->GetUniParListPtr)();
	if ((par = FindUniPar_UniParMgr(&parList, parName, UNIPAR_SEARCH_ABBR)) ==
	  NULL) {
		NotifyError(wxT("%s: Could not find parameter '%s' for process '%s'"),
		  funcName, parName, data->module->name);
		return(NULL);
	}
	return(par);

}

/*************************** GetUniParListPtr *********************************/

/*
 * This function returns the specified parameter list pointer from a module.
 */

DSAM_API UniParListPtr
GetUniParListPtr_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GetUniParListPtr_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	SET_PARS_POINTER(data);
	return((* data->module->GetUniParListPtr)());

}

/*************************** PrintPars ****************************************/

/*
 * This function prints the parameters for a module.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
PrintPars_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("PrintPars_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	SET_PARS_POINTER(data);
	return((* data->module->PrintPars)());

}

/*************************** ReadPars *****************************************/

/*
 * This function reads the parameters for a module.
 * An attempt is made to use the new parameter file format first.  If that fails
 * then the old parameter file format is used, if it exists.
 * The test for the new parameter file format is done by assuming that the first
 * 'parName' parameter read is a valid parameter name.  If it fails to find
 * it then the it is assumed that the parameter file uses the old format.
 * For the first parameter test, all error messages are turned off.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
ReadPars_ModuleMgr(EarObjectPtr data, WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_ModuleMgr");
	BOOLN	ok = TRUE;
	WChar	*filePath, parName[MAXLINE], parValue[MAX_FILE_PATH];
	WChar	failedParName[MAXLINE] = wxT("");
	int		parCount = 0;
	FILE	*fp;
	UniParPtr	par;
	UniParListPtr	parList, tempParList;

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	SET_PARS_POINTER(data);
	if (data->module->specifier == SIMSCRIPT_MODULE)
		return(ReadPars_Utility_SimScript(fileName));
	parList = (* data->module->GetUniParListPtr)();
	if ((DSAM_strcmp(fileName, NO_FILE) == 0) || !parList)
		return(TRUE);
	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open parameter file '%s'.\n"), funcName,
		  fileName);
		return(FALSE);
	}
	Init_ParFile();
	SetEmptyLineMessage_ParFile(FALSE);
	while (GetPars_ParFile(fp, wxT("%s %s"), parName, parValue)) {
		parCount++;
		tempParList = parList;
		if ((par = FindUniPar_UniParMgr(&tempParList, parName, UNIPAR_SEARCH_ABBR)) == NULL) {
			NotifyError(wxT("%s: Unknown parameter '%s' for module '%s'."),
			  funcName, parName, data->module->name);
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
		if (*failedParName)
			NotifyError(wxT("%s: Unknown parameter '%s' for module '%s'."),
			  funcName, failedParName, data->module->name);
		NotifyError(wxT("%s: Invalid parameters, in %s module parameter ")
		  wxT("file '%s'."), funcName, data->module->name, fileName);
		return(FALSE);
	}
	return(TRUE);

}

/*************************** PrintSimParFile **********************************/

/*
 * This function prints the parameters for the simulation module.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
PrintSimParFile_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("PrintSimParFile_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	SET_PARS_POINTER(data);
	if (data->module->specifier != SIMSCRIPT_MODULE) {
		NotifyError(wxT("%s: This function can only be used with a ")
		  wxT("simulation."), funcName);
		return(FALSE);
	}
	return(PrintSimParFile_Utility_SimScript());

}

/*************************** WriteSimParFile **********************************/

/*
 * This function prints a simulation as a wxT(".spf") file.
 * It returns FALSE if it fails in any way.
 */

BOOLN
WriteSimParFile_ModuleMgr(WChar *fileName, EarObjectPtr data)
{
	/*static const WChar *funcName = wxT("WriteSimParFile_ModuleMgr");*/
	BOOLN	ok = TRUE;
	FILE *oldFp = GetDSAMPtr_Common()->parsFile;

	SetParsFile_Common(fileName, OVERWRITE);
	ok = PrintSimParFile_ModuleMgr(data);
	fclose(GetDSAMPtr_Common()->parsFile);
	GetDSAMPtr_Common()->parsFile = oldFp;
	return(ok);

}

/*************************** WritePars ****************************************/

/*
 * This function writes the parameters for a process.  If the process is a
 * simulation then it will write an SPF file.
 * The base file name is given, and the extension is added as appropriate.
 * If the base file name already has an extension, then that is used.
 */

DSAM_API BOOLN
WritePars_ModuleMgr(WChar *baseFileName, EarObjectPtr process)
{
	BOOLN	ok = TRUE, addExtension;
	WChar	filePath[MAX_FILE_PATH];

	DSAM_strcpy(filePath, GetParsFileFPath_Common(baseFileName));
	addExtension = (!DSAM_strrchr(filePath, '.'));
	if (process->module->specifier != SIMSCRIPT_MODULE) {
		if (addExtension)
			DSAM_strcat(filePath, wxT(".par"));
		ok = WriteParFile_UniParMgr(filePath, GetUniParListPtr_ModuleMgr(
		  process));
	} else {
		if (addExtension)
			DSAM_strcat(filePath, wxT(".spf"));
		ok = WriteSimParFile_ModuleMgr(filePath, process);
	}
	return(ok);

}


/*************************** GetSimParFileFlag ********************************/

/*
 * This function returns the SimParFileFlag for a simulation script.
 * It also returns FALSE if it fails, but in this case it prints an error
 * messages.
 * It is not being called in places where it will fail.
 */

int
GetSimFileType_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GetSimFileType_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	if (data->module->specifier != SIMSCRIPT_MODULE) {
		NotifyError(wxT("%s: This function can only be used with a ")
		  wxT("simulation."), funcName);
		return(FALSE);
	}
	return((int) ((SimScriptPtr) data->module->parsPtr)->simFileType);

}

/*************************** GetParsFilePath **********************************/

/*
 * This function returns the parsFilePath for a simulation script.
 * It also returns NULL if it fails, but in this case it prints an error
 * messages.
 * It is not being called in places where it will fail.
 */

WChar *
GetParsFilePath_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GetParsFilePath_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	if (data->module->specifier != SIMSCRIPT_MODULE) {
		NotifyError(wxT("%s: This function can only be used with a ")
		  wxT("simulation."), funcName);
		return(NULL);
	}
	return(((SimScriptPtr) data->module->parsPtr)->parsFilePath);

}

/*************************** ResetProcess *************************************/

/*
 * This function runs the module process reset routine
 */

DSAM_API BOOLN
ResetProcess_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("ResetProcess_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);

	SET_PARS_POINTER(data);
	data->updateProcessFlag = TRUE;
	(* data->module->ResetProcess)(data);
	return(TRUE);

}

/*************************** RunProcessStandard *******************************/

/*
 * This function runs the module process
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
RunProcessStandard_ModuleMgr(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunProcessStandard_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);

	if (GetDSAMPtr_Common()->interruptRequestedFlag)
		return(FALSE);

	if (data->module->onFlag) {
		SET_PARS_POINTER(data);
		return((* data->module->RunProcess)(data));
	} else {
		(* nullModule->SetParsPointer)(nullModule);
		return((* nullModule->RunProcess)(data));
	}

}

/*************************** SetPar *******************************************/

/*
 * This function sets the specified value of a universal parameter list for a
 * module.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
SetPar_ModuleMgr(EarObjectPtr data, const WChar *parName, const WChar *value)
{
	static const WChar *funcName = wxT("SetPar_ModuleMgr");

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	SET_PARS_POINTER(data);
	switch (data->module->specifier) {
	case SIMSCRIPT_MODULE:
		if (SetControlParValue_Utility_SimScript(parName, value, FALSE))
			return(TRUE);
		if (SetSimUniParValue_Utility_SimScript(parName, value))
			return(TRUE);
		NotifyError(wxT("%s: Could not find parameter '%s' for process '%s'"),
		  funcName, parName, data->module->name);
		return(FALSE);
	default: {
		UniParPtr	par;
		UniParListPtr	parList = (* data->module->GetUniParListPtr)();
		if ((par = FindUniPar_UniParMgr(&parList, parName,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			NotifyError(wxT("%s: Could not find parameter '%s' for process ")
			  wxT("'%s'"), funcName, parName, data->module->name);
			return(FALSE);
		}
		return(SetParValue_UniParMgr(&parList, par->index, value));
		}
	}

}

/*************************** SetRealPar ***************************************/

/*
 * This function calls the converts a real value to a string then calls the
 * 'SetPar_' routine.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealPar_ModuleMgr(EarObjectPtr data, const WChar *name, Float value)
{
	WChar	stringValue[MAXLINE];

	DSAM_snprintf(stringValue, MAXLINE, wxT("%g"), value);
	return(SetPar_ModuleMgr(data, name, stringValue));

}

/*************************** SetRealArrayPar **********************************/

/*
 * This function calls the converts a real value to a string with its respective
 * index value then calls the 'SetPar_' routine.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealArrayPar_ModuleMgr(EarObjectPtr data, const WChar *name, int index,
  Float value)
{
	/* static const WChar *funcName = wxT("SetRealPar_ModuleMgr"); */
	WChar	arrayElementString[MAXLINE];

	DSAM_snprintf(arrayElementString, MAXLINE, wxT("%d:%g"), index, value);
	return(SetPar_ModuleMgr(data, name, arrayElementString));

}

/************************** SetTestDestory ************************************/

/*
 * This routine sets the global 'ModuleTestDestoryFunc' function pointer.
 */

DSAM_API void
SetTestDestroy_ModuleMgr(BOOLN (* Func)(void))
{
	TestDestroy_ModuleMgr = Func;

}


/************************** SetRunProcess *************************************/

/*
 * This routine sets the global 'RunProcess' function pointer.
 */

DSAM_API void
SetRunProcess_ModuleMgr(BOOLN (* Func)(EarObjectPtr))
{
	RunProcess_ModuleMgr = Func;

}


/************************** SetNull *******************************************/

/*
 * This routine sets the null module.
 */

BOOLN
SetNull_ModuleMgr(ModulePtr theModule)
{
	theModule->specifier = NULL_MODULE;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->RunProcess = RunModel_ModuleMgr_Null;
	return(TRUE);

}

/************************** FreeNull ******************************************/

/*
 * This routine frees the null module.
 * It first copies the pointer to temporary pointer so that the conditional
 * statement in the Free_ModuleMgr routine will actually work.
 */

DSAM_API void
FreeNull_ModuleMgr(void)
{
	ModulePtr	tempPtr;

	if (!nullModule)
		return;
	tempPtr = nullModule;
	nullModule = NULL;
	Free_ModuleMgr(&tempPtr);

}

/**************************** InLineProcess ***********************************/

/*
 * This function checks whether this module is being used in-line within an
 * EarObject pipline.
 * This is checked by ensuring that the calling module's "RunProcess" function.
 * is the same as the process module's "RunProcess" function.
 */

BOOLN
InLineProcess_ModuleMgr(EarObjectPtr data, BOOLN (* RunProcess)(
  EarObjectPtr theObject))
{

	if (data->module && (data->module->RunProcess == RunProcess)) {
		return(TRUE);
	}
	return(FALSE);

}

