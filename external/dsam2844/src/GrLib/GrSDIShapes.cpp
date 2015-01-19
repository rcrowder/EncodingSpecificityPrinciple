/**********************
 *
 * File:		GrSDIShapes.cpp
 * Purpose: 	Shape classes for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		14 Nov 2002
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

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"
#include "UtSSSymbols.h"
#include "ExtXMLDocument.h"

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
/****************************** SDIAnalysisShape Methods **********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIAnalysisShape, SDIEllipseShape)

/****************************** Constructor ***********************************/

SDIAnalysisShape::SDIAnalysisShape(double width, double height):
  SDIEllipseShape(width, height)
{
}

/****************************** AddXMLInfo ************************************/

void
SDIAnalysisShape::AddXMLInfo(DSAMXMLNode *parent)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_ANALYSIS_SHAPE_ELEMENT);
	SDIEllipseShape::AddXMLInfo(myElement);
	parent->AddChild(myElement);

}

/******************************************************************************/
/****************************** SDIDisplayShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIDisplayShape, SDIRectangleShape)

/****************************** Constructor ***********************************/

SDIDisplayShape::SDIDisplayShape(double width, double height):
  SDIRectangleShape(width, height)
{
	// 0.3 of the smaller rectangle dimension
	SetCornerRadius((double) -0.3);
}

/****************************** AddXMLInfo ************************************/

void
SDIDisplayShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_DISPLAY_SHAPE_ELEMENT);
	SDIRectangleShape::AddXMLInfo(myElement);
	node->AddChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIDisplayShape::GetXMLInfo(wxXmlNode *myElement)
{
	SDIRectangleShape::GetXMLInfo(myElement);
	return(true);

}
/******************************************************************************/
/****************************** SDIControlShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIControlShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIControlShape::SDIControlShape(double width, double height): SDIPolygonShape(
  width, height)
{
	wxList *thePoints = new wxList;
	wxRealPoint *point = new wxRealPoint(0.0, (-h/2.0));
	thePoints->Append((wxObject*) point);

	point = new wxRealPoint((w/2.0), 0.0);
	thePoints->Append((wxObject*) point);

	point = new wxRealPoint(0.0, (h/2.0));
	thePoints->Append((wxObject*) point);

	point = new wxRealPoint((-w/2.0), 0.0);
	thePoints->Append((wxObject*) point);

	Create(thePoints);

}

/****************************** AddXMLInfo ************************************/

void
SDIControlShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_CONTROL_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node->AddChild(myElement);

}

/****************************** AddXMLInfo ************************************/

bool
SDIControlShape::GetXMLInfo(wxXmlNode *myElement)
{
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIFilterShape Methods ************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIFilterShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIFilterShape::SDIFilterShape(double width, double height): SDIPolygonShape(
  width, height)
{

	double	pipe = w / 6.0;
	double	neck = h / 8.0;
	double	hW = w / 10.0;
	double	hH = h / 4.0;

	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + pipe), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint(0.0, (h / 2.0 - neck)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - pipe), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), hH));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hW), hH));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hW), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - pipe), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint(0.0, (-h / 2.0 + neck)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + pipe), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + hW), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + hW), +hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), +hH));
	Create(thePoints);

}

/****************************** AddXMLInfo ************************************/

void
SDIFilterShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_FILTER_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node->AddChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIFilterShape::GetXMLInfo(wxXmlNode *myElement)
{
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIIOShape Methods ****************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIIOShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIIOShape::SDIIOShape(double width, double height): SDIPolygonShape(width,
  height)
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	if (w == 0.0)
		w = 60.0;
	if (h == 0.0)
		h = 60.0;

	double	offset = h / 8.0;
	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 - offset), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - offset), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 + offset), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + offset), (-h /
	  2.0)));
	Create(thePoints);

}

/****************************** AddXMLInfo ************************************/

void
SDIIOShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_IO_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node->AddChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIIOShape::GetXMLInfo(wxXmlNode *myElement)
{
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDILineShape Methods **************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDILineShape, wxLineShape)

/****************************** AddXMLControlPointsInfo ***********************/

void
SDILineShape::AddXMLControlPointsInfo(DSAMXMLNode *node, wxNode *cPNode)
{
	if (!cPNode)
		return;
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_CONTROL_POINTS_ELEMENT);
	while (cPNode) {
		wxRealPoint *point = (wxRealPoint *) cPNode->GetData();
		AddPointInfo(myElement, point);
		cPNode = cPNode->GetNext();
	}
	node->AddChild(myElement);

}

/****************************** AddXMLArrowListInfo ***************************/

void
SDILineShape::AddXMLArrowListInfo(DSAMXMLNode *node, wxNode *aNode)
{
	if (!aNode)
		return;
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_ARROW_LIST_ELEMENT);
	while (aNode) {
		wxArrowHead *head = (wxArrowHead *) aNode->GetData();
		DSAMXMLNode *arrowElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  SHAPE_XML_ARROW_ELEMENT);
		arrowElement->AddProperty(DSAM_XML_ID_ATTRIBUTE, head->GetId());
		arrowElement->AddProperty(SHAPE_XML_NAME_ATTRIBUTE, head->
		  GetName());
		arrowElement->AddProperty(SHAPE_XML_TYPE_ATTRIBUTE, head->_GetType());
		arrowElement->AddProperty(SHAPE_XML_END_ATTRIBUTE, head->GetArrowEnd());
		arrowElement->AddProperty(SHAPE_XML_X_OFFSET_ATTRIBUTE, head->
		  GetXOffset());
		arrowElement->AddProperty(SHAPE_XML_Y_OFFSET_ATTRIBUTE, head->
		  GetYOffset());
		arrowElement->AddProperty(SHAPE_XML_SIZE_ATTRIBUTE, head->
		  GetArrowSize());
		arrowElement->AddProperty(SHAPE_XML_SPACING_ATTRIBUTE, head->
		  GetSpacing());
		myElement->AddChild(arrowElement);
		aNode = aNode->GetNext();
	}
	node->AddChild(myElement);

}

/****************************** AddXMLInfo ************************************/

void
SDILineShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_LINE_SHAPE_ELEMENT);
	if (m_from)
		myElement->AddProperty(SHAPE_XML_FROM_ATTRIBUTE, m_from->GetId());
	if (m_to)
		myElement->AddProperty(SHAPE_XML_TO_ATTRIBUTE, m_to->GetId());
	if (m_attachmentFrom)
		myElement->AddProperty(SHAPE_XML_ATTACHMENT_FROM_ELEMENT,
		  m_attachmentFrom);
	if (m_attachmentTo)
		myElement->AddProperty(SHAPE_XML_ATTACHMENT_TO_ELEMENT, m_attachmentTo);
	if (m_alignmentStart)
		myElement->AddProperty(SHAPE_XML_ALIGNMENT_START_ELEMENT,
		  m_alignmentStart);
	if (m_alignmentEnd)
		myElement->AddProperty(SHAPE_XML_ALIGNMENT_END_ELEMENT, m_alignmentEnd);
	myElement->AddProperty(SHAPE_XML_IS_SPLINE_ATTRIBUTE, IsSpline());
	if (GetMaintainStraightLines())
		myElement->AddProperty(SHAPE_XML_KEEP_LINES_STRAIGHT_ELEMENT,
		  GetMaintainStraightLines());
	AddXMLControlPointsInfo(myElement, GetLineControlPoints()->GetFirst());
	AddXMLArrowListInfo(myElement, GetArrows().GetFirst());
	node->AddChild(myElement);

}

/****************************** GetXMLControlPointsInfo ***********************/

bool
SDILineShape::GetXMLControlPointsInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT(
	  "SDILineShape::GetXMLControlPointsInfo");
	bool	ok = true;
	wxRealPoint	*point;
	wxXmlNode	*child;

	if (m_lineControlPoints)
		ClearPointList(*m_lineControlPoints);
	else
		m_lineControlPoints = new wxList;
	for (child = myElement->GetChildren(); child; child = child->GetNext()) {
		if ((child->GetName() == SHAPE_XML_POINT_ELEMENT) &&
		  ((point = GetPointInfo(child)) == NULL)) {
			NotifyError(wxT("%s: Could not set point."), funcName);
			ok = false;
			break;
		}
		m_lineControlPoints->Append((wxObject*) point);
	}

	return(ok);

}

/****************************** GetXMLArrowListInfo ***************************/

bool
SDILineShape::GetXMLArrowListInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDILineShape::GetXMLArrowListInfo");
	bool	ok = true;
	int		arrowEnd = 0, id = -1, type = ARROW_ARROW;
	double	xOffset = 0.0, yOffset = 0.0, size = 0.0, spacing = 5.0;
    wxString arrowName;
	wxXmlNode	*child;
	MyXmlProperty	*prop;

	for (child = myElement->GetChildren(); child; child = child->GetNext())
		if (child->GetName() == SHAPE_XML_ARROW_ELEMENT) {
			for (prop = (MyXmlProperty *) child->GetProperties(); prop; prop =
			  prop->GetNext())
				if (prop->GetName() == DSAM_XML_ID_ATTRIBUTE)
					prop->GetPropVal(&id);
				else if (prop->GetName() == SHAPE_XML_NAME_ATTRIBUTE)
					arrowName = prop->GetValue();
				else if (prop->GetName() == SHAPE_XML_TYPE_ATTRIBUTE)
					prop->GetPropVal(&type);
				else if (prop->GetName() == SHAPE_XML_END_ATTRIBUTE)
					prop->GetPropVal(&arrowEnd);
				else if (prop->GetName() == SHAPE_XML_X_OFFSET_ATTRIBUTE)
					prop->GetPropVal(&xOffset);
				else if (prop->GetName() == SHAPE_XML_Y_OFFSET_ATTRIBUTE)
					prop->GetPropVal(&yOffset);
				else if (prop->GetName() == SHAPE_XML_SIZE_ATTRIBUTE)
					prop->GetPropVal(&size);
				else if (prop->GetName() == SHAPE_XML_SPACING_ATTRIBUTE)
					prop->GetPropVal(&spacing);
			if (!ok)
				break;
			wxRegisterId(id);
			wxArrowHead *arrowHead = AddArrow(type, arrowEnd, size, xOffset,
			  arrowName, NULL, id);
			arrowHead->SetYOffset(yOffset);
			arrowHead->SetSpacing(spacing);
		}
	return(ok);

}

/****************************** GetXMLInfo ************************************/

bool
SDILineShape::GetXMLInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDILineShape::GetXMLInfo");
	bool	ok = true;
	int		iVal = 0;
	wxXmlNode	*child;
	MyXmlProperty	*prop;

	for (prop = (MyXmlProperty *) myElement->GetProperties(); prop; prop =
	  prop->GetNext())
		if (prop->GetName() == SHAPE_XML_IS_SPLINE_ATTRIBUTE) {
			if (prop->GetPropVal(&iVal)) {
				SetSpline(iVal != 0);
				SetMaintainStraightLines(iVal != 0);
			} else
				ok = false;
		} else if (prop->GetName() == SHAPE_XML_ATTACHMENT_FROM_ELEMENT)
			prop->GetPropVal(&m_attachmentFrom);
		else if (prop->GetName() == SHAPE_XML_ATTACHMENT_TO_ELEMENT)
			prop->GetPropVal(&m_attachmentTo);
		else if (prop->GetName() == SHAPE_XML_ALIGNMENT_START_ELEMENT)
			prop->GetPropVal(&m_alignmentStart);
		else if (prop->GetName() == SHAPE_XML_ALIGNMENT_END_ELEMENT)
			prop->GetPropVal(&m_alignmentEnd);
	// Compatibility: check for no regions.
	if (m_regions.GetCount() == 0) {
		wxShapeRegion *newRegion = new wxShapeRegion;
		newRegion->SetName(wxT("Middle"));
		newRegion->SetSize(150, 50);
		m_regions.Append((wxObject *)newRegion);
		if (m_text.GetCount() > 0) {
			newRegion->ClearText();
			wxNode *node = m_text.GetFirst();
			while (node) {
				wxShapeTextLine *textLine = (wxShapeTextLine *)node->GetData();
				wxNode *next = node->GetNext();
				newRegion->GetFormattedText().Append((wxObject *)textLine);
				delete node;
				node = next;
			}
		}

		newRegion = new wxShapeRegion;
		newRegion->SetName(wxT("Start"));
		newRegion->SetSize(150, 50);
		m_regions.Append((wxObject *)newRegion);

		newRegion = new wxShapeRegion;
		newRegion->SetName(wxT("End"));
		newRegion->SetSize(150, 50);
		m_regions.Append((wxObject *)newRegion);
	}
	for (child = myElement->GetChildren(); ok && child; child = child->GetNext())
		if ((child->GetName() == SHAPE_XML_CONTROL_POINTS_ELEMENT) &&
		  !GetXMLControlPointsInfo(child))
			ok = false;
		else if ((child->GetName() == SHAPE_XML_ARROW_LIST_ELEMENT) &&
		  !GetXMLArrowListInfo(child))
			ok = false;
	if (!ok)
		NotifyError(wxT("%s: Failed to load XML information."), funcName);
	return(ok);

}

/******************************************************************************/
/****************************** SDIModelShape Methods *************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIModelShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIModelShape::SDIModelShape(double width, double height): SDIPolygonShape(
  width, height)
{
	double	edge = h / 4.0;
	double	hW = w / 16.0;
	double	hH = h / 4.0;

	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - edge), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0 -
	  edge)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (-h / 2.0 +
	  edge)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - edge), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + hW), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + hW), hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), hH));
	Create(thePoints);

}

/****************************** AddXMLInfo ************************************/

void
SDIModelShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_MODEL_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node->AddChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIModelShape::GetXMLInfo(wxXmlNode *myElement)
{
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIUserShape Methods **************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIUserShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIUserShape::SDIUserShape(double width, double height): SDIPolygonShape(width,
  height)
{

	double	flange = h / 8.0;
	double	base = w / 12.0;
	double	slope = w / 14.0;
	double	hd = w / 4.0;

	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base + slope),
	  (h / 2.0 - flange)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hd),  (h / 2.0 -
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hd),  (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), 0.0));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hd), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hd), (-h / 2.0 +
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base + slope),
	  (-h / 2.0 + flange)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (-h / 2.0)));
	Create(thePoints);

}

/****************************** AddXMLInfo ************************************/

void
SDIUserShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_USER_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node->AddChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIUserShape::GetXMLInfo(wxXmlNode *myElement)
{
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDITransformShape Methods *********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDITransformShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDITransformShape::SDITransformShape(double width, double height):
  SDIPolygonShape(width, height)
{
	double	inset = h / 7.0;
	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - inset), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + inset), (-h /
	  2.0)));
	Create(thePoints);

}

/****************************** AddXMLInfo ************************************/

void
SDITransformShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_TRANSFORM_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node->AddChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDITransformShape::GetXMLInfo(wxXmlNode *myElement)
{
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIUtilityShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIUtilityShape, SDIRectangleShape)

/****************************** Constructor ***********************************/

SDIUtilityShape::SDIUtilityShape(double width, double height):
  SDIRectangleShape(width, height)
{
}

/****************************** AddXMLInfo ************************************/

void
SDIUtilityShape::AddXMLInfo(DSAMXMLNode *node)
{
	DSAMXMLNode *myElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_UTILITY_SHAPE_ELEMENT);
	SDIRectangleShape::AddXMLInfo(myElement);
	node->AddChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIUtilityShape::GetXMLInfo(wxXmlNode *myElement)
{
	SDIRectangleShape::GetXMLInfo(myElement);
	return(true);

}

#endif /* USE_WX_OGL */
