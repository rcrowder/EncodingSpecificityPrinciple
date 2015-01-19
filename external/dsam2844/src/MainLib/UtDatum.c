/**********************
 *
 * File:		UtDatum.c
 * Purpose:		This module contains the process control, "datum" management
 *				code.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				11-03-97 LPO: This code has been amended to allow the numbers
 *				of inputs and outputs for an EarObject to be specified.
 *				The number of inputs defines how many of the previous
 *				EarObjects should be connected to a particular EarObject.
 *				The number of outputs defines how many of the following
 *				EarObjects should be connected accordingly.
 *				21-08-97 LPO: Removed rampInterval parameter - use new ramp
 *				module.
 *				19-11-98 LPO: Corrected the "General parameter list' bug, that
 *				meant that the 'array_index' general parameter was not being
 *				found for a particular process.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for
 *				modules with no parameters.
 *				27-01-99 LPO: The static 'stepCount' in the 'InstallInst_'
 *				routine was not being reset to zero when installing 'datum'
 *				nodes at the head of the list.  This occurred with the
 *				GUI mode, where simulations needed to be recreated.
 *				02-02-99 LPO: The 'FindModuleUniPar_' routine now also returns
 *				the simulation 'DatumPtr' at which the universal parameter was
 *				found.
 *				The 'SetUniParValue_' routine has been moved here from the
 *				UtSimScript module, as it needs to use the
 *				'SetParsPointer_ModuleMgr' routine.
 *				05-05-99 LPO: The 'InitialiseModules_' routine now sets the
 *				display signal's window title using the 'stepNumber' field, if
 *				it has not already been set.
 *				24-08-99 LPO: Implemented nicer output for the
 *				'PrintInstructions_" routine.
 *				07-09-99 LPO: Fixed major problem with the FindModuleUniPar
 *				routine: It was replacing the simScript simulation field when
 *				operating on sub-simulations.
 * Author:		L. P. O'Mard
 * Created:		01 Jul 1996
 * Updated:		08 Sep 1999
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
#include <string.h>
#include <ctype.h>

#if DEBUG
#	include <time.h>
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeNSpecLists.h"
#include "GeModuleMgr.h"

#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtSSParser.h"
#include "UtString.h"

#include "UtDatum.h"

/*#define DEBUG	1*/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

uInt	datumStepCount = 0;
DatumPtr	(* Execute_Utility_Datum)(DatumPtr, DatumPtr, int) =
			  ExecuteStandard_Utility_Datum;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/


/****************************** GetLastInst ***********************************/

/*
 * This routine returns a pointer to the last Datum instruction in a
 * simulation instruction list.
 */

DatumPtr
GetLastInst_Utility_Datum(DatumPtr head)
{
	DatumPtr	pc;

	if (head == NULL)
		return(NULL);
	for (pc = head; pc->next != NULL; pc = pc->next)
		;
	return(pc);

}

/****************************** InitInst **************************************/

/*
 * Initialises a datum instruction. The space is allocated and the basic
 * settings assigned.
 */

DSAM_API DatumPtr
InitInst_Utility_Datum(int type)
{
	static const WChar *funcName = wxT("InitInst_Utility_Datum");
	DatumPtr	datum;

	if ((datum = (DatumPtr) malloc(sizeof (Datum))) == NULL) {
		NotifyError(wxT("%s: Out of memory for Datum."), funcName);
		return(NULL);
	}
	datum->onFlag = TRUE;
	datum->threadSafe = TRUE;
	datum->type = type;
	datum->label = NULL;
	datum->classSpecifier = -1;
	switch (type) {
	case PROCESS:
		datum->u.proc.parFile = NULL_STRING;
		datum->u.proc.moduleName = NULL_STRING;
		datum->u.proc.inputList = NULL;
		datum->u.proc.outputList = NULL;
		break;
	case RESET:
		datum->u.ref.string = NULL_STRING;
		datum->classSpecifier = CONTROL_MODULE_CLASS;
		break;
	case REPEAT:
		datum->u.loop.count = 1;
		datum->u.loop.stopPC = NULL;
		datum->classSpecifier = CONTROL_MODULE_CLASS;
		break;
	case STOP:
		datum->classSpecifier = CONTROL_MODULE_CLASS;
		break;
	default:
		;
	} /* Switch */
	datum->data = NULL;
	datum->shapePtr = NULL;
	datum->next = NULL;
	datum->previous = NULL;
	datum->passedThreadEnd = NULL;
	datum->stepNumber = datumStepCount++;
	return datum;

}

/****************************** InstallInst ***********************************/

/*
 * Install a code datum instruction in table.
 * Datum instructions are added to the end of the list.
 */

DSAM_API DatumPtr
InstallInst_Utility_Datum(DatumPtr *head, int type)
{
	static const WChar *funcName = wxT("InstallInst_Utility_Datum");
	DatumPtr	datum, pc;

	if ((datum = InitInst_Utility_Datum(type)) == NULL) {
		NotifyError(wxT("%s: Could not initialise datum instruction."),
		  funcName);
		return(NULL);
	}
	if ((pc = GetLastInst_Utility_Datum(*head)) == NULL) {
		*head = datum;
		datum->previous = NULL;
	} else {
		pc->next = datum;
		datum->previous = pc;
	}
	return datum;

}

/****************************** GetChainStart *********************************/

/*
 * This routine finds the start of a datum chain.
 * It expects the argument, "pc" to be correctly initialised.
 */

DatumPtr
GetChainStart_Utility_Datum(DatumPtr pc)
{
	while (pc->previous)
		pc = pc->previous;
	return(pc);

}

/****************************** InsertsInst ***********************************/

/*
 * Insert a code datum instruction into a list
 * It expects the datum to have been already created elsewhere.
 * If the datum instruction already has a, "previous" pointer set, then the
 * 'pos' datum will be placed before the 'datum' datum instead.
 */

DSAM_API BOOLN
InsertInst_Utility_Datum(DatumPtr *head, DatumPtr pos, DatumPtr datum)
{
	if (!*head)	/* Start of simulation list */
		*head = pos;
	else if (datum == *head)
		*head = GetChainStart_Utility_Datum(pos);
	if (datum->previous) {
		datum->previous->next = pos;
		pos->previous = datum->previous;
	}
	datum->previous = pos;
	pos->next = datum;
	return(TRUE);

}

/****************************** RemoveConnection ******************************/

/*
 * Removes a connection label from a list.
 */

void
RemoveConnection_Utility_Datum(DynaListPtr *list, WChar *label)
{
	static const WChar	*funcName = wxT("RemoveConnection_Utility_Datum");
	DynaListPtr	p;

	if (!*list)
		return;
	if ((p = FindElement_Utility_DynaList(*list, CmpLabel_Utility_Datum,
	  label)) == NULL) {
		NotifyError(wxT("%s: Could not find label '%s' in list."), funcName,
		  label);
		return;
	}
	Remove_Utility_DynaList(list, p);

}

/****************************** DisconnectInst ********************************/

/*
 * Disconnect datum instructions in a list.
 * Bear in mind that when disconnecting two processes, there may be processes
 * in between, i.e. multiply connected processes.  They need to be between for
 * process run flow.
 */

DSAM_API void
DisconnectInst_Utility_Datum(DatumPtr *head, DatumPtr from, DatumPtr to)
{
	if ((from->type == PROCESS) && (to->type == PROCESS)) {
		RemoveConnection_Utility_Datum(&from->u.proc.outputList, to->label);
		RemoveConnection_Utility_Datum(&to->u.proc.inputList, from->label);
	}
	if (from->next != to)
		to = from->next;
	from->next = NULL;
	to->previous = NULL;
	if ((*head == from) && !from->previous)
		*head = (to->next)? to: NULL;
	else if ((*head == to) && !to->next)
		*head = NULL;

}

/****************************** ConnectInst ***********************************/

/*
 * Connect datum instructions in a list.
 */

DSAM_API BOOLN
ConnectInst_Utility_Datum(DatumPtr *head, DatumPtr from, DatumPtr to)
{
	static const WChar *funcName = wxT("ConnectInst_Utility_Datum");

	if (!InsertInst_Utility_Datum(head, from, to)) {
		NotifyError(wxT("%s: Could not add process to chain."),
		  funcName);
		return(FALSE);
	}
	if ((from->type == PROCESS) && (to->type == PROCESS)) {
		Append_Utility_DynaList(&from->u.proc.outputList, to->label);
		Append_Utility_DynaList(&to->u.proc.inputList, from->label);
	}
	return(TRUE);

}

/****************************** FreeInstruction *******************************/

/*
 * This routine frees a single Datum instructions.
 * It does not manage the simulation connections.
 * Note that all of the strings where initialised to 'NULL_STRING', but if
 * they were set, then the string was allocated space.
 */

DSAM_API void
FreeInstruction_Utility_Datum(DatumPtr *pc)
{
	if (!*pc)
		return;

	if ((*pc)->label)
		free((*pc)->label);
	switch ((*pc)->type) {
	case PROCESS:
		if (*(*pc)->u.proc.parFile != '\0')
			free((*pc)->u.proc.parFile);
		if (*(*pc)->u.proc.moduleName != '\0')
			free((*pc)->u.proc.moduleName);
		if ((*pc)->u.proc.inputList)
			FreeList_Utility_DynaList(&(*pc)->u.proc.inputList);
		if ((*pc)->u.proc.outputList)
			FreeList_Utility_DynaList(&(*pc)->u.proc.outputList);
		Free_EarObject(&(*pc)->data);
		break;
	case RESET:
		if (*(*pc)->u.ref.string != '\0')
			free((*pc)->u.ref.string);
		break;
	case REPEAT:
		break;
	default:
		;
	} /* switch */
	free((*pc));
	*pc = NULL;

}

/****************************** FreeInstFromSim *******************************/

/*
 * This routine frees a single Datum instruction from the simulation.
 * Note that all of the strings where initialised to 'NULL_STRING', but if
 * they were set, then the string was allocated space.
 */

DSAM_API void
FreeInstFromSim_Utility_Datum(DatumPtr *start, DatumPtr pc)
{
	if (!pc)
		return;

	if (!pc->previous) {
		*start = pc->next;
		if (*start)
			(*start)->previous = NULL;
	} else
		pc->previous->next = pc->next;
	FreeInstruction_Utility_Datum(&pc);

}

/****************************** FreeInstructions ******************************/

/*
 * This routine frees the Datum instructions in the simulation.
 */

void
FreeInstructions_Utility_Datum(DatumPtr *pc)
{
	while (*pc)
		FreeInstFromSim_Utility_Datum(pc, *pc);

}

/****************************** PrintIndentAndLabel ***************************/

/*
 * This routine prints a specified number of tabs, with the label at the
 * previous tab level.
 */

void
PrintIndentAndLabel_Utility_Datum(DatumPtr pc, int indentLevel)
{
	int		i;

	for (i = 0; i < indentLevel; i++)
		DPrint(wxT("\t"));
	if (((pc->type == PROCESS) || (pc->type == REPEAT)) && pc->label &&
	  (pc->label[0] != '\0'))
		DPrint(wxT("%s%%"), pc->label);
	DPrint(wxT("\t"));

}

/****************************** PrintConnections ******************************/

/*
 * This routine prints the connection labels in a dynamic array list.
 */

void
PrintConnections_Utility_Datum(DynaListPtr list)
{
	DynaListPtr	node;

	for (node = list; node != NULL; node = node->next) {
		if (node != list)
			DPrint(wxT(", "));
		DPrint(wxT("%s"), (WChar *) node->data);
	}

}

/****************************** GetProcessName ********************************/

/*
 * This routine returns the process name.
 */

DSAM_API WChar *
GetProcessName_Utility_Datum(DatumPtr pc)
{
	static const WChar *funcName = wxT("GetProcessName_Utility_Datum");
	static WChar		string[MAXLINE];

	if (!pc) {
		NotifyError(wxT("%s: Null process pointer given."), funcName);
		return(NULL);
	}
	switch (pc->type) {
	case PROCESS:
		return(pc->u.proc.moduleName);
	case REPEAT:
		return(wxT("repeat"));
	case RESET:
		return(wxT("reset"));
	default:
		Snprintf_Utility_String(string, MAXLINE, wxT("default %d"), pc->type);
		return(string);
	} /* switch */

}

/****************************** PrintSimScript ********************************/

/*
 * This routine prints the supplied simulation script simulation script.
 * It returns 'TRUE' if there are subsimulation scripts to print.
 */

BOOLN
PrintSimScript_Utility_Datum(DatumPtr pc, WChar *scriptName, int indentLevel,
  WChar *prefix, BOOLN checkForSubSimScripts)
{
	BOOLN	subSimScripts = FALSE;
	WChar	*p, *scriptNameNoPath;
	int		i;

	if (pc == NULL)
		return(subSimScripts);
	if ((p = DSAM_strrchr(scriptName, '/')) != NULL)
		scriptNameNoPath = p + 1;
	else if ((p = DSAM_strrchr(scriptName, '\\')) != NULL)
		scriptNameNoPath = p + 1;
	else
		scriptNameNoPath = scriptName;
	DPrint(wxT("%sbegin %s {\n\n"), prefix, scriptNameNoPath);
	for ( ; pc != NULL; pc = pc->next) {
		DPrint(wxT("%s"), prefix);
		switch (pc->type) {
		case PROCESS:
			if (pc->data->module->specifier == SIMSCRIPT_MODULE)
				subSimScripts = TRUE;
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			if (!pc->data->module->onFlag)
				DPrint(wxT("%c "), SIMSCRIPT_DISABLED_MODULE_CHAR);
			DPrint(wxT("%-16s"), pc->u.proc.moduleName);
			if (pc->u.proc.inputList || pc->u.proc.outputList) {
				DPrint(wxT("("));
				PrintConnections_Utility_Datum(pc->u.proc.inputList);
				DPrint(wxT("->"));
				PrintConnections_Utility_Datum(pc->u.proc.outputList);
				DPrint(wxT(")"));
			}
			if ((*pc->u.proc.parFile == '\0') || DSAM_strcmp(pc->u.proc.parFile,
			  NO_FILE) == 0)
				DPrint(wxT("\n"));
			else if (checkForSubSimScripts && (pc->data->module->specifier ==
			  SIMSCRIPT_MODULE))
				DPrint(wxT("\t< (%s)\n"),  pc->u.proc.parFile);
			else
				DPrint(wxT("\t< %s\n"),  pc->u.proc.parFile);
			break;
		case REPEAT:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel++);
			DPrint(wxT("%s %d {\n"), GetProcessName_Utility_Datum(pc), pc->u.
			  loop.count);
			break;
		case RESET:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			DPrint(wxT("%s\t%s\n"), GetProcessName_Utility_Datum(pc),
			  pc->u.ref.string);
			break;
		case STOP:
			PrintIndentAndLabel_Utility_Datum(pc, --indentLevel);
			DPrint(wxT("}\n"));
			break;
		default:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			DPrint(wxT("%s\tvar = %d\n"), GetProcessName_Utility_Datum(pc), pc->
			  u.loop.count);
			break;
		} /* switch */
	}
	for (i = 0; i < indentLevel; i++)
		DPrint(wxT("\t"));
	DPrint(wxT("\n%s}\n"), prefix);
	return(subSimScripts);

}

/****************************** PrintInstructions *****************************/

/*
 * This routine prints the simulation process instructions.
 */

void
PrintInstructions_Utility_Datum(DatumPtr pc, WChar *scriptName, int indentLevel,
  WChar *prefix)
{
	BOOLN	subSimScripts = FALSE;
	DatumPtr	start = pc;

	if (pc == NULL)
		return;
	subSimScripts = PrintSimScript_Utility_Datum(pc, scriptName, indentLevel,
	  prefix, TRUE);
	if (!subSimScripts)
		return;
	DPrint(wxT("%s#Sub-simulation scripts\n"), prefix);
	for (pc = start ; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier ==
		  SIMSCRIPT_MODULE)) {
			DPrint(wxT("%s"), prefix);
			PrintInstructions_Utility_Datum(((SimScriptPtr)
			  pc->data->module->parsPtr)->simulation, pc->u.proc.parFile,
			  indentLevel + 1, prefix);
		}

}

/****************************** CmpProcessLabels ******************************/

/*
 * This function compares the labels of two process nodes.
 * It returns a negative, zero, or positive integer values according to whether
 * the labels are less than, equal or greater than respectively.
 */

DSAM_API int
CmpProcessLabels_Utility_Datum(void *a, void *b)
{
	DatumPtr	aPtr = (DatumPtr) a, bPtr = (DatumPtr) b;

	if (aPtr->label && bPtr->label)
		return(StrCmpNoCase_Utility_String(aPtr->label, bPtr->label));
	if (!aPtr->label && !bPtr->label)
		return(0);
	if (!aPtr->label && bPtr->label)
		return(-1);
	return(1);

}

/****************************** CmpProcessLabel *******************************/

/*
 * This function compares the labels of two process nodes.
 * It returns a negative, zero, or positive integer values according to whether
 * the labels are less than, equal or greater than respectively.
 */

DSAM_API int
CmpProcessLabel_Utility_Datum(void *labelPtr, void *processNode)
{
	WChar	*label = (WChar *) labelPtr;
	DatumPtr	ptr = (DatumPtr) processNode;

	if (ptr->label)
		return(StrCmpNoCase_Utility_String(label, ptr->label));
	return(1);

}

/****************************** PrintLabel ************************************/

/*
 * This routine is used to print the binary label list.  It is to be used for
 * debug purposes.
 */

void
PrintLabel_Utility_Datum(void *p)
{
	DatumPtr	ptr = (DatumPtr) p;

	if (ptr->label)
		DSAM_printf(wxT(" %s\n"), ptr->label);

}

/****************************** CmpLabel **************************************/

/*
 * This function cmpares a two string labels, passed as void pointers.
 * It returns a negative, zero, or postive integer values according to whether
 * the labels are less than, equal or greater than respectively.
 */

int
CmpLabel_Utility_Datum(void *a, void *b)
{
	WChar	*aLabel = (WChar *) a;
	WChar	*bLabel = (WChar *) b;

	if (aLabel && bLabel)
		return(StrCmpNoCase_Utility_String(aLabel, bLabel));
	if (!aLabel && !bLabel)
		return(0);
	if (!aLabel && bLabel)
		return(-1);
	return(1);

}

/****************************** SetOutputConnections **************************/

/*
 * This function sets up the specified connections for a simulation.
 * It assumes that the list has been properly initialised.
 * A ':' is used for the error message label because there seems to be an
 * error in the GNU C vsnwprintf treatment of '%%'.
 */

BOOLN
SetOutputConnections_Utility_Datum(DatumPtr pc, DynaBListPtr labelBList)
{
	static const WChar *funcName = wxT("SetOutputConnections_Utility_Datum");
	WChar	*label;
	DatumPtr	foundPC;
	DynaListPtr	p;
	DynaBListPtr	foundPtr;

	for (p = pc->u.proc.outputList; p != NULL; p = p->next) {
		label = (WChar *) p->data;
		if ((foundPtr = FindElement_Utility_DynaBList(labelBList,
		  CmpProcessLabel_Utility_Datum, label)) == NULL) {
			NotifyError(wxT("%s: Could not find process '%s: %s.%lu' output ")
			  wxT("labelled '%s' in the simulation script."), funcName, pc->
			  label, pc->data->module->name, pc->data->handle, label);
			return(FALSE);
		}
		foundPC = (DatumPtr) foundPtr->data;
		if (!FindElement_Utility_DynaList(foundPC->u.proc.inputList,
		  CmpLabel_Utility_Datum, pc->label)) {
			NotifyError(wxT("%s: Could not find label '%s' in\nprocess ")
			  wxT("'%s: %s.%lu' inputs."), funcName, pc->label, foundPC->label,
			  foundPC->data->module->name, foundPC->data->handle);
			return(FALSE);
		}
		if (!ConnectOutSignalToIn_EarObject(pc->data, foundPC->data)) {
			NotifyError(wxT("%s: Could not set '%s' connection for process ")
			  wxT("%s.%lu."), funcName, label, pc->data->module->name, pc->
			  data->handle);
			return(FALSE);
		}
	}
	return(TRUE);

}

/****************************** CheckInputConnections *************************/

/*
 * This function checks that specified input connections are correctly resolved.
 */

BOOLN
CheckInputConnections_Utility_Datum(DatumPtr pc, DynaBListPtr labelBList)
{
	static const WChar *funcName = wxT("CheckInputConnections_Utility_Datum");
	WChar	*label;
	DatumPtr	foundPC;
	DynaListPtr	p;
	DynaBListPtr	foundPtr;

	for (p = pc->u.proc.inputList; p != NULL; p = p->next) {
		label = (WChar *) p->data;
		if ((foundPtr = FindElement_Utility_DynaBList(labelBList,
		  CmpProcessLabel_Utility_Datum, label)) == NULL)  {
			NotifyError(wxT("%s: Could not find process '%s%% %s.%lu' input ")
			  wxT("labelled '%s' in the simulation script."), funcName, pc->
			  label, pc->data->module->name, pc->data->handle, label);
			return(FALSE);
		}
		foundPC = (DatumPtr) foundPtr->data;
		if (!FindElement_Utility_DynaList(foundPC->u.proc.outputList,
		  CmpLabel_Utility_Datum, pc->label)) {
			NotifyError(wxT("%s: Could not find label '%s' in\nprocess ")
			  wxT("'%s%% %s.%lu' outputs."), funcName, pc->label, foundPC->
			  label, foundPC->data->module->name, foundPC->data->handle);
			return(FALSE);
		}
	}
	return(TRUE);

}

/****************************** ResolveInstLabels *****************************/

/*
 * This routine checks that the labels used in instructions actually exist
 * within the simulation script instructions (such as reset, process connections
 * etc.).
 * It also sets the EarObject data pointer to that corresponding to the label,
 * sets up the connections or whatever is the appropriate action.
 * It must be used after the EarObjects have been initialised, if the data
 * pointers are to be valid.
 * It returns FALSE if it fails in any way.
 * Any errors here must always be reported, even if the error diagnostics had
 * been turned off.
 */

DSAM_API BOOLN
ResolveInstLabels_Utility_Datum(DatumPtr start, DynaBListPtr labelBList)
{
	static const WChar *funcName = wxT("ResolveInstLabels_Utility_Datum");
	BOOLN		ok;
	FILE		*savedErrorsFileFP = GetDSAMPtr_Common()->errorsFile;
	DatumPtr	pc;
	DynaBListPtr	p;

	SetErrorsFile_Common(wxT("screen"), OVERWRITE);
	for (pc = start, ok = TRUE; pc != NULL; pc = pc->next)
		switch (pc->type) {
		case RESET:
			if ((p = FindElement_Utility_DynaBList(labelBList,
			  CmpProcessLabel_Utility_Datum, pc->u.ref.string)) == NULL) {
				NotifyError(wxT("%s: Could not find label '%s' in the ")
				  wxT("simulation script."), funcName, pc->u.ref.string);
				ok = FALSE;
			} else
				pc->u.ref.pc = (DatumPtr) p->data;
			break;
		case PROCESS:
			if (pc->u.proc.outputList && !SetOutputConnections_Utility_Datum(
			  pc, labelBList)) {
				NotifyError(wxT("%s: Failed to set output connections."),
				  funcName);
				ok = FALSE;
			}
			if (pc->u.proc.inputList && !CheckInputConnections_Utility_Datum(
			  pc, labelBList)) {
				NotifyError(wxT("%s: Failed to resolve input connections."),
				  funcName);
				ok = FALSE;
			}
			break;
		default:
			;
		}
	GetDSAMPtr_Common()->errorsFile = savedErrorsFileFP;
	return(ok);

}

/****************************** SetDefaultConnections *************************/

/*
 * This routine sets up the default forward connection of EarObjects.
 * It assumes that the list has been properly initialised.
 * It assumes that if the next process has its input specified, then no
 * connection should be made.
 */

DSAM_API BOOLN
SetDefaultConnections_Utility_Datum(DatumPtr start)
{
	DatumPtr	pc1, pc2;

	for (pc1 = start; (pc1 != NULL); pc1 = pc1->next)
		if ((pc1->type == PROCESS) && !pc1->u.proc.outputList) {
			for (pc2 = pc1->next; (pc2 != NULL) && (pc2->type != PROCESS);
			  pc2 = pc2->next)
				;
			if ((pc2 != NULL) && !pc2->u.proc.inputList)
				ConnectOutSignalToIn_EarObject( pc1->data, pc2->data );
		}
	return(TRUE);

}

/****************************** SetDefaultLabel ******************************/

/*
 * This routine sets the default label for a process.
 * If a label has not been defined for a process, then by default it is set to
 * the process' step number, or the next unused process step number if the
 * 'labelBList' argument is not NULL.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
SetDefaultLabel_Utility_Datum(DatumPtr pc, DynaBListPtr labelBList)
{
	static const WChar *funcName = wxT("SetDefaultLabel_Utility_Datum");
	BOOLN	foundUnusedLabel = FALSE;
	WChar	label[MAXLINE];
	int		labelNumber;
	FILE	*savedErrorsFileFP;

	if (((pc->type != PROCESS) && (pc->type != REPEAT) && (pc->type !=
	  RESET)) || pc->label)
		return(TRUE);
	labelNumber = pc->stepNumber;
	savedErrorsFileFP = GetDSAMPtr_Common()->errorsFile;
	SetErrorsFile_Common(wxT("off"), OVERWRITE);
	while (!foundUnusedLabel) {
		Snprintf_Utility_String(label, MAXLINE, wxT("%s%d"),
		  DATUM_DEFAULT_LABEL_PREFIX, labelNumber);
		if (!labelBList || (FindElement_Utility_DynaBList(labelBList,
		  CmpProcessLabel_Utility_Datum, label) == NULL))
			foundUnusedLabel = TRUE;
		else
			labelNumber++;
	}
	GetDSAMPtr_Common()->errorsFile = savedErrorsFileFP;

	if ((pc->label = InitString_Utility_String(label)) == NULL) {
		NotifyError(wxT("%s: Out of memory for label '%s'."), funcName, label);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetDefaultLabels ******************************/

/*
 * This routine sets the default labels.
 * If a label has not been defined for a process, then by default it is set to
 * the process step number.
 */

BOOLN
SetDefaultLabels_Utility_Datum(DatumPtr start)
{
	static const WChar *funcName = wxT("SetDefaultLabels_Utility_Datum");
	DatumPtr	pc;

	for (pc = start; (pc != NULL); pc = pc->next)
		if (!SetDefaultLabel_Utility_Datum(pc, NULL)) {
			NotifyError(wxT("%s: Could not set default label '%s' for process ")
			  wxT("'%s'."), funcName, pc->stepNumber, pc->u.proc.moduleName);
			return(FALSE);
		}
	return(TRUE);

}

/****************************** InitProcessInst *******************************/

/*
 * This routine initialises a process instruction.
 * This routine assumes that the instruction is correctly initialised as a
 * process.
 */

DSAM_API BOOLN
InitProcessInst_Utility_Datum(DatumPtr pc)
{
	static const WChar	*funcName = wxT("InitProcessInst_Utility_Datum");

	if (pc->data)
		Free_EarObject(&pc->data);
	if ((pc->data = Init_EarObject(pc->u.proc.moduleName)) == NULL) {
		NotifyError(wxT("%s: Could not initialise process '%s' labelled '%s'."),
		  funcName, pc->u.proc.moduleName, pc->label);
		return(FALSE);
	}
	pc->classSpecifier = pc->data->module->classSpecifier;
	return(TRUE);

}

/****************************** InitialiseEarObjects **************************/

/*
 * This routine initialises and EarObjects for the simulation script.
 */

BOOLN
InitialiseEarObjects_Utility_Datum(DatumPtr start, DynaBListPtr *labelBList)
{
	static const WChar	*funcName = wxT("InitialiseEarObjects_Utility_Datum");
	BOOLN	ok = TRUE;
	DatumPtr	pc;

	if (start == NULL) {
		NotifyError(wxT("%s: Simulation not initialised.\n"), funcName);
		return(FALSE);
	}
	if (!SetDefaultLabels_Utility_Datum(start)) {
		NotifyError(wxT("%s: Could not set default labels."), funcName);
		return(FALSE);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS) {
			if (!InitProcessInst_Utility_Datum(pc)) {
				NotifyError(wxT("%s: Could not initialise process with '%s'."),
				  funcName, pc->u.proc.moduleName);
				ok = FALSE;
			}
			if (pc->label && !Insert_Utility_DynaBList(labelBList,
			  CmpProcessLabels_Utility_Datum, pc)) {
				NotifyError(wxT("%s: Cannot insert process labelled '%s' into ")
				  wxT("simulation."), funcName, pc->label);
				ok = FALSE;
			}
		}
	if (ok)
		ok = ResolveInstLabels_Utility_Datum(start, *labelBList);

	if (ok && !SetDefaultConnections_Utility_Datum(start)) {
		NotifyError(wxT("%s Could not set default foward connections."),
		  funcName);
		ok = FALSE;
	}
	return (ok);

}

/****************************** FreeEarObjects ********************************/

/*
 * This routine frees the EarObjects for the simulation script.
 */

BOOLN
FreeEarObjects_Utility_Datum(DatumPtr start)
{
	/* static const WChar	*funcName = wxT("FreeEarObjects_Utility_Datum"); */
	DatumPtr	pc;
	
	for (pc = start; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS)
			Free_EarObject(&pc->data);
	return(TRUE);
}



/****************************** NameAndLabel **********************************/

/*
 * This routine returns a pointer to a string containing the proces name and
 * label for a Datum structure.
 * A copy of this information should be used if a permanent string is required.
 */

DSAM_API WChar *
NameAndLabel_Utility_Datum(DatumPtr pc)
{
	static const WChar	*funcName = wxT("NameAndLabel_Utility_Datum");
	static WChar	string[MAXLINE];

	if (!pc) {
		NotifyError(wxT("%s: Pointer not initialised."), funcName);
		return(NULL);
	}
	Snprintf_Utility_String(string, MAXLINE, wxT("%s.%s"), 
	  GetProcessName_Utility_Datum(pc), pc->label);
	return(string);

}

/****************************** InitialiseModules *****************************/

/*
 * This routine initialises all the necessary modules.
 * If it encounters the 'DISPLAY_MODULE' module, then it will attempt to set the
 * window title.  It checks for a NULL 'UniParListPtr' which indicates that the
 * GUI library is not being used.
 * 
 * It assumes that all 'internal' sub-simulation scripts that have the
 * simulation already initialised only needs the other parameters to be
 * initialised.
 */

BOOLN
InitialiseModules_Utility_Datum(DatumPtr start)
{
	BOOLN	ok;
	DatumPtr	pc;
	
	for (pc = start, ok = TRUE; (pc != NULL) && ok; pc = pc->next)
		if (pc->type == PROCESS ) {
			if (GetDSAMPtr_Common()->usingGUIFlag && (pc->data->module->
			  specifier == DISPLAY_MODULE) && (GetUniParPtr_ModuleMgr(pc->data,
			  wxT("win_title"))->valuePtr.s[0] == '\0')) {
				SetPar_ModuleMgr(pc->data, wxT("win_title"),
				  NameAndLabel_Utility_Datum(pc));
			}
			if ((DSAM_strcmp(pc->u.proc.parFile, NO_FILE) != 0) &&
			  ((pc->data->module->specifier != SIMSCRIPT_MODULE) ||
			  !GetSimulation_ModuleMgr(pc->data))) {
				if (!ReadPars_ModuleMgr(pc->data, pc->u.proc.parFile))
					ok = FALSE;
			}
		}
	return(ok);

}

/****************************** PrintParsModules ******************************/

/*
 * This routine issues the PrintPars command for all the process modules.
 */

BOOLN
PrintParsModules_Utility_Datum(DatumPtr start)
{
	BOOLN	ok;
	DatumPtr	pc;
	
	for (pc = start, ok = TRUE; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS)
			if ((pc->data == NULL) || !PrintPars_ModuleMgr( pc->data ))
				ok = FALSE;
	return(ok);

}

/****************************** PrintParListModules ***************************/

/*
 * This routine prints the universal parameter lists for all the process
 * modules.
 */

BOOLN
PrintParListModules_Utility_Datum(DatumPtr start, WChar *prefix)
{
	BOOLN	ok = TRUE;
	WChar	suffix[MAXLINE], fmtParFileName[MAXLINE];
	DatumPtr		pc;
	UniParListPtr	parList;

	for (pc = start; ok && (pc != NULL); pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier !=
		  NULL_MODULE)) {
			if ((pc->data == NULL) || ((parList = GetUniParListPtr_ModuleMgr(
			  pc->data )) == NULL))
				continue;
			else if (pc->data->module->specifier == SIMSCRIPT_MODULE)
				ok = PrintParListModules_Utility_Datum(GetSimulation_ModuleMgr(
				  pc->data), prefix);
			else {
				Snprintf_Utility_String(fmtParFileName, MAXLINE, wxT("(%s)"),
				  pc->u.proc.parFile);
				DPrint(wxT("##----- %-20s %20s -----##\n"),
				  NameAndLabel_Utility_Datum(pc), fmtParFileName);
				Snprintf_Utility_String(suffix, MAXLINE, wxT(".%s"),
				  NameAndLabel_Utility_Datum(pc));
				ok = (parList)? PrintPars_UniParMgr(parList, prefix, suffix):
				  TRUE;
				DPrint(wxT("\n"));
			}
		}
	return(ok);

}

/****************************** CheckParLists *********************************/

/*
 * This routine checks if the universal parameters lists require updating.
 * This is particularly necessary for the CFList lists and similar structures.
 * This routine will not return an error if a module has a null parameter list.
 */

BOOLN
CheckParLists_Utility_Datum(DatumPtr start)
{
	BOOLN	ok = TRUE;
	DatumPtr		pc;
	UniParListPtr	parList;

	for (pc = start; ok && (pc != NULL); pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier !=
		  NULL_MODULE)) {
			if (pc->data == NULL)
				ok = FALSE;
			else {
				if (((parList = GetUniParListPtr_ModuleMgr( pc->data )) !=
				  NULL) && !CheckParList_UniParMgr(parList))
					ok = FALSE;
			}
		}
	return(ok);

}

/****************************** ResetSimulation *******************************/

/*
 * This routine resets all of the simulation processes.
 */

void
ResetSimulation_Utility_Datum(DatumPtr start)
{
	DatumPtr		pc;

	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier !=
		  NULL_MODULE))
			if (pc->data != NULL)
				ResetProcess_EarObject(pc->data);

}

/****************************** ExecuteStandard *******************************/

/*
 * This routine executes a set of simulation instructions.
 * It continues until it reachs  a NULL instuction.
 * It returns a pointer to the last instruction run.
 */

#define	GET_PROCESS(DATA)	((!threadIndex)? (DATA): &(DATA)->threadProcs[ \
							  threadIndex - 1])

DSAM_API DatumPtr
ExecuteStandard_Utility_Datum(DatumPtr start, DatumPtr passedEnd,
  int threadIndex)
{
	static const WChar *funcName = wxT("ExecuteStandard_Utility_Datum");
	int			i;
	DatumPtr	pc, lastInstruction = NULL;
	EarObjectPtr	process;

	if (start == NULL)
		return NULL;
	for (pc = start; pc != passedEnd; pc = pc->next) {
		switch (pc->type) {
		case PROCESS: {
			process = GET_PROCESS(pc->data);
			if (!RunProcess_ModuleMgr(process)) {
				NotifyError(wxT("%s: Could not run process '%s'."), funcName,
				  pc->label);
				return(NULL);
			}
		#	if DEBUG
			printf(wxT("%s: Debug: Run '%s' with channels %d -> %d.\n"),
			  funcName, pc->label, process->outSignal->offset,
			  process->outSignal->numChannels - 1);
		#	endif
			break; }
		case RESET:
			process = GET_PROCESS(pc->u.ref.pc->data);
			if (!process->module->onFlag)
				break;
			ResetProcess_EarObject(process);
			break;
		case STOP:
			return (pc);
		case REPEAT:
			if (!pc->u.loop.count) {
				NotifyError(wxT("%s: Illegal zero 'repeat' count."), funcName);
				return(NULL);
			}
			if (!pc->u.loop.stopPC) {
				NotifyError(wxT("%s: Repeat has no end point."), funcName);
				return(NULL);
			}
#			if DEBUG
			clock_t startLoop = clock();
#			endif
			for (i = 0; i < pc->u.loop.count; i++)
				lastInstruction = Execute_Utility_Datum(pc->next, passedEnd,
				  threadIndex);
			pc = lastInstruction;
#			if DEBUG
			clock_t EndLoop = clock();
#			endif
			break;
		default:
			break;
		} /* switch */
		if ((lastInstruction = pc) == NULL)
			break;
	}
	return(lastInstruction);

}

#undef GET_PROCESS

/****************************** GetLastProcess ********************************/

/*
 * This routine returns the last process in a simulation.
 * It returns NULL if it fails in any way.
 */

EarObjectPtr
GetLastProcess_Utility_Datum(DatumPtr start)
{
	EarObjectPtr	lastProcess = NULL;
	DatumPtr	pc;

    for (pc = start; pc != NULL; pc = pc->next)
    	if (pc->type == PROCESS)
    		lastProcess = pc->data;
	return(lastProcess);

}

/****************************** GetFirstProcess *******************************/

/*
 * This routine returns the first process in a simulation.
 * It returns NULL if it fails in any way.
 */

EarObjectPtr
GetFirstProcess_Utility_Datum(DatumPtr start)
{
	DatumPtr	pc;

    for (pc = start; pc != NULL; pc = pc->next)
    	if (pc->type == PROCESS)
    		return(pc->data);
	return(NULL);

}

/****************************** GetFirstProcessInst ***************************/

/*
 * This routine returns the first process instruction in a simulation.
 * It returns NULL if it fails in any way.
 */

DSAM_API DatumPtr
GetFirstProcessInst_Utility_Datum(DatumPtr start)
{
	DatumPtr	pc;

    for (pc = start; pc != NULL; pc = pc->next)
    	if (pc->type == PROCESS)
    		return(pc);
	return(NULL);

}

/****************************** GetPreviousProcessInst ************************/

/*
 * This routine returns the previous process instruction in a simulation.
 * It returns NULL if it fails in any way.
 */

DSAM_API DatumPtr
GetPreviousProcessInst_Utility_Datum(DatumPtr start)
{
	DatumPtr	pc;

    for (pc = start; pc != NULL; pc = pc->previous)
    	if (pc->type == PROCESS)
    		return(pc);
	return(NULL);

}

/****************************** FindLabelledProcess ***************************/

/*
 * This routine returns the pointer to a process' EarObject in a simulation,
 * Specified by a label.
 * If the label is the '*' wild card, than the first process found is returned.
 * It returns NULL if it fails in any way.
 */

EarObjectPtr
FindLabelledProcess_Utility_Datum(DatumPtr start, WChar *label)
{
	static const WChar *funcName = wxT("FindLabelledProcess_Utility_Datum");
	DatumPtr	pc;

	if (label == NULL) {
		NotifyError(wxT("%s: Illegal label!"), funcName);
		return(NULL);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && ((label[0] == '*') || (DSAM_strcmp(label,
		  pc->label) == 0)))
			return(pc->data);
	return(NULL);

}

/****************************** FindLabelledProcessInst ***********************/

/*
 * This routine returns the pointer to a process instruction in a simulation,
 * Specified by a label.
 * If the label is the '*' wild card, than the first process found is returned.
 * It returns NULL if it fails in any way.
 * If it does not find the label, then it expects the calling routine to give
 * The correct error message.
 */

DatumPtr
FindLabelledProcessInst_Utility_Datum(DatumPtr start, WChar *label)
{
	static const WChar *funcName = wxT("FindLabelledProcessInst_Utility_Datum");
	DatumPtr	pc;

	if (label == NULL) {
		NotifyError(wxT("%s: Illegal NULL label!"), funcName);
		return(NULL);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && ((label[0] == '*') || (DSAM_strcmp(label,
		  pc->label) == 0)))
			return(pc);
	return(NULL);

}

/****************************** FindLabelledInst ******************************/

/*
 * This routine returns the pointer to an instruction in a simulation,
 * specified by a label.
 * If the label is the '*' wild card, than the first instruction found is
 * returned.
 * It returns NULL if it fails in any way.
 * If it does not find the label, then it expects the calling routine to give
 * The correct error message.
 */

DatumPtr
FindLabelledInst_Utility_Datum(DatumPtr start, const WChar *label)
{
	static const WChar *funcName = wxT("FindLabelledInst_Utility_Datum");
	DatumPtr	pc;

	if (label == NULL) {
		NotifyError(wxT("%s: Illegal NULL label!"), funcName);
		return(NULL);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if (((pc->type == PROCESS) || (pc->type == REPEAT)) && ((label[0] ==
		  '*') || (DSAM_strcmp(label, pc->label) == 0)))
			return(pc);
	return(NULL);

}

/****************************** FindModuleProcessInst *************************/

/*
 * This routine returns the pointer to a process' instruction datum in a
 * simulation, specified by a substring of its module name.
 * It returns NULL if it fails in any way.
 * If it does not find the label, then it expects the calling routine to give
 * The correct error message.
 */

DatumPtr
FindModuleProcessInst_Utility_Datum(DatumPtr start, WChar *moduleName)
{
	static const WChar *funcName = wxT("FindModuleProcessInst_Utility_Datum");
	WChar		upperName[MAXLINE];
	DatumPtr	pc;

	if (moduleName == NULL) {
		NotifyError(wxT("%s: Illegal module name!"), funcName);
		return(NULL);
	}
	ToUpper_Utility_String(upperName, moduleName);
	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (DSAM_strstr(pc->data->module->name,
		  upperName) != NULL))
			return(pc);
	NotifyError(wxT("%s: Could not find module '%s' in the simulation script."),
	  funcName, upperName);
	return(NULL);

}

/****************************** FindModuleUniPar ******************************/

/*
 * This function finds a process' universal parameter in a
 * simulation, specified by a substring of its parameter abbreviation.
 * It returns NULL if it fails in any way.
 * If it does not find the label, then it expects the calling routine to give
 * The correct error message.
 * The function will check for process numbers at the end of the parameter,
 * e.g. "freq_pt.0" name.
 * This routine returns a pointer to the parameter list, and the index within
 * the list, as both of these are needed for processing structures like CFList.
 * The general parameters are encoded by adding the number of parameters in the
 * list to the parameter specifier.
 * The 'FindUniPar_UniParMgr' changes the 'pList' value, cannot be used again
 * when a general list parameter is searched for.
 * This routine will not produce error diagnostics if the 'pList' is NULL.
 * If the process label starts with a digit, then it is assumed that an older
 * SPF file is being read.
 */

BOOLN
FindModuleUniPar_Utility_Datum(UniParListPtr *parList, uInt *index,
  DatumPtr *pc, const WChar *parSpecifier, BOOLN diagnosticsOn)
{
	static const WChar *funcName = wxT("FindModuleUniPar_Utility_Datum");
	WChar		*p, parName[MAXLINE], processName[MAXLINE], processStr[MAXLINE];
	WChar		processLabel[MAXLINE];
	DatumPtr	tempPc;
	UniParPtr		par;
	UniParListPtr	pList = NULL;

	if (parSpecifier == NULL) {
		NotifyError(wxT("%s: Illegal parameter name '%s'!"), funcName,
		  parSpecifier);
		return(FALSE);
	}
	DSAM_strncpy(parName, parSpecifier, MAXLINE);
	if ((p = DSAM_strchr(parName, UNIPAR_NAME_SEPARATOR)) == NULL) {
		processName[0] = '\0';
		processLabel[0] = '\0';
	} else {
		*p = '\0';
		DSAM_strncpy(processName, p + 1, MAXLINE);
		if ((p = DSAM_strchr(processName, UNIPAR_NAME_SEPARATOR)) != NULL) {
			Snprintf_Utility_String(processLabel, MAXLINE, wxT("%s%s"),
			  (isdigit(*(p + 1)))? DATUM_DEFAULT_LABEL_PREFIX: wxT(""), p + 1);
			*p = '\0';
		} else
			processLabel[0] = '\0';
	}
	for ( ; *pc != NULL; *pc = (*pc)->next)
		if (((*pc)->type == PROCESS) && ((*pc)->data->module->specifier !=
		  NULL_MODULE)) {
			if ((*pc)->data->module->specifier == SIMSCRIPT_MODULE) {
				tempPc = ((SimScriptPtr) (*pc)->data->module->parsPtr)->
				  simulation;
				if (FindModuleUniPar_Utility_Datum(parList, index, &tempPc,
				  parSpecifier, FALSE)) {
					*pc = tempPc;
					return(TRUE);
				}
			}
			if (((processName[0] == '\0') || (StrNCmpNoCase_Utility_String(
			  (*pc)->data->module->name, processName) == 0)) && ((pList =
			  GetUniParListPtr_ModuleMgr((*pc)->data)) != NULL) &&
			  ((processLabel[0] == '\0') || (StrCmpNoCase_Utility_String(
			  (*pc)->label, processLabel) == 0))) {
				if ((par = FindUniPar_UniParMgr(&pList, parName,
				  UNIPAR_SEARCH_ABBR)) != NULL) {
			  		*parList = pList;
					*index = par->index;
					return(TRUE);
				}
			}
		}
	if (diagnosticsOn && pList) {
		if (processLabel[0] == '\0')
			processStr[0] = '\0';
		else
			Snprintf_Utility_String(processStr, MAXLINE, wxT(" for process ")
			  wxT("[%s] "), processLabel);
		NotifyError(wxT("%s: Could not find parameter '%s'%s in the ")
		  wxT("simulation script."), funcName, parName, processStr);
	}
	if (!pList)
		*parList = NULL;
	return(FALSE);

}

/****************************** FindProcess ***********************************/

/*
 * This function finds in a simulation, specified by a substring of its
 * module name, with an optional step number.
 * It returns NULL if it fails in any way.
 */

EarObjectPtr
FindProcess_Utility_Datum(DatumPtr pc, WChar *processSpecifier)
{
	static const WChar *funcName = wxT("FindProcess_Utility_Datum");
	WChar	*p, processName[MAXLINE], processStr[MAXLINE];
	WChar	processLabel[MAXLINE];

	if (processSpecifier == NULL) {
		NotifyError(wxT("%s: Illegal parameter name '%s'!"), funcName,
		  processSpecifier);
		return(NULL);
	}
	DSAM_strncpy(processName, processSpecifier, MAXLINE);
	if ((p = DSAM_strchr(processName, UNIPAR_NAME_SEPARATOR)) == NULL)
		processLabel[0] = '\0';
	else
		DSAM_strncpy(processLabel, p + 1, MAXLINE);
	if ((p = DSAM_strchr(processName, UNIPAR_NAME_SEPARATOR)) != NULL)
		*p = '\0';
	for ( ; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier !=
		  NULL_MODULE)) {
			if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
				return(FindProcess_Utility_Datum(((SimScriptPtr) pc->data->
				  module->parsPtr)->simulation, processSpecifier));
			}
			if (((processName[0] == '\0') || (StrNCmpNoCase_Utility_String(
			  pc->data->module->name, processName) == 0)) && ((processLabel[
			  0] == '\0') || (StrNCmpNoCase_Utility_String(pc->label,
			  processLabel) == 0)))
				return(pc->data);
		}
	if (processLabel[0] == '\0')
		processStr[0] = '\0';
	else
		Snprintf_Utility_String(processStr, MAXLINE, wxT(", label [%s] "),
		  processLabel);
	NotifyError(wxT("%s: Could not find process '%s'%s in the simulation ")
	  wxT("script."), funcName, processName, processStr);
	return(NULL);

}

/****************************** GetInstIntVal *********************************/

/*
 * This routine returns the integer value for a labelled instruction from the
 * module's simulation script.
 */

int
GetInstIntVal_Utility_Datum(DatumPtr start, WChar *label)
{
	static const WChar *funcName = wxT("GetInstIntVal_Utility_Datum");
	DatumPtr	pc;

	if (label == NULL) {
		NotifyError(wxT("%s: Illegal NULL label!"), funcName);
		exit(1);
	}
	if ((pc = FindLabelledInst_Utility_Datum(start, label)) == NULL) {
		NotifyError(wxT("%s: Could not find label in simulation script."),
		  funcName);
		exit(1);
	}
	if (pc->type != REPEAT) {
		NotifyError(wxT("%s: Labelled instruction has no associated integer ")
		  wxT("value."), funcName);
		exit(1);
	}
	return (pc->u.loop.count);

}

/****************************** SetControlParValue ****************************/

/*
 * This function sets the control parameter values for a labelled instruction.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetControlParValue_Utility_Datum(DatumPtr start, const WChar *label, const WChar *value,
  BOOLN diagsOn)
{
	static const WChar	*funcName = wxT("SetControlParValue_Utility_Datum");
	DatumPtr	pc;

	if (start == NULL) {
		NotifyError(wxT("%s: Simulation not initialised."), funcName);
		return(FALSE);
	}
	if ((pc = FindLabelledInst_Utility_Datum(start, label)) == NULL) {
		if (diagsOn)
			NotifyError(wxT("%s: Could not find label '%s' in simulation ")
			  wxT("script."), funcName, label);
		return(FALSE);
	}
	switch (pc->type) {
	case REPEAT: {
		int count = DSAM_atoi(value);
		if (count <= 0) {
			NotifyError(wxT("%s: Illegal value for repeat loop count (%d).\n"),
			  funcName);
			return(FALSE);
		}
		pc->u.loop.count = count;
		break; }
	case PROCESS: {
		int status = Identify_NameSpecifier(value, BooleanList_NSpecLists(0));
		if (status == GENERAL_BOOLEAN_NULL) {
			NotifyError(wxT("%s: Illegal switch state (%s)."), funcName, value);
			return(FALSE);
		}
		Enable_ModuleMgr(pc->data, status);
		break; }
	default:
		NotifyError(wxT("%s: Labelled instruction, '%s' has no associated ")
		  wxT("integer value."), funcName, label);
		return(FALSE);
	} /* switch */
	return(TRUE);

}

/****************************** SetUniParValue ********************************/

/*
 * This function sets a simulation universal parameter value.
 * It returns TRUE if the operation is successful.
 * If the returned 'parList' is NULL, then no parameter is set.
 */

DSAM_API BOOLN
SetUniParValue_Utility_Datum(DatumPtr start, const WChar *parName, const WChar *parValue)
{
	static const WChar	*funcName = wxT("SetUniParValue_Utility_Datum");
	uInt	index;
	UniParListPtr		parList;
	DatumPtr	pc;

	if (start == NULL) {
		NotifyError(wxT("%s: Simulation not initialised."), funcName);
		return(FALSE);
	}
	pc = start;
	if (!FindModuleUniPar_Utility_Datum(&parList, &index, &pc, parName, TRUE)) {
		NotifyError(wxT("%s: Could not find '%s' parameter in simulation."),
		  funcName, parName);
		return(FALSE);
	}
	(* pc->data->module->SetParsPointer)(pc->data->module);
	return(SetParValue_UniParMgr(&parList, index, parValue));

}

/****************************** ResetStepCount ********************************/

/*
 * This routine resets the global datumStepCount variable.
 * It is needed because MS VC++ complains.
 */

DSAM_API void
ResetStepCount_Utility_Datum(void)
{
	datumStepCount = 0;

}

/****************************** FindNearestProcesses **************************/

/*
 * This routine ensures that process instructions are connected 'though' non-
 * process instructions like 'reset' and 'repeat'.
 * It changes the pointer arguments to the nearest process found.
 * It return's TRUE if a nearest process is found.
 */

DSAM_API BOOLN
FindNearestProcesses_Utility_Datum(DatumPtr *fromPc, DatumPtr *toPc)
{
	if (((*fromPc)->type != PROCESS) && ((*toPc)->type != PROCESS))
		return(FALSE);
	while (*fromPc && ((*fromPc)->type != PROCESS))
		*fromPc = (*fromPc)->previous;
	if (!*fromPc)
		return(FALSE);
	while (*toPc && ((*toPc)->type != PROCESS))
		*toPc = (*toPc)->next;
	if (!*toPc)
		return(FALSE);
	return(TRUE);

}

/****************************** SetDefaultProcessFileName *********************/

/*
 * This routine sets the process file name to the module number with the
 * step number as a suffix, but only if the file name has not already been set.
 */

void
SetDefaultProcessFileName_Utility_Datum(DatumPtr pc)
{
	WChar	fileName[MAXLINE];

	if (pc->type != PROCESS)
		return;
	if ((*pc->u.proc.parFile == '\0') || (DSAM_strcmp(pc->u.proc.parFile,
	  NO_FILE) == 0)) {
		Snprintf_Utility_String(fileName, MAXLINE, wxT("%s_%d.par"), pc->u.proc.
		  moduleName, pc->stepNumber);
		pc->u.proc.parFile = InitString_Utility_String(fileName);
	}

}

/*************************** WriteParFiles ************************************/

/*
 * This function prints a simulation's '.par' files and the simulation scripts
 * of any sub-simulations.
 * This separate routine is required because of the way that the GUI code works.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
WriteParFiles_Datum(WChar *filePath, DatumPtr start)
{
	static const WChar *funcName = wxT("WriteParFiles_Datum");
	WChar	fileName[MAX_FILE_PATH];
	DatumPtr	pc;
	UniParListPtr	parList;

	if (!start) {
		NotifyError(wxT("%s: Simulation not initialised."), funcName);
		return(FALSE);
	}
	for (pc = start; pc; pc = pc->next) {
		if (pc->type != PROCESS)
			continue;
		if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
			BOOLN ok = TRUE;
			if (!WriteParFiles_Datum(filePath, GetSimulation_ModuleMgr(
			  pc->data)))
				ok = FALSE;
			if (ok && !WriteSimScript_Datum(pc->u.proc.parFile,
			  GetSimulation_ModuleMgr(pc->data)))
				ok = FALSE;
			if (!ok) {
				NotifyError(wxT("%s: Failed to write sub-simulation '%s'."),
				  funcName, pc->u.proc.parFile);
				return(FALSE);
			}
			continue;
		}
		SET_PARS_POINTER(pc->data);
		if ((parList = (* pc->data->module->GetUniParListPtr)()) == NULL)
			return(TRUE);
		SetDefaultProcessFileName_Utility_Datum(pc);
		Snprintf_Utility_String(fileName, MAX_FILE_PATH, wxT("%s/%s"), filePath,
		  pc->u.proc.parFile);
		if (!WriteParFile_UniParMgr(fileName, parList)) {
			NotifyError(wxT("%s: Failed to write parameter file '%s'."),
			  funcName, fileName);
			return(FALSE);
		}
	}
	return(TRUE);

}

/*************************** WriteSimScript ***********************************/

/*
 * This function prints a simulation as a ".sim" file.
 * This separate routine is required because of the way that the GUI code works.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
WriteSimScript_Datum(WChar *fileName, DatumPtr start)
{
	static const WChar *funcName = wxT("WriteSimScript_Datum");
	FILE *oldFp = GetDSAMPtr_Common()->parsFile;

	if (!start) {
		NotifyError(wxT("%s: Simulation not initialised."), funcName);
		return(FALSE);
	}
	SetParsFile_Common(fileName, OVERWRITE);
	PrintSimScript_Utility_Datum(start, fileName, 0, wxT(""), FALSE);
	fclose(GetDSAMPtr_Common()->parsFile);
	GetDSAMPtr_Common()->parsFile = oldFp;
	return(TRUE);

}

/************************** SetExecute ****************************************/

/*
 * This routine sets the global 'RunProcess' function pointer.
 */

DSAM_API void
SetExecute_Utility_Datum(DatumPtr (* Func)(DatumPtr, DatumPtr, int))
{
	Execute_Utility_Datum = Func;

}

/************************** EnableProcess *************************************/

DSAM_API BOOLN
EnableProcess_Utility_Datum(DatumPtr pc, BOOLN status)
{
	static const WChar *funcName = wxT("EnableProcess_Utility_Datum");

	if (!pc) {
		NotifyError(wxT("%s: Instruction not initialised."), funcName);
		return(FALSE);
	}
	if (pc->type != PROCESS) {
		NotifyError(wxT("%s: Instruction is not PROCESS type."), funcName);
		return(FALSE);
	}
	pc->onFlag = status;
	Enable_ModuleMgr(pc->data, status);
	return(TRUE);

}

