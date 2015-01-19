/**********************
 *
 * File:		UtBinSignal.h
 * Purpose:		This routine generates a binned histogram from the EarObject's
 *				input signal.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				It adds to previous data if the output signal has already been
 *				initialised and the data->updateProcessFlag is set to FALSE.
 *				Otherwise it will overwrite the old signal or create a new
 *				signal as required.
 *				The data->updateProcessFlag facility is useful for repeated
 *				runs.
 * Author:		L. P. O'Mard
 * Created:		21 Mar 1996
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

#ifndef _UTBINSIGNAL_H
#define _UTBINSIGNAL_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_BINSIGNAL_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_BINSIGNAL_MODE,
	UTILITY_BINSIGNAL_BINWIDTH

} BinSignalParSpecifier;

typedef enum {

	UTILITY_BINSIGNAL_AVERAGE_MODE,
	UTILITY_BINSIGNAL_SUM_MODE,
	UTILITY_BINSIGNAL_MODE_NULL

} UtilityBinSignalModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;
	Float	binWidth;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;
	int		numBins;
	Float	wBinWidth, dt;

} BinSignal, *BinSignalPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BinSignalPtr	binSignalPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_BinSignal(EarObjectPtr data);

BOOLN	Free_Utility_BinSignal(void);

BOOLN	InitModeList_Utility_BinSignal(void);

BOOLN	Init_Utility_BinSignal(ParameterSpecifier parSpec);

UniParListPtr	GetUniParListPtr_Utility_BinSignal(void);

BOOLN	PrintPars_Utility_BinSignal(void);

BOOLN	Process_Utility_BinSignal(EarObjectPtr data);

void	ResetProcess_Utility_BinSignal(EarObjectPtr data);

BOOLN	SetBinWidth_Utility_BinSignal(Float theBinWidth);

BOOLN	SetMode_Utility_BinSignal(WChar * theMode);

BOOLN	InitModule_Utility_BinSignal(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_BinSignal(ModulePtr theModule);

BOOLN	SetPars_Utility_BinSignal(WChar * mode, Float binWidth);

BOOLN	SetUniParList_Utility_BinSignal(void);

__END_DECLS

#endif
