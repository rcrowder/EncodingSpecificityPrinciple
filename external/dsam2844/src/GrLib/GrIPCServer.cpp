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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "ExtIPCUtils.h"
#include "ExtSocket.h"
#include "ExtSocketServer.h"
#include "ExtIPCServer.h"
#include "GrIPCServer.h"
#include "GrSimMgr.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

GrIPCServer::GrIPCServer(const wxString& hostName, uShort theServicePort):
  IPCServer(hostName, theServicePort)
{

}

/****************************** Destructor ************************************/

GrIPCServer::~GrIPCServer(void)
{
	
}

/******************************************************************************/
/****************************** IPCServer Methods *****************************/
/******************************************************************************/

/****************************** LoadSimFile ***********************************/

/*
 * This routine loads the simulation file.  This is a virtual function and
 * classes that call this routine should create their on instance of this
 * this function.
 */

bool
GrIPCServer::LoadSimFile(const wxString& fileName)
{
	wxGetApp().CreateDocument(fileName);
	return(true);

}

/****************************** SetNotification *******************************/

/*
 * Sets up the notification for event handling.
 */

void
GrIPCServer::SetNotification(wxEvtHandler *theHandler)
{
	GetServer()->SetEventHandler(*theHandler, IPC_APP_SERVER_ID);
	GetServer()->SetNotify(wxSOCKET_CONNECTION_FLAG);
	GetServer()->Notify(TRUE);

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/
