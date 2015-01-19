/**********************
 *
 * File:		ExtIPCClient.h
 * Purpose:		Inter-process communication client extension code module.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		09 Jan 2004
 * Updated:		
 * Copyright:	(c) 2004, 2010 Lowel P. O'Mard
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

#ifndef _EXTIPCCLIENT_H
#define _EXTIPCCLIENT_H 1

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/socket.h>
#endif

#include "GeCommon.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
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

/********************************** Pre-references ****************************/

class wxFileName;

/*************************** IPCClient ****************************************/

class DSAMEXT_API IPCClient: public wxSocketClient {

	bool	ok;
	IPCUtils	iPCUtils;
	wxString	buffer;

  public:
	IPCClient(const wxString& hostName, uShort theServicePort);
	virtual	~IPCClient(void);

	unsigned char	CheckStatus(void);
	bool	CreateSPFFromSimScript(wxFileName &fileName);
	bool	Errors(void);
	bool	GetAllOutputFiles(void);
	wxString& GetBuffer(void)	{ return buffer; }
	bool	GetParValue(const wxString &parName);
	EarObjectPtr	GetSimProcess(void);
	IPCUtils * GetIPCUtils(void)	{ return &iPCUtils; }
	bool	InitSimFromFile(const wxString &simFileName);
	bool	InitSimulation(const wxString& simulation);
	bool	Ok(void)	{ return ok; }
	bool	ReadString(wxString &s);
	bool	RunSimulation(void);
	bool	SendArguments(int argc, wxChar **argv);
	bool	SendCommand(IPCCommandSpecifier command);
	bool	SendInputProcess(void);
	void	WaitForReady(void)	{ while (CheckStatus() == IPC_STATUS_BUSY); }
};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

