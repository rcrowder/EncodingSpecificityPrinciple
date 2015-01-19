/**********************
 *
 * File:		UtCreateBinaural.h
 * Purpose:		This routine creates an interleaved binaural signal from two
 *				EarObject's output signals.
 * Comments:	Written using ModuleProducer version 1.9.
 *				The output signal channels will be interleaved: LRLRLR... with
 *				inSignal[0] and inSignal[1] as left (L) and right (R)
 *				respectively.  If the two signals are already binaural, then
 *				they will be merged into a single binaural signal.
 *				It is not worth thread enabling this utility.
 * Author:		L. P. O'Mard
 * Created:		Feb 14 1996
 * Updated:	
 * Copyright:	(c) 2005, 2010 Lowel P. O'Mard
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

#ifndef _UTCREATEBINAURAL_H
#define _UTCREATEBINAURAL_H 1

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

BOOLN	CheckData_Utility_CreateBinaural(EarObjectPtr data);

BOOLN	InitModule_Utility_CreateBinaural(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_CreateBinaural(ModulePtr theModule);

BOOLN	Process_Utility_CreateBinaural(EarObjectPtr data);

__END_DECLS

#endif
