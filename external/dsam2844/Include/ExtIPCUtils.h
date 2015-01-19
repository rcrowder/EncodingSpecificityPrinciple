/**********************
 *
 * File:		ExtIPCUtils.h
 * Purpose:		Inter-process communication server extension utility code
 *				module.
 * Comments:	This code module was revised from the ExtIPCServer.cpp code
 *				module.
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

#ifndef _EXTIPCUTILS_H
#define _EXTIPCUTILS_H 1

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/socket.h>
#endif

#include "UtNameSpecs.h"
#include "FiSndFile.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define EXTIPCUTILS_MAX_COMMAND_CHAR		10
#define EXTIPCUTILS_MEMORY_FILE_NAME		wxT("+.aif")
#define EXTIPCUTILS_DEFAULT_SERVER_PORT		3300


#ifndef HEADER_ONLY /* This allows for the simple extraction of the defines. */

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	IPC_STATUS_READY = 0,
	IPC_STATUS_BUSY,
	IPC_STATUS_ERROR

};

enum {

	IPC_APP_SOCKET_ID = 1,
	IPC_APP_SERVER_ID

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IPC_COMMAND_QUIT,

	IPC_COMMAND_ERRMSGS,
	IPC_COMMAND_GET,
	IPC_COMMAND_GETFILES,
	IPC_COMMAND_GETPAR,
	IPC_COMMAND_INIT,
	IPC_COMMAND_PUT,
	IPC_COMMAND_PUT_ARGS,
	IPC_COMMAND_RUN,
	IPC_COMMAND_SET,
	IPC_COMMAND_STATUS,

	IPC_COMMAND_NULL

} IPCCommandSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

/*************************** IPCUtils *****************************************/

class DSAMEXT_API IPCUtils {

	bool	inProcessConnectedFlag, outProcessConnectedFlag;
	EarObjectPtr	outProcess, inProcess;
	EarObjectPtr	inProcessCustomer, outProcessSupplier;
#	if HAVE_SNDFILE
	DFVirtualIOPtr	inVIOPtr;
#	endif

  public:

	IPCUtils(void);
	~IPCUtils(void);

	NameSpecifier *	CommandList(int index);
	void	ConnectToOutProcess(EarObjectPtr supplierProcess);
	void	ConnectToInProcess(EarObjectPtr customerProcess);
	void	DisconnectInProcess(void);
	void	DisconnectOutProcess(void);
	EarObjectPtr	GetInProcess(void)	{ return inProcess; }
	bool	InProcessConnected(void)	{ return inProcessConnectedFlag; }
	EarObjectPtr	GetOutProcess(void)	{ return outProcess; }
	bool	InitInProcess(void);
	bool	InitOutProcess(void);
	void	ResetInProcess(void);
	void	ResetOutProcess(void);
	bool	RunOutProcess(void);
	bool	RunInProcess(void);

#	if HAVE_SNDFILE
	DFVirtualIOPtr	GetInVIOPtr(void)	{ return inVIOPtr; }
	bool	InitInputMemory(sf_count_t length);
#	endif

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

NameSpecifier *	CommandList_IPCUtils(int index);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif /* HEADER_ONLY */
#endif

