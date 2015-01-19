/**********************
 *
 * File:		UtDynaBList.c
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GeCommon.h"
#include "UtDynaBList.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Insert ****************************************/

/*
 * Add a new node to the binary list.
 * The node must initially be set to NULL.
 * A NULL is returned if it fails in any way, otherwise it returns a pointer
 * to the new node.
 * The 'CmpFunc' is used to compare the node data.  It is expected to return
 * negative, xero and positive integer values when the test comparison is
 * less than, equal and greater than respectively.
 */

DSAM_API DynaBListPtr
Insert_Utility_DynaBList(DynaBListPtr *nodePtr, int (* CmpFunc)(void *, void *),
  void *data)
{
	static const WChar *funcName = wxT("Insert_Utility_DynaBList");
	int		cmpResult;
	DynaBListPtr	newNode;

	if (nodePtr && *nodePtr) {
		cmpResult = CmpFunc(data, (*nodePtr)->data);
		if (cmpResult == 0) {
			NotifyError(wxT("%s: Duplicated data."), funcName);
			return(NULL);
		}
		if (cmpResult > 0)
			return(Insert_Utility_DynaBList(&(*nodePtr)->right, CmpFunc, data));
		else
			return(Insert_Utility_DynaBList(&(*nodePtr)->left, CmpFunc, data));
	}
	if (*nodePtr) {
		NotifyError(wxT("%s: Duplicate label entry. Not inserted."), funcName);
		return(NULL);
	}
	if ((newNode = (DynaBListPtr) malloc(sizeof (DynaBList))) == NULL) {
		NotifyError(wxT("%s: Out of memory for DynaList."), funcName);
		return(NULL);
	}
	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;

	if (nodePtr)
		*nodePtr = newNode;
	return(newNode);

}

/****************************** Remove ****************************************/

/*
 * This routine removes a specified node from the binary list.
 */

DSAM_API BOOLN
Remove_Utility_DynaBList(DynaBListPtr *nodePtr, int (* CmpFunc)(void *, void *),
  void *data)
{
	static const WChar *funcName = wxT("Remove_Utility_DynaBList");
	int		cmpResult;
	DynaBListPtr	p;

	if (!*nodePtr) {
		NotifyError(wxT("%s: Element not found."), funcName);
		return(FALSE);
	}
	cmpResult = CmpFunc(data, (*nodePtr)->data);
	if (cmpResult > 0)
		return(Remove_Utility_DynaBList(&(*nodePtr)->right, CmpFunc, data));
	if (cmpResult < 0)
	 	return(Remove_Utility_DynaBList(&(*nodePtr)->left, CmpFunc, data));
	if ((*nodePtr)->right) {
		for (p = (*nodePtr)->right; p->left != NULL; p = p->left)
			;
		p->left = (*nodePtr)->left;
	} else
		(*nodePtr)->right = (*nodePtr)->left;
	p = *nodePtr;
	*nodePtr = p->right;
	free(p);
	return(TRUE);

}

/****************************** FreeList **************************************/

/*
 * This routine frees the dynamic binary list.
 */

void
FreeList_Utility_DynaBList(DynaBListPtr *nodePtr)
{
	if (!*nodePtr)
		return;
	FreeList_Utility_DynaBList(&(*nodePtr)->left);
	FreeList_Utility_DynaBList(&(*nodePtr)->right);
	free(*nodePtr);
	*nodePtr = NULL;

}

/****************************** PrintList *************************************/

/*
 * This routine traverses the list and prints out the elements in ascending
 * order.
 */

void
PrintList_Utility_DynaBList(DynaBListPtr nodePtr, void (* PrintFunc)(void *))
{
	if (!nodePtr)
		return;
	PrintList_Utility_DynaBList(nodePtr->left, PrintFunc);
	PrintFunc(nodePtr->data);
	PrintList_Utility_DynaBList(nodePtr->right, PrintFunc);

}

/****************************** FindElement ***********************************/

/*
 * This routine returns with a specified element from the list
 */

DSAM_API DynaBListPtr
FindElement_Utility_DynaBList(DynaBListPtr nodePtr, int (* CmpFunc)(void *,
  void *), void *data)
{
	static const WChar *funcName = wxT("FindElement_Utility_DynaBList");
	int		cmpResult;

	if (!nodePtr) {
		NotifyError(wxT("%s: Element not found."), funcName);
		return(NULL);
	}
	cmpResult = CmpFunc(data, nodePtr->data);
	if (cmpResult == 0)
		return(nodePtr);
	if (cmpResult > 0)
		return(FindElement_Utility_DynaBList(nodePtr->right, CmpFunc, data));
	else
		return(FindElement_Utility_DynaBList(nodePtr->left, CmpFunc, data));

}

