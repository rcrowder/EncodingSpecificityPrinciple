/**********************
 *
 * File:		ExtIPCServer.cpp
 * Purpose:		Inter-process communication server extension code module.
 * Comments:	This code module was revised from the GrSimMgr.cpp code module.
 *				Communication is always done in multi-byte format.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 2003
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
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "UtString.h"
#include "FiDataFile.h"
#include "UtSSParser.h"
#include "ExtMainApp.h"
#include "ExtIPCUtils.h"
#include "ExtSocket.h"
#include "ExtSocketServer.h"
#include "ExtIPCServer.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IPCServer	*iPCServer = NULL;

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * The superServerFlag argument indicates that the server is to be started by
 * inetd/xinetd.
 */

IPCServer::IPCServer(const wxString& hostName, uShort theServicePort,
  bool superServerFlag)
{
	wxIPV4address	addr;

	iPCServer = this;
	ok = true;
	simulationInitialisedFlag = false;
	successfulRunFlag = false;
	sock = NULL;

	if (!hostName.IsEmpty())
		addr.Hostname(hostName);
	addr.Service((!theServicePort)? EXTIPCUTILS_DEFAULT_SERVER_PORT:
	  theServicePort);
	myServer = new SocketServer(addr, superServerFlag, wxSOCKET_NONE);
	if (!myServer->Ok()) {
		ok = FALSE;
		return;
	}
	SetNotifyFunc(Notify_IPCServer);

}

/****************************** Destructor ************************************/

/*
 * The EarObjects are free'd by the main controlling routine.
 */

IPCServer::~IPCServer(void)
{
	if (myServer)
		myServer->Destroy();
	iPCServer = NULL;
	
}

/******************************************************************************/
/****************************** IPCServer Methods *****************************/
/******************************************************************************/

/****************************** InitConnection ********************************/

/*
 * This function initialised the connection and returns a pointer to the
 * socket.
 */

SocketBase *
IPCServer::InitConnection(bool wait)
{
	static const wxChar *funcName = wxT("IPCServer::OnServerEvent");
	wxString	salutation;
	wxIPV4address	addr;

	sock = myServer->Accept(wait);
	if (!sock) {
		NotifyError(wxT("%s: Couldn't accept a new connection.\n"), funcName);
		return(NULL);
	}
	sock->SetFlags(wxSOCKET_WAITALL);
	myServer->GetLocal(addr);
	salutation.Printf(wxT("Host %s running %s, DSAM version %s.\n"), addr.
	  Hostname().c_str(), EXTIPC_DEFAULT_SERVER_NAME, DSAM_VERSION);
	sock->Write(salutation.mb_str(), (wxUint32) salutation.length());
	return(sock);


}

/****************************** OnInit ****************************************/

/*
 */

void
IPCServer::OnInit(void)
{
	static const wxChar *funcName = wxT("IPCServer::OnInit");
	unsigned char c;
	bool	simLoaded;

	if (!GetPtr_AppInterface()->simulationFinishedFlag) {
		return;
	}
	iPCUtils.ResetInProcess();
	iPCUtils.ResetOutProcess();
	wxString tempFileName = wxFileName::CreateTempFileName(wxT("simFile"));
	wxRemoveFile(tempFileName);
	tempFileName.append(wxT(".spf"));
	wxFFileOutputStream	*oStream = new wxFFileOutputStream(tempFileName);
	if (!oStream->Ok()) {
		NotifyError(wxT("%s: Could not open temporary file '%s'."), funcName,
		  tempFileName.c_str());
		return;
	}
	while (!sock->Read(&c, 1).Error() && (c != (unsigned char) EOF))
		oStream->PutC(c);
	delete oStream;
	simLoaded = LoadSimFile(tempFileName);
	wxRemoveFile(tempFileName);
	if (!simLoaded) {
		NotifyError(wxT("%s: Could not load simulation."), funcName);
		return;
	}
	iPCUtils.InitOutProcess();
	iPCUtils.ConnectToOutProcess(GetSimProcess_AppInterface());
	simulationInitialisedFlag = true;

}

/****************************** OnPut *****************************************/

/*
 * Reads signal from client.
 */

void
IPCServer::OnPut(void)
{
	static const wxChar *funcName = wxT("IPCServer::OnPut");
	unsigned char	*p;
	sf_count_t	i, length;

 	sock->Read(&length, sizeof(length));
	if (!iPCUtils.InitInputMemory(length)) {
		NotifyError(wxT("%s: Could not initialise memory for input process ")
		  wxT("signal"), funcName);
		return;
	}
	p = iPCUtils.GetInVIOPtr()->data;
	for (i = 0; i < length; i++)
		sock->Read(p++, 1); // Use buffering and reads longer than 1 char here.
	iPCUtils.GetInVIOPtr()->length = length;
	if (!iPCUtils.InitInProcess()) {
		NotifyError(wxT("%s: Could not initialise the input process."),
		  funcName);
		return;
	}

}

/****************************** OnPutArgs *************************************/

/*
 */

void
IPCServer::OnPutArgs(void)
{
	bool	ok = true;
	unsigned char	c;
	wxUint8	i, argc;
	wxString	s;

 	sock->Read(&argc, sizeof(argc));
	ok = dSAMMainApp->InitArgv(argc);
	for (i = 0; i < argc; i++) {
		s.Clear();
		while (!sock->Read(&c, 1).Error() && (c != '\0'))
			s += c;
		s += wxT('\0');
		dSAMMainApp->SetArgvString(i, (wxChar *) s.c_str(), (wxUint32) s.length());
	}
	ResetCommandArgFlags_AppInterface();

}

/****************************** OnGet *****************************************/

/*
 */

void
IPCServer::OnGet(void)
{
	sf_count_t	length = 0;
	DFVirtualIOPtr	vIOPtr;

	if (!GetPtr_AppInterface()->simulationFinishedFlag || !_OutSig_EarObject(
	  GetSimProcess_AppInterface())) {
		sock->Write(&length, sizeof(length));
		return;
	}
	iPCUtils.RunOutProcess();
	vIOPtr = SND_FILE_VIO_PTR(iPCUtils.GetOutProcess());
	length = vIOPtr->length;
 	sock->Write(&length, sizeof(length));
	sock->Write(vIOPtr->data, vIOPtr->length);

}

/****************************** BuildFileList *********************************/

/*
 */

void
IPCServer::BuildFileList(wxArrayString &list, DatumPtr pc)
{
	while (pc) {
		if (pc->type == PROCESS)
			switch (pc->data->module->specifier) {
			case SIMSCRIPT_MODULE:
				BuildFileList(list, pc);
				break;
			case DATAFILE_MODULE:
				if (!pc->onFlag || ((DataFilePtr) pc->data->module->parsPtr)->
				  inputMode)
					break;
				list.Add((wxChar *) GetUniParPtr_ModuleMgr(pc->data,
				  (WChar *) wxT("fileName"))->valuePtr.s);
				break;
			default:
				;
			} /* Switch */
		pc = pc->next;
	}
}

/****************************** OnGetFiles ************************************/

/*
 */

void
IPCServer::OnGetFiles(void)
{
	static const wxChar *funcName = wxT("IPCServer::OnGetFiles");
	wxUint8	byte, numFiles = 0;
	sf_count_t	i, j, length;
	wxArrayString	list;
	wxString	nameOnly;
	wxFileName	fileName;

	if (!GetPtr_AppInterface()->simulationFinishedFlag || !successfulRunFlag) {
 		sock->Write(&numFiles, sizeof(numFiles));
		return;
	}
	BuildFileList(list, GetSimulation_AppInterface());
	numFiles = (wxUint32) list.Count();
	sock->Write(&numFiles, 1);
	for (i = 0; i < numFiles; i++) {
		fileName = (wxChar *) GetParsFileFPath_Common((wxChar *) list[i].c_str(
		  ));
		nameOnly = fileName.GetFullName();
		sock->Write(nameOnly.mb_str(), (wxUint32) nameOnly.length());
		sock->Write(wxT("\n"), 1);
		wxFFileInputStream inStream(fileName.GetFullPath());
		if (!inStream.Ok()) {
			NotifyError(wxT("%s: Could not open '%s' for transfer."), funcName,
			  list[i].c_str());
			return;
		}
		wxDataInputStream	data(inStream);

		length = (sf_count_t) inStream.GetSize();
		sock->Write(&length, sizeof(length));
		for (j = 0; j < length; j++) {
			byte = data.Read8();
			sock->Write(&byte, 1);
		}
		if (!wxRemoveFile(fileName.GetFullPath()))
			NotifyError(wxT("%s: Could not remove file '%s' from server."),
			  funcName, fileName.GetFullPath().c_str());
	}

}

/****************************** OnGetPar **************************************/

/*
 */

void
IPCServer::OnGetPar(void)
{
	static const wxChar *funcName = wxT("IPCServer::OnGetPar");
	unsigned char c;
	wxString	parName, value;
	UniParPtr	par;

	while (!sock->Read(&c, 1).Error() && (c != '\n'))
		if (c != '\r')
			parName += c;
	if ((par = GetUniParPtr_ModuleMgr(GetSimProcess_AppInterface(), (wxChar *)
	  parName.c_str())) == NULL) {
		NotifyError(wxT("%s: Could not find '%s' parameter."), funcName,
		  parName.c_str());
		sock->Write("", 1);
		return;
	}
	value = GetParString_UniParMgr(par);
	sock->Write(value.mb_str(), (wxUint32) value.length());
	sock->Write(wxT("\n"), 1);

}

/****************************** OnSet *****************************************/

/*
 */

void
IPCServer::OnSet(void)
{
	static const wxChar *funcName = wxT("IPCServer::SetParameters");
	size_t	i, numTokens;
	unsigned char c;
	wxString	cmdStr, parameter, value;

	while (!sock->Read(&c, 1).Error() && (c != '\n'))
		if (c != '\r')
			cmdStr += c;
	wxStringTokenizer tokenizer(cmdStr);
	if ((numTokens = tokenizer.CountTokens()) % 2 != 0) {
		NotifyError(wxT("%s: parameter settings must be in <name> <value> ")
		  wxT("pairs."), funcName);
		return;
	}
	for (i = 0; i < numTokens / 2; i++) {
		parameter = tokenizer.GetNextToken().c_str();
		value = tokenizer.GetNextToken().c_str();
		if (!SetProgramParValue_AppInterface((wxChar *) parameter.c_str(),
		  (wxChar *) value.c_str(), FALSE) && !SetUniParValue_Utility_Datum(
		  GetSimulation_AppInterface(), (wxChar *) parameter.c_str(),
		  (wxChar *) value.c_str())) {
			NotifyError(wxT("%s: Could not set '%s' parameter to '%s'."),
			  funcName, (wxChar *)  parameter.c_str(), (wxChar *) value.
			  c_str());
			return;
		}
	}

}

/****************************** OnExecute *************************************/

/*
 */

void
IPCServer::OnExecute(void)
{
	static const wxChar *funcName = wxT("IPCServer::OnExecute");

	if (!GetPtr_AppInterface()->simulationFinishedFlag) {
		return;
	}
	if (!simulationInitialisedFlag) {
		NotifyError(wxT("%s: Simulation not initialised."), funcName);
		return;
	}
	if (iPCUtils.GetInProcess() && !iPCUtils.InProcessConnected())
		iPCUtils.ConnectToInProcess(GetSimProcess_AppInterface());

	iPCUtils.RunInProcess();
	successfulRunFlag = CXX_BOOL(OnExecute_AppInterface());

}

/****************************** OnStatus **************************************/

/*
 */

void
IPCServer::OnStatus(void)
{
	unsigned char status = IPC_STATUS_READY;


	if (!GetPtr_AppInterface()->simulationFinishedFlag) {
		status = IPC_STATUS_BUSY;
		sock->Write(&status, 1);
		return;
	}
	if (notificationList.GetCount() > 0) {
		status = IPC_STATUS_ERROR;
		sock->Write(&status, 1);
		return;
	}
	sock->Write(&status, 1);
	
}

/****************************** ProcessInput **********************************/

/*
 * This routine processes the socket input.
 * The call to 'wxFileName::CreateTempFileName' creates the temporary file:
 * I could find no way to stop this, so I have to remove the file before adding
 * the extension and re-creating it.
 */

bool
IPCServer::ProcessInput(void)
{
	static const wxChar *funcName = wxT("IPCServer::ProcessInput");
	bool endProcessing;
	unsigned char c;

	sock->SetNotify(wxSOCKET_LOST_FLAG);
	buffer.Clear();
	while (((endProcessing = sock->Read(&c, 1).Error()) == false) && (c !=
	  '\n'))
		if (c != '\r')
			buffer += c;
	
	switch (Identify_NameSpecifier((wxChar *) buffer.c_str(), iPCUtils.
	  CommandList(0))) {
	case IPC_COMMAND_QUIT:
		endProcessing = true;
		OnExit_AppInterface();
		break;
	case IPC_COMMAND_ERRMSGS:
		OnErrMsgs();
		notificationList.Clear();
		break;
	case IPC_COMMAND_GET:
		OnGet();
		break;
	case IPC_COMMAND_GETFILES:
		OnGetFiles();
		break;
	case IPC_COMMAND_GETPAR:
		OnGetPar();
		break;
	case IPC_COMMAND_INIT:
		OnInit();
		break;
	case IPC_COMMAND_PUT:
		OnPut();
		break; 
	case IPC_COMMAND_PUT_ARGS:
		OnPutArgs();
		break; 
	case IPC_COMMAND_RUN:
		OnExecute();
		break;
	case IPC_COMMAND_SET:
		OnSet();
		break;
	case IPC_COMMAND_STATUS:
		OnStatus();
		break;
	default:
		if (buffer.Length())
			NotifyError(wxT("%s: Unknown command given (%s)."), funcName,
			  buffer.c_str());
	}
	sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
	return(!endProcessing);

}

/****************************** OnErrMsgs *************************************/

/*
 * This routine prints the notification list.  If the index is given then
 * only the specified notication is printed.  A negative index assumes that
 * all notifications should be printed.
 */

void
IPCServer::OnErrMsgs(int index)
{
	wxUint8 numNotifications;

	if (!notificationList.GetCount() || ((index >= 0) && ((notificationList.
	  GetCount() < (size_t) index)))) {
		numNotifications = 0;
		sock->Write(&numNotifications, 1);
		return;
	}
	if (index < 0) {
		numNotifications = (wxUint32) notificationList.GetCount();
		sock->Write(&numNotifications, 1);
		for (unsigned int i = 0; i < notificationList.GetCount(); i++)
			sock->Write(notificationList[i].mb_str(), (wxUint32) notificationList[i].
			  length());
	} else {
		numNotifications = 1;
		sock->Write(&numNotifications, 1);
		sock->Write(notificationList[index].mb_str(), (wxUint32) notificationList[index].
		  length());
	}
		
}

/****************************** LoadSimFile ***********************************/

/*
 * This routine loads the simulation file.  This is a virtual function and
 * classes that call this routine should create their on instance of this
 * this function.
 */

bool
IPCServer::LoadSimFile(const wxString& fileName)
{
	FreeSim_AppInterface();
	if (!SetParValue_UniParMgr(&GetPtr_AppInterface()->parList,
	  APP_INT_SIMULATIONFILE, (wxChar *) fileName.c_str()))
		return(false);
	return(dSAMMainApp->ResetSimulation());

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

/*************************** GetPtr_IPCServer *********************************/

/*
 */

IPCServer *
GetPtr_IPCServer(void)
{
	static const wxChar *funcName = wxT("GetPtr_IPCServer");

	if (!iPCServer) {
		NotifyError(wxT("%s: Server not initialised.\n"), funcName);
		return(NULL);
	}
	return(iPCServer);

}

/***************************** Notify *****************************************/

/*
 * All notification messages are stored in the notification list.
 * This list is reset when the noficiationCount is zero.
 */

void
Notify_IPCServer(const wxChar *message, CommonDiagSpecifier type)
{
	wxString	mesg;

	if (!GetDSAMPtr_Common()->notificationCount)
		GetPtr_IPCServer()->ClearNotifications();
	mesg.Printf(wxT("%s: %s\n"), DiagnosticTitle(type), message);
	GetPtr_IPCServer()->AddNotification(mesg);

} /* NotifyMessage */

