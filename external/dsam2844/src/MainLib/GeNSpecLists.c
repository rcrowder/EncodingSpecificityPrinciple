/**********************
 *
 * File:		GeNSpecLists.c
 * Purpose:		This module contains the common specifier lists used by various
 *				modules, and also the common associated actions.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		26 Nov 1997
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "GeCommon.h"
#include "GeNSpecLists.h"
#include "UtString.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** BooleanList ***********************************/

/*
 * This routine returns a name specifier for the boolean mode list.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

DSAM_API NameSpecifier *
BooleanList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("OFF"),	GENERAL_BOOLEAN_OFF},
					{ wxT("ON"),	GENERAL_BOOLEAN_ON},
					{ 0,			GENERAL_BOOLEAN_NULL},

				};
	return (&modeList[index]);

}

/****************************** DiagModeList **********************************/

/*
 * This routine returns a name specifier for the diagnostic mode list.
 * If the NULL value is returned, this is normally interpreted as using the
 * searched string as a file name.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
DiagModeList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("OFF"),			GENERAL_DIAGNOSTIC_OFF_MODE},
					{ wxT("SCREEN"),		GENERAL_DIAGNOSTIC_SCREEN_MODE},
					{ wxT("ERROR"),			GENERAL_DIAGNOSTIC_ERROR_MODE},
					{ DEFAULT_FILE_NAME,	GENERAL_DIAGNOSTIC_FILE_MODE},
					{ 0,					GENERAL_DIAGNOSTIC_MODE_NULL},

				};
	return (&modeList[index]);

}

/****************************** FitFuncModeList *******************************/

/*
 * This routine returns a name specifier for the fitfunction mode list.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
FitFuncModeList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("EXP_FUNC1"),		GENERAL_FIT_FUNC_EXP1_MODE},
					{ wxT("LINEAR_FUNC1"),	GENERAL_FIT_FUNC_LINEAR1_MODE},
					{ wxT("LOG_FUNC1"),		GENERAL_FIT_FUNC_LOG1_MODE},
					{ wxT("LOG_FUNC2"),		GENERAL_FIT_FUNC_LOG2_MODE},
					{ wxT("POLY_FUNC1"),	GENERAL_FIT_FUNC_POLY1_MODE},
					{ 0,					GENERAL_FIT_FUNC_NULL},

				};
	return (&modeList[index]);

}

/****************************** InitPhaseModeList *****************************/

/*
 * This routine returns a name specifier for the Phase mode  list.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
PhaseModeList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("RANDOM"),			GENERAL_PHASE_RANDOM },
					{ wxT("SINE"), 				GENERAL_PHASE_SINE },
					{ wxT("COSINE"), 			GENERAL_PHASE_COSINE },
					{ wxT("ALTERNATING"),		GENERAL_PHASE_ALTERNATING },
					{ wxT("SCHROEDER"),			GENERAL_PHASE_SCHROEDER },
					{ wxT("PLACK_AND_WHITE"),	GENERAL_PHASE_PLACK_AND_WHITE },
					{ wxT("USER"),				GENERAL_PHASE_USER },
					{ 0,						GENERAL_PHASE_NULL },
				};
	return(&modeList[index]);

}

/****************************** EarModeList ********************************/

/*
 * This routine returns a name specifier for the Phase mode  list.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
EarModeList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("LEFT"),	GENERAL_EAR_LEFT },
					{ wxT("RIGHT"),	GENERAL_EAR_RIGHT },
					{ wxT("BOTH"),	GENERAL_EAR_BOTH },
					{ 0,			GENERAL_EAR_NULL },
				};
	return(&modeList[index]);

}

/****************************** SpacingModeList ********************************/

/*
 * This routine returns a name specifier for the spacing mode  list.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
SpacingModeList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("ERB"),		GENERAL_SPACINGMODE_ERB },
					{ wxT("LINEAR"),	GENERAL_SPACINGMODE_LINEAR },
					{ wxT("OCTAVE"),	GENERAL_SPACINGMODE_OCTAVE },
					{ 0,				GENERAL_SPACINGMODE_NULL },
				};
	return(&modeList[index]);

}

/****************************** SetPhaseArray *********************************/

/*
 * This routine sets a phase array according to the phase mode.
 * It assumes that the phase array has the correct memory allocated.
 */

void
SetPhaseArray_NSpecLists(Float *phase, long *ranSeed, RandParsPtr randPars,
  int phaseMode, Float phaseVariable, int lowestHarmonic, int numHarmonics)
{
	int		i, harmonicNumber;
	Float	piOver2 = PI / 2.0;

	for (i = 0; i < numHarmonics; i++) {
		harmonicNumber = lowestHarmonic + i;
		switch (phaseMode) {
			case GENERAL_PHASE_RANDOM:
				*ranSeed = (long) phaseVariable;
				phase[i] = PIx2 * Ran01_Random(randPars);
				break;
			case GENERAL_PHASE_SINE:
				phase[i] = 0.0;
				break;
			case GENERAL_PHASE_COSINE:
				phase[i] = PI / 2.0;
				break;
			case GENERAL_PHASE_ALTERNATING:
				phase[i] = ((i % 2) == 0)? 0.0: piOver2;
				break;
			case GENERAL_PHASE_SCHROEDER:
				phase[i] = phaseVariable * PI * harmonicNumber * (harmonicNumber +
				  1) / numHarmonics;
				break;
			case GENERAL_PHASE_PLACK_AND_WHITE:
				phase[i] = harmonicNumber * phaseVariable;
				break;
			case GENERAL_PHASE_USER:
				phase[i] = phaseVariable;
				break;
			case GENERAL_PHASE_NULL:
				phase[i] = 0.0;
				break;
		} /* switch */
	}

}

/****************************** GetNumListEntries *****************************/

/*
 * This function returns the number of list entries.  It is a general
 * method which will allow me to simply change the number of entries without
 * introducing errors.
 */

int
GetNumListEntries_NSpecLists(NameSpecifierPtr list)
{
	static WChar *funcName = wxT("GetNumListEntries_NSpecLists");
	int		count = 0;

	if (!list) {
		NotifyError(wxT("%s: list not initialised, -1 will be returned."),
		  funcName);
		return(-1);
	}
	while ((list++)->name)
		count++;
	return(count + 1);

}

/****************************** InitNameList **********************************/

/*
 * This routine intialises a name list array, using a prototype.
 */

NameSpecifier *
InitNameList_NSpecLists(NameSpecifierPtr prototypeList, WChar *textPtr)
{
	static WChar *funcName = wxT("InitNameList_NSpecLists");
	int		i, numEntries;
	NameSpecifierPtr	list;

	if ((numEntries = GetNumListEntries_NSpecLists(prototypeList)) < 1) {
		NotifyError(wxT("%s: Prototyp list not set up correctly."), funcName);
		return(NULL);
	}
	if ((list = (NameSpecifier *) calloc(numEntries, sizeof(NameSpecifier))) ==
	  NULL) {
		NotifyError(wxT("%s: Could not allocate memory for %d entries."),
		  funcName,
		  numEntries);
		return(NULL);
	}
	for (i = 0; i < numEntries; i++) {
		list[i] = prototypeList[i];
	}
	list[numEntries - 2].name = textPtr;
	return(list);

}

/****************************** IdentifyDiagMode ******************************/

/*
 * This is a special indentify routine for the diagnostic mode.
 * If the mode cannot be found or is the wxT("file") mode then the second from
 * last, i.e. the mode string will be copied to the string pointed to by the
 * name specifier 'name' field of the 'GENERAL_DIAGNOSTIC_FILE_MODE' entry.
 */

int
IdentifyDiag_NSpecLists(WChar *mode, NameSpecifierPtr list)
{
	int		specifier;

	switch (specifier = Identify_NameSpecifier(mode, list)) {
	case GENERAL_DIAGNOSTIC_FILE_MODE:
	case GENERAL_DIAGNOSTIC_MODE_NULL:
		specifier = GENERAL_DIAGNOSTIC_FILE_MODE;
		DSAM_strncpy((WChar *) list[(int) GENERAL_DIAGNOSTIC_FILE_MODE].name, mode,
		  MAX_FILE_PATH);
		break;
	default:
		;
	}
	return(specifier);

}

/****************************** OpenDiagnostics *******************************/

/*
 * This function sets up the diagnostics.
 * It returns FALSE if it fails in anyway.
 */

BOOLN
OpenDiagnostics_NSpecLists(FILE **fp, NameSpecifierPtr list, int mode)
{
	static const WChar *funcName = wxT("OpenDiagnostics_NSpecLists");
	WChar	*filePath, *fileName;

	switch (mode) {
	case GENERAL_DIAGNOSTIC_OFF_MODE:
		return(TRUE);
	case GENERAL_DIAGNOSTIC_FILE_MODE:
		fileName = (WChar *) list[(int) GENERAL_DIAGNOSTIC_FILE_MODE].name;
		filePath = (IS_ABSOLUTE_PATH(fileName))? fileName:
		  GetParsFileFPath_Common(fileName);
		if ((*fp = DSAM_fopen(filePath, "w")) == NULL) {
			NotifyError(wxT("%s: Could not open file '%s' for diagnostics."),
			  funcName, filePath);
			return(FALSE);
		}
		break;
	case GENERAL_DIAGNOSTIC_SCREEN_MODE:
		*fp = stdout;
		break;
	default:
		NotifyError(wxT("%s: Mode was not correctly set (%d)."), funcName,
		  mode);
		return(FALSE);
	} /* switch */
	return(TRUE);

}

/****************************** CloseDiagnostics ******************************/

/*
 * This routine closes the output file pointer, if it is not the standard
 * output.
 */

void
CloseDiagnostics_NSpecLists(FILE **fp)
{
	if (*fp && (*fp != stdout)) {
		fclose(*fp);
		*fp = NULL;
	}

}

