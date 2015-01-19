/**********************
 *
 * File:		AudPerSS.c
 * Purpose:		This is a simple test program, running an auditory periphery
 *				model.  It has been written using simulation specification
 *				programming.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		27 Jan 1996
 * Updated:		
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "DSAM.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PROGRAM_NAME				wxT("AudPerSS")
#ifndef PROGRAM_VERSION
#	define	PROGRAM_VERSION			wxT("1.1.0")
#endif
#define	TEST_FREQUENCY				2000.0

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** RegisterUserModules ***************************/

/*
 * This routine registers my user modules.
 */

BOOLN
RegisterUserModules(void)
{
	/*if (!RegEntry_ModuleReg("Test_Module", InitModule_User_Testmod))
		return(FALSE);*/
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This routine carries out general initialisation tasks.
 * It sets the initialCommand global variable.
 */

BOOLN
Init(void)
{
	static WChar *funcName = PROGRAM_NAME wxT(": Init");

	if (!GetPtr_AppInterface() && !Init_AppInterface(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise the application interface."),
		  funcName);
		exit(1);
	}

	SetAppName_AppInterface(PROGRAM_NAME);
	SetAppVersion_AppInterface(PROGRAM_VERSION);
	SetCompiledDSAMVersion_AppInterface(DSAM_VERSION);

	SetTitle_AppInterface(PROGRAM_NAME);

	SetAppRegisterUserModules_AppInterface(RegisterUserModules);
	return(TRUE);

}

/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int MainSimulation(MAIN_ARGS)
{
	int		chan;
	ChanData	*outPtr;
	ChanLen	sample;

	EarObjectPtr	process;

	if (!InitProcessVariables_AppInterface(Init, ARGC, ARGV))
		return(1);

	if (SetRealSimPar_AppInterface(wxT("frequency"), TEST_FREQUENCY))
		DPrint(wxT("%s: Frequency set to %g Hz by program\n"), PROGRAM_NAME,
		  TEST_FREQUENCY);
	else
		NotifyError(wxT("%s: Could not set frequency for simulation."),
		  PROGRAM_NAME);
	PrintSimPars_AppInterface();

	DPrint(wxT("Starting process...\n"));
	ResetSim_AppInterface();

	if (!RunSim_AppInterface())
		return(0);
	
	/* Uncomment the following lines to show how you can use the output from
	 * the simulation.
	 */
	/*
	process = GetSimProcess_AppInterface();
	for (chan = 0; chan < process->outSignal->numChannels; chan++) {
		outPtr = process->outSignal->channel[chan];
		printf("Channel %d\n", chan);
		for (sample = 0; sample < process->outSignal->length; sample++)
			printf("%g\n", *outPtr++);
	}
	*/
	Free_AppInterface();
	DPrint(wxT("Finished test.\n"));
	return(0);
	
}
