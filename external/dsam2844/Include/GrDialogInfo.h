/**********************
 *
 * File:		GrDialogInfo.h
 * Purpose:		This is a dialog for handling the information for dialogs and
 *				also has a pointer to the dialog.
 * Comments:	14-01-99 LPO: This was revised from the ModuleInfo routine in
 *				the GrSimModuleDiag module.
 * Author:		L. P. O'Mard
 * Created:		14 Jan 1999
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
 
#ifndef _GRDIALOGINFO_H
#define _GRDIALOGINFO_H 1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

/*************************** DialogInfo ***************************************/

class DialogInfo {

	int		specifier;

  public:
 	wxObject	*ptr;
	
	DialogInfo(wxObject *thePtr, int theSpecifier);
	~DialogInfo(void);

	wxObject *	GetPtr(void)	{ return ptr; };
	int		GetSpecifier(void)	{ return specifier; };

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

