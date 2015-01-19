/******************
 *		
 * File:		ExtProcChainThread.h
 * Purpose: 	Chain Process thread class module.  It processes a chain of
 *				equal channelled processes.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		23 Sep 2004
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

#ifndef	_EXTPROCCHAINTHREAD_H
#define _EXTPROCCHAINTHREAD_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ProcThread ***************************************/

class ProcChainThread: public ProcThread {
	uInt	origNumChannels;
	wxCondition	*myCondition;
	wxMutex		*myMutex;
	DatumPtr	simulation;

  public:

	ProcChainThread(int theIndex, int offset, int numChannels,
	  DatumPtr start, wxMutex *mutex, wxCondition *condition,
	  int *theThreadCount);

	virtual void *Entry();

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


__END_DECLS

#endif
