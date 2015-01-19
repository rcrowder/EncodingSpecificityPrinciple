/**********************
 *
 * File:		IOAudioIn.c
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#if USE_PORTAUDIO

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "IOAudioIn.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AudioInPtr	audioInPtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_IO_AudioIn(void)
{
	if (audioInPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IO_AudioIn();
	if (audioInPtr->parList)
		FreeList_UniParMgr(&audioInPtr->parList);
	if (audioInPtr->parSpec == GLOBAL) {
		free(audioInPtr);
		audioInPtr = NULL;
	}
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This function initialises the module by setting module's parameter
 * pointer structure.
 * The GLOBAL option is for hard programming - it sets the module's
 * pointer to the global parameter structure and initialises the
 * parameters. The LOCAL option is for generic programming - it
 * initialises the parameter structure currently pointed to by the
 * module's parameter pointer.
 */

BOOLN
Init_IO_AudioIn(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IO_AudioIn");

	if (parSpec == GLOBAL) {
		if (audioInPtr != NULL)
			Free_IO_AudioIn();
		if ((audioInPtr = (AudioInPtr) malloc(sizeof(AudioIn))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (audioInPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	audioInPtr->parSpec = parSpec;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->deviceID = -1;
	audioInPtr->numChannels = 1;
	audioInPtr->sampleRate = 44100.0;
	audioInPtr->duration = 10e-3;
	audioInPtr->segmentsPerBuffer = 10;
	audioInPtr->sleep = 0.0;
	audioInPtr->gain = 0.0;

	if (!SetUniParList_IO_AudioIn()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IO_AudioIn();
		return(FALSE);
	}
	audioInPtr->portAudioInitialised = FALSE;
	audioInPtr->pAError = paNoError;
	audioInPtr->stream = NULL;
	audioInPtr->buffer = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IO_AudioIn(void)
{
	static const WChar *funcName = wxT("SetUniParList_IO_AudioIn");
	UniParPtr	pars;

	if ((audioInPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IO_AUDIOIN_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = audioInPtr->parList->pars;
	SetPar_UniParMgr(&pars[IO_AUDIOIN_DEVICEID], wxT("DEVICE_ID"),
	  wxT("Device ID/number (int)."),
	  UNIPAR_INT,
	  &audioInPtr->deviceID, NULL,
	  (void * (*)) SetDeviceID_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_NUMCHANNELS], wxT("NUM_CHANNELS"),
	  wxT("Number of input channels, i.e. 1 = mono, 2 = stereo (int.)"),
	  UNIPAR_INT,
	  &audioInPtr->numChannels, NULL,
	  (void * (*)) SetNumChannels_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_SEGMENTSPERBUFFER], wxT(
	  "SEGMENTS_PER_BUFFER"),
	  wxT("Main input buffer length: multiples of the (segment) duration ")
	    wxT("(int)."),
	  UNIPAR_INT,
	  &audioInPtr->segmentsPerBuffer, NULL,
	  (void * (*)) SetSegmentsPerBuffer_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_SAMPLERATE], wxT("SAMPLE_RATE"),
	  wxT("Input sample rate = 1 / sampling interval (Hz)."),
	  UNIPAR_REAL,
	  &audioInPtr->sampleRate, NULL,
	  (void * (*)) SetSampleRate_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_DURATION], wxT("DURATION"),
	  wxT("Input duration or segment duration (s)."),
	  UNIPAR_REAL,
	  &audioInPtr->duration, NULL,
	  (void * (*)) SetDuration_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_SLEEP], wxT("SLEEP"),
	  wxT("Sleep time between process reads (ms)."),
	  UNIPAR_REAL,
	  &audioInPtr->sleep, NULL,
	  (void * (*)) SetSleep_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_GAIN], wxT("GAIN"),
	  wxT("Signal gain (dB)."),
	  UNIPAR_REAL,
	  &audioInPtr->gain, NULL,
	  (void * (*)) SetGain_IO_AudioIn);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IO_AudioIn(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (audioInPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(audioInPtr->parList);

}

/****************************** SetDeviceID ***********************************/

/*
 * This function sets the module's deviceID parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeviceID_IO_AudioIn(int theDeviceID)
{
	static const WChar	*funcName = wxT("SetDeviceID_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->deviceID = theDeviceID;
	return(TRUE);

}

/****************************** SetNumChannels ********************************/

/*
 * This function sets the module's numChannels parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumChannels_IO_AudioIn(int theNumChannels)
{
	static const WChar	*funcName = wxT("SetNumChannels_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((theNumChannels < 1 ) || (theNumChannels > 2)) {
		NotifyError(wxT("%s: The number of channels must be 1 or 2."),
		  funcName);
		return(FALSE);
	}
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->numChannels = theNumChannels;
	return(TRUE);

}

/****************************** SetSampleRate *********************************/

/*
 * This function sets the module's sampleRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSampleRate_IO_AudioIn(Float theSampleRate)
{
	static const WChar	*funcName = wxT("SetSampleRate_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->sampleRate = theSampleRate;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_IO_AudioIn(Float theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSegmentsPerBuffer **************************/

/*
 * This function sets the module's segmentsPerBuffer parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSegmentsPerBuffer_IO_AudioIn(int theSegmentsPerBuffer)
{
	static const WChar	*funcName = wxT("SetSegmentsPerBuffer_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->segmentsPerBuffer = theSegmentsPerBuffer;
	return(TRUE);

}

/****************************** SetSleep **************************************/

/*
 * This function sets the module's sleep parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSleep_IO_AudioIn(Float theSleep)
{
	static const WChar	*funcName = wxT("SetSleep_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->sleep = theSleep;
	return(TRUE);

}

/****************************** SetGain ***************************************/

/*
 * This function sets the module's gain parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGain_IO_AudioIn(Float theGain)
{
	static const WChar	*funcName = wxT("SetGain_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->gain = theGain;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_IO_AudioIn(void)
{
	static const WChar	*funcName = wxT("PrintPars_IO_AudioIn");

	if (audioInPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Audio Module Parameters:-\n"));
	DPrint(wxT("\tdeviceID = %d ??\n"), audioInPtr->deviceID);
	DPrint(wxT("\tnumChannels = %d ??\n"), audioInPtr->numChannels);
	DPrint(wxT("\tsegmentsPerBuffer = %g ??\n"), audioInPtr->segmentsPerBuffer);
	DPrint(wxT("\tsampleRate = %g ??\n"), audioInPtr->sampleRate);
	DPrint(wxT("\tduration = %g ??\n"), audioInPtr->duration);
	DPrint(wxT("\tsleep = %g ??\n"), audioInPtr->sleep);
	DPrint(wxT("\tgain = %g ??\n"), audioInPtr->gain);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IO_AudioIn(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IO_AudioIn");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	audioInPtr = (AudioInPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_IO_AudioIn(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IO_AudioIn");

	if (!SetParsPointer_IO_AudioIn(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IO_AudioIn(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = audioInPtr;
	theModule->Free = Free_IO_AudioIn;
	theModule->GetUniParListPtr = GetUniParListPtr_IO_AudioIn;
	theModule->PrintPars = PrintPars_IO_AudioIn;
	theModule->RunProcess = ReadSignal_IO_AudioIn;
	theModule->SetParsPointer = SetParsPointer_IO_AudioIn;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

BOOLN
CheckData_IO_AudioIn(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IO_AudioIn");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** NotifyError ***********************************/

/*
 * This routine returns PortAudio specific error messages.
 */

void
NotifyError_IO_AudioIn(WChar *format, ...)
{
	WChar	msg[LONG_STRING];
	va_list	args;

	va_start(args, format);
	DSAM_vsnprintf(msg, LONG_STRING, format, args);
	va_end(args);
	NotifyError(wxT("%s (PortAudioError [%d]: %s)\n"), msg, audioInPtr->pAError,
	  Pa_GetErrorText(audioInPtr->pAError));

}

/****************************** ResetBuffer ***********************************/

/*
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may be called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */

void
ResetBuffer_IO_AudioIn(void)
{
	AudioInPtr p = audioInPtr;

	p->frameIndex = 0;
	p->segmentIndex = 0;

}

/****************************** RecordCallback ********************************/

/*
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may be called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */


#if	IO_AUDIOIN_PORTAUDIO_V_19
int
RecordCallback_IO_AudioIn(const void *inputBuffer, void *outputBuffer,
  unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags, void *userData)
#else
int
RecordCallback_IO_AudioIn(void *inputBuffer, void *outputBuffer,
  unsigned long framesPerBuffer, PaTimestamp outTime, void *userData)
#endif /* IO_AUDIOIN_PORTAUDIO_V_19 */
{
	/*static const WChar *funcName = wxT("RecordCallback_IO_AudioIn");*/
	long	i, framesToCalc;

	AudioInPtr p = (AudioInPtr) userData;
	const AudioInSample	*inPtr = (const AudioInSample *) inputBuffer;
	ChanData	*outPtr[2] = { NULL, NULL };
	SignalDataPtr	bufSignal = p->buffer->outSignal;
    unsigned long framesLeft = bufSignal->length - p->frameIndex;

    framesToCalc = (framesLeft < framesPerBuffer)? framesLeft: framesPerBuffer;
	outPtr[LEFT_CHAN] = bufSignal->channel[LEFT_CHAN] + p->frameIndex;
	if (bufSignal->numChannels == 2)
		outPtr[RIGHT_CHAN] = bufSignal->channel[RIGHT_CHAN] + p->frameIndex;
	if (inputBuffer) {
		for (i = 0; i < framesToCalc; i++) {
			*outPtr[LEFT_CHAN]++ = (ChanData) *inPtr++;
			if (bufSignal->numChannels == 2)
				*outPtr[RIGHT_CHAN]++ = (ChanData) *inPtr++;
		}
	} else {
		for (i = 0; i < framesToCalc; i++) {
			*outPtr[LEFT_CHAN]++ = (ChanData) IO_AUDIOIN_SAMPLE_SILENCE;
			if (bufSignal->numChannels == 2)
				*outPtr[RIGHT_CHAN]++ = (ChanData) IO_AUDIOIN_SAMPLE_SILENCE;
		}
	}
	/*DSAM_printf(wxT("RecordCallback_IO_AudioIn: Debug: frameIndex = %ld,")
	  wxT(" %ld\n"), p->frameIndex, p->segmentIndex);*/
	p->frameIndex += framesToCalc;
	if ((ChanLen) (p->frameIndex - p->segmentIndex) >= _OutSig_EarObject(p->
	  data)->length) {
		/*printf(wxT("RecordCallback_IO_AudioIn: Debug: Got segment, length = ")
		 wxT("%d.\n"), p->frameIndex - p->segmentIndex);*/
		p->segmentReadyFlag = TRUE;
	}
	/*if (p->frameIndex <= p->segmentIndex) {
		NotifyError(wxT("RecordCallback_IO_AudioIn: Debug: Buffer pointers ")
		  wxT("colliding.\n"));
		return(paComplete);
	}*/
	if ((ChanLen) p->frameIndex >= bufSignal->length)
		p->frameIndex = 0;
	return(paContinue);

}


/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_IO_AudioIn(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_IO_AudioIn");
	PaStreamParameters  inputParameters;
	AudioInPtr p = audioInPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_IO_AudioIn();
		if ((p->buffer = Init_EarObject(wxT("NULL"))) == NULL) {
			NotifyError(wxT("%s: Could not initialise previous data EarObject"),
			  funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(p->buffer, _OutSig_EarObject(data)->
		  numChannels, _OutSig_EarObject(data)->length * p->segmentsPerBuffer,
		  _OutSig_EarObject(data)->dt)) {
			NotifyError(wxT("%s: Cannot initialise channels for previous ")
			  wxT("data."), funcName);
			return(FALSE);
		}
		p->data = data;
		if ((p->pAError = Pa_Initialize()) != paNoError) {
			NotifyError_IO_AudioIn(wxT("%s: Could not initialise PortAudio"),
			  funcName);
			return(FALSE);
		}
		p->portAudioInitialised = TRUE;
#		ifdef IO_AUDIOIN_PORTAUDIO_V_19
		inputParameters.device = (p->deviceID < 0)? Pa_GetDefaultInputDevice():
		  p->deviceID;
		if (inputParameters.device == paNoDevice) {
			NotifyError_IO_AudioIn(wxT("%s: Input device not available."),
			  funcName);
			return(FALSE);
		}
		inputParameters.suggestedLatency = Pa_GetDeviceInfo(
		  inputParameters.device )->defaultLowInputLatency;
#		else
		inputParameters.device = (p->deviceID < 0)? Pa_GetDefaultInputDeviceID(
		  ): p->deviceID;
		inputParameters.suggestedLatency = 0.0;
#		endif
		inputParameters.channelCount = p->numChannels;
		inputParameters.sampleFormat = IO_AUDIOIN_SAMPLE_FORMAT;
		inputParameters.hostApiSpecificStreamInfo = NULL;
#		ifdef IO_AUDIOIN_PORTAUDIO_V_19
		(p->pAError = Pa_OpenStream(&p->stream, &inputParameters, NULL,
		  p->sampleRate, paFramesPerBufferUnspecified, paClipOff,
		  RecordCallback_IO_AudioIn, p));
#		else
		(p->pAError = Pa_OpenStream(&p->stream, inputParameters.device,
		  inputParameters.channelCount, inputParameters.sampleFormat, NULL,
		  paNoDevice, 0, inputParameters.sampleFormat, NULL, p->sampleRate,
		  paFramesPerBufferUnspecified, 0, paClipOff, RecordCallback_IO_AudioIn,
		  p));
#		endif
		if ( p->pAError != paNoError) {
			NotifyError_IO_AudioIn(wxT("%s: Could not open stream"), funcName);
			return(FALSE);
		}
		p->frameIndex = 0;
		p->segmentIndex = 0;
		if ((p->pAError = Pa_StartStream(p->stream)) != paNoError) {
			NotifyError_IO_AudioIn(wxT("%s: Could not start PortAudio stream"),
			  funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		p->segmentReadyFlag = FALSE;
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_IO_AudioIn(void)
{
	static const WChar	*funcName = wxT("FreeProcessVariables_IO_AudioIn");
	AudioInPtr p = audioInPtr;

	Free_EarObject(&p->buffer);
	if (p->stream) {
		if ((p->pAError = Pa_CloseStream(p->stream)) != paNoError) {
			NotifyError_IO_AudioIn(wxT("%s: Could not close PortAudio stream"),
			  funcName);
		}
	}
	if (p->portAudioInitialised)
		Pa_Terminate();
	return(TRUE);

}

/****************************** ReadSignal ************************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if
 * required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used.
 */

BOOLN
ReadSignal_IO_AudioIn(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("ReadSignal_IO_AudioIn");
	BOOLN	ok = TRUE;
	AudioInPtr p = audioInPtr;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_IO_AudioIn(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Audio input module process"));
		data->externalDataFlag = TRUE;
		if (!InitOutSignal_EarObject(data, (uShort) p->numChannels, (ChanLen)
		  floor(p->duration * p->sampleRate + 0.5), 1.0 / p->sampleRate)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		_OutSig_EarObject(data)->rampFlag = TRUE;
		if (!InitProcessVariables_IO_AudioIn(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	do {
#		ifdef IO_AUDIOIN_PORTAUDIO_V_19
		p->pAError = Pa_IsStreamActive(p->stream);
#		else
		p->pAError = Pa_StreamActive(p->stream);
#		endif
		if (p->sleep > 0)
			Pa_Sleep((int) p->sleep);
	} while (!p->segmentReadyFlag && (p->pAError == 1));
	outSignal->channel[LEFT_CHAN] = p->buffer->outSignal->channel[
	  LEFT_CHAN] + p->segmentIndex;
	if (p->buffer->outSignal->numChannels == 2)
		outSignal->channel[RIGHT_CHAN] = p->buffer->outSignal->channel[
		  RIGHT_CHAN] + p->segmentIndex;
	p->segmentIndex += outSignal->length;
	if ((ChanLen) p->segmentIndex > p->buffer->outSignal->length)
		p->segmentIndex = 0;
	p->segmentReadyFlag = FALSE;
	if ((p->pAError < 0) && !GetDSAMPtr_Common()->segmentedMode)
		NotifyError_IO_AudioIn(wxT("%s: Failed to record sound."), funcName);
	if (p->pAError < 1)
		ok = FALSE;
	if (ok && (fabs(p->gain) > DSAM_EPSILON))
		GaindB_SignalData(outSignal, p->gain);
	SetProcessContinuity_EarObject(data);
	return(ok);

}

#endif /* USE_PORTAUDIO */
