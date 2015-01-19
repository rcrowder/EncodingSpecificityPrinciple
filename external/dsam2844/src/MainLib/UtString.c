/**********************
 *
 * File:		UtString.c
 * Purpose:		This module contains routines that do string handling.
 * Comments:	08-06-99 LPO: Added the 'StrNCmpNoCase' routine.
 *				23-08-99 LPO: Moved the 'InitString_Utility_SimScript' to this
 *				module.
 * Author:		L. P. O'Mard
 * Created:		07 Oct 1998
 * Updated:		23 Aug 1999
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
#include <string.h>

#if DSAM_USE_UNICODE
#	include <wctype.h>
#else
#	include <ctype.h>
#endif


#include "GeCommon.h"
#include "UtString.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/


/****************************** ToUpper ***************************************/

/*
 * This routine copies the second string into the first, converting all
 * characters to upper case.
 * It carries out no checks on string lengths or the validity of strings.
 */

void
ToUpper_Utility_String(WChar *upperCaseString, const WChar *string)
{
	WChar		*p, *pp;

	for (p = upperCaseString, pp = (WChar *) string; *pp != '\0'; )
		*p++ = DSAM_toupper(*pp++);
	*p = '\0';

}

/****************************** StrCmpNoCase *********************************/

/*
 * This routine carries out the standard DSAM_strcmp function, but it ignores
 * the case of the operands by converting them both to uppercase.
 */

int
StrCmpNoCase_Utility_String(WChar *s1, WChar *s2)
{
	static WChar *funcName = wxT("StrCmpNoCase_Utility_String");
	WChar	upperString[2][LONG_STRING], *string[2];
	size_t		i, len[2];

	string[0] = s1;
	string[1] = s2;
	for (i = 0; i < 2; i++) {
		if ((len[i] = DSAM_strlen(string[i])) > LONG_STRING) {
			NotifyError(wxT("%s: String '%s' exceeds current available length ")
			  wxT("(%d)."), funcName, string[i], LONG_STRING);
			return (-1);
		}
		ToUpper_Utility_String(upperString[i], string[i]);
	}
	return(DSAM_strcmp(upperString[0], upperString[1]));

}

/****************************** StrNCmpNoCase *********************************/

/*
 * This routine carries out the standard strncmp function, but it ignores the
 * case of the operands by converting them both to uppercase.
 * The length used is that of the second argument.
 */

DSAM_API int
StrNCmpNoCase_Utility_String(const WChar *fullString, const WChar *abbrevString)
{
	static WChar *funcName = wxT("StrNCmpNoCase_Utility_String");
	WChar	upperString[2][LONG_STRING], *string[2];
	int		i, len[2];

	string[0] = (WChar *) fullString;
	string[1] = (WChar *) abbrevString;
	for (i = 0; i < 2; i++) {
		if ((len[i] = (int) DSAM_strlen(string[i])) > LONG_STRING) {
			NotifyError(wxT("%s: String '%s' exceeds current available length ")
			  wxT("(%d)."), funcName, string[i], LONG_STRING);
			return (-1);
		}
		ToUpper_Utility_String(upperString[i], string[i]);
	}
	return(DSAM_strncmp(upperString[0], upperString[1], len[1]));

}

/****************************** InitString ************************************/

/*
 * This routine allocates memory for a string and returns a pointer to the
 * the string.
 * It exits terminally if it fails.
 */

DSAM_API WChar *
InitString_Utility_String(WChar *string)
{
	static const WChar	*funcName = wxT("InitString_Utility_String");
	WChar	*p;

	if ((p = (WChar *) calloc((DSAM_strlen(string) + 1), sizeof(WChar))) ==
	  NULL) {
		NotifyError(wxT("%s: Out of memory for string '%s'."), funcName,
		  string);
		exit(1);
	}
	DSAM_strcpy(p, string);
	return(p);

}

/****************************** QuotedString **********************************/

/*
 * This routine returns a pointer to a temporary quoted string.  A copy should
 * be made if a permanent string is required.
 */

WChar *
QuotedString_Utility_String(const WChar *string)
{
	static WChar		newString[LONG_STRING];

	Snprintf_Utility_String(newString, LONG_STRING, wxT("\"%s\""), string);
	return (newString);

}

/**************************** GetSuffix ***************************************/

/*
 * This routine returns the suffix of a file name i.e. any WCharacters
 * after a ".".
 * It returns the entire file name if no suffix is returned.
 */

WChar *
GetSuffix_Utility_String(WChar *fileName)
{
	WChar	*p;

	if ((p = DSAM_strrchr(fileName, '.')) != NULL)
		return(p + 1);
	else
		return(fileName);

}

/**************************** GetFileNameFPath ********************************/

/*
 * This routine returns the filename with the path removed.
 * It returns the entire file name if there is no file path.
 *
 */

WChar *
GetFileNameFPath_Utility_String(WChar *fileName)
{
	WChar	*p;

	p = DSAM_strrchr(fileName, '/');
	if (!p)
		p = DSAM_strrchr(fileName, '\\');
	if (p == NULL)
		return(fileName);
	return(++p);

}

/**************************** RemoveChar **************************************/

/*
 * This routine removes a specified character from a string.
 * It expects the string to be properly terminated with a null character.
 */

WChar *
RemoveChar_Utility_String(WChar *string, WChar c)
{
	WChar	*p1, *p2;

	for (p1 = p2 = string; *p2; p1++)
		if (*p1 != c)
			*p2++ = *p1;
	*p2 = '\0';
	return(string);

}

/**************************** SubStrReplace ***********************************/

/*
 * This routine substitutes a substring within a string.  It returns
 * NULL if the sub-string is not found. otherwise it returns the position
 * of the start of the substituted string.
 * It expects the argument strings to be properly terminated with a null
 * character.
 * It expects the 'string' variable to be large enough to hold the newly created
 * string.
 */

WChar *
SubStrReplace_Utility_String(WChar *string, WChar *subString, WChar *repString)
{
	WChar	*s;
	size_t	subSLen = DSAM_strlen(subString), repSLen = (int) DSAM_strlen(repString);

	if ((s = DSAM_strstr(string, subString)) == NULL)
		return(NULL);
	DSAM_memmove(s + repSLen, s + subSLen, DSAM_strlen(s) - subSLen + 1);
	DSAM_memcpy(s, repString, repSLen);
	return(string);

}

/**************************** ConvUTF8 ****************************************/

/*
 * This function returns a pointer to a UTF8 string.  if working in non-unicode
 * format, then the original string is returned.
 * The returned string should be considered as temporary.
 */

DSAM_API char *
ConvUTF8_Utility_String(const WChar *src)
{
#	ifndef DSAM_USE_UNICODE
	return(src);
#	else
	static const WChar *funcName = wxT("ConvUTF8_Utility_String");
	static char	dest[LONG_STRING];

	if (wcslen(src) > LONG_STRING - 1) {
		NotifyError(wxT("%s: Static string length (%d) is too short for ")
		  wxT("string '%s'"), funcName, LONG_STRING, src);
		return(NULL);
	}
	if (wcstombs(dest, src, LONG_STRING - 1) < 0 ) {
		NotifyError(wxT("%s: Failed to convert wide character string (%d)."),
		  funcName, LONG_STRING);
		return(NULL);
	}
	return(dest);
#	endif

}

/**************************** MbToWC ******************************************/

/*
 * This function returns a wChar from a multi-byte character.
 * It returns NULL if it fails in any way.
 */

WChar
MBToWC_Utility_String(const char *mb)
{
#	ifndef DSAM_USE_UNICODE
	return(*mb);
#	else
	static const WChar *funcName = wxT("MBToWC_Utility_String");
	WChar	wc;

	if (mbtowc(&wc, mb, MB_CUR_MAX) < 0 ) {
		NotifyError(wxT("%s: Failed to convert wide character (%d)."),
		  funcName, MAXLINE);
		return('\0');
	}
	return(wc);
#	endif

}

/**************************** MBSToWCS ****************************************/

/*
 * This function returns a WChar string from a multi-btye string.
 * The converted string pointer is a static string which should be considered as
 * temporary.
 * The function 'mbsrtowcs' is thread-safe while 'mbstwcs' is not.
 * It returns NULL if it fails in any way.
 */

WChar *
MBSToWCS_Utility_String(const char *mb)
{
#	ifndef DSAM_USE_UNICODE
	return(mb);
#	else
	static const WChar *funcName = wxT("MBSToWCS_Utility_String");
	static WChar	dest[LONG_STRING];
	mbstate_t	state;

	if (strlen(mb) > LONG_STRING - 1) {
		NotifyError(wxT("%s: Static string length (%d) is too short for ")
		  wxT("string argument."), funcName, LONG_STRING);
		return(NULL);
	}
	memset(&state, 0, sizeof(mbstate_t));
	if (mbsrtowcs(dest, &mb, LONG_STRING, &state) < 0 ) {
		NotifyError(wxT("%s: Failed to convert wide character (%d)."),
		  funcName, LONG_STRING);
		return(NULL);
	}
	return(dest);
#	endif

}

/**************************** MBSToWCS2 ****************************************/

/*
 * This function returns a WChar string from a multi-btye string.
 * The converted string pointer is allocated memory which needs to be deallocated
 * The function 'mbsrtowcs' is thread-safe while 'mbstwcs' is not.
 * It returns NULL if it fails in any way.
 */

WChar *
MBSToWCS2_Utility_String(const char *mb)
{
	static const WChar *funcName = wxT("MBSToWCS2_Utility_String");
	size_t	len;
	WChar	*dest;
	mbstate_t	state;

	len = strlen(mb);
	if ((dest = (WChar *) calloc(len + 1, sizeof (WChar *))) == NULL) {
		NotifyError(wxT("%s: out of memory for string (%d)"), funcName, len + 1);
		return(NULL);
	}
#	ifndef DSAM_USE_UNICODE
	strcpy(dest, mb);
#	else
	memset(&state, 0, sizeof(mbstate_t));
	if (mbsrtowcs(dest, &mb, len, &state) < 0 ) {
		NotifyError(wxT("%s: Failed to convert wide character (%d)."),
		  funcName, len);
		free(dest);
		return(NULL);
	}
#	endif
	return(dest);

}

/**************************** ConvWCSIOFormat *********************************/

/*
 * This routine converts the '%s' formats to the wide char '%S' in a string.
 * It also does the conversion of '%c' to '%C'.
 * It expects the memory for both of the strings to of been previously allocated
 * and of equal lengths.
 */

void
ConvWCSIOFormat_Utility_String(wchar_t *dest, const wchar_t *src)
{
	BOOLN	fmtOn = FALSE;
	wchar_t	*d = dest, *s = (wchar_t *) src;

#	if defined(_MSC_VER)
	DSAM_strcpy(dest, src);
#	else
	for (*d++ = *s++; (*s); d++, s++) {
		if (*(s - 1) == '%')
			fmtOn = TRUE;
		if (fmtOn && ((*s == 'c') || (*s == 's'))) {
			*d = towupper(*s);
			fmtOn = FALSE;
		} else {
			*d = *s;
			if (iswspace(*s) || (*s == '%'))
				fmtOn = FALSE;
		}
	}
	*d = '\0';
#	endif /* _MSC_VER */

}

/**************************** ConvIOFormat ************************************/

/*
 * This function converts all instances of '%s' to '%S' and '%c' to '%C' to
 * allow the correct output of wide characters when using UNICODE format.
 * The switch statement is used because 'toupper' does not work with wide chars.
 * When the string is too long, NotifyError cannot be used as it calls this
 * function and we will get into a loop.
 * No checks a made for NULL destination strings.
 * I returns 'FALSE' if it fails in any way.
 */

BOOLN
ConvIOFormat_Utility_String(WChar *dest, const WChar *s, size_t size)
{
	static const WChar	*funcName = wxT("ConvIOFormat_Utility_String");

	if (!s || *s == '\0') {
		*dest = '\0';
		return(TRUE);
	}
	if (DSAM_strlen(s) > size) {
		fwprintf(stderr, wxT("%S: Error.  String too long (%d)\n"), funcName,
		  DSAM_strlen(s));
		*dest = '\0';
		return(FALSE);
	}
#	if DSAM_USE_UNICODE
	ConvWCSIOFormat_Utility_String(dest, (wchar_t *) s);
#	else
	strcpy(dest, s);
#	endif
	return(TRUE);

}

/**************************** Vsnprintf ***************************************/

/*
 * This function replaces the standard 'vsnprintf' routine which needs changes
 * to the string '%s' format in UNICODE mode.
 */

int
Vsnprintf_Utility_String(WChar *str, size_t size,  const WChar *format, va_list args)
{
	static const WChar *funcName = wxT("Vsnprintf_Utility_String");
	BOOLN	ok = TRUE;
	int		result = 0;

#	if DSAM_USE_UNICODE
	WChar	*p;
	if ((p = (WChar *) calloc(wcslen(format) + 1, sizeof(WChar))) == NULL) {
		NotifyError(wxT("%s: Out of memory."), funcName);
		ok = FALSE;
	}
	if (ok) {
		ConvWCSIOFormat_Utility_String(p, format);
		format = p;
		result = vswprintf(str, size, format, args);
		free(p);
	} else
		result = -1;

#	else
	result = vsnprintf(str, size, format, args);
#	endif

	return(result);

}

/**************************** Snprintf ****************************************/

/*
 * This function replaces the standard 'snprintf' routine which needs changes
 * to the string '%s' format in UNICODE mode.
 */

DSAM_API int
Snprintf_Utility_String(WChar *str, size_t size, const WChar *format, ...)
{
	int		result = 0;
	va_list	args;

	va_start(args, format);
	result = Vsnprintf_Utility_String(str, size, format, args);
	va_end(args);

	return(result);

}

/**************************** fprintf *****************************************/

/*
 * This function replaces the standard 'fprintf' routine which needs changes
 * to the string '%s' and '%c' formats in UNICODE mode.
 */

DSAM_API int
fprintf_Utility_String(FILE *fp,  const WChar *format, ...)
{
	static const WChar *funcName = wxT("fprintf_Utility_String");
	BOOLN	ok = TRUE;
	int		result = 0;

	va_list	args;

#	if DSAM_USE_UNICODE
	WChar	*p;
	if ((p = (WChar *) calloc(wcslen(format) + 1, sizeof(WChar))) == NULL) {
		NotifyError(wxT("%s: Out of memory."), funcName);
		ok = FALSE;
	}
	if (ok) {
		ConvWCSIOFormat_Utility_String(p, format);
		format = p;
		va_start(args, format);
		result = vfwprintf(fp, format, args);
		va_end(args);
		free(p);
	} else
		result = -1;

#	else
	va_start(args, format);
	result = vfprintf(fp, format, args);
	va_end(args);
#	endif


	return(result);

}
