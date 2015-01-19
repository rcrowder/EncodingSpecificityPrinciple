/*******
 *
 * Simple1.c
 *
 ********/

#include <stdlib.h> 
#include <stdio.h>

#include "DSAM.h"

int MainSimulation(void)
{
	EarObjectPtr	stimulus = NULL, pEFilter = NULL, bMFilter = NULL;
	EarObjectPtr	hairCell = NULL;
	
	DPrint(wxT("Starting Test Harness...\n"));
	
	/* Initialise EarObjects. */
	
	stimulus = Init_EarObject(wxT("Stim_click"));
	pEFilter = Init_EarObject(wxT("Filt_BandPass"));
	bMFilter = Init_EarObject(wxT("BM_GammaT"));
	hairCell = Init_EarObject(wxT("IHC_Meddis86"));

	/* Set up EarObject connections. */
	
	ConnectOutSignalToIn_EarObject( stimulus, pEFilter );
	ConnectOutSignalToIn_EarObject( pEFilter, bMFilter );
	ConnectOutSignalToIn_EarObject( bMFilter, hairCell );
 
	/* Initialise modules. */

	DPrint(wxT("Module parameters...\n\n") );

	ReadPars_ModuleMgr( stimulus, wxT("Click1.par"));
	PrintPars_ModuleMgr( stimulus );

	ReadPars_ModuleMgr( pEFilter, wxT("PreEmph1.par"));
	PrintPars_ModuleMgr( pEFilter );

	ReadPars_ModuleMgr( bMFilter, wxT("GammaTLog.par"));
	PrintPars_ModuleMgr( bMFilter );

	ReadPars_ModuleMgr( hairCell, wxT("Meddis86.par"));
	PrintPars_ModuleMgr( hairCell );


    /* Main simulation process */
	
	printf(wxT("\n"));
	RunProcess_ModuleMgr(stimulus);
	RunProcess_ModuleMgr(pEFilter);
	RunProcess_ModuleMgr(bMFilter);
	RunProcess_ModuleMgr(hairCell);

	WriteOutSignal_DataFile(wxT("output.dat"), hairCell);
	FreeAll_EarObject();
	DPrint(wxT("Finished test.\n"));
	return(0);
	
}
