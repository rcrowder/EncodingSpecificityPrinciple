/**********************
 *
 * File:		AnIntensity.h
 * Purpose:		This module calculates the intensity for a signal, starting
 *				from an offset position.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard
 * Created:		12 Jun 1996
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

#ifndef _ANINTENSITY_H
#define _ANINTENSITY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_INTENSITY_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_INTENSITY_TIMEOFFSET,
	ANALYSIS_INTENSITY_EXTENT

} IntensityParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	Float	timeOffset;
	Float	extent;

	/* Private members */
	UniParListPtr	parList;
	ChanLen	timeOffsetIndex, wExtent;

} Intensity, *IntensityPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	IntensityPtr	intensityPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_Intensity(EarObjectPtr data);

BOOLN	CheckData_Analysis_Intensity(EarObjectPtr data);

BOOLN	Free_Analysis_Intensity(void);

UniParListPtr	GetUniParListPtr_Analysis_Intensity(void);

BOOLN	Init_Analysis_Intensity(ParameterSpecifier parSpec);

BOOLN	PrintPars_Analysis_Intensity(void);

BOOLN	InitModule_Analysis_Intensity(ModulePtr theModule);

BOOLN	SetExtent_Analysis_Intensity(Float theExtent);

BOOLN	SetParsPointer_Analysis_Intensity(ModulePtr theModule);

BOOLN	SetTimeOffset_Analysis_Intensity(Float theTimeOffset);

BOOLN	SetUniParList_Analysis_Intensity(void);

__END_DECLS

#endif
