/**********************
 *
 * File:		AudPer86.c
 * Purpose:		This is a simple test program, running an auditory periphery
 *				model.  It has been written using hard programming.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		27 Jan 1996
 * Copyright:	(c) 1997, University of Essex
 *
 **********************/

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <DSAM.h>

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PARAMETERS_FILE		wxT("AudPer86.par")	/* Name of paramters file.*/
#define	NUM_CHANNELS		1			/* No. of channels for the filter. */
#define CHANNEL				0			/* Channel no. for test. */

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

WChar	outputFile[MAXLINE], stParFile[MAXLINE], trParFile[MAXLINE];
WChar	pEParFile[MAXLINE], bMParFile[MAXLINE], hCParFile[MAXLINE];

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** ReadParsFromFile ******************************/

/*
 * This program reads a specified number of parameters from a file.
 * It expects there to be one parameter per line.
 */
 
void
ReadParsFromFile(WChar *fileName)
{
	FILE	*fp;
	
	if ((fp = DSAM_fopen(fileName, "r")) == NULL) {
		NotifyError(wxT("ReadTestPars: Cannot open data file '%s'.\n"), fileName);
		exit(1);
	}
	DPrint(wxT("Reading parameters from file: %s\n"), fileName);
	Init_ParFile();
	GetPars_ParFile(fp, wxT("%s"), outputFile);
	GetPars_ParFile(fp, wxT("%s"), stParFile);
	GetPars_ParFile(fp, wxT("%s"), trParFile);
	GetPars_ParFile(fp, wxT("%s"), pEParFile);
	GetPars_ParFile(fp, wxT("%s"), bMParFile);
	GetPars_ParFile(fp, wxT("%s"), hCParFile);
	fclose(fp);
	Free_ParFile();
	
}

/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int main(void)
{
	EarObjectPtr	stimulus = NULL, gate = NULL, pEFilter = NULL;
	EarObjectPtr	bMFilter = NULL, hairCell = NULL, intensity = NULL;
	
	DPrint(wxT("Starting Test Harness...\n"));
	
	ReadParsFromFile(PARAMETERS_FILE);
	DPrint(wxT("\nIn this test a pure tone stimulus is presented to an\n"));
	DPrint(wxT("auditory periphery model.\n"));
	DPrint(wxT("\n"));
	
	/* Initialise EarObjects. */
	
	stimulus = Init_EarObject( wxT("null") );
	gate = Init_EarObject( wxT("null") );
	pEFilter = Init_EarObject( wxT("null") );
	bMFilter = Init_EarObject( wxT("null") );
	hairCell = Init_EarObject( wxT("null") );
	intensity = Init_EarObject( wxT("null") );

	/* Set up EarObject connections. */
	
	ConnectOutSignalToIn_EarObject( stimulus, intensity );

	ConnectOutSignalToIn_EarObject( stimulus, gate );
	ConnectOutSignalToIn_EarObject( gate, pEFilter );
	ConnectOutSignalToIn_EarObject( pEFilter, bMFilter );
	ConnectOutSignalToIn_EarObject( bMFilter, hairCell );

	/* Initialise modules. */

	DPrint(wxT("Module parameters...\n\n") );

	Init_PureTone( GLOBAL );
	ReadPars_PureTone( stParFile );
	PrintPars_PureTone();

	Init_Transform_Gate( GLOBAL );
	ReadPars_Transform_Gate( trParFile );
	PrintPars_Transform_Gate();

	Init_Filter_BandPass( GLOBAL );
	ReadPars_Filter_BandPass( pEParFile );
	PrintPars_Filter_BandPass();

	Init_BasilarM_GammaT( GLOBAL );
	ReadPars_BasilarM_GammaT( bMParFile );
	PrintPars_BasilarM_GammaT();

	Init_IHC_Meddis86( GLOBAL );
	ReadPars_IHC_Meddis86( hCParFile );
	PrintPars_IHC_Meddis86();
	
	Init_Analysis_Intensity( GLOBAL );
	SetTimeOffset_Analysis_Intensity( gatePtr->duration );
	PrintPars_Analysis_Intensity();

	/* Start main process and print diagonstics. */
	
	DPrint(wxT("\nStarting main process...\n\n") );
	GenerateSignal_PureTone( stimulus );
	PrintProcessName_EarObject( wxT("1-stimulus: '%s'.\n"), stimulus );
	Process_Transform_Gate( gate );
	PrintProcessName_EarObject( wxT("2-gate: '%s'.\n"), gate );
	Calc_Analysis_Intensity( intensity );
	DPrint(wxT("\tStimulus intensity = %g dB SPL.\n"),
	  GetResult_EarObject(intensity, CHANNEL ));
	RunModel_Filter_BandPass( pEFilter );
	PrintProcessName_EarObject(wxT("3-Outer-/middle-ear: '%s'.\n"), pEFilter );
	RunModel_BasilarM_GammaT( bMFilter );
	PrintProcessName_EarObject(wxT("4-Basilar membrane: '%s'.\n"), bMFilter );
	RunModel_IHC_Meddis86( hairCell );
	PrintProcessName_EarObject(wxT("5-Inner hair cell (IHC): '%s'.\n"), hairCell );
	WriteOutSignal_DataFile(outputFile, hairCell );

	FreeAll_EarObject();

	DPrint(wxT("Finished test.\n"));
    return(0);
	
}
