/******************
 *
 * File:		FiASCII.h
 * Purpose:		This Filing module deals with the writing reading of ASCII
 *				data files.
 * Comments:	14-04-98 LPO: This module has now been separated from the main
 *				FiDataFile module.
 * Authors:		L. P. O'Mard
 * Created:		17 Apr 1998
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

#ifndef	_FIASCII_H
#define _FIASCII_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

Float	GetDuration_ASCII(WChar *fileName);

BOOLN	ReadFile_ASCII(WChar *fileName, EarObjectPtr data);

BOOLN	WriteFile_ASCII(WChar *fileName, EarObjectPtr data);

__END_DECLS

#endif
