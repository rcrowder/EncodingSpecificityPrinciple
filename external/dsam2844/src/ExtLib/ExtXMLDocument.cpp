/**********************
 *
 * File:		ExtXMLDocument.cpp
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

#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtSimScript.h"
#include "UtString.h"
#include "UtAppInterface.h"
#include "ExtMainApp.h"

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
/****************************** Constructor ***********************************/
/******************************************************************************/

DSAMXMLDocument::DSAMXMLDocument(void)
{
	mySimProcess = NULL;
	simScriptPtr = NULL;

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

DSAMXMLDocument::~DSAMXMLDocument(void)
{

}

/******************************************************************************/
/****************************** Create ****************************************/
/******************************************************************************/

void
DSAMXMLDocument::Create(EarObjectPtr simProcess)
{
	SetVersion(wxT("1.1"));
	DSAMXMLNode *dSAMElement = new DSAMXMLNode(wxXML_ELEMENT_NODE, DSAM_XML_DSAM_ELEMENT);
	dSAMElement->AddProperty(DSAM_XML_VERSION_ATTRIBUTE, GetDSAMPtr_Common()->
	  version);
	AddAppInfo(dSAMElement);
	AddSimulation(dSAMElement, simProcess);
	SetRoot(dSAMElement);

}

/******************************************************************************/
/****************************** AddSimConnections **************************/
/******************************************************************************/

void
DSAMXMLDocument::AddSimConnections(DSAMXMLNode *node, DynaListPtr list,
  const WChar * type)
{
	DynaListPtr	dNode;

	for (dNode = list; dNode != NULL; dNode = dNode->next) {
		DSAMXMLNode *connectionElement = new DSAMXMLNode(wxXML_ELEMENT_NODE, type);
		connectionElement->AddProperty(DSAM_XML_LABEL_ATTRIBUTE, (WChar *) 
		  dNode->data);
		node->AddChild(connectionElement);
	}
	
}

/******************************************************************************/
/****************************** AddSimObjects *********************************/
/******************************************************************************/

#define CREATE_OBJECT_ELEMENT(PC, TYPE)	\
			DSAMXMLNode *objectElement = new DSAMXMLNode(wxXML_ELEMENT_NODE, \
			  DSAM_XML_OBJECT_ELEMENT); \
			objectElement->AddProperty(DSAM_XML_TYPE_ATTRIBUTE, (TYPE)); \
			if ((PC)->label && *(PC)->label) \
				objectElement->AddProperty(DSAM_XML_LABEL_ATTRIBUTE, \
				  (PC)->label)

DatumPtr
DSAMXMLDocument::AddSimObjects(DSAMXMLNode *parent, DatumPtr start)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::AddSimObjects");
	DatumPtr	pc, lastInstruction = NULL;

	for (pc = start; pc; pc = pc->next) {
		switch (pc->type) {
		case PROCESS: {
			if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
				AddSimulation(parent, pc->data);
				break;
			}
			CREATE_OBJECT_ELEMENT(pc, DSAM_XML_PROCESS_ATTRIBUTE_VALUE);
			objectElement->AddProperty(DSAM_XML_NAME_ATTRIBUTE, 
			  pc->data->module->name);
			if (!pc->data->module->onFlag)
				objectElement->AddProperty(DSAM_XML_ENABLED_ATTRIBUTE, wxT("0"));
			AddSimConnections(objectElement, pc->u.proc.inputList,
			  DSAM_XML_INPUT_ELEMENT);
			AddSimConnections(objectElement, pc->u.proc.outputList,
			  DSAM_XML_OUTPUT_ELEMENT);
			AddParList(objectElement, GetUniParListPtr_ModuleMgr(pc->data));
			AddShapeInfo(objectElement, pc->shapePtr);
			parent->AddChild(objectElement);
			break; }
		case REPEAT: {
			CREATE_OBJECT_ELEMENT(pc, GetProcessName_Utility_Datum(pc));
			objectElement->AddProperty(DSAM_XML_COUNT_ATTRIBUTE, wxString::
			  Format(wxT("%d"), pc->u.loop.count));
			AddShapeInfo(objectElement, pc->shapePtr);
			pc = lastInstruction = AddSimObjects(objectElement, pc->next);
			parent->AddChild(objectElement);
			break; }
		case RESET: {
			CREATE_OBJECT_ELEMENT(pc, GetProcessName_Utility_Datum(pc));
			objectElement->AddProperty(DSAM_XML_OBJLABEL_ATTRIBUTE, pc->u.
			  ref.string);
			AddShapeInfo(objectElement, pc->shapePtr);
			parent->AddChild(objectElement);
			break; }
		case STOP:
			return(pc);
			break;
		default:
			NotifyError(wxT("%s: Not yet implemented (%d)"), funcName, pc->
			  type);
		}
		if ((lastInstruction = pc) == NULL)
			break;
	}
	return (lastInstruction);

}

#undef CREATE_OBJECT_ELEMENT

/******************************************************************************/
/****************************** AddSimulation ******************************/
/******************************************************************************/

void
DSAMXMLDocument::AddSimulation(DSAMXMLNode *parent, EarObjectPtr simProcess)
{
	DSAMXMLNode *simElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  DSAM_XML_SIMULATION_ELEMENT);
	AddParList(simElement, GetUniParListPtr_ModuleMgr(simProcess));
	parent->AddChild(simElement);
	AddSimObjects(simElement, GetSimulation_ModuleMgr(simProcess));
	AddLineShapes(simElement);

}

/******************************************************************************/
/****************************** AddParGeneral *********************************/
/******************************************************************************/

void
DSAMXMLDocument::AddParGeneral(DSAMXMLNode *parent, UniParPtr p)
{
	int		i, oldIndex;
	wxString	str;

	switch (p->type) {
	case UNIPAR_CFLIST: {
		DSAMXMLNode *cFListElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  DSAM_XML_CFLIST_ELEMENT);
		AddParList(cFListElement, (*p->valuePtr.cFPtr)->cFParList);
		AddParList(cFListElement, (*p->valuePtr.cFPtr)->bParList);
		parent->AddChild(cFListElement);
		break; }
	case UNIPAR_ICLIST: {
		DynaListPtr	iCNode;
		DSAMXMLNode *iCListElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  DSAM_XML_ICLIST_ELEMENT);
		AddParList(iCListElement, (*p->valuePtr.iCPtr)->parList);
		parent->AddChild(iCListElement);
		DSAMXMLNode *iCElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  DSAM_XML_ION_CHANNELS_ELEMENT);		
		for (iCNode = (*p->valuePtr.iCPtr)->ionChannels; iCNode; iCNode =
		  iCNode->next) {
			AddParList(iCElement, ((IonChannelPtr) iCNode->data)->parList);
		}
		iCListElement->AddChild(iCElement);
		break; }
	case UNIPAR_PARARRAY: {
		DSAMXMLNode *parArrayElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  DSAM_XML_PARARRAY_ELEMENT);
		parArrayElement->AddProperty(DSAM_XML_NAME_ATTRIBUTE, (*p->valuePtr.pAPtr)->
		  name);
		AddParList(parArrayElement, (*p->valuePtr.pAPtr)->parList);
		parent->AddChild(parArrayElement);
		break; }
	case UNIPAR_PARLIST:
		if (p->valuePtr.parList.process)
			SET_PARS_POINTER(*p->valuePtr.parList.process);
		AddParList(parent, *p->valuePtr.parList.list, p->abbr);
		break;
	case UNIPAR_INT_ARRAY:
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
	case UNIPAR_STRING_ARRAY:
	case UNIPAR_NAME_SPEC_ARRAY:
		for (i = 0; i < *p->valuePtr.array.numElements; i++) {
			oldIndex = p->valuePtr.array.index;
			p->valuePtr.array.index = i;
			DSAMXMLNode *parElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
			  DSAM_XML_PAR_ELEMENT);
			parElement->AddProperty(DSAM_XML_NAME_ATTRIBUTE, p->abbr);
			str.Printf(wxT("%d:%s"), i, GetParString_UniParMgr(p));
			parElement->AddProperty(DSAM_XML_VALUE_ATTRIBUTE, str);
			parent->AddChild(parElement);
			p->valuePtr.array.index = oldIndex;
		}
		break;
	case UNIPAR_SIMSCRIPT:
		break;
	default:
		if (*GetParString_UniParMgr(p) == '\0')
			break;
		DSAMXMLNode *parElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
		  DSAM_XML_PAR_ELEMENT);
		parElement->AddProperty(DSAM_XML_NAME_ATTRIBUTE, p->abbr);
		parElement->AddProperty(DSAM_XML_VALUE_ATTRIBUTE, 
		  GetParString_UniParMgr(p));
		parent->AddChild(parElement);
	}
	
}

/******************************************************************************/
/****************************** AddParListStandard ****************************/
/******************************************************************************/

/*
 * This routine runs backwards so that parameters are 'inserted' in the original
 * order.
 */

void
DSAMXMLDocument::AddParListStandard(DSAMXMLNode *parent, UniParListPtr parList)
{
	int		i;
	UniParPtr	p;

	for (i = 0, p = parList->pars; i < parList->numPars; i++, p++)
		if (p->enabled)
			AddParGeneral(parent, p);

}

/******************************************************************************/
/****************************** AddParList ************************************/
/******************************************************************************/

void
DSAMXMLDocument::AddParList(DSAMXMLNode *parent, UniParListPtr parList, const wxChar
  *name)
{
	if (!parList)
		return;
	DSAMXMLNode *parListElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  DSAM_XML_PAR_LIST_ELEMENT);
	if (name)
		parListElement->AddProperty(DSAM_XML_NAME_ATTRIBUTE, name);
	AddParListStandard(parListElement, parList);
	parent->AddChild(parListElement);

}

/******************************************************************************/
/****************************** AddAppInfo ************************************/
/******************************************************************************/

void
DSAMXMLDocument::AddAppInfo(DSAMXMLNode *parent)
{
	DSAMXMLNode *appElement = new DSAMXMLNode(wxXML_ELEMENT_NODE,
	  DSAM_XML_APPLICATION_ELEMENT);
	appElement->AddProperty(DSAM_XML_NAME_ATTRIBUTE, GetPtr_AppInterface()->
	  appName);
	appElement->AddProperty(DSAM_XML_VERSION_ATTRIBUTE, GetPtr_AppInterface()->
	  appVersion);
	AddParList(appElement, GetPtr_AppInterface()->parList);
	parent->AddChild(appElement);
	
}

/******************************************************************************/
/****************************** CreateNotification ****************************/
/******************************************************************************/

/*
 * This routine creates the notification for warning and error messages.
 */

wxString &
DSAMXMLDocument::CreateNotification(wxXmlNode *node, const wxChar *format, va_list
  args)
{
	static wxString	msg;
	wxString	location;

	msg.PrintfV(format, args);
//	location.Printf(wxT(": line %d, column %d"), node->Row(), node->Column());
	msg += location;
	return(msg);

}

/******************************************************************************/
/****************************** XMLNotifyError ******************************/
/******************************************************************************/

/*
 * This routine prints an error, adding the line and column of the problem
 */

void
DSAMXMLDocument::XMLNotifyError(wxXmlNode *node, const wxChar *format, ...)
{
	va_list	args;

	va_start(args, format);
	NotifyError((wxChar *) CreateNotification(node, format, args).c_str());
	va_end(args);

}

/******************************************************************************/
/****************************** XMLNotifyWarning ******************************/
/******************************************************************************/

/*
 * This routine prints an error, adding the line and column of the problem
 */

void
DSAMXMLDocument::XMLNotifyWarning(wxXmlNode *node, const wxChar *format, ...)
{
	va_list	args;

	va_start(args, format);
	NotifyWarning((wxChar*) CreateNotification(node, format, args).c_str());
	va_end(args);

}

/******************************************************************************/
/****************************** ValidVersion **********************************/
/******************************************************************************/

/*
 * This routine checks if the first two version numbers of two version strings
 * correspond.
 */

bool
DSAMXMLDocument::ValidVersion(const wxString &s1, const wxString &s2)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::ValidVersion");
	size_t		pos;

	if ((pos = s1.find(DSAM_VERSION_SEPARATOR, s1.find(DSAM_VERSION_SEPARATOR) +
	  1)) == s1.npos) {
		NotifyError(wxT("%s: Could not find valid version (%s)."), funcName,
		  s1.c_str());
		return(false);
	}
	if (s1.compare(0, pos, s2, 0, pos) != 0) {
		NotifyError(wxT("%s: Only versions '%s.x' are fully supported."),
		  funcName, s2.substr(0, pos).c_str());
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** GetParArrayInfo *******************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetParArrayInfo(wxXmlNode * parArrayElement, UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetParArrayInfo");
	wxString	parName;
	UniParPtr	par;
	UniParList	*subParList;
	wxXmlNode	*parListElement;

	if (!parArrayElement->GetPropVal(DSAM_XML_NAME_ATTRIBUTE, &parName)) {
		XMLNotifyError(parArrayElement, wxT("%s: Element must have a ")
		  wxT("name."), funcName);
		return(false);
	}
	subParList = parList;
	if ((par = FindUniPar_UniParMgr(&subParList, (wxChar *) parName.c_str(),
	  UNIPAR_SEARCH_ABBR)) == NULL) {
		XMLNotifyError(parArrayElement, wxT("%s: Parameter '%s' not ")
		  wxT("found"), funcName, (wxChar *) parName.c_str());
		return(false);
	}
	if ((parListElement = parArrayElement->GetChildren()) == NULL) {
	  	XMLNotifyError(parArrayElement, wxT("%s: Could not find sub-")
		  wxT("parameter list for '%s' par_array."), funcName, (wxChar *)
		  parName.c_str());
		return(false);
	}
	if (!GetParListInfo(parListElement, subParList)) {
		XMLNotifyError(parArrayElement, wxT("%s: Could not set sub-")
		  wxT("parameter list"), funcName);
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** GetCFListInfo *********************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetCFListInfo(wxXmlNode *cFListElement, UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetCFListInfo");
	UniParPtr	par;
	wxXmlNode	*parListElement;

	if ((par = FindUniPar_UniParMgr(&parList, wxT("CFLIST"),
	  UNIPAR_SEARCH_ABBR)) == NULL) {
		XMLNotifyError(cFListElement, wxT("%s: CFlist not found"),
		  funcName);
		return(false);
	}
	if (!(parListElement = cFListElement->GetChildren()) || (parListElement->
	  GetName() != DSAM_XML_PAR_LIST_ELEMENT)) {
		XMLNotifyError(cFListElement, wxT("%s CFList parameters not found"),
		  funcName);
		return(false);
	}
	if (!GetParListInfo(parListElement, (*par->valuePtr.cFPtr)->cFParList)) {
		XMLNotifyError(cFListElement, wxT("%s: Could not set CFList ")
		  wxT("parameters"), funcName);
		return(false);
	}
	if (!(parListElement = parListElement->GetNext()) || (parListElement->
	  GetName() != DSAM_XML_PAR_LIST_ELEMENT)) {
		XMLNotifyError(cFListElement, wxT("%s CFList bandwidth parameters not found"),
		  funcName);
		return(false);
	}
	if (!GetParListInfo(parListElement, (*par->valuePtr.cFPtr)->bParList)) {
		XMLNotifyError(cFListElement, wxT("%s: Could not set CFList ")
		  wxT("bandwidth parameters"), funcName);
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** GetICListInfo *********************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetICListInfo(wxXmlNode *iCListElement, UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetICListInfo");
	UniParPtr	par;
	wxXmlNode	*parListElement, *iCElement, *iCParListElement;

	if ((par = FindUniPar_UniParMgr(&parList, wxT("ICLIST"),
	  UNIPAR_SEARCH_ABBR)) == NULL) {
		XMLNotifyError(iCListElement, wxT("%s: IClist not found"), funcName);
		return(false);
	}
	if ((parListElement = iCListElement->GetChildren()) == NULL) {
		XMLNotifyError(iCListElement, wxT("%s ICList parameters not found"),
		  funcName);
		return(false);
	}
	if (!GetParListInfo(parListElement, (*par->valuePtr.iCPtr)->parList)) {
		XMLNotifyError(iCListElement, wxT("%s: Could not set main ICList ")
		  wxT("parameters"), funcName);
		return(false);
	}
	if ((iCElement = parListElement->GetNext()) && (iCElement->GetName() !=
	  DSAM_XML_ION_CHANNELS_ELEMENT)) {
		XMLNotifyError(iCListElement, wxT("%s: No ion_channel information ")
		  wxT("found."), funcName);
		return(false);
	}
		
	for (iCParListElement = iCElement->GetChildren(); iCParListElement;
	  iCParListElement = iCParListElement->GetNext()) {
		if (iCParListElement->GetName() != DSAM_XML_PAR_LIST_ELEMENT) {
			XMLNotifyError(iCElement, wxT("%s: Could not find an ion channel ")
			  DSAM_XML_PAR_LIST_ELEMENT wxT(" element."), funcName);
			return(false);
		}
		if (!GetParListInfo(iCParListElement, parList)) {
			XMLNotifyError(iCElement, wxT("%s: Could not set ion ")
			  wxT("channel parameters"), funcName);
			return(false);
		}
	}
	return(true);

}

/******************************************************************************/
/****************************** GetParInfo ************************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetParInfo(wxXmlNode *parListElement, UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetParInfo");
	wxString	parName, parValue;
	UniParPtr	par;
	wxXmlNode	*child;

	for (child = parListElement->GetChildren(); child; child = child->GetNext())
		if (child->GetName() == DSAM_XML_PAR_ELEMENT) {
			if (!child->GetPropVal(DSAM_XML_NAME_ATTRIBUTE, &parName)) {
				XMLNotifyError(child, wxT("%s: Missing parameter '%s'."),
				  funcName, DSAM_XML_NAME_ATTRIBUTE);
				return(false);
			}
			if ((par = FindUniPar_UniParMgr(&parList, (wxChar *) parName.c_str(),
			  UNIPAR_SEARCH_ABBR)) == NULL) {
				XMLNotifyError(child, wxT("%s: Parameter '%s' not found."),
				  funcName, parName.c_str());
				return(false);
			}
			if (!child->GetPropVal(DSAM_XML_VALUE_ATTRIBUTE, &parValue)) {
				XMLNotifyError(child, wxT("%s: Missing parameter '%s'."),
				  funcName, DSAM_XML_VALUE_ATTRIBUTE);
				return(false);
			}
			if (!SetParValue_UniParMgr(&parList, par->index, (wxChar *) parValue.
			  c_str())) {
				XMLNotifyError(child, wxT("%s: Could not set %s parameter to ")
				  wxT("%s."), funcName, parName.c_str(), parValue.c_str());
				return(false);
			}
		} else if (child->GetName() == DSAM_XML_PAR_LIST_ELEMENT) {
			if (!child->GetPropVal(DSAM_XML_NAME_ATTRIBUTE, &parName)) {
				XMLNotifyError(child, wxT("%s: '%s' Sub-parameter ")
				  wxT("must have a name."), funcName, DSAM_XML_PAR_LIST_ELEMENT);
				return(false);
			}
			if ((par = FindUniPar_UniParMgr(&parList, (wxChar *) parName.c_str(),
			  UNIPAR_SEARCH_ABBR)) == NULL) {
				XMLNotifyError(child, wxT("%s: Sub-parameter list ")
				  wxT("'%s' parameter '%s' not found"), funcName,
				  DSAM_XML_PAR_LIST_ELEMENT, parName.c_str());
				return(false);
			}
			if (!GetParListInfo(child, *par->valuePtr.parList.list)) {
				XMLNotifyError(child, wxT("%s: Could not set sub-")
				  wxT("parameter list"), funcName);
				return(false);
			}
		} else if ((child->GetName() == DSAM_XML_CFLIST_ELEMENT) &&
		  !GetCFListInfo(child, parList)) {
			XMLNotifyError(parListElement, wxT("%s: Failed reading CFList ")
			  wxT("parameters."), funcName);
			return(false);
		} else if ((child->GetName() == DSAM_XML_PARARRAY_ELEMENT) &&
		  !GetParArrayInfo(child, parList)) {
			XMLNotifyError(parListElement, wxT("%s: Failed reading ")
			  wxT("parArray parameters."), funcName);
			return(false);
		} else if ((child->GetName() == DSAM_XML_ICLIST_ELEMENT) &&
		  !GetICListInfo(child, parList)) {
			XMLNotifyError(parListElement, wxT("%s: Failed reading ")
			  wxT("IC list parameters."), funcName);
			return(false);
		}

	return(true);

}

/******************************************************************************/
/****************************** GetParListInfo ********************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetParListInfo(wxXmlNode *parListElement, UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetParListInfo");
	wxString	parName;

	if (parListElement->GetName() != DSAM_XML_PAR_LIST_ELEMENT) {
		XMLNotifyError(parListElement, wxT("%s: Element is not a ")
		  DSAM_XML_PAR_LIST_ELEMENT wxT(" element."), funcName);
		return(false);
	}
	if (!GetParInfo(parListElement, parList)) {
		XMLNotifyError(parListElement, wxT("%s: Failed reading parameters."),
		  funcName);
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** GetApplicationInfo ****************************/
/******************************************************************************/

void
DSAMXMLDocument::GetApplicationInfo(wxXmlNode *appElement)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetApplicationInfo");
	wxString	appName, appVersion;
	wxXmlNode	*parListElement;
	
	if (!appElement || (appElement->GetName() != DSAM_XML_APPLICATION_ELEMENT))
		return;
	if (!appElement->GetPropVal(DSAM_XML_NAME_ATTRIBUTE, &appName))
		return;

	if (!appElement->GetPropVal(DSAM_XML_VERSION_ATTRIBUTE, &appVersion) ||
	  !ValidVersion(appVersion, GetPtr_AppInterface()->appVersion)) {
		XMLNotifyWarning(appElement, wxT("%s: Invalid application version, ")
		  wxT("element ignored."), funcName);
		return;
	}
	if ((parListElement = appElement->GetChildren()) && (parListElement->
	  GetName() == DSAM_XML_PAR_LIST_ELEMENT))
		GetParListInfo(parListElement, GetPtr_AppInterface()->parList);
}

/******************************************************************************/
/****************************** GetConnectionInfo *****************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetConnectionInfo(wxXmlNode *connectionElement, DynaListPtr *p)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetConnectionInfo");
	wxString	label;

	if (!connectionElement->GetPropVal(DSAM_XML_LABEL_ATTRIBUTE, &label)) {
		XMLNotifyError(connectionElement, wxT("%s: Connection has no label."),
		  funcName);
		return(false);
	}
	Append_Utility_DynaList(p, InstallSymbol_Utility_SSSymbols(
	  &simScriptPtr->symList, (wxChar *) label.c_str(), STRING)->name);
	return(true);

}

/******************************************************************************/
/****************************** InstallProcess ********************************/
/******************************************************************************/

DatumPtr
DSAMXMLDocument::InstallProcess(wxXmlNode *objectElement)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::InstallProcess");
	wxString	moduleName, enabledStatus;
	DatumPtr	pc;
	wxXmlNode	*node;

	if (!objectElement->GetPropVal(DSAM_XML_NAME_ATTRIBUTE, &moduleName)) {
		XMLNotifyError(objectElement, wxT("%s: Missing process name"),
		  funcName);
		return(NULL);
	}
	if ((pc = InstallInst(objectElement, PROCESS)) == NULL) {
		XMLNotifyError(objectElement, wxT("%s: Could not initialise ")
		  wxT("instruction for '%s'"), funcName, (wxChar *) moduleName.c_str());
		return(NULL);
	}
	pc->u.proc.moduleName = InitString_Utility_String((wxChar *) moduleName.
	  c_str());
	if (!InitProcessInst_Utility_Datum(pc)) {
		XMLNotifyError(objectElement, wxT("%s: Could not initialise process ")
		  wxT("'%s'"), funcName, (wxChar *) moduleName.c_str());
		return(NULL);
	}
	if (objectElement->GetPropVal(DSAM_XML_ENABLED_ATTRIBUTE, &enabledStatus))
		EnableProcess_Utility_Datum(pc, DSAM_atoi(enabledStatus.c_str()));
	for (node = objectElement->GetChildren(); node; node = node->GetNext())
		if ((node->GetName() == DSAM_XML_PAR_LIST_ELEMENT) &&
		  !GetParListInfo(node, GetUniParListPtr_ModuleMgr(pc->data))) {
			XMLNotifyError(objectElement, wxT("%s: Could not initialise '%s' ")
			  wxT("module parameters"), funcName, (wxChar *) moduleName.c_str());
			return(NULL);
		} else if ((node->GetName() == DSAM_XML_INPUT_ELEMENT) &&
		  !GetConnectionInfo(node, &pc->u.proc.inputList)) {
			XMLNotifyError(objectElement, wxT("%s: Could not find '%s' module input ")
			  wxT("connections"), funcName, (wxChar *) moduleName.c_str());
			return(NULL);
		} else if ((node->GetName() == DSAM_XML_OUTPUT_ELEMENT) &&
		  !GetConnectionInfo(node, &pc->u.proc.outputList)) {
			XMLNotifyError(objectElement, wxT("%s: Could not find '%s' module output ")
			  wxT("connections"), funcName, (wxChar *) moduleName.c_str());
			return(NULL);
		} else if (node->GetName() == DSAM_XML_SHAPE_ELEMENT)
			GetShapeInfo(node, pc);

	if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
		XMLNotifyError(objectElement, wxT("%s: Must process sub simulation.  ")
		  wxT("Remember to check the preservation of the simProcess pars ")
		  wxT("pointer."),
		  funcName);

	}
	return(pc);

}

/******************************************************************************/
/****************************** InstallInst ***********************************/
/******************************************************************************/

DatumPtr
DSAMXMLDocument::InstallInst(wxXmlNode *objectElement, int type)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::InstallInst");
	wxString	label;
	DatumPtr	pc;

	if ((pc = InstallInst_Utility_Datum(simScriptPtr->simPtr, type)) == NULL) {
		XMLNotifyError(objectElement, wxT("%s: Could not install ")
		  wxT("instruction"), funcName);
		return(NULL);
	}
	if (objectElement->GetPropVal(DSAM_XML_LABEL_ATTRIBUTE, &label))
		pc->label = InitString_Utility_String((wxChar *) label.c_str());
	return(pc);

}

/******************************************************************************/
/****************************** InstallSimulationNodes ************************/
/******************************************************************************/

bool
DSAMXMLDocument::InstallSimulationNodes(wxXmlNode *startSimElement)
{
	static const wxChar *funcName = wxT(
	  "DSAMXMLDocument::InstallSimulationNodes");
	long	longVal;
	wxString	label, objectType, propVal;
	SymbolPtr	sp;
	DatumPtr	pc;
	wxXmlNode		*objectElement, *child;

	for (objectElement = startSimElement; objectElement && (objectElement->
	  GetName() == DSAM_XML_OBJECT_ELEMENT); objectElement = objectElement->
	  GetNext()) {
		if (!objectElement->GetPropVal(DSAM_XML_TYPE_ATTRIBUTE, &objectType)) {
			XMLNotifyError(objectElement, wxT("%s: Missing object 'type' in ")
			  wxT("simulation"), funcName);
			return(false);
		}
		child = objectElement->GetChildren();
		if (objectType.compare(DSAM_XML_PROCESS_ATTRIBUTE_VALUE) == 0) {
			if ((pc = InstallProcess(objectElement)) == NULL) {
				XMLNotifyError(objectElement, wxT("%s: Could not install ")
				  wxT("simulation object"), funcName);
				return(false);
			}
			GetShapeInfo(child, pc);
		} else if ((sp = LookUpSymbol_Utility_SSSymbols(dSAMMainApp->GetSymList(
		  ), (wxChar *) objectType.c_str())) != NULL) {
			switch (sp->type) {
			case REPEAT:
				if ((pc = InstallInst(objectElement, sp->type)) == NULL)
					break;
				if (!objectElement->GetPropVal(DSAM_XML_COUNT_ATTRIBUTE, &propVal)) {
					XMLNotifyError(objectElement, wxT("%s: Could not find ")
					  wxT("count."), funcName);
					return(false);
				}
				propVal.ToLong(&longVal);
				pc->u.loop.count = (int) longVal;
				GetShapeInfo(child, pc);
				while (child->GetName() != DSAM_XML_OBJECT_ELEMENT)
					child = child->GetNext();
				if (!InstallSimulationNodes(child))
					return(false);
				pc->u.loop.stopPC = InstallInst_Utility_Datum(simScriptPtr->
				  simPtr, STOP);
				break;
			case RESET:
				if ((pc = InstallInst(objectElement, sp->type)) == NULL)
					break;
				if (!objectElement->GetPropVal(DSAM_XML_OBJLABEL_ATTRIBUTE, &label)) {
					XMLNotifyError(objectElement, wxT("%s: reset process label ")
					  wxT("missing"), funcName);
					return(false);
				}
				pc->u.ref.string = InitString_Utility_String((wxChar *) label.
				  c_str());
				GetShapeInfo(child, pc);
				break;
			default:
				;
			}
			if (!pc) {
				XMLNotifyError(objectElement, wxT("%s: Could not install ")
				  wxT("simulation '%s' object"), funcName, sp->name);
				return(false);
			}
		}
		if (pc->label && *pc->label &&  !Insert_Utility_DynaBList(
		  simScriptPtr->labelBListPtr, CmpProcessLabels_Utility_Datum, pc)) {
			XMLNotifyError(objectElement, wxT("%s: Cannot insert process ")
			  wxT("labelled '%s' into simulation."), funcName, pc->label);
			return(false);
		}
	}
	return(true);

}

/******************************************************************************/
/****************************** GetSimulationInfo *****************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetSimulationInfo(wxXmlNode *simElement)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetSimulationInfo");
	bool	ok = true;
	DatumPtr	simulation;
	wxXmlNode	*parListElement, *startSimElement;

	if ((mySimProcess = Init_EarObject(wxT("Util_SimScript"))) == NULL) {
		NotifyError(wxT("%s: Could not initialise process."), funcName);
		return(false);
	}
	SET_PARS_POINTER(mySimProcess);
	SetProcessSimPtr_Utility_SimScript(mySimProcess);
	simScriptPtr = (SimScriptPtr) mySimProcess->module->parsPtr;
	simScriptPtr->simFileType = UTILITY_SIMSCRIPT_XML_FILE;
	SetParsFilePath_Utility_SimScript(GetPtr_AppInterface()->workingDirectory);
	if ((parListElement = simElement->GetChildren()) != NULL)
		GetParListInfo(parListElement, GetUniParListPtr_ModuleMgr(
		  mySimProcess));
	startSimElement = parListElement->GetNext();
	if (!InstallSimulationNodes(startSimElement)) {
		XMLNotifyError(simElement, wxT("%s: Could not install simulation."),
		  funcName);
		ok = false;
	}
	simulation = (ok)? GetSimulation_ModuleMgr(mySimProcess): NULL;
	if (ok)
		ok = CXX_BOOL(ResolveInstLabels_Utility_Datum(simulation,
		  simScriptPtr->labelBList));
	if (ok)
		SetDefaultConnections_Utility_Datum(simulation);
	if (ok)
		GetConnectionsInfo(simElement);
	if (ok && !SetSimulation_Utility_SimScript(simulation)) {
		XMLNotifyError(simElement, wxT("%s: Not enough lines, or invalid ")
		  wxT("parameters, in simulation node"), funcName);
		ok = false;
	}
	if (!ok)
		Free_EarObject(&mySimProcess);
	return(ok);
	
}

/******************************************************************************/
/****************************** Translate *************************************/
/******************************************************************************/

/*
 * This routine translates the document into information used by DSAM.
 */

bool
DSAMXMLDocument::Translate(void)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::Translate");
	wxString	docDSAMVersion;
	wxXmlNode	*appElement, *simElement;

	if (GetRoot()->GetName() != DSAM_XML_DSAM_ELEMENT) {
		NotifyError(wxT("%s: This is not a DSAM document, or file is corrupt."),
		  funcName);
		return(false);
	}
	if (!GetRoot()->GetPropVal(DSAM_XML_VERSION_ATTRIBUTE, &docDSAMVersion)) {
		NotifyError(wxT("%s: Could not get DSAM version."), funcName);
		return(false);
	}
	if (docDSAMVersion.empty() || !ValidVersion(docDSAMVersion.c_str(),
	  GetDSAMPtr_Common()->version)) {
		NotifyError(wxT("%s: Could not find valid DSAM version."), funcName);
		return(false);
	}
	appElement = GetRoot()->GetChildren();
	GetApplicationInfo(appElement);
	if (!(simElement = appElement->GetNext()) || (simElement->GetName() !=
	  DSAM_XML_SIMULATION_ELEMENT)) {
		NotifyError(wxT("%s: Could not load simulation."), funcName);
		return(false);
	}
	if (!GetSimulationInfo(simElement))
		return(false);
	return(true);
}

/******************************************************************************/
/****************************** FindXMLNode ***********************************/
/******************************************************************************/

/*
 * This function finds a named node in a list of nodes.
 * It returns NULL if the named node is not found;
 */

wxXmlNode *
DSAMXMLDocument::FindXMLNode(wxXmlNode *node, const wxString &name)
{
	for (; node; node = node->GetNext())
		if (node->GetName() == name)
			return(node);
	return(NULL);

}

