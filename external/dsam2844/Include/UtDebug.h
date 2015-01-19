/********************
 *
 * UtDebug.h
 *
 * This header file is for the debug module used in testing.
 *
 * by Lowel O'Mard 26-10-93.
 *
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

/*
 * The following include files enable me to use WriteOutSignal_DataFile
 * while only adding the UtDebug.h header.
 */

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/


/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern EarObjectPtr	debug;		/* This is used for module debugging. */

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void		Free_Debug(void);

void		Init_Debug(void);

BOOLN	CheckInit_Debug(void);

BOOLN	SetChannelValue_Debug(int channel, ChanLen sample, Float value);

BOOLN	SetPars_Debug(uShort numChannels, ChanLen length,
			  Float samplingInterval);

__END_DECLS

void	WriteArray_Debug(WChar *fileName, Float *p, ChanLen length, int increment);

BOOLN	WriteOutSignal_Debug(WChar *fileName);
