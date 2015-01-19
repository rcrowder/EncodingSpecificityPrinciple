/******************
 *		
 * File:		ExtSimThread.cpp
 * Purpose: 	Simulation thread class module.
 * Comments:	This was re-named from the RunMgr Class
 *				The 'RunMgr' class allows the MainSimulation program to send
 *				run information to the simulation manager.  The class contains
 *				classes that can be overridden.
 *				26-01-99 LPO: Introduced the 'ListSimParameters' function which
 *				prints the parameters for a simulation.
 *				Introduced the 'StautsChanged' function which indicates when
 *				the 'CheckProgInitialisation' routine needs to be run to
 *				re-initialise the simulation.
 *				29-01-99 LPO: Added a universal parameter list for the program
 *				parameters.
 *				05-05-99 LPO: Introduced the 'ListProgParameters' routine.
 *				14-05-99 LPO: Introduced the 'GetSimParFile' routine which
 *				returns a pointer to the simulation parameter file name.
 * Author:		L. P. O'Mard
 * Created:		20 Sep 1998
 * Updated:		14 May 1999
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtAppInterface.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"

#include "ExtMainApp.h"
#include "ExtRunThreadedProc.h"
#include "ExtProcThread.h"
#include "ExtSimThread.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

SimThread::SimThread(wxThreadKind kind): wxThread(kind)
{
	SetTestDestroy_ModuleMgr(TestDestroy_SimThread);
	if (runThreadedProc) {
		runThreadedProc->SetNumThreads(GetPtr_AppInterface()->numThreads);
		runThreadedProc->SetThreadMode(GetPtr_AppInterface()->threadMode);
	}

}

/****************************** Entry *****************************************/

// The thread execution starts here.
// This is a virtual function used by the WxWin threading code.

void *
SimThread::Entry()
{
	SetInterruptRequestStatus_Common(FALSE);
	dSAMMainApp->SetRunIndicators(true);
	bool ok = dSAMMainApp->RunSimulation();
	GetPtr_AppInterface()->simulationFinishedFlag = TRUE;

	return((void *) ok);

}


/****************************** OnExit ****************************************/

// called when the thread exits - whether it terminates normally or is
// stopped with Delete() (but not when it is Kill()ed!)

void
SimThread::OnExit()
{
	wxCriticalSectionLocker	locker(dSAMMainApp->mainCritSect);

	dSAMMainApp->simThread = NULL;
	SetTestDestroy_ModuleMgr(NULL);
	dSAMMainApp->SetRunIndicators(false);
	SwitchGUILocking_Common(FALSE);
	SetInterruptRequestStatus_Common(FALSE);

}

/****************************** SuspendDiagnostics ****************************/

void
SimThread::SuspendDiagnostics(void)
{
	wxCriticalSectionLocker locker(diagsCritSect);
	SetDiagMode(COMMON_OFF_DIAG_MODE);

}

/******************************************************************************/
/****************************** General Routines ******************************/
/******************************************************************************/

/*************************** TestDestroy **************************************/

/*
 * This routine is a dummy routine for the TestDestroy function required in
 * thread programs to notify that threads need to end.
 */
 
BOOLN
TestDestroy_SimThread(void)
{
	return(dSAMMainApp->simThread->MyTestDestroy());

}

