/**********************
 *
 * File:		IOAudioIn.h
 * Purpose:		Audio input from sound system.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 * Author:		L. P. O'Mard
 * Created:		08 Nov 2005
 * Updated:
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

#ifndef _IOAUDIOIN_H
#define _IOAUDIOIN_H 1

#if USE_PORTAUDIO

#include <portaudio.h>

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	IO_AUDIOIN_MOD_NAME			wxT("IO_AudioIn")
#define IO_AUDIOIN_NUM_PARS			7
#define	IO_AUDIOIN_SAMPLE_FORMAT	paFloat32
#define IO_AUDIOIN_SAMPLE_SILENCE	0.0f

#ifdef paUseHostApiSpecificDeviceSpecification
#	define IO_AUDIOIN_PORTAUDIO_V_19		1
#else
#	define paComplete						1
#	define paContinue						0
#	define paFramesPerBufferUnspecified		(0)
#endif

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

#ifndef IO_AUDIOIN_PORTAUDIO_V_19

typedef Float PaTime;
typedef int PaDeviceIndex;
typedef unsigned long PaSampleFormat;
typedef struct PaStreamParameters {

	PaDeviceIndex device;
	int		channelCount;
    PaSampleFormat	sampleFormat;
	PaTime	suggestedLatency;
	void	*hostApiSpecificStreamInfo;

} PaStreamParameters;
#endif

typedef	float AudioInSample;

typedef enum {

	IO_AUDIOIN_DEVICEID,
	IO_AUDIOIN_NUMCHANNELS,
	IO_AUDIOIN_SEGMENTSPERBUFFER,
	IO_AUDIOIN_SAMPLERATE,
	IO_AUDIOIN_DURATION,
	IO_AUDIOIN_SLEEP,
	IO_AUDIOIN_GAIN

} AudioInParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		deviceID;
	int		numChannels;
	int		segmentsPerBuffer;
	Float	sampleRate;
	Float	duration;
	Float	sleep;
	Float	gain;

	/* Private members */
	UniParListPtr	parList;
	BOOLN	portAudioInitialised;
	BOOLN	streamStarted;
	BOOLN	segmentReadyFlag;
	long		frameIndex;
	long		segmentIndex;
	PaError	pAError;
	PaStream	*stream;
	EarObjectPtr	buffer;
	EarObjectPtr	data;

} AudioIn, *AudioInPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	AudioInPtr	audioInPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IO_AudioIn(EarObjectPtr data);

BOOLN	FreeProcessVariables_IO_AudioIn(void);

BOOLN	Free_IO_AudioIn(void);

UniParListPtr	GetUniParListPtr_IO_AudioIn(void);

BOOLN	InitModule_IO_AudioIn(ModulePtr theModule);

BOOLN	InitProcessVariables_IO_AudioIn(EarObjectPtr data);

BOOLN	Init_IO_AudioIn(ParameterSpecifier parSpec);

void	NotifyError_IO_AudioIn(WChar *format, ...);

BOOLN	PrintPars_IO_AudioIn(void);

BOOLN	ReadSignal_IO_AudioIn(EarObjectPtr data);

#ifdef	IO_AUDIOIN_PORTAUDIO_V_19
int	RecordCallback_IO_AudioIn(const void *inputBuffer,
			  void *outputBuffer, unsigned long framesPerBuffer,
			  const PaStreamCallbackTimeInfo* timeInfo,
			  PaStreamCallbackFlags statusFlags, void *userData);
#else
int	RecordCallback_IO_AudioIn(void *inputBuffer,
			  void *outputBuffer, unsigned long framesPerBuffer,
			  PaTimestamp outTime, void *userData);
#endif /* IO_AUDIOIN_PORTAUDIO_V_19 */

void	ResetBuffer_IO_AudioIn(void);

BOOLN	SetDeviceID_IO_AudioIn(int theDeviceID);

BOOLN	SetDuration_IO_AudioIn(Float theDuration);

BOOLN	SetGain_IO_AudioIn(Float theGain);

BOOLN	SetNumChannels_IO_AudioIn(int theNumChannels);

BOOLN	SetParsPointer_IO_AudioIn(ModulePtr theModule);

BOOLN	SetSampleRate_IO_AudioIn(Float theSampleRate);

BOOLN	SetSegmentsPerBuffer_IO_AudioIn(int theSegmentsPerBuffer);

BOOLN	SetSleep_IO_AudioIn(Float theSleep);

BOOLN	SetUniParList_IO_AudioIn(void);

__END_DECLS

#endif /* USE_PORTAUDIO */
#endif
