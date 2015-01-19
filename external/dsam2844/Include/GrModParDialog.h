/**********************
 *
 * File:		GrModParDialog.h
 * Purpose:		Module parameter handling routines.
 *				This is a dialog for changing the parameters of a module using
 *				the module's parameter list.
 * Comments:	04-10-98 LPO: This module has been set up so that before any
 *				attempt to run the simulation, the parameters in the dialogs
 *				are always checked to see if they have been changed (without
 *				pressing <return>).
 * 				23-10-98 LPO: This Module has been taken from the GrSimMgr
 *				module which used to contain both this and the SimModuleDialog
 *				29-01-99 LPO: The 	ResetGUIDialogs() is used by the
 *				'TextProc', 'ButtonProc' and 'ListProc' routines, to ensure
 *				that error messages are given as dialogs.
 * Author:		L. P. O'Mard
 * Created:		01 Oct 1998
 * Updated:		29 Jan 1999
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
 
#ifndef _GRMODPARDIALOG_H
#define _GRMODPARDIALOG_H 1

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtIonChanList.h"

#include "GrParControl.h"
#include "GrDialogInfo.h"
#include "GrParListInfo.h"
#include "GrParListInfoList.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	MODPARDIALOG_MAX_PARLISTS			6
#define	MODPARDIALOG_MAIN_PARENT_INFONUM	-1
#define	MODPARDIALOG_DISPLAY_PARENT_INFONUM	-2

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	DL_ID_QUIT = 1,
	DL_ID_CANCEL,
	DL_ID_OK,

	DL_ID_ADD_IC,
	DL_ID_BUTTON,
	DL_ID_DELETE_IC,
	DL_ID_CHECK_BOX,
	DL_ID_CHOICE,
	DL_ID_COMBO_BOX,
	DL_ID_LIST_BOX,
	DL_ID_SIM_LIST_BOX,
	DL_ID_SLIDER,
	DL_ID_TEXT

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ModuleParDialog **********************************/

class ModuleParDialog: public wxDialog {

	bool	updateParent, enableTextCtrlsFlag, enableNotebookPagingFlag;
	wxButton	*deleteICBtn, *addICBtn;
	wxObject	*myHandler;
	UniParListPtr	parList;

 public:
	DatumPtr	pc;
	wxButton	*okBtn, *cancelBtn, *quitBtn;
	ParListInfoList	*parListInfoList;

	ModuleParDialog(wxWindow *parent, const wxString& title, DatumPtr pc,
	  UniParListPtr theParList = NULL, wxObject *theHandler = NULL, int x =
	 -1, int y = -1, int width = -1, int height = -1, long style =
	  wxDEFAULT_DIALOG_STYLE);
	~ModuleParDialog(void);

	bool	UpdateParent(void);

	void	DeleteDialog(void);
	void	OnICButton(wxCommandEvent& event);
	void	OnPageChanged(wxNotebookEvent &event);

	bool	CheckChangedValues(void);
	void	DisableCancelBtn(void)	{ if (cancelBtn) cancelBtn->Enable(FALSE); }
	void	EnableControls(void);
	wxNode *	GetParListNode(wxNode *node, long tag);
	bool	GetUpdateParent(void)	{ return updateParent; }
	void	OnButton(wxCommandEvent& event);
	void	OnCancel(wxCommandEvent& event);
	void	OnCheckBox(wxCommandEvent& event);
	void	OnChoice(wxCommandEvent& event);
	void	OnComboBox(wxCommandEvent& event);
	void	OnCloseWindow(wxCloseEvent& event);
	void	OnOk(wxCommandEvent& event);
	void	OnQuit(wxCommandEvent& event);
	void	OnSliderUpdate(wxCommandEvent& event);
	void	OnText(wxCommandEvent& event);
	void	SetNotebookPanel(int election);
	void	SetUpdateParent(bool status)	{ updateParent = status; }
	void	SetNotebookSelection(void);

	DECLARE_EVENT_TABLE()

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

