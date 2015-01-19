/**********************
 *
 * File:		AnGeneral.h
 * Purpose:		This module contains various general analysis routines.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
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

#ifndef	_ANGENERAL_H
#define _ANGENERAL_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	ANALYSIS_SINGLE_CHANNEL	1		/* For single channel routines. */
#define	ANALYSIS_CHANNEL		0		/* For single channel routines. */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

Float	EuclideanDistance_GenAnalysis(Float *arrayA, Float *arrayB,
		  ChanLen startIndex, ChanLen endIndex);

BOOLN	LinearRegression_GenAnalysis(Float *y0, Float *gradient, Float *y,
		  Float dx, ChanLen startIndex, ChanLen length);

__END_DECLS

#endif
