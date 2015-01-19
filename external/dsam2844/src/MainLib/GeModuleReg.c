/******************
 *
 * File:		GeModuleReg.c
 * Comments:	This module handles the register of all the library and user
 *				process modules.
 *				It is necssary to use a module specifier to identify
 *				non-standard process modules such as SIMSCRIPT_MODULE and the
 *				NULL_MODULE
 * Authors:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:
 * Copyright:	(c) 2001, 2010 Lowel P. O'Mard
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "DSAM.h"

/******************************************************************************/
/************************** Global Variables **********************************/
/******************************************************************************/

int			numUserModules, maxUserModules;
ModRegEntry	*userModuleList = NULL;

/******************************************************************************/
/************************** Subroutines and functions *************************/
/******************************************************************************/

/************************** LibraryList ***************************************/

/*
 * This routine returns an entry to the module library list.
 */

DSAM_API ModRegEntryPtr
LibraryList_ModuleReg(uShort index)
{
	static ModRegEntry	moduleList[] = {

	{ wxT("ANA_ACF"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_ACF },
	{ wxT("ANA_ALSR"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_ALSR },
	{ wxT("ANA_AVERAGES"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_Averages },
	{ wxT("ANA_CCF"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_CCF },
	{ wxT("ANA_CONVOLUTION"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_Convolution },
	{ wxT("ANA_FINDBIN"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_FindBin },
	{ wxT("ANA_FINDNEXTINDEX"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_FindNextIndex },
	{ wxT("ANA_FOURIERT"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_FourierT },
	{ wxT("ANA_HISTOGRAM"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_Histogram },
	{ wxT("ANA_INTENSITY"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_Intensity },
	{ wxT("ANA_ISIH"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_ISIH },
	{ ANALYSIS_SAC_MOD_NAME,
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_SAC },
	{ wxT("ANA_SAI"),
		ANALYSIS_MODULE_CLASS, ANA_SAI_MODULE,
		InitModule_Analysis_SAI },
	{ wxT("ANA_SYNCHINDEX"),
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_SynchIndex },
	{ ANALYSIS_SPIKEREGULARITY_MOD_NAME,
		ANALYSIS_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Analysis_SpikeRegularity },
	{ wxT("AN_SG_BINOMIAL"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_ANSpikeGen_Binomial },
	{ wxT("AN_SG_CARNEY"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_ANSpikeGen_Carney },
	{ wxT("AN_SG_MEDDIS02"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_ANSpikeGen_Meddis02 },
	{ wxT("AN_SG_SIMPLE"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_ANSpikeGen_Simple },
	{ wxT("BM_CARNEY"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_Carney },
	{ wxT("BM_COOKE"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_Cooke },
	{ wxT("BM_DRNL"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_DRNL },
	{ wxT("BM_GAMMAC"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_GammaChirp },
#	if HAVE_FFTW3
		{ BM_GC_DYN_MOD_NAME,
			MODEL_MODULE_CLASS, PROCESS_MODULE,
			InitModule_BasilarM_GammaChirp_Dyn },
#	endif
	{ wxT("BM_GAMMAT"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_GammaT },
	{ wxT("BM_ZHANG"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_Zhang },
	{ BM_ZILANYBRUCE_MOD_NAME,
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_BasilarM_ZilanyBruce },
	{ DATAFILE_IN_MOD_NAME,
		IO_MODULE_CLASS, DATAFILE_MODULE,
		InitModule_DataFile },
	{ DATAFILE_OUT_MOD_NAME,
		IO_MODULE_CLASS, DATAFILE_MODULE,
		InitModule_DataFile },
	{ wxT("DISPLAY_SIGNAL"),
		DISPLAY_MODULE_CLASS, DISPLAY_MODULE,
		InitModule_SignalDisp },
	{ wxT("FILT_BANDPASS"),
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_BandPass },
	{ FILTER_DIST_LOW_PASS_MOD_NAME,
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_DLowPass },
	{ wxT("FILT_FIR"),
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_FIR },
	{ FILTER_LOW_PASS_MOD_NAME,
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_LowPass },
	{ wxT("FILT_MULTIBPASS"),
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_MultiBPass },
	{ FILTER_ZBMIDDLEEAR_MOD_NAME,
		FILTER_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Filter_ZBMiddleEar },
	{ wxT("IHC_CARNEY"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Carney },
	{ wxT("IHC_COOKE91"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Cooke91 },
	{ wxT("IHC_MEDDIS86"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Meddis86 },
	{ wxT("IHC_MEDDIS86A"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Meddis86a },
	{ MEDDIS2000_MOD_NAME,
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Meddis2000 },
	{ wxT("IHC_ZHANG"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_Zhang },
	{ IHC_ZILANYBRUCE_MOD_NAME,
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHC_ZilanyBruce },
	{ wxT("IHCRP_CARNEY"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_Carney },
	{ wxT("IHCRP_MEDDIS"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_Meddis },
	{ IHCRP_LOPEZPOVEDA_MOD_NAME,
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_LopezPoveda },
	{ IHCRP_SHAMMA_MOD_NAME,
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_Shamma },
	{ IHCRP_SHAMMA3STATEVELIN_MOD_NAME,
		MODEL_MODULE_CLASS, PROCESS_MODULE,
	  InitModule_IHCRP_Shamma3StateVelIn },
	{ wxT("IHCRP_ZHANG"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IHCRP_Zhang },
#	if USE_PORTAUDIO
	{ IO_AUDIOIN_MOD_NAME,
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_IO_AudioIn },
#	endif /* USE_PORTAUDIO */
	{ wxT("NEUR_ARLEKIM"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Neuron_ArleKim },
	{ wxT("NEUR_HHUXLEY"),
		MODEL_MODULE_CLASS, NEUR_HHUXLEY_MODULE,
		InitModule_Neuron_HHuxley },
	{ wxT("NEUR_MCGREGOR"),
		MODEL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Neuron_McGregor },
	{ wxT("NULL"),
		CONTROL_MODULE_CLASS, NULL_MODULE,
		SetNull_ModuleMgr },
#	ifdef _PAMASTER1_H
	{ wxT("MPI_MASTER1"),
		CONTROL_MODULE_CLASS, PROCESS_MODULE,
		InitModule_MPI_Master1 },
#	endif
	{ wxT("STIM_CLICK"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Click },
	{ CMR_HARMONICMASKER_MOD_NAME,
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_CMR_HarmonicMasker },
	{ CMR_NOISEMASKER_MOD_NAME,
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_CMR_NoiseMasker },
	{ wxT("STIM_EXPGATEDTONE"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Stimulus_ExpGatedTone },
	{ wxT("STIM_HARMONIC"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Harmonic },
	{ HARMONIC_FREQSWEPT_MOD_NAME,
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Harmonic_FreqSwept },
	{ wxT("STIM_PULSETRAIN"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PulseTrain },
	{ wxT("STIM_PURETONE"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone },
	{ wxT("STIM_PURETONE_2"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_2 },
	{ wxT("STIM_PURETONE_AM"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_AM },
	{ wxT("STIM_PURETONE_BINAURAL"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_Binaural },
	{ wxT("STIM_PURETONE_FM"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_FM },
	{ wxT("STIM_PURETONE_MULTI"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_PureTone_Multi },
	{ wxT("STIM_PURETONE_MULTIPULSE"),
		IO_MODULE_CLASS, PROCESS_MODULE,
	  InitModule_PureTone_MultiPulse },
	{ wxT("STIM_STEPFUN"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_StepFunction },
	{ wxT("STIM_WHITENOISE"),
		IO_MODULE_CLASS, PROCESS_MODULE,
		InitModule_WhiteNoise },
	{ wxT("TRANS_COLLECTSIGNALS"),
		TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_CollectSignals },
	{ wxT("TRANS_DAMP"), TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_Gate },
	{ wxT("TRANS_GATE"),
		TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_Gate },
	{ wxT("TRANS_RAMP"), TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_Gate },
	{ wxT("TRANS_SETDBSPL"),
		TRANSFORM_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Transform_SetDBSPL },
	{ wxT("UTIL_ACCUMULATE"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Accumulate },
	{ wxT("UTIL_AMPMOD"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_AmpMod },
	{ UTILITY_AMPMOD_NOISE_MOD_NAME,
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_AmpMod_Noise },
	{ wxT("UTIL_BINSIGNAL"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_BinSignal },
	{ wxT("UTIL_COMPRESSION"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Compression },
	{ UTILITY_COLLATESIGNALS_MOD_NAME,
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_CollateSignals },
	{ wxT("UTIL_CONVMONAURAL"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_ConvMonaural },
	{ wxT("UTIL_CREATEBINAURAL"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_CreateBinaural },
	{ wxT("UTIL_CREATEJOINED"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_CreateJoined },
	{ wxT("UTIL_DELAY"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Delay },
	{ wxT("UTIL_HALFWAVERECTIFY"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_HalfWaveRectify },
	{ wxT("UTIL_ITERATEDRIPPLE"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_IteratedRipple },
	{ wxT("UTIL_LOCALCHANS"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_LocalChans },
	{ wxT("UTIL_MATHOP"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_MathOp },
	{ wxT("UTIL_PADSIGNAL"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_PadSignal },
	{ wxT("UTIL_PAUSE"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Pause },
	{ wxT("UTIL_REDUCECHANNELS"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_ReduceChannels },
	{ wxT("UTIL_REDUCEDT"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_ReduceDt },
	{ wxT("UTIL_REFRACTORYADJUST"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_RefractoryAdjust },
	{ wxT("UTIL_SAMPLE"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Sample },
	{ wxT("UTIL_SELECTCHANNELS"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_SelectChannels },
	{ wxT("UTIL_SHAPEPULSE"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_ShapePulse },
	{ wxT("UTIL_SIMSCRIPT"),
		UTILITY_MODULE_CLASS, SIMSCRIPT_MODULE,
		InitModule_Utility_SimScript },
	{ wxT("UTIL_STANDARDISE"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Standardise },
	{ wxT("UTIL_STROBE"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Strobe },
	{ wxT("UTIL_SWAPLR"),
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_SwapLR },
	{ UTILITY_TRANSPOSE_MOD_NAME,
		UTILITY_MODULE_CLASS, PROCESS_MODULE,
		InitModule_Utility_Transpose },

	{ NULL,
		0, MODULE_SPECIFIER_NULL,
		NULL }

	};

	return(&moduleList[index]);

}

/************************** UserList ******************************************/

/*
 * This routine returns an entry to the user module list.
 */

DSAM_API ModRegEntryPtr
UserList_ModuleReg(uShort index)
{
	if (!userModuleList)
		return(NULL);
	if (index > numUserModules)
		return(NULL);
	return(&userModuleList[index]);

}

/************************** Identify ******************************************/

/*
 * This function searches a list of registration entries.
 */

ModRegEntryPtr
Identify_ModuleReg(ModRegEntryPtr list, const WChar *name)
{
	if (!list)
		return(NULL);
	for ( ; list->name && (StrNCmpNoCase_Utility_String(list->name, name) != 0);
	  list++)
		;
	return ((!list->name)? NULL: list);

}

/************************** GetRegEntry ***************************************/

/*
 * This function returns a process module registration from the global lists.
 * It returns a pointer to the appropriate number, or zero if it does not find
 * identify the name.
 * All lists are expected to end with the null, "", string.
 * The names in the list are all assumed to be in upper case.
 * It ends terminally (exit(1)) if the list is not initialised.
 */

ModRegEntryPtr
GetRegEntry_ModuleReg(const WChar *name)
{
	static const WChar *funcName = wxT("GetRegEntry_ModuleReg");
	ModRegEntryPtr	regEntryPtr;

	if (name == NULL) {
		NotifyError(wxT("%s: Illegal null name."), funcName);
		return(NULL);
	}
	if (DSAM_strlen(name) >= MAXLINE) {
		NotifyError(wxT("%s: Search name is too long (%s)"), funcName, name);
		return(NULL);
	}
	if ((regEntryPtr = Identify_ModuleReg(LibraryList_ModuleReg(0), name)) !=
	  NULL)
		return(regEntryPtr);

	return(Identify_ModuleReg(userModuleList, name));

}

/************************** FreeUserModuleList ********************************/

/*
 * This function frees the user module list.
 */

void
FreeUserModuleList_ModuleReg(void)
{
	if (userModuleList)
		free(userModuleList);
	userModuleList = NULL;

}

/************************** InitUserModuleList ********************************/

/*
 * This function initialises the user module list.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitUserModuleList_ModuleReg(int theMaxUserModules)
{
	static WChar *funcName = wxT("InitUserModuleList_ModuleReg");
	int		i;
	ModRegEntryPtr	regEntry;

	FreeUserModuleList_ModuleReg();
	maxUserModules = (theMaxUserModules > 0)? theMaxUserModules:
	  MODULE_REG_DEFAAULT_USER_MODULES;
	if ((userModuleList = (ModRegEntryPtr) calloc(maxUserModules + 1, sizeof(
	  ModRegEntry))) == NULL) {
		NotifyError(wxT("%s: Out of memory for %d entries."), funcName,
		  maxUserModules);
		return(FALSE);
	}
	regEntry = userModuleList;
	for (i = 0; i < maxUserModules + 1; i++, regEntry++) {
		regEntry->name = NULL;
		regEntry->specifier = PROCESS_MODULE;
		regEntry->InitModule = NULL;
	}
	numUserModules = 0;
	return(TRUE);

}

/************************** RegEntry ******************************************/

/*
 * This function registers a user module entry into the user module list.
 * it
 * It returns FALSE if it fails in any way.
 */

DSAM_API BOOLN
RegEntry_ModuleReg(const WChar *name, BOOLN (* InitModuleFunc)(ModulePtr))
{
	static const WChar *funcName = wxT("RegEntry_ModuleReg");
	WChar	upperName[MAXLINE];
	ModRegEntryPtr	regEntry;

	if (numUserModules == maxUserModules) {
		NotifyError(wxT("%s: Maximum number (%d) of user modules registered.\n")
		  wxT("Use the SetMaxUserModules_AppInterface to increase this ")
		  wxT("number."), funcName, maxUserModules);
		return(FALSE);
	}
	if (!name) {
		NotifyError(wxT("%s: Illegal null or zero length module name."),
		  funcName);
		return(FALSE);
	}
	if (GetRegEntry_ModuleReg((WChar *) name)) {
		NotifyError(wxT("%s: Module name '%s' already used."), funcName, name);
		return(FALSE);
	}
	if (!InitModuleFunc) {
		NotifyError(wxT("%s: InitModule function has not been set."), funcName);
		return(FALSE);
	}
	ToUpper_Utility_String(upperName, (WChar *) name);
	regEntry = &userModuleList[numUserModules++];
	regEntry->name = (WChar *) name;
	regEntry->classSpecifier = USER_MODULE_CLASS;
	regEntry->specifier = PROCESS_MODULE;
	regEntry->InitModule = InitModuleFunc;

	return(TRUE);

}

/************************** GetNumUserModules *********************************/

/*
 * This routine returns the number of user modules.
 */

int
GetNumUserModules_ModuleReg(void)
{

	return(numUserModules);

}
