/**********************
 *
 * File:		GrSDIView.cpp
 * Purpose: 	Implements view functionality for Simulation Design Interface
 * Comments:	Revised from Julian Smart's Ogledit/view.cpp
 * Author:		L.P.O'Mard
 * Created:		04 Nov 2002
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

#include "ExtCommon.h"

#include <wx/docview.h>
#include <wx/colordlg.h>
#include <wx/cmdproc.h>

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "GrIPCServer.h"
#include "GrSDIPalette.h"
#include "GrSDICanvas.h"
#include "GrSDIFrame.h"
#include "GrSimMgr.h"
#include "GrSDIDiagram.h"
#include "GrSDIEvtHandler.h"
#include "GrSDIDoc.h"
#include "GrSDIView.h"
#include "GrSDICommand.h"

#include "GeUniParMgr.h"
#include "UtAppInterface.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIView, wxView)

BEGIN_EVENT_TABLE(SDIView, wxView)
    EVT_COMBOBOX(SDIFRAME_ZOOM_COMBOBOX, SDIView::OnZoomSel)
	EVT_MENU(SDIFRAME_CUT, SDIView::OnCut)
	EVT_MENU(SDIFRAME_CHANGE_BACKGROUND_COLOUR,
	  SDIView::OnChangeBackgroundColour)
	EVT_MENU(SDIFRAME_EDIT_MENU_ENABLE, SDIView::OnEditEnable)
	EVT_MENU(SDIFRAME_EDIT_MENU_PROPERTIES, SDIView::OnEditProperties)
	EVT_MENU(SDIFRAME_EDIT_MENU_READ_PAR_FILE, SDIView::OnReadParFile)
	EVT_MENU(SDIFRAME_EDIT_MENU_WRITE_PAR_FILE, SDIView::OnWriteParFile)
	EVT_MENU(SDIFRAME_EDIT_PROCESS, SDIView::OnSetProcessLabel)
    EVT_TEXT_ENTER(SDIFRAME_ZOOM_COMBOBOX, SDIView::OnZoomSel)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** ProcessListDialog *****************************/
/******************************************************************************/

void
SDIView::ProcessListDialog(void)
{
	wxShape *theShape = FindSelectedShape();

	if (theShape) {
		wxArrayString	*procList = wxGetApp().GetProcessList(((SDIEvtHandler *)
		  theShape->GetEventHandler())->processType);
		wxSingleChoiceDialog dialog(theShape->GetCanvas(), wxT("Select a ")
		  wxT("process"), wxT("Please select a process"), procList->Count(),
		  &procList->Item(0));

		if (dialog.ShowModal() == wxID_OK) {
			SDICanvas *canvas = (SDICanvas *) theShape->GetCanvas();
			canvas->view->GetDocument()->GetCommandProcessor()->Submit(
			  new SDICommand(wxT("Edit process name"), SDIFRAME_EDIT_PROCESS,
			  (SDIDocument *) canvas->view->GetDocument(),
			  dialog.GetStringSelection(), theShape));

		}
	}

}

/******************************************************************************/
/****************************** EditCtrlProperties ****************************/
/******************************************************************************/

void
SDIView::EditCtrlProperties(void)
{
	wxShape *theShape = FindSelectedShape();

	if (theShape) {
		DatumPtr	pc = ((SDIEvtHandler *) theShape->GetEventHandler())->pc;
		SDICanvas *canvas = (SDICanvas *) theShape->GetCanvas();

		switch (pc->type) {
		case REPEAT: {
			wxString	oldStrCount;
			oldStrCount.Printf(wxT("%d"), pc->u.loop.count);
			wxString newStrCount = wxGetTextFromUser(wxT("Enter repeat count"),
			  wxT("Shape Control Par"), oldStrCount);
			canvas->view->GetDocument()->GetCommandProcessor()->Submit(
			  new SDICommand(wxT("Edit control parameters"),
			  SDIFRAME_EDIT_PROCESS, (SDIDocument *) canvas->view->
			  GetDocument(), newStrCount, theShape));
			break; }
		default:
			;
		} /* switch */
	}

}

/******************************************************************************/
/****************************** OnCreate **************************************/
/******************************************************************************/

// What to do when a view is created. Creates actual
// windows for displaying the view.

bool
SDIView::OnCreate(wxDocument *doc, long flags)
{
	frame = wxGetApp().GetFrame();
	//frame->palette = new EditorToolPalette(frame, wxPoint(0, 0), wxSize(-1,
	//  -1),wxTB_HORIZONTAL);
	canvas = frame->canvas;
	canvas->view = this;

	ResetGUIDialogs();

	SetFrame(frame);
	Activate(TRUE);

	// Initialize the edit menu Undo and Redo items
	doc->GetCommandProcessor()->SetEditMenu(((SDIFrame *) frame)->editMenu);
	doc->GetCommandProcessor()->Initialize();


	wxShapeCanvas *shapeCanvas = (wxShapeCanvas *)canvas;
	SDIDocument *diagramDoc = (SDIDocument *)doc;
	shapeCanvas->SetDiagram(diagramDoc->GetDiagram());
	diagramDoc->GetDiagram()->SetCanvas(shapeCanvas);

	return TRUE;
}

/******************************************************************************/
/****************************** OnDraw ****************************************/
/******************************************************************************/

#define CENTER  FALSE // Place the drawing to the center of the page


// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void
SDIView::OnDraw(wxDC *dc)
{
	printf("SDIView::OnDraw: Entered\n");
	/* You might use THIS code if you were scaling
	* graphics of known size to fit on the page.
	*/
	int w, h;

	// We need to adjust for the graphic size, a formula will be added
	float maxX = 900;
	float maxY = 700;
	// A better way of find the maxium values would be to search through
	// the linked list

	// Let's have at least 10 device units margin
	float marginX = 10;
	float marginY = 10;

	// Add the margin to the graphic size
	maxX += (2 * marginX);
	maxY += (2 * marginY);

	// Get the size of the DC in pixels
	dc->GetSize (&w, &h);

	// Calculate a suitable scaling factor
	float scaleX = (float) (w / maxX);
	float scaleY = (float) (h / maxY);

	// Use x or y scaling factor, whichever fits on the DC
	float actualScale = wxMin (scaleX, scaleY);

	float posX, posY;
	// Calculate the position on the DC for centring the graphic
	if (CENTER == TRUE) { // center the drawing
		posX = (float) ((w - (200 * actualScale)) / 2.0);
		posY = (float) ((h - (200 * actualScale)) / 2.0);
	} else {   // Use defined presets
		posX = 10;
		posY = 35;
	}

	// Set the scale and origin
	dc->SetUserScale (actualScale, actualScale);
	dc->SetDeviceOrigin ((long) posX, (long) posY);

	// This part was added to preform the print preview and printing functions

	// Get the current diagram
	wxDiagram *diagram_p=((SDIDocument*)GetDocument())->GetDiagram();
	if (diagram_p->GetShapeList()) {
		/*wxCursor *old_cursor = NULL; */
		wxNode *current = diagram_p->GetShapeList()->GetFirst();

		while (current) {// Loop through the entire list of shapes
			wxShape *object = (wxShape *)current->GetData();
			if (!object->GetParent())
				object->Draw(* dc); // Draw the shape onto our printing dc
			current = current->GetNext();//Procede to the next shape in the list
		}
	}
}

/******************************************************************************/
/****************************** OnUpdate **************************************/
/******************************************************************************/

void
SDIView::OnUpdate(wxView *sender, wxObject *hint)
{

	if (canvas)
		canvas->Refresh();

}

/******************************************************************************/
/****************************** OnClose ***************************************/
/******************************************************************************/

// Clean up windows used for displaying the view.
bool
SDIView::OnClose(bool deleteWindow)
{
	if (!GetDocument()->Close())
		return FALSE;

	SDIDocument *diagramDoc = (SDIDocument *)GetDocument();
	diagramDoc->GetDiagram()->SetCanvas(NULL);

	canvas->ClearBackground();
	canvas->SetDiagram(NULL);
	canvas->view = NULL;
	canvas = NULL;

	wxString s = wxTheApp->GetAppName();
	if (frame)
		frame->SetTitle(s);

	SetFrame((wxFrame *) NULL);

	Activate(FALSE);

	return TRUE;

}

/******************************************************************************/
/****************************** FindSelectedShape *****************************/
/******************************************************************************/

wxShape *
SDIView::FindSelectedShape(void)
{
	SDIDocument *doc = (SDIDocument *)GetDocument();
	wxShape *theShape = NULL;
	wxNode *node = doc->GetDiagram()->GetShapeList()->GetFirst();
	while (node) {
		wxShape *eachShape = (wxShape *)node->GetData();
		if (eachShape->Selected()) {
			theShape = eachShape;
			node = NULL;
		} else
			node = node->GetNext();
	}
	return theShape;
}

/******************************************************************************/
/****************************** OnCut *****************************************/
/******************************************************************************/

void
SDIView::OnCut(wxCommandEvent& event)
{
	SDIDocument *doc = (SDIDocument *)GetDocument();

	wxShape *theShape = FindSelectedShape();
	if (theShape)
		doc->GetCommandProcessor()->Submit(new SDICommand(wxT("Cut"),
		  SDIFRAME_CUT, doc, NULL, -1, 0.0, 0.0, TRUE, theShape));

}

/******************************************************************************/
/****************************** OnChangeBackgroundColour **********************/
/******************************************************************************/

void
SDIView::OnChangeBackgroundColour(wxCommandEvent& event)
{
	SDIDocument *doc = (SDIDocument *)GetDocument();

	wxShape *theShape = FindSelectedShape();
	if (theShape) {
		wxColourData data;
		data.SetChooseFull(TRUE);
		data.SetColour(theShape->GetBrush()->GetColour());

		wxColourDialog *dialog = new wxColourDialog(frame, &data);
		wxBrush *theBrush = NULL;
		if (dialog->ShowModal() == wxID_OK) {
			wxColourData retData = dialog->GetColourData();
			wxColour col = retData.GetColour();
			theBrush = wxTheBrushList->FindOrCreateBrush(col, wxSOLID);
		}
		dialog->Close();

		if (theBrush)
			doc->GetCommandProcessor()->Submit(new SDICommand(wxT("Change ")
			  wxT("colour"), SDIFRAME_CHANGE_BACKGROUND_COLOUR, doc, theBrush,
			  theShape));
	}

}

/******************************************************************************/
/****************************** OnSetProcessLabel *****************************/
/******************************************************************************/

void
SDIView::OnSetProcessLabel(wxCommandEvent& WXUNUSED(event))
{
	ProcessListDialog();

}

/******************************************************************************/
/****************************** OnEditEnable **********************************/
/******************************************************************************/

/*
 * This routine toggles the process' enabled status.
 * The "Move" call redraw's the shape.
 */

void
SDIView::OnEditEnable(wxCommandEvent& WXUNUSED(event))
{
	wxShape *shape = FindSelectedShape();
	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();
	Enable_ModuleMgr(myHandler->pc->data, !myHandler->pc->data->module->onFlag);
	shape->SetBrush((myHandler->pc->data->module->onFlag)?
	  DIAGRAM_ENABLED_BRUSH: DIAGRAM_DISENABLED_BRUSH);

	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
	shape->Move(dc, shape->GetX(), shape->GetY());

}

/******************************************************************************/
/****************************** OnEditProperties ******************************/
/******************************************************************************/

void
SDIView::OnEditProperties(wxCommandEvent& WXUNUSED(event))
{
	wxShape *shape = FindSelectedShape();
	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();

	myHandler->ProcessProperties(shape->GetX(), shape->GetY());

}

/******************************************************************************/
/****************************** OnReadParFile *********************************/
/******************************************************************************/

void
SDIView::OnReadParFile(wxCommandEvent& WXUNUSED(event))
{
	static const wxChar *funcName = wxT("SDIView::OnReadParFile");

	wxShape *shape = FindSelectedShape();
	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();

	wxFileDialog dialog(shape->GetCanvas(), wxT("Choose a file"), wxGetCwd());
	if (dialog.ShowModal() != wxID_OK)
		return;
	wxFileName fileName = dialog.GetPath();
	if (!ReadPars_ModuleMgr(myHandler->pc->data, (wxChar *) fileName.
	  GetFullPath().c_str()))
		NotifyWarning(wxT("%s: Could not read parameters from file '%s'."),
		  funcName, fileName.GetFullPath().c_str());

}

/******************************************************************************/
/****************************** OnWriteParFile ********************************/
/******************************************************************************/

void
SDIView::OnWriteParFile(wxCommandEvent& WXUNUSED(event))
{
	static const wxChar *funcName = wxT("SDIView::OnWriteParFile");

	wxShape *shape = FindSelectedShape();
	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();

	wxFileDialog dialog(shape->GetCanvas(), wxT("Save a file"), wxGetCwd(),
	  wxT(""), wxT("*.par"), wxSAVE);
	if (dialog.ShowModal() != wxID_OK)
		return;
	wxFileName fileName = dialog.GetPath();

	DiagModeSpecifier	oldDiagMode = GetDSAMPtr_Common()->diagMode;
	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
	if (!WritePars_ModuleMgr((wxChar *) fileName.GetFullPath().c_str(),
	  myHandler->pc->data))
		NotifyWarning(wxT("%s: Could not write parameters to file '%s'."),
		  funcName, fileName.GetFullPath().c_str());
	SetDiagMode(oldDiagMode);

}

/******************************************************************************/
/****************************** OnZoomSel *************************************/
/******************************************************************************/

void
SDIView::OnZoomSel(wxCommandEvent& event)
{
	double	scale, percent;

	wxComboBox* combo = (wxComboBox*) event.GetEventObject();
	wxASSERT( combo != NULL );

	combo->GetValue().ToDouble(&percent);
	scale = percent / 100.0;

	SDIDocument *diagramDoc = (SDIDocument *)GetDocument();
	((SDIDiagram *) diagramDoc->GetDiagram())->Rescale(scale, scale);
	canvas->Refresh();

}

#endif /* HAVE_WX_OGL_H */
