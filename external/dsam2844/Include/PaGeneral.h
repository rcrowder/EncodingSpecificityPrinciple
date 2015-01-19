/**********************
 *
 * File:		PaGeneral.h
 * Purpose:		This header file holds the general parallel functions etc.
 * Comments:	
 * Author:		
 * Created:		Nov 28 1995
 * Updated:		02 Jul 1996
 * Copyright:	(c) 1998, 2010 Lowel P. O'Mard
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

#ifndef _PAGENERAL_H
#define _PAGENERAL_H 1

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/* Data transfer */
#define	DATA_BLOCK_SIZE			5000

/****************************** Misc ******************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/
/****************************** Communications ********************************/

typedef enum {

	WORKER_CHANNEL_NORMAL_MODE,
	WORKER_CHANNEL_SET_TO_BM_MODE,
	WORKER_CHANNEL_MODE_NULL

} WorkerChanModeSpecifier;
	
typedef enum {

	WORKER_DIAGNOSTICS_OFF,
	WORKER_DIAGNOSTICS_FILE,
	WORKER_DIAGNOSTICS_SCREEN,
	WORKER_DIAGNOSTICS_NULL

} WorkerDiagModeSpecifier;

typedef enum {

	MASTER_INIT_TAG,
	MASTER_MESG_TAG,
	MASTER_DATA_TAG,
	MASTER_EXIT_TAG,
	MASTER_SET_TAG,
	MASTER_END_CHANNEL_TAG,
	MASTER_END_SIGNAL_TAG,
	WORKER_INIT_TAG,
	WORKER_MESG_TAG,
	WORKER_DATA_TAG,
	WORKER_END_CHANNEL_TAG,
	WORKER_END_SIGNAL_TAG,
	WORKER_SET_TAG

} TagSpecifier;

typedef enum {

	INIT_SIMULATION,
	RUN_SIMULATION,
	INIT_SIGNAL_PARS,
	REQUEST_SIGNAL_PARS,
	REQUEST_CHANNEL_DATA,
	PRINT_SIMULATION_PARS

} ControlSpecifier;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External Functions ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

#ifdef __cplusplus

extern "C" int	MPI_Comm_size(MPI_Comm comm, int *size);
extern "C" int	MPI_Comm_rank(MPI_Comm comm, int *rank);
extern "C" int	MPI_Finalize(void);
extern "C" int	MPI_Get_count(MPI_Status *status, MPI_Datatype datatype,
				  int *count);
extern "C" int	MPI_Get_processor_name(WChar *name, int *resultlen);
extern "C" int	MPI_Init(int *argc, WChar ***argv);
extern "C" int	MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag,
				  MPI_Status *status);
extern "C" int	 MPI_Irecv(void* buf, int count, MPI_Datatype datatype,
				   int source, int tag, MPI_Comm comm, MPI_Request *request);
extern "C" int	MPI_Isend(void* buf, int count, MPI_Datatype datatype, int dest,
				  int tag, MPI_Comm comm, MPI_Request *request);
extern "C" int	MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag,
				  MPI_Status *status);
extern "C" int	MPI_Recv(void* buf, int count, MPI_Datatype datatype,
				  int source, int tag, MPI_Comm comm, MPI_Status *status);
extern "C" int	MPI_Send(void* buf, int count, MPI_Datatype datatype, int dest,
				  int tag, MPI_Comm comm);
extern "C" int	MPI_Wait(MPI_Request *request, MPI_Status *status);

#endif

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

WChar *	ControlStr_MPI_General(ControlSpecifier control);

NameSpecifier *	InitWorkerChanModeList_MPI_General(void);

NameSpecifier *	InitWorkerDiagModeList_MPI_General(void);

SignalDataPtr	ReceiveSignalPars_MPI_General(int sourceRank, int tag);

BOOLN	SendChannelData_MPI_General(SignalDataPtr signal, int chanOffset,
		  int numChannels, int receiverRank, int dataTag, int endChannelTag);

BOOLN	SendSignalPars_MPI_General(SignalDataPtr signal, int numChannels,
		  int receiverRank, int tag);

BOOLN	SetOutSignal_MPI_General(EarObjectPtr data,
		  SignalDataPtr templateSignal);

__END_DECLS

#endif
