/**********************
 *
 * File:		GrSDIDiagram.cpp
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

#include <wx/tokenzr.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"
#include "GrSDIEvtHandler.h"
#include "GrSDIDiagram.h"
#include "GrSDICanvas.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtAppInterface.h"
#include "UtSimScript.h"

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

SDIDiagram::SDIDiagram(void)
{
	ok = false;
	loadIDsFromFile = false;
	x = DIAGRAM_DEFAULT_INITIAL_X;
	y = DIAGRAM_DEFAULT_INITIAL_Y;
	direction = 1;
	xScale = 1.0;
	yScale = 1.0;
	simProcess = NULL;

}

/******************************************************************************/
/****************************** AddShape **************************************/
/******************************************************************************/

/*
 * My Add shape method.
 */

void
SDIDiagram::AddShape(wxShape *shape)
{
	wxDiagram::AddShape(shape);
	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();
	if (myHandler->GetProcessType() == CONTROL_MODULE_CLASS) {
		wxNode *node = shape->GetChildren().GetFirst();
		if (node)
			AddShape((wxShape *)node->GetData());
	}

}

/******************************************************************************/
/****************************** CreateLoadShape *******************************/
/******************************************************************************/

/*
 * When 'pc' is NULL, a line shape is being set up.
 */

wxShape *
SDIDiagram::CreateLoadShape(DatumPtr pc, wxClassInfo *shapeInfo,
  const wxBrush *brush)
{
	bool	lineShape = (!pc);
	wxShape *shape = CreateBasicShape(shapeInfo, (lineShape)? -1:
	  pc->classSpecifier, brush);

	AddShape(shape);
	if (!lineShape) {
		SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();
		myHandler->pc = pc;
		shape->SetSize(DIAGRAM_DEFAULT_SHAPE_WIDTH,
		  DIAGRAM_DEFAULT_SHAPE_HEIGHT);
		pc->shapePtr = shape;
		if (pc->type == PROCESS)
			pc->data->shapePtr = shape;
		((SDIShape *) shape)->ResetLabel();
	}
	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
	shape->Move(dc, x, y);
	shape->Show(TRUE);
	return(shape);

}

/******************************************************************************/
/****************************** UpdateAutoShapePos ****************************/
/******************************************************************************/

/*
 * This routine sets the automatic position for the next shape.
 */

void
SDIDiagram::UpdateAutoShapePos(wxShape *shape)
{
	double	boxWidth, boxHeight, totalWidth;

	shape->GetBoundingBoxMax(&boxWidth, &boxHeight);
	totalWidth = boxWidth + DIAGRAM_DEFAULT_X_SEPARATION;
	x += direction * totalWidth;
	if (((direction > 0) && (x + totalWidth) > shape->GetCanvas()->
	  GetClientSize().GetWidth()) || ((direction < 0) && (x - totalWidth) <
	  0)) {
		direction = - direction;
		UpdateAutoYPos();
	}

}

/******************************************************************************/
/****************************** DrawSimShapes *********************************/
/******************************************************************************/

void
SDIDiagram::DrawSimShapes()
{
	DatumPtr	pc;
	ModulePtr	module;
	wxShape		*shape;
	SDICanvas	*canvas = (SDICanvas *) GetCanvas();

	if ((pc = GetSimulation_ModuleMgr(simProcess)) == NULL)
		return;
	while (pc) {
		shape = NULL;
		switch (pc->type) {
		case PROCESS: {
			module = pc->data->module;
			shape = CreateLoadShape(pc, canvas->GetClassInfo(module->
			  classSpecifier), (module->onFlag)? DIAGRAM_ENABLED_BRUSH:
			    DIAGRAM_DISENABLED_BRUSH);
			UpdateAutoShapePos(shape);
			break; }
		case REPEAT:
		case RESET:
			shape = CreateLoadShape(pc, canvas->GetClassInfo(
			  CONTROL_MODULE_CLASS), DIAGRAM_ENABLED_BRUSH);
			UpdateAutoShapePos(shape);
			break;
		case STOP: {
			DatumPtr	ppc = pc;
			while (ppc && (ppc->type != PROCESS))
				ppc = ppc->previous;
			shape = (wxShape *) ppc->shapePtr;
			pc->shapePtr = shape;
			break; }
		default:
			wxLogError(wxT("SDIDiagram::DrawSimShapes: datum type %d not ")
			  wxT("implemented.\n"), pc->type);
		} /* switch */
		pc = pc->next;
	}
}

/******************************************************************************/
/****************************** AddLineShape **********************************/
/******************************************************************************/

wxShape *
SDIDiagram::AddLineShape(wxShape *fromShape, wxShape *toShape, int lineType)
{
	wxShape *shape = CreateBasicShape(CLASSINFO(SDILineShape), lineType,
	  wxRED_BRUSH);
	SDILineShape *lineShape = (SDILineShape *) shape;
	switch (lineType) {
	case REPEAT:
		lineShape->MakeLineControlPoints(4);
		lineShape->AddArrow(ARROW_ARROW, ARROW_POSITION_MIDDLE,
		  DIAGRAM_ARROW_SIZE, 0.0, DIAGRAM_ARROW_TEXT);
		break;
	default:
		lineShape->MakeLineControlPoints(2);
		lineShape->AddArrow(ARROW_ARROW, ARROW_POSITION_END,
		  DIAGRAM_ARROW_SIZE, 0.0, DIAGRAM_ARROW_TEXT);
	} /* switch */
	AddShape(shape);
	fromShape->AddLine(lineShape, toShape);
	shape->Show(TRUE);

	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
	// It won't get drawn properly unless you move both
	// connected images
	fromShape->Move(dc, fromShape->GetX(), fromShape->GetY());
	toShape->Move(dc, toShape->GetX(), toShape->GetY());
	return(shape);

}

/******************************************************************************/
/****************************** DrawDefaultConnection *************************/
/******************************************************************************/

/*
 * This routine draws all a line to the next datum which is not type "STOP".
 */

void
SDIDiagram::DrawDefaultConnection(DatumPtr pc, wxShape *shape)
{
	DatumPtr 	toPc;

	for (toPc = pc->next; toPc && (toPc->type == STOP); toPc = toPc->
	  next)
		;
	if (toPc)
		AddLineShape(shape, (wxShape *) toPc->shapePtr, -1);

}

/******************************************************************************/
/****************************** DrawSimConnections ****************************/
/******************************************************************************/

/*
 * This routine draws all standard connections by going through the data
 * output connection lists.
 * A "for" loop must be used here and not "while" because "continue" is used
 * and we need pc = pc->next to always be executed.
 * This routine needs the process 'shapePtr' field to be set.
 */

void
SDIDiagram::DrawSimConnections(void)
{
	DatumPtr	pc, toPc;
	DynaBListPtr labelBList;

	if ((pc = GetSimulation_ModuleMgr(simProcess)) == NULL)
		return;
	labelBList = ((SimScriptPtr) simProcess->module->parsPtr)->labelBList;
	for (; pc != NULL; pc = pc->next) {
		if (pc->type == STOP)
			continue;
		wxShape *fromShape = (wxShape *) (pc->shapePtr);
		switch (pc->type) {
		case PROCESS: 
			if (pc->u.proc.outputList) {
				for (DynaListPtr p = pc->u.proc.outputList; p != NULL; p = p->
				  next) {
					toPc = (DatumPtr) FindElement_Utility_DynaBList(labelBList,
					  CmpProcessLabel_Utility_Datum, (char *) p->data)->data;
					AddLineShape(fromShape, (wxShape *) toPc->shapePtr, -1);
				}
			} else {
				for (toPc = pc->next; toPc && (toPc->type == STOP); toPc =
				  toPc->next)
					;
				if (!toPc)
					continue;
				switch (toPc->type) {
				case RESET:
				case REPEAT:
					AddLineShape(fromShape, (wxShape *) toPc->shapePtr, -1);
					break;
				default:
					EarObjRefPtr	p;
					for (p = pc->data->customerList; p != NULL; p = p->next)
						AddLineShape(fromShape, (wxShape *) p->earObject->
						  shapePtr, -1);
				} /* switch */
			}
			break;
		case REPEAT: {
			int	level = 0;
			for (toPc = pc->next; (toPc->type != STOP) || level; toPc = toPc->
			  next)
				switch (toPc->type) {
				case REPEAT:
					level++;
					break;
				case STOP:
					level--;
					break;
				default:
					;
				} /* switch */
			AddLineShape(fromShape, (wxShape *) toPc->shapePtr, REPEAT);
			DrawDefaultConnection(pc, fromShape);
			break; }
		case RESET:
			DrawDefaultConnection(pc, fromShape);
			break;
		default:
			;
		} /* switch */
	}

}

/******************************************************************************/
/****************************** DrawSimulation ********************************/
/******************************************************************************/

void
SDIDiagram::DrawSimulation(void)
{
	if (ok)
		return;
	DrawSimShapes();
	DrawSimConnections();
	ok = true;

}

/******************************************************************************/
/****************************** CreateBasicShape ******************************/
/******************************************************************************/

wxShape *
SDIDiagram::CreateBasicShape(wxClassInfo *shapeInfo, int type,
							 const wxBrush *brush)
{
	SDIShape *theShape = (SDIShape *) shapeInfo->CreateObject();
	if (!loadIDsFromFile)
		theShape->AssignNewIds();
	theShape->SetEventHandler(new SDIEvtHandler(theShape, theShape, wxT(""),
	  type));
	theShape->SetCentreResize(false);
	theShape->SetPen(wxBLACK_PEN);
	theShape->SetBrush((wxBrush *) brush);
	theShape->GetFont()->SetPointSize((int) (SHAPE_DEFAULT_FONT_POINTSIZE *
	  xScale));
	return(theShape);

}

/******************************************************************************/
/****************************** VerifyDiagram *********************************/
/******************************************************************************/

/*
 * This function checks that the loaded diagram corresponds with the simulation.
 * It returns 'false' if it finds any discrepancies.
 * Nothing should be selected at this point, so there will be no need to 
 * run the 'UnselectAllShapes' routine, so that 'control' shapes are not in
 * the shape list.
 */

bool
SDIDiagram::VerifyDiagram(void)
{
	static const wxChar *funcName = wxT("SDIDiagram::VerifyDiagram");
	bool	connectionOk;
	int		i, numDiagConnections = 0, numSimConnections = 0;
	DatumPtr	pc, toPc;
	EarObjectPtr	fromProcess, toProcess;
	wxNode *node = GetShapeList()->GetFirst();

	// Check processes exist for each shape line, and that the connection
	// exists in the simulation
	while (node) {
		wxShape *shape = (wxShape *) node->GetData();
		if (shape->IsKindOf(CLASSINFO(wxLineShape))) {
			wxLineShape *lineShape = (wxLineShape *) shape;
			wxShape *fromShape = lineShape->GetFrom();
			wxShape *toShape = lineShape->GetTo();
			if (!fromShape || !toShape) {
				NotifyError(wxT("%s: Diagram line is not connected to a valid ")
				  wxT("process."), funcName);
				return (false);
			}
			if ((SHAPE_PC(fromShape)->type == PROCESS) && (SHAPE_PC(toShape)->
			  type == PROCESS)) {
				fromProcess = SHAPE_PC(fromShape)->data;
				toProcess = SHAPE_PC(toShape)->data;
				for (i = 0, connectionOk = false; !connectionOk && (i <
				  toProcess->numInSignals); i++)
					 connectionOk = (fromProcess->outSignalPtr == toProcess->
					   inSignal[i]);
				if (!connectionOk) {
					NotifyError(wxT("%s: Diagram line does not correspond to ")
					  wxT("a simulation connection.\n"), funcName);
					return(false);
				}
			}
			numDiagConnections++;
		}
		node = node->GetNext();
	}
	// Check if there are any undrawn connections or shapes
	if ((pc = GetSimulation_ModuleMgr(simProcess)) == NULL)
		return(false);
	while (pc) {
		if (pc->type == PROCESS) {
			if (!pc->shapePtr && pc->data) {
				NotifyError(wxT("%s: Process has no description (step %d, ")
				  wxT("label %s'."), funcName, pc->stepNumber, pc->label);
				return (false);
			}
				numSimConnections += pc->data->numInSignals;
		} else {
			for (toPc = pc->next; toPc && (toPc->type == STOP); toPc =
			  toPc->next)
				;
			if (toPc)
				numSimConnections++;
		}
		pc = pc->next;
	}
	if (numDiagConnections != numSimConnections) {
		NotifyError(wxT("%s: The number of diagram lines (%d) does not ")
		  wxT("correspond\nto the number of simulation connections (%d)."),
		  funcName, numDiagConnections, numSimConnections);
		return(false);
	}
	return(TRUE);

}

/******************************************************************************/
/****************************** RedrawShapeLabel ******************************/
/******************************************************************************/

// Temporary

void
SDIDiagram::RedrawShapeLabel(wxShape *shape)
{
	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
    SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();

	shape->FormatText(dc, myHandler->label);
	shape->Draw(dc);

}

/******************************************************************************/
/****************************** UnselectAllShapes *****************************/
/******************************************************************************/

/*
 * This routine unselects all shapes.
 * It returns 'true' if the canvas needs to be redrawn.
 */

bool
SDIDiagram::UnselectAllShapes(void)
{
	bool	redraw = false;
	wxShape	*shape;
	wxNode *node = GetShapeList()->GetFirst();
	wxClientDC	dc(GetCanvas());
	GetCanvas()->PrepareDC(dc);

	while (node) {
		shape = (wxShape *) node->GetData();
		if (shape->Selected()) {
			shape->Select(false, &dc);
			redraw = true;
		}
		node = node->GetNext();
	}
	return(redraw);

}

/******************************************************************************/
/****************************** Rescale ***************************************/
/******************************************************************************/

/*
 * This routine redraws the diagram using a new scale;
 * If there is a selection, then there are additional 'control' shapes added
 * to the diagram shape list - first unselect everything to simplify code.
 */

void
SDIDiagram::Rescale(double theXScale, double theYScale)
{
	double	rescaleX, rescaleY;
	wxShape *shape;
	wxNode *node = GetShapeList()->GetFirst();

	UnselectAllShapes();
	rescaleX = theXScale / xScale;
	rescaleY = theYScale / yScale;
	xScale = theXScale;
	yScale = theYScale;
	while (node) {
		shape = (wxShape *) node->GetData();
		shape->SetSize(DIAGRAM_DEFAULT_SHAPE_WIDTH * xScale,
			  DIAGRAM_DEFAULT_SHAPE_HEIGHT * yScale);
		shape->SetX(shape->GetX() * rescaleX);
		shape->SetY(shape->GetY() * rescaleY);
		shape->GetFont()->SetPointSize((int) (SHAPE_DEFAULT_FONT_POINTSIZE *
		  xScale));
		if (!shape->IsKindOf(CLASSINFO(wxLineShape)))
			RedrawShapeLabel(shape);
		wxClientDC dc(shape->GetCanvas());
		shape->GetCanvas()->PrepareDC(dc);
		shape->Move(dc, shape->GetX(), shape->GetY());
		node = node->GetNext();
	}

}

#endif /* USE_WX_OGL */
