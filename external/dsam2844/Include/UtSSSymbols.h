/**********************
 *
 * File:		UtSSSymbols.h
 * Purpose:		This is the header file for the simulation script symbols
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

#ifndef _UTSSSYMOBLS_H
#define _UTSSSYMOBLS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct	Symbol	{	/* Symbol table entry. */

	WChar	*name;
	short	type;			/* IDENTIFIER, KEYWORD */
	struct Symbol	*next;	/* To link to next symbol */

} Symbol, *SymbolPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SymbolPtr	sSSymList;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DSAM_API void		FreeSymbols_Utility_SSSymbols(SymbolPtr *list);

DSAM_API void		InitKeyWords_Utility_SSSymbols(SymbolPtr *list);

DSAM_API SymbolPtr	InstallSymbol_Utility_SSSymbols(SymbolPtr *list, WChar *s,
					  int type);

DSAM_API SymbolPtr	LookUpSymbol_Utility_SSSymbols(SymbolPtr list, WChar *s);

void		PrintSymbols_Utility_SSSymbols(SymbolPtr list);

__END_DECLS

#endif
