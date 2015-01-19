/******************
 *
 * File:		FiSndFile.h
 * Purpose:		This Filing reads sound format files using the libsndfile
 * 				library.
 * Comments:	
 * Authors:		L. P. O'Mard
 * Created:		07 Nov 2006
 * Updated:		
 * Copyright:	(c) 2010 Lowel P. O'Mard
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

#ifndef	_FISNDFILE_H
#define _FISNDFILE_H	1

#if HAVE_SNDFILE

#include <sndfile.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SND_FILE_FORMAT_DELIMITERS		" "
#define SND_FILE_FORMAT_PAR_SEPARATOR	':'
#define SND_FILE_TEST_FILE_MEMORY_SIZE	512

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

#define SND_FILE_ARRAY_LEN(x)	((int) (sizeof (x)) / (sizeof ((x) [0])))
#define SND_FILE_VIO_PTR(earObj)	(((DataFilePtr) (earObj)->module->parsPtr)->vIOPtr)


/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef struct {
	
	sf_count_t	offset;
	sf_count_t	length;
	sf_count_t	maxLength;
	unsigned char *data;

} DFVirtualIO, *DFVirtualIOPtr ;

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	Free_SndFile(void);

DSAM_API void	FreeVirtualIOMemory_SndFile(DFVirtualIOPtr *p);

double	GetDuration_SndFile(WChar *fileName);

DSAM_API BOOLN	InitVirtualIOMemory_SndFile(DFVirtualIOPtr *p,
				  sf_count_t maxLength);

BOOLN	ReadFile_SndFile(WChar *fileName, EarObjectPtr data);

BOOLN	WriteFile_SndFile(WChar *fileName, EarObjectPtr data);

__END_DECLS

#endif /* HAVE_SNDFILE */
#endif /* _FISNDFILE_H */
