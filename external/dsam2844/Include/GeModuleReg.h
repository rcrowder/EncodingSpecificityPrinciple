/******************
 *
 * File:		GeModuleReg.h
 * Comments:	This module handles the register of all the library and user
 *				process modules.
 *				It is necssary to use a module specifier to identify
 *				non-standard process modules such as SIMSCRIPT_MODULE and the
 *				NULL_MODULE
 * Authors:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		
 * Copyright:	(c) 2001, 2010 Lowel P. O'Mard
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

#ifndef	_MODULEREG_H
#define _MODULEREG_H	1

#include "GeUniParMgr.h"
#include "UtNameSpecs.h"
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	MODULE_REG_DEFAAULT_USER_MODULES	20
#define	NULL_MODULE_NAME		"NULL"

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/*************************** Misc. Macros *************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef struct {

	WChar	*name;
	ModuleClassSpecifier	classSpecifier;
	ModuleSpecifier		specifier;
	BOOLN	(* InitModule )(ModulePtr);

} ModRegEntry, *ModRegEntryPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

int		GetNumUserModules_ModuleReg(void);

ModRegEntryPtr	Identify_ModuleReg(ModRegEntryPtr list, const WChar *name);

BOOLN	InitUserModuleList_ModuleReg(int theMaxUserModules);

void	FreeUserModuleList_ModuleReg(void);

ModRegEntryPtr	GetRegEntry_ModuleReg(const WChar *name);

DSAM_API ModRegEntryPtr	LibraryList_ModuleReg(uShort index);

DSAM_API BOOLN	RegEntry_ModuleReg(const WChar *name, BOOLN (* InitModuleFunc)(
				  ModulePtr));

DSAM_API ModRegEntryPtr	UserList_ModuleReg(uShort index);

__END_DECLS

#endif
