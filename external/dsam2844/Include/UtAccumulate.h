/**********************
 *
 * File:		UtAccumulate.h
 * Purpose:		This general utility accumulates its input signal into it
 *				output signal.
 * 				It adds to previous data if the output signal has already been
 * 				initialised and the data->updateProcessFlag is set to FALSE.
 * 				Otherwise it will overwrite the old signal or create a new
 *				signal as required.
 * 				The data->updateProcessFlag facility is useful for repeated
 *				runs.  It is set to FALSE before the routine returns.
 * Comments:	Written using ModuleProducer version 1.8.
 *				It does not have any parameters.
 *				06-09-96 LPO: Now can accumulate output from more than one
 *				EarObject.
 *				27-01-99 LPO: 
 * Author:		L. P. O'Mard
 * Created:		Dec 21 1995
 * Updated:		27 Jan 1998
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

#ifndef _UTACCUMULATE_H
#define _UTACCUMULATE_H 1

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

BOOLN	CheckData_Utility_Accumulate(EarObjectPtr data);

BOOLN	InitModule_Utility_Accumulate(ModulePtr theModule);

void	ResetProcess_Utility_Accumulate(EarObjectPtr data);

BOOLN	SetParsPointer_Utility_Accumulate(ModulePtr theModule);

BOOLN	Process_Utility_Accumulate(EarObjectPtr data);

__END_DECLS

#endif
