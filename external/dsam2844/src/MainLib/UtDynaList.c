/**********************
 *
 * File:		UtDynaList.c
 * Purpose:		This module contains the general dynamic list management code.
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GeCommon.h"
#include "UtDynaList.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/


/****************************** GetLastInst ***********************************/

/*
 * This routine returns a pointer to the last DynaList instruction in a
 * simulation instruction list.
 */

DynaListPtr
GetLastInst_Utility_DynaList(DynaListPtr head)
{
	DynaListPtr	p;

	if (head == NULL)
		return(NULL);
	for (p = head; p->next != NULL; p = p->next)
		;
	return(p);

}

/****************************** Append ****************************************/

/*
 * Add a new node to the end of the list.
 * The node must initially be set to NULL.  If NULL is passed as the
 * 'nodePtr' value, then the node returned will be the head of the list.
 * A NULL is returned if it fails in any way, otherwise it returns a pointer
 * to the new node.
 */

DSAM_API DynaListPtr
Append_Utility_DynaList(DynaListPtr *nodePtr, void *data)
{
	static const WChar *funcName = wxT("Append_Utility_DynaList");
	DynaListPtr	newNode;

	if (nodePtr && *nodePtr && (*nodePtr)->next)
		return(Append_Utility_DynaList(&(*nodePtr)->next, data));
	if ((newNode = (DynaListPtr) malloc(sizeof (DynaList))) == NULL) {
		NotifyError(wxT("%s: Out of memory for DynaList."), funcName);
		return(NULL);
	}
	newNode->data = data;
	if (!nodePtr) {
		newNode->previous = NULL;
		newNode->next = NULL;
		return(newNode);
	}
	newNode->previous = *nodePtr;
	if (newNode->previous) {
		newNode->next = (*nodePtr)->next;
		(*nodePtr)->next = newNode;
	} else {
		newNode->next = NULL;
		*nodePtr = newNode;
	}
	return (newNode);

}

/****************************** Insert ****************************************/

/*
 * Add a new node to the top of the list.
 * The node must initially be set to NULL.  If NULL is poassed as the
 * 'nodePtr' value, then the node returned will be the head of the list.
 * A NULL is returned if it fails in any way, otherwise it returns a pointer
 * to the new node.
 */

DSAM_API DynaListPtr
Insert_Utility_DynaList(DynaListPtr *nodePtr, void *data)
{
	static const WChar *funcName = wxT("Insert_Utility_DynaList");
	DynaListPtr	newNode;

	if ((newNode = (DynaListPtr) malloc(sizeof (DynaList))) == NULL) {
		NotifyError(wxT("%s: Out of memory for DynaList."), funcName);
		return(NULL);
	}
	newNode->data = data;
	newNode->previous = NULL;
	newNode->next = *nodePtr;
	*nodePtr = newNode;
	if (newNode->next)
		newNode->next->previous = newNode;
	return (newNode);

}

/****************************** Pull ******************************************/

/*
 * This routine returns the data at the top of a list, then removes the node
 * from the list.  It assumes that it is passed the top of the list.
 */

void *
Pull_Utility_DynaList(DynaListPtr *nodePtr)
{
	static const WChar *funcName = wxT("Pull_Utility_DynaList");
	void *data;
	DynaListPtr	ptr;

	if (!*nodePtr) {
		NotifyError(wxT("%s: Attempt to 'pull' from empty list."), funcName);
		return (NULL);
	}
	ptr = *nodePtr;
	data = ptr->data;
	*nodePtr = ptr->next;
	if (*nodePtr)
		(*nodePtr)->previous = ptr->previous;
	free(ptr);
	return(data);

}

/****************************** Remove ****************************************/

/*
 * This routine removes a node from a list.
 */

DSAM_API BOOLN
Remove_Utility_DynaList(DynaListPtr *list, DynaListPtr ptr)
{
	static const WChar *funcName = wxT("Remove_Utility_DynaList");

	if (!*list) {
		NotifyError(wxT("%s: List is empty."), funcName);
		return (FALSE);
	}
	if (!ptr) {
		NotifyError(wxT("%s: Node is NULL."), funcName);
		return (FALSE);
	}
	if (!ptr->previous) {
		*list = ptr->next;
		if (ptr->next)
			ptr->next->previous = NULL;
	} else {
		ptr->previous->next = ptr->next;
		if (ptr->next)
			ptr->next->previous = ptr->previous;
	}
	free(ptr);
	return(TRUE);

}

/****************************** GetMemberData *********************************/

/*
 * This routine returns the data pointer for a specific node position in the
 * list.
 * It returns FALSE if it fails in any way.
 */

DSAM_API void *
GetMemberData_Utility_DynaList(DynaListPtr list, int index)
{
	static const WChar *funcName = wxT("GetMemberData_Utility_DynaList");
	int		count;

	count = 0;
	while (list) {
		if (count == index)
			return(list->data);
		list = list->next;
		count++;
	}
	NotifyError(wxT("%s: Could not find list member %d of %d.\n"), funcName,
	  index, count);
	return(NULL);

}

/****************************** FreeList **************************************/

/*
 * This routine removes all nodes from a list.  It does not free the memory
 * for the data.  THe data should either not need deallocation or it should be
 * deallocated elsewhere.
 */

void
FreeList_Utility_DynaList(DynaListPtr *list)
{
	while (*list)
		Pull_Utility_DynaList(list);

}

/****************************** GetNumElements ********************************/

/*
 * This function returns the number of elements in the list.
 */

int
GetNumElements_Utility_DynaList(DynaListPtr list)
{
	int		count = 0;
	DynaListPtr	node;

	for (node = list; node != NULL; node = node->next)
		count++;
	return(count);

}

/****************************** FindElement ***********************************/

/*
 * This routine returns with a specified element from the list
 */

DSAM_API DynaListPtr
FindElement_Utility_DynaList(DynaListPtr start, int (* CmpFunc)(void *,
  void *), void *data)
{
	static const WChar *funcName = wxT("FindElement_Utility_DynaList");
	DynaListPtr	p;

	for (p = start; p != NULL; p = p->next)
		if (CmpFunc(p->data, data) == 0)
			return(p);
	NotifyError(wxT("%s: Element not found."), funcName);
	return(NULL);

}

