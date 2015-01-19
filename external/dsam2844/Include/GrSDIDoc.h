/**********************
 *
 * File:		GrSDIDoc.h
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

#ifndef _GRSDIDOC_H
#define _GRSDIDOC_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/docview.h>
#include <wx/string.h>
#include <wx/filesys.h>

#include <wx/ogl/ogl.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SDI_DOCUMENT_ALL_FILES_EXT		wxT("*")
#define SDI_DOCUMENT_SIM_FILE_EXT		wxT("sim")
#define SDI_DOCUMENT_SPF_FILE_EXT		wxT("spf")
#define SDI_DOCUMENT_XML_FILE_EXT		wxT("xml")

#if wxUSE_STD_IOSTREAM
#	define	SDI_DOC_OSTREAM wxSTD ostream
#	define	SDI_DOC_ISTREAM wxSTD istream
#else
#	define	SDI_DOC_OSTREAM wxOutputStream
#	define	SDI_DOC_ISTREAM wxInputStream
#endif

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

/*
 * A diagram document, which contains a diagram.
 */
 
class SDIDocument: public wxDocument
{
	DECLARE_DYNAMIC_CLASS(SDIDocument)

  private:

  public:
	SDIDiagram diagram;

	SDIDocument(void);
	~SDIDocument(void);

	virtual SDI_DOC_OSTREAM&	SaveObject(SDI_DOC_OSTREAM& stream);
	virtual SDI_DOC_ISTREAM&	LoadObject(SDI_DOC_ISTREAM& stream);

	inline wxDiagram *	GetDiagram() { return &diagram; }

	void	ClearDocument(void);
	bool	OnCloseDocument(void);
	bool	OnNewDocument(void);
	bool	OnOpenDocument(const wxString& fileName);
	bool	Revert(void);
	SDI_DOC_OSTREAM&	SaveXMLObject(SDI_DOC_OSTREAM& stream);
	void	SetSimWorkingDirectory(const wxString &directory);

};

#endif
