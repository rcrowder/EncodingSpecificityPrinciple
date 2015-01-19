/**********************
 *
 * File:		GrIPCServer.h
 * Purpose:		Inter-process communication server extension code module for
 *				The GUI.
 * Comments:	This is a subclass of the IPCServer class (ExtIPCServer code 
 *				module.
 * Author:		L. P. O'Mard
 * Created:		03 Dec 2003
 * Updated:		
 * Copyright:	(c) 2003, 2010 Lowel P. O'Mard
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

#ifndef _GRIPCSERVER_H
#define _GRIPCSERVER_H 1

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/socket.h>
#endif

#include "ExtIPCServer.h"

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


/*************************** GrIPCServer **************************************/

class GrIPCServer: public IPCServer {

  public:
	GrIPCServer(const wxString& hostName, uShort theServicePort);
	~GrIPCServer(void);

	bool	LoadSimFile(const wxString& fileName);
	void	SetNotification(wxEvtHandler *theHandler);

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

