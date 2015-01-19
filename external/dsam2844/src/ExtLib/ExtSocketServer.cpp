/**********************
 *
 * File:		ExtSocketServer.cpp
 * Purpose:		This class enables the use with the inetd/xinetd superserver.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		25 Feb 2004
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <wx/socket.h>
#include <wx/log.h>

#include "ExtSocketServer.h"
#include "ExtSocket.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

SocketServer::SocketServer(wxSockAddress& addr, bool useSuperServer,
  wxSocketFlags flags): wxSocketServer(addr, flags)
{
	usingSuperServer = useSuperServer;

}

/****************************** Accept ****************************************/

SocketBase *
SocketServer::Accept(bool wait)
{
	static const char *funcName = "SocketServer::Accept";

	if (!usingSuperServer) {
		SocketBase *socket = new SocketBase();
		if (!wxSocketServer::AcceptWith(*socket, wait)) {
			wxLogError(wxT("%s: Could not accept connection."), funcName);
			return(NULL);
		}
		return(socket);
	}
	return(new SSSocket());

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

