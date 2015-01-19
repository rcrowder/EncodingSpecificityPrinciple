/******************
 *
 * File:		FiDataFile.h
 * Purpose:		This Filing module deals with the writing reading of data files.
 * Comments:	The sound format file routines were revised from sources from
 *				Lyon's Cochlear Model, The Program, Malcolm Slaney, Advanced
 *				Technology Group, Apple Computer, Inc., malcom@apple.com 1988.
 *				(Not the MicroSoft Wave file format routines.)
 *				10-11-97 LPO:  I have removed the SetTimeContinuity_EarObject()
 *				call from the individual routines.  The
 *				SetProcessContinuity_EarObject is now called only from the
 *				main ReadSignal_DataFile routine as it will reflect any
 *				internl changes in that routine.
 *				12-11-97 LPO: Put in dataFilePtr == NULL tests for setting
 *				routines.
 *				23-11-97 LPO: Added WriteOutSignal_DataFile_Named so that output
 *				can be specified from simulation specifications.
 *				14-04-97 LPO: Moved the sound file format specific routines into
 *				individual module files, i.e. FiRaw.[ch], FiAIFF.[ch] and
 *				FiMSWave.[ch].
 *				20-04-98 LPO: The dataFilePtr->maxSamples field is now set from
 *				the duration parameter which is read as a parameter.
 *				09-09-98 LPO: In ReadSignal_.. the SetContinuity_EarObject
 *				routine is only called if there is a successful read.
 *				03-02-99 LPO: The 'SetFileName_' routine now also sets the
 *				'type' specifier, It is still redundantly checked in the
 *				'ReadDataFile_' routine but this is kept to ensure that this
 *				part of the code can be used without the rest of DSAM.
 *				21-04-99 LPO: Changed the name of the 'WriteOutSignal' routine
 *				to 'WriteOutSignalMain'.  The new 'WriteOutSignal' now calls
 *				this routine, first setting 'dataFilePtr' to a temporary local
 *				pointer which is free'd after use.
 *				The same has been done for 'ReadSignal_'.
 *				Set the 'normalisation' initialisation to -1.0 so that
 *				automatic normalisation is employed.
 *				27-04-99 LPO: Implemented 32 bit file support.
 *				04-08-99 LPO: The 'InitProcessVariables_' routine now correctly
 *				implements the auto-normalisation for calculating the
 *				'datafilePtr->normalise' parameter.
 * Authors:		L. P. O'Mard.
 * Created:		12 Jul 1993
 * Updated:		04 Aug 1999
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

#ifndef	_FIDATAFILE_H
#define _FIDATAFILE_H	1

#if HAVE_SNDFILE
#	include <sndfile.h>
#	include "FiSndFile.h"
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*************************** Misc definitions *********************************/

#define DATAFILE_NUM_PARS		10
#define DATAFILE_MOD_NAME_PREFIX	wxT("DataFile_")
#define DATAFILE_IN_MOD_NAME	DATAFILE_MOD_NAME_PREFIX wxT("IN")
#define DATAFILE_OUT_MOD_NAME	DATAFILE_MOD_NAME_PREFIX wxT("OUT")
#define DATAFILE_BUFFER_FRAMES	1024
#define DATAFILE_NEGLIGIBLE_SR_DIFF		1.0e-10
#define DATAFILE_CHANNEL_LABEL_SPACE	9
#define	STDIN_STDOUT_FILE_DIRN	'-'		/* For the direction of files. */
#define	MEMORY_FILE_DIRN		'+'		/*           "                 */
#define	MAXLINE_LARGE			256		/* For very "wide" ASCII files */
#define	FOR_ASCII_READING		"r"
#define	FOR_ASCII_APPENDING		"a"
#define	FOR_ASCII_WRITING		"w"

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum Specifiers **********************************/
/******************************************************************************/

typedef enum {

	DATAFILE_FILENAME,
	DATAFILE_SUBFORMATTYPE,
	DATAFILE_WORDSIZE,
	DATAFILE_ENDIAN,
	DATAFILE_NUMCHANNELS,
	DATAFILE_NORMALISE,
	DATAFILE_DEFAULTSAMPLERATE,
	DATAFILE_DURATION,
	DATAFILE_TIMEOFFSET,
	DATAFILE_GAIN

} DataFileParSpecifier;

typedef enum {

	ASCII_DATA_FILE	= 0,
	NULL_DATA_FILE	= 0xFFFF

}  FileFormatSpecifier;

typedef enum {

	DATA_FILE_DEFAULT_ENDIAN,
	DATA_FILE_LITTLE_ENDIAN,
	DATA_FILE_BIG_ENDIAN,
	DATA_FILE_CPU_ENDIAN,
	DATA_FILE_ENDIAN_NULL

} DataFileEndianModeSpecifier;

typedef enum {

	DATA_FILE_INTERLEAVELEVEL,
	DATA_FILE_NUMWINDOWFRAMES,
	DATA_FILE_STATICTIMEFLAG,
	DATA_FILE_OUTPUTTIMEOFFSET,
	DATA_FILE_NORMALISATION,
	DATA_FILE_DSAMVERSION,
	DATA_FILE_LARGEDT

} DataFileDSAMFormatSpecifier;

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

#if !HAVE_SNDFILE
	typedef ChanLen	sf_count_t;
#endif /* HAVE_SNDFILE */

typedef struct {

	BOOLN	updateProcessVariablesFlag;

	ParameterSpecifier parSpec;

	WChar	name[MAX_FILE_PATH];/* - used by Generate Signal, set by ReadPars.*/
	int		subFormatType;
	int		wordSize;			/* can be either 1 or 2 byte words. */
	int		endian;				/* can be either 0 or 1, little- or big-endian*/
	int		numChannels;		/* used only by Raw binary support at present.*/
	Float	defaultSampleRate;	/* - for when this is not specified. */
	Float 	duration;			/* This can be set to restrict the data size. */
	Float	timeOffset;			/* Set to miss date at start of signal. */
	Float	gain;				/* gain for normalised input signals. */
	Float	normalisation;		/* AIFF, Raw, MS Wave support, <0.0 for auto. */

	/* Call back routines. */
	Float (* GetDuration)(void);

	/* Private parameters */
	NameSpecifier	*endianModeList;
	UniParListPtr	parList;
	BOOLN	inputMode;
	char	*titleString;
	double	normalise;			/* Set for scaling: AIFF, Raw, MS Wave etc. */
	sf_count_t	timeOffsetIndex;
	sf_count_t	timeOffsetCount;
	sf_count_t	maxSamples;		/* This can be set to restrict the data size. */
	sf_count_t	numSamples;		/* Set for modes which may lose previous value. */
	FileFormatSpecifier	type;
#	if HAVE_SNDFILE
		SF_INFO	sFInfo;
		SNDFILE	*sndFile;
		SF_VIRTUAL_IO	*vIOFuncs;
		DFVirtualIOPtr	vIOPtr;
#	endif /* HAVE_SNDFILE */
	double	*buffer;

} DataFile, *DataFilePtr;


/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

extern	DataFilePtr	dataFilePtr;
/* extern	int32		maxSamples; */

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckParsRead_DataFile(void);

NameSpecifier *	DSAMFormatList_DataFile(int index);

ChanLen	FileLength_DataFile(FILE *fp);

FileFormatSpecifier		Format_DataFile(WChar *formatExtension);

BOOLN	Free_DataFile(void);

void	FreeBuffer_DataFile(void);

void	FreeProcessVariables_DataFile(void);

void	FreeSoundFormatLists_DataFile(void);

int		GetSndFormat_DataFile(int specifier);

int		GetSndSubFormat_DataFile(int specifier);

UniParListPtr	GetUniParListPtr_DataFile(void);

BOOLN	InitSubFormatTypeList_DataFile(void);

BOOLN	Init_DataFile(ParameterSpecifier parSpec);

BOOLN	InitBuffer_DataFile(SignalDataPtr signal, const WChar *callingFunction);

BOOLN	InitEndianModeList_DataFile(void);

BOOLN	InitModule_DataFile(ModulePtr theModule);

BOOLN	InitProcessVariables_DataFile(EarObjectPtr data, sf_count_t length,
		  Float sampleRate);

int		NumberOfColumns_DataFile(FILE *fp);

FILE *	OpenFile_DataFile(WChar *fileName, char *mode);

BOOLN	PrintPars_DataFile(void);

int32	ReadFileIdentifier_DataFile(FILE *fp, int32 target, WChar *filetype);

ChanData	ReadSample_DataFile(FILE *fp);

BOOLN	ReadSignal_DataFile(WChar *fileName, EarObjectPtr data);

BOOLN	ReadSignal_DataFile_Named(EarObjectPtr data);

BOOLN	ReadSignalMain_DataFile(WChar *fileName, EarObjectPtr data);

BOOLN	SetDefaultSampleRate_DataFile(Float theDefaultSampleRate);

BOOLN	SetDuration_DataFile(Float theDuration);

BOOLN	SetEndian_DataFile(WChar *endian);

BOOLN	SetFileName_DataFile(WChar *fileName);

BOOLN	SetGain_DataFile(Float theGain);

sf_count_t	SetIOSectionLength_DataFile(EarObjectPtr data);

BOOLN	SetNumChannels_DataFile(int numChannels);

BOOLN	SetNormalisation_DataFile(Float normalisation);

BOOLN	SetParsPointer_DataFile(ModulePtr theModule);

void	SetRWFormat_DataFile(int endian);

BOOLN	SetSubFormatType_DataFile(WChar * theSubFormatType);

BOOLN	SetTimeOffset_DataFile(Float timeOffset);

BOOLN	SetUniParList_DataFile(void);

BOOLN	SetWordSize_DataFile(int wordSize);

BOOLN	WriteOutSignal_DataFile(WChar *fileName, EarObjectPtr data);

BOOLN	WriteOutSignal_DataFile_Named(EarObjectPtr data);

BOOLN	WriteOutSignalMain_DataFile(WChar *fileName, EarObjectPtr data);

BOOLN	WriteSignal_DataFile(FILE *fp, SignalDataPtr signal);

__END_DECLS

#endif
