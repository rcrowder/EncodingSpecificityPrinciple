/**********************
 *
 * File:		UtNameSpecs.c
 * Purpose:		This module incorporates the unified name specification code
 *				for the use of names to describe functions: name specifiers.
 * Comments:	14-12-96 LPO: The Identify_NameSpecifier routine has been
 *				improved so that it can identify abreviated names.
 * Author:		L. P. O'Mard.
 * Created:		10 Jun 1996
 * Updated:		14 Dec 1996
 * Copyright:	(c) 1998, 2010 Lowel P. O'Mard
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
#include "UtNameSpecs.h"
#include "UtString.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/************************** GetNullSpec ***************************************/

/*
 * This routine returns the null specifier at the end of a list.
 * It assumes that the list is not NULL.
 */

int
GetNullSpec_NameSpecifier(NameSpecifierPtr list)
{
	static WChar *funcName = wxT("GetNullSpec_NameSpecifier");

	for (  ; list->name != NULL; list++)
		;
	if (list->name != NULL) {
		NotifyError(wxT("%s: Incorrectly set up list."), funcName);
		exit(1);
	}
	return(list->specifier);

}

/************************** Identify ******************************************/

/*
 * This routine identifies a name from a supplied list, returning the
 * appropriate number, or zero if it does not find identify the name.
 * All lists are expected to end with the null, "", string.
 * The names in the list are all assumed to be in upper case.
 * It ends terminally (exit(1)) if the list is not initialised.
 */

DSAM_API int
Identify_NameSpecifier(const WChar *name, NameSpecifierPtr list)
{
	static WChar *funcName = wxT("Identify_NameSpecifier");
	size_t	length;

	if (list == NULL) {
		NotifyError(wxT("%s: List not correctly initialised."), funcName);
		exit(1);
	}
	if (name == NULL) {
		NotifyError(wxT("%s: Illegal null name."), funcName);
		return(GetNullSpec_NameSpecifier(list));
	}
	length = DSAM_strlen(name);
	if (!length)
		return(GetNullSpec_NameSpecifier(list));
	for ( ; list->name && StrNCmpNoCase_Utility_String(list->name, name) != 0;
	  list++)
		;
	return(list->specifier);

}

/****************************** FreeNameAllocatedList **************************/

/*
 * This routine frees a list for which the names have been allocated memory.
 * by the creating routine.
 */

void
FreeNameAllocatedList_NameSpecifier(NameSpecifierPtr *list)
{
	NameSpecifierPtr	p;

	if (!*list)
		return;
	for (p = *list; p->name; p++)
		free((WChar *) p->name);
	free(*list);
	*list = NULL;

}

/************************** PrintList *****************************************/

/*
 * This routine lists all of the names in a name specifier list.
 */

void
PrintList_NameSpecifier(NameSpecifierPtr list)
{
	static WChar *funcName = wxT("PrintList_NameSpecifier");

	if (list == NULL) {
		NotifyError(wxT("%s: List not correctly initialised."), funcName);
		exit(1);
	}
	DPrint(wxT("Name specifier list:-\n"));
	for (  ; list->name != NULL; list++)
		DPrint(wxT("\t%s\t(%d)\n"), list->name, list->specifier);

}

