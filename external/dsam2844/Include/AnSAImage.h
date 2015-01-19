/**********************
 *
 * File:		AnSAImage.h
 * Purpose:		This module implements the AIM stabilised auditory image (SAI)
 *				process.
 *				See Patterson R. D. and Allerhand M. H. (1995) "Time-domain
 *				modeling of peripheral auditory processing: A modular
 *				Architecture and a software platform", J. Acoust. Soc. Am. 98,
 *				1890-1894.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 *				This module uses the UtStrobe utility module.
 *				29-10-97 LPO: Introduced 'dataBuffer' EarObject for the
 *				correct treatment during segment processing.  The previous data
 *				is assumed to be the negative width plus zero.
 *				04-11-97 LPO: Introduced the correct 'strobe is time zero'
 *				behaviour.
 *				04-11-97 LPO: Improved buffer algorithm so the signal segments
 *				less than the SAI length can be used.
 *				18-11-97 LPO: Introduced the inputDecayRate parameter.
 *				11-12-97 LPO: Allowed the "delayed" strobe modes to be processed
 *				correctly, by interpreting delays as an additional shift using
 *				the postive and negative widths.
 *				28-05-98 LPO: Put in check for zero image decay half-life.
 *				09-06-98 LPO: The strobe information is now output as part of
 *				the diagnostics.
 *				12-06-98 LPO: Corrected numLastSamples problem, which was
 *				causing strobes to be processed passed the end of the strobe
 *				data.
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 *				08-10-98 LPO: The initialisation and free'ing of the 'Strobe'
 *				utility module is now done at the same time as the SAI module.
 *				This was necessary for the UniPar implementation.
 * Author:		L. P. O'Mard
 * Created:		12 Oct 1997
 * Updated:		08 Oct 1998
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

#ifndef _ANSAIMAGE_H
#define _ANSAIMAGE_H 1

#include "UtStrobe.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define SAI_NUM_PARS			7
#define SAI_NUM_SUB_PROCESSES	1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	SAI_STROBEDATA

} SAISubProcessSpecifier;

typedef enum {

	SAI_DIAGNOSTIC_MODE,
	SAI_INTEGRATION_MODE,
	SAI_STROBE_SPECIFICATION,
	SAI_NEGATIVE_WIDTH,
	SAI_POSITIVE_WIDTH,
	SAI_INPUT_DECAY_RATE,
	SAI_IMAGE_DECAY_HALF_LIFE

} SAIParSpecifier;

typedef enum {

	SAI_INTEGRATION_MODE_STI,
	SAI_INTEGRATION_MODE_AC,
	SAI_INTEGRATION_MODE_NULL

} AnalysisIntegrationModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		integrationMode;
	WChar	strobeSpecification[MAX_FILE_PATH];
	Float	negativeWidth;
	Float	positiveWidth;
	Float	inputDecayRate;
	Float	imageDecayHalfLife;

	/* Private members */
	NameSpecifier	*diagnosticModeList;
	NameSpecifier	*integrationModeList;
	UniParListPtr	parList;
	WChar			diagnosticString[MAX_FILE_PATH];
	int				strobeInSignalIndex;
	int				numThreads;
	Float			*inputDecay;
	FILE			*fp;
	ChanLen			*inputCount;
	ChanLen			zeroIndex;
	ChanLen			positiveWidthIndex;
	ChanLen			*decayCount;
	EarObjectPtr	strobeData;
	EarObjectPtr	dataBuffer;
	EarObjectPtr	strobeDataBuffer;

} SAImage, *SAImagePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SAImagePtr	sAImagePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Analysis_SAI(EarObjectPtr data);

BOOLN	Free_Analysis_SAI(void);

BOOLN	FreeProcessVariables_Analysis_SAI(void);

UniParListPtr	GetUniParListPtr_Analysis_SAI(void);

BOOLN	InitIntegrationModeList_Analysis_SAI(void);

BOOLN	Init_Analysis_SAI(ParameterSpecifier parSpec);

BOOLN	InitInputDecayArray_Analysis_SAI(EarObjectPtr data);

BOOLN	InitProcessVariables_Analysis_SAI(EarObjectPtr data);

void	OutputStrobeData_Analysis_SAI(void);

BOOLN	PrintPars_Analysis_SAI(void);

BOOLN	Process_Analysis_SAI(EarObjectPtr data);

void	ProcessFrameSection_Analysis_SAI(EarObjectPtr data,
		  ChanData **strobeStatePtrs, ChanData **dataPtrs, ChanLen strobeOffset,
		  ChanLen frameOffset, ChanLen sectionLength);

void	PushBufferData_Analysis_SAI(EarObjectPtr data, ChanLen frameLength);

void	ResetProcess_Analysis_SAI(EarObjectPtr data);

BOOLN	SetDiagnosticMode_Analysis_SAI(WChar *theDiagnosticMode);

BOOLN	SetImageDecayHalfLife_Analysis_SAI(Float theImageDecayHalfLife);

BOOLN	SetInputDecayRate_Analysis_SAI(Float theInputDecayRate);

BOOLN	SetIntegrationMode_Analysis_SAI(WChar * theIntegrationMode);

BOOLN	SetDelay_Analysis_SAI(Float theDelay);

BOOLN	SetDelayTimeout_Analysis_SAI(Float theDelayTimeout);

BOOLN	InitModule_Analysis_SAI(ModulePtr theModule);

BOOLN	SetNegativeWidth_Analysis_SAI(Float theNegativeWidth);

BOOLN	SetParsPointer_Analysis_SAI(ModulePtr theModule);

BOOLN	SetPositiveWidth_Analysis_SAI(Float thePositiveWidth);

BOOLN	SetStrobeSpecification_Analysis_SAI(WChar *theStrobeSpecification);

BOOLN	SetThresholdDecayRate_Analysis_SAI(Float theThresholdDecayRate);

BOOLN	SetThreshold_Analysis_SAI(Float theThreshold);

BOOLN	SetTypeMode_Analysis_SAI(WChar *theTypeMode);

BOOLN	SetUniParList_Analysis_SAI(void);

__END_DECLS

#endif
