/******************
 *		
 * File:		ExtSimThread.h
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
 * Copyright:	(c) 1999, 2001, 2010 Lowel P. O'Mard
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

#ifndef	_EXTSIMTHREAD_H
#define _EXTSIMTHREAD_H	1

#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"

#include "UtDatum.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SimThread ****************************************/

class SimThread: public wxThread {

  public:
	wxCriticalSection	critSect, diagsCritSect;

	SimThread(wxThreadKind kind = wxTHREAD_DETACHED);

	virtual void *Entry();
	virtual void OnExit();
	
	DatumPtr	ExecuteSimulation(DatumPtr start);
	bool	MyTestDestroy(void)	{ return TestDestroy(); }
	bool	PreThreadProcessInit(DatumPtr pc);
	bool	RunThreadEnabledProcess(DatumPtr pc);
	void	SuspendDiagnostics(void);

};

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DatumPtr	ExecuteSimulation_SimThread(DatumPtr start);

BOOLN	TestDestroy_SimThread(void);

__END_DECLS

#endif
