/**********************
 *
 * File:		GrSDICommand.cpp
 * Purpose: 	Command class for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 *				Most user interface commands are routed through this, to give
 *				us the Undo/Redo mechanism. If you add more commands, such as
 *				changing the shape colour, you will need to add members to
 *				'remember' what the user applied (for 'Do') and what the
 *				previous state was (for 'Undo').
 *				You can have one member for each property to be changed.
 *				Assume we also have a pointer member wxShape *shape, which is
 *				set to the shape being changed. Let's assume we're changing
 *				the shape colour. Our member for this is shapeColour.
 *				- In 'Do':
 *					o Set a temporary variable 'temp' to the current colour
 *						for 'shape'.
 *					o Change the colour to the new colour.
 *					o Set shapeColour to the _old_ colour, 'temp'.
 *				- In 'Undo':
 *					o Set a temporary variable 'temp' to the current colour
 *						for 'shape'.
 *					o Change the colour to shapeColour (the old colour).
 *					o Set shapeColour to 'temp'.
 *
 *				So, as long as we have a pointer to the shape being changed, we
 *				only need one member variable for each property.
 *
 *				PROBLEM: when an Add shape command is redone, the 'shape'
 *				pointer changes.
 *				Assume, as here, that we keep a pointer to the old shape so we
 *				reuse it when we recreate.
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

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/docview.h>
#include <wx/cmdproc.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtAppInterface.h"

#include "GrSDIDiagram.h"
#include "GrSDIDoc.h"
#include "GrSDIFrame.h"
#include "GrSDICommand.h"
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
/****************************** Constructor 1 *********************************/
/******************************************************************************/

/*
 * Implementation of drawing command
 */

SDICommand::SDICommand(const wxString& name, int command, SDIDocument *ddoc,
  wxClassInfo *info, int theProcessType, double xx, double yy, bool sel,
  wxShape *theShape, wxShape *fs, wxShape *ts): wxCommand(TRUE, name)
{
	SetBasic(command, ddoc, theShape);
	fromShape = fs;
	toShape = ts;
	shapeInfo = info;
	processType = theProcessType;
	x = xx;
	y = yy;
	selected = sel;

}

/******************************************************************************/
/****************************** Constructor 2 *********************************/
/******************************************************************************/

SDICommand::SDICommand(const wxString& name, int command, SDIDocument *ddoc,
  wxBrush *backgroundColour, wxShape *theShape): wxCommand(TRUE, name)
{
	SetBasic(command, ddoc, theShape);
	shapeBrush = backgroundColour;

}

/******************************************************************************/
/****************************** Constructor 3 *********************************/
/******************************************************************************/

SDICommand::SDICommand(const wxString& name, int command, SDIDocument *ddoc,
  const wxString& lab, wxShape *theShape): wxCommand(TRUE, name)
{
	SetBasic(command, ddoc, theShape);
	shapeLabel = lab;

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDICommand::~SDICommand(void)
{
	if (shape && deleteShape) {
		shape->SetCanvas(NULL);
		delete shape;
	}

}

/******************************************************************************/
/****************************** SetBasic **************************************/
/******************************************************************************/

void
SDICommand::SetBasic(int command, SDIDocument *ddoc, wxShape *theShape)
{
	doc = ddoc;
	cmd = command;
	shape = theShape;
	x = 0.0;
	y = 0.0;
	selected = FALSE;
	shapePen = NULL;
	deleteShape = FALSE;
	fromShape = NULL;
	toShape = NULL;
	shapeInfo = NULL;
	shapeBrush = NULL;
	processType = -1;

}

/******************************************************************************/
/****************************** ConnectInstructions ***************************/
/******************************************************************************/

/*
 * This routine connects two specified processes and inserts them into the
 * simulation.
 * It also inserts the instructions into the simulation.
 */

bool
SDICommand::ConnectInstructions(wxShape *fromShape, wxShape *toShape)
{
	static const wxChar *funcName = wxT("SDICommand::ConnectInstructions");
	bool	ok = true;

	DatumPtr	toPc = SHAPE_PC(toShape);
	DatumPtr	fromPc = SHAPE_PC(fromShape);

	if (!fromPc || !toPc) {
		wxLogError(wxT("%s: Both processes must be set before a connection\n")
		  wxT("can be made."), funcName);
		return(false);
	}
	// Ensure only process instructions are connected for data flow.
	if (FindNearestProcesses_Utility_Datum(&fromPc, &toPc))
		ok = CXX_BOOL(ConnectOutSignalToIn_EarObject(fromPc->data, toPc->data));
	if (ok)
		ok = CXX_BOOL(ConnectInst_Utility_Datum(GetSimPtr_AppInterface(),
		  fromPc, toPc));
	return(ok);

}

/******************************************************************************/
/****************************** DisconnectProcessInsts ************************/
/******************************************************************************/

/*
 * This routine disconnects two specified processes.
 * The temporary 'pc' pointers are used because the
 * 'FindNearestProcesses_Utility_Datum' routine changes the pointers passed as
 * arguments.
 */

void
SDICommand::DisconnectProcessInsts(wxShape *fromShape, wxShape *toShape)
{
	DatumPtr	tempToPc = SHAPE_PC(toShape);
	DatumPtr	tempFromPc = SHAPE_PC(fromShape);

	if (FindNearestProcesses_Utility_Datum(&tempFromPc, &tempToPc))
		DisconnectOutSignalFromIn_EarObject(tempFromPc->data, tempToPc->data);
	DisconnectInst_Utility_Datum(GetSimPtr_AppInterface(), SHAPE_PC(fromShape),
	  SHAPE_PC(toShape));
		
}

/******************************************************************************/
/****************************** RedrawShapeLabel ******************************/
/******************************************************************************/

void
SDICommand::RedrawShapeLabel(wxShape *shape)
{
//*** you are here
//*** put some of this this into GrBasicShape?

	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
    SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();

//	((SDIDiagram *) doc->GetDiagram())->AdjustShapeToLabel(dc, shape,
//	  myHandler->label);
	shape->FormatText(dc, myHandler->label);
	shape->Draw(dc);

	doc->Modify(TRUE);
	doc->UpdateAllViews();
}

/******************************************************************************/
/****************************** AddLineShape **********************************/
/******************************************************************************/

bool
SDICommand::AddLineShape(int lineType)
{
	wxShape *theShape = NULL;
	if (shape)
		theShape = shape; // Saved from undoing the line
	else if (!SHAPE_PC(fromShape) || !SHAPE_PC(toShape))
		return(false);
	else {
		switch (lineType) {
		case REPEAT:
			if (!InsertInst_Utility_Datum(GetSimPtr_AppInterface(), SHAPE_PC(
			  toShape), SHAPE_PC(fromShape)->u.loop.stopPC))
				return(false);
			SHAPE_PC(fromShape)->u.loop.stopPC = InitInst_Utility_Datum(STOP);
			break;
		default:
			if (!ConnectInstructions(fromShape, toShape)) {
				return(false);
			}
		} /* switch */
		theShape = ((SDIDiagram *) doc->GetDiagram())->AddLineShape(fromShape,
		  toShape, lineType);
	}

	shape = theShape;
	deleteShape = FALSE;

	// Create processes if necessary and connect them

	doc->Modify(TRUE);
	doc->UpdateAllViews();
	return(true);

}

/******************************************************************************/
/****************************** Do ********************************************/
/******************************************************************************/

bool
SDICommand::Do(void)
{
	switch (cmd) {
	case SDIFRAME_CUT: {
		if (shape) {
			SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler(
			  );
			deleteShape = TRUE;

			shape->Select(FALSE);

			// Generate commands to explicitly remove each connected line.
			if (!shape->IsKindOf(CLASSINFO(wxLineShape)))
				RemoveLines(shape);

			doc->GetDiagram()->RemoveShape(shape);
			if (shape->IsKindOf(CLASSINFO(wxLineShape))) {
				wxLineShape *lineShape = (wxLineShape *)shape;
				fromShape = lineShape->GetFrom();
				toShape = lineShape->GetTo();
				if (myHandler->processType == REPEAT)
					FreeInstFromSim_Utility_Datum(GetSimPtr_AppInterface(),
					  SHAPE_PC(fromShape)->u.loop.stopPC);
				else
					DisconnectProcessInsts(fromShape, toShape);
			} else {
				myHandler->FreeInstruction();
			}
			shape->Unlink();

			doc->Modify(TRUE);
			doc->UpdateAllViews();
		}

		break; }
	case SDIFRAME_ADD_SHAPE: {
		SDIDiagram *diagram = (SDIDiagram *) doc->GetDiagram();
		wxShape *theShape = NULL;
		if (shape)
			theShape = shape; // Saved from undoing the shape
		else {
			theShape = ((SDIDiagram *) doc->GetDiagram())->CreateBasicShape(
			  shapeInfo, processType, wxCYAN_BRUSH);
			theShape->SetSize(DIAGRAM_DEFAULT_SHAPE_WIDTH * diagram->GetXScale(
			  ), DIAGRAM_DEFAULT_SHAPE_HEIGHT * diagram->GetYScale());
		}
		doc->GetDiagram()->AddShape(theShape);
		theShape->Show(TRUE);

		wxClientDC dc(theShape->GetCanvas());
		theShape->GetCanvas()->PrepareDC(dc);

		theShape->Move(dc, x, y);

		shape = theShape;
		deleteShape = FALSE;

		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_ADD_LINE: {
		if (!AddLineShape(-1))
			return(false);
		break; }
	case SDIFRAME_ADD_REPEAT_LINE: {
		if (!AddLineShape(REPEAT))
			return(false);
		break; }
	case SDIFRAME_CHANGE_BACKGROUND_COLOUR: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);

			wxBrush *oldBrush = shape->GetBrush();
			shape->SetBrush(shapeBrush);
			shapeBrush = oldBrush;
			shape->Draw(dc);

			doc->Modify(TRUE);
			doc->UpdateAllViews();
		}
		break; }
	case SDIFRAME_EDIT_PROCESS: {
		if (shape) {
			SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler(
			  );
			wxString oldLabel(myHandler->label);
			myHandler->label = shapeLabel;
			shapeLabel = oldLabel;

			if (!myHandler->pc)
				myHandler->InitInstruction();
			myHandler->EditInstruction();

			RedrawShapeLabel(shape);
		}
		break; }
	case SDIFRAME_SET_RESET: {
		DatumPtr	toPc = SHAPE_PC(toShape);
		DatumPtr	fromPc = SHAPE_PC(fromShape);
		if (!toPc || (toPc->type != PROCESS))
			return(false);

		savedInfo = fromPc;
		shapeLabel = (wxChar *) fromPc->u.ref.string;
		fromPc->u.ref.string = toPc->label;

		fromPc->u.ref.pc = toPc;
		((SDIEvtHandler *) fromShape->GetEventHandler())->ResetLabel();
		RedrawShapeLabel(fromShape);
		RedrawShapeLabel(toShape);
		break; }
	}
	return TRUE;

}

/******************************************************************************/
/****************************** Undo ******************************************/
/******************************************************************************/

bool
SDICommand::Undo(void)
{
	switch (cmd) {
	case SDIFRAME_CUT: {
		if (shape) {
			doc->GetDiagram()->AddShape(shape);
			shape->Show(TRUE);

			if (shape->IsKindOf(CLASSINFO(wxLineShape))) {
				wxLineShape *lineShape = (wxLineShape *)shape;
				fromShape->AddLine(lineShape, toShape);
				ConnectInstructions(fromShape, toShape);
			}
			if (selected)
				shape->Select(TRUE);

			deleteShape = FALSE;
		}
		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_ADD_SHAPE: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);

			shape->Select(FALSE, &dc);
			doc->GetDiagram()->RemoveShape(shape);
			shape->Unlink();
			deleteShape = TRUE;
		}
		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_ADD_LINE: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);
			DisconnectProcessInsts(fromShape, toShape);
			shape->Select(FALSE, &dc);
			doc->GetDiagram()->RemoveShape(shape);
			shape->Unlink();
			deleteShape = TRUE;
		}
		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_ADD_REPEAT_LINE: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);
			FreeInstFromSim_Utility_Datum(GetSimPtr_AppInterface(), SHAPE_PC(
			  fromShape)->u.loop.stopPC);
			shape->Select(FALSE, &dc);
			doc->GetDiagram()->RemoveShape(shape);
			shape->Unlink();
			deleteShape = TRUE;
		}
		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_CHANGE_BACKGROUND_COLOUR: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);

			wxBrush *oldBrush = shape->GetBrush();
			shape->SetBrush(shapeBrush);
			shapeBrush = oldBrush;
			shape->Draw(dc);

			doc->Modify(TRUE);
			doc->UpdateAllViews();
		}
		break; }
	case SDIFRAME_EDIT_PROCESS: {
		if (shape) {
			SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler(
			  );
			wxString oldLabel(myHandler->label);
			myHandler->label = shapeLabel;
			shapeLabel = oldLabel;

			if (myHandler->pc)
				myHandler->FreeInstruction();

			RedrawShapeLabel(shape);
		}

		break; }
	case SDIFRAME_SET_RESET: {
		DatumPtr	fromPc = SHAPE_PC(fromShape);

		wxString	oldLabel;
		oldLabel = (wxChar *) fromPc->u.ref.string;
		fromPc->u.ref.string = (wxChar *) shapeLabel.c_str();
		shapeLabel = oldLabel;

		DatumPtr	oldPc = fromPc;
		fromPc->u.ref.pc = (DatumPtr) savedInfo;
		savedInfo = oldPc;

		((SDIEvtHandler *) fromShape->GetEventHandler())->ResetLabel();
		RedrawShapeLabel(fromShape);

		break; }
	}
	return TRUE;

}

/******************************************************************************/
/****************************** RemoveLines ***********************************/
/******************************************************************************/

// Remove each individual line connected to a shape by sending a command.
void
SDICommand::RemoveLines(wxShape *shape)
{
	wxNode *node = shape->GetLines().GetFirst();
	while (node) {
		wxLineShape *line = (wxLineShape *)node->GetData();
		doc->GetCommandProcessor()->Submit(new SDICommand(wxT("Cut"),
		  SDIFRAME_CUT, doc, NULL, -1, 0.0, 0.0, line->Selected(), line));

		node = shape->GetLines().GetFirst();
	}

}

#endif /* USE_WX_OGL */
