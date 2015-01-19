/**********************
 *
 * File:		ExtXMLDocument.h
 * Purpose: 	DSAM simulation base XML class.
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

#ifndef _EXTXMLDOCUMENT_H
#define _EXTXMLDOCUMENT_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/string.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

#include "UtDynaList.h"
#include "UtDynaBList.h"
#include "UtSSParser.h"
#include "UtSimScript.h"
#include "ExtXMLNode.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define DSAM_XML_APPLICATION_ELEMENT		wxT("application")
#define DSAM_XML_CFLIST_ELEMENT				wxT("cflist")
#define DSAM_XML_CONNECTIONS_ELEMENT		wxT("connections")
#define DSAM_XML_DSAM_ELEMENT				wxT("dsam")
#define DSAM_XML_ICLIST_ELEMENT				wxT("iclist")
#define DSAM_XML_INPUT_ELEMENT				wxT("input")
#define DSAM_XML_ION_CHANNELS_ELEMENT		wxT("ion_channels")
#define DSAM_XML_PAR_ELEMENT				wxT("par")
#define DSAM_XML_PAR_LIST_ELEMENT			wxT("par_list")
#define DSAM_XML_PARARRAY_ELEMENT			wxT("par_array")
#define DSAM_XML_OBJECT_ELEMENT				wxT("object")
#define DSAM_XML_OUTPUT_ELEMENT				wxT("output")
#define DSAM_XML_SHAPE_ELEMENT				wxT("shape")
#define DSAM_XML_SIMULATION_ELEMENT			wxT("simulation")

#define DSAM_XML_COUNT_ATTRIBUTE			wxT("count")
#define DSAM_XML_ID_ATTRIBUTE				wxT("id")
#define DSAM_XML_ENABLED_ATTRIBUTE			wxT("enabled")
#define DSAM_XML_LABEL_ATTRIBUTE			wxT("label")
#define DSAM_XML_NAME_ATTRIBUTE				wxT("name")
#define DSAM_XML_OBJLABEL_ATTRIBUTE			wxT("obj_label")
#define DSAM_XML_TYPE_ATTRIBUTE				wxT("type")
#define DSAM_XML_VALUE_ATTRIBUTE			wxT("value")
#define DSAM_XML_VERSION_ATTRIBUTE			wxT("version")

#define DSAM_XML_CURRENT_ATTRIBUTE_VALUE	wxT("current")
#define DSAM_XML_ORIGINAL_ATTRIBUTE_VALUE	wxT("original")
#define DSAM_XML_PROCESS_ATTRIBUTE_VALUE	wxT("process")

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Pre reference definitions ************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIDocument **************************************/

class DSAMEXT_API DSAMXMLDocument: public wxXmlDocument
{

  private:
	EarObjectPtr	mySimProcess;
	SimScriptPtr	simScriptPtr;

  public:
  
	DSAMXMLDocument(void);
	~DSAMXMLDocument(void);

	virtual void	AddShapeInfo(DSAMXMLNode *parent, void *shape) { ; }
	virtual void	AddLineShapes(DSAMXMLNode *node)	{ ; }
	virtual void	GetShapeInfo(wxXmlNode *shapeElement, DatumPtr pc)	{ ; }
	virtual void	GetConnectionsInfo(wxXmlNode *parent)	{ ;}

	void	AddAppInfo(DSAMXMLNode *parent);
	void	AddParGeneral(DSAMXMLNode *parent, UniParPtr p);
	void	AddParList(DSAMXMLNode *parent, UniParListPtr parList, const wxChar *
			  name = NULL);
	void	AddParListStandard(DSAMXMLNode *parent, UniParListPtr parList);
	void	AddSimConnections(DSAMXMLNode *node, DynaListPtr list, const WChar * type);
	DatumPtr	AddSimObjects(DSAMXMLNode *parent, DatumPtr start);
	void	AddSimulation(DSAMXMLNode *parent, EarObjectPtr simProcess);
	void	Create(EarObjectPtr simProcess);
	wxString &	CreateNotification(wxXmlNode *node, const wxChar *format, va_list
	 			  args);
	wxXmlNode *	FindXMLNode(wxXmlNode *node, const wxString &name);
	void	GetApplicationInfo(wxXmlNode *appElement);
	bool	GetCFListInfo(wxXmlNode *cFListElement, UniParList *parList);
	bool	GetConnectionInfo(wxXmlNode *connectionElement, DynaListPtr *p);
	bool	GetICListInfo(wxXmlNode *iCListElement, UniParList *parList);
	bool	GetParArrayInfo(wxXmlNode * parArrayElement, UniParList *parList);
	bool	GetParListInfo(wxXmlNode *parListElement, UniParList *parList);
	bool	GetParInfo(wxXmlNode *parElement, UniParList *parList);
	EarObjectPtr	GetSimProcess(void)		{ return mySimProcess; }
	bool	GetSimulationInfo(wxXmlNode *simElement);
	DatumPtr	InstallInst(wxXmlNode *objectElement, int type);
	DatumPtr	InstallProcess(wxXmlNode *objectElement);
	bool	InstallSimulationNodes(wxXmlNode *startSimElement);
	void	SetSimScriptPtr(SimScriptPtr ptr)	{ simScriptPtr = ptr; }
	bool	Translate(void);
	bool	ValidVersion(const wxString &s1, const wxString &s2);
	void	XMLNotifyWarning(wxXmlNode *node, const wxChar *format, ...);
	void	XMLNotifyError(wxXmlNode *node, const wxChar *format, ...);
	
};

#endif
