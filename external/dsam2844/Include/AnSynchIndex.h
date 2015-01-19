/**********************
 *
 * File:		AnSynchIndex.h
 * Purpose:		Calculates the synchronisation index (vector strength) from
 *				the input signal.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				The input is expected to come from a period histogram (PH).
 * Author:		L. P. O'Mard
 * Created:		1 Mar 1996
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

#ifndef _ANSYNCHINDEX_H
#define _ANSYNCHINDEX_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

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

BOOLN	InitModule_Analysis_SynchIndex(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_SynchIndex(ModulePtr theModule);

BOOLN	Calc_Analysis_SynchIndex(EarObjectPtr data);

BOOLN	CheckData_Analysis_SynchIndex(EarObjectPtr data);

__END_DECLS

#endif
