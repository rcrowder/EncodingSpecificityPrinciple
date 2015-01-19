/**********************
 *
 * File:		GrSDIDoc.cpp
 * Purpose: 	Simulation Design Interface base document class.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
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
#include "UtAppInterface.h"

#include "GrIPCServer.h"
#include "GrSimMgr.h"
#include "GrSDIFrame.h"
#include "GrSDIDiagram.h"
#include "GrSDIDoc.h"
#include "ExtXMLDocument.h"
#include "GrMainApp.h"
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

IMPLEMENT_DYNAMIC_CLASS(SDIDocument, wxDocument)

/******************************************************************************/
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIDocument::SDIDocument(void)
{
	wxGetApp().GetGrMainApp()->SetDiagram(&diagram);

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDIDocument::~SDIDocument(void)
{
	FreeSim_AppInterface();
	wxGetApp().SetAudModelLoadedFlag(false);

}

/******************************************************************************/
/****************************** SetSimWorkingDirectory ************************/
/******************************************************************************/

void
SDIDocument::SetSimWorkingDirectory(const wxString &directory)
{
	SET_PARS_POINTER(GetPtr_AppInterface()->audModel);
	SetProcessSimPtr_Utility_SimScript(GetPtr_AppInterface()->audModel);
	SetWorkingDirectory_AppInterface((wxChar *) directory.c_str());
	SetParsFilePath_Utility_SimScript(GetPtr_AppInterface()->workingDirectory);

}

/******************************************************************************/
/****************************** ClearDocument *********************************/
/******************************************************************************/

void
SDIDocument::ClearDocument(void)
{
	diagram.DeleteAllShapes();
	FreeSim_AppInterface();
	wxClientDC dc(diagram.GetCanvas());
	diagram.Clear(dc);			// This was not being done automatically.
	diagram.SetSimProcess(NULL);
	wxGetApp().SetAudModelLoadedFlag(false);

}

/******************************************************************************/
/****************************** OnCloseDocument *******************************/
/******************************************************************************/

bool
SDIDocument::OnCloseDocument(void)
{
	wxDocument::OnCloseDocument();
	ClearDocument();
	return true;

}

/******************************************************************************/
/****************************** OnOpenDocument ********************************/
/******************************************************************************/

bool
SDIDocument::OnOpenDocument(const wxString& fileName)
{
	static const wxChar *funcName = wxT("SDIDocument::OnOpenDocument");
	bool	ok = wxDocument::OnOpenDocument(fileName);

	if (ok && !diagram.VerifyDiagram()) {
		NotifyError(wxT("%s: Diagram verification failed.\n"), funcName);
		ClearDocument();
		ok = false;
	}

	return (ok);

}

/******************************************************************************/
/****************************** OnNewDocument *********************************/
/******************************************************************************/

bool
SDIDocument::OnNewDocument(void)
{
	static const wxChar *funcName = wxT("SDIDocument::OnNewDocument");

	wxDocument::OnNewDocument();
	FreeSim_AppInterface();
	wxClientDC dc(diagram.GetCanvas());
	diagram.Clear(dc);			// This was not being done automatically.
	if ((GetPtr_AppInterface()->audModel = Init_EarObject(wxT(
	  "Util_SimScript"))) == NULL) {
		NotifyError(wxT("%s: Could not initialise process."), funcName);
		return(false);
	}
	SetSimWorkingDirectory(wxGetCwd());
	SetSimulationFileFlag_AppInterface(FALSE);
	wxGetApp().SetAudModelLoadedFlag(true);
	return (true);

}

/******************************************************************************/
/****************************** Revert ****************************************/
/******************************************************************************/

bool
SDIDocument::Revert(void)
{
	if (!wxGetApp().GetAudModelLoadedFlag())
		return(true);
	wxFileName	fileName = GetFilename();
	OnCloseDocument();
	return(OnOpenDocument(fileName.GetFullName()));

}

/******************************************************************************/
/****************************** SaveObject ************************************/
/******************************************************************************/

SDI_DOC_OSTREAM&
SDIDocument::SaveObject(SDI_DOC_OSTREAM& stream)
{
	wxFileName	tempFileName, fileName = GetFilename();

	wxDocument::SaveObject(stream);
	if (wxGetCwd() != fileName.GetPath())
		SetSimWorkingDirectory(fileName.GetPath());
	if (fileName.GetExt().IsSameAs(SDI_DOCUMENT_XML_FILE_EXT, FALSE))
		return(SaveXMLObject(stream));

	DiagModeSpecifier oldDiagMode = GetDSAMPtr_Common()->diagMode;
	tempFileName.AssignTempFileName(wxT("diag"));

	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
	if (fileName.GetExt().IsSameAs(SDI_DOCUMENT_SIM_FILE_EXT, FALSE)) {
		WriteParFiles_Datum((wxChar *) fileName.GetPath().c_str(),
		  GetSimulation_AppInterface());
		WriteSimScript_Datum((wxChar *)tempFileName.GetFullPath().c_str(),
		  GetSimulation_AppInterface());
	} else {
		FILE *oldFp = GetDSAMPtr_Common()->parsFile;
		SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
		SetParsFile_Common((wxChar * )tempFileName.GetFullPath().c_str(),
		  OVERWRITE);
		ListParameters_AppInterface();
		fclose(GetDSAMPtr_Common()->parsFile);
		GetDSAMPtr_Common()->parsFile = oldFp;
	}
	SetDiagMode(oldDiagMode);
	wxTransferFileToStream(tempFileName.GetFullPath(), stream);
	wxRemoveFile(tempFileName.GetFullPath());

	return stream;

}

/******************************************************************************/
/****************************** LoadObject ************************************/
/******************************************************************************/

SDI_DOC_ISTREAM&
SDIDocument::LoadObject(SDI_DOC_ISTREAM& stream)
{
	static const wxChar *funcName = wxT("SDIDocument::LoadObject");
	bool	isXMLFile;
	wxFileName tempFileName;
	wxDocument::LoadObject(stream);

	wxFileName	fileName = GetFilename();

	diagram.DeleteAllShapes();
	ResetStepCount_Utility_Datum();
	if ((isXMLFile = (GetSimFileType_Utility_SimScript((wxChar *) fileName.
	  GetExt().c_str()) == UTILITY_SIMSCRIPT_XML_FILE)) == true) {
		wxGetApp().GetGrMainApp()->SetSimulationFile(fileName);
		if (!wxGetApp().GetGrMainApp()->LoadXMLDocument()) {
			NotifyError(wxT("%s: Could not load XML Document."), funcName);
			return(stream);
		}
		GetPtr_AppInterface()->canLoadSimulationFlag = false;
	} else {
// Do I need this stream-swapping stuff?
		wxGetApp().GetGrMainApp()->SetSimulationFile(fileName);
//		tempFileName = wxFileName::CreateTempFileName("simFile");
//		wxTransferStreamToFile(stream, tempFileName.GetFullName());
//		wxGetApp().GetGrMainApp()->SetSimulationFile(tempFileName);
	}
	if (!wxGetApp().GetFrame()->LoadSimulation()) {
		wxRemoveFile(tempFileName.GetFullName());
		return(stream);
	}
	GetPtr_AppInterface()->canLoadSimulationFlag = true;

	diagram.SetSimProcess(GetSimProcess_AppInterface());
	if (!isXMLFile)
		diagram.DrawSimulation();

//	if (!isXMLFile) {
//		wxRemoveFile(tempFileName.GetFullName());
//		wxGetApp().GetGrMainApp()->SetSimulationFile(fileName);
//	}
	SetParsFilePath_Common(GetPtr_AppInterface()->workingDirectory);
	wxGetApp().SetAudModelLoadedFlag(true);
	wxString lastDirectory = fileName.GetPath();
	GetDocumentManager( )->SetLastDirectory(lastDirectory);
	return stream;

}

/******************************************************************************/
/****************************** SaveXMLObject *********************************/
/******************************************************************************/

SDI_DOC_OSTREAM&
SDIDocument::SaveXMLObject(SDI_DOC_OSTREAM& stream)
{
	wxDocument::SaveObject(stream);
	diagram.SetSimProcess(GetSimProcess_AppInterface());
	SDIXMLDocument	doc(&diagram);
	doc.Create();
	doc.Save(GetFilename());
	return(stream);

}

#endif /* USE_WX_OGL */
