/******************
 *
 * File:		FiSndFile.c
 * Purpose:		This Filing reads sound format files using the libsndfile
 * 				library.
 * Comments:
 * Authors:		L. P. O'Mard
 * Created:		07 Nov 2006
 * Updated:
 * Copyright:	(c) 2006, 2010 Lowel P. O'Mard
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#if HAVE_SNDFILE
#	include <sndfile.h>
#endif

/******************************************************************************/
/****************************** HAVE_SNDFILE compile **************************/
/******************************************************************************/

#if	HAVE_SNDFILE

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtString.h"
#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiSndFile.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/**************************** Free ********************************************/

/*
 * Free and close the sound file.
 */

void
Free_SndFile(void)
{
	static const WChar *funcName = wxT("Free_SndFile");
	int error;
	DataFilePtr p = dataFilePtr;

	if (!p->sndFile)
		return;
	if ((error = sf_close(p->sndFile)) != 0)
		NotifyError(wxT("%s: Could not close file: %s"), funcName,
		  MBSToWCS_Utility_String(sf_error_number(error)));
	p->sndFile = NULL;

}

/**************************** GetWordSize *************************************/

/*
 * Returns the sound format word size.
 */

int
GetWordSize_SndFile(int format)
{
	static const WChar *funcName = wxT("GetWordSize_SndFile");
	int		subType = format & 0xFF;

	if (subType < 5)
		return(subType);
	if (subType & SF_FORMAT_PCM_U8)
		return(1);
	else if (subType & SF_FORMAT_FLOAT)
		return(4);
	else if (subType & SF_FORMAT_DOUBLE)
		return(8);

	NotifyError(wxT("Undefined format (%d)."), funcName, subType);
	return(-1);

}

/**************************** FreeVirtualIOMemory *****************************/

/*
 * This routine frees the memory allocated for memory reading.
 */

DSAM_API void
FreeVirtualIOMemory_SndFile(DFVirtualIOPtr *p)
{
	if (*p == NULL)
		return;
	if ((*p)->data != NULL)
		free((*p)->data);
	free((*p));
	(*p) = NULL;

}

/**************************** ResetVirtualIOMemory *****************************/

/*
 * This routine sets the memory offset and length to zero.
 */

void
ResetVirtualIOMemory_SndFile(DFVirtualIOPtr p)
{
	p->offset = 0;
	p->length = 0;
}

/**************************** InitVirtualIOMemory *****************************/

/*
 * This routine initialises the memory pointers module parameters to values.
 * It returns a pointer to the structures memory if successful, otherwise it
 * returns NULL.
 */

DSAM_API BOOLN
InitVirtualIOMemory_SndFile(DFVirtualIOPtr *p, sf_count_t maxLength)
{
	static const WChar *funcName = wxT("InitMemory_SndFile");

	if (!(*p)) {
		if (((*p) = (DFVirtualIOPtr) malloc(sizeof(DFVirtualIO))) == NULL) {
			NotifyError(wxT("%s: Cannot allocate memoryfor virtual IO memory ")
			  wxT("pointer."), funcName);
			return(FALSE);
		}
		(*p)->data = NULL;
	}
	if ((*p)->maxLength != maxLength) {
		if ((*p)->data)
			free((*p)->data);
		if (((*p)->data = (unsigned char *) malloc((size_t) maxLength)) == NULL) {
			NotifyError(wxT("%s: Cannot allocate data memory."), funcName);
			free((*p));
			*p = NULL;
			return(FALSE);
		}
	}
	ResetVirtualIOMemory_SndFile(*p);
	(*p)->maxLength = maxLength;
	return(TRUE);

}

/**************************** VirtualIOGetFileLen *****************************/

static sf_count_t
VirtualIOGetFileLen_SndFile(void *user_data)
{
	DFVirtualIOPtr vf = (DFVirtualIOPtr) user_data ;

	return vf->length;

}

/**************************** VirtualIOSeek ***********************************/

static sf_count_t
VirtualIOSeek_SndFile(sf_count_t offset, int whence, void *user_data)
{
	DFVirtualIOPtr vf = (DFVirtualIOPtr) user_data ;

	switch (whence) {
	case SEEK_SET :
		vf->offset = offset ;
		break ;
	case SEEK_CUR :
		vf->offset = vf->offset + offset ;
		break ;
	case SEEK_END :
		vf->offset = vf->length + offset ;
		break ;
	default :
		break ;
	}

	return vf->offset ;
}

/**************************** VirtualIORead ***********************************/

static sf_count_t
VirtualIORead_SndFile(void *ptr, sf_count_t count, void *user_data)
{
	DFVirtualIOPtr vf = (DFVirtualIOPtr) user_data ;

	/*
	**	This will break badly for files over 2Gig in length, but
	**	is sufficient for testing.
	*/
	if (vf->offset + count > vf->length)
		count = vf->length - vf->offset ;

	memcpy (ptr, vf->data + vf->offset, (size_t) count) ;
	vf->offset += count ;

	return count;

}

/**************************** VirtualIOWrite **********************************/

static sf_count_t
VirtualIOWrite_SndFile(const void *ptr, sf_count_t count, void *user_data)
{
	DFVirtualIOPtr vf = (DFVirtualIOPtr) user_data ;
	/*
	**	This will break badly for files over 2Gig in length, but
	**	is sufficient for testing.
	*/
	if (vf->offset >= vf->maxLength)
		return 0 ;

	if (vf->offset + count > vf->maxLength)
		count = sizeof (vf->data) - vf->offset ;

	memcpy (vf->data + vf->offset, ptr, (size_t) count) ;
	vf->offset += count ;

	if (vf->offset > vf->length)
		vf->length = vf->offset ;

	return count ;

}

/**************************** VirtualIOTell ***********************************/

static sf_count_t
VirtualIOTell_SndFile(void *user_data)
{
	DFVirtualIOPtr vf = (DFVirtualIOPtr) user_data ;

	return vf->offset ;
} /* vftell */

/**************************** InitVirtualIO ***********************************/

BOOLN
InitVirtualIO_SndFile(void)
{
	static const WChar *funcName = wxT("InitVirtualIO_SndFile");
	DataFilePtr	p = dataFilePtr;

	if (p->vIOFuncs)
		return(TRUE);
	if ((p->vIOFuncs = (SF_VIRTUAL_IO *) malloc(sizeof(SF_VIRTUAL_IO))) == NULL) {
		NotifyError(wxT("%s: Out of memory for virtual IO structure."),
		  funcName);
		return(FALSE);
	}
	p->vIOFuncs->get_filelen = VirtualIOGetFileLen_SndFile;
	p->vIOFuncs->seek = VirtualIOSeek_SndFile;
	p->vIOFuncs->read = VirtualIORead_SndFile;
	p->vIOFuncs->write = VirtualIOWrite_SndFile;
	p->vIOFuncs->tell = VirtualIOTell_SndFile;
	return(TRUE);

}

/**************************** DetermineFileSize *******************************/

/*
 * This routine determines the size of a file by a test writing of the file
 * format chosen to memory.
 */

sf_count_t
DetermineFileSize_SndFile(SignalDataPtr signal)
{
	static const WChar *funcName = wxT("DetermineFileSize_SndFile");
	int		i;
	double	a[][2] = {{1.0}, {1.0, 2.0}};
	sf_count_t	na[] = {1, 2}, length[2], headerSize, sampleSize;
	DataFilePtr	p = dataFilePtr;
	DFVirtualIOPtr	vIOPtr = NULL;
	SNDFILE *	fp;
	SF_INFO sFInfo = p->sFInfo;

	if (!InitVirtualIOMemory_SndFile(&vIOPtr, SND_FILE_TEST_FILE_MEMORY_SIZE)) {
		NotifyError(wxT("%s: Out of memory for test memory (%d)"), funcName,
		  SND_FILE_TEST_FILE_MEMORY_SIZE);
		return(0);
	}
	sFInfo.channels = 1;
	for (i = 0; i < 2; i++) {
		if ((fp = sf_open_virtual(p->vIOFuncs, SFM_WRITE, &sFInfo, vIOPtr)) ==
		  NULL) {
			NotifyError(wxT("%s: Couldn't open virtual data file: error '%s'"),
			  funcName, MBSToWCS_Utility_String(sf_strerror(NULL)));
			return(FALSE);
		}
		sf_set_string(fp, SF_STR_TITLE, p->titleString);
		sf_write_double(fp, a[i], na[i]);
		length[i] = vIOPtr->length;
		sf_close(fp);
		ResetVirtualIOMemory_SndFile(vIOPtr);
	}
	FreeVirtualIOMemory_SndFile(&vIOPtr);
	sampleSize = length[1] - length[0];
	headerSize = length[0] - sampleSize;
	return(headerSize + sampleSize * signal->length * signal->numChannels);

}

/**************************** OpenFile ****************************************/

/*
 * This function opens the interaction with the file.
 * It assumes if the sndFile field is not NULL then a file has been correctly
 * opened.
 * Because the LibSndFile library format has the sample rate as an integer, the
 * dt == DF value used by FFT output produces a zero sample rate.  The sample rate is
 * therefore set to "1" and a string value is provided.
 */

BOOLN
OpenFile_SndFile(WChar *fileName, int mode, SignalDataPtr signal)
{
	static const WChar *funcName = wxT("OpenFile_SndFile");
	WChar	*parFilePath;
	int		format, sampleRate;
	DataFilePtr	p = dataFilePtr;

	if (p->sndFile)
		Free_SndFile();
	format = GetSndFormat_DataFile(p->type);
	if ((format == SF_FORMAT_RAW) || (mode == SFM_WRITE)) {
		sampleRate = (int) floor(p->defaultSampleRate + 0.5);
		p->sFInfo.samplerate = (sampleRate > 0)? sampleRate: 1;
		p->sFInfo.channels = (signal)? signal->numChannels: p->numChannels;
		p->sFInfo.format = format | GetSndSubFormat_DataFile(
		  p->subFormatType);
		if (!sf_format_check(&p->sFInfo)) {
			NotifyError(wxT("%s: Illegal format for sound file."), funcName);
			return(FALSE);
		}
		if (mode == SFM_WRITE) {
			switch (p->endian) {
			case DATA_FILE_LITTLE_ENDIAN:
				p->sFInfo.format |= SF_ENDIAN_LITTLE;
				break;
			case DATA_FILE_BIG_ENDIAN:
				p->sFInfo.format |= SF_ENDIAN_BIG;
				break;
			case DATA_FILE_CPU_ENDIAN:
				p->sFInfo.format |= SF_ENDIAN_CPU;
				break;
			default:
				;
			}
		}
	}
	switch (*fileName) {
	case STDIN_STDOUT_FILE_DIRN:
		/*return((mode[0] == 'r')? stdin: stdout);*/
		NotifyWarning(wxT("%s: Pipes not yet implemented."), funcName);
		return(FALSE);
	case MEMORY_FILE_DIRN: /* Memory pointer */
		InitVirtualIO_SndFile();
		switch (mode) {
		case SFM_WRITE:
			if (!InitVirtualIOMemory_SndFile(&p->vIOPtr,
			  DetermineFileSize_SndFile(signal))) {
				NotifyError(wxT("%s: Could not initialise virtual memory"),
				  funcName);
				return(FALSE);
			}
			break;
		case SFM_READ:
			if (!p->vIOPtr) {
				NotifyError(wxT("%s: Memory not allocated for virtual IO ")
				  wxT("Reading."),
				  funcName);
				return(FALSE);
			}
			p->vIOPtr->offset = 0;
			break;
		default:
			NotifyError(wxT("%s: Mode not implemented (%d)."), funcName,
			  mode);
		}
		if ((p->sndFile = sf_open_virtual(p->vIOFuncs, mode, &p->sFInfo,
		  p->vIOPtr)) == NULL) {
			NotifyError(wxT("%s: Couldn't open virtual data file: error '%s'"),
			  funcName, MBSToWCS_Utility_String(sf_strerror(NULL)));
			return(FALSE);
		}
		return(TRUE);
	default:
		parFilePath = GetParsFileFPath_Common(fileName);
		if ((p->sndFile = sf_open(ConvUTF8_Utility_String(parFilePath), mode,
		  &p->sFInfo)) == NULL) {
			NotifyError(wxT("%s: Could not open file '%s' (%s)."), funcName,
			  fileName, MBSToWCS_Utility_String(sf_error_number(sf_error(
			  p->sndFile))));
			return(FALSE);
		}
	}
	return(TRUE);

}

/**************************** ReadFrames **************************************/

/*
 * This routine reads the data frames using a buffer for speed.
 */

BOOLN
ReadFrames_SndFile(SignalDataPtr outSignal, sf_count_t length)
{
	static const WChar *funcName = wxT("ReadFrames_SndFile");
	register ChanData	*outPtr;
	register double		*inPtr;
	int		chan;
	sf_count_t	count = 0, frames, i, bufferFrames;
	DataFilePtr	p = dataFilePtr;

	if (!InitBuffer_DataFile(outSignal, funcName))
		return(FALSE);
	bufferFrames = DATAFILE_BUFFER_FRAMES;
	while (count < length) {
		if ((length - count) < bufferFrames)
			bufferFrames = length - count;
		frames = sf_readf_double(p->sndFile, p->buffer, bufferFrames);
		if (!frames)
			break;
		for (chan = 0; chan < outSignal->numChannels; chan++) {
			outPtr = outSignal->channel[chan] + count;
			inPtr = p->buffer + chan;
			for (i = 0, inPtr = p->buffer + chan; i < frames; i++, inPtr +=
			  outSignal->numChannels)
				*outPtr++ = *inPtr * p->normalise;
		}
		count += frames;
	}
	if (count == length)
		return(TRUE);
	return(FALSE);

}

/**************************** ParseTitleString *********************************/

/*
 * This function parses a title string extracting any valid settings.
 */

void
ParseTitleString_SndFile(const char *titleString, SignalDataPtr signal)
{
	char	*p, *token, *parName, *parValue;

	if (!titleString)
		return;
	for (token = strtok((char *) titleString, SND_FILE_FORMAT_DELIMITERS); token;
	  token = strtok(NULL, SND_FILE_FORMAT_DELIMITERS)) {
		if ((p = strchr(token, SND_FILE_FORMAT_PAR_SEPARATOR)) == NULL)
			continue;
		*p = '\0';
		parName = token;
		parValue = p + 1;
		if (isdigit(*parName)) {	/* Assume channel label */
			int		chan = (int) strtol(parName, &p, 10);
			if ((chan < 0) || (chan > signal->numChannels))
				continue;
			signal->info.chanLabel[chan] = strtod(parValue, &p);
		} else {
			switch (Identify_NameSpecifier(MBSToWCS_Utility_String(parName),
			  DSAMFormatList_DataFile(0))) {
			case DATA_FILE_INTERLEAVELEVEL:
				SetInterleaveLevel_SignalData(signal, (uShort) strtol(parValue,
				  &p, 10));
				break;
			case DATA_FILE_NUMWINDOWFRAMES:
				SetNumWindowFrames_SignalData(signal, (uShort) strtol(parValue,
				  &p, 10));
				break;
			case DATA_FILE_STATICTIMEFLAG:
				SetStaticTimeFlag_SignalData(signal, (BOOLN) strtol(parValue,
				  &p, 10));
				break;
			case DATA_FILE_OUTPUTTIMEOFFSET:
				SetOutputTimeOffset_SignalData(signal, strtod(parValue, &p));
				break;
			case DATA_FILE_NORMALISATION:
				dataFilePtr->normalise = strtod(parValue, &p);
				break;
			case DATA_FILE_DSAMVERSION:
				break;
			case DATA_FILE_LARGEDT: {
				double largeDt = strtod(parValue, &p);
				if (largeDt > 0.0)
					signal->dt = largeDt;
				break; }
			default:
				;
			}
		}
	}

}

/**************************** ReadFile ****************************************/

/*
 * This function reads a file in Microsoft Wave format.
 * The data is stored in the output signal of an EarObject which will have
 * one channel.
 * This is the default file, and is assumed if no suffix is given
 * If a dash, '-.suffix', is given as the file name, then the data will be
 * read from the standard input.
 * I am not quite certain about the stereo format.  At present it will assume
 * that if there are two channels, then it is a stereo signal.
 * It returns TRUE if successful.
 */

BOOLN
ReadFile_SndFile(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("ReadFile_SndFile");
	BOOLN	ok = TRUE;
	const char *titleString;
	sf_count_t	length;
	DataFilePtr	p = dataFilePtr;
	SignalDataPtr	outSignal;

	if (!OpenFile_SndFile(fileName, SFM_READ, NULL)) {
		NotifyError(wxT("%s: Could not open file '%s'\n"), funcName,
		  fileName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, wxT("Read '%s' Sound data file"),
	  GetFileNameFPath_Utility_String(fileName));
	if (!GetDSAMPtr_Common()->segmentedMode || (data->timeIndex ==
	  PROCESS_START_TIME)) {
	  	p->subFormatType = p->sFInfo.format & SF_FORMAT_SUBMASK;
		p->numChannels = p->sFInfo.channels;
		p->defaultSampleRate = p->sFInfo.samplerate;
		p->wordSize = GetWordSize_SndFile(p->sFInfo.format);
		p->numSamples = p->sFInfo.frames;
		if (!InitProcessVariables_DataFile(data, p->numSamples, p->sFInfo.
		  samplerate)) {
			NotifyError(wxT("%s: Could not initialise process variables."),
			  funcName);
			return(FALSE);
		}
	}
	if ((length = SetIOSectionLength_DataFile(data)) <= 0)
		return(FALSE);
	if (!InitOutSignal_EarObject(data, (uShort) p->numChannels, (ChanLen) length,
	  1.0 / p->defaultSampleRate)) {
		NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
		return(FALSE);
	}
	outSignal = _OutSig_EarObject(data);
	if (p->numChannels == 2)
		SetInterleaveLevel_SignalData(outSignal, 2);
	titleString = sf_get_string(p->sndFile, SF_STR_TITLE);
	ParseTitleString_SndFile(titleString, outSignal);
	sf_seek(p->sndFile, (int32) (data->timeIndex + p->timeOffsetCount),
	  SEEK_SET);
	ok = ReadFrames_SndFile(outSignal, length);
	Free_SndFile();
	return(ok);

}

/**************************** GetDuration *************************************/

/*
 * This function extracts the duration of the file in a signal by reading the
 * minimum of information from the file.
 * It returns a negative value if it fails to read the file duration.
 */

double
GetDuration_SndFile(WChar *fileName)
{
	static const WChar *funcName = wxT("GetDuration_SndFile");

	if (OpenFile_SndFile(fileName, SFM_READ, NULL)) {
		NotifyError(wxT("%s: Could not read initial file structure from '%s'."),
		  funcName, fileName);
		return(-1.0);
	}
	return((double) dataFilePtr->sFInfo.frames / dataFilePtr->sFInfo.
	  samplerate);
	Free_SndFile();

}

/**************************** WriteFrames **************************************/

/*
 * This routine writes the data frames using a buffer for speed.
 */

BOOLN
WriteFrames_SndFile(SignalDataPtr inSignal)
{
	static const WChar *funcName = wxT("WriteFrames_SndFile");
	register double *outPtr;
	register ChanData	*inPtr;
	int		chan;
	sf_count_t	count = 0, frames, i, bufferFrames;
	DataFilePtr	p = dataFilePtr;

	if (!InitBuffer_DataFile(inSignal, funcName))
		return(FALSE);
	bufferFrames = DATAFILE_BUFFER_FRAMES;
	while (count < (sf_count_t) inSignal->length) {
		if (((sf_count_t) inSignal->length - count) < bufferFrames)
			bufferFrames = inSignal->length - count;
		for (chan = 0; chan < inSignal->numChannels; chan++) {
			inPtr = inSignal->channel[chan] + count;
			for (i = bufferFrames, outPtr = p->buffer + chan; i--; outPtr +=
			  inSignal->numChannels)
				*outPtr = *inPtr++ / p->normalise;
		}
		frames = sf_writef_double(p->sndFile, p->buffer, bufferFrames);
		if (frames != bufferFrames) {
			NotifyError(wxT("%s: Failed to write sound data."), funcName);
			return(FALSE);
		}
		count += frames;
	}
	if (count == inSignal->length)
		return(TRUE);
	return(FALSE);

}

/**************************** CalculateNormalisation **************************/

/*
 * This routine calculates the normalisation factor for a signal.
 * It expects the signal to be correctly initialised.
 * It returns zero if the maximum value is 0.0.
 * It returns the normalisation argument value, if it is greater than zero in
 * non-auto mode.
 */

double
CalculateNormalisation_SndFile(SignalDataPtr signal)
{
	int		chan;
	ChanLen	i;
	ChanData	*dataPtr, maxValue;

	for (chan = signal->numChannels, maxValue = -DBL_MAX; chan-- ;) {
		dataPtr = signal->channel[chan];
		for (i = signal->length; i-- ; dataPtr++)
			if (fabs(*dataPtr) > maxValue)
				maxValue = fabs(*dataPtr);
	}
	if (maxValue < DSAM_EPSILON)
		return(1.0);
	return(maxValue);

}


/**************************** AddToString *************************************/

/*
 * Adds to a string, while checking that the string length is not exceeded.
 */

BOOLN
AddToString_SndFile(char *a, size_t *aLen, char *b, size_t maxLen)
{
	static const WChar *funcName = wxT("AddToString_SndFile");
	size_t	bLen = strlen(b);

	if ((bLen + *aLen) > maxLen) {
		NotifyError(wxT("%s: String too long for string concatenation (%u/%u)."),
		  funcName, bLen + *aLen, maxLen);
		return(FALSE);
	}
	strcat(a, b);
	*aLen += bLen;
	return(TRUE);

}

/**************************** CreateTitleString *******************************/

/*
 * This function creates and returns the title string.
 * The memory for this string must be "freed" by the calling program.
 */

char *
CreateTitleString_SndFile(SignalDataPtr signal)
{
	static const WChar *funcName = wxT("CreateTitleString_SndFile");
	char *s, *channelString, mainParString[LONG_STRING], workStr[MAXLINE];
	size_t	length, mainStringLen, maxChannelStringLen, channelStringLen;
	int		i;
	NameSpecifierPtr	list;
	DataFilePtr	p = dataFilePtr;

	mainStringLen = 0;
	mainParString[0] = '\0';
	for (list = DSAMFormatList_DataFile(0); list->name; list++) {
		if (!AddToString_SndFile(mainParString, &mainStringLen, " ", LONG_STRING))
			return(NULL);
		if (!AddToString_SndFile(mainParString, &mainStringLen,
		  ConvUTF8_Utility_String(list->name), LONG_STRING))
			return(NULL);
		switch (list->specifier) {
		case DATA_FILE_INTERLEAVELEVEL:
			sprintf(workStr, ":%d", signal->interleaveLevel);
			break;
		case DATA_FILE_NUMWINDOWFRAMES:
			sprintf(workStr, ":%d", signal->interleaveLevel);
			break;
		case DATA_FILE_STATICTIMEFLAG:
			sprintf(workStr, ":%d", signal->staticTimeFlag);
			break;
		case DATA_FILE_OUTPUTTIMEOFFSET:
			sprintf(workStr, ":%g", signal->outputTimeOffset);
			break;
		case DATA_FILE_NORMALISATION:
			sprintf(workStr, ":%.10g", p->normalise);
			break;
		case DATA_FILE_DSAMVERSION:
			sprintf(workStr, ":%s", ConvUTF8_Utility_String(DSAM_VERSION));
			break;
		case DATA_FILE_LARGEDT:
			sprintf(workStr, ":%.17g", (signal->dt > 1.0)? signal->dt: 0.0);
			break;
		default:
			NotifyError(wxT("%s: Unknown DSAM format specifier (%d)"),
			  funcName);
			return(NULL);
		};
		if (!AddToString_SndFile(mainParString, &mainStringLen, workStr,
		  LONG_STRING))
			return(NULL);
	}
	maxChannelStringLen = DATAFILE_CHANNEL_LABEL_SPACE * signal->numChannels;
	if ((channelString = (char *) malloc(maxChannelStringLen + 1)) == NULL) {
		NotifyError(wxT("%s: Out of memory for channelString (%d)."), funcName,
		  maxChannelStringLen);
		return(NULL);
	}
	channelStringLen = 0;
	*channelString = '\0';
	for (i = 0; i < signal->numChannels; i++) {
		sprintf(workStr, " %d:%.0f", i, signal->info.chanLabel[i]);
		if (!AddToString_SndFile(channelString, &channelStringLen, workStr,
		  maxChannelStringLen)) {
			free(channelString);
			return(NULL);
		}
	}
	length = mainStringLen + channelStringLen + 1;
	if ((s = (char *) malloc(length)) == NULL) {
		NotifyError(wxT("%s: Out of memory for string (%d)."), funcName,
		  length);
		return(NULL);
	}
	strcpy(s, mainParString);
	strcat(s, channelString);
	free(channelString);
	return(s);

}

/**************************** WriteFile ***************************************/

/*
 * This function writes the data from the output signal of an EarObject.
 * It returns FALSE if it fails in any way.
 * If the fileName is a dash, '-.suffix', then the file will be written to the
 * standard output.
 */

BOOLN
WriteFile_SndFile(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("WriteFile_SndFile");
	BOOLN	ok = TRUE;
	DataFilePtr	p = dataFilePtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	SetProcessName_EarObject(data, wxT("Output '%s' Sound data file"),
	  GetFileNameFPath_Utility_String(fileName));
	p->defaultSampleRate = 1.0 / outSignal->dt;
	if (!GetDSAMPtr_Common()->segmentedMode || (data->firstSectionFlag)) {
		if (p->titleString)
			free(p->titleString);
		dataFilePtr->normalise = (p->normalisation > 0.0)? p->normalisation:
		  CalculateNormalisation_SndFile(outSignal);
		if ((p->titleString = CreateTitleString_SndFile(outSignal)) == NULL)
			return(FALSE);
		if (!OpenFile_SndFile(fileName, SFM_WRITE, outSignal)) {
			NotifyError(wxT("%s: Could not open file '%s'\n"), funcName,
			  fileName);
			return(FALSE);
		}
		sf_set_string(p->sndFile, SF_STR_TITLE, p->titleString);
	} else {
		if (!OpenFile_SndFile(fileName, SFM_RDWR, outSignal)) {
			NotifyError(wxT("%s: Could not open file '%s'\n"), funcName,
			  fileName);
			return(FALSE);
		}
		if (sf_command(p->sndFile, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_TRUE) ==
		  0) {
 			NotifyError(wxT("%s: Failed to set auto update header: '%s'."),
 			  funcName, MBSToWCS_Utility_String(sf_strerror(p->sndFile)));
 			return(FALSE);
		}
		if ((p->sFInfo.channels != outSignal->numChannels) || (fabs(p->
		  sFInfo.samplerate - (1.0 / outSignal->dt)) > DATAFILE_NEGLIGIBLE_SR_DIFF) ||
		  ((p->sFInfo.format | SF_FORMAT_SUBMASK) != dataFilePtr->
		  subFormatType)) {
			NotifyError(wxT("%s: Cannot append to different format file!"),
			  funcName);
			return(FALSE);
		}
		sf_seek(p->sndFile, 0, SEEK_END);
	}
	ok = WriteFrames_SndFile(outSignal);
	free(p->titleString);
	p->titleString = NULL;
	Free_SndFile();
	return(ok);

}

#endif /* HAVE_SNDFILE */

