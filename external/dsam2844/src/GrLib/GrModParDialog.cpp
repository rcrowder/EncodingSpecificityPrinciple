/**********************
 *
 * File:		GrModParDialog.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"
#include "wx/statline.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtDynaList.h"
#include "UtDatum.h"
#include "GeModuleMgr.h"

#include "GrSimMgr.h"
#include "GrSDICanvas.h"
#include "GrSDIFrame.h"
#include "GrSDIEvtHandler.h"
#include "GrModParDialog.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/******************************************************************************/
/****************************** ModuleParDialog Methods ***********************/
/******************************************************************************/

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

BEGIN_EVENT_TABLE(ModuleParDialog, wxDialog)
	EVT_BUTTON(wxID_CANCEL,					ModuleParDialog::OnCancel)
	EVT_BUTTON(wxID_OK, 					ModuleParDialog::OnOk)
	EVT_BUTTON(-1 /* DL_ID_BUTTON */,		ModuleParDialog::OnButton)
	EVT_CHECKBOX(-1 /* DL_ID_CHECK_BOX */,	ModuleParDialog::OnCheckBox) 
	EVT_CHOICE(-1 /* DL_ID_CHOICE */,		ModuleParDialog::OnChoice) 
	EVT_COMBOBOX(-1 /* DL_ID_COMBO_BOX */,	ModuleParDialog::OnComboBox)
	EVT_SLIDER(-1 /* DL_ID_SLIDER */,		ModuleParDialog::OnSliderUpdate)
	EVT_TEXT(DL_ID_TEXT,					ModuleParDialog::OnText)
	EVT_CLOSE(ModuleParDialog::OnCloseWindow)
//	EVT_BUTTON(DL_ID_ADD_IC,	ModuleParDialog::OnICButton)
//	EVT_BUTTON(DL_ID_DELETE_IC,	ModuleParDialog::OnICButton)
	EVT_NOTEBOOK_PAGE_CHANGED(PARLISTINFOLIST_ID_NOTEBOOK,
	  ModuleParDialog::OnPageChanged)
END_EVENT_TABLE()

/****************************** Constructor ***********************************/

/*
 * I had to use a wxMessage rather than the label of the MyText to get the
 * description to the left of the value, and correctly lined up.
 * Negative tags are being used for the MyText items to avoid conflict with
 * other events.
 */

ModuleParDialog::ModuleParDialog(wxWindow *parent, const wxString& title,
  DatumPtr thePC, UniParListPtr theParList, wxObject *theHandler, int x, int y,
  int width, int height, long style): wxDialog(parent, -1, title,  wxPoint(x,
  y), wxSize(width, height), style)
{
	updateParent = FALSE;
	enableTextCtrlsFlag = FALSE;
	enableNotebookPagingFlag = FALSE;
	pc = thePC;
	okBtn = NULL;
	cancelBtn = NULL;
	quitBtn = NULL;
	deleteICBtn = NULL;
	myHandler = theHandler;
	parList = theParList;

	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(topSizer);
	
	parListInfoList = new ParListInfoList(this, pc, parList);
	EnableControls();

	topSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(3,
	  3), wxHORIZONTAL), wxSizerFlags().Expand());

	// Set up buttons
	wxBoxSizer *buttonBox = new wxBoxSizer( wxHORIZONTAL );
	buttonBox->Add(new wxButton(this,  wxID_OK, wxT("Ok")), wxSizerFlags().
	  Border(wxALL, 7));
	cancelBtn = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
	buttonBox->Add(cancelBtn, wxSizerFlags().Border(wxALL, 7));
	topSizer->Add(buttonBox, wxSizerFlags().Center());

/*??	if (parListInfoList->GetUseNotebookControls()) {
		addICBtn = new wxButton(this, DL_ID_ADD_IC, "Add IC");
		addICBtn->Enable(FALSE);
		addICBtn->SetToolTip("Press button to add an ion channel.");

		deleteICBtn = new wxButton(this, DL_ID_DELETE_IC, "Delete IC");
		deleteICBtn->SetToolTip("Press button to delete an ion channel.");
		deleteICBtn->Enable(FALSE);

		c = new wxLayoutConstraints;
		c->left.RightOf(cancelBtn, 4);
		c->bottom.SameAs(cancelBtn, wxBottom);
		c->width.AsIs();
		c->height.AsIs();
		addICBtn->SetConstraints(c);

		c = new wxLayoutConstraints;
		c->left.RightOf(addICBtn, 4);
		c->bottom.SameAs(addICBtn, wxBottom);
		c->width.AsIs();
		c->height.AsIs();
		deleteICBtn->SetConstraints(c);

	} else {
		addICBtn = NULL;
		deleteICBtn = NULL;
	}
*/
	topSizer->SetSizeHints(this);

}

/****************************** Destructor ************************************/

ModuleParDialog::~ModuleParDialog(void)
{
	if (parListInfoList)
		delete parListInfoList;

}

/****************************** EnableControls ********************************/

/*
 * This routine enables the control call backs that should not start working
 * until the necessary parameters are initialised.
 */

void
ModuleParDialog::EnableControls(void)
{
	enableTextCtrlsFlag = TRUE;
	enableNotebookPagingFlag = TRUE;

}

/****************************** UpdateParent **********************************/

/*
 * This function takes care of paremt updating that is required.
 */

bool
ModuleParDialog::UpdateParent(void)
{
	updateParent = false;
	wxGetApp().GetFrame()->canvas->view->GetDocument()->Modify(true);
	return (true);

}

/****************************** CheckChangedValues ****************************/

/*
 * This routine checks the changed values and returns false if any of them
 * cannot be set.
 * This routine also sets the main parList update flag (parListInfo[0]) to
 * It assumes that there is at least one member in the list.
 * TRUE if any of the sub-parList update flags are set to TRUE;
 */

bool
ModuleParDialog::CheckChangedValues(void)
{
	bool	ok = true;
	size_t	i;

	for (i = 0; i < parListInfoList->list.GetCount(); i++) {
		if (!parListInfoList->list[i]->CheckChangedValues())
			ok = false;
		if (parListInfoList->list[i]->parList->updateFlag) {
			parListInfoList->list[0]->parList->updateFlag = TRUE;
			if(cancelBtn)
				cancelBtn->Enable(FALSE);
		}
	}
	if (ok) {
		if (parListInfoList->list[0]->parList->updateFlag)
			updateParent = true;
		if (!CheckParList_UniParMgr(parListInfoList->list[0]->parList))
			ok = false;
	}
	if (ok && !UpdateParent())
		ok = false;
	return(ok);

}

/****************************** GetParListNode ********************************/

/*
 * This routine recursively checks for the appropriate list for a tag.
 */

wxNode *
ModuleParDialog::GetParListNode(wxNode *node, long tag)
{
	ParListInfo	*p = (ParListInfo *) node->GetData();
	
	if ((tag - p->GetOffset()) > (p->parList->numPars - 1))
		return(GetParListNode(node->GetNext(), tag));
	return(node);

}

/****************************** SetNotebookPanel ******************************/

/*
 * This routine the main parlist 'notebookPanel' field.
 */

void
ModuleParDialog::SetNotebookPanel(int selection)
{
	ParListInfo *info = parListInfoList->list[0];

	info->parList->notebookPanel = selection;
}

/****************************** SetNotebookSelection **************************/

/*
 * This routine is a fudge because the default panel selection cannot be set
 * within the constructor.
 */

void
ModuleParDialog::SetNotebookSelection(void)
{
	if (parList && parListInfoList->notebook)
		parListInfoList->notebook->SetSelection(parList->notebookPanel);

}

/****************************** DeleteDialog **********************************/

void
ModuleParDialog::DeleteDialog(void)
{
//	switch (infoNum) {
//	case MODPARDIALOG_MAIN_PARENT_INFONUM:
//		wxGetApp().GetFrame()->SetMainParDialog(NULL);
//		break;
//	case MODPARDIALOG_DISPLAY_PARENT_INFONUM:
//		if (IsModal())
//			EndModal(wxID_CANCEL);
//		else
//			Close();
//		break;
//	default:
//		printf("ModuleParDialog::DeleteDialog: calling dialog pointer needs ")
//		  "to be set to NULL\n");
//	}

	if (!pc)
		wxGetApp().GetFrame()->SetMainParDialog(NULL);
	else {
		if (myHandler)
			((SDIEvtHandler *) myHandler)->SetDialog(NULL);
		else
			printf("ModuleParDialog::DeleteDialog: calling dialog pointer "
			  "needs to be set to NULL\n");
	}
	wxGetApp().GetFrame()->DeleteFromDialogList(this);

}

/******************************************************************************/
/****************************** Call backs ************************************/
/******************************************************************************/

/****************************** OnICButton ************************************/

/*
 * This routine operates on the panels of a note book for the Ion channel
 * operations.
 * It assumes that the ParListInfo of the main ICList page will give access
 * to the ICList structure.
 */

void 
ModuleParDialog::OnICButton(wxCommandEvent& event)
{
	static const wxChar *funcName = wxT("ModuleParDialog::OnICButton");
	ParListInfo	*info = parListInfoList->list[
				  PARLISTINFOLIST_MAIN_ICLIST_PAGES - 1];
	IonChanListPtr	theICs = info->parList->handlePtr.iCs;

	ResetGUIDialogs();
	NotifyError(wxT("%s: Not yet Implemented."), funcName);
	return;
	switch (event.GetId()) {
	case DL_ID_ADD_IC: {
		IonChannelPtr	iC;
		printf("Add IC\n");
		if ((iC = InitIonChannel_IonChanList(funcName,
		  theICs->numTableEntries)) == NULL) {
			NotifyError(wxT("%s: Could not initialise ion channel."), funcName);
			return;
		}
		SetICGeneralPars_IonChanList(iC, ICLIST_BOLTZMANN_MODE,
		  wxT("<description>"), (WChar *) wxT("on"), 0.0, 0.0, 1);
		SetICGeneralParsFromICList_IonChanList(iC, theICs);
		SetIonChannelUniParList_IonChanList(theICs, iC);
		SetIonChannelUniParListMode_IonChanList(iC);
		if (!Append_Utility_DynaList(&theICs->ionChannels, iC)) {
			NotifyError(wxT("%s: Could not add ion channel to list."), funcName);
			FreeIonChannel_IonChanList(&iC);
			return;
		}
		parListInfoList->SetStandardInfo(NULL, info->GetPC(), iC->parList,
		  (wxChar *) iC->description);
		break; }
	case DL_ID_DELETE_IC:
		printf("Delete IC\n");
		break;
	default:
		NotifyError(wxT("%s: Unknown button ID (%d)."), funcName, event.GetId());
		return;
	}

}

/****************************** OnOk ******************************************/

/*
 * This routine updates the module's parameters and closes the dialog.
 */

void 
ModuleParDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
	ResetGUIDialogs();
	if (!CheckChangedValues())
		return;
	if (IsModal())
		EndModal(wxID_OK);
	else
		Close();

}

/****************************** OnPageChanged *********************************/

void 
ModuleParDialog::OnPageChanged(wxNotebookEvent &event)
{
	if (!enableNotebookPagingFlag)
		return;
	int	selection = event.GetSelection();

	SetNotebookPanel(selection);
	if (deleteICBtn) {
		int	index = selection - PARLISTINFOLIST_MAIN_ICLIST_PAGES;

		if ((index < 0)) {
			deleteICBtn->Enable(FALSE);
			return;
		}
		deleteICBtn->Enable(TRUE);
		ParListInfo	*info = parListInfoList->list[index +
		  PARLISTINFOLIST_MAIN_ICLIST_PAGES];
		info->parList->handlePtr.iCs->currentIC = (IonChannelPtr)
		  GetMemberData_Utility_DynaList(info->parList->handlePtr.iCs->
		  ionChannels, index);
	}

}

/****************************** OnButton **************************************/

void
ModuleParDialog::OnButton(wxCommandEvent& event)
{
	wxButton *but = (wxButton *) event.GetEventObject();
	ParControl *control = ((ParControlHandle *) but->GetClientData())->GetPtr();
	UniParPtr	par = control->GetPar();

	switch (par->type) {
	case UNIPAR_NAME_SPEC_WITH_FPATH:
	case UNIPAR_NAME_SPEC_WITH_FILE: {
		wxComboBox *cB = control->GetComboBox();
		wxString	path;

		if (par->type == UNIPAR_NAME_SPEC_WITH_FPATH) {
			wxDirDialog dialog(this, wxT("Choose a directory"), wxGetCwd());
			if (dialog.ShowModal() != wxID_OK)
				break;
			path = dialog.GetPath();
		} else {
			wxFileDialog dialog(this, wxT("Choose a file"), wxGetCwd());
			if (dialog.ShowModal() != wxID_OK)
				break;
			path = dialog.GetPath();
		}
		cB->Delete(cB->GetSelection());
		cB->Append(path);
		cB->SetSelection(cB->GetCount() - 1);
		cB->SetInsertionPointEnd();
		control->SetUpdateFlag(TRUE);
		break; }
	case UNIPAR_FILE_NAME: {
		wxFileDialog dialog(this, wxT("Choose a file"), wxGetCwd(), wxT(""),
		  (wxChar *) par->valuePtr.file.defaultExtension);
		if (dialog.ShowModal() != wxID_OK)
			break;
		wxTextCtrl *textCtrl = control->GetTextCtrl();
		textCtrl->SetValue(dialog.GetPath());
		textCtrl->SetInsertionPointEnd();
		control->SetUpdateFlag(TRUE);
		break; }
	default:
		;
	}

}

/****************************** OnCancel **************************************/

void
ModuleParDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
		Close();

}

/****************************** OnCheckBox ************************************/

void
ModuleParDialog::OnCheckBox(wxCommandEvent& event)
{
	wxCheckBox *cB = (wxCheckBox *) event.GetEventObject();
	ParControl *control = ((ParControlHandle *) cB->GetClientData())->GetPtr();
	ParListInfo *info = parListInfoList->list[control->GetInfoNum()];

	control->SetUpdateFlag(TRUE);
	info->SetParValue(control);
	info->CheckInterDependence();

}

/****************************** OnChoice **************************************/

void
ModuleParDialog::OnChoice(wxCommandEvent& event)
{
	wxChoice *c = (wxChoice *) event.GetEventObject();
	ParControl *control = ((ParControlHandle *) c->GetClientData(0))->GetPtr();
	ParListInfo *info = parListInfoList->list[control->GetInfoNum()];

	control->SetUpdateFlag(TRUE);
	info->SetParValue(control);
	info->CheckInterDependence();
	return;

}

/****************************** OnComboBox ************************************/

void
ModuleParDialog::OnComboBox(wxCommandEvent& event)
{
	wxComboBox *cB = (wxComboBox *) event.GetEventObject();
	ParControl *control = ((ParControlHandle *) cB->GetClientData(0))->GetPtr();
	ParListInfo *info = parListInfoList->list[control->GetInfoNum()];

	control->SetUpdateFlag(TRUE);
	switch (control->GetPar()->type) {
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH: {
		wxComboBox *cB = (wxComboBox *) event.GetEventObject();
		control->GetButton()->Enable(cB->GetCurrentSelection() == (int) (cB->GetCount() -
		  1));
		cB->SetInsertionPointEnd();
		break; }
	default:
		;
	}
	info->SetParValue(control);
	info->CheckInterDependence();
	DisableCancelBtn();

}

/****************************** OnSliderUpdate ********************************/

void
ModuleParDialog::OnSliderUpdate(wxCommandEvent& event)
{
	static const wxChar *funcName = wxT("ModuleParDialog::OnSliderUpdate");
	int		i;

	wxSlider	*slider = (wxSlider *) event.GetEventObject();
	int			index = slider->GetValue();
	ParControl	*control = ((ParControlHandle *) slider->GetClientData(
				  ))->GetPtr();
	ParControl	*infoCtrl;
	ParListInfo	*info = parListInfoList->list[control->GetInfoNum()];

	for (i = 0; i < info->GetNumPars(); i++) {
		infoCtrl = info->GetParControl(i);
		if (infoCtrl->GetSlider() == control->GetSlider()) {
			if (infoCtrl->GetUpdateFlag() && !info->SetParValue(infoCtrl)) {
				NotifyError(wxT("%s: Cannot update array value."), funcName);
				return;
			}
			infoCtrl->GetPar()->valuePtr.array.index = index - 1;
			infoCtrl->ResetValue();
			infoCtrl->SetUpdateFlag(FALSE);
			DisableCancelBtn();
		}
	}

}

/****************************** OnText ****************************************/

/*
 * This routine returns as soon as a wxTextCtrl is set, so this has to be tested
 * for, using the 'enableTextCtrlsFlag' flag.
 * For now I have had to put back in the NULL handle test to cope with the -1
 * ID for the handler allowing things like the wxFileDialog to call this
 * routine.
 */

void
ModuleParDialog::OnText(wxCommandEvent& event)
{
	wxTextCtrl *txt = (wxTextCtrl *) event.GetEventObject();

	if (!enableTextCtrlsFlag)
		return;

	ParControlHandle *handle = (ParControlHandle *) txt->GetClientData();

	if (handle->GetPtr()->GetPar()->enabled)
		handle->GetPtr()->SetUpdateFlag(TRUE);

}

/****************************** OnCloseWindow *********************************/

void 
ModuleParDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	DeleteDialog();
	Destroy();

}

