/******************
 *
 * File:		GeUniParMgr.c
 * Purpose:		This file contains the universal parameter manager routines
 * Comments:	Note that the "UNIPAR_STRING" values only have the initial
 *				values.  I have got to check this.
 *				08-10-98 LPO: Fixed bug with name specifiers, by introducing
 *				the 'UNIPAR_NAME_SPEC' type: 'LINEAR' mode was being selected
 *				by the system when 'SINE' was asked for - I hadn't noticed that.
 *				12-10-98 LPO: Parameter lists that return structures, such as
 *				the CFList, will only carry out the setting function when all
 *				parameters have been set.
 *				06-11-98 LPO: Implemented the array handling for the likes of
 *				the StMPTone module.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for
 *				modules with no parameters.
 *				11-12-98 LPO: Introduced the second array index for the IC list.
 *				19-01-99 LPO: The 'arrayIndex[0]' always refers to the last
 *				(lowest?) index level, i.e. for the 'ICList' mode
 *				'arrayIndex[1]' refers to the ion ionChannel, and
 *				'arrayIndex[0]' refers to the respective ion channel parameter
 *				array element.
 *				27-01-99 LPO: Corrected problem in 'FindUniPar_' where it was
 *				returning before checking the rest of a parameter list after a
 *				sub module's parameter list.
 *				29-04-99 LPO: The 'FindUniPar_' routine can now find the
 *				'CFLIST' abbreviation so that the 'CFListPtr' pointer can be
 *				accessed.
 *				19-05-99 LPO: I have changed the 'UNIPAR_FILE_NAME' code so that
 *				I can save the 'defaultExtension' to be used with the GUI.
 *				30-05-99 LPO: The strings and file names are now printed
 *				surrounded by speech marks, '"'.
 *				02-06-99 LPO: I have updated the 'FindUniPar_' routine so that
 *				it now will find the parameters within a simulation script.
 *				03-06-99 LPO: Corrected the problem with 'SetParValue_' not
 *				treating the general list parameters correctly.
 *				08-09-99 LPO: In the 'FindUniPar_' routine only the sub-
 *				parameters for a module would be set, and not the module
 *				parameter file name itself.  This has been fixed.
 *				26-10-99 LPO: Introduced the 'enabled' flag for the 'UniPar'
 *				structure.  Printing, setting operations and such like will not
 *				be carried out on 'disabled' parameters.
 * Authors:		L. P. O'Mard
 * Created:		24 Sep 1998
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

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeNSpecLists.h"
#include "UtNameSpecs.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtIonChanList.h"
#include "UtParArray.h"
#include "UtString.h"
#include "UtDatum.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"

/******************************************************************************/
/************************ Global variables ************************************/
/******************************************************************************/

/******************************************************************************/
/************************ Subroutines and functions ***************************/
/******************************************************************************/

/************************ InitList ********************************************/

/*
 * This routine initialises a universal parameter list structure.
 */

DSAM_API UniParListPtr
InitList_UniParMgr(UniParModeSpecifier mode, int numPars, void *handlePtr)
{
	static const WChar *funcName = wxT("IniList_UniParMgr");
	int		i;
	UniParListPtr	p;

	if ((p = (UniParListPtr) malloc(sizeof(UniParList))) == NULL) {
		NotifyError(wxT("%s: Could not allocate UniParList."), funcName);
		return(NULL);
	}
	if ((p->pars = (UniPar *) calloc(numPars, sizeof(UniPar))) == NULL) {
		NotifyError(wxT("%s: Could not allocate %d UniPars."), funcName,
		  numPars);
		FreeList_UniParMgr(&p);
		return(NULL);
	}
	p->updateFlag = FALSE;
	p->notebookPanel = -1;
	p->mode = mode;
	p->numPars = numPars;
	switch (mode) {
	case UNIPAR_SET_CFLIST:
		p->handlePtr.cFs = (CFListPtr) handlePtr;
		break;
	case UNIPAR_SET_IC:
	case UNIPAR_SET_ICLIST:
		p->handlePtr.iCs = (IonChanListPtr) handlePtr;
		break;
	case UNIPAR_SET_PARARRAY:
		p->handlePtr.parArray.ptr = (ParArrayPtr) handlePtr;
		p->handlePtr.parArray.SetFunc = NULL;
		break;
	default:
		;
	}
	for (i = 0; i < numPars; i++) {
		p->pars[i].mode = mode;
		p->pars[i].index = i;
	}
	p->GetPanelList = NULL;
	return(p);

}

/************************ FreeList ********************************************/

/*
 * This routine frees the memory allocated for a universal parameter list
 * structure.
 * It also sets the pointer, passed by reference, to NULL;
 */

DSAM_API void
FreeList_UniParMgr(UniParListPtr *list)
{
	if (*list == NULL)
		return;
	if ((*list)->pars)
		free((*list)->pars);
	free(*list);
	*list = NULL;

}

/************************ SetPar **********************************************/

/*
 * This routine sets the members of a universal parameter structure.
 * The 'mode' member is set elsewhere e.g., InitList_UniParMgr.
 */

void
SetPar_UniParMgr(UniParPtr par, const WChar *abbreviation, const WChar *description,
  UniParTypeSpecifier type, void *ptr1, void *ptr2, void * (* Func))
{
	static const WChar	*funcName = wxT("SetPar_UniParMgr");

	par->enabled = TRUE;
	par->type = type;
	par->abbr = abbreviation;
	par->altAbbr = NULL;
	par->desc = description;
	switch (type) {
	case UNIPAR_INT:
	case UNIPAR_INT_AL:
	case UNIPAR_BOOL:
		par->valuePtr.i = (int *) ptr1;
		break;
	case UNIPAR_BOOL_ARRAY:
	case UNIPAR_INT_ARRAY:
		par->valuePtr.array.index = 0;
		par->valuePtr.array.pPtr.i = (int **) ptr1;
		par->valuePtr.array.numElements = (int *) ptr2;
		break;
	case UNIPAR_LONG:
		par->valuePtr.l = (long *) ptr1;
		break;
	case UNIPAR_REAL:
		par->valuePtr.r = (Float *) ptr1;
		break;
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
		par->valuePtr.array.index = 0;
		par->valuePtr.array.pPtr.r = (Float **) ptr1;
		par->valuePtr.array.numElements = (int *) ptr2;
		break;
	case UNIPAR_STRING:
		par->valuePtr.s = (WChar *) ptr1;
		break;
	case UNIPAR_STRING_ARRAY:
		par->valuePtr.array.index = 0;
		par->valuePtr.array.pPtr.s = (WChar ***) ptr1;
		par->valuePtr.array.numElements = (int *) ptr2;
		break;
	case UNIPAR_FILE_NAME:
		par->valuePtr.file.name = (WChar *) ptr1;
		par->valuePtr.file.defaultExtension = (WChar *) ptr2;
		break;
	case UNIPAR_MODULE:
		par->valuePtr.module.parFile = (WChar *) ptr1;
		par->valuePtr.module.parList = (UniParListPtr) ptr2;
		break;
	case UNIPAR_CFLIST:
		par->valuePtr.cFPtr = (CFListPtr *) ptr1;
		break;
	case UNIPAR_PARARRAY:
		par->valuePtr.pAPtr = (ParArrayPtr *) ptr1;
		break;
	case UNIPAR_ICLIST:
		par->valuePtr.iCPtr = (IonChanListPtr *) ptr1;
		break;
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		par->valuePtr.nameList.specifier = (int *) ptr1;
		par->valuePtr.nameList.list = (NameSpecifier *) ptr2;
		break;
	case UNIPAR_NAME_SPEC_ARRAY:
		par->valuePtr.array.index = 0;
		par->valuePtr.array.pPtr.nameList.specifier = (int **) ((void **) ptr1)[
		  0];
		par->valuePtr.array.numElements = (int *) ((void **) ptr1)[1];
		par->valuePtr.array.pPtr.nameList.list = (NameSpecifier *) ptr2;
		break;
	case UNIPAR_SIMSCRIPT:
		par->valuePtr.simScript.simulation = (DatumPtr *) ptr1;
		par->valuePtr.simScript.fileName = (WChar *) ptr2;
		break;
	case UNIPAR_PARLIST:
		par->valuePtr.parList.list = (UniParListPtr *) ptr1;
		par->valuePtr.parList.process = (EarObjectPtr *) ptr2;
		break;
	default:
		NotifyError(wxT("%s: Universal parameter not yet implemented (%d)."),
		  funcName, type);
	}
	switch (par->mode) {
	case UNIPAR_SET_GENERAL:
	case UNIPAR_SET_SIMSPEC:
		switch (type) {
		case UNIPAR_INT:
		case UNIPAR_INT_AL:
			par->FuncPtr.SetInt = (BOOLN (*)(int)) Func;
			break;
		case UNIPAR_INT_ARRAY:
			par->FuncPtr.SetIntArrayElement = (BOOLN (*)(int , int)) Func;
			break;
		case UNIPAR_LONG:
			par->FuncPtr.SetLong = (BOOLN (*)(long)) Func;
			break;
		case UNIPAR_REAL:
			par->FuncPtr.SetReal = (BOOLN (*)(Float)) Func;
			break;
		case UNIPAR_REAL_ARRAY:
		case UNIPAR_REAL_DYN_ARRAY:
			par->FuncPtr.SetRealArrayElement = (BOOLN (*)(int, Float)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
		case UNIPAR_FILE_NAME:
		case UNIPAR_PARLIST:
			par->FuncPtr.SetString = (BOOLN (*)(const WChar *)) Func;
			break;
		case UNIPAR_BOOL_ARRAY:
		case UNIPAR_STRING_ARRAY:
		case UNIPAR_NAME_SPEC_ARRAY:
			par->FuncPtr.SetStringArrayElement = (BOOLN (*)(int, WChar *)) Func;
			break;
		case UNIPAR_CFLIST:
			par->FuncPtr.SetCFList = (BOOLN (*)(CFListPtr)) Func;
			break;
		case UNIPAR_PARARRAY:
			if (!*par->valuePtr.pAPtr || !(*par->valuePtr.pAPtr)->parList)
				NotifyError(wxT("%s: Could not set par array handle function."),
				  funcName);
			(*par->valuePtr.pAPtr)->parList->handlePtr.parArray.SetFunc =
			  (BOOLN (*)(ParArrayPtr)) Func;
			break;
		case UNIPAR_ICLIST:
			par->FuncPtr.SetICList = (BOOLN (*)(IonChanListPtr)) Func;
			break;
		case UNIPAR_SIMSCRIPT:
			par->FuncPtr.SetDatumPtr = (BOOLN (*)(DatumPtr)) Func;
			break;
		default:
			NotifyError(wxT("%s: Universal parameter not yet implemented ")
			  wxT("(%d)."), funcName, type);
		}
		break;
	case UNIPAR_SET_CFLIST:
		switch (type) {
		case UNIPAR_INT:
		case UNIPAR_INT_AL:
			par->FuncPtr.SetCFListInt = (BOOLN (*)(CFListPtr, int)) Func;
			break;
		case UNIPAR_REAL:
			par->FuncPtr.SetCFListReal = (BOOLN (*)(CFListPtr, Float)) Func;
			break;
		case UNIPAR_REAL_ARRAY:
			par->FuncPtr.SetCFListRealArrayElement = (BOOLN (*)(CFListPtr, int,
			  Float)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
			par->FuncPtr.SetCFListString = (BOOLN (*)(CFListPtr, const WChar *)) Func;
			break;
		default:
			NotifyError(wxT("%s: Universal parameter (CFList) not yet ")
			  wxT("implemented (%d)."), funcName, type);
		}
		break;
	case UNIPAR_SET_PARARRAY:
		switch (type) {
		case UNIPAR_INT:
			par->FuncPtr.SetParArrayInt = (BOOLN (*)(ParArrayPtr, int)) Func;
			break;
		case UNIPAR_REAL_ARRAY:
		case UNIPAR_REAL_DYN_ARRAY:
			par->FuncPtr.SetParArrayRealArrayElement = (BOOLN (*)(ParArrayPtr,
			  int, Float)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
			par->FuncPtr.SetParArrayString = (BOOLN (*)(ParArrayPtr,
			  const WChar *)) Func;
			break;
		default:
			NotifyError(wxT("%s: Universal parameter (ParArray) not yet ")
			  wxT("implemented (%d)."), funcName, type);
		}
		break;
	case UNIPAR_SET_ICLIST:
		switch (type) {
		case UNIPAR_INT:
		case UNIPAR_INT_AL:
			par->FuncPtr.SetICListInt = (BOOLN (*)(IonChanListPtr, int)) Func;
			break;
		case UNIPAR_REAL:
			par->FuncPtr.SetICListReal = (BOOLN (*)(IonChanListPtr,
			  Float)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
			par->FuncPtr.SetICListString = (BOOLN (*)(IonChanListPtr,
			  const WChar *)) Func;
			break;
		default:
			NotifyError(wxT("%s: Universal parameter (IonChanList) not yet ")
			  wxT("implemented (%d)."), funcName, type);
		}
		break;
	case UNIPAR_SET_IC:
		switch (type) {
		case UNIPAR_INT:
		case UNIPAR_INT_AL:
			par->FuncPtr.SetICInt = (BOOLN (*)(IonChannelPtr, int)) Func;
			break;
		case UNIPAR_REAL:
			par->FuncPtr.SetICReal = (BOOLN (*)(IonChannelPtr, Float)) Func;
			break;
		case UNIPAR_REAL_ARRAY:
			par->FuncPtr.SetICRealArrayElement = (BOOLN (*)(IonChannelPtr, int,
			  Float)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
		case UNIPAR_FILE_NAME:
			par->FuncPtr.SetICString = (BOOLN (*)(IonChannelPtr, const WChar *)) Func;
			break;
		default:
			NotifyError(wxT("%s: Universal parameter (IonChannel) not yet ")
			  wxT("implemented (%d)."), funcName, type);
		}
		break;
	default:
		;
	}

}

/************************ SetPanelListFunction ********************************/

/*
 * Sets the function that returns the panel list for displaying the parameters
 * of a module on different notebook pages.
 */

BOOLN
SetGetPanelListFunc_UniParMgr(UniParListPtr list, NameSpecifier * (* Func)(int))
{
	static const WChar *funcName = wxT("SetPanelListFunc_UniParMgr");

	if (!list) {
		NotifyError(wxT("%s: List not initialised."), funcName);
		return(FALSE);
	}
	list->GetPanelList = Func;
	return(TRUE);

}

/************************ SetAltAbbreviation **********************************/

/*
 * This routine sets an alternative abbreviation for a universal parameter.
 * This facility is to be used for old or changed parameter names which will
 * soon be made obsolete.
 */

void
SetAltAbbreviation_UniParMgr(UniParPtr p, WChar *altAbbr)
{
	static const WChar	*funcName = wxT("SetAltAbbreviation_UniParMgr");

	if (!altAbbr || (*altAbbr == '\0')) {
		NotifyError(wxT("%s: No string given!"), funcName);
		return;
	}
	p->altAbbr = altAbbr;

}

/************************ FormatPar *******************************************/

/*
 * This function formats the full parameter specfication for printing.
 * The string is static and is overwritten each time it is used.
 */

DSAM_API WChar *
FormatPar_UniParMgr(UniParPtr p, WChar *suffix)
{
	static WChar	string[MAXLINE];

	Snprintf_Utility_String(string, MAXLINE, wxT("%s%s"), p->abbr, suffix);
	return(string);

}

/************************ PrintValue ******************************************/

/*
 * Print the value in a universal parameter.
 */

DSAM_API BOOLN
PrintValue_UniParMgr(UniParPtr p)
{
	static const WChar	*funcName = wxT("PrintValue_UniParMgr");

	switch (p->type) {
	case UNIPAR_BOOL:
		DPrint(wxT("%-10s\t"), BooleanList_NSpecLists(*p->valuePtr.i)->name);
		break;
	case UNIPAR_INT:
	case UNIPAR_INT_AL:
		DPrint(wxT("%-10d\t"), *p->valuePtr.i);
		break;
	case UNIPAR_LONG:
		DPrint(wxT("%-10ld\t"), *p->valuePtr.l);
		break;
	case UNIPAR_REAL:
		DPrint(wxT("%-10g\t"), *p->valuePtr.r);
		break;
	case UNIPAR_STRING:
		DPrint(wxT("%-10s\t"), QuotedString_Utility_String(p->valuePtr.s));
		break;
	case UNIPAR_FILE_NAME:
		DPrint(wxT("%-10s\t"), QuotedString_Utility_String(p->valuePtr.file.
		  name));
		break;
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		DPrint(wxT("%-10s\t"), QuotedString_Utility_String(p->valuePtr.nameList.
		  list[*p->valuePtr.nameList.specifier].name));
		break;
	default:
		NotifyError(wxT("%s: Universal parameter not yet implemented (%d)."),
		  funcName, p->type);
		return(FALSE);
	}
	return(TRUE);

}

/************************ FormatArrayString ***********************************/

/*
 * This routine formats the array string.
 */

WChar *
FormatArrayString_UniParMgr(UniParPtr p, int index, WChar *suffix)
{
	static const WChar	*funcName = wxT("FormatArrayString_UniParMgr");
	static WChar	string[MAXLINE];

	switch (p->type) {
	case UNIPAR_BOOL_ARRAY:
		Snprintf_Utility_String(string, MAXLINE, wxT("\t%s\t%3d:%-10s\n"),
		  FormatPar_UniParMgr(p, suffix), index, QuotedString_Utility_String(
				  BooleanList_NSpecLists(index)->name));
		break;
	case UNIPAR_INT_ARRAY:
		Snprintf_Utility_String(string, MAXLINE, wxT("\t%s\t%3d:%-10d\n"),
		  FormatPar_UniParMgr(p, suffix), index, (*p->valuePtr.array.pPtr.i)[
		  index]);
		break;
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
		Snprintf_Utility_String(string, MAXLINE, wxT("\t%s\t%3d:%-10g\n"),
		  FormatPar_UniParMgr(p, suffix), index, (*p->valuePtr.array.pPtr.r)[
		  index]);
		break;
	case UNIPAR_STRING_ARRAY:
		Snprintf_Utility_String(string, MAXLINE, wxT("\t%s\t%3d:%-10s\n"),
		  FormatPar_UniParMgr(p, suffix), index, QuotedString_Utility_String(
		  (*p->valuePtr.array.pPtr.s)[index]));
		break;
	case UNIPAR_NAME_SPEC_ARRAY:
		Snprintf_Utility_String(string, MAXLINE, wxT("\t%s\t%3d:%-10s\n"),
		  FormatPar_UniParMgr(p, suffix), index, QuotedString_Utility_String(
		  p->valuePtr.array.pPtr.nameList.list[(*p->valuePtr.array.pPtr.
		  nameList.specifier)[index]].name));
		break;
	default:
		Snprintf_Utility_String(string, MAXLINE, wxT("%s: Universal parameter ")
		  wxT("not yet implemented (%d)."), funcName, p->type);
	}
	return(string);

}

/************************ PrintPar ********************************************/

/*
 * Print a universal parameter.
 */

BOOLN
PrintPar_UniParMgr(UniParPtr p, WChar *prefix, WChar *suffix)
{
	static const WChar *funcName = wxT("PrintPar_UniParMgr");
	BOOLN	ok = TRUE;
	WChar	string[LONG_STRING];
	DynaListPtr	node;

	if (p == NULL) {
		NotifyError(wxT("%s: Universal parameter not initialised."), funcName);
		return(FALSE);
	}
	switch (p->type) {
	case UNIPAR_BOOL:
	case UNIPAR_INT:
	case UNIPAR_INT_AL:
	case UNIPAR_LONG:
	case UNIPAR_REAL:
	case UNIPAR_STRING:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
	case UNIPAR_FILE_NAME:
		if ((DSAM_strlen(p->abbr) + DSAM_strlen(suffix)) >= LONG_STRING) {
			NotifyError(wxT("%s: Combined string '%s%s' is greater than %d."),
			  funcName, p->abbr, suffix, MAXLINE);
			return(FALSE);
		}
		Snprintf_Utility_String(string, LONG_STRING, wxT("%s%s"), p->abbr,
		  suffix);
		DPrint(wxT("%s%-25s\t"), prefix, string);
		PrintValue_UniParMgr(p);
		DPrint(wxT("%s\n"), p->desc);
		break;
	case UNIPAR_MODULE:
		DPrint(wxT("# Module parameter file: %s\n"),
		  p->valuePtr.module.parFile);
		ok = PrintPars_UniParMgr(p->valuePtr.module.parList,
		  UNIPAR_SUB_PAR_LIST_MARKER, suffix);
		break;
	case UNIPAR_PARLIST: {
		WChar newSuffix[MAXLINE];
		if (!*(p->valuePtr.parList.list))
			break;
		if (p->valuePtr.parList.process)
			SET_PARS_POINTER(*p->valuePtr.parList.process);
		DPrint(wxT("# Sub-parameter list: %s: \n"), p->desc);
		DSAM_strcpy(newSuffix, suffix);
		if (!p->FuncPtr.SetString) {	/* Lowest parList */
			WChar 	newLabel[MAXLINE];
			Snprintf_Utility_String(newLabel, MAXLINE, wxT(".%s"), p->abbr);
			SubStrReplace_Utility_String(newSuffix, UNIPAR_TOP_PARENT_LABEL,
			  newLabel);
		}
		ok = PrintPars_UniParMgr(*(p->valuePtr.parList.list),
		  UNIPAR_SUB_PAR_LIST_MARKER, newSuffix);
		break; }
	case UNIPAR_CFLIST:
		DPrint(wxT("# CFList parameters:\n"));
		if (!PrintPars_UniParMgr((*p->valuePtr.cFPtr)->cFParList,
		  UNIPAR_SUB_PAR_LIST_MARKER, suffix))
			ok = FALSE;
		if ((*p->valuePtr.cFPtr)->bandwidth && !PrintPars_UniParMgr(
		  (*p->valuePtr.cFPtr)->bParList, UNIPAR_SUB_PAR_LIST_MARKER,
		  suffix))
			ok = FALSE;
		break;
	case UNIPAR_PARARRAY:
		DPrint(wxT("# %s parameters:\n"), (*p->valuePtr.pAPtr)->name);
		if (!PrintPars_UniParMgr((*p->valuePtr.pAPtr)->parList,
		  UNIPAR_SUB_PAR_LIST_MARKER, suffix))
			ok = FALSE;
		break;
	case UNIPAR_ICLIST:
		DPrint(wxT("# IonChanList parameters:\n"));
		if (!PrintPars_UniParMgr((*p->valuePtr.iCPtr)->parList,
		  UNIPAR_SUB_PAR_LIST_MARKER, suffix))
			ok = FALSE;
		for (node = (*p->valuePtr.iCPtr)->ionChannels; node; node =
		  node->next) {
			IonChannelPtr iC = (IonChannelPtr) node->data;
			DPrint(wxT("# <---- Ion channel %s ---->\n"), iC->description);
			if (!PrintPars_UniParMgr(iC->parList,
			  UNIPAR_SUB_PAR_LIST_MARKER, suffix))
				ok = FALSE;
		}
		break;
	case UNIPAR_SIMSCRIPT:
		DPrint(wxT("# Parameters for '%s' simulation:\n"),
		  p->valuePtr.simScript.fileName);
		PrintParListModules_Utility_Datum(*p->valuePtr.simScript.simulation,
		  UNIPAR_SUB_PAR_LIST_MARKER);
		break;
	case UNIPAR_BOOL_ARRAY:
	case UNIPAR_INT_ARRAY:
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
	case UNIPAR_STRING_ARRAY:
	case UNIPAR_NAME_SPEC_ARRAY: {
		int		i;
		DPrint(wxT("# %s:\n"), p->desc);
		for (i = 0; i < *p->valuePtr.array.numElements; i++)
			DPrint(wxT("%s"), FormatArrayString_UniParMgr(p, i, suffix));
		break; }
	default:
		NotifyError(wxT("%s: Universal parameter not yet implemented (%d)."),
		  funcName, p->type);
		ok = FALSE;
	}
	return(ok);

}

/************************ PrintPars *******************************************/

/*
 * Print the parameters in a universal parameter list.
 */

BOOLN
PrintPars_UniParMgr(UniParListPtr list, WChar *prefix, WChar *suffix)
{
	static const WChar *funcName = wxT("PrintPars_UniParMgr");
	int		i;

	if (list == NULL) {
		NotifyError(wxT("%s: List not initialised."), funcName);
		return(FALSE);
	}
	for (i = 0; i < list->numPars; i++) {
		if (!PrintPar_UniParMgr(&list->pars[i], prefix, suffix)) {
			NotifyError(wxT("%s: Could not print '%s' parameter."), funcName,
				list->pars[i].abbr);
			return(FALSE);
		}
	}
	return(TRUE);

}

/************************ ResetCFList *****************************************/

/*
 * This routine resets the CFList pointer handle for a universal parameter list.
 * The re-creation of the CFList structure's lists will will ensure that the
 * all the list 'updateFlag' flags will be set to FALSE.
 * The 'bandwidthMode' structure must be preserved, as it will already
 * have been reset.
 */

BOOLN
ResetCFList_UniParMgr(UniParListPtr parList)
{
	static const WChar *funcName = wxT("ResetCFListPointer_UniParMgr");
	CFListPtr	theCFs;

	theCFs = parList->handlePtr.cFs;
	if (!RegenerateList_CFList(theCFs)) {
		NotifyError(wxT("%s: Could not regenerate CFList."), funcName);
		return(FALSE);
	}
	theCFs->bParList->updateFlag = FALSE;
	return(TRUE);

}

/************************ CheckParList ****************************************/

/*
 * This routine checks the universal parameter list's 'updateFlag' and tries to
 * update the associate parameter list handle if necessary.
 * It does this recursively for modules or structures such as CFLists.
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
CheckParList_UniParMgr(UniParListPtr list)
{
	static const WChar *funcName = wxT("CheckParList_UniParMgr");
	BOOLN	ok = TRUE;
	int		i;
	UniParPtr	p;

	if (list == NULL) {
		NotifyError(wxT("%s: List not initialised."), funcName);
		return(FALSE);
	}
	switch (list->mode) {
	case UNIPAR_SET_GENERAL:
	case UNIPAR_SET_SIMSPEC:
		for (i = 0; i < list->numPars; i++) {
			p = &list->pars[i];
			switch (p->type) {
			case UNIPAR_MODULE:
				ok = CheckParList_UniParMgr(p->valuePtr.module.parList);
				break;
			case UNIPAR_PARLIST:
				if (!*(p->valuePtr.parList.list))
					break;
				if (p->valuePtr.parList.process)
					SET_PARS_POINTER(*p->valuePtr.parList.process);
				ok = CheckParList_UniParMgr(*(p->valuePtr.parList.list));
				break;
			case UNIPAR_CFLIST:
				if (!*p->valuePtr.cFPtr) {
					ok = FALSE;
					break;
				}
				if (!CheckParList_UniParMgr((*p->valuePtr.cFPtr)->cFParList))
					ok = FALSE;
				if (!CheckParList_UniParMgr((*p->valuePtr.cFPtr)->bParList))
					ok = FALSE;
				break;
			case UNIPAR_PARARRAY:
				if (!*p->valuePtr.pAPtr) {
					ok = FALSE;
					break;
				}
				if (!CheckParList_UniParMgr((*p->valuePtr.pAPtr)->parList))
					ok = FALSE;
				break;
			case UNIPAR_ICLIST:
				if (!*p->valuePtr.iCPtr || !CheckParList_UniParMgr(
				  (*p->valuePtr.iCPtr)->parList)) {
					ok = FALSE;
					break;
				}
				break;
			default:
				;
			}
		}
		break;
	case UNIPAR_SET_CFLIST:
		if (list->updateFlag)
			ok = ResetCFList_UniParMgr(list);
		break;
	case UNIPAR_SET_PARARRAY:
		break;
	case UNIPAR_SET_ICLIST: {
		DynaListPtr	node;
		IonChanListPtr	theICs = list->handlePtr.iCs;
		IonChannelPtr	iC;

		if (list->updateFlag)
			ok = PrepareIonChannels_IonChanList(theICs);
		for (node = theICs->ionChannels; ok && node; node = node->next) {
			iC = (IonChannelPtr) node->data;
			if (iC->parList->updateFlag)
				ok = ResetIonChannel_IonChanList(theICs, iC);
		}
		break; }
	default:
		;
	}
	list->updateFlag = !ok;
	return(ok);

}

/************************ GetParString ****************************************/

/*
 * This function returns a pointer to string version of a universal parameter.
 */

DSAM_API WChar *
GetParString_UniParMgr(UniParPtr p)
{
	static const WChar *funcName = wxT("GetParString_UniParMgr");
	static WChar string[LONG_STRING];

	if (p == NULL) {
		NotifyError(wxT("%s: Parameter not initialised."), funcName);
		return(NULL);
	}
	if (((p->type == UNIPAR_INT_ARRAY) || (p->type == UNIPAR_REAL_ARRAY) ||
	  (p->type == UNIPAR_REAL_DYN_ARRAY) || (p->type == UNIPAR_STRING_ARRAY) ||
	  (p->type == UNIPAR_NAME_SPEC_ARRAY)) && (*p->valuePtr.array.numElements ==
	  0))
		return(wxT(""));

	switch (p->type) {
	case UNIPAR_BOOL:
		DSAM_strncpy(string, BooleanList_NSpecLists(*p->valuePtr.i)->name,
		  LONG_STRING);
		break;
	case UNIPAR_BOOL_ARRAY:
		DSAM_strncpy(string, BooleanList_NSpecLists(p->valuePtr.array.
		  index)->name, LONG_STRING);
		break;
	case UNIPAR_INT:
	case UNIPAR_INT_AL:
		DSAM_snprintf(string, LONG_STRING, wxT("%d"), *p->valuePtr.i);
		break;
	case UNIPAR_INT_ARRAY:
		DSAM_snprintf(string, LONG_STRING, wxT("%d"), (*p->valuePtr.array.pPtr.
		  i)[p->valuePtr.array.index]);
		break;
	case UNIPAR_LONG:
		DSAM_snprintf(string, LONG_STRING, wxT("%ld"), *p->valuePtr.l);
		break;
	case UNIPAR_REAL:
		DSAM_snprintf(string, LONG_STRING, wxT("%g"), *p->valuePtr.r);
		break;
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
		DSAM_snprintf(string, LONG_STRING, wxT("%g"), (*p->valuePtr.array.pPtr.
		  r)[p->valuePtr.array.index]);
		break;
	case UNIPAR_STRING:
		DSAM_strncpy(string, p->valuePtr.s, LONG_STRING);
		break;
	case UNIPAR_STRING_ARRAY:
		DSAM_strncpy(string, (*p->valuePtr.array.pPtr.s)[p->valuePtr.array.
		  index], LONG_STRING);
		break;
	case UNIPAR_FILE_NAME:
		DSAM_strncpy(string, p->valuePtr.file.name, LONG_STRING);
		break;
	case UNIPAR_MODULE:
		DSAM_strncpy(string, p->valuePtr.module.parFile, LONG_STRING);
		break;
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		DSAM_strncpy(string, p->valuePtr.nameList.list[*p->valuePtr.nameList.
		  specifier].name, LONG_STRING);
		break;
	case UNIPAR_NAME_SPEC_ARRAY:
		DSAM_strncpy(string, p->valuePtr.array.pPtr.nameList.list[*p->valuePtr.
		  array.pPtr.nameList.specifier[p->valuePtr.array.index]].name,
		  LONG_STRING);
		break;
	default:
		NotifyError(wxT("%s: Universal parameter not yet implemented (%d)."),
		  funcName, p->type);
		return(wxT(""));
	}
	return(string);

}

/************************ ParseArrayValue *************************************/

/*
 * This function parses a value, separating the string into index, parameter
 * values if the UNIPAR_INDEX_SEPARATOR appears in the string.
 * The values are returned in the respective arguments.
 * The index is set to the parameter's index field if no index is specified.
 * The function returns FALSE if it fails in any way.
 */

BOOLN
ParseArrayValue_UniParMgr(UniParPtr par, const WChar *parValue, WChar **parValuePtr,
  int *index)
{
	static const WChar *funcName = wxT("ParseArrayValue_UniParMgr");
	WChar	*p, *s, string[MAXLINE];
	size_t	i, pos;

	if (!par) {
		NotifyError(wxT("%s: Universal parameter not initalised.\n"), funcName);
		return(FALSE);
	}
	if ((par->type != UNIPAR_BOOL_ARRAY) && (par->type != UNIPAR_INT_ARRAY) &&
	  (par->type != UNIPAR_REAL_ARRAY) && (par->type != UNIPAR_STRING_ARRAY) &&
	  (par->type != UNIPAR_NAME_SPEC_ARRAY) && (par->type != UNIPAR_REAL_DYN_ARRAY)) {
		NotifyError(wxT("%s: Universal parameter is not an array type (%d).\n"),
		  funcName, par->type);
		return(FALSE);
	}
	index[0] = par->valuePtr.array.index;
	for (i = 0, s = (WChar *) parValue; (i < UNIPAR_MAX_ARRAY_INDEX) && (p = DSAM_strchr(
	  s, UNIPAR_INDEX_SEPARATOR)); i++) {
		pos = p - s;
		DSAM_strncpy(string, s, pos);
		string[pos] = '\0';
		index[i] = DSAM_atoi(string);
		s = p + 1;
	}
	if ((par->type != UNIPAR_REAL_DYN_ARRAY) && (index[0] >=
	  *par->valuePtr.array.numElements)) {
		NotifyError(wxT("%s: Index range must be 0 - %d for '%s' (%d)."),
		  funcName, *par->valuePtr.array.numElements - 1, par->abbr, index[0]);
		return(FALSE);
	}
	*parValuePtr = s;
	if (i == 0)
		index[0] = par->valuePtr.array.index;
	return(TRUE);

}

/************************ SetGeneralParValue **********************************/

/*
 * Set a general parameter in a universal parameter list, where the value is in
 * string form.
 */

BOOLN
SetGeneralParValue_UniParMgr(UniParListPtr parList, uInt index, const WChar *parValue)
{
	static const WChar *funcName = wxT("SetGeneralParValue_UniParMgr");
	BOOLN	ok = FALSE;
	WChar	*arrayValue;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	UniParPtr	p;

	p = &parList->pars[index];
	switch (p->type) {
	case UNIPAR_INT:
	case UNIPAR_INT_AL:
		ok = (* p->FuncPtr.SetInt)(DSAM_atoi(parValue));
		break;
	case UNIPAR_INT_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError(wxT("%s: Could not set array value."), funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetIntArrayElement)(arrayIndex[0], DSAM_atoi(
		  arrayValue));
		break;
	case UNIPAR_LONG:
		ok = (* p->FuncPtr.SetLong)(DSAM_atol(parValue));
		break;
	case UNIPAR_REAL:
		ok = (* p->FuncPtr.SetReal)(DSAM_atof(parValue));
		break;
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError(wxT("%s: Could not set array value."), funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetRealArrayElement)(arrayIndex[0], DSAM_atof(
		  arrayValue));
		break;
	case UNIPAR_BOOL_ARRAY:
	case UNIPAR_STRING_ARRAY:
	case UNIPAR_NAME_SPEC_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError(wxT("%s: Could not set array value."), funcName);
			return(FALSE);
		}
		RemoveChar_Utility_String(arrayValue, '"');
		ok = (* p->FuncPtr.SetStringArrayElement)(arrayIndex[0], arrayValue);
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
	case UNIPAR_FILE_NAME:
		ok = (* p->FuncPtr.SetString)(parValue);
		break;
	case UNIPAR_CFLIST:
		ok = (* p->FuncPtr.SetCFList)((CFListPtr) parValue);
		break;
	case UNIPAR_ICLIST:
		ok = (* p->FuncPtr.SetICList)((IonChanListPtr) parValue);
		break;
	case UNIPAR_PARARRAY:
		break;
	default:
		NotifyError(wxT("%s: Universal parameter type not yet implemented ")
		  wxT("(%d)."), funcName, p->type);
		ok = FALSE;
	}
	return(ok);

}

/************************ SetCFListParValue ***********************************/

/*
 * Set a CFList parameter in a universal parameter list, where the value is in
 * string form.
 */

BOOLN
SetCFListParValue_UniParMgr(UniParListPtr *parList, uInt index, const WChar *parValue)
{
	static const WChar *funcName = wxT("SetCFListParValue_UniParMgr");
	BOOLN	ok = FALSE;
	WChar	*arrayValue;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	UniParPtr	p;

	p = &(*parList)->pars[index];
	switch (p->type) {
	case UNIPAR_INT:
	case UNIPAR_INT_AL:
		ok = (* p->FuncPtr.SetCFListInt)((*parList)->handlePtr.cFs, DSAM_atoi(
		  parValue));
		break;
	case UNIPAR_REAL:
		ok = (* p->FuncPtr.SetCFListReal)((*parList)->handlePtr.cFs, DSAM_atof(
		  parValue));
		break;
	case UNIPAR_REAL_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError(wxT("%s: Could not set array value."), funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetCFListRealArrayElement)(
		  (*parList)->handlePtr.cFs, arrayIndex[0], DSAM_atof(arrayValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		ok = (* p->FuncPtr.SetCFListString)((*parList)->handlePtr.cFs,
		  parValue);
		break;
	default:
		NotifyError(wxT("%s: Universal parameter type not yet implemented ")
		  wxT("(%d)."), funcName, p->type);
		ok = FALSE;
	}
	if (ok && (*parList)->updateFlag)
		ok = RegenerateList_CFList((*parList)->handlePtr.cFs);
	return(ok);

}

/************************ SetParArrayParValue *********************************/

/*
 * Set a ParArray parameter in a universal parameter list, where the value is
 * in string form.
 */

BOOLN
SetParArrayParValue_UniParMgr(UniParListPtr *parList, uInt index,
  const WChar *parValue)
{
	static const WChar *funcName = wxT("SetParArrayParValue_UniParMgr");
	BOOLN	ok = FALSE;
	WChar	*arrayValue;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	UniParPtr	p;

	p = &(*parList)->pars[index];
	switch (p->type) {
	case UNIPAR_INT:
		ok = (* p->FuncPtr.SetParArrayInt)((*parList)->handlePtr.parArray.ptr,
		  DSAM_atoi(parValue));
		break;
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError(wxT("%s: Could not set array value."), funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetParArrayRealArrayElement)(
		  (*parList)->handlePtr.parArray.ptr, arrayIndex[0], DSAM_atof(
		  arrayValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		ok = (* p->FuncPtr.SetParArrayString)((*parList)->handlePtr.parArray.
		  ptr, parValue);
		break;
	default:
		NotifyError(wxT("%s: Universal parameter type not yet implemented ")
		  wxT("(%d)."), funcName, p->type);
		ok = FALSE;
	}
	if (ok)
		(* (*parList)->handlePtr.parArray.SetFunc)((*parList)->handlePtr.
		  parArray.ptr);
	return(ok);

}

/************************ SetCurrentIC ****************************************/

/*
 * This routine sets the IC index for an IC list structure.
 * It assumes that the arguments are correctly initialised.
 * It returns FALSE if the ion channel is not found and there are no free un-set
 * ion channels which it can use.
 */

BOOLN
SetCurrentIC_UniParMgr(IonChanListPtr theICs, const WChar *description)
{
	DynaListPtr	node;
	IonChannelPtr	iC;

	for (node = theICs->ionChannels; node; node = node->next) {
		iC = (IonChannelPtr) node->data;
		if ((iC->description[0] == '\0') || (StrNCmpNoCase_Utility_String(
		  iC->description, description) == 0)) {
			theICs->currentIC = iC;
			return(TRUE);
		}
	}
	return(FALSE);

}

/************************ SetICParValue ***************************************/

/*
 * Set a Ion channel parameter in a universal parameter list, where the
 * value is in string form.
 */

BOOLN
SetICParValue_UniParMgr(UniParListPtr parList, uInt index, const WChar *parValue)
{
	static const WChar *funcName = wxT("SetICParValue_UniParMgr");
	BOOLN	ok = FALSE;
	WChar	*arrayValue;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	UniParPtr	p;
	IonChanListPtr	theICs = parList->handlePtr.iCs;

	p = &parList->pars[index];
	switch (p->type) {
	case UNIPAR_INT:
	case UNIPAR_INT_AL:
		ok = (* p->FuncPtr.SetICInt)(theICs->currentIC, DSAM_atoi(parValue));
		break;
	case UNIPAR_REAL:
		ok = (* p->FuncPtr.SetICReal)(theICs->currentIC, DSAM_atof(parValue));
		break;
	case UNIPAR_REAL_ARRAY:
		if (!theICs->currentIC) {
			NotifyError(wxT("%s: Current IC not set but must be set first."),
			  funcName);
			return(FALSE);
		}
		if (!ParseArrayValue_UniParMgr(&theICs->currentIC->parList->pars[index],
		  parValue, &arrayValue, arrayIndex)) {
			NotifyError(wxT("%s: Could not set array value."), funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetICRealArrayElement)(theICs->currentIC,
		  arrayIndex[0], DSAM_atof(arrayValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
	case UNIPAR_FILE_NAME:
		if ((index == ICLIST_IC_DESCRIPTION) && !SetCurrentIC_UniParMgr(theICs,
		  parValue)) {
			NotifyError(wxT("%s: Could not find '%s' ion channel."), funcName,
			  parValue);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetICString)(theICs->currentIC, parValue);
		break;
	default:
		NotifyError(wxT("%s: Universal parameter type not yet implemented ")
		  wxT("(%d)."), funcName, p->type);
		ok = FALSE;
	}
	return(ok);

}

/************************ SetICListParValue ***********************************/

/*
 * Set a Ion channel List parameter in a universal parameter list, where the
 * value is in string form.
 */

BOOLN
SetICListParValue_UniParMgr(UniParListPtr *parList, uInt index, const WChar *parValue)
{
	static const WChar *funcName = wxT("SetICListParValue_UniParMgr");
	BOOLN	ok = FALSE;
	UniParPtr	p = &(*parList)->pars[index];
	IonChanListPtr	theICs = (*parList)->handlePtr.iCs;

	switch (p->type) {
	case UNIPAR_INT:
	case UNIPAR_INT_AL:
		ok = (* p->FuncPtr.SetICListInt)(theICs, DSAM_atoi(parValue));
		break;
	case UNIPAR_REAL:
		ok = (* p->FuncPtr.SetICListReal)(theICs, DSAM_atof(parValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		ok = (* p->FuncPtr.SetICListString)(theICs, parValue);
		break;
	default:
		NotifyError(wxT("%s: Universal parameter type not yet implemented ")
		  wxT("(%d)."), funcName, p->type);
		ok = FALSE;
	}
	if (ok && (*parList)->updateFlag)
		ok = PrepareIonChannels_IonChanList(theICs);
	return(ok);

}

/************************ SetParValue *****************************************/

/*
 * Set a parameter in a universal parameter list, where the value is in string
 * form.
 */

DSAM_API BOOLN
SetParValue_UniParMgr(UniParListPtr *parList, uInt index, const WChar *parValue)
{
	static const WChar *funcName = wxT("SetParValue_UniParMgr");
	BOOLN	ok = TRUE;

	if ((*parList) == NULL) {
		NotifyError(wxT("%s: universal parameter not initialised."), funcName);
		return(FALSE);
	}
	switch ((*parList)->mode) {
	case UNIPAR_SET_GENERAL:
	case UNIPAR_SET_SIMSPEC:
		ok = SetGeneralParValue_UniParMgr(*parList, index, parValue);
		break;
	case UNIPAR_SET_CFLIST:
		ok = SetCFListParValue_UniParMgr(parList, index, parValue);
		break;
	case UNIPAR_SET_PARARRAY:
		ok = SetParArrayParValue_UniParMgr(parList, index, parValue);
		break;
	case UNIPAR_SET_IC:
		ok = SetICParValue_UniParMgr(*parList, index, parValue);
		break;
	case UNIPAR_SET_ICLIST:
		ok = SetICListParValue_UniParMgr(parList, index, parValue);
		break;
	default:
		NotifyError(wxT("%s: Universal parameter mode not yet implemented ")
		  wxT("(%d)."), funcName, (*parList)->mode);
		ok = FALSE;
	}
	(*parList)->updateFlag = ok;
	return(ok);

}

/*************************** SetRealParValue **********************************/

/*
 * This function calls the converts a real value to a string then calls the
 * 'SetParValue_' routine.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealParValue_UniParMgr(UniParListPtr *parList, uInt index, Float parValue)
{
	WChar	stringValue[MAXLINE];

	DSAM_snprintf(stringValue, MAXLINE, wxT("%g"), parValue);
	return(SetParValue_UniParMgr(parList, index, stringValue));

}

/************************ Cmp *************************************************/

/*
 * This function compares a universal parameter name or type with the respective
 * search item, according to the mode.
 * It returns values similar to 'strcmp': It returns an integer less than,
 * equal to, or greater than zero if the value is found, respectively, to be
 * less than, to match, or be greater than the search item.
 */

int
Cmp_UniParMgr(UniParPtr p, void *item, UniParSearchSpecifier mode)
{
	switch (mode) {
	case UNIPAR_SEARCH_ABBR:
		if (StrNCmpNoCase_Utility_String(p->abbr, (WChar *) item) == 0)
			return(0);
		if (p->altAbbr)
			return(StrNCmpNoCase_Utility_String(p->altAbbr, (WChar *) item));
		break;
	case UNIPAR_SEARCH_TYPE:
		return(p->type - *((int *) item));
	}
	return(-1);

}

/************************ FindUniPar ******************************************/

/*
 * This function returns a pointer to a universal parameter specified by an
 * abbreviation within a list.
 * It searches recursively down lists for 'UNIPAR_MODULE' types.
 * It searches recursively down lists for 'UNIPAR_PARLIST' types.
 * It searches through simulation scripts, i.e. 'UNIPAR_SIMSCRIPT' types.
 * It assumes that 'parName' points to a valid string.
 * It removes any strings after a period, wxT(".") including the period.
 * It returns NULL if it fails in any way.
 * The function sets the argument 'parList' so the recursive searches return
 * the correct parList.
 */

DSAM_API UniParPtr
FindUniPar_UniParMgr(UniParListPtr *parList, const WChar *parName,
  UniParSearchSpecifier mode)
{
	static const WChar *funcName = wxT("FindUniPar_UniParMgr");
	int		i;
	UniParPtr	p, par = NULL;
	UniParListPtr	tempParList;

	if (*parList == NULL) {
		NotifyError(wxT("%s: Parameter list not initialised."), funcName);
		return(NULL);
	}
	for (i = 0; (i < (*parList)->numPars) && !par; i++) {
		p = &(*parList)->pars[i];
		switch (p->type) {
		case UNIPAR_MODULE:
			if (Cmp_UniParMgr(p, (WChar *) parName, mode) == 0) {
				par = p;
				break;
			}
			tempParList = p->valuePtr.module.parList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName, mode)) !=
			  NULL) {
				*parList = tempParList;
				break;	/* here in case anything is added under this. */
			}
		   break;
		case UNIPAR_PARLIST:
			if (p->valuePtr.parList.process)
				SET_PARS_POINTER(*p->valuePtr.parList.process);
			if (Cmp_UniParMgr(p, (WChar *) parName, mode) == 0) {
				par = p;
				break;
			}
			if ((tempParList = *(p->valuePtr.parList.list)) == NULL)
				break;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName, mode)) !=
			  NULL) {
				*parList = tempParList;
				break;	/* here in case anything is added under this. */
			}
		   break;
		case UNIPAR_CFLIST:
			if (Cmp_UniParMgr(p, (WChar *) parName, mode) == 0) {
				par = p;
				break;
			}
			if (!*p->valuePtr.cFPtr)
				return(NULL);
			tempParList = (*p->valuePtr.cFPtr)->cFParList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName, mode)) !=
			  NULL) {
				*parList = tempParList;
				break;
			}
			tempParList = (*p->valuePtr.cFPtr)->bParList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName, mode)) !=
			  NULL) {
				*parList = tempParList;
				break;
			}
			break;
		case UNIPAR_PARARRAY:
			if (Cmp_UniParMgr(p, (WChar *) parName, mode) == 0) {
				par = p;
				break;
			}
			if (!*p->valuePtr.pAPtr)
				break;
			tempParList = (*p->valuePtr.pAPtr)->parList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName, mode)) !=
			  NULL) {
				*parList = tempParList;
				break;
			}
			break;
		case UNIPAR_ICLIST: {
			DynaListPtr	node;
			IonChanListPtr	theICs = *p->valuePtr.iCPtr;

			if (Cmp_UniParMgr(p, (WChar *) parName, mode) == 0) {
				par = p;
				break;
			}
			if (!theICs)
				return(NULL);
			tempParList = theICs->parList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName, mode)) !=
			  NULL) {
				*parList = tempParList;
				break;
			}
			if (!theICs->ionChannels) {
				if (!PrepareIonChannels_IonChanList(theICs)) {
					NotifyError(wxT("%s: Could not set the ion channels."),
					  funcName);
					return(FALSE);
				}
			}
			for (node = theICs->ionChannels; node; node =
			  node->next) {
				tempParList = ((IonChannelPtr) node->data)->parList;
				if ((par = FindUniPar_UniParMgr(&tempParList, parName, mode)) !=
				  NULL) {
					*parList = tempParList;
					break;
				}
			}
			break; }
		case UNIPAR_SIMSCRIPT: {
			uInt	index;
			DatumPtr	pc = *p->valuePtr.simScript.simulation;

			tempParList = *parList;
			if (FindModuleUniPar_Utility_Datum(&tempParList, &index, &pc,
			  parName, TRUE) && (index < tempParList->numPars)) {
				par = &tempParList->pars[index];
				break;
			}
			break; }
		default:
			if (Cmp_UniParMgr(p, (WChar *) parName, mode) == 0) {
				par = p;
				break;
			}
		}
	}
	return(par);

}

/****************************** PrintSubParList *******************************/

/*
 * This routine prints the names and values for a sub parameter list, using
 * the format used for reading in parameters using simulation files.
 */

void
PrintSubParList_UniParMgr(UniParListPtr parList)
{
	int		i;
	UniParPtr	par;

	if (!parList)
		return;
	for (i = 0; i < parList->numPars; i++) {
		par = &parList->pars[i];
		if (par->enabled)
			PrintPar_UniParMgr(par, wxT(""), wxT(""));
	}
}

/****************************** PrintParList **********************************/

/*
 * This routine prints the names and values for a parameter list, using the
 * format used for reading in parameters using simulation files.
 */

DSAM_API BOOLN
PrintParList_UniParMgr(UniParListPtr parList)
{
	static WChar *funcName = wxT("PrintParList_UniParMgr");
	int		i;

	if (!parList) {
		NotifyError(wxT("%s: Parameter list not initialised."), funcName);
		return(FALSE);
	}
	switch (parList->mode) {
	case UNIPAR_SET_GENERAL:
	case UNIPAR_SET_SIMSPEC:
	case UNIPAR_SET_PARARRAY:
		for (i = 0; i < parList->numPars; i++)
			if (parList->pars[i].enabled)
				PrintPar_UniParMgr(&parList->pars[i], wxT(""), wxT(""));
		break;
	case UNIPAR_SET_CFLIST: {
		CFListPtr	theCFs = parList->handlePtr.cFs;
		DPrint(wxT("\n#CF List Parameters:-\n"));
		PrintSubParList_UniParMgr(theCFs->cFParList);
		DPrint(wxT("\n"));
		PrintSubParList_UniParMgr(theCFs->bParList);
		break; }
	case UNIPAR_SET_ICLIST: {
		DynaListPtr	node;
		IonChanListPtr	theICs = parList->handlePtr.iCs;
		IonChannelPtr	iC;
		DPrint(wxT("\n#IC List Parameters:-\n"));
		for (node = theICs->ionChannels; node; node = node->next) {
			iC = (IonChannelPtr) node->data;
			PrintSubParList_UniParMgr(iC->parList);
			DPrint(wxT("\n"));
		}
		break; }
	default:
		NotifyError(wxT("%s: Mode '%d' not implemented."), funcName, parList->
		  mode);
	}
	return(TRUE);

}

/****************************** ResizeDoubleArray *****************************/

/*
 * This routine resizes an array.
 */

BOOLN
ResizeFloatArray_UniParMgr(Float **array, int *oldLength, int newLength)
{
	static const WChar *funcName = wxT("ResizeFloatArray_UniParMgr");
	register Float	*newArray, *oldArray;
	int		i, length;
	Float	*savedArray = NULL;

	if (newLength == *oldLength)
		return(TRUE);
	if (*array)
		savedArray = *array;
	if ((*array = (Float *) calloc(newLength, sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' selectionArray."),
		  funcName, newLength);
		return(FALSE);
	}
	if (savedArray) {
		newArray = *array;
		oldArray = savedArray;
		length = MINIMUM(*oldLength, newLength);
		for (i = 0; i < length; i++)
			*newArray++ = *oldArray++;
		free(savedArray);
	}
	*oldLength = newLength;
	return(TRUE);
}

/****************************** WriteParFile **********************************/

/*
 * This function writes a parameter file from a parList.
 * It assumes that the parList had been correctly initialised.
 */

BOOLN
WriteParFile_UniParMgr(WChar *fileName, UniParListPtr parList)
{
	static const WChar *funcName = wxT("WriteParFile_UniParMgr");
	FILE *oldFp = GetDSAMPtr_Common()->parsFile;

	if (!SetParsFile_Common(fileName, OVERWRITE)) {
		NotifyError(wxT("%s: Could not open parameter file '%s' for writing."),
		  funcName, fileName);
		return(FALSE);
	}
	PrintParList_UniParMgr(parList);
	fclose(GetDSAMPtr_Common()->parsFile);
	GetDSAMPtr_Common()->parsFile = oldFp;
	return(TRUE);

}
