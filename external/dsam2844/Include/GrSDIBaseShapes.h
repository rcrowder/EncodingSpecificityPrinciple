/**********************
 *
 * File:		GrSDIBaseShapes.h
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

#ifndef _GRSDIBASESHAPES_H
#define _GRSDIBASESHAPES_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/ogl/ogl.h>
#include <wx/ogl/drawn.h>

#include "ExtXMLNode.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SHAPE_DEFAULT_FONT_POINTSIZE	5
#define SDI_STANDARD_SHAPE_WIDTH		100

// Misc. elements.
#define SHAPE_XML_ARCS_ELEMENT				wxT("arcs")
#define SHAPE_XML_ARROW_LIST_ELEMENT		wxT("arrow_list")
#define SHAPE_XML_ARROW_ELEMENT				wxT("arrow")
#define SHAPE_XML_ALIGNMENT_END_ELEMENT		wxT("align_end")
#define SHAPE_XML_ALIGNMENT_START_ELEMENT	wxT("align_start")
#define SHAPE_XML_ATTACHMENT_FROM_ELEMENT	wxT("attachment_from")
#define SHAPE_XML_ATTACHMENT_TO_ELEMENT		wxT("attachment_To")
#define SHAPE_XML_ATTACHMENTS_ELEMENT		wxT("attachments")
#define SHAPE_XML_BRUSH_ELEMENT				wxT("brush")
#define	SHAPE_XML_CONTROL_POINTS_ELEMENT	wxT("control_points")
#define SHAPE_XML_FONT_ELEMENT				wxT("font")
#define SHAPE_XML_INPUT_ELEMENT				"input"
#define SHAPE_XML_IS_SPLINE_ELEMENT			wxT("is_spline")
#define SHAPE_XML_KEEP_LINES_STRAIGHT_ELEMENT	wxT("keep_lines_straight")
#define SHAPE_XML_LINE_ELEMENT				wxT("line")
#define SHAPE_XML_MISC_ELEMENT				"misc"
#define SHAPE_XML_OBJECT_ELEMENT			"object"
#define SHAPE_XML_OUTPUT_ELEMENT			"output"
#define SHAPE_XML_PEN_ELEMENT				wxT("pen")
#define SHAPE_XML_REGION_ELEMENT			wxT("region")
#define SHAPE_XML_REGIONS_ELEMENT			wxT("regions")
#define SHAPE_XML_SHAPE_ELEMENT				wxT("shape")
#define SHAPE_XML_TEXT_ELEMENT				wxT("text")

// Shape-related attributes.
#define	SHAPE_XML_BRANCH_SPACING_ATTRIBUTE	wxT("branch_spacing")
#define	SHAPE_XML_BRANCH_STYLE_ATTRIBUTE	wxT("branch_style")
#define	SHAPE_XML_CENTRE_RESIZE_ATTRIBUTE	wxT("centre_resize")
#define	SHAPE_XML_COLOUR_ATTRIBUTE			wxT("colour")
#define SHAPE_XML_CORNER_ATTRIBUTE			wxT("corner")
#define SHAPE_XML_COUNT_ATTRIBUTE			"count"
#define SHAPE_XML_END_ATTRIBUTE				wxT("end")
#define SHAPE_XML_FAMILY_ATTRIBUTE			wxT("family")
#define	SHAPE_XML_FIXED_HEIGHT_ATTRIBUTE	wxT("fixed_height")
#define	SHAPE_XML_FIXED_WIDTH_ATTRIBUTE		wxT("fixed_width")
#define SHAPE_XML_FORMAT_MODE_ATTRIBUTE		wxT("format_mode")
#define SHAPE_XML_FROM_ATTRIBUTE			wxT("from")
#define SHAPE_XML_HEIGHT_ATTRIBUTE			wxT("height")
#define	SHAPE_XML_HILITE_ATTRIBUTE			wxT("hilite")
#define	SHAPE_XML_IS_SPLINE_ATTRIBUTE		wxT("is_spline")
#define SHAPE_XML_OBJLABEL_ATTRIBUTE		"obj_label"
#define	SHAPE_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE	wxT("maintain_aspect_ratio")
#define	SHAPE_XML_MIN_HEIGHT_ATTRIBUTE		wxT("min_height")
#define	SHAPE_XML_MIN_WIDTH_ATTRIBUTE		wxT("min_width")
#define	SHAPE_XML_MODE_ATTRIBUTE			"mode"
#define	SHAPE_XML_NAME_ATTRIBUTE			wxT("name")
#define	SHAPE_XML_NECK_LENGTH_ATTRIBUTE		wxT("neck_length")
#define	SHAPE_XML_PARENT_ATTRIBUTE			wxT("parent")
#define SHAPE_XML_PEN_COLOUR_ATTRIBUTE		wxT("pen_colour")
#define SHAPE_XML_PEN_STYLE_ATTRIBUTE		wxT("pen_style")
#define SHAPE_XML_POINT_ELEMENT				wxT("point")
#define SHAPE_XML_POINT_LIST_ELEMENT		wxT("point_list")
#define SHAPE_XML_POINTSIZE_ATTRIBUTE		wxT("point_size")
#define SHAPE_XML_PROPORTION_X_ATTRIBUTE	wxT("proportion_x")
#define SHAPE_XML_PROPORTION_Y_ATTRIBUTE	wxT("proportion_y")
#define	SHAPE_XML_ROTATION_ATTRIBUTE		wxT("rotation")
#define	SHAPE_XML_SENSITIVITY_ATTRIBUTE		wxT("sensitivity")
#define	SHAPE_XML_SHADOW_MODE_ATTRIBUTE		wxT("shadow_mode")
#define	SHAPE_XML_SIZE_ATTRIBUTE			wxT("size")
#define	SHAPE_XML_SPACE_ATTACHMENTS_ATTRIBUTE	wxT("space_attachments")
#define	SHAPE_XML_SPACING_ATTRIBUTE			wxT("spacing")
#define SHAPE_XML_STYLE_ATTRIBUTE			wxT("style")
#define	SHAPE_XML_STEM_LENGTH_ATTRIBUTE		wxT("stem_length")
#define	SHAPE_XML_TO_ATTRIBUTE				wxT("to")
#define SHAPE_XML_TEXT_COLOUR_ATTRIBUTE		wxT("text_colour")
#define SHAPE_XML_TYPE_ATTRIBUTE			wxT("type")
#define	SHAPE_XML_USE_ATTACHMENTS_ATTRIBUTE	wxT("use_attachments")
#define	SHAPE_XML_WEIGHT_ATTRIBUTE			wxT("weight")
#define	SHAPE_XML_WIDTH_ATTRIBUTE			wxT("width")
#define	SHAPE_XML_X_ATTRIBUTE				wxT("x")
#define	SHAPE_XML_X_OFFSET_ATTRIBUTE		wxT("x_offset")
#define	SHAPE_XML_Y_ATTRIBUTE				wxT("y")
#define	SHAPE_XML_Y_OFFSET_ATTRIBUTE		wxT("y_offset")

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** MyXmlProperty ************************************/

class MyXmlProperty: public wxXmlProperty
{
private:
	long	lVal;
public:
	MyXmlProperty *	GetNext()	{ return (MyXmlProperty *) wxXmlProperty::
		GetNext(); }
	bool	PropValOk()	{ if (!GetValue().IsEmpty()) return true;
			  NotifyError(wxT("%s: Could not find property value."), GetName().c_str());
			  return false; }
	bool	GetPropVal(double *val)	{ if (!PropValOk()) return(false);
			  GetValue().ToDouble(val); return(true);	}
	bool	GetPropVal(long *val)	{ if (!PropValOk()) return(false);
			  GetValue().ToLong(val); return(true);	}
	bool	GetPropVal(int *val)	{ if (!PropValOk()) return(false);
			  GetValue().ToLong(&lVal); *val = (int) lVal; return(true); }
	bool	GetPropVal(bool *val)	{ if (!PropValOk()) return(false);
			  GetValue().ToLong(&lVal); *val = (lVal != 0); return(true);	}
};

/*************************** SDIShape *****************************************/

class SDIShape: public wxShape
{
	DECLARE_ABSTRACT_CLASS(SDIShape)

  private:
	long	lVal;

  public:
	SDIShape(wxShapeCanvas *can = NULL);

	virtual	void	AddXMLInfo(DSAMXMLNode *parent) { ; }
	virtual	void	MyAddLine(wxShape* from, wxShape *to) { ; };
	virtual	bool	GetXMLInfo(wxXmlNode *node) { return true; }

	void	AddAttachmentsInfo(DSAMXMLNode *parent);
	void	AddBrushInfo(DSAMXMLNode *parent);
	void	AddFontInfo(DSAMXMLNode *parent, wxFont *font);
	void	AddLineInfo(DSAMXMLNode *parent);
	void	AddPenInfo(DSAMXMLNode *parent);
	friend	void	AddPointInfo(DSAMXMLNode *parent, wxRealPoint *point);
	void	AddRegions(DSAMXMLNode *parent);
	void	AddShapeInfo(DSAMXMLNode *parent);
	bool	GetAttachmentsInfo(wxXmlNode *myElement);
	bool	GetBoundingBoxPos(wxRealPoint &lBottom, wxRealPoint &rTop);
	bool	GetBrushInfo(wxXmlNode *myElement);
	wxFont *	GetFontInfo(wxXmlNode *myElement);
	bool	GetPenInfo(wxXmlNode *myElement);
	friend	wxRealPoint *	GetPointInfo(wxXmlNode *myElement);
	wxShapeRegion *	GetRegionInfo(wxXmlNode *myElement);
	bool	GetRegionsInfo(wxXmlNode *myElement);
	bool	GetShapeInfo(wxXmlNode *myElement);
	void	GetPropVal(wxXmlProperty *prop, double *val)
			  { prop->GetValue().ToDouble(val); }
	void	GetPropVal(wxXmlProperty *prop, long *val)
			  { prop->GetValue().ToLong(val); }
	void	GetPropVal(wxXmlProperty *prop, int *val)
			  { prop->GetValue().ToLong(&lVal); *val = (int) lVal; }
	void	GetPropVal(wxXmlProperty *prop, bool *val)
			  { prop->GetValue().ToLong(&lVal); *val = (lVal != 0); }
	void	ResetLabel(void);

};

/*************************** SDIPolygonShape **********************************/

class SDIPolygonShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIPolygonShape)

	double	w, h;

  public:
    SDIPolygonShape(double width = 0.0, double height = 0.0);

	virtual	void AddXMLInfo(DSAMXMLNode *node);
	virtual	bool GetXMLInfo(wxXmlNode *node);

	friend	wxRealPoint *	GetPointInfo(wxXmlNode *myElement);
	bool	GetPointListInfo(wxXmlNode *myElement, wxList *points);

};

/*************************** SDIEllipseShape **********************************/

/*
 * A few new shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */

class SDIEllipseShape: public wxEllipseShape
{
	DECLARE_DYNAMIC_CLASS(SDIEllipseShape)

  private:
  public:
	SDIEllipseShape(double width = 0.0, double height = 0.0);

	virtual	void AddXMLInfo(DSAMXMLNode *node);
	virtual	bool GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDIRectangleShape ********************************/

class SDIRectangleShape: public wxRectangleShape
{
	DECLARE_DYNAMIC_CLASS(SDIRectangleShape)

  public:
    SDIRectangleShape(double width = 0.0, double height = 0.0);

	virtual	void AddXMLInfo(DSAMXMLNode *node);
	virtual	bool GetXMLInfo(wxXmlNode *myElement);

};

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

void	AddPointInfo(DSAMXMLNode *parent, wxRealPoint *point);

wxRealPoint *	GetPointInfo(wxXmlNode *myElement);

#endif
