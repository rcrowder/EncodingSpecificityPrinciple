/**********************
 *
 * File:		UtFIRFilter.h
 * Purpose:		This contains the structures for the FIR Filter.
 * Comments:	This module is to be put into UtFilters.
 * Author:		L. P. O'Mard.
 * Created:		04 Dec 2000
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

#ifndef _UTFIRFILTERS_H
#define _UTFIRFILTERS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct {

	int		numChannels;
	int		m;
	Float	*c;
	Float	*state;

} FIRCoeffs, *FIRCoeffsPtr;

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

void	FIR_FIRFilters(EarObjectPtr data, FIRCoeffsPtr p);

void	FreeFIRCoeffs_FIRFilters(FIRCoeffsPtr *p);

FIRCoeffsPtr	InitFIRCoeffs_FIRFilters(int numChannels, int numTaps,
				  int numBands, Float *bands, Float *desired, Float *weights,
				  int type);

void	ProcessBuffer_FIRFilters(EarObjectPtr data, FIRCoeffsPtr p);

__END_DECLS

#endif
