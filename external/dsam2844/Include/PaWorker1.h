/**********************
 *
 * File:		PrWorker1.h
 * Purpose:		Header for for the program whichruns an auditory model using
 *				MPI.
 * Comments:	This is a complete program whose executable will be loaded
 * 				using the MPI loader.
 * Author:		
 * Created:		Nov 28 1995
 * Updated:	
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

#ifndef _PAWORKER1_H
#define _PAWORKER1_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PA_WORKER1_SET_COMMAND_MAX_ARRAY	10
#define	PA_WORKER1_MAX_SIMULATIONS			10

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct {

	int				chanCount;
	ChanLen			sampleCount;
	EarObjHandle	handle;
	EarObjectPtr	preProcessData;
	EarObjectPtr	simulation;
	
} Work, *WorkPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

WorkPtr	AddWork(EarObjHandle handle);

BOOLN	GetParsFromMaster(WorkPtr workPtr);

WorkPtr	GetWorkPtr(EarObjHandle handle);

void	Initialise(void);

void	ProcessInitTag(void);

void	ProcessMesgTag(void);

void	ProcessSetTag(void);

__END_DECLS

#endif
