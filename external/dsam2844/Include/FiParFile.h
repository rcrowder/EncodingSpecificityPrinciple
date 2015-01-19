/**********************
 *
 * File:		FiParFile.h
 * Purpose:		This module contains the general parameter file handling
 *				routines for the DSAM Core Routines library.
 * Comments:	07-05-99 LPO: I have changed the 'IdentifyFormat_' routine so
 *				that it now returns any identified extra format characters using
 *				the 'extraFmt' header argument.
 *				29-05-99 LPO: Introduced the 'ExtractQuotedString_' routine so
 *				that strings can now be quoted, so that multiple words can be
 *				read.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		29 May 1999
 * Copyright:	(c) 1999, 2010 Lowel P. O'Mard
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

#ifndef	_FIPARFILE_H
#define _FIPARFILE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define FORMAT_DELIMITERS		wxT(" ,\t")		/* Characters to separate
											 * formatting commands. */

/******************************************************************************/
/*************************** Type Definitions *********************************/
/******************************************************************************/

typedef struct {

	BOOLN	diagnosticsFlag;	/* Set to TRUE to print diagnostics. */
	BOOLN	emptyLineMessageFlag; /* Set to TRUE to GetPars_.. error message. */
	int		parLineCount;	/*  A record of the parameter file lines read. */

} ParFile, *ParFilePtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

__BEGIN_DECLS

extern ParFilePtr parFile;

__END_DECLS

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckInit_ParFile(void);

BOOLN	CommentOrBlankLine_ParFile(WChar *line);

BOOLN	ExtractQuotedString_ParFile(WChar *string, WChar *source);

BOOLN	Free_ParFile(void);

WChar	*GetLine_ParFile(WChar *line, int maxChars, FILE *fp);

int		GetLineCount_ParFile(void);

BOOLN	GetPars_ParFile(FILE *fp, const WChar *fmt, ...);

int		IdentifyFormat_ParFile(WChar *fmt, WChar *extraFmt);

BOOLN	Init_ParFile(void);

void	SetEmptyLineMessage_ParFile(BOOLN status);

__END_DECLS

#endif
