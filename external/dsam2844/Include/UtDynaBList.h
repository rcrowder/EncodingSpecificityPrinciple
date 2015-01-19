/**********************
 *
 * File:		UtDynaBList.h
 * Purpose:		This module contains the generic dynamic binary list management
 *				code.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		23 Nov 2000
 * Updated:		
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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

#ifndef _UTDYNABLIST_H
#define _UTDYNABLIST_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct DynaBList {

	void			*data;
	struct DynaBList	*left;
	struct DynaBList	*right;

} DynaBList, *DynaBListPtr;

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

DSAM_API DynaBListPtr	FindElement_Utility_DynaBList(DynaBListPtr nodePtr,
						  int (* CmpFunc)(void *, void *), void *data);

void	FreeList_Utility_DynaBList(DynaBListPtr *nodePtr);

DSAM_API DynaBListPtr	Insert_Utility_DynaBList(DynaBListPtr *nodePtr,
						  int (* CmpFunc)(void *, void *), void *data);

void	PrintList_Utility_DynaBList(DynaBListPtr nodePtr, void (* PrintFunc)(
		  void *));

DSAM_API BOOLN	Remove_Utility_DynaBList(DynaBListPtr *nodePtr,
				  int (* CmpFunc)(void *, void *), void *data);

__END_DECLS

#endif
