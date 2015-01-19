/**********************
 *
 * File:		GrParListInfoList.cpp
 * Purpose:		Module parameter information list handling routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		11 Feb 1999
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
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtDynaList.h"
#include "UtSSSymbols.h"
#include "UtDatum.h"
#include "UtSSParser.h"

#include "GrModParDialog.h"

#include "GrParListInfoList.h"

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

ParListInfoList::ParListInfoList(ModuleParDialog *theParent, DatumPtr pc,
  UniParListPtr parList)
{
	static const wxChar *funcName = wxT("ParListInfoList::ParListInfoList");

	notebookPanel = 0;
	parent = theParent;
	notebook = NULL;
	useNotebookControls = FALSE;

	if ((pc == NULL) && (parList == NULL)) {
		NotifyError(wxT("%s: Both the simulation (pc) and the parameter list ")
		  wxT("are NULL."), funcName);
		return;
	}
	
	if (parList->GetPanelList)
		SetPanelledModuleInfo((wxPanel *) parent, pc, parList);
	else
		SetStandardInfo((wxPanel *) parent, pc, parList, wxT("General"));
	if (notebook) {
		if (parList->notebookPanel < 0)
			parList->notebookPanel = notebookPanel;
//??		parent->SetSize(GetNotebookSize());
	}

}

/****************************** Destructor ************************************/

ParListInfoList::~ParListInfoList(void)
{

}

/****************************** GetNotebookSize *******************************/

/*
 * This function calculates the maximum size for a notebook.
 * This is then used to calculate the height of the parent of the notebook,
 * because I cannot get constraints to work properly when notebooks are used.
 * There is a bug in MSW wxWindows in that there is no function for slider that
 * returns a wxSize element.
 */

wxSize
ParListInfoList::GetNotebookSize(void) const
{
	int		i, maxWidth = 325, maxHeight = 0, heightSum, width, height;
	size_t	j;
	wxSize	infoSize, cSize, sSize;
	ParControl	*control;

	for (j = 0; j < list.Count(); j++) {
		if (list[j]->GetSize() == wxDefaultSize) {
			for (i = 0, heightSum = 0; i < list[j]->GetNumPars(); i++) {
				control = list[j]->GetParControl(i);
				cSize = control->GetSize();
				heightSum += cSize.GetHeight() + PARLISTINFO_DEFAULT_Y_MARGIN;
				if (control->GetSlider()) {
					// See note in function comment.
					// sSize = control->GetSlider()->GetSize(&width, &height);
					control->GetSlider()->GetSize(&width, &height);
					sSize.Set(width, height);
					heightSum += sSize.GetHeight() +
					  PARLISTINFO_DEFAULT_Y_MARGIN;
					if ((sSize.GetWidth() + PARLISTINFO_DEFAULT_X_MARGIN * 2) >
					  maxWidth)
						maxWidth = sSize.GetWidth() + 4 *
						  PARLISTINFO_DEFAULT_X_MARGIN;
				}
			}
			if (heightSum > maxHeight)
				maxHeight = heightSum;
		} else {
			infoSize = list[j]->GetSize();
			if (infoSize.GetWidth() > maxWidth)
				maxWidth = infoSize.GetWidth();
			if (infoSize.GetHeight() > maxHeight)
				maxHeight = infoSize.GetHeight();
		}
	}
	return(wxSize(maxWidth, maxHeight + PARLISTINFOLIST_NOTEBOOK_ADD_HEIGHT));

}

/****************************** UseNewNotebook ********************************/

/*
 * This function checks if a parameter is of a type that requires a new
 * notebook.
 */

bool
ParListInfoList::UseNewNotebook(UniParPtr par)
{
	switch(par->type) {
	case UNIPAR_CFLIST:
	case UNIPAR_MODULE:
		return(TRUE);
	case UNIPAR_PARLIST:
		return(*par->valuePtr.parList.list != NULL);
 	case UNIPAR_ICLIST:
		useNotebookControls = TRUE;
 		return(TRUE);
	default:
		return(FALSE);
	}

}

/****************************** UsingNotebook *********************************/

/*
 * This routine checks if a notebook needs to be initialised.
 */

wxPanel *
ParListInfoList::UsingNotebook(UniParListPtr parList, const wxString& title)
{
	/* static const wxChar *funcName = "ParListInfoList::UsingNotebook"; */
	bool	useNewNotebook = FALSE;
	int		i;
	wxPanel	*panel = NULL;

	if (!notebook) {
		for (i = 0; (i < parList->numPars) && !useNewNotebook; i++)
			if (UseNewNotebook(&parList->pars[i]))
				useNewNotebook = TRUE;
		 if (parList->GetPanelList)
		 	useNewNotebook = TRUE;
	}
	if (useNewNotebook) {
		notebook = new wxNotebook(parent, PARLISTINFOLIST_ID_NOTEBOOK);

		parent->GetSizer()->Add(notebook, 1, wxGROW);
		notebook->SetAutoLayout(true);
	}
	if (notebook) {
		panel = new wxPanel(notebook, -1);
		panel->SetSizer(new wxBoxSizer(wxVERTICAL));
		panel->SetAutoLayout(true);
		if (parList->mode == UNIPAR_SET_CFLIST)
			notebookPanel = (int) notebook->GetPageCount() - 1;
		notebook->AddPage(panel, title);
	}
	return(panel);

}

/****************************** SetPanelledModuleInfo *************************/

/*
 * This routine sets the special information for modules that have parameters
 * separated into different notebook pages.
 */

void
ParListInfoList::SetPanelledModuleInfo(wxPanel *panel, DatumPtr pc,
  UniParListPtr parList, int offset, int panelNum)
{
	int		numPars;
	wxPanel	*newPanel;
	ParListInfo	*infoPtr;
	NameSpecifierPtr	panelSpec1, panelSpec2;

	panelSpec1 = ( *parList->GetPanelList)(panelNum);
	if (!panelSpec1->name)
		return;

	panelSpec2 = ( *parList->GetPanelList)(panelNum + 1);
	if ((parList->pars[panelSpec1->specifier].type != UNIPAR_CFLIST) &&
	  (newPanel = UsingNotebook(parList, (wxChar *) panelSpec1->name)) != NULL)
		panel = newPanel;
	numPars = panelSpec2->specifier - panelSpec1->specifier;
	infoPtr = new ParListInfo(panel, pc, parList, (int) list.Count(), offset,
	  numPars);
	list.Add(infoPtr);
	panel->GetSizer()->Add(infoPtr->GetSizer(), 0, wxALIGN_CENTER_VERTICAL);
	SetPanelledModuleInfo(panel, pc, parList, panelSpec2->specifier, panelNum +
	  1);
	SetSubParListInfo(infoPtr);

}

/****************************** SetSubParListInfo *****************************/

/*
 * This routine sets the sub-parameter list info information..
 */

void
ParListInfoList::SetSubParListInfo(ParListInfo *info)
{
	static const wxChar *funcName = wxT("ParListInfoList::SetSubParListInfo");
	int		i;
	UniParPtr	p;
	wxPanel		*panel = info->GetParent();
	DatumPtr	pc = info->GetPC();
	ParControl	*control;

	for (i = 0; i < info->GetNumPars(); i++) {
		control = info->GetParControl(i);
		if ((control->GetTag() == ParControl::SPECIAL) &&
		  (control->GetSpecialType() == PARCONTROL_SUB_LIST)) {
			p = control->GetPar();
			switch (p->type) {
			case UNIPAR_CFLIST: {
				CFListPtr	theCFs = *p->valuePtr.cFPtr;
				SetStandardInfo(panel, pc, theCFs->cFParList, wxT("CF List"));
				if ((theCFs->bandwidthMode.specifier !=
				  BANDWIDTH_INTERNAL_DYNAMIC) && (theCFs->bandwidthMode.
				  specifier !=BANDWIDTH_DISABLED))
					SetStandardInfo(panel, pc, theCFs->bParList, wxT(
					"Bandwidths"));
				break; }
			case UNIPAR_ICLIST: {
				IonChanListPtr	theICs = *p->valuePtr.iCPtr;
				DynaListPtr	node;
				IonChannelPtr	iC;

				SetStandardInfo(panel, pc, theICs->parList,  wxT("IC List ")
				  wxT("Gen."));
				for (node = theICs->ionChannels; node; node = node->next) {
					iC = (IonChannelPtr) node->data;
					SetStandardInfo(panel, pc, iC->parList, (wxChar *) iC->
					  description);
				}
				break; }
			case UNIPAR_MODULE: {
				const wxChar *moduleName;

				switch (pc->data->module->specifier) {
				case ANA_SAI_MODULE:
					moduleName = wxT("Strobe");
					break;
				default:
					moduleName = wxT("Sub-Module");
				}
				SetStandardInfo(panel, pc, p->valuePtr.module.parList,
				  (wxChar *) moduleName);
				break; }
			case UNIPAR_PARLIST: {
				if (!*p->valuePtr.parList.list)
					break;
				if (p->valuePtr.parList.process)
					SET_PARS_POINTER(*p->valuePtr.parList.process);
				if ((*p->valuePtr.parList.list)->GetPanelList)
					SetPanelledModuleInfo(panel, pc, *p->valuePtr.parList.list);
				else
					SetStandardInfo(panel, pc, *p->valuePtr.parList.list,
					  (wxChar *) p->desc);
				break; }
			case UNIPAR_PARARRAY: {
				ParArrayPtr	parArray = *p->valuePtr.pAPtr;
				SetStandardInfo(panel, pc, parArray->parList, (wxChar *)
				  parArray->name);
				break; }
			case UNIPAR_SIMSCRIPT: {
//				DatumPtr	pc = *p->valuePtr.simScript.simulation;
//
//				info->SetModuleListBox();
//				for (; pc != NULL; pc = pc->next)
//					if (pc->type == PROCESS) {
//						parent->dialogList.Add(new DialogInfo((wxObject *) pc,
//						  (int) UNIPAR_SIMSCRIPT));
//					}
//				
				break; }
			default:
				NotifyError(wxT("%s: Unknown universal parameter type (%d)."),
				  funcName, p->type);
				return;
			}
		}
	}

}

/****************************** SetStandardInfo *******************************/

/*
 * This routine sets the ParListInfo array.
 * It sets the list recursively.
 */

void
ParListInfoList::SetStandardInfo(wxPanel *panel, DatumPtr pc,
  UniParListPtr parList, const wxString& title, int offset, int numPars)
{
	wxPanel	*newPanel;
	ParListInfo	*infoPtr;

	if ((newPanel = UsingNotebook(parList, title)) != NULL)
		panel = newPanel;
	infoPtr = new ParListInfo(panel, pc, parList, (int) list.Count(), offset,
	  numPars);
	list.Add(infoPtr);
	panel->GetSizer()->Add(infoPtr->GetSizer(), 0, wxALIGN_CENTER_VERTICAL);
	parList->updateFlag = FALSE;
	SetSubParListInfo(infoPtr);

}

/****************************** UpdateAllControlValues ************************/

/*
 * This routine updates all of the control values in all of the 'ParListInfo'
 * records.
 */

void
ParListInfoList::UpdateAllControlValues(void)
{
	size_t	i;

	for (i = 0; i < list.Count(); i++)
		list[i]->UpdateControlValues();

}
