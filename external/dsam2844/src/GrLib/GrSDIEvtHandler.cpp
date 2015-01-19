/**********************
 *
 * File:		GrSDIEvtHandler.cpp
 * Purpose: 	Diagram class Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		13 Nov 2002
 * Updated:
 * Copyright:	(c) 2002, 2010 Lowel P. O'Mard
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

#ifdef USE_WX_OGL

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"
#include <wx/docview.h>
#include <wx/cmdproc.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtDynaList.h"
#include "UtDatum.h"
#include "GeModuleMgr.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtSimScript.h"
#include "UtString.h"

#include "GrSDIFrame.h"
#include "GrSDICanvas.h"
#include "GrSDIDiagram.h"
#include "GrSDIView.h"
#include "GrSDICommand.h"
#include "GrModParDialog.h"
#include "GrIPCServer.h"
#include "GrSimMgr.h"
#include "GrMainApp.h"
#include "DiSignalDisp.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"
#include "GrSDIEvtHandler.h"

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
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIEvtHandler::SDIEvtHandler(wxShapeEvtHandler *prev, wxShape *shape,
  const wxString& lab, int theProcessType): wxShapeEvtHandler(prev, shape)
{
	label = lab;
	processType = theProcessType;
	pc = NULL;
	dialog = NULL;

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDIEvtHandler::~SDIEvtHandler(void)
{
	if (dialog)
		delete dialog;

}

/******************************************************************************/
/****************************** ResetLabel ************************************/
/******************************************************************************/

/*
 * This routine resets the label using the various respective positions for
 * the different elements.
 */

void
SDIEvtHandler::ResetLabel(void)
{
	label.Empty();
	label.Printf(wxT("{ %s }\n"), pc->label);
	label.MakeLower();
	switch (pc->type) {
	case REPEAT: {
		wxString strCount;
		strCount.Printf(wxT("%s\n[ %d ]"), GetProcessName_Utility_Datum(pc),
		  pc->u.loop.count);
		label += strCount;
		break; }
	case RESET: {
		wxString str;
		str.Printf(wxT("%s\n[ %s ]"), GetProcessName_Utility_Datum(pc), pc->u.
		  ref.string);
		label += str;
		break; }
	case PROCESS: {
		if (pc->data->module->specifier != DISPLAY_MODULE) {
			label += (wxChar *) GetProcessName_Utility_Datum(pc);
			break;
		}
		wxString str;
		str.Printf(wxT("%s\n\"%s\""), GetProcessName_Utility_Datum(pc),
		  GetUniParPtr_ModuleMgr(pc->data, wxT("win_title"))->valuePtr.s);
		label += str;
		break; }
	default:
		label += wxT("\n");
		label += (wxChar *) GetProcessName_Utility_Datum(pc);
	}

}

/******************************************************************************/
/****************************** InitInstruction *******************************/
/******************************************************************************/

bool
SDIEvtHandler::InitInstruction(void)
{
	static const wxChar	*funcName = wxT("SDIEvtHandler::InitInstruction");

	if (pc) {
		NotifyError(wxT("%s: datum already initialised!"), funcName);
		return(false);
	}
	switch (processType) {
	case CONTROL_MODULE_CLASS: {
		SymbolPtr sp = LookUpSymbol_Utility_SSSymbols(wxGetApp().GetGrMainApp(
		  )->GetSymList(), (wxChar *) label.c_str());
		if ((pc = InitInst_Utility_Datum(sp->type)) == NULL) {
			NotifyError(wxT("%s: Could not create '%s' control intruction for ")
			  wxT("process '%s'."), funcName, (wxChar *) label.c_str());
			return(false);
		}
		break; }
	case ANALYSIS_MODULE_CLASS:
	case DISPLAY_MODULE_CLASS:
	case FILTER_MODULE_CLASS:
	case IO_MODULE_CLASS:
	case MODEL_MODULE_CLASS:
	case TRANSFORM_MODULE_CLASS:
	case USER_MODULE_CLASS:
	case UTILITY_MODULE_CLASS:
		if ((pc = InitInst_Utility_Datum(PROCESS)) == NULL) {
			NotifyError(wxT("%s: Could not create new intruction for process ")
			  wxT("'%s'."), funcName, (wxChar *) label.c_str());
			return(false);
		}
		break;
	default:
		NotifyError(wxT("%s: Unknown process type (%d).\n"), funcName,
		  processType);
		return(false);
	}
	EarObjectPtr simProcess = ((SDIDiagram *) GetShape()->GetCanvas()->
	  GetDiagram())->GetSimProcess();
	DynaBListPtr labelBList = (!simProcess)? NULL: ((SimScriptPtr) simProcess->
	  module->parsPtr)->labelBList;
	if (!SetDefaultLabel_Utility_Datum(pc, labelBList)) {
		NotifyError(wxT("%s: Could not create set the default process label."),
		  funcName);
		return(false);
	}
	if (simProcess && !Insert_Utility_DynaBList(((SimScriptPtr) simProcess->
	  module->parsPtr)->labelBListPtr, CmpProcessLabels_Utility_Datum, pc)) {
		NotifyError(wxT("%s: Could not insert label '%s' into the label list."),
		  funcName, pc->label);
		return(false);
	}
	pc->shapePtr = GetShape();
	return(true);

}

/******************************************************************************/
/****************************** EditInstruction *******************************/
/******************************************************************************/

bool
SDIEvtHandler::EditInstruction(void)
{
	static const wxChar	*funcName = wxT("SDIEvtHandler::EditInstruction");

	if (label.IsEmpty())
		return(true);
	switch (processType) {
	case CONTROL_MODULE_CLASS: {
		switch (pc->type) {
		case REPEAT: {
			long	count;

			label.ToLong(&count);
			if (count >= 1)
				pc->u.loop.count = count;
			ResetLabel();
			break; }
		default:
			;
		} /* switch */
		break; }
	case ANALYSIS_MODULE_CLASS:
	case DISPLAY_MODULE_CLASS:
	case FILTER_MODULE_CLASS:
	case IO_MODULE_CLASS:
	case MODEL_MODULE_CLASS:
	case TRANSFORM_MODULE_CLASS:
	case USER_MODULE_CLASS:
	case UTILITY_MODULE_CLASS:
		if (*pc->u.proc.moduleName != '\0')
			free(pc->u.proc.moduleName);
		pc->u.proc.moduleName = InitString_Utility_String((wxChar *) label.
		  c_str());
		if (!pc->data)
			InitProcessInst_Utility_Datum(pc);
		else {
			ResetProcess_EarObject(pc->data);
			Free_ModuleMgr(&(pc->data->module));
			pc->data->module = Init_ModuleMgr(pc->u.proc.moduleName);
		}
		pc->data->shapePtr = pc->shapePtr;
		break;
	default:
		NotifyError(wxT("%s: Unknown process type (%d).\n"), funcName,
		  processType);
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** FreeInstruction *******************************/
/******************************************************************************/

/*
 * This routine free's the memory for an instruction.
 * It does not manage the simulation connections, as these are taken care of by
 * the 'line' connection code.
 */

void
SDIEvtHandler::FreeInstruction(void)
{
	if (!pc)
		return;
	EarObjectPtr simProcess = ((SDIDiagram *) GetShape()->GetCanvas()->
	  GetDiagram())->GetSimProcess();
	if (simProcess) {
		DynaBListPtr *labelBListPtr = ((SimScriptPtr) simProcess->module->
		  parsPtr)->labelBListPtr;
		Remove_Utility_DynaBList(labelBListPtr, CmpProcessLabels_Utility_Datum,
		  pc);
	}
	FreeInstruction_Utility_Datum(&pc);

}

/******************************************************************************/
/****************************** SetSelectedShape ******************************/
/******************************************************************************/

/*
 * This routine returns TRUE if the shape was already selected.
 */

bool
SDIEvtHandler::SetSelectedShape(wxClientDC &dc)
{
	bool	alreadySelected = false;

	if (GetShape()->Selected())
		alreadySelected = true;
	else {
		// Ensure no other shape is selected, to simplify Undo/Redo code
		bool redraw = ((SDIDiagram *) GetShape()->GetCanvas()->GetDiagram())->
		  UnselectAllShapes();
		GetShape()->Select(true, &dc);
		if (redraw) {
			if (pc && (pc->type == REPEAT)) {
				wprintf(wxT("SDIEvtHandler::SetSelectedShape: Redraw repeat shape\n"));
				wxNode *node = GetShape()->GetChildren().GetFirst();
				((SDIShape *) node->GetData())->ResetLabel();
			} else
				GetShape()->GetCanvas()->Redraw(dc);
		}
	}
	return(alreadySelected);

}

/******************************************************************************/
/****************************** ProcessProperties *****************************/
/******************************************************************************/

/*
 * This routine returns TRUE if the shape was already selected.
 */

void
SDIEvtHandler::ProcessProperties(double x, double y)
{
	if (!pc) {
		if (GetShape()->IsKindOf(CLASSINFO(wxLineShape)))
			return;
		SDICanvas *canvas = (SDICanvas *) GetShape()->GetCanvas();
		((SDIView *) canvas->view)->ProcessListDialog();
	} else if (!dialog) {
		switch (pc->type) {
		case PROCESS: {
			UniParListPtr	parList = GetUniParListPtr_ModuleMgr(pc->data);
			wxString	title;

			if (!parList)
				break;
			switch (parList->mode) {
			case UNIPAR_SET_SIMSPEC: {
				DSAM_printf(wxT("SDIEvtHandler::OnLeftDoubleClick: Open child ")
				  wxT("SDI window.\n"));
				break; }
			default: {
				int		winX, winY;
				SDICanvas	*canvas = (SDICanvas *) GetShape()->GetCanvas();
				canvas->parent->GetPosition(&winX, &winY);

				title = (wxChar *) ((pc->data->module->specifier ==
				  DISPLAY_MODULE)? parList->pars[DISPLAY_WINDOW_TITLE].valuePtr.
				  s: NameAndLabel_Utility_Datum(pc));
				dialog = new ModuleParDialog(canvas->parent, title, pc,
				  parList, this, (int) (winX + x), (int) (winY + y), 500,
				  500, wxDEFAULT_DIALOG_STYLE);
				wxGetApp().GetFrame()->AddToDialogList(dialog);
				dialog->SetNotebookSelection();
				dialog->Show(TRUE);
				}
			} /* switch */
			break; }
		case REPEAT: {
			SDICanvas *canvas = (SDICanvas *) GetShape()->GetCanvas();
			((SDIView *) canvas->view)->EditCtrlProperties();
			break; }
		default:
			;
		} /* switch */
	}

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** OnLeftClick ***********************************/
/******************************************************************************/

/*
 * SDIEvtHandler: an event handler class for all shapes
 */
 
void
SDIEvtHandler::OnLeftClick(double x, double y, int keys, int attachment)
{
	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	if (keys == 0) {
		if (SetSelectedShape(dc)) {
			GetShape()->Select(false, &dc);
			// Redraw because bits of objects will be are missing
			GetShape()->GetCanvas()->Redraw(dc);
		}
	} else 
		if (keys & KEY_CTRL) {
			printf("SDIEvtHandler::OnLeftClick: Debug: ctrl left click.\n");
		} else {
			printf("SDIEvtHandler::OnLeftClick: Debug: shift left click.\n");
		}

}

/******************************************************************************/
/****************************** OnLeftDoubleClick *****************************/
/******************************************************************************/

/*
 * SDIEvtHandler: an event handler class for all shapes
 */

/*
 * If this routine is called, and the instruction is unset, then call the set
 * instruction dialog, otherwiase edit the instruction parameters.
 * Any subsequent changes of instruction needs to be done using "right click".
 */

void
SDIEvtHandler::OnLeftDoubleClick(double x, double y, int keys, int attachment)
{
	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	if (keys == 0) {
		SetSelectedShape(dc);
		ProcessProperties(x, y);
	} else
		if (keys & KEY_CTRL) {
 		   // Do something for CONTROL
		} else {
		   // Do something for for SHIFT ?
		}

}

/******************************************************************************/
/****************************** OnRightClick **********************************/
/******************************************************************************/

/*
 * SDIEvtHandler: an event handler class for all shapes
 */

void
SDIEvtHandler::OnRightClick(double x, double y, int keys, int attachment)
{
	SDICanvas *canvas = (SDICanvas *)GetShape()->GetCanvas();
	wxClientDC dc(canvas);
	canvas->PrepareDC(dc);

	if (keys == 0) {
		SetSelectedShape(dc);
		if (!pc)
			return;
		wxMenu menu(wxT("Edit Process"));
		menu.Append(SDIFRAME_EDIT_MENU_ENABLE, (pc->data->module->onFlag)?
		  wxT("Disa&ble"): wxT("Ena&ble"), wxT("Enable/disable process"));
		menu.AppendSeparator();
		menu.Append(SDIFRAME_EDIT_MENU_READ_PAR_FILE, wxT("&Read par. file"),
		  wxT("Read parameter file."));
		menu.Append(SDIFRAME_EDIT_MENU_WRITE_PAR_FILE, wxT("&Write par. file"),
		  wxT("Write parameter file."));
		menu.AppendSeparator();
		menu.Append(SDIFRAME_EDIT_MENU_PROPERTIES, wxT("&Properties..."));

		canvas->PopupMenu(&menu, (int) x, (int) y);

	} else
		if (keys & KEY_CTRL) {
			printf("SDIEvtHandler::OnRightClick: Debug: ctrl left click.\n");
		} else {
			printf("SDIEvtHandler::OnRightClick: Debug: shift left click.\n");
		}

}

/******************************************************************************/
/****************************** OnBeginDragRight ******************************/
/******************************************************************************/

/*
 * Implement connection of two shapes by right-dragging between them.
 */

void
SDIEvtHandler::OnBeginDragRight(double x, double y, int keys, int attachment)
{
	// Force attachment to be zero for now. Eventually we can deal with
	// the actual attachment point, e.g. a rectangle side if attachment mode
	// is on.
	attachment = 0;

	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
	dc.SetLogicalFunction(OGLRBLF);
	dc.SetPen(dottedPen);
	double xp, yp;
	GetShape()->GetAttachmentPosition(attachment, &xp, &yp);
	dc.DrawLine((long) xp, (long) yp, (long) x, (long) y);
	GetShape()->GetCanvas()->CaptureMouse();

}

/******************************************************************************/
/****************************** OnDragRight ***********************************/
/******************************************************************************/

void
SDIEvtHandler::OnDragRight(bool draw, double x, double y, int keys,
  int attachment)
{
	// Force attachment to be zero for now
	attachment = 0;

	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
	dc.SetLogicalFunction(OGLRBLF);
	dc.SetPen(dottedPen);
	double xp, yp;
	GetShape()->GetAttachmentPosition(attachment, &xp, &yp);
	dc.DrawLine((long) xp, (long) yp, (long) x, (long) y);

}

/******************************************************************************/
/****************************** OnEndDragRight ********************************/
/******************************************************************************/

void
SDIEvtHandler::OnEndDragRight(double x, double y, int keys, int attachment)
{
	GetShape()->GetCanvas()->ReleaseMouse();
	SDICanvas *canvas = (SDICanvas *)GetShape()->GetCanvas();

	// Check if we're on an object
	int new_attachment;
	wxShape *otherShape = canvas->FindFirstSensitiveShape(x, y, &new_attachment,
	  OP_DRAG_RIGHT);

	if (!otherShape || otherShape->IsKindOf(CLASSINFO(wxLineShape)))
		return;

	DatumPtr	pc = SHAPE_PC(GetShape());
	if (pc && pc->next) {
		switch (pc->type) {
		case REPEAT:
			if (pc->u.loop.stopPC)	/* Existing repeat connection */
				break;
			canvas->view->GetDocument()->GetCommandProcessor()->Submit(
			  new SDICommand(wxT("'repeat' connection"),
			  SDIFRAME_ADD_REPEAT_LINE, (SDIDocument *) canvas->view->
			  GetDocument(), CLASSINFO(wxLineShape), -1, 0.0, 0.0, FALSE, NULL,
			  GetShape(), otherShape));
			return;
		case RESET:
			canvas->view->GetDocument()->GetCommandProcessor()->Submit(
			  new SDICommand(wxT("Set 'reset'"), SDIFRAME_SET_RESET,
			  (SDIDocument *) canvas->view->GetDocument(), NULL,
			  -1, 0.0, 0.0, FALSE, NULL, GetShape(), otherShape));
			return;
		default:
			;
		}

	}
	canvas->view->GetDocument()->GetCommandProcessor()->Submit(new SDICommand(
	  wxT("process connection"), SDIFRAME_ADD_LINE, (SDIDocument *) canvas->
	  view->GetDocument(), CLASSINFO(wxLineShape), -1, 0.0, 0.0, FALSE, NULL,
	  GetShape(), otherShape));

}

/******************************************************************************/
/****************************** OnEndSize *************************************/
/******************************************************************************/

void
SDIEvtHandler::OnEndSize(double x, double y)
{
	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	if (processType == CONTROL_MODULE_CLASS) {
		wxNode *node = GetShape()->GetChildren().GetFirst();
		((SDIShape *) node->GetData())->ResetLabel();
	} else
		GetShape()->FormatText(dc, label);
}

/******************************************************************************/
/****************************** OnMovePost ************************************/
/******************************************************************************/

void
SDIEvtHandler::OnMovePost(wxDC& dc, double x, double y, double oldX,
  double oldY, bool display)
{
	((SDICanvas *) GetShape()->GetCanvas())->view->GetDocument()->Modify(true);

}

#endif /* USE_WX_OGL */
