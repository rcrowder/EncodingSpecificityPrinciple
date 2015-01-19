/**********************
 *
 * File:		PaMaster1.h
 * Purpose:		This module runs an auditory model in parallel channels, using
 *				MPI.
 * Comments:	Written using ModuleProducer version 1.5.
 *				A simulation EarObject is initialised so that values from the
 *				simulation script can be obtained.
 * Author:		L. P. O'Mard
 * Created:		28 Nov 1995
 * Updated:		18 Mar 1997
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifndef _PAMASTER1_H
#define _PAMASTER1_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define MPI_MASTER1_NUM_PARS			5

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	MPI_MASTER1_NUMWORKERS,
	MPI_MASTER1_DIAGNOSTICSMODE,
	MPI_MASTER1_CHANNELMODE,
	MPI_MASTER1_SIMSPECPARFILE,
	MPI_MASTER1_CFLISTPARFILE

} Master1ParSpecifier;

typedef struct {

	int		rank;
	int		numChannels;
	int		offset;
	int		chanCount;
	ChanLen	sampleCount;

} WorkDetail, *WorkDetailPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN		updateProcessVariablesFlag;
	BOOLN		diagnosticsModeFlag, channelModeFlag, numWorkersFlag;
	WChar		*simScriptParFile;
	WChar		*cFListParFile;
	int			diagnosticsMode;
	int			channelMode;
	int			numWorkers;
	
	/* Private parameters */
	NameSpecifier	*workerDiagModeList;
	NameSpecifier	*workerChanModeList;
	UniParListPtr	parList;
	BOOLN			ok, printTheCFs;
	int				numChannels;
	int				interleaveLevel;
	CFListPtr		theCFs;
	ChanData		**dataPtr;
	WorkDetail		*workDetails;
	SetComPtr		setComQueue;
	SimScript			simScript;
	EarObjHandle	handle;

} Master1, *Master1Ptr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	Master1Ptr	master1Ptr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_MPI_Master1(EarObjectPtr data);

BOOLN	CheckPars_MPI_Master1(void);

int		FindWorkDetail_MPI_Master1(int rank);

BOOLN	Free_MPI_Master1(void);

BOOLN	FreeProcessVariables_MPI_Master1(void);

CFListPtr	GetCFListPtr_MPI_Master1(void);

DatumPtr	GetSimulation_MPI_Master1(void);

UniParListPtr	GetUniParListPtr_MPI_Master1(void);

BOOLN	Init_MPI_Master1(ParameterSpecifier parSpec);

void	InitChanModeList_MPI_Master1(void);

BOOLN	InitProcessVariables_MPI_Master1(EarObjectPtr data);

BOOLN	PrintPars_MPI_Master1(void);

BOOLN	QueueCommand_MPI_Master1(void *parameter, int parCount,
		  TypeSpecifier type, WChar *label, CommandSpecifier command,
		  ScopeSpecifier scope);

BOOLN	ReadCFListParFile_MPI_Master1(void);

BOOLN	ReadPars_MPI_Master1(WChar *fileName);

BOOLN	DriveWorkers_MPI_Master1(EarObjectPtr data);

int		GetNumWorkers_MPI_Master1(void);

BOOLN	SendControlToWorkers_MPI_Master1(EarObjectPtr data, int numWorkers,
		  ControlSpecifier control);

BOOLN	SendInSignalToWorkers_MPI_Master1(EarObjectPtr data);

int		SendParsToWorkers_MPI_Master1(EarObjectPtr data, int worker);

BOOLN	SendQueuedCommands_MPI_Master1(void);
  
BOOLN	SendSetCommandToWorker_MPI_Master1(WorkDetailPtr work, SetComPtr com);

BOOLN	SetCFListParFile_MPI_Master1(WChar *theCFListParFile);

BOOLN	SetChannelMode_MPI_Master1(WChar *theChannelMode);

BOOLN	SetDiagnosticsMode_MPI_Master1(WChar *theDiagnosticsMode);

BOOLN	InitModule_MPI_Master1(ModulePtr theModule);

BOOLN	SetNumWorkers_MPI_Master1(int theNumWorkers);

BOOLN	SetParsPointer_MPI_Master1(ModulePtr theModule);

BOOLN	SetSimScriptParFile_MPI_Master1(WChar *theSimScriptParFile);

BOOLN	SetPars_MPI_Master1(WChar *simScriptParFile, WChar *cFListParFile, 
		  WChar *diagnosticsMode, WChar *channelMode, int numWorkers);

BOOLN	SetWorkDivision_MPI_Master1(int masterRank);

void	Shutdown_MPI_Master1(int masterRank, int status);

BOOLN	SetUniParList_MPI_Master1(void);

__END_DECLS

#endif
