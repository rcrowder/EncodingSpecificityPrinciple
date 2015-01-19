/**********************
 *
 * File:		UtSSSymbols.c
 * Purpose:		This is the source file for the simulation script symbols
 *				module.
 * Comments:	23-08-99 LPO: This routine was extracted from the UtSimScript
 *				module.
 * Author:		L. P. O'mard
 * Created:		23 Aug 1999
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtString.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SymbolPtr	sSSymList = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** InitKeyWords **********************************/

/*
 * This routine installs the keywords into the symbol table.
 */

DSAM_API void
InitKeyWords_Utility_SSSymbols(SymbolPtr *list)
{
	int		i;
	static struct {

		WChar	*name;
		int		type;

	} keywords[] = {

		{ wxT("begin"),		BEGIN },
		{ wxT("repeat"),	REPEAT },
		{ wxT("reset"),		RESET },
		{ 0, 0 }

	};

	for (i = 0; keywords[i].name; i++)
		InstallSymbol_Utility_SSSymbols(list, keywords[i].name,
		  keywords[i].type);

}

/****************************** LookUp ****************************************/

/*
 * Find s in symbol table.
 */

DSAM_API SymbolPtr
LookUpSymbol_Utility_SSSymbols(SymbolPtr list, WChar *s)
{
	Symbol	*sp;
	
	for (sp = list; sp != (SymbolPtr) 0; sp = sp->next)
		if (DSAM_strcmp(sp->name, s) == 0)
			return sp;
	return 0;

}

/****************************** InstallSymbol *********************************/

/*
 * Install tokens in symbol table.
 */

DSAM_API SymbolPtr
InstallSymbol_Utility_SSSymbols(SymbolPtr *list, WChar *s, int type)
{
	static const WChar	*funcName = wxT("InstallSymbol_Utility_SSSymbols");
	Symbol	*sp;

	if ((sp = (SymbolPtr) malloc(sizeof (Symbol))) == NULL) {
		NotifyError(wxT("%s: Out of memory for symbol entry."), funcName);
		exit(1);
	}
	sp->name = InitString_Utility_String(s);
	sp->type = type;
	sp->next = *list;		/* put at front of list */
	*list = sp;
	return sp;

}

/****************************** FreeSymbols ***********************************/

/*
 * This routine frees the contents of the Symbol table.
 */

DSAM_API void
FreeSymbols_Utility_SSSymbols(SymbolPtr *list)
{
	SymbolPtr	sp, temp;
	
	for (sp = *list; sp != NULL; ) {
		temp = sp;
		sp = sp->next;
		free(temp->name);
		free(temp);
	}
	*list = NULL;

}

/****************************** PrintSymbols **********************************/

/*
 * This routine prints the contents of a symbol list.
 */

void
PrintSymbols_Utility_SSSymbols(SymbolPtr list)
{
	while (list) {
		DSAM_printf(wxT("\t%s\n"), list->name);
		list = list->next;
	}

}
