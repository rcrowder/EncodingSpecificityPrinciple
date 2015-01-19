/**********************
 *
 * File:		UtPause.h
 * Purpose:		This module allows the processing of a simulation to be
 *				interrupted, producing a specified message.
 * Comments:	Written using ModuleProducer version 1.2.4 (May  7 1999).
 *				27-05-99 LPO: Added the 'alertMode' option.
 *				02-03-05 LPO: There is no point in thread enabling this module.
 * Author:		L. P. O'Mard
 * Created:		07 May 1999
 * Updated:		27 May 1999
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

#ifndef _UTPAUSE_H
#define _UTPAUSE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_PAUSE_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_PAUSE_ALERTMODE,
	UTILITY_PAUSE_DELAY,
	UTILITY_PAUSE_MESSAGE

} PauseParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		alertMode;
	int		delay;
	WChar	message[LONG_STRING];

	/* Private members */
	UniParListPtr	parList;

} Pause, *PausePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	PausePtr	pausePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Utility_Pause(EarObjectPtr data);

BOOLN	Free_Utility_Pause(void);

UniParListPtr	GetUniParListPtr_Utility_Pause(void);

BOOLN	Init_Utility_Pause(ParameterSpecifier parSpec);

void	Notify_Utility_Pause(WChar *format, ...);

BOOLN	PrintPars_Utility_Pause(void);

BOOLN	Process_Utility_Pause(EarObjectPtr data);

BOOLN	SetAlertMode_Utility_Pause(WChar * theAlertMode);

BOOLN	SetDelay_Utility_Pause(int theDelay);

BOOLN	SetMessage_Utility_Pause(WChar *theMessage);

BOOLN	InitModule_Utility_Pause(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_Pause(ModulePtr theModule);

BOOLN	SetUniParList_Utility_Pause(void);

__END_DECLS

#endif
