/**********************
 *
 * File:		GrSDIXMLDoc.cpp
 * Purpose: 	Simulation Design Interface base XML class.
 * Comments:	Contains a revision of the wxExpr code for saving the SDI.
 * Author:		L.P.O'Mard
 * Created:		14 Feb 2005
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

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"

#include "ExtXMLNode.h"

#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"

#include "GrSDIEvtHandler.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"
#include "GrSDIDiagram.h"
#include "GrSDIXMLDoc.h"

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

SDIXMLDocument::SDIXMLDocument(SDIDiagram *theDiagram)
{
	diagramLoadInitiated = false;
	diagram = theDiagram;
	diagram->SetLoadIDsFromFile(true);
	if (diagram->GetSimProcess())
		SetSimScriptPtr((SimScriptPtr) diagram->GetSimProcess()->module->
		  parsPtr);

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDIXMLDocument::~SDIXMLDocument(void)
{

}

/******************************************************************************/
/****************************** AddShapeInfo **********************************/
/******************************************************************************/

void
SDIXMLDocument::AddShapeInfo(DSAMXMLNode *parent, void *shapePtr)
{
	SDIShape *shape = (SDIShape *) shapePtr;
	shape->AddShapeInfo(parent);

}

/******************************************************************************/
/****************************** AddLineShapes *********************************/
/******************************************************************************/

void
SDIXMLDocument::AddLineShapes(DSAMXMLNode *parent)
{
	wxShape *shape;

	DSAMXMLNode *connectionsElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  DSAM_XML_CONNECTIONS_ELEMENT);

	wxNode *node = diagram->GetShapeList()->GetFirst();
	while (node) {
		shape = (wxShape *) node->GetData();
		if (shape->IsKindOf(CLASSINFO(wxLineShape)))
			AddShapeInfo(connectionsElement, shape);
		node = node->GetNext();
	}
	parent->AddChild(connectionsElement);

}

/******************************************************************************/
/****************************** Create ****************************************/
/******************************************************************************/

void
SDIXMLDocument::Create(void)
{
	DSAMXMLDocument::Create(diagram->GetSimProcess());

}

/******************************************************************************/
/****************************** GetClassInfo **********************************/
/******************************************************************************/

wxClassInfo *
SDIXMLDocument::GetClassInfo(wxXmlNode *shapeElement)
{
	static const wxChar *funcName = wxT("SDIXMLDocument::GetClassInfo");
	wxString	type;

	shapeElement->GetPropVal(DSAM_XML_TYPE_ATTRIBUTE, &type);
	if (type.empty()) {
		XMLNotifyWarning(shapeElement, wxT("%s: Could not find shape type"),
		  funcName);
		diagram->SetOk(false);
		return(NULL);
	}
	wxClassInfo *classInfo = wxClassInfo::FindClass(type);
	if (!classInfo) {
		XMLNotifyWarning(shapeElement, wxT("%s: Could not identify class for ")
		  wxT("'%s' shape type"), funcName, type.c_str());
		diagram->SetOk(false);
		return(NULL);
	}
	return(classInfo);

}

/******************************************************************************/
/****************************** CreateLoadShape *******************************/
/******************************************************************************/

SDIShape *
SDIXMLDocument::CreateLoadShape(wxXmlNode *shapeElement, DatumPtr pc)
{
	static const wxChar *funcName = wxT("SDIXMLDocument::CreateLoadShape");

	wxClassInfo *classInfo = GetClassInfo(shapeElement);
	if (!classInfo) {
		XMLNotifyWarning(shapeElement, wxT("%s: Could not identify class for ")
		  wxT("shape type"), funcName);
		diagram->SetOk(false);
		return(NULL);
	}
	SDIShape *shape = (SDIShape *) diagram->CreateLoadShape(pc, classInfo,
	  NULL);
	if (!shape) {
		XMLNotifyWarning(shapeElement, wxT("%s: Could not create shape."),
		  funcName);
		delete classInfo;
		return(NULL);
	}
	if (!((SDIShape *)shape)->GetShapeInfo(shapeElement))
		diagram->SetOk(false);
	return(shape);

}

/******************************************************************************/
/****************************** GetShapeInfo **********************************/
/******************************************************************************/

void
SDIXMLDocument::GetShapeInfo(wxXmlNode *shapeElement, DatumPtr pc)
{
	if (!diagramLoadInitiated) {
		diagramLoadInitiated = true;
		diagram->SetOk(true);
	}
	if (!shapeElement || (shapeElement->GetName() != SHAPE_XML_SHAPE_ELEMENT))
		return;
	SDIShape *shape = CreateLoadShape(shapeElement, pc);

}

/******************************************************************************/
/****************************** FindShape *************************************/
/******************************************************************************/

wxShape *
SDIXMLDocument::FindShape(long id)
{
	wxShape	*shape;

	wxNode *node = diagram->GetShapeList()->GetFirst();
	while (node) {
		shape = (wxShape *) node->GetData();
		if (shape->GetId() == id)
			return(shape);
		node = node->GetNext();
	}
	return(NULL);

}

/******************************************************************************/
/****************************** GetConnectionsInfo ****************************/
/******************************************************************************/

void
SDIXMLDocument::GetConnectionsInfo(wxXmlNode *simElement)
{
	wxXmlNode	*connectionsElement;

	if ((connectionsElement = FindXMLNode(simElement->GetChildren(),
	 DSAM_XML_CONNECTIONS_ELEMENT)) != NULL)
		GetLineShapeInfo(connectionsElement);

}

/******************************************************************************/
/****************************** GetLineShapeInfo ******************************/
/******************************************************************************/

void
SDIXMLDocument::GetLineShapeInfo(wxXmlNode *myElement)
{
	static const wxChar *funcName = wxT("SDIXMLDocument::GetLineShapeInfo");
	bool	ok = true;
	int		fromId = -1, toId = -1;
	wxXmlNode	*child, *lineShapeElement;
	MyXmlProperty  *prop;

	for (child = myElement->GetChildren(); ok && child; child = child->GetNext())
		if (child->GetName() == SHAPE_XML_SHAPE_ELEMENT) {
			wxClassInfo *classInfo = GetClassInfo(child);
			if (!classInfo) {
				XMLNotifyWarning(child, wxT("%s: Could not identify class."),
				  funcName);
				diagram->SetOk(false);
				return;
			}
			SDIShape *shape = CreateLoadShape(child, NULL);
			if ((lineShapeElement = FindXMLNode(child->GetChildren(),
			  SHAPE_XML_LINE_SHAPE_ELEMENT)) == NULL) {
				XMLNotifyWarning(child, wxT("%s: Could not find line_shape information."),
				  funcName);
				ok = false;
				break;
			}
			for (prop = (MyXmlProperty *) lineShapeElement->GetProperties();
			  ok && prop; prop = prop->GetNext())
				if ((prop->GetName() == SHAPE_XML_FROM_ATTRIBUTE) &&
					!prop->GetPropVal(&fromId))
						ok = false;
				else if ((prop->GetName() == SHAPE_XML_TO_ATTRIBUTE) &&
				  !prop->GetPropVal(&toId))
						ok = false;

			wxLineShape *lineShape = (wxLineShape *) shape;
			if ((fromId >= 0) && (toId >= 0)) {
				wxShape *from = FindShape(fromId);
				wxShape *to = FindShape(toId);
				from->AddLine((wxLineShape *) shape, to, lineShape->
				  GetAttachmentFrom(), lineShape->GetAttachmentTo());
			}
		}
	diagram->SetOk(ok);

}
