/**********************
 *
 * File:		StFMTone.h
 * Purpose:		Frequency modulated pure tone signal generation paradigm.
 * Comments:	Written using ModuleProducer version 1.5.
 * Author:		Almudena
 * Created:		Nov 28 1995
 * Updated:		12 Mar 1997
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

#ifndef _STFMTONE_H
#define _STFMTONE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PURETONE_FM_NUM_PARS			8

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	PURETONE_FM_FREQUENCY,
	PURETONE_FM_INTENSITY,
	PURETONE_FM_PHASE,
	PURETONE_FM_MODULATIONDEPTH,
	PURETONE_FM_MODULATIONFREQUENCY,
	PURETONE_FM_MODULATIONPHASE,
	PURETONE_FM_DURATION,
	PURETONE_FM_SAMPLINGINTERVAL

} FMToneParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	Float	frequency;
	Float	intensity;
	Float	duration;
	Float	dt;
	Float	phase;
	Float	modulationDepth;
	Float	modulationFrequency;
	Float	modulationPhase;

	/* Private members */
	UniParListPtr	parList;

} FMTone, *FMTonePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FMTonePtr	fMTonePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_PureTone_FM(EarObjectPtr data);

BOOLN	Free_PureTone_FM(void);

BOOLN	GenerateSignal_PureTone_FM(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_PureTone_FM(void);

BOOLN	Init_PureTone_FM(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_FM(void);

BOOLN	SetModulationDepth_PureTone_FM(Float theModulationDepth);

BOOLN	SetDuration_PureTone_FM(Float theDuration);

BOOLN	SetModulationFrequency_PureTone_FM(Float theModulationFrequency);

BOOLN	SetFrequency_PureTone_FM(Float theFrequency);

BOOLN	SetIntensity_PureTone_FM(Float theIntensity);

BOOLN	InitModule_PureTone_FM(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone_FM(ModulePtr theModule);

BOOLN	SetModulationPhase_PureTone_FM(Float thePhaseFM);

BOOLN	SetPhase_PureTone_FM(Float thePhase);

BOOLN	SetSamplingInterval_PureTone_FM(Float theSamplingInterval);

BOOLN	SetUniParList_PureTone_FM(void);

__END_DECLS

#endif
