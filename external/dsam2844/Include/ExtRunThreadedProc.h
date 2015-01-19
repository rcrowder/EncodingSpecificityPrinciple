/******************
 *
 * File:		ExtRunThreadedProc.h
 * Purpose: 	This module runs a processes using threads of possible.
 * Comments:	It was deceided to create this module rather than creating
 *				a replacement ExecuteSimulation routine.
 * Author:		L. P. O'Mard
 * Created:		30 Sep 2004
 * Updated:
 * Copyright:	(c) 2004, 2010 Lowel P. O'Mard
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

#ifndef	_EXTRUNTHREADEDPROC_H
#define _EXTRUNTHREADEDPROC_H	1

#include "GeSignalData.h"
#include "GeEarObject.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** RunThreadedProc **********************************/

class RunThreadedProc {

	int		numThreads, chansPerThread, remainderChans, threadCount;
	int		threadMode;

  public:
	RunThreadedProc(void);

	DatumPtr	CleanUpThreadRuns(DatumPtr start);
	bool	DetermineChannelChains(DatumPtr start, bool *brokenChain);
	DatumPtr	Execute(DatumPtr start);
	DatumPtr	ExecuteMultiThreadChain(DatumPtr start);
	DatumPtr	ExecuteStandardChain(DatumPtr start);
	bool	InitialiseProcesses(DatumPtr start);
	bool	InitThreadProcesses(EarObjectPtr data);
	bool	PreThreadSimulationInit(DatumPtr start, bool *brokenChain);
	bool	PreThreadProcessInit(EarObjectPtr data);
	void	RestoreProcess(EarObjectPtr process);
	void	RestoreSimulation(DatumPtr simulation);
	bool	RunProcess(EarObjectPtr data);
	void	SetNumThreads(int theNumThreads);
	void	SetThreadDistribution(int numChannels);
	bool	SetThreadMode(int mode);

};

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

extern RunThreadedProc	*runThreadedProc;

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

BOOLN	RunProcess_RunThreadedProc(EarObjectPtr data);

DatumPtr	Execute_RunThreadedProc(DatumPtr start, DatumPtr passedEnd,
			  int threadIndex);

__END_DECLS

#endif
