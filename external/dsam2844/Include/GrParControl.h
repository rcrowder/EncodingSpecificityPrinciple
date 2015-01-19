/**********************
 *
 * File:		GrParControl.h
 * Purpose:		Parameter control structure classes.
 * Comments:	.
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
 
#ifndef _GRPARCONTROL_H
#define _GRPARCONTROL_H 1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PARCONTROL_VGAP				3
#define PARCONTROL_HGAP				3
#define PARCONTROL_BUTTON_MARGIN	3

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PARCONTROL_SUB_LIST

} ParControlTypeSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParControl ***************************************/

class ParControl {

  public:
	enum	Tag { UNSET, SPECIAL, CHECK_BOX, TEXT_CTRL, CHOICE, COMBO_BOX,
				  LIST_BOX } tag;

  private:
	bool	updateFlag;
	int		infoNum;
	UniParPtr	par;
	wxSizer	*sizer;
	union {

		wxCheckBox	*checkBox;
		wxChoice	*choice;
		wxComboBox	*comboBox;
		wxListBox	*listBox;
		wxTextCtrl	*textCtrl;

		ParControlTypeSpecifier	type;

	};
	wxButton		*button;
	wxSlider		*slider;
	wxStaticText	*label;

	void	Check(Tag t)	{ if (tag != t) wxLogError(wxT("GrParListInfo: ")
							  wxT("Illegal ParControl")); }

  public:

	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  wxCheckBox *cB);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  wxTextCtrl *tC, wxStaticText  *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer, wxChoice *c,
	  wxStaticText *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  wxComboBox *cB, wxStaticText *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  wxListBox *lB, wxStaticText *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  ParControlTypeSpecifier t);
	~ParControl(void);

	void	Init(Tag theTag, int theInfoNum, wxSizer *theSizer,
			  UniParPtr thePar);

	wxButton *		GetButton(void)		{ return button; }
	wxCheckBox *	GetCheckBox(void)	{ Check(CHECK_BOX); return checkBox; }
	wxChoice *		GetChoice(void)		{ Check(CHOICE); return choice; }
	wxComboBox *	GetComboBox(void)	{ Check(COMBO_BOX); return comboBox; }
	wxControl *		GetControl(void);
	int				GetInfoNum(void)	{ return infoNum; }
	wxListBox *		GetListBox(void)	{ Check(LIST_BOX); return listBox; }
	UniParPtr		GetPar(void)		{ return par; }
	wxSize			GetSize(void) const;
	wxSizer *		GetSizer(void)		{ return sizer; }
	wxSlider *		GetSlider(void)		{ return slider; }
	ParControlTypeSpecifier	GetSpecialType(void)
										{ Check(SPECIAL); return type; }
	Tag				GetTag(void)		{ return tag; }
	wxTextCtrl *	GetTextCtrl(void)	{ Check(TEXT_CTRL); return textCtrl; }
	bool			GetUpdateFlag(void)	{ return updateFlag; }

	void	PostInit(void);
	void	ResetValue(void);
	void	SetEnable(void);
	void	SetButton(wxButton *theButton);
	void	SetSlider(wxSlider *theSlider);
	void	SetUpdateFlag(bool state)			{ updateFlag = state; }

};
		
/*************************** ParControlHandle *********************************/

class ParControlHandle {

	ParControl	*parControl;

  public:

	ParControlHandle(ParControl *theParControl)	{ parControl = theParControl; }

	ParControl *	GetPtr(void)	{ return parControl; }

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

