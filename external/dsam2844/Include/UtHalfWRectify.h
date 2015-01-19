/**********************
 *
 * File:		UtHalfWRectify.h
 * Purpose:		This proces module half-wave rectifies a signal, setting
 *				all negative values to zero.
 * Comments:	Written using ModuleProducer version 1.2.6 (Jul 22 1999).
 * Author:		L. P. O'Mard
 * Created:		22 Jul 1999
 * Updated:	
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

#ifndef _UTHALFWRECTIFY_H
#define _UTHALFWRECTIFY_H 1

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

BOOLN	CheckData_Utility_HalfWaveRectify(EarObjectPtr data);

BOOLN	InitModule_Utility_HalfWaveRectify(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_HalfWaveRectify(ModulePtr theModule);

BOOLN	Process_Utility_HalfWaveRectify(EarObjectPtr data);

__END_DECLS

#endif
