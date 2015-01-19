/**********************
 *
 * File:		PaWorker1.c
 * Purpose:		This program runs an auditory model using MPI.
 * Comments:	This is a complete program whose executable will be loaded
 * 				using the MPI loader.
 *		To Do		Implement SetUniParValue_Utility_SimScript(...) use.
 * Author:		L. P. O'Mard
 * Created:		28 Nov 1995
 * Updated:		07 Mar 1997
 * Copyright:	(c) 1997, 2010 Lowel P. O'Mard
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "DSAM.h")
#include "PaWorker1.h"
#include "UtSSParser.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	SIMULATION_SPEC_MODULE_NAME	wxT("Utility_SimScript")

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

static WChar	charBuffer[BUFSIZ], hostName[MPI_MAX_PROCESSOR_NAME];
WChar	simScriptParFile[MAX_FILE_PATH], workerName[MAXLINE];
int		masterRank, myRank, diagnosticsMode, channelMode;
int		workCount = 0;

CFListPtr	theCFs;
Work		work[PA_WORKER1_MAX_SIMULATIONS];
EarObjectPtr	preProcess[PA_WORKER1_MAX_SIMULATIONS];

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** GetParsFromMaster *****************************/

/*
 * This program reads a specified number of parameters from a file.
 * It expects there to be one parameter per line.
 */
 
BOOLN
GetParsFromMaster(WorkPtr workPtr)
{
	static const WChar *funcName = wxT("GetParsFromMaster");
	BOOLN	ok = TRUE, response;
	int		i, numChannels;
	double	dummy, *frequencies, *bandwidths;
	MPI_Status	status;

	MPI_Recv(simScriptParFile, MAXLINE, MPI_INT, masterRank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD, &status);
	MPI_Recv(&numChannels, 1, MPI_INT, masterRank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD, &status);
	MPI_Recv(&diagnosticsMode, 1, MPI_INT, masterRank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD, &status);
	MPI_Recv(&channelMode, 1, MPI_INT, masterRank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD, &status);
	if (channelMode == WORKER_CHANNEL_SET_TO_BM_MODE) {
		if ((frequencies = (double *) calloc(numChannels, sizeof(double))) ==
		  NULL) {
			NotifyError(wxT("%s: %s: Out of memory for %d frequencies."),
			  workerName, funcName, numChannels);
			for (i = 0; i < numChannels; i++)
				MPI_Recv(&dummy, 1, MPI_DOUBLE, masterRank, MASTER_MESG_TAG,
				  MPI_COMM_WORLD, &status);
			return(FALSE);
		}
		MPI_Recv(frequencies, numChannels, MPI_DOUBLE, masterRank,
		  MASTER_MESG_TAG, MPI_COMM_WORLD, &status);
		if ((bandwidths = (double *) calloc(numChannels, sizeof(double))) ==
		  NULL) {
			NotifyError(wxT("%s: %s: Out of memory for %d bandwidths."),
			  workerName, funcName, numChannels);
			for (i = 0; i < numChannels; i++)
				MPI_Recv(&dummy, 1, MPI_DOUBLE, masterRank, MASTER_MESG_TAG,
				  MPI_COMM_WORLD, &status);
			return(FALSE);
		}
		MPI_Recv(bandwidths, numChannels, MPI_DOUBLE, masterRank,
		  MASTER_MESG_TAG, MPI_COMM_WORLD, &status);
		if ((theCFs = GenerateUser_CFList(numChannels, frequencies)) == NULL) {
			NotifyError(wxT("%s: %s: Out of memory for CFList"), workerName,
			  funcName);
			free(frequencies);
			return(FALSE);
		}
		SetBandwidths_CFList(theCFs, wxT("USER"), bandwidths);
	}
	MPI_Probe(masterRank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	if (status.MPI_TAG != MASTER_INIT_TAG)
		MPI_Recv(&response, 1, MPI_INT, masterRank, MASTER_MESG_TAG,
		  MPI_COMM_WORLD, &status);
	else {
		Free_EarObject(&workPtr->preProcessData);
		if ((workPtr->preProcessData = Init_EarObject(wxT("null"))) == NULL)
			ok = FALSE;
		if (!ok || !SetOutSignal_MPI_General(workPtr->preProcessData,
		  ReceiveSignalPars_MPI_General(masterRank, MASTER_INIT_TAG))) {
			NotifyError(wxT("%s: %s: Could not initialise pre-process data."),
			  workerName, funcName);
			ok = FALSE;
		}
	}
	return(ok);

}

/****************************** GetWorkPtr ************************************/

/*
 * This routine finds a pointer to a work structure which corresponds to the
 * specified handle.
 */

WorkPtr
GetWorkPtr(EarObjHandle handle)
{
	static WChar *funcName = wxT("GetWorkPtr");
	int		i;

	for (i = 0; i < workCount; i++)
		if (work[i].handle == handle)
			return(&work[i]);
	NotifyError(wxT("%s: %s: Could not find work with handle '%u'."),
	  workerName, funcName, handle);
	 return(NULL);

}

/****************************** AddWork ***************************************/

/*
 * This routine adds work to the list of simulations running.
 * It increments the global workCount variable.
 * It first checks to ensure that the work specification does not already
 * exist for the handle.
 * It returns NULL if it is succesful, otherwise it returns a pointer to the
 * work.
 */

WorkPtr
AddWork(EarObjHandle handle)
{
	static WChar *funcName = wxT("AddWork");
	int		i;
	WorkPtr	workPtr;
	

	for (i = 0; i < workCount; i++)
		if (work[i].handle == handle)
			return(&work[i]);
	
	if (workCount == PA_WORKER1_MAX_SIMULATIONS) {
		NotifyError(wxT("%s: %s: Attempt to exceed maximum simulations (%d)."),
		  workerName, funcName, PA_WORKER1_MAX_SIMULATIONS);
		return(NULL);
	}
	workPtr = &work[workCount++];
	workPtr->handle = handle;
	workPtr->chanCount = 0;
	workPtr->sampleCount = 0;
	workPtr->preProcessData = NULL;
	workPtr->simulation = NULL;
	return(workPtr);

}

/****************************** ProcessSetTag *********************************/

/*
 * This routine carries out the set tag processing from the master.
 * It processes the 'set' conversation.
 * It allows for the use of the '*' wild card for executing the command on more
 * than one process.
 */

void
ProcessSetTag(void)
{
	static const WChar *funcName = wxT("ProcessSetTag");
	BOOLN	foundLabel;
	WChar	label[MAXLINE];
	int		command, ok;
	long	longArray[PA_WORKER1_SET_COMMAND_MAX_ARRAY];
	double	doubleArray[PA_WORKER1_SET_COMMAND_MAX_ARRAY];
	WorkPtr		workPtr;
	DatumPtr	pc;
	IonChanListPtr iCList;
	EarObjHandle	handle;
	MPI_Status		status;

	ok = TRUE;
	MPI_Recv(&command, 1, MPI_INT, masterRank, MASTER_SET_TAG, MPI_COMM_WORLD,
	  &status);
	MPI_Recv(&handle, 1, MPI_UNSIGNED, masterRank, MASTER_SET_TAG,
	  MPI_COMM_WORLD, &status);
	MPI_Recv(charBuffer, BUFSIZ, MPI_CHAR, masterRank, MASTER_SET_TAG,
	  MPI_COMM_WORLD, &status);
	if (strlen(charBuffer) >= MAXLINE) {
		NotifyError(wxT("Worker[%d]: %s: Label '%s' too long."), myRank,
		  funcName, charBuffer);
		ok = FALSE;
		MPI_Send(&ok, 1, MPI_INT, masterRank, WORKER_SET_TAG, MPI_COMM_WORLD);
		return;
	}
	if ((workPtr = GetWorkPtr(handle)) == NULL) {
		NotifyError(wxT("%s: %s: Could not find work pointer (%d)."),
		  workerName, funcName);
		ok = FALSE;
		MPI_Send(&ok, 1, MPI_INT, masterRank, WORKER_SET_TAG, MPI_COMM_WORLD);
		return;
	}
	DSAM_strcpy(label, charBuffer);
	pc = FindLabelledProcessInst_Utility_Datum(*GetUniParPtr_ModuleMgr(
	  workPtr->simulation, wxT("simulation"))->valuePtr.simScript.simulation,
	  label);
	if ((foundLabel = pc != NULL) == FALSE) {
		NotifyError(wxT("Worker[%d]: %s: Labelled process not initialised, or ")
		  wxT("label '%s' not found."), myRank, funcName, label);
		ok = FALSE;
	}
	switch (command) {
	case PRINT_PARS:
		if (foundLabel)
			do {
				if (!PrintPars_ModuleMgr(pc->data))
					ok = FALSE;
				pc = pc->next;
			} while (ok && ((pc = FindLabelledProcessInst_Utility_Datum(pc,
			  label)) != NULL));
		break;
	case SET_AMPLITUDE:
		MPI_Recv(doubleArray, 1, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetAmplitude, pc->data, doubleArray[0]))
			ok = FALSE;
		break;
	case SET_DATA_FILE_NAME:
		MPI_Recv(charBuffer, BUFSIZ, MPI_CHAR, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetFileName, pc->data, charBuffer))
			ok = FALSE;
		break;
	case SET_DELAY:
		MPI_Recv(doubleArray, 1, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetDelay, pc->data, doubleArray[0]))
			ok = FALSE;
		break;
	case SET_INDIVIDUAL_FREQ:
		MPI_Recv(doubleArray, 2, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun2(SetIndividualFreq, pc->data,
		  (int) doubleArray[0], doubleArray[1]))
			ok = FALSE;
		break;
	case SET_INDIVIDUAL_DEPTH:
		MPI_Recv(doubleArray, 2, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun2(SetIndividualDepth, pc->data,
		  (int) doubleArray[0], doubleArray[1]))
			ok = FALSE;
		break;
	case SET_INTENSITY:
		MPI_Recv(doubleArray, 1, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetIntensity, pc->data, doubleArray[0]))
			ok = FALSE;
		break;
	case SET_FREQUENCY:
		MPI_Recv(doubleArray, 1, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetFrequency, pc->data, doubleArray[0]))
			ok = FALSE;
		break;
	case SET_PERIOD:
		MPI_Recv(doubleArray, 1, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetPeriod, pc->data, doubleArray[0]))
			ok = FALSE;
		break;
	case SET_PULSERATE:
		MPI_Recv(doubleArray, 1, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetPulseRate, pc->data, doubleArray[0]))
			ok = FALSE;
		break;
	case SET_KDECAYTCONST:
		MPI_Recv(doubleArray, 1, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetKDecayTConst, pc->data, doubleArray[0]))
			ok = FALSE;
		break;
	case SET_MEMBRANETCONST:
		MPI_Recv(doubleArray, 1, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetMembraneTConst, pc->data,
		  doubleArray[0]))
			ok = FALSE;
		break;
	case SET_RANSEED:
		MPI_Recv(longArray, 1, MPI_LONG, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || !DoFun1(SetRanSeed, pc->data, longArray[0]))
			ok = FALSE;
		break;
	case SET_MAXCONDUCTANCE:
		MPI_Recv(doubleArray, 2, MPI_DOUBLE, masterRank, MASTER_SET_TAG,
		  MPI_COMM_WORLD, &status);
		if (!foundLabel || ((iCList = DoFun(GetICListPtr, pc->data)) == NULL)) {
			ok = FALSE;
			break;
		} 
		iCList->ionChannels[(int) doubleArray[0]].maxConductance =
		  doubleArray[1];
		break;
	case WRITE_OUT_SIGNAL:
		if (foundLabel)
			do {
	   			sprintf(charBuffer, wxT("worker%d_step%d.dat"), myRank,
	   			  pc->stepNumber);
				if (!WriteOutSignal_DataFile(charBuffer, pc->data))
					ok = FALSE;
				pc = pc->next;
			} while (ok && ((pc = FindLabelledProcessInst_Utility_Datum(pc,
			  label)) != NULL));
		break;
	default:
		NotifyError(wxT("Worker[%d]: %s: Received unknown command (%d)."),
		  myRank, funcName, command);
		ok = FALSE;
		break;
	} /* switch */
	MPI_Send(&ok, 1, MPI_INT, masterRank, WORKER_SET_TAG, MPI_COMM_WORLD);

}

/****************************** ProcessMesgTag ********************************/

/*
 * This routine carries out the message tag processing from the master.
 * The module parameters are only printed just before the first run.
 */

void
ProcessMesgTag(void)
{
	static WChar *funcName = wxT("ProcessMesgTag");
	int		control, ok, tag;
	DatumPtr	bMInst = NULL;
	CFListPtr	oldCFs;
	WorkPtr		workPtr;
	EarObjHandle	handle;
	MPI_Status		status;

	MPI_Recv(&control, 1, MPI_INT, masterRank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD, &status);
	MPI_Recv(&handle, 1, MPI_UNSIGNED, masterRank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD, &status);
	if ((control != INIT_SIMULATION) && ((workPtr = GetWorkPtr(handle)) ==
	  NULL)) {
		NotifyError(wxT("%s: %s: Could not find work pointer (%d)."),
		  workerName, funcName, handle);
		ok = FALSE;
		MPI_Send(&ok, 1, MPI_INT, masterRank, WORKER_MESG_TAG, MPI_COMM_WORLD);
		return;
	}
	ok = TRUE;
	tag = WORKER_MESG_TAG;
	switch (control) {
	case INIT_SIMULATION:
		if ((workPtr = AddWork(handle)) == NULL) {
			NotifyError(wxT("%s: %s: Could not add work."), workerName,
			  funcName);
			ok = FALSE;
			break;
		}
		if (!GetParsFromMaster(workPtr))
			ok = FALSE;
		if ((diagnosticsMode == WORKER_DIAGNOSTICS_OFF) ||
		  (diagnosticsMode == WORKER_DIAGNOSTICS_FILE)) {
			sprintf(charBuffer, wxT("worker%d.warnings"), myRank);
			SetWarningsFile(charBuffer, OVERWRITE);
		}
		if (diagnosticsMode == WORKER_DIAGNOSTICS_FILE) {
			sprintf(charBuffer, wxT("worker%d.params"), myRank);
			SetParsFile(charBuffer, OVERWRITE);
		}
		if (ok && (workPtr->simulation == NULL) && ((workPtr->simulation =
		  Init_EarObject(SIMULATION_SPEC_MODULE_NAME)) == NULL))
			ok = FALSE;
		if (!ok || !ReadPars_ModuleMgr(workPtr->simulation, simScriptParFile))
			ok = FALSE;
		else {
			SET_PARS_POINTER(simulation);
			PrintPars_Utility_SimScript();
			switch (channelMode) {
			case WORKER_CHANNEL_SET_TO_BM_MODE:
				if (((bMInst = FindModuleProcessInst_Utility_Datum(
				  *GetUniParPtr_ModuleMgr(workPtr->simulation, wxT(
				  "simulation"))->valuePtr.simScript.simulation, wxT("BM_"))) !=
				  NULL) && ((oldCFs = DoFun(GetCFListPtr, bMInst->data)) !=
				  NULL)) {
					Free_CFList(&oldCFs);
					DoFun1(SetCFList, bMInst->data, theCFs);
				}
				break;
			default:
				break;
			} /* switch */
			if (workPtr->preProcessData)
				  workPtr->simulation->inSignal[0] =
				    workPtr->preProcessData->outSignalPtr;
			workPtr->simulation->updateProcessFlag = FALSE;
		}
		break;
	case RUN_SIMULATION:
		ok = DoProcess(workPtr->simulation);
		break;
	case REQUEST_SIGNAL_PARS:
		if (!SendSignalPars_MPI_General(workPtr->simulation->outSignal,
		  workPtr->simulation->outSignal->numChannels, masterRank,
		  WORKER_INIT_TAG))
			ok = FALSE;
		tag = WORKER_INIT_TAG;
		break;
	case REQUEST_CHANNEL_DATA:
		SendChannelData_MPI_General(workPtr->simulation->outSignal, 0,
		  workPtr->simulation->outSignal->numChannels, masterRank,
		  WORKER_DATA_TAG, WORKER_END_CHANNEL_TAG);
		tag = WORKER_END_SIGNAL_TAG;
		break;
	default:
		NotifyError(wxT("Worker[%d]: Received unknown control (%d)."),
		  myRank, control);
		ok = FALSE;
		break;
	} /* switch */
	MPI_Send(&ok, 1, MPI_INT, masterRank, tag, MPI_COMM_WORLD);

}

/****************************** ProcessInitTag ********************************/

/*
 * This routine carries out the init tag processing from the master.
 * It initialises a particular simulation, based on the EarObject handle
 * sent by the calling master instance.
 */

void
ProcessInitTag(void)
{
	static const WChar *funcName = wxT("ProcessInitTag");
	BOOLN			ok = TRUE, finished = FALSE, response;
	unsigned int	handle;
	int				packageLength;
	ChanData		*outPtr;
	WorkPtr			workPtr;
	EarObjectPtr	data;
	MPI_Status		status;
	MPI_Request		request;

	MPI_Recv(&handle, 1, MPI_UNSIGNED, masterRank, MASTER_INIT_TAG,
	  MPI_COMM_WORLD, &status);
	workPtr = GetWorkPtr(handle);
	data = workPtr->preProcessData;
	outPtr = _OutSig_EarObject(data)->channel[workPtr->chanCount++];
	while (!finished) {
		MPI_Probe(masterRank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		switch (status.MPI_TAG) {
		case MASTER_DATA_TAG:
			MPI_Get_count(&status, MPI_DOUBLE, &packageLength);
			if (_OutSig_EarObject(data)->length < workPtr->sampleCount +
			  packageLength) {
				NotifyError(wxT("%s: %s: Channel length is longer than the ")
				  wxT("initialised length for this pre-process data."),
				  workerName, funcName);
				ok = FALSE;
				finished = TRUE;
			}
			MPI_Irecv(outPtr, packageLength, MPI_DOUBLE, masterRank,
			  MASTER_DATA_TAG, MPI_COMM_WORLD, &request);
			/*** Do something useful here? ***/
			workPtr->sampleCount += packageLength;
			MPI_Wait(&request, NULL);
			outPtr += packageLength;
			break;
		case MASTER_END_CHANNEL_TAG:
			MPI_Recv(&response, 1, MPI_INT, masterRank, MASTER_END_CHANNEL_TAG,
			  MPI_COMM_WORLD, &status);
			outPtr = _OutSig_EarObject(data)->channel[workPtr->chanCount++];
			if (_OutSig_EarObject(data)->length != workPtr->sampleCount) {
				NotifyError(wxT("%s: %s: Channel length (%u) is shorter than ")
				  wxT("the \ninitialised length for this pre-process data ")
				  wxT("(%u)."), workerName, funcName, workPtr->sampleCount,
				  _OutSig_EarObject(data)->length);
				ok = FALSE;
			}
			workPtr->sampleCount = 0;
			break;
		case MASTER_END_SIGNAL_TAG:
			MPI_Recv(&response, 1, MPI_INT, masterRank, MASTER_END_SIGNAL_TAG,
			  MPI_COMM_WORLD, &status);
			workPtr->chanCount = 0;
			finished = TRUE;
			break;
		default:
			NotifyError(wxT("%s: %s: Unknown tag received (%d)."), workerName,
			  funcName, status.MPI_TAG);
			break;
		} /* switch */
	}

}

/****************************** Initialise ************************************/

/*
 * This routine carries out the main initialisation for the worker process.
 * The Ran01_Random is called to ensure that each worker starts with a
 * a different random seed.  This random seed is reproducable as it is
 * dependent upon the worker's rank.
 */

void
Initialise(void)
{
	int		nameLength;
	long	randomSeed;
	MPI_Status		status;

	MPI_Comm_rank( MPI_COMM_WORLD, &myRank);
	MPI_Get_processor_name(hostName, &nameLength);
	sprintf(workerName, wxT("Worker[%d]"), myRank);
	printf(wxT("%s: Initialised on %s...\n"), workerName, hostName);
	MPI_Recv(&masterRank, 1, MPI_INT, MPI_ANY_SOURCE, MASTER_INIT_TAG,
	  MPI_COMM_WORLD, &status);
	MPI_Recv(charBuffer, BUFSIZ, MPI_CHAR, MPI_ANY_SOURCE, MASTER_INIT_TAG,
	  MPI_COMM_WORLD, &status);
	randomSeed = -myRank;
	Ran01_Random(&randomSeed);

}

/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int main(int argc, WChar *argv[])
{
	int		finished, ok;
	MPI_Status		status;

	MPI_Init( &argc, &argv ); 
	Initialise();

	for (finished = FALSE; !finished; ) {
		MPI_Probe(masterRank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		switch (status.MPI_TAG) {
		case MASTER_MESG_TAG:
			ProcessMesgTag();
			break;
		case MASTER_SET_TAG:
			ProcessSetTag();
			break;
		case MASTER_EXIT_TAG:
			MPI_Recv(&ok, 1, MPI_INT, masterRank, MASTER_EXIT_TAG,
			  MPI_COMM_WORLD, &status);
			finished = TRUE;
			printf(wxT("Worker, rank = %d finished (Master says: %s).\n"), myRank,
			  (ok == TRUE)? wxT("OK": "ERROR"));
			break;
		case MASTER_INIT_TAG:
			ProcessInitTag();
			break;
		default:
			NotifyError(wxT("%s: Received unknown tag (%d)."), workerName,
			  status.MPI_TAG);
			finished = TRUE;
			break;
		} /* switch */
	}
	MPI_Finalize();
	return(0);
	
}
