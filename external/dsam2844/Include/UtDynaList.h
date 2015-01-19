/**********************
 *
 * File:		UtDynaList.h
 * Purpose:		This module contains the generic dynamic list management code.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		01 Sep 1999
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

#ifndef _UTDYNALIST_H
#define _UTDYNALIST_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct DynaList {

	void			*data;
	struct DynaList	*previous;			/* To link to previous node */
	struct DynaList	*next;				/* To link to next node */

} DynaList, *DynaListPtr;

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

DSAM_API DynaListPtr	Append_Utility_DynaList(DynaListPtr *nodePtr,
						  void *data);

DSAM_API DynaListPtr	FindElement_Utility_DynaList(DynaListPtr start,
						  int (* CmpFunc)(void *, void *), void *data);

void		FreeList_Utility_DynaList(DynaListPtr *nodePtr);

DynaListPtr	GetLastInst_Utility_DynaList(DynaListPtr head);

DSAM_API void *		GetMemberData_Utility_DynaList(DynaListPtr list,
					  int index);

int			GetNumElements_Utility_DynaList(DynaListPtr list);

DSAM_API DynaListPtr	Insert_Utility_DynaList(DynaListPtr *nodePtr,
						  void *data);

void *		Pull_Utility_DynaList(DynaListPtr *nodePtr);

DSAM_API BOOLN		Remove_Utility_DynaList(DynaListPtr *list,
					  DynaListPtr ptr);

__END_DECLS

#endif
