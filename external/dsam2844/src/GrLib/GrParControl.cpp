/**********************
 *
 * File:		GrParControl.cpp
 * Purpose:		Parameter control structure classes.
 * Comments:	The client data set should be free'd by the "wxControl"
 *				destructors (I think).  There was a problem with the client
 *				data being de-allocated somewhere.
 * Author:		L. P. O'Mard
 * Created:		29 Oct 1999
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

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/checkbox.h>
#		include <wx/combobox.h>
#		include <wx/textctrl.h>
#		include <wx/button.h>
#		include <wx/choice.h>
#		include <wx/listbox.h>
#		include <wx/slider.h>
#		include <wx/stattext.h>
#		include <wx/log.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GeCommon.h"
#include "GeUniParMgr.h"

#include "GrParControl.h"

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
/****************************** ParControl Methods ****************************/
/******************************************************************************/

/****************************** Constructors **********************************/

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
  wxCheckBox *cB)
{
	Init(CHECK_BOX, theInfoNum, theSizer, thePar);
	checkBox = cB;
	PostInit();
}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
  wxTextCtrl *tC, wxStaticText  *labelTC)
{
	Init(TEXT_CTRL, theInfoNum, theSizer, thePar);
	textCtrl = tC;
	label = labelTC;
	PostInit();

}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
  wxChoice *c, wxStaticText *labelTC)
{
	Init(CHOICE, theInfoNum, theSizer, thePar);
	choice = c;
	label = labelTC;
	PostInit();

}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
  wxComboBox *cB, wxStaticText *labelTC)
{
	Init(COMBO_BOX, theInfoNum, theSizer, thePar);
	comboBox = cB;
	label = labelTC;
	PostInit();

}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
  wxListBox *lB, wxStaticText *labelTC)
{
	Init(LIST_BOX, theInfoNum, theSizer, thePar);
	listBox = lB;
	label = labelTC;
	PostInit();

}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
  ParControlTypeSpecifier t)
{
	Init(SPECIAL, theInfoNum, theSizer, thePar);
	type = t;

}

/****************************** Destructors ***********************************/

ParControl::~ParControl(void)
{
	ParControlHandle *handle = NULL;

	switch (tag) {
	case UNSET:
	case SPECIAL:
		break;
	case CHOICE:
		handle = (ParControlHandle *) choice->GetClientData();
		break;
	case COMBO_BOX:
		handle = (ParControlHandle *)comboBox->GetClientData();
		break;
	case LIST_BOX:
		handle = (ParControlHandle *)listBox->GetClientData();
		break;
	default: {
		wxControl *control = GetControl();

		if (control)
			handle = (ParControlHandle *) control->GetClientData();
		break; }
	}
	if (handle)
		delete handle;
	
}


/****************************** Init ******************************************/

/*
 * This routine intialises the values for a control.
 */

void
ParControl::Init(Tag theTag, int theInfoNum, wxSizer *theSizer,
  UniParPtr thePar)
{
	tag = theTag;
	infoNum = theInfoNum;
	par = thePar;
	updateFlag = FALSE;
	label = NULL;
	button = NULL;
	slider = NULL;
	sizer = theSizer;

}

/****************************** PostInit **************************************/

/*
 * This routine intialises the values for a control which need to be done after
 * the control has been initialised.
 * The client data must of a global context.  This is why I have used 'par'
 * instead of 'this'.
 */

void
ParControl::PostInit(void)
{
	switch (tag) {
	case UNSET:
	case SPECIAL:
		break;
	case CHOICE:
		choice->SetClientData(0, new ParControlHandle(this));
		break;
	case COMBO_BOX:
		comboBox->SetClientData(0, new ParControlHandle(this));
		break;
	case LIST_BOX:
		listBox->SetClientData(0, new ParControlHandle(this));
		break;
	default: {
		wxControl *control = GetControl();

		if (control)
			control->SetClientData(new ParControlHandle(this));
		break; }
	}

}

/****************************** GetControl ************************************/

/*
 * This function returns a pointer to the base wxControl for a ParControl.
 */

wxControl *
ParControl::GetControl(void)
{
	static const wxChar *funcName = wxT("ParControl::GetControl");

	switch (tag) {
	case UNSET:
	case SPECIAL:
		return(NULL);
	case CHECK_BOX:
		return(checkBox);
	case TEXT_CTRL:
		return(textCtrl);
	case CHOICE:
		return(choice);
	case COMBO_BOX:
		return(comboBox);
	case LIST_BOX:
		return(listBox);
	default:
		NotifyError(wxT("%s: Illegal tag. (%d);\n"), funcName, tag);
		return(NULL);
	}

}

/****************************** SetEnable *************************************/

/*
 * This routine enables or disable a control and its label.
 * Note that the buttons for the special file controls is controlled by the
 * selection
 */

void
ParControl::SetEnable(void)
{
	if (!par)
		return;

	wxControl *control = GetControl();

	if (control)
		control->Enable(CXX_BOOL(par->enabled));

	if (label)
		label->Enable(CXX_BOOL(par->enabled));

	if (button)
		switch (par->type) {
		case UNIPAR_NAME_SPEC_WITH_FPATH:
		case UNIPAR_NAME_SPEC_WITH_FILE:
			button->Enable(par->enabled && (comboBox->GetCurrentSelection() ==
			  (int) comboBox->GetCount() - 1));
			break;
		default:	
			button->Enable(CXX_BOOL(par->enabled));
		}
	if (!par->enabled)
		updateFlag = FALSE;

}

/****************************** SetButton *************************************/

/*
 * This routine sets the extra button for a control.
 */

void
ParControl::SetButton(wxButton *theButton)
{
	button = theButton;
	button->SetClientData(new ParControlHandle(this));

}

/****************************** SetSlider ************************************/

/*
 * This routine sets the extra Slider for a control.
 */

void
ParControl::SetSlider(wxSlider *theSlider)
{
	slider = theSlider;
	slider->SetClientData(new ParControlHandle(this));

}

/****************************** ResetValue ************************************/

/*
 * This routine resets the panel values for a control.
 * The update flag needs to be preserved for the TEXT_CTRL because the action
 * of setting the text causes 'updateFlag' to be set.
 */

void
ParControl::ResetValue(void)
{
	static const wxChar *funcName = wxT("ParControl::ResetValue");

	switch (tag) {
	case UNSET:
	case SPECIAL:
	case COMBO_BOX:
	case LIST_BOX:
		break;
	case CHOICE:
		choice->SetSelection(*par->valuePtr.nameList.specifier);
		break;
	case CHECK_BOX:
		checkBox->SetValue(CXX_BOOL(*par->valuePtr.i));
		break;
	case TEXT_CTRL: {
		bool	oldUpdateFlag = updateFlag;
		textCtrl->SetValue((wxChar *) GetParString_UniParMgr(par));
		textCtrl->SetInsertionPointEnd();
		updateFlag = oldUpdateFlag;
		} break;
	default:
		NotifyError(wxT("%s: Illegal tag. (%d);\n"), funcName, tag);
		return;
	}

}

/****************************** GetSize ***************************************/

/*
 * This routine returns a wxSize class for a control.
 * It returns 'wxDefaultSize' if the ParControl does not have a wxControl.
 */

wxSize
ParControl::GetSize(void) const
{
	switch (tag) {
	case CHECK_BOX:
		return(checkBox->GetSize());
	case TEXT_CTRL:
		return(textCtrl->GetSize());
	case CHOICE:
		return(choice->GetSize());
	case COMBO_BOX:
		return(comboBox->GetSize());
	case LIST_BOX:
		return(listBox->GetSize());
	default:
		return(wxDefaultSize);
	}

}
