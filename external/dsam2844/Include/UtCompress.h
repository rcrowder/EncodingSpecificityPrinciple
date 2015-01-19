/**********************
 *
 * File:		UtCompress.h
 * Purpose:		Compresses an input signal using log or power compression.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard revised from AIM code
 * Created:		28 Jun 1996
 * Updated:
 * Copyright:	(c) 1996, 2010 Lowel P. O'Mard
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

#ifndef _UTCOMPRESS_H
#define _UTCOMPRESS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_COMPRESSION_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_COMPRESSION_MODE,
	UTILITY_COMPRESSION_SIGNALMULTIPLIER,
	UTILITY_COMPRESSION_POWEREXPONENT,
	UTILITY_COMPRESSION_MINRESPONSE

} CompressionParSpecifier;

typedef enum {

	COMPRESS_LOG_MODE,
	COMPRESS_POWER_MODE,
	COMPRESS_NULL

} CompressModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;
	Float	signalMultiplier;
	Float	powerExponent;
	Float	minResponse;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;
	Float	scale, minInput;

} Compression, *CompressionPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CompressionPtr	compressionPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_Compression(EarObjectPtr data);

BOOLN	Free_Utility_Compression(void);

UniParListPtr	GetUniParListPtr_Utility_Compression(void);

BOOLN	Init_Utility_Compression(ParameterSpecifier parSpec);

BOOLN	InitModeList_Utility_Compression(void);

BOOLN	PrintPars_Utility_Compression(void);

BOOLN	Process_Utility_Compression(EarObjectPtr data);

BOOLN	SetMinResponse_Utility_Compression(Float theMinResponse);

BOOLN	SetMode_Utility_Compression(WChar *theMode);

BOOLN	InitModule_Utility_Compression(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_Compression(ModulePtr theModule);

BOOLN	SetPowerExponent_Utility_Compression(Float thePowerExponent);

BOOLN	SetSignalMultiplier_Utility_Compression(Float theSignalMultiplier);

BOOLN	SetUniParList_Utility_Compression(void);

__END_DECLS

#endif
