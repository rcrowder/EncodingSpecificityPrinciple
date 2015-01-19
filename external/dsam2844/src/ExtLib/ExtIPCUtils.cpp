/**********************
 *
 * File:		ExtIPCUtils.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include <ExtCommon.h>

#include <wx/socket.h>
#include <wx/filename.h>
#include "wx/datstrm.h"
#include <wx/wfstream.h>
#include <wx/protocol/protocol.h>
#include <wx/tokenzr.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiSndFile.h"
#include "FiDataFile.h"
#include "ExtIPCUtils.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

IPCUtils::IPCUtils(void)
{
	inVIOPtr = NULL;
	outProcess = NULL;
	inProcess = NULL;
	inProcessCustomer = NULL;
	outProcessSupplier = NULL;
	inProcessConnectedFlag = false;
	outProcessConnectedFlag = false;

}

/****************************** Destructor ************************************/

IPCUtils::~IPCUtils(void)
{
	ResetOutProcess();
	ResetInProcess();
	if (inVIOPtr)
		FreeVirtualIOMemory_SndFile(&inVIOPtr);
}

/****************************** CommandList ***********************************/

NameSpecifier *
IPCUtils::CommandList(int index)
{
	static NameSpecifier	modeList[] = {

					{ (WChar *) wxT("QUIT"),	IPC_COMMAND_QUIT},
					{ (WChar *) wxT("ERRMSGS"),	IPC_COMMAND_ERRMSGS},
					{ (WChar *) wxT("GET"),		IPC_COMMAND_GET},
					{ (WChar *) wxT("GETFILES"),IPC_COMMAND_GETFILES},
					{ (WChar *) wxT("GETPAR"),	IPC_COMMAND_GETPAR},
					{ (WChar *) wxT("INIT"),	IPC_COMMAND_INIT},
					{ (WChar *) wxT("PUT"),		IPC_COMMAND_PUT},
					{ (WChar *) wxT("PUT_ARGS"),IPC_COMMAND_PUT_ARGS},
					{ (WChar *) wxT("RUN"),		IPC_COMMAND_RUN},
					{ (WChar *) wxT("SET"),		IPC_COMMAND_SET},
					{ (WChar *) wxT("STATUS"),	IPC_COMMAND_STATUS},
					{ NULL,						IPC_COMMAND_NULL},
				
				};
	return (&modeList[index]);

}

/****************************** InitOutProcess ********************************/

/*
 */

bool
IPCUtils::InitOutProcess(void)
{
	static const wxChar *funcName = wxT("IPCServer::InitOutProcess");

	if (!outProcess && ((outProcess = Init_EarObject(wxT("DataFile_Out"))) ==
	  NULL)) {
		NotifyError(wxT("%s: Cannot initialised input process EarObject."), 
		  funcName);
		return(false);
	}
	SetPar_ModuleMgr(outProcess, wxT("filename"), EXTIPCUTILS_MEMORY_FILE_NAME);
	return(true);

}

/****************************** InitInProcess *********************************/

/*
 */

bool
IPCUtils::InitInProcess(void)
{
	static const wxChar *funcName = wxT("IPCUtils::InitInProcess");

	if (!inProcess && ((inProcess = Init_EarObject(wxT("DataFile_In"))) ==
	  NULL)) {
		NotifyError(wxT("%s: Cannot initialised input process EarObject."), 
		  funcName);
		return(false);
	}
	SetPar_ModuleMgr(inProcess, wxT("filename"), EXTIPCUTILS_MEMORY_FILE_NAME);
	SND_FILE_VIO_PTR(inProcess) = inVIOPtr;
	return(true);

}

/****************************** ConnectToOutProcess ***************************/

/*
 */

void
IPCUtils::ConnectToOutProcess(EarObjectPtr supplierProcess)
{
	outProcessSupplier = supplierProcess;
	ConnectOutSignalToIn_EarObject(outProcessSupplier, outProcess);
	outProcessConnectedFlag = true;

}

/****************************** ConnectToInProcess ****************************/

/*
 */

void
IPCUtils::ConnectToInProcess(EarObjectPtr customerProcess)
{
	inProcessCustomer = customerProcess;
	ConnectOutSignalToIn_EarObject(inProcess, inProcessCustomer);
	inProcessConnectedFlag = true;

}

/****************************** DisconnectOutProcess **************************/

/*
 */

void
IPCUtils::DisconnectOutProcess(void)
{
	if (!outProcessConnectedFlag)
		return;
	DisconnectOutSignalFromIn_EarObject(outProcessSupplier, outProcess);
	outProcessConnectedFlag = false;

}

/****************************** DisconnectInProcess ***************************/

/*
 */

void
IPCUtils::DisconnectInProcess(void)
{
	if (!inProcessConnectedFlag)
		return;
	DisconnectOutSignalFromIn_EarObject(inProcess, inProcessCustomer);
	inProcessConnectedFlag = false;

}

/****************************** ResetOutProcess *******************************/

/*
 * This routine resets the output process ready for the new simulation.
 */

void
IPCUtils::ResetOutProcess(void)
{
	if (!outProcess)
		return;
	DisconnectOutProcess();
	Free_EarObject(&outProcess);

}

/****************************** ResetInProcess ********************************/

/*
 * This routine resets the input process ready for the new simulation.
 */

void
IPCUtils::ResetInProcess(void)
{
	if (!inProcess)
		return;
	DisconnectInProcess();
	Free_EarObject(&inProcess);

}

/****************************** RunOutProcess ********************************/

/*
 * This function writes the simulation output to the 'memory file'.
 * This call is set up to ignore segment processing mode, as the memory
 * allocated for the output data does not grow.  Such growth would be in
 * appropriate for socket transfer.  The other side of the socket must sort this
 * out as required.
 */

bool
IPCUtils::RunOutProcess(void)
{
	static const wxChar *funcName = wxT("IPCUtils::RunOutProcess");
	bool	ok = true;
	BOOLN	oldUsingGUIFlag = GetDSAMPtr_Common()->usingGUIFlag;
	BOOLN	oldSegmentedMode = GetDSAMPtr_Common()->segmentedMode;

	GetDSAMPtr_Common()->segmentedMode = FALSE;
	GetDSAMPtr_Common()->usingGUIFlag = FALSE;
	if (!RunProcess_ModuleMgr(outProcess)) {
		NotifyError(wxT("%s: Could not run output process."), funcName);
		ok = false;
	}
	GetDSAMPtr_Common()->segmentedMode = oldSegmentedMode;
	GetDSAMPtr_Common()->usingGUIFlag = oldUsingGUIFlag;
	return(ok);

}

/****************************** InitInputMemory *******************************/

/*
 */

bool
IPCUtils::InitInputMemory(sf_count_t length)
{
	static const wxChar *funcName = wxT("IPCUtils::InitInputMemory");

	if (!InitVirtualIOMemory_SndFile(&inVIOPtr, length)) {
		NotifyError(wxT("%s: Could not initialise memory for input process ")
		  wxT("signal"), funcName);
		return(false);
	}
	return(true);

}
/****************************** RunInProcess **********************************/

/*
 * This function runs the input process.  It assumes that the DataFile
 * process is GUI safe, and so temporarily unsets the 'usingGUIFlag'.
 * This avoids an attempted call to the "TestDestroy_SimThread" routine.
 */

bool
IPCUtils::RunInProcess(void)
{
	static const wxChar *funcName = wxT("IPCUtils::RunInProcess");
	bool	ok = true;
	BOOLN	oldUsingGUIFlag = GetDSAMPtr_Common()->usingGUIFlag;

	if (!inProcess)
		return(false);
	ResetProcess_EarObject(inProcess);
	GetDSAMPtr_Common()->usingGUIFlag = FALSE;
	if (!RunProcess_ModuleMgr(inProcess)) {
		NotifyError(wxT("%s: Could not run input process."), funcName);
		ok = false;
	}
	GetDSAMPtr_Common()->usingGUIFlag = oldUsingGUIFlag;
	return(ok);

}

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

