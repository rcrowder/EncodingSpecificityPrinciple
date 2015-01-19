/******************
 *
 * File:		GeCommon.c
 * Purpose:		This file contains the common system routines:
 * Comments:	25-03-98 LPO: In graphics support mode the error and warning
 *				messages are sent to a window.
 *				It wasn't possible to create a single Notify_Message routine
 *				that could be called by "NotifyError" and "NotifyWarning"
 *				because, I think, of the way that the stdarg macros work.
 *				26-09-98 LPO: Introduced the minus sign convention for formats
 *				e.g. "-10s".  I have also introduced the "CloseFiles" routine.
 *				At present this is only being used by the simulation manager
 *				so that files are closed after the simulation has been run.
 *				28-09-98 LPO: Introduced the MAX_FILE_PATH constant definition
 *				and used in in all modules which need to store file names.
 *				29-01-99 LPO: Under the GRAPHICS mode the 'NotifyError' routine
 *				will produce a dialog the first time (if the
 *				dSAM.'dialogOutputFlag' flag is set to TRUE).  After the first
 *				dialog is created, then subsequent calls will send error
 *				messages to the console.
 *				04-02-99 LPO: Introduced the 'CloseFile' routine to prevent
 *				attempts to close the system files, such as 'stdin'.  It is now
 *				being used by all the respective 'Fi..' modules.
 *				30-04-99 LPO: Added the 'version' field to the 'dSAM' global
 *				structure.  It will mean that programs can report the current
 *				run-time library version rather than the compile-time version.
 *				I also introduced the 'parsFilePath' so that. This can be set
 *				to read parameters from a specified file path.
 *				09-05-99 LPO: Introduced the 'COMMON_GENERAL_DIAGNOSTIC' option
 *				for printing diagnostics.
 *				12-05-99 LPO: Introduced the 'GetParsFileFPath_Common'.
 *				08-07-99 LPO: Introduced the UNSET_FILE_PTR constant for the
 *				DSAM global structure file pointers.  This was necessary because
 *				these pointers cannot be used as initialisers under
 *				egcs 2.91.66 (gcc) because they are not constants.  The
 *				alternative would be to call a routine to initialise the dSAM
 *				global structure, but I do not want to do that.
 *				15-10-99 LPO: The 'usingGUIFlag' is set when MyApp is
 *				initialised.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		08 Jul 1999
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "GeCommon.h"
#include "UtString.h"
#include "GeNSpecLists.h"

/******************************************************************************/
/************************ Global variables ************************************/
/******************************************************************************/

DSAM	dSAM = {

			FALSE,		/* appInitialisedFlag */
			FALSE,		/* segmentedMode */
			FALSE,		/* usingGUIFlag */
			FALSE,		/* lockGUIFlag */
			FALSE,		/* usingExtFlag */
			FALSE,		/* interruptRequestedFlag */
			NULL,		/* diagnosticsPrefix */
			DSAM_VERSION,/* version */
			NULL,		/* parsFilePath */
			0,			/* notificationCount */
			UNSET_FILE_PTR,	/* warningsFile */
			UNSET_FILE_PTR,	/* errorsFile */
			UNSET_FILE_PTR,	/* parsFile */
			COMMON_CONSOLE_DIAG_MODE,	/* diagMode */
			DPrintStandard,	/* DPrint */
			NotifyStandard	/* Notify */

		};

/******************************************************************************/
/************************ Subroutines and functions ***************************/
/******************************************************************************/

/*************************** DPrintStandard ***********************************/

/*
 * This routine prints out a diagnostic message, preceded by a bell sound.
 * It is used in the same way as the vprintf statement.
 * This is the standard version for ANSI C.
 */

void
DPrintStandard(const WChar *format, va_list args)
{
	CheckInitParsFile_Common();
	if (dSAM.diagnosticsPrefix)
		DSAM_fprintf(dSAM.parsFile, STR_FMT, dSAM.diagnosticsPrefix);
	DSAM_vfprintf(dSAM.parsFile, format, args);

}

/*************************** DPrint *******************************************/

/*
 * This routine prints out a diagnostic message.
 * It is used in the same way as the printf statement.
 * This is the standard version for ANSI C.
 */

DSAM_API void
DPrint(const WChar *format, ...)
{
	va_list	args;
#	if DSAM_USE_UNICODE
	WChar	newFormat[LONG_STRING];
#	endif
	CheckInitParsFile_Common();
	if (!dSAM.parsFile || !dSAM.DPrint || (dSAM.diagMode ==
	  COMMON_OFF_DIAG_MODE))
		return;
#	if DSAM_USE_UNICODE
	ConvIOFormat_Utility_String(newFormat, format, LONG_STRING);
	format = newFormat;
#	endif
	va_start(args, format);
	if (dSAM.diagMode == COMMON_CONSOLE_DIAG_MODE)
		DPrintStandard(format, args);
	else
		(* dSAM.DPrint)(format, args);
	va_end(args);

}

/*************************** DPrintBuffer *************************************/

/*
 * This routine formats the diagnostic output to a buffer, using the
 * "EmptyDiagBuffer" print routine - which is specific to the output form.
 */

DSAM_API void
DPrintBuffer_Common(const WChar *format, va_list args,	void (* EmptyDiagBuffer)(
  WChar *, size_t *))
{
	static	const WChar *funcName = wxT("DPrintBuffer_Common");
	BOOLN	longVar;
	WChar	*p, *s, buffer[LONG_STRING], *f = NULL, subFormat[SMALL_STRING];
	size_t	i, c, tabPosition;

	if (dSAM.diagMode == COMMON_OFF_DIAG_MODE)
		return;
	if (!EmptyDiagBuffer) {
		NotifyError(wxT("%s: The 'EmptyDiagBuffer' routine has not been set."),
		  funcName);
		return;
	}
	if (dSAM.diagnosticsPrefix)
		DSAM_strncpy(buffer, dSAM.diagnosticsPrefix, MAXLINE);
	else
		*buffer = '\0';
	for (p = (WChar *) format, c = DSAM_strlen(buffer); *p != '\0'; p++)
		if (c >= LONG_STRING - 1)
			(* EmptyDiagBuffer)(buffer, &c);
		else if (*p == '%') {
			(* EmptyDiagBuffer)(buffer, &c);
			for (f = subFormat, *f++ = *p++; isdigit(*p) || (*p == '.') ||
			  (*p == '-'); )
				*f++ = *p++;
			longVar = (*p == 'l');
			if (longVar)
				*f++ = *p++;
			*f++ = *p;
			*f = '\0';
			switch (*p) {
			case 'f':
			case 'g':
				Snprintf_Utility_String(buffer, MAXLINE, subFormat, va_arg(args,
				  Float));
				break;
			case 'd':
				Snprintf_Utility_String(buffer, MAXLINE, subFormat, (longVar)?
				  va_arg(args, long): va_arg(args, int));
				break;
			case 'u':
				Snprintf_Utility_String(buffer, MAXLINE, subFormat, (longVar)?
				  va_arg(args, unsigned long): va_arg(args, unsigned));
				break;
			case 'c':
				Snprintf_Utility_String(buffer, MAXLINE, subFormat, va_arg(args,
				  int));
				break;
			case 's':
			case 'S':
				s = va_arg(args, WChar *);
				if (DSAM_strlen(s) >= LONG_STRING) {
					NotifyError(wxT("%s: Buffer(%d) is too small for string ")
					  wxT("(%d)."), funcName, LONG_STRING, DSAM_strlen(s));
					return;
				}
				Snprintf_Utility_String(buffer, MAXLINE, subFormat, s);
				break;
			case '%':
				Snprintf_Utility_String(buffer, MAXLINE, wxT("%%"));
				break;
			default:
				Snprintf_Utility_String(buffer, MAXLINE, wxT("%c"), *p);
				break;
			}
			c = DSAM_strlen(buffer);
		} else if (*p == '\t') {
			tabPosition = TAB_SPACES * (c / TAB_SPACES + 1);
			for (i = c; (i < tabPosition) && (c < LONG_STRING - 1); i++)
				buffer[c++] = ' ';
		} else {
			buffer[c++] = *p;
		}
	if (c > 0)
		(* EmptyDiagBuffer)(buffer, &c);

}

/*************************** DiagnosticTitle **********************************/

/*
 * This is a simple routine to return the pointer to a diagnostic title.
 */

DSAM_API const WChar *
DiagnosticTitle(CommonDiagSpecifier type)
{
	static NameSpecifier	list[] = {

			{ wxT("Error"),			COMMON_ERROR_DIAGNOSTIC },
			{ wxT("Warning"),		COMMON_WARNING_DIAGNOSTIC },
			{ wxT("Alert"),			COMMON_GENERAL_DIAGNOSTIC },
			{ wxT("Alert (stop)"),	COMMON_GENERAL_DIAGNOSTIC_WITH_CANCEL }
		};

	return(list[type].name);
}

/***************************** NotifyStandard *********************************/

/*
 * This routine prints out a message to the respective file descriptor.
 * This is the standard version which can be used both in GUI mode and non-
 * GUI mode.
 * It does not take any further action, as responses to errors differ: some
 * errors are recoverable while others are fatal.
 */

void
NotifyStandard(const WChar *message, CommonDiagSpecifier type)
{
	FILE	*fp;

	switch (type) {
	case COMMON_ERROR_DIAGNOSTIC:
		fp = dSAM.errorsFile;
		break;
	case COMMON_WARNING_DIAGNOSTIC:
		fp = dSAM.warningsFile;
		break;
	default:
		fp = stdout;
	}
	DSAM_fprintf(fp, message);
	DSAM_fprintf(fp, wxT("\n"));

} /* NotifyStandard */

/***************************** NotifyError ************************************/

/*
 * This routine prints out an error message, preceded by a bell sound.
 * It is used in the same way as the printf statement.
 * It does not take any further action, as responses to errors differ: some
 * errors are recoverable while others are fatal.
 * This routine will be incorporated in an alert dialog in the DSAM
 * application.
 */

DSAM_API void
NotifyError(const WChar *format, ...)
{
	WChar	message[LONG_STRING];
	va_list	args;

	CheckInitErrorsFile_Common();
	if (!dSAM.errorsFile)
		return;
	if ((dSAM.errorsFile == stderr) && (!dSAM.usingGUIFlag ||
	  (dSAM.diagMode == COMMON_DIALOG_DIAG_MODE)))
		DSAM_fprintf(stderr, wxT("\07"));
	va_start(args, format);
	Vsnprintf_Utility_String(message, LONG_STRING, format, args);
	va_end(args);
	(* dSAM.Notify)(message, COMMON_ERROR_DIAGNOSTIC);
	dSAM.notificationCount++;

} /* NotifyError */

/*************************** NotifyWarning ************************************/

/*
 * This routine prints out a warning message, preceded by a bell sound.  It is
 * used in the same way as the printf statement.  It does not take any further
 * action.
 */

DSAM_API void
NotifyWarning(const WChar *format, ...)
{
	WChar	message[LONG_STRING];
	va_list	args;

	CheckInitWarningsFile_Common();
	if (!dSAM.warningsFile)
		return;
	va_start(args, format);
	Vsnprintf_Utility_String(message, LONG_STRING, format, args);
	va_end(args);
	(* dSAM.Notify)(message, COMMON_WARNING_DIAGNOSTIC);
	va_start(args, format);
	dSAM.notificationCount++;

} /* NotifyWarning */

/*************************** GetFilePtr ***************************************/

/*
 * This function returns a file pointer according to a specified mode.
 * The default is the standard output, which is set by sending
 * a null string as the function argument.
 * The function returns 'stderr' if it fails.
 */

FILE *
GetFilePtr(const WChar *outputSpecifier, FileAccessSpecifier mode)
{
	static const WChar *funcName = wxT("GetFilePtr");
	char	*fileAccess;
	FILE	*fp;

	switch (Identify_NameSpecifier(outputSpecifier,
	  DiagModeList_NSpecLists(0))) {
	case GENERAL_DIAGNOSTIC_OFF_MODE:
		return(NULL);
	case GENERAL_DIAGNOSTIC_SCREEN_MODE:
		return(stdout);
	case GENERAL_DIAGNOSTIC_ERROR_MODE:
		return(stderr);
	default:
		fileAccess = (mode == APPEND)? "a": "w";
		if ((fp = DSAM_fopen(outputSpecifier,
		  fileAccess)) == NULL) {
			NotifyError(wxT("%s: Could not open file '%s' output sent to ")
			  wxT("stderr."), funcName, outputSpecifier);
			return(stderr);
		}
	}
	return(fp);

}

/*************************** SetWarningsFile **********************************/

/*
 * This function sets the file to which warnings should be sent.
 * The default is the standard output, which can be reset by sending
 * a null string as the function argument.
 * The function returns TRUE if successful.
 */

void
SetWarningsFile_Common(const WChar *outputSpecifier, FileAccessSpecifier mode)
{
	dSAM.warningsFile = GetFilePtr(outputSpecifier, mode);

}

/*************************** SetParsFile **************************************/

/*
 * This function sets the file to which parameter listings should be sent.
 * The default is the standard output, which can be reset by sending
 * a null string as the function argument.
 * The function returns TRUE if successful.
 */

DSAM_API BOOLN
SetParsFile_Common(const WChar *outputSpecifier, FileAccessSpecifier mode)
{
	dSAM.parsFile = GetFilePtr(outputSpecifier, mode);
	return(dSAM.parsFile != stderr);

}

/*************************** SetErrorsFile ************************************/

/*
 * This function sets the file to which errors should be sent.
 * The default is the standard error, which can be reset by sending
 * a null string as the function argument.
 * The function returns TRUE if successful.
 */

DSAM_API void
SetErrorsFile_Common(const WChar *outputSpecifier, FileAccessSpecifier mode)
{
	dSAM.errorsFile = GetFilePtr(outputSpecifier, mode);

}

/*************************** SetSegmentedMode ********************************/

/*
 * This routine sets the segmented mode variable. It instructs process
 * routines not to reset their parameters after the first run, or to introduce
 * the necessary time offset.
 */

void
SetSegmentedMode(BOOLN setting)
{
	dSAM.segmentedMode = setting;

}

/*************************** SetDiagnosticsPrefix *****************************/

/*
 * This routine sets the prefix to be printed before diagnostic output using
 * 'DPrint'.
 */

void
SetDiagnosticsPrefix(WChar *prefix)
{
	dSAM.diagnosticsPrefix = prefix;

}

/*************************** CloseFile ****************************************/

/*
 * This routine closes a file, checking that it is not any of the special
 * system files.
 */

void
CloseFile(FILE *fp)
{
	if (fp && (fp != UNSET_FILE_PTR) && (fp != stdin) && (fp != stdout) &&
	  (fp != stderr))
		fclose(fp);

}

/*************************** CloseFiles ***************************************/

/*
 * This routine closes any files opened by DSAM, i.e. diagnostic files.
 */

DSAM_API void
CloseFiles(void)
{
	CloseFile(dSAM.warningsFile);
	CloseFile(dSAM.errorsFile);
	CloseFile(dSAM.parsFile);

}

/*************************** ResetGUIDialogs **********************************/

/*
 * This routine resets the notification field so that output is sent to dialogs
 * and not to the console.
 * It also sets the dialog mode for GUI dialogs.
 */

DSAM_API void
ResetGUIDialogs(void)
{
	dSAM.notificationCount = 0;
	SetDiagMode(COMMON_DIALOG_DIAG_MODE);

}

/*************************** SetDiagMode **************************************/

/*
 * This routine sets the 'dialogOutputFlag', defining if output is sent to
 * dialogs and not to the console.
 */

DSAM_API void
SetDiagMode(DiagModeSpecifier mode)
{
	dSAM.diagMode = mode;

}

/*************************** SetDPrintFunc ************************************/

/*
 * This routine sets the 'DPrintFunc', defining where diagnostic output is sent.
 */

DSAM_API void
SetDPrintFunc(void (* Func)(const WChar *, va_list))
{
	dSAM.DPrint = Func;

}

/*************************** SetNotifyFunc ************************************/

/*
 * This routine sets the 'DPrintFunc', defining where diagnostic output is sent.
 */

DSAM_API void
SetNotifyFunc(void (* Func)(const WChar *, CommonDiagSpecifier))
{
	dSAM.Notify = Func;

}

/*************************** SetUsingGUIStatus *******************************/

/*
 * This routine sets the 'usingGUIFlag', defining if output is sent to
 * dialogs and not to the console.
 */

DSAM_API void
SetUsingGUIStatus(BOOLN status)
{
	dSAM.usingGUIFlag = status;

}

/*************************** SetUsingExtStatus ********************************/

/*
 * This routine sets the 'usingExtFlag'.
 */

DSAM_API void
SetUsingExtStatus(BOOLN status)
{
	dSAM.usingExtFlag = status;

}

/*************************** SetInterruptRequestStatus ************************/

/*
 * This routine sets the 'SetInterruptRequestStatus'.  This flag is set when
 * an interrupt has been requested, and processing must be curtailed.
 */

DSAM_API void
SetInterruptRequestStatus_Common(BOOLN status)
{
	dSAM.interruptRequestedFlag = status;

}

/*************************** SetParsFilePath **********************************/

/*
 * This function sets the file path field of the global 'dSAM' structure.
 * At the moment this is used by the simulation script utility.
 * In use, a zero-length string should assume the current directory.
 */

void
SetParsFilePath_Common(WChar *name)
{
	if ((name == NULL) || (name[0] == '\0'))
		dSAM.parsFilePath = NULL;
	else
		dSAM.parsFilePath = name;

}

/*************************** FindFilePathAndName ******************************/

/*
 * This routine returns as argument pointers the pointers to the file path and
 * file name of a string.
 * It sets the path and name to a zero length string if no path or name is find
 * in the 'filePath' respectively.
 */

void
FindFilePathAndName_Common(WChar *filePath, WChar *path, WChar *name)
{
	WChar *p;

	if (filePath && (((p = DSAM_strrchr(filePath, '/')) != NULL) || ((p =
	  DSAM_strrchr(filePath, '\\')) != NULL))) {
		if (path) {
			DSAM_strncpy(path, filePath, p - filePath);
			path[p - filePath] = '\0';
		}
		if (name)
			DSAM_strcpy(name, p + 1);
		return;
	}
	if (path)
		path[0] = '\0';
	if (name) {
		if (filePath)
			DSAM_strcpy(name, filePath);
		else
			name[0] = '\0';
	}

}

/*************************** GetParsFileFPath *********************************/

/*
 * This routine returns a pointer to the parameter file file-path.
 * It always assumes that any file path in 'parFile' is to be appended to
 * 'dsam.parsFilePath', unless the 'parFile' string starts with '/' or contains
 * ":\" when an absolute path is assumed.
 */

DSAM_API WChar *
GetParsFileFPath_Common(WChar *parFile)
{
	static const WChar *funcName = wxT("GetParsFileFPath_Common");
	static WChar filePath[MAX_FILE_PATH];

	if (!dSAM.parsFilePath)
		return(parFile);
	if (DSAM_strlen(parFile) >= MAX_FILE_PATH) {
		parFile[MAX_FILE_PATH - 1] = '\0';
		NotifyWarning(wxT("%s: file path is too long, truncating to '%s'"),
		  funcName, parFile);
	}
	if ((parFile[0] == '/') || DSAM_strstr(parFile, wxT(":\\")))
		DSAM_strcpy(filePath, parFile);
	else
		Snprintf_Utility_String(filePath, MAX_FILE_PATH, wxT("%s/%s"),
		  dSAM.parsFilePath, parFile);
	return (filePath);

}

/*************************** SwitchDiagnostics ********************************/

/*
 * This routine sets the specified diagnostics file point to NULL, and remembers
 * the previous value so that it can switch it back to the previous value.
 */

void
SwitchDiagnostics_Common(CommonDiagSpecifier specifier, BOOLN on)
{
	static FILE 	*oldWarningsFile = UNSET_FILE_PTR;
	static FILE 	*oldErrorsFile = UNSET_FILE_PTR;
	static FILE 	*oldParsFile = UNSET_FILE_PTR;

	switch (specifier) {
	case COMMON_ERROR_DIAGNOSTIC:
		if (on)
			dSAM.errorsFile = oldErrorsFile;
		else {
			oldErrorsFile = dSAM.errorsFile;
			dSAM.errorsFile = NULL;
		}
		break;
	case COMMON_WARNING_DIAGNOSTIC:
		if (on)
			dSAM.warningsFile = oldWarningsFile;
		else {
			oldWarningsFile = dSAM.warningsFile;
			dSAM.warningsFile = NULL;
		}
		break;
	case COMMON_GENERAL_DIAGNOSTIC:
	case COMMON_GENERAL_DIAGNOSTIC_WITH_CANCEL:
		if (on)
			dSAM.parsFile = oldParsFile;
		else {
			oldParsFile = dSAM.parsFile;
			dSAM.parsFile = NULL;
		}
		break;
	}

}

/*************************** CheckInitErrorsFile ******************************/

/*
 * This checks that the DSAM global structure errors file pointer has been
 * initialised.
 * It will set the pointer to the default value if it is unset.
 */

void
CheckInitErrorsFile_Common(void)
{
	if (dSAM.errorsFile == UNSET_FILE_PTR)
		dSAM.errorsFile = DEFAULT_ERRORS_FILE;

}

/*************************** CheckInitParsFile ********************************/

/*
 * This checks that the DSAM global structure parameters file pointer has been
 * initialised.
 * It will set the pointer to the default value if it is unset.
 */

DSAM_API void
CheckInitParsFile_Common(void)
{
	if (dSAM.parsFile == UNSET_FILE_PTR)
		dSAM.parsFile = DEFAULT_PARS_FILE;

}

/*************************** CheckInitWarningsFile ****************************/

/*
 * This checks that the DSAM global structure warnings file pointer has been
 * initialised.
 * It will set the pointer to the default value if it is unset.
 */

void
CheckInitWarningsFile_Common(void)
{
	if (dSAM.warningsFile == UNSET_FILE_PTR)
		dSAM.warningsFile = DEFAULT_WARNINGS_FILE;

}

/*************************** FreeFloatArray **********************************/

/*
 * This frees the memory for a double array, after first checking that it is not
 * set to NULL.
 */

void
FreeFloatArray_Common(Float **p)
{
	if (*p == NULL)
		return;
	free(*p);
	*p = NULL;

}

/*************************** GetDSAMPtr ***************************************/

/*
 * This return returns the global DSAMPtr structure pointer.
 */

DSAM_API DSAMPtr
GetDSAMPtr_Common(void)
{
	return(&dSAM);

}

/*************************** SwitchGUILocking *********************************/

/*
 * This routine turns the GUI locking on and off to avoid conflicts.
 */

DSAM_API void
SwitchGUILocking_Common(BOOLN on)
{
	dSAM.lockGUIFlag = on;

}

/****************************** ResizeFloatArray *****************************/

/*
 * This routine resizes an array.
 */

BOOLN
ResizeFloatArray_Common(Float **array, int *oldLength, int length)
{
	static const WChar *funcName = wxT("ResizeFloatArray_Common");
	register Float	*newArray, *oldArray;
	int		i;
	Float	*savedArray = NULL;

	if (length == *oldLength)
		return(TRUE);
	if (*array)
		savedArray = *array;
	if ((*array = (Float *) calloc(length, sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate memory for '%d' selectionArray."),
		  funcName, length);
		return(FALSE);
	}
	if (savedArray) {
		newArray = *array;
		oldArray = savedArray;
		for (i = 0; i < *oldLength; i++)
			*newArray++ = *oldArray++;
		free(savedArray);
	}
	*oldLength = length;
	return(TRUE);
}
