/**********************
 *
 * File:		GrParListInfo.cpp
 * Purpose:		Module parameter list handling routines.
 * Author:		L. P. O'Mard
 * Created:		23 Oct 1998
 * Updated:		13 Jan 1999
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

// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif

// Any files included regardless of precompiled headers

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeNSpecLists.h"
#include "GeModuleMgr.h"
#include "UtDynaList.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtSSParser.h"

#include "GrParControl.h"
#include "GrModParDialog.h"

#include "GrParListInfo.h"

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

/****************************** Constructor ***********************************/

ParListInfo::ParListInfo(wxPanel *theParent, DatumPtr thePC,
  UniParListPtr theParList, int theInfoNum, int theoffset, int theNumPars)
{
	static const wxChar *funcName = wxT("ParListInfo::ParListInfo");

	parent = theParent;
	pc = thePC;
	parList = theParList;
	infoNum = theInfoNum;
	offset = theoffset;
	maxWidth = maxHeight = -1;
	numPars = (theNumPars < 0)? parList->numPars: theNumPars;

	// Set Up parameters list.
	if ((controlList = (ParControl **) calloc(numPars, sizeof(ParControl *))) ==
	  NULL) {
		NotifyError(wxT("%s: Out of memory for controlList"), funcName);
		return;
	}
	sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddSpacer(3);

	switch (parList->mode) {
	case UNIPAR_SET_IC:
		SetParListIonChannel();
		break;
	default:
		SetParListStandard();
	}

}

/****************************** Destructor ************************************/

/*
 * Note that the individual 'MyText' items will be deleted when the dialog panel
 * is deleted.
 */

ParListInfo::~ParListInfo(void)
{
	int		i;

	if (controlList) {
		for (i = 0; i < numPars; i++)
			if (controlList[i])
				delete controlList[i];
			else
				wxMessageBox(wxT("ParListInfo::~ParListInfo unset control!"));
		free(controlList);
	}

}

/****************************** SetParBoolean *********************************/

/*
 * This routine sets the boolean parameter check box.
 */

void
ParListInfo::SetParBoolean(UniParPtr par, int index)
{
	wxCheckBox *checkBox = new wxCheckBox(parent, DL_ID_CHECK_BOX + index,
	  (wxChar *) par->abbr);
	checkBox->SetValue(CXX_BOOL(*par->valuePtr.i));
	checkBox->SetToolTip((wxChar *) par->desc);

	wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer(1, PARCONTROL_VGAP,
	  PARCONTROL_HGAP);
	flexGridSizer->Add(checkBox, wxSizerFlags(1).Align(wxGROW |
	  wxALIGN_CENTER_VERTICAL));

	controlList[index] = new ParControl(par, infoNum, flexGridSizer, checkBox);


}

/****************************** SetParNameList ********************************/

/*
 * This routine sets the boolean parameter check box.
 */

void
ParListInfo::SetParNameList(UniParPtr par, int index)
{
	NameSpecifierPtr list;

	wxComboBox *cBox = new wxComboBox(parent, DL_ID_COMBO_BOX + index, wxT(""),
	  wxDefaultPosition, wxSize(PARLISTINFO_CHOICE_ITEM_WIDTH,-1), 0, NULL,
	  wxCB_READONLY);
	for (list = par->valuePtr.nameList.list; list->name; list++)
		cBox->Append((wxChar *) list->name);
	cBox->SetSelection(*par->valuePtr.nameList.specifier);
	cBox->SetToolTip((wxChar *) par->desc);

	wxStaticText *labelText = new wxStaticText(parent, -1, (wxChar *) par->
	  abbr);

	wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer(2, PARCONTROL_VGAP,
	  PARCONTROL_HGAP);
	flexGridSizer->Add(cBox, wxSizerFlags(1).Align(wxGROW |
	  wxALIGN_CENTER_VERTICAL));
	flexGridSizer->Add(labelText, wxSizerFlags().Align(wxALIGN_RIGHT |
	  wxALIGN_CENTER_VERTICAL));

	controlList[index] = new ParControl(par, infoNum, flexGridSizer, cBox,
	  labelText);

}

/****************************** SetParNameListWithText ************************/

/*
 * This routine sets the combo box with a button for browsing the file or
 * path.
 * The button text must have its own memory space, or when the dialog is
 * closed it can cause a crash.
 */

void
ParListInfo::SetParNameListWithText(UniParPtr par, int index)
{
	NameSpecifierPtr list;

	wxComboBox *cBox = new wxComboBox(parent, DL_ID_COMBO_BOX + index, wxT(""),
	  wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	for (list = par->valuePtr.nameList.list; list->name; list++)
		cBox->Append((wxChar *) list->name);
	cBox->SetSelection(*par->valuePtr.nameList.specifier);
	cBox->SetInsertionPointEnd();
	cBox->SetToolTip((wxChar *) par->desc);

	wxButton *browseBtn = new wxButton(parent, DL_ID_BUTTON + index,
	  PARLISTINFO_BROWSE_BUTTON_TEXT, wxDefaultPosition, wxSize(
	  PARLISTINFO_BROWSE_BUTTON_WIDTH, -1));
	browseBtn->SetToolTip(wxT("Click this button to browse for a file ")
	  wxT("(path)."));
	browseBtn->Enable(cBox->GetCurrentSelection() == ((int) cBox->GetCount() - 1));

	wxStaticText *labelText = new wxStaticText(parent, index, (wxChar *) par->
	  abbr);

	wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer(3, PARCONTROL_VGAP,
	  PARCONTROL_HGAP);
	flexGridSizer->Add(cBox, wxSizerFlags(1).Align(wxGROW |
	  wxALIGN_CENTER_VERTICAL));
	flexGridSizer->Add(browseBtn, wxSizerFlags().Border(wxALL,
	  PARCONTROL_BUTTON_MARGIN));
	flexGridSizer->Add(labelText, wxSizerFlags().Align(wxALIGN_RIGHT |
	  wxALIGN_CENTER_VERTICAL));

	controlList[index] = new ParControl(par, infoNum, flexGridSizer, cBox,
	  labelText);
	controlList[index]->SetButton(browseBtn);

}

/****************************** SetParFileName ********************************/

/*
 * This routine sets the text box for the file name with a browse button.
 * and description for the parameter.
 * The button text must have its own memory space, or when the dialog is
 * closed it can cause a crash.
 */

void
ParListInfo::SetParFileName(UniParPtr par, int index)
{
	wxTextCtrl	*textCtrl = new wxTextCtrl(parent, DL_ID_TEXT,
	  (wxChar *) GetParString_UniParMgr(par), wxDefaultPosition, wxSize(
	  PARLISTINFO_TEXT_ITEM_WIDTH, -1), wxHSCROLL);

	textCtrl->SetToolTip((wxChar *) par->desc);
	textCtrl->SetInsertionPointEnd();

	wxButton *browseBtn = new wxButton(parent, DL_ID_BUTTON + index,
	  PARLISTINFO_BROWSE_BUTTON_TEXT, wxDefaultPosition, wxSize(
	  PARLISTINFO_BROWSE_BUTTON_WIDTH, -1));
	browseBtn->SetToolTip(wxT("Click this button to browse for a file."));

	wxStaticText *labelText = new wxStaticText(parent, index, (wxChar *) par->
	  abbr);

	wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer(3, PARCONTROL_VGAP,
	  PARCONTROL_HGAP);
	flexGridSizer->Add(textCtrl, wxSizerFlags(1).Align(wxGROW |
	  wxALIGN_CENTER_VERTICAL));
	flexGridSizer->Add(browseBtn, wxSizerFlags().Border(wxALL,
	  PARCONTROL_BUTTON_MARGIN));
	flexGridSizer->Add(labelText, wxSizerFlags().Align(wxALIGN_RIGHT |
	  wxALIGN_CENTER_VERTICAL));

	controlList[index] = new ParControl(par, infoNum, flexGridSizer, textCtrl,
	  labelText);
	controlList[index]->SetButton(browseBtn);

}

/****************************** SetParStandard ********************************/

/*
 * This routine sets the standard parameter text box and description for a
 * parameter.
 */

void
ParListInfo::SetParStandard(UniParPtr par, int index)
{

	wxTextCtrl	*textCtrl = new wxTextCtrl(parent, DL_ID_TEXT, (wxChar *)
	  GetParString_UniParMgr(par), wxDefaultPosition, wxSize(
	  PARLISTINFO_TEXT_ITEM_WIDTH, -1), wxTE_PROCESS_ENTER);
	if (par->type != UNIPAR_INT_AL)
		textCtrl->SetToolTip((wxChar *) par->desc);
	else
		textCtrl->SetToolTip(wxString((wxChar *) par->desc) + wxT("\nYou must ")
		  wxT("press <return> after changing this parameter."));
	textCtrl->SetInsertionPointEnd();

	wxStaticText *labelText = new wxStaticText(parent, index, (wxChar *) par->
	  abbr);

	wxFlexGridSizer *flexGridSizer = new wxFlexGridSizer(2, PARCONTROL_VGAP,
	  PARCONTROL_HGAP);
	flexGridSizer->Add(textCtrl, wxSizerFlags(1).Align(wxGROW |
	  wxALIGN_CENTER_VERTICAL));
	flexGridSizer->Add(labelText, wxSizerFlags().Align(wxALIGN_RIGHT |
	  wxALIGN_CENTER_VERTICAL));

	controlList[index] = new ParControl(par, infoNum, flexGridSizer, textCtrl,
	  labelText);

	switch (par->type) {
	case UNIPAR_INT_AL: {
		controlList[index]->SetSlider(CreateSlider(index, *(controlList[index]->
		  GetPar()->valuePtr.array.numElements)));
		break; }
	case UNIPAR_INT_ARRAY:
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_STRING_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
	case UNIPAR_NAME_SPEC_ARRAY: {
		int		i;
		for (i = index - 1; i >= 0; i--)
			if (controlList[i]->GetSlider() && (controlList[i]->GetPar(
			  )->valuePtr.array.numElements == controlList[index]->GetPar(
			  )->valuePtr.array.numElements))
				controlList[index]->SetSlider(controlList[i]->GetSlider());
		break; }
	default:
		;
	}

}

/****************************** SetParGeneral *********************************/

/*
 * This routine sets the general parameter format.  It was separated from the
 * SetParListStandard routine so that other custon routines could use it too.
 * As sub-lists use the note book, the control sizer should not be added for
 * sub-lists.
 */

void
ParListInfo::SetParGeneral(UniParPtr par, int index)
{
	bool	subList = false;

	switch (par->type) {
	case UNIPAR_CFLIST:
	case UNIPAR_ICLIST:
	case UNIPAR_SIMSCRIPT:
	case UNIPAR_MODULE:
	case UNIPAR_PARLIST:
	case UNIPAR_PARARRAY:
		controlList[index] = new ParControl(par, infoNum, NULL,
		  PARCONTROL_SUB_LIST);
		subList = true;
		break;
	case UNIPAR_BOOL:
		SetParBoolean(par, index);
		break;
	case UNIPAR_NAME_SPEC:
		SetParNameList(par, index);
		break;
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		SetParNameListWithText(par, index);
		break;
	case UNIPAR_FILE_NAME:
		SetParFileName(par, index);
		break;
	default:
		SetParStandard(par, index);
	}
	controlList[index]->SetEnable();
	if (!subList)
		sizer->Add(controlList[index]->GetSizer(), 0, wxALIGN_LEFT |
		  wxALIGN_CENTER_VERTICAL);

}

/****************************** SetParListStandard ****************************/

/*
 * This routine sets the standard parameter list format.
 */

void
ParListInfo::SetParListStandard(void)
{
	int		i;

	for (i = 0; i < numPars; i++) {
		SetParGeneral(&parList->pars[i + offset], i);
		if ((i == 1) && (parList->mode == UNIPAR_SET_PARARRAY))
			controlList[i]->SetSlider(CreateSlider(i, parList->handlePtr.
			  parArray.ptr->numParams));

	}

}

/****************************** SetParListIonChannel **************************/

/*
 * This routine sets the ion channel parameter list.
 * The 'ALPHA' and 'BETA' controls must be done in separate loops so that the
 * "SetSlider" routine can be used.
 */

void
ParListInfo::SetParListIonChannel(void)
{
	const int	numHHCols = ICLIST_IC_FUNC2_K - ICLIST_IC_FUNC1_A + 1;
	int		i, index;
	UniParPtr	par;
	wxSlider	*slider = NULL;
	wxString	heading;
	wxTextCtrl	*hHuxleyAlphaTC[numHHCols], *hHuxleyBetaTC[numHHCols];
	wxStaticText *hHuxleyLabel[numHHCols];

	maxHeight = 0;
	for (i = 0; i <= ICLIST_IC_FILE_NAME; i++) {
		SetParGeneral(&parList->pars[i], i);
		maxHeight += GetParControl(i)->GetSize().GetHeight() +
		  PARLISTINFO_DEFAULT_Y_MARGIN;
	}

	for (i = 0; i < numHHCols; i++) {
		heading.sprintf(wxT("    %c"), 'A' + i);
		hHuxleyLabel[i] = new wxStaticText(parent, -1, heading,
		  wxDefaultPosition, wxSize(PARLISTINFO_IC_TEXT_ITEM_WIDTH, -1));

		index = i + ICLIST_IC_FUNC1_A;
		par = &parList->pars[index];
		if (i == 0)
			slider = CreateSlider(index, *par->valuePtr.array.numElements);
		hHuxleyAlphaTC[i] = new wxTextCtrl(parent, DL_ID_TEXT, (wxChar *)
		  GetParString_UniParMgr(par), wxDefaultPosition, wxSize(
		  PARLISTINFO_IC_TEXT_ITEM_WIDTH, -1));
		hHuxleyAlphaTC[i]->SetToolTip((wxChar *) par->desc);
		controlList[index] = new ParControl(par, infoNum, NULL, hHuxleyAlphaTC[
		  i], hHuxleyLabel[i]);
		controlList[index]->SetEnable();
		controlList[index]->SetSlider(slider);
	}
	for (i = 0; i < numHHCols; i++) {
		index = i + ICLIST_IC_FUNC2_A;
		par = &parList->pars[index];
		hHuxleyBetaTC[i] = new wxTextCtrl(parent, DL_ID_TEXT, (wxChar *)
		  GetParString_UniParMgr(par), wxDefaultPosition, wxSize(
		  PARLISTINFO_IC_TEXT_ITEM_WIDTH, -1));
		hHuxleyBetaTC[i]->SetToolTip((wxChar *) par->desc);
		controlList[index] = new ParControl(par, infoNum, NULL,
		  hHuxleyBetaTC[i], hHuxleyLabel[i]);
		controlList[index]->SetSlider(slider);
		controlList[index]->SetEnable();

	}
	int  width, height;
	GetParControl(ICLIST_IC_FUNC1_A)->GetSlider()->GetSize(&width, &height);
	maxHeight += height + PARLISTINFO_DEFAULT_Y_MARGIN + 3 *
	  (GetParControl(ICLIST_IC_FUNC1_A)->GetSize().GetHeight() +
	  PARLISTINFO_DEFAULT_Y_MARGIN);
	maxWidth = numHHCols * (GetParControl(ICLIST_IC_FUNC1_A)->GetSize(
	  ).GetWidth() + PARLISTINFO_DEFAULT_X_MARGIN) + 2 *
	  PARLISTINFO_DEFAULT_X_MARGIN;
/* ?? 	for (i = 0; i < numHHCols; i++) {
		c = new wxLayoutConstraints;
		if (i == 0)
			c->left.SameAs(parent, wxLeft, 4);
		else
			c->left.RightOf(hHuxleyLabel[i - 1], 4);
		if (!lastControl)
			c->top.SameAs(parent, wxTop, 4);
		else
			c->top.SameAs(lastControl, wxBottom, 4);
		c->width.AsIs();
		c->height.AsIs();
		hHuxleyLabel[i]->SetConstraints(c);

		c = new wxLayoutConstraints;
		c->left.SameAs(hHuxleyLabel[i], wxLeft);
		c->top.Below(hHuxleyLabel[i], 4);
		c->width.AsIs();
		c->height.AsIs();
		hHuxleyAlphaTC[i]->SetConstraints(c);

		c = new wxLayoutConstraints;
		c->left.SameAs(hHuxleyAlphaTC[i], wxLeft);
		c->top.Below(hHuxleyAlphaTC[i], 4);
		c->width.AsIs();
		c->height.AsIs();
		hHuxleyBetaTC[i]->SetConstraints(c);
	}
*/

}

/****************************** CreateSlider **********************************/

/*
 */

wxSlider *
ParListInfo::CreateSlider(int index, int numElements)
{
	wxSlider *slider = new wxSlider(parent, DL_ID_SLIDER + index, 1, 1,
	  numElements, wxDefaultPosition, wxSize(PARLISTINFO_SILDER_ITEM_WIDTH,
	  -1), wxSL_LABELS | wxSL_HORIZONTAL);

	sizer->Add(slider, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	return(slider);

}

/****************************** SetEnabledControls ****************************/

/*
 * This routine enables or disables the controls for this info structure.
 * It is called when the enabled controls can have been changed.
 */

void
ParListInfo::SetEnabledControls(void)
{
	int		i;

	for (i = 0; i < numPars; i++)
		if (controlList[i])
			controlList[i]->SetEnable();
		else
			wxMessageBox(wxT("ParListInfo::SetEnabledControls: Unset ")
			  wxT("control!"));

}

/****************************** SetParValue ***********************************/

/*
 * This routine sets a parameter value.
 */

bool
ParListInfo::SetParValue(ParControl *control)
{
	wxString	value;

	switch (control->GetTag()) {
	case ParControl::CHECK_BOX:
		value = (wxChar *) ((control->GetCheckBox()->GetValue())?
		  BooleanList_NSpecLists(GENERAL_BOOLEAN_ON)->name:
		  BooleanList_NSpecLists(GENERAL_BOOLEAN_OFF)->name);
		break;
	case ParControl::CHOICE:
		value = control->GetChoice()->GetStringSelection();
		break;
	case ParControl::COMBO_BOX:
		value = control->GetComboBox()->GetStringSelection();
		break;
	case ParControl::TEXT_CTRL:
		value = control->GetTextCtrl()->GetValue();
		break;
	default:
		value = wxT("Value not yet being set!");
	}
	if (pc)
		SET_PARS_POINTER(pc->data);
	if (SetParValue_UniParMgr(&parList, control->GetPar()->index, (wxChar *)
	  value.c_str())) {
		control->SetUpdateFlag(FALSE);
		return(TRUE);
	}
	return(FALSE);

}

/****************************** CheckChangedValues ****************************/

/*
 * This routine checks the changed values and returns false if any of them
 * cannot be set.
 * If the parameter list has been updated, then it is assumed that all the
 * the parameters must be checked.
 */

bool
ParListInfo::CheckChangedValues(void)
{
	bool	ok = TRUE;
	int		i;

	for (i = 0; i < numPars; i++)
		if (controlList[i]->GetUpdateFlag()) {
			if (!SetParValue(controlList[i]))
				ok = FALSE;
		}
	return(ok);

}

/****************************** GetParControl *********************************/

/*
 * This function returns a parameter control.
 */

ParControl *
ParListInfo::GetParControl(int i)
{
	static const wxChar *funcName = wxT("ParListInfo::GetParControl");
	if (i < numPars)
		return(controlList[i]);
	NotifyError(wxT("%s: Control index %d is out of bound."), funcName, i);
	return(NULL);

}

/****************************** ResetControlValues ****************************/

/*
 * This routine resets all of the controls to their original values.
 */

void
ParListInfo::ResetControlValues(void)
{
	int		i;

	for (i = 0; i < numPars; i++)
		controlList[i]->ResetValue();

}

/****************************** UpdateControlValues ***************************/

/*
 * This routine updates all of the controls to their original values, unless
 * they have been set.
 */

void
ParListInfo::UpdateControlValues(void)
{
	int		i;

	for (i = 0; i < numPars; i++)
		if (!controlList[i]->GetUpdateFlag())
			controlList[i]->ResetValue();

}

/****************************** CheckInterDependence **************************/

/*
 * This routine checks the interdependence of parameters.  If a dependent
 * parameter is changed, then the parList->updateFlag will be set.
 */

void
ParListInfo::CheckInterDependence(void)
{
	if (parList->updateFlag) {
		SetEnabledControls();
		UpdateControlValues();
	}
}
