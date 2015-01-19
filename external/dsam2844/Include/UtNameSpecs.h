/**********************
 *
 * File:		UtNameSpecs.h
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

#ifndef _UTNAMESPECS_H
#define _UTNAMESPECS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct {

	const WChar	*name;
	int		specifier;

} NameSpecifier, *NameSpecifierPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DSAM_API int		Identify_NameSpecifier(const WChar *name, NameSpecifierPtr list);

void	FreeNameAllocatedList_NameSpecifier(NameSpecifierPtr *list);

int		GetNullSpec_NameSpecifier(NameSpecifierPtr list);

void	PrintList_NameSpecifier(NameSpecifierPtr list);

__END_DECLS

#endif
