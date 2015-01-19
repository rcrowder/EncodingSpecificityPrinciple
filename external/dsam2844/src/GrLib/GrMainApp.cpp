/**********************
 *
 * File:		GrMainApp.cpp
 * Purpose:		This routine contains the classed GUI specific class derived
 *				from the MainApp class for handling the DSAM extensions
 *				library.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		16 Dec 2003
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

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtSSSymbols.h"
#include "UtAppInterface.h"
#include "ExtMainApp.h"
#include "GrSimMgr.h"
#include "GrSDIFrame.h"
#include "ExtXMLDocument.h"
#include "GrSDIXMLDoc.h"
#include "GrMainApp.h"
#include "DiSignalDisp.h"
#include "GrDisplayS.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/
/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * This is the main routine for using the extensions library, and is called by
 * the "main" subroutine in the header file.
 * The 'simFile2' variable is used to see if the simulation file has been
 * changed by the options.
 */

GrMainApp::GrMainApp(int theArgc, wxChar **theArgv): MainApp(theArgc, theArgv,
  MainSimulation_MainApp)
{

	dSAMMainApp = this;
	diagram = NULL;
	wxGetApp().ResetDefaultDisplayPos();
	if (GetPtr_AppInterface())
		wxGetApp().SetConfiguration(GetPtr_AppInterface()->parList);
	SetOnExecute_AppInterface(OnExecute_MyApp);
	SetOnExit_AppInterface(OnExit_MyApp);
	SetFuncGetWindowPosition_SignalDisp(GetWindowPosition_DisplayS);
	SetFuncInitCriticalSection_SignalDisp(InitCriticalSection_DisplayS);
	SetFuncFreeCriticalSection_SignalDisp(FreeCriticalSection_DisplayS);
	SetFuncFreeDisplay_SignalDisp(FreeDisplay_DisplayS);
	SetFuncShowSignal_SignalDisp(ShowSignal_DisplayS);
	GetPtr_AppInterface()->parList->pars[APP_INT_SIMULATIONFILE].enabled =
	  FALSE;
	ResetGUIDialogs();
	SetDiagMode(COMMON_DIALOG_DIAG_MODE);
	InitMain();

}

/****************************** InitXMLDocument *******************************/

/*
 * This initialises the document from an XML file.  If the 'diagram' is NULL
 * then the non-gui form is used - for list and exit.
 */

void
GrMainApp::InitXMLDocument(void)
{
	doc = (diagram)? new SDIXMLDocument(diagram): new DSAMXMLDocument();

}

/****************************** ResetSimulation *******************************/

bool
GrMainApp::ResetSimulation(void)
{
	bool ok = MainApp::ResetSimulation();
	wxGetApp().EnableSimParMenuOptions(ok);
	return(ok);

}

/****************************** SetRunIndicators ******************************/

void
GrMainApp::SetRunIndicators(bool on)
{
	wxGetApp().programMenu->Enable(SDIFRAME_STOP_SIMULATION, on);
//	wxGetApp().GetFrame()->SetStatusText((on)? wxT("Simulation Running..."):
//	  wxT("Simulation finished."), SDIFRAME_SIM_STATUS_FIELD);

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

