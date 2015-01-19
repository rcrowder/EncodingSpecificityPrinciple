/**********************
 *
 * File:		GrSDIBaseShapes.cpp
 * Purpose: 	Base Shape classes for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 *				This code was moved from the GrSDIShapes code module.
 * Author:		L.P.O'Mard
 * Created:		10 Apr 2005
 * Updated:
 * Copyright:	(c) 2005, 2010 Lowel P. O'Mard
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
#include "GeModuleMgr.h"
#include "GrSDIEvtHandler.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"
#include "UtSSSymbols.h"
#include "ExtXMLNode.h"
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
/****************************** Basic Shape Methods ***************************/
/******************************************************************************/

/******************************************************************************/
/****************************** SDIShape Methods ******************************/
/******************************************************************************/

IMPLEMENT_ABSTRACT_CLASS(SDIShape, wxShape)

/****************************** Constructor ***********************************/

SDIShape::SDIShape(wxShapeCanvas *can): wxShape(can)
{
}

/******************************************************************************/
/****************************** AddPenInfo ************************************/
/******************************************************************************/

void
SDIShape::AddPenInfo(DSAMXMLNode *parent)
{
	wxPen	*myPen;

	if ((myPen = GetPen()) == NULL)
		return;
	DSAMXMLNode *penElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_PEN_ELEMENT);
	if (myPen->GetWidth() != 1)
		penElement->AddProperty(SHAPE_XML_WIDTH_ATTRIBUTE,
		  myPen->GetWidth());
	if ( myPen->GetStyle() != wxSOLID)
		penElement->AddProperty(SHAPE_XML_STYLE_ATTRIBUTE,
		  myPen->GetStyle());
	wxString penColour = wxTheColourDatabase->FindName(myPen->GetColour());
	if (penColour == wxEmptyString) {
		wxString hex(oglColourToHex(myPen->GetColour()));
		hex = wxString(wxT("#")) + hex;
		penElement->AddProperty(SHAPE_XML_COLOUR_ATTRIBUTE, hex);
	} else if (penColour != wxT("BLACK"))
		penElement->AddProperty(SHAPE_XML_COLOUR_ATTRIBUTE, penColour);
	if (penElement->GetProperties())
		parent->AddChild(penElement);
	else
		delete penElement;

}

/******************************************************************************/
/****************************** AddBrushInfo **********************************/
/******************************************************************************/

void
SDIShape::AddBrushInfo(DSAMXMLNode *parent)
{
	wxBrush	*myBrush;

	if ((myBrush = GetBrush()) == NULL)
		return;
	DSAMXMLNode *brushElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_BRUSH_ELEMENT);
	wxString brushColour = wxTheColourDatabase->FindName(myBrush->GetColour(
	  ));

	if (brushColour == wxEmptyString) {
		wxString hex(oglColourToHex(myBrush->GetColour()));
		hex = wxString(wxT("#")) + hex;
		brushElement->AddProperty(SHAPE_XML_COLOUR_ATTRIBUTE, hex);
	} else if (brushColour != wxT("WHITE"))
		brushElement->AddProperty(SHAPE_XML_COLOUR_ATTRIBUTE, brushColour);

	if (myBrush->GetStyle() != wxSOLID)
		brushElement->AddProperty(SHAPE_XML_TYPE_ATTRIBUTE, myBrush->GetStyle());
	if (brushElement->GetProperties())
		parent->AddChild(brushElement);
	else
		delete brushElement;

}

/******************************************************************************/
/****************************** AddLineInfo ***********************************/
/******************************************************************************/

void
SDIShape::AddLineInfo(DSAMXMLNode *parent)
{
	if (!GetLines().GetCount())
		return;
	DSAMXMLNode *arcsElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_ARCS_ELEMENT);
	wxNode *node = GetLines().GetFirst();
	while (node) {
		DSAMXMLNode *lineElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  SHAPE_XML_LINE_ELEMENT);
		wxShape *line = (wxShape *) node->GetData();
		lineElement->AddProperty(DSAM_XML_ID_ATTRIBUTE, line->GetId());
		arcsElement->AddChild(lineElement);
		node = node->GetNext();
	}
	if (arcsElement->GetChildren())
		parent->AddChild(arcsElement);
	else
		delete arcsElement;

}

/******************************************************************************/
/****************************** AddAttachmentsInfo ****************************/
/******************************************************************************/

void
SDIShape::AddAttachmentsInfo(DSAMXMLNode *parent)
{
	if (!GetAttachments().GetCount())
		return;
	DSAMXMLNode *attachmentsElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_ATTACHMENTS_ELEMENT);
	wxNode *node = GetAttachments().GetFirst();
	while (node) {
		wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
		DSAMXMLNode *pointElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  SHAPE_XML_POINT_ELEMENT);
		pointElement->AddProperty(DSAM_XML_ID_ATTRIBUTE, point->m_id);
		pointElement->AddProperty(SHAPE_XML_X_ATTRIBUTE, point->m_x);
		pointElement->AddProperty(SHAPE_XML_Y_ATTRIBUTE, point->m_y);
		attachmentsElement->AddChild(pointElement);
		node = node->GetNext();
	}
	if (attachmentsElement->GetProperties())
		parent->AddChild(attachmentsElement);
	else
		delete attachmentsElement;

}

/******************************************************************************/
/****************************** AddFontInfo ***********************************/
/******************************************************************************/

void
SDIShape::AddFontInfo(DSAMXMLNode *parent, wxFont *font)
{
	DSAMXMLNode *fontElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_FONT_ELEMENT);

	fontElement->AddProperty(SHAPE_XML_POINTSIZE_ATTRIBUTE, (font)?
	  font->GetPointSize() : SHAPE_DEFAULT_FONT_POINTSIZE);
	fontElement->AddProperty(SHAPE_XML_FAMILY_ATTRIBUTE, (font)?
	  font->GetFamily() : wxDEFAULT);
	fontElement->AddProperty(SHAPE_XML_STYLE_ATTRIBUTE, (font)?
	  font->GetStyle() : wxDEFAULT);
	fontElement->AddProperty(SHAPE_XML_WEIGHT_ATTRIBUTE, (font)?
	  font->GetWeight() : wxNORMAL);
	parent->AddChild(fontElement);

}

/******************************************************************************/
/****************************** AddRegions ************************************/
/******************************************************************************/

void
SDIShape::AddRegions(DSAMXMLNode *parent)
{
	wxNode *node = GetRegions().GetFirst();
	DSAMXMLNode *regionsElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_REGIONS_ELEMENT);

	while (node) {
		wxShapeRegion *region = (wxShapeRegion *) node->GetData();

		DSAMXMLNode *regionElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  SHAPE_XML_REGION_ELEMENT);

		regionElement->AddProperty(SHAPE_XML_X_ATTRIBUTE, region->m_x);
		regionElement->AddProperty(SHAPE_XML_Y_ATTRIBUTE, region->m_y);
		regionElement->AddProperty(SHAPE_XML_WIDTH_ATTRIBUTE, region->
		  GetWidth());
		regionElement->AddProperty(SHAPE_XML_HEIGHT_ATTRIBUTE, region->
		  GetHeight());

		regionElement->AddProperty(SHAPE_XML_MIN_WIDTH_ATTRIBUTE, region->
		  m_minWidth);
		regionElement->AddProperty(SHAPE_XML_MIN_HEIGHT_ATTRIBUTE,
		  region->m_minHeight);
		regionElement->AddProperty(SHAPE_XML_PROPORTION_X_ATTRIBUTE,
		  region->m_regionProportionX);
		regionElement->AddProperty(SHAPE_XML_PROPORTION_Y_ATTRIBUTE,
		  region->m_regionProportionY);

		regionElement->AddProperty(SHAPE_XML_FORMAT_MODE_ATTRIBUTE, region->
		  m_formatMode);

		AddFontInfo(regionElement, region->m_font);

		regionElement->AddProperty(SHAPE_XML_TEXT_COLOUR_ATTRIBUTE, region->
		  m_textColour);

		// New members for pen colour/style
		regionElement->AddProperty(SHAPE_XML_PEN_COLOUR_ATTRIBUTE, region->
		  m_penColour);
		regionElement->AddProperty(SHAPE_XML_PEN_STYLE_ATTRIBUTE, region->
		  m_penStyle);

		// Formatted text:
		// text1 = ((x y string) (x y string) ...)

		wxNode *textNode = region->m_formattedText.GetFirst();
		while (textNode) {
			DSAMXMLNode *textElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
			  SHAPE_XML_TEXT_ELEMENT);
			wxShapeTextLine *line = (wxShapeTextLine *)textNode->GetData();
			textElement->AddProperty(SHAPE_XML_X_ATTRIBUTE, line->GetX());
			textElement->AddProperty(SHAPE_XML_Y_ATTRIBUTE, line->GetY());
			wxXmlNode *textElementText = new wxXmlNode(textElement,
			  wxXML_TEXT_NODE, wxEmptyString, line->GetText());
			regionElement->AddChild(textElement);
			textNode = textNode->GetNext();
		}

		regionsElement->AddChild(regionElement);

		node = node->GetNext();
	}
	if (regionsElement->GetChildren())
		parent->AddChild(regionsElement);
	else
		delete regionsElement;

}

/******************************************************************************/
/****************************** AddShapeInfo **********************************/
/******************************************************************************/

void
SDIShape::AddShapeInfo(DSAMXMLNode *parent)
{
	wxString	str;

	DSAMXMLNode *shapeElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_SHAPE_ELEMENT);
	shapeElement->AddProperty(DSAM_XML_TYPE_ATTRIBUTE, GetClassInfo()->
	  GetClassName());
	shapeElement->AddProperty(DSAM_XML_ID_ATTRIBUTE, GetId());
	shapeElement->AddProperty(SHAPE_XML_X_ATTRIBUTE, GetX());
	shapeElement->AddProperty(SHAPE_XML_Y_ATTRIBUTE, GetY());
	AddPenInfo(shapeElement);
	AddBrushInfo(shapeElement);
	AddLineInfo(shapeElement);

	// Misc. attributes
	if (m_attachmentMode != 0)
		shapeElement->AddProperty(SHAPE_XML_USE_ATTACHMENTS_ATTRIBUTE,
		  m_attachmentMode);
	if (m_sensitivity != OP_ALL)
		shapeElement->AddProperty(SHAPE_XML_SENSITIVITY_ATTRIBUTE,
		  m_sensitivity);
	if (!m_spaceAttachments)
		shapeElement->AddProperty(SHAPE_XML_SPACE_ATTACHMENTS_ATTRIBUTE,
		  m_spaceAttachments);
	if (m_fixedWidth)
		shapeElement->AddProperty(SHAPE_XML_FIXED_WIDTH_ATTRIBUTE,
		  m_fixedWidth);
	if (m_fixedHeight)
		shapeElement->AddProperty(SHAPE_XML_FIXED_HEIGHT_ATTRIBUTE,
		  m_fixedHeight);
	if (m_shadowMode != SHADOW_NONE)
		shapeElement->AddProperty(SHAPE_XML_SHADOW_MODE_ATTRIBUTE,
		  m_shadowMode);
	if (!m_centreResize)
		shapeElement->AddProperty(SHAPE_XML_CENTRE_RESIZE_ATTRIBUTE, wxT("0"));
	shapeElement->AddProperty(SHAPE_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE,
	  m_maintainAspectRatio);
	if (m_highlighted)
		shapeElement->AddProperty(SHAPE_XML_HILITE_ATTRIBUTE,
		  m_highlighted);

	if (m_parent) // For composite objects
		shapeElement->AddProperty(SHAPE_XML_PARENT_ATTRIBUTE,
		  m_parent->GetId());
	if (m_rotation != 0.0)
		shapeElement->AddProperty(SHAPE_XML_ROTATION_ATTRIBUTE,
		  m_rotation);

	if (!IsKindOf(CLASSINFO(wxLineShape))) {
		shapeElement->AddProperty(SHAPE_XML_NECK_LENGTH_ATTRIBUTE,
		  GetBranchNeckLength());
		shapeElement->AddProperty(SHAPE_XML_STEM_LENGTH_ATTRIBUTE,
		  GetBranchStemLength());
		shapeElement->AddProperty(SHAPE_XML_BRANCH_SPACING_ATTRIBUTE,
		  GetBranchSpacing());
		shapeElement->AddProperty(SHAPE_XML_BRANCH_STYLE_ATTRIBUTE,
		  GetBranchStyle());
	}

	AddAttachmentsInfo(shapeElement);
	AddRegions(shapeElement);
	if (IsKindOf(CLASSINFO(SDIAnalysisShape)))
		((SDIAnalysisShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIControlShape)))
		((SDIControlShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIDisplayShape)))
		((SDIDisplayShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIFilterShape)))
		((SDIFilterShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIIOShape)))
		((SDIIOShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDILineShape)))
		((SDILineShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIModelShape)))
		((SDIModelShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIUserShape)))
		((SDIUserShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDITransformShape)))
		((SDITransformShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIUtilityShape)))
		((SDIUtilityShape *) this)->AddXMLInfo(shapeElement);

	parent->AddChild(shapeElement);
}

/****************************** GetPenInfo ************************************/

bool
SDIShape::GetPenInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIShape::GetPenInfo");
	bool	ok = true;
	int		penWidth = 1, penStyle = wxSOLID;
	wxString	penColour = wxEmptyString;
	MyXmlProperty	*prop;

	for (prop = (MyXmlProperty *) myElement->GetProperties(); prop; prop =
	  prop->GetNext())
		if (prop->GetName() == SHAPE_XML_WIDTH_ATTRIBUTE)
			prop->GetPropVal(&penWidth);
		else if (prop->GetName() == SHAPE_XML_STYLE_ATTRIBUTE)
			prop->GetPropVal(&penStyle);
		else if (prop->GetName() == SHAPE_XML_COLOUR_ATTRIBUTE)
			penColour = prop->GetValue();


	if (penColour == wxEmptyString)
		penColour = wxT("BLACK");
	if (penColour.GetChar(0) == '#') {
		wxColour col(oglHexToColour(penColour.After('#')));
		m_pen = wxThePenList->FindOrCreatePen(col, penWidth, penStyle);
	} else
		m_pen = wxThePenList->FindOrCreatePen(penColour, penWidth, penStyle);

	if (!m_pen)
		m_pen = wxBLACK_PEN;

	return(ok);

}

/****************************** GetBrushInfo **********************************/

bool
SDIShape::GetBrushInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIShape::GetBrushInfo");
	bool	ok = true;
	int		brushStyle = wxSOLID;
	wxString	brushColour = wxEmptyString;
	MyXmlProperty	*prop;

	for (prop = (MyXmlProperty *) myElement->GetProperties(); prop; prop =
	  prop->GetNext())
		if (prop->GetName() == SHAPE_XML_STYLE_ATTRIBUTE)
			prop->GetPropVal(&brushStyle);
		else if (prop->GetName() == SHAPE_XML_COLOUR_ATTRIBUTE)
			brushColour = prop->GetValue();

	if (brushColour == wxEmptyString)
		brushColour = wxT("WHITE");

	if (brushColour.GetChar(0) == '#') {
		wxColour col(oglHexToColour(brushColour.After('#')));
		m_brush = wxTheBrushList->FindOrCreateBrush(col, brushStyle);
	} else
		m_brush = wxTheBrushList->FindOrCreateBrush(brushColour, brushStyle);

	if (!m_brush)
		m_brush = wxWHITE_BRUSH;
	return(true);

}

/****************************** GetAttachmentsInfo ****************************/

bool
SDIShape::GetAttachmentsInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIShape::GetAttachmentsInfo");
	bool	ok = true;
	wxXmlNode	*child;
	MyXmlProperty	*prop;

	for (child = myElement->GetChildren(); child; child = child->GetNext())
		if (child->GetName() == SHAPE_XML_POINT_ELEMENT) {
			wxAttachmentPoint *point = new wxAttachmentPoint;
			for (prop = (MyXmlProperty *) child->GetProperties(); prop; prop =
			  prop->GetNext())
				if (prop->GetName() == DSAM_XML_ID_ATTRIBUTE)
					prop->GetPropVal(&point->m_id);
				else if (prop->GetName() == SHAPE_XML_X_ATTRIBUTE)
					prop->GetPropVal(&point->m_x);
				else if (prop->GetName() == SHAPE_XML_Y_ATTRIBUTE)
					prop->GetPropVal(&point->m_y);
			m_attachmentPoints.Append((wxObject *)point);
		}
	return(ok);

}

/****************************** GetFontInfo ***********************************/

wxFont *
SDIShape::GetFontInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIShape::GetFontInfo");
	bool	ok = true;
	int		fontSize = SHAPE_DEFAULT_FONT_POINTSIZE, fontFamily = wxSWISS;
	int		fontStyle = wxNORMAL, fontWeight = wxNORMAL;
	MyXmlProperty	*prop;

	for (prop = (MyXmlProperty *) myElement->GetProperties(); prop; prop =
	  prop->GetNext())
		if (prop->GetName() == SHAPE_XML_POINTSIZE_ATTRIBUTE)
			prop->GetPropVal(&fontSize);
		else if (prop->GetName() == SHAPE_XML_FAMILY_ATTRIBUTE)
			prop->GetPropVal(&fontFamily);
		else if (prop->GetName() == SHAPE_XML_STYLE_ATTRIBUTE)
			prop->GetPropVal(&fontStyle);
		else if (prop->GetName() == SHAPE_XML_WEIGHT_ATTRIBUTE)
			prop->GetPropVal(&fontWeight);

	wxFont *font = wxTheFontList->FindOrCreateFont(fontSize, fontFamily,
	  fontStyle, fontWeight);
	return(font);

}

/****************************** GetRegionInfo *********************************/

wxShapeRegion *
SDIShape::GetRegionInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIShape::GetRegionInfo");
	bool	ok = true;
	int		formatMode = FORMAT_NONE, penStyle = wxSOLID;
	double	x = 0.0, y = 0.0, width = 0.0, height = 0.0, minWidth = 5.0;
	double	minHeight = 5.0, regionProportionX = -1.0, regionProportionY = -1.0;
	wxString regionTextColour = wxEmptyString, penColour = wxEmptyString;
	wxXmlNode *child;
	MyXmlProperty	*prop;

	wxShapeRegion *region = new wxShapeRegion;
	for (prop = (MyXmlProperty *) myElement->GetProperties(); prop; prop =
	  prop->GetNext())
		if (prop->GetName() == SHAPE_XML_X_ATTRIBUTE)
			prop->GetPropVal(&x);
		else if (prop->GetName() == SHAPE_XML_Y_ATTRIBUTE)
			prop->GetPropVal(&y);
		else if (prop->GetName() == SHAPE_XML_WIDTH_ATTRIBUTE)
			prop->GetPropVal(&width);
		else if (prop->GetName() == SHAPE_XML_HEIGHT_ATTRIBUTE)
			prop->GetPropVal(&height);
		else if (prop->GetName() == SHAPE_XML_MIN_WIDTH_ATTRIBUTE)
			prop->GetPropVal(&minWidth);
		else if (prop->GetName() == SHAPE_XML_MIN_HEIGHT_ATTRIBUTE)
			prop->GetPropVal(&minHeight);
		else if (prop->GetName() == SHAPE_XML_PROPORTION_X_ATTRIBUTE)
			prop->GetPropVal(&regionProportionX);
		else if (prop->GetName() == SHAPE_XML_PROPORTION_Y_ATTRIBUTE)
			prop->GetPropVal(&regionProportionY);
		else if (prop->GetName() == SHAPE_XML_FORMAT_MODE_ATTRIBUTE)
			prop->GetPropVal(&formatMode);
		else if (prop->GetName() == SHAPE_XML_TEXT_COLOUR_ATTRIBUTE)
			regionTextColour = prop->GetValue();
		else if (prop->GetName() == SHAPE_XML_PEN_COLOUR_ATTRIBUTE)
			penColour = prop->GetValue();
		else if (prop->GetName() == SHAPE_XML_PEN_STYLE_ATTRIBUTE)
			prop->GetPropVal(&penStyle);

	region->SetPosition(x, y);
	region->SetSize(width, height);
	region->SetMinSize(minWidth, minHeight);
	region->SetProportions(regionProportionX, regionProportionY);
	region->SetFormatMode(formatMode);

	if (regionTextColour.empty())
		regionTextColour = wxT("BLACK");
	region->m_textColour = regionTextColour;

	if (!penColour.empty())
		region->SetPenColour(penColour);
	region->SetPenStyle(penStyle);

	for (child = myElement->GetChildren(); child; child = child->GetNext())
		if (child->GetName() == SHAPE_XML_FONT_ELEMENT)
			region->SetFont(GetFontInfo(child));
		else if (child->GetName() == SHAPE_XML_TEXT_ELEMENT) {
			for (prop = (MyXmlProperty *) child->GetProperties(); prop; prop =
			  prop->GetNext())
				if (prop->GetName() == SHAPE_XML_X_ATTRIBUTE)
					prop->GetPropVal(&x);
				else if (prop->GetName() == SHAPE_XML_Y_ATTRIBUTE)
					prop->GetPropVal(&y);
				wxShapeTextLine *line = new wxShapeTextLine(x, y,
				  child->GetNodeContent());
				region->m_formattedText.Append(line);
		}
	return(region);

}

/****************************** GetRegionsInfo ********************************/

bool
SDIShape::GetRegionsInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIShape::GetRegionsInfo");
	bool	ok = true;
	int		formatMode = FORMAT_NONE, penStyle = wxSOLID;
	double	x = 0.0, y = 0.0, width = 0.0, height = 0.0, minWidth = 5.0;
	double	minHeight = 5.0, regionProportionX = -1.0, regionProportionY = -1.0;
	wxString regionTextColour = wxEmptyString, penColour = wxEmptyString;
	wxXmlNode *child;

	ClearRegions();
	for (child = myElement->GetChildren(); child; child = child->GetNext())
		if (child->GetName() == SHAPE_XML_REGION_ELEMENT)
			m_regions.Append(GetRegionInfo(child));
	return(ok);

}

/****************************** GetShapeInfo **********************************/

bool
SDIShape::GetShapeInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIShape::GetShapeInfo");
	bool	ok = true;
	double	x = 0.0, y = 0.0;
	wxString	value;
	wxXmlNode	*child;
	MyXmlProperty	*prop;

	for (prop = (MyXmlProperty *) myElement->GetProperties(); ok && prop; prop =
	  prop->GetNext())
		if (prop->GetName() == DSAM_XML_ID_ATTRIBUTE) {
			if (prop->GetPropVal(&m_id))
				wxRegisterId(m_id);
			else
				ok = false;
		} else if (prop->GetName() == SHAPE_XML_X_ATTRIBUTE) {
			if (prop->GetPropVal(&x))
				SetX(x);
			else
				ok = false;
		} else if (prop->GetName() == SHAPE_XML_Y_ATTRIBUTE) {
			if (prop->GetPropVal(&y))
				SetY(y);
			else
				ok = false;
		} else if (prop->GetName() == SHAPE_XML_TEXT_COLOUR_ATTRIBUTE)
			SetTextColour(prop->GetValue());
		else if (prop->GetName() == SHAPE_XML_USE_ATTACHMENTS_ATTRIBUTE)
			prop->GetPropVal(&m_attachmentMode);
		else  if (prop->GetName() == SHAPE_XML_SENSITIVITY_ATTRIBUTE)
			prop->GetPropVal(&m_sensitivity);
		else if (prop->GetName() == SHAPE_XML_SPACE_ATTACHMENTS_ATTRIBUTE)
			prop->GetPropVal(&m_spaceAttachments);
		else if (prop->GetName() == SHAPE_XML_FIXED_WIDTH_ATTRIBUTE)
			prop->GetPropVal(&m_fixedWidth);
		else if (prop->GetName() == SHAPE_XML_FIXED_HEIGHT_ATTRIBUTE)
			prop->GetPropVal(&m_fixedHeight);
		else if (prop->GetName() == SHAPE_XML_FORMAT_MODE_ATTRIBUTE)
			prop->GetPropVal(&m_formatMode);
		else if (prop->GetName() == SHAPE_XML_SHADOW_MODE_ATTRIBUTE)
			prop->GetPropVal(&m_shadowMode);
		else if (prop->GetName() == SHAPE_XML_NECK_LENGTH_ATTRIBUTE)
			prop->GetPropVal(&m_branchNeckLength);
		else if (prop->GetName() == SHAPE_XML_STEM_LENGTH_ATTRIBUTE)
			prop->GetPropVal(&m_branchStemLength);
		else if (prop->GetName() == SHAPE_XML_BRANCH_SPACING_ATTRIBUTE)
			prop->GetPropVal(&m_branchSpacing);
		else if (prop->GetName() == SHAPE_XML_BRANCH_STYLE_ATTRIBUTE)
			prop->GetPropVal(&m_branchStyle);
		else if (prop->GetName() == SHAPE_XML_CENTRE_RESIZE_ATTRIBUTE)
			prop->GetPropVal(&m_centreResize);
		else if (prop->GetName() == SHAPE_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE)
			prop->GetPropVal(&m_maintainAspectRatio);
		else if (prop->GetName() == SHAPE_XML_HILITE_ATTRIBUTE)
			prop->GetPropVal(&m_highlighted);
		else if (prop->GetName() == SHAPE_XML_ROTATION_ATTRIBUTE)
			prop->GetPropVal(&m_rotation);

	//? SetFont(oglMatchFont((iVal)? iVal: 10));

	ClearText();

	wxString brushString = wxEmptyString;
	m_attachmentMode = ATTACHMENT_MODE_NONE;

	for (child = myElement->GetChildren(); ok && child; child = child->GetNext())
		if (child->GetName() == SHAPE_XML_PEN_ELEMENT)
			GetPenInfo(child);
		else if (child->GetName() == SHAPE_XML_BRUSH_ELEMENT)
			GetBrushInfo(child);
		else if (child->GetName() == SHAPE_XML_ATTACHMENTS_ELEMENT)
			GetAttachmentsInfo(child);
		else if (child->GetName() == SHAPE_XML_REGIONS_ELEMENT)
			GetRegionsInfo(child);
		else if ((child->GetName() == SHAPE_XML_ANALYSIS_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDIAnalysisShape)))
			((SDIAnalysisShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_CONTROL_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDIControlShape)))
			((SDIControlShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_DISPLAY_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDIDisplayShape)))
			((SDIDisplayShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_FILTER_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDIFilterShape)))
			((SDIFilterShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_IO_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDIIOShape)))
			((SDIIOShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_LINE_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDILineShape)))
			((SDILineShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_MODEL_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDIModelShape)))
			((SDIModelShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_USER_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDIUserShape)))
			((SDIUserShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_TRANSFORM_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDITransformShape)))
			((SDITransformShape *) this)->GetXMLInfo(child);
		else if ((child->GetName() == SHAPE_XML_UTILITY_SHAPE_ELEMENT) &&
		  IsKindOf(CLASSINFO(SDIUtilityShape)))
			((SDIUtilityShape *) this)->GetXMLInfo(child);
	return(ok);

}

/****************************** GetBoundingBoxPos ******************************/

bool
SDIShape::GetBoundingBoxPos(wxRealPoint &lBottom, wxRealPoint &rTop)
{
	double	w, h;

	GetBoundingBoxMax(&w, &h);
	lBottom.x = -w / 2.0 + GetX();
	lBottom.y = h / 2.0 + GetY();
	rTop.x = w / 2.0 + GetX();
	rTop.y = -h / 2.0 + GetY();
	return(true);

}

/******************************************************************************/
/****************************** ResetShapeLabel *******************************/
/******************************************************************************/

/*
 */

void
SDIShape::ResetLabel(void)
{
	SDIEvtHandler *myHandler = (SDIEvtHandler *) GetEventHandler();
	if (myHandler->pc) {
		myHandler->ResetLabel();
		wxClientDC dc(GetCanvas());
		FormatText(dc, myHandler->label);
		GetCanvas()->PrepareDC(dc);
	}
	if (myHandler->GetProcessType() == CONTROL_MODULE_CLASS) {
		wxNode *node = GetChildren().GetFirst();
		if (node)
			((SDIShape *)node->GetData())->ResetLabel();
	}

}

/******************************************************************************/
/****************************** SDIPolygonShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIPolygonShape, wxPolygonShape)

/****************************** Constructor ***********************************/

SDIPolygonShape::SDIPolygonShape(double width, double height): wxPolygonShape()
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	w = (width == 0.0)? 60.0: width;
	h = (height == 0.0)? 60.0: height;

}

/****************************** AddXMLInfo ************************************/

void
SDIPolygonShape::AddXMLInfo(DSAMXMLNode *parent)
{
	wxObjectList::compatibility_iterator objectNode = GetPoints()->GetFirst();
	DSAMXMLNode *pointListElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_POINT_LIST_ELEMENT);
	pointListElement->AddProperty(DSAM_XML_ID_ATTRIBUTE,
	  DSAM_XML_CURRENT_ATTRIBUTE_VALUE);
	while (objectNode) {
		wxRealPoint *point = (wxRealPoint *)objectNode->GetData();
		AddPointInfo(pointListElement, point);
		objectNode = objectNode->GetNext();
	}
	parent->AddChild(pointListElement);
	DSAMXMLNode *originalPointListElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_POINT_LIST_ELEMENT);
	originalPointListElement->AddProperty(DSAM_XML_ID_ATTRIBUTE,
	  DSAM_XML_ORIGINAL_ATTRIBUTE_VALUE);
	// Save the original (unscaled) points
	objectNode = GetOriginalPoints()->GetFirst();
	while (objectNode) {
		wxRealPoint *point = (wxRealPoint *)objectNode->GetData();
		AddPointInfo(originalPointListElement, point);
		objectNode = objectNode->GetNext();
	}
	parent->AddChild(originalPointListElement);
}

/****************************** GetPointListInfo ******************************/

bool
SDIPolygonShape::GetPointListInfo(wxXmlNode *myElement, wxList *points)
{
	static const WChar *funcName = wxT("SDIPolygonShape::GetPointListInfo");
	bool	ok = true;
	wxXmlNode	*child;
	wxRealPoint	*point;

	for (child = myElement->GetChildren(); ok && child; child = child->GetNext())
		if (child->GetName() == SHAPE_XML_POINT_ELEMENT) {
			if ((point = GetPointInfo(child)) == NULL) {
				NotifyError(wxT("%s: Could not set point."), funcName);
				ok = false;
				break;
			}
			points->Append((wxObject*) point);
		}
	return(ok);

}

/****************************** GetXMLInfo ************************************/

bool
SDIPolygonShape::GetXMLInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIPolygonShape::GetXMLInfo");
	bool	ok = true;
	double	height = 100.0, width = 100.0;
	wxRealPoint	*point;
	wxXmlNode	*child;
	MyXmlProperty	*prop;

	Create(NULL);
	wxList	*points = GetPoints();
	wxList	*originalPoints = GetOriginalPoints();
	for (child = myElement->GetChildren(); ok && child; child = child->GetNext())
		if (child->GetName() == SHAPE_XML_POINT_LIST_ELEMENT) {
			prop = (MyXmlProperty *) child->GetProperties();
			if (!prop && (prop->GetName() != DSAM_XML_ID_ATTRIBUTE))
				ok = false;
			else if (prop->GetValue() == DSAM_XML_CURRENT_ATTRIBUTE_VALUE) {
				if (!GetPointListInfo(child, points))
					ok = false;
			} else if (prop->GetValue() == DSAM_XML_ORIGINAL_ATTRIBUTE_VALUE) {
				double minX = 1000;
				double minY = 1000;
				double maxX = -1000;
				double maxY = -1000;
				if (GetPointListInfo(child, originalPoints)) {
					wxObjectList::compatibility_iterator objectNode = originalPoints->GetFirst();
					while (objectNode) {
						wxRealPoint *point = (wxRealPoint *)objectNode->GetData();
						if (point->x < minX)
							minX = point->x;
						if (point->y < minY)
							minY = point->y;
						if (point->x > maxX)
							maxX = point->x;
						if (point->y > maxY)
							maxY = point->y;
						objectNode = objectNode->GetNext();
					}
					SetOriginalWidth(maxX - minX);
					SetOriginalHeight(maxY - minY);
				} else
					ok = false;
			}
		}
	if (points->IsEmpty()) { // If no points_list assign a diamond instead.
		point = new wxRealPoint(0.0, (-height/2));
		points->Append((wxObject*) point);

		point = new wxRealPoint((width/2), 0.0);
		points->Append((wxObject*) point);

		point = new wxRealPoint(0.0, (height/2));
		points->Append((wxObject*) point);

		point = new wxRealPoint((-width/2), 0.0);
		points->Append((wxObject*) point);

		point = new wxRealPoint(0.0, (-height/2));
		points->Append((wxObject*) point);
	}
	if (originalPoints->IsEmpty()) {
		point = new wxRealPoint(0.0, (-height/2));
		originalPoints->Append((wxObject*) point);

		point = new wxRealPoint((width/2), 0.0);
		originalPoints->Append((wxObject*) point);

		point = new wxRealPoint(0.0, (height/2));
		originalPoints->Append((wxObject*) point);

		point = new wxRealPoint((-width/2), 0.0);
		originalPoints->Append((wxObject*) point);

		point = new wxRealPoint(0.0, (-height/2));
		originalPoints->Append((wxObject*) point);

		SetOriginalWidth(width);
		SetOriginalHeight(height);
	}
	CalculateBoundingBox();
	return(ok);

}

/******************************************************************************/
/****************************** SDIEllipseShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIEllipseShape, wxEllipseShape)

/****************************** Constructor ***********************************/

SDIEllipseShape::SDIEllipseShape(double width, double height): wxEllipseShape(
  width, height)
{
}

/****************************** AddXMLInfo ************************************/

void
SDIEllipseShape::AddXMLInfo(DSAMXMLNode *node)
{
	node->AddProperty(SHAPE_XML_WIDTH_ATTRIBUTE, GetWidth());
	node->AddProperty(SHAPE_XML_HEIGHT_ATTRIBUTE, GetHeight());

}

/****************************** GetXMLInfo ************************************/

bool
SDIEllipseShape::GetXMLInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIEllipseShape::GetXMLInfo");
	bool	ok = true;
	MyXmlProperty	*prop;

	for (prop = (MyXmlProperty *) myElement->GetProperties(); ok && prop; prop =
	  prop->GetNext())
		if ((prop->GetName() == SHAPE_XML_WIDTH_ATTRIBUTE) && !prop->GetPropVal(
		  &m_width))
			ok = false;
		else if ((prop->GetName() == SHAPE_XML_HEIGHT_ATTRIBUTE) && !prop->
		  GetPropVal(&m_height))
			ok = false;
	return(ok);

}

/******************************************************************************/
/****************************** SDIRectangleShape Methods *********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIRectangleShape, wxRectangleShape)

/****************************** Constructor ***********************************/

SDIRectangleShape::SDIRectangleShape(double width, double height):
  wxRectangleShape(width, height)
{

}

/****************************** AddXMLInfo ************************************/

void
SDIRectangleShape::AddXMLInfo(DSAMXMLNode *node)
{

	node->AddProperty(SHAPE_XML_WIDTH_ATTRIBUTE, GetWidth());
	node->AddProperty(SHAPE_XML_HEIGHT_ATTRIBUTE, GetHeight());
	if (m_cornerRadius != 0.0)
		node->AddProperty(SHAPE_XML_CORNER_ATTRIBUTE, m_cornerRadius);

}

/****************************** GetXMLInfo ************************************/

bool
SDIRectangleShape::GetXMLInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIRectangleShape::GetXMLInfo");
	bool	ok = true;
	MyXmlProperty	*prop;

	for (prop = (MyXmlProperty *) myElement->GetProperties(); ok && prop; prop =
	  prop->GetNext())
		if ((prop->GetName() == SHAPE_XML_WIDTH_ATTRIBUTE) && !prop->GetPropVal(
		  &m_width))
			ok = false;
		else if ((prop->GetName() == SHAPE_XML_HEIGHT_ATTRIBUTE) && !prop->
		  GetPropVal(&m_height))
			ok = false;
	return(ok);

}

/******************************************************************************/
/****************************** Friend Methods ********************************/
/******************************************************************************/

/****************************** AddPointInfo **********************************/

void
AddPointInfo(DSAMXMLNode *parent, wxRealPoint *point)
{
	DSAMXMLNode *pointElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  SHAPE_XML_POINT_ELEMENT);
	pointElement->AddProperty(SHAPE_XML_X_ATTRIBUTE, point->x);
	pointElement->AddProperty(SHAPE_XML_Y_ATTRIBUTE, point->y);
	parent->AddChild(pointElement);

}

/****************************** GetPointInfo **********************************/

wxRealPoint *
GetPointInfo(wxXmlNode *myElement)
{
	MyXmlProperty	*prop;

	wxRealPoint *point = new wxRealPoint;
	for (prop = (MyXmlProperty *) myElement->GetProperties(); prop; prop =
	  prop->GetNext())
		if (prop->GetName() == SHAPE_XML_X_ATTRIBUTE)
			prop->GetPropVal(&point->x);
		else if (prop->GetName() == SHAPE_XML_Y_ATTRIBUTE)
			prop->GetPropVal(&point->y);
	return(point);

}

#endif /* USE_WX_OGL */
