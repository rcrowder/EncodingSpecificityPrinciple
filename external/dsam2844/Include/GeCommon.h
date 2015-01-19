/******************
 *
 * File:		GeCommon.h
 * Purpose:		This file contains the common system routines:
 * Comments:	25-03-98 LPO: In graphics support mode the error and warning
 *				messages are sent to a window.
 *				It wasn't possible to create a single NotifyMessage routine
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
 *				dSAM.'dialogOutputFlag' flag is set to true).  After the first
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

#ifndef	_GECOMMON_H
#define _GECOMMON_H	1

#include <stdarg.h>
#include <float.h>		/* - sort out DBL_MAX previously defined problems. */

#if DSAM_USE_UNICODE
#	ifndef __USE_UNIX98			/* For FC5 headers */
#		define __USE_UNIX98 1
#	endif /* __USE_UNIX98 */
#	include <wchar.h>
#endif

#ifdef DMALLOC
#	include "dmalloc.h"
#endif

/******************************************************************************/
/*************************** Pre-constant Macro Definitions *******************/
/******************************************************************************/

/* The following definition is required for unicode internationalisation support
 */

#ifndef wxT
#	if DSAM_USE_UNICODE
#		define wxT(X)	L ## X
#	else
#		define wxT(X)	X
#	endif
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*#define USE_FFTW3_THREADS 1*/

/*
 * Normal definitions
 */

#ifndef PI
# define	PI	3.141592653589792434		/* From MacEar */
#endif

#define PIx2	6.283185307179584868
#define PI_2	1.570796326794896217

#define	SQRT_2	1.4142135623730951

#ifndef LN_2
#	define LN_2	0.69314718055994529
#endif

#ifndef LOG_2
#	define LOG_2	0.30102999566398
#endif

#define MAXLINE				80		/* Max. No. of characters in a line. */
#define SMALL_STRING		10		/* For small string operations. */
#define LONG_STRING			255		/* For very long strings. */
#define MAX_FILE_PATH		255		/* For file names - can have long paths */
#define UNSET_STRING		wxT("<unset>") /* initial string value for arrays.*/
#define TAB_SPACES			4		/* Spaces per tab with GRAPHICS_SUPPORT */
#define	NO_FILE				wxT("not_set") /*-for when this must be indicated.*/
#define	DEFAULT_FILE_NAME	wxT("<file name>")
#define DEFAULT_ERRORS_FILE		stderr
#define DEFAULT_WARNINGS_FILE	stdout
#define DEFAULT_PARS_FILE		stdout
#define UNSET_FILE_PTR			(FILE *) -1
#define NULL_MODULE_PROCESS_NAME	wxT("Null process module")
#define DEFAULT_DT				0.01e-3
#define DEFAULT_INTENSITY		56.0
#define	DSAM_VERSION_SEPARATOR	'.'

#ifndef DSAM_DATA_INSTALL_DIR
#	define	DSAM_DATA_INSTALL_DIR	wxT(".") /* Used to find DSAM help files. */
#endif

#ifndef TRUE
#	define	TRUE		1
#endif
#ifndef FALSE
#	define	FALSE		0
#endif

#ifndef HAVE_INT16
#	define HAVE_INT16
#	if defined(SIZEOF_INT) && (SIZEOF_INT == 2)
		typedef	 int	int16;
#	else /* Assume sizeof(short) == 2 */
		typedef	 short	int16;
#	endif /* SIZEOF_INT */
#endif /* HAVE_INT16 */

#ifndef HAVE_INT32
#	define HAVE_INT32
#	if defined(SIZEOF_LONG) && (SIZEOF_LONG == 8)
		typedef  int	int32;
#	else /* Assume sizeof(long) == 4 */
		typedef  long	int32;
#	endif /* SIZEOF_LONG */
#endif /* HAVE_INT32 */

#ifndef DBL_MAX
#	define	DBL_MAX		((Float) 1.0e37)
#endif

#ifndef FLT_MAX
#	define	FLT_MAX		((float) 1.0e37)
#endif

#define	PROCESS_START_TIME		0

#ifdef WIN32
#	ifndef snprintf
#		define snprintf _snprintf
#	endif
#	ifndef vsnprintf
#		define vsnprintf _vsnprintf
#	endif
#endif

#define	LEFT_CHAN	0
#define RIGHT_CHAN	1

#if DSAM_USE_FLOAT
#	define	DSAM_EPSILON	FLT_EPSILON
#	define	DSAM_POW		powf
#else
#	define	DSAM_EPSILON	DBL_EPSILON
#	define	DSAM_POW		pow
#endif

/*
 * This next code is for UNICODE compilation options.
 */

#if DSAM_USE_UNICODE
#	if DSAM_USE_FLOAT
#		define DSAM_atof(X)		wcstof((X), NULL)
#	else
#		define DSAM_atof(X)		wcstod((X), NULL)
#	endif
#	define DSAM_atoi(X)		(int) wcstol((X), NULL, 0)
#	define DSAM_atol(X)		wcstol((X), NULL, 0)
#	define DSAM_fgetc		fgetwc
#	define DSAM_fgets		fgetws
#	define DSAM_fprintf		fwprintf
#	define DSAM_fopen(P, M)	(fopen(ConvUTF8_Utility_String(P), (M)))
#	define DSAM_fscanf		fwscanf
#	define DSAM_memcpy		wmemcpy
#	define DSAM_memmove		wmemmove
#	define DSAM_printf		wprintf
#	define DSAM_snprintf	swprintf
#	define DSAM_sscanf		swscanf
#	define DSAM_strcat		wcscat
#	define DSAM_strchr		wcschr
#	define DSAM_strcmp		wcscmp
#	define DSAM_strcpy		wcscpy
#	define DSAM_strlen		wcslen
#	define DSAM_strncat		wcsncat
#	define DSAM_strncmp		wcsncmp
#	define DSAM_strncpy		wcsncpy
#	define DSAM_strpbrk		wcspbrk
#	define DSAM_strrchr		wcsrchr
#	define DSAM_strstr		wcsstr
#	define DSAM_strtok		wcstok
#	define DSAM_toupper		towupper
#	define DSAM_ungetc		ungetwc
#	define DSAM_vfprintf	vfwprintf
#	define DSAM_vsnprintf	vswprintf
#	define STR_FMT			wxT("%S")
#else
#	define DSAM_atof		atof
#	define DSAM_atoi		atoi
#	define DSAM_atol		atol
#	define DSAM_fgetc		fgetwc
#	define DSAM_fgets		fgets
#	define DSAM_fprintf		fprintf
#	define DSAM_fopen(P, M)	(fopen((P), (M))
#	define DSAM_fscanf		fscanf
#	define DSAM_memcpy		memcpy
#	define DSAM_memmove		memmove
#	define DSAM_printf		printf
#	define DSAM_snprintf	snprintf
#	define DSAM_sscanf		sscanf
#	define DSAM_strcat		strcat
#	define DSAM_strchr		strchr
#	define DSAM_strcmp		strcmp
#	define DSAM_strcpy		strcpy
#	define DSAM_strlen		strlen
#	define DSAM_strncat		strncat
#	define DSAM_strncmp		strncmp
#	define DSAM_strncpy		strncpy
#	define DSAM_strpbrk		strpbrk
#	define DSAM_strrchr		strrchr
#	define DSAM_strstr		strstr
#	define DSAM_toupper		toupper
#	define DSAM_ungetc		ungetc
#	define DSAM_vfprintf	vfprintf
#	define DSAM_vsnprintf	vsnprintf
#	define STR_FMT			"%s"
#endif /* DSAM_USE_UNICODE */

/* ----------------------------------------------------------------------------
 * Making or using DSAM as a Windows DLL
 * ----------------------------------------------------------------------------
 */

#if defined(_WINDOWS)

	/* __declspec works in BC++ 5 and later, as well as VC++ and gcc */
#if defined(__MSVISUALC__) || defined(__BORLANDC__) || defined(__GNUC__)
#  ifdef DSAM_MAKING_DLL
#    define DSAM_API __declspec( dllexport )
#  elif defined(DSAM_USING_DLL)
#    define DSAM_API __declspec( dllimport )
#  else
#    define DSAM_API
#  endif
#else
#    define DSAM_API
#endif

#elif defined(__PM__)

#  if (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))

#    ifdef DSAM_MAKING_DLL
#      define DSAM_API _Export
#    elif defined(DSAM_USING_DLL)
#      define DSAM_API _Export
#    else
#      define DSAM_API
#    endif

#  else

#    define DSAM_API

#  endif

#else  /* !(MSW or OS2) */

#  define DSAM_API

#endif /* __WINDOWS */

/* ----------------------------------------------------------------------------
 * Making or using DSAM_ext as a Windows DLL
 * ----------------------------------------------------------------------------
 */

#if defined(_WINDOWS)

	/* __declspec works in BC++ 5 and later, as well as VC++ and gcc */
#if defined(__MSVISUALC__) || defined(__BORLANDC__) || defined(__GNUC__)
#  ifdef DSAM_MAKING_EXT_DLL
#    define DSAMEXT_API __declspec( dllexport )
#  elif defined(DSAM_USING_EXT_DLL)
#    define DSAMEXT_API __declspec( dllimport )
#  else
#    define DSAMEXT_API
#  endif
#else
#    define DSAMEXT_API
#endif

#elif defined(__PM__)

#  if (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))

#    ifdef DSAM_MAKING_EXT_DLL
#      define DSAMEXT_API _Export
#    elif defined(DSAM_USING_EXT_DLL)
#      define DSAMEXT_API _Export
#    else
#      define DSAMEXT_API
#    endif

#  else

#    define DSAMEXT_API

#  endif

#else  /* !(MSW or OS2) */

#  define DSAMEXT_API

#endif /* __WINDOWS */

/* ----------------------------------------------------------------------------
 * Making or using DSAM_g as a Windows DLL
 * ----------------------------------------------------------------------------
 */

#if defined(_WINDOWS)

	/* __declspec works in BC++ 5 and later, as well as VC++ and gcc */
#if defined(__MSVISUALC__) || defined(__BORLANDC__) || defined(__GNUC__)
#  ifdef DSAM_MAKING_G_DLL
#    define DSAMG_API __declspec( dllexport )
#  elif defined(DSAM_USING_G_DLL)
#    define DSAMG_API __declspec( dllimport )
#  else
#    define DSAMG_API
#  endif
#else
#    define DSAMG_API
#endif

#elif defined(__PM__)

#  if (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))

#    ifdef DSAM_MAKING_G_DLL
#      define DSAMG_API _Export
#    elif defined(DSAM_USING_G_DLL)
#      define DSAMG_API _Export
#    else
#      define DSAMG_API
#    endif

#  else

#    define DSAMG_API

#  endif

#else  /* !(MSW or OS2) */

#  define DSAMG_API

#endif /* __WINDOWS */

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

#define RMS_AMP(DBVALUE)		(20.0 * pow(10.0, (DBVALUE) / 20.0))

#define DB_SPL(RMS_AMPLITUDE)	(20.0 * log10((RMS_AMPLITUDE) / 20.0))

#define OCTAVE(F, X)	(pow(10, (X) * LOG_2 + log10(F)))

#define MSEC(SECONDS)			((SECONDS) * 1.0e+3)
#define MILLI(VALUE)			((VALUE) * 1.0e+3)
#define NANO(VALUE)				((VALUE) * 1.0e+9)
#define PICO(VALUE)				((VALUE) * 1.0e+12)

#define SIGN(X)					(((X) < 0.0)? -1: 1)
#define SQR(X) 					((X) * (X))

#define	DEGREES_TO_RADS(DEGREES)	((DEGREES) * PI / 180.0)
#define	RADS_TO_DEGREES(RADS)	((RADS) * 180.0 / PI)

#define	MINIMUM(A, B)			(((A) < (B))? (A): (B))

#define	MAXIMUM(A, B)			(((A) > (B))? (A): (B))

#define DBL_GREATER(A, B) (((A) - (B)) > DSAM_EPSILON)

#define IS_ABSOLUTE_PATH(S)		(((S)[0] == '/') || ((S)[1] == ':'))

#define	POSSIBLY_NULL_STRING_PTR(S)	(((S))? (S): UNSET_STRING)

#define ELAPSED_TIME(START, FINISH) ((Float) ((FINISH) - (START)) / \
		  CLOCKS_PER_SEC)

/*
 * This next definition is need for compiler niceties in MS Visual C++
 * otherwise it complains about "int being forces to 'true' or 'false'.
 */

#ifdef __cplusplus
#	define CXX_BOOL(A)		((A) != 0)
#endif

/*
 * __BEGIN_DECLS should be used at the beginning of all C declarations,
 * so that C++ compilers don't mangle their names.  Use __END_DECLS at
 * the end of C declarations.
 * This example was taken from the GNU libtool-1.12 documentation as a method
 * for ensuring that C libraries can be safely used with C++ code.
 */

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
# define __CONST_DECLS const
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
# define __CONST_DECLS /* empty */
#endif

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

#if !defined(BOOLEAN_ALGEBRA)
#	define BOOLEAN_ALGEBRA
	typedef	int	BOOLN;
#endif

typedef	unsigned long	ChanLen;	/* For the channel indices. */
#if DSAM_USE_FLOAT
	typedef	float	Float;
#else
	typedef	double	Float;
#endif
typedef	Float	ChanData;	/* Data type for channel data. */
typedef	unsigned short	uShort;
typedef	unsigned short	uInt;

#if DSAM_USE_UNICODE
	typedef wchar_t		WChar;		/* Type for unicode text */
	typedef wint_t		CInt;
#else
	typedef char		WChar;
	typedef int			CInt;
#endif

typedef enum {

	LOCAL,
	GLOBAL

} ParameterSpecifier;

typedef enum {

	COMMON_OFF_DIAG_MODE,
	COMMON_CONSOLE_DIAG_MODE,
	COMMON_DIALOG_DIAG_MODE

} DiagModeSpecifier;

typedef enum {

	COMMON_ERROR_DIAGNOSTIC,
	COMMON_WARNING_DIAGNOSTIC,
	COMMON_GENERAL_DIAGNOSTIC,
	COMMON_GENERAL_DIAGNOSTIC_WITH_CANCEL

} CommonDiagSpecifier;

typedef struct {

	BOOLN	appInitialisedFlag;	/* TRUE when application initialised. */
	BOOLN	segmentedMode;		/* TRUE, when in segmented mode. */
	BOOLN	usingGUIFlag;		/* TRUE when the GUI is being used. */
	BOOLN	lockGUIFlag;		/* TRUE when the GUI locker should be used. */
	BOOLN	usingExtFlag;		/* TRUE when the extensions are being used. */
	BOOLN	interruptRequestedFlag;	/* TRUE, when an interrupt is in process. */
	WChar	*diagnosticsPrefix;	/* Printed before diagnostics output. */
	WChar	*version;			/* Global version; shared library will show */
	WChar	*parsFilePath;		/* File path for parameter files. */
	int		notificationCount;	/* Count of notification diagnostics. */
	FILE	*warningsFile;		/* File to which warnings should be sent. */
	FILE	*errorsFile;		/* File to which errors should be sent. */
	FILE	*parsFile;			/* File for parameter listings. */
	DiagModeSpecifier	diagMode; /* Output form for diagnostics. */
	void	(* DPrint)(const WChar *, va_list);	/* Generic routine. */
	void 	(* Notify)(const WChar *, CommonDiagSpecifier);/*Gen. Rtn*/

} DSAM, *DSAMPtr;

typedef	enum {

	APPEND,
	OVERWRITE

} FileAccessSpecifier;

/******************************************************************************/
/*************************** Preference type definitions **********************/
/******************************************************************************/

typedef struct UniParListNode	*UniParListPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern DSAM	dSAM;

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	CheckInitErrorsFile_Common(void);

DSAM_API void	CheckInitParsFile_Common(void);

void	CheckInitWarningsFile_Common(void);

void	CloseFile(FILE *fp);

DSAM_API void	CloseFiles(void);

DSAM_API const WChar *	DiagnosticTitle(CommonDiagSpecifier type);

DSAM_API void	DPrint(const WChar *format, ...);

DSAM_API void	DPrintBuffer_Common(const WChar *format, va_list args,
				  void (* EmptyDiagBuffer)(WChar *, size_t *));

void	DPrintStandard(const WChar *format, va_list args);

void	FindFilePathAndName_Common(WChar *filePath, WChar *path, WChar *name);

void	FreeFloatArray_Common(Float **p);

DSAM_API DSAMPtr	GetDSAMPtr_Common(void);

FILE *	GetFilePtr(const WChar *outputSpecifier, FileAccessSpecifier mode);

DSAM_API WChar *	GetParsFileFPath_Common(WChar *parFile);

DSAM_API void	NotifyError(const WChar *format, ...);

void	NotifyStandard(const WChar *message, CommonDiagSpecifier type);

DSAM_API void	NotifyWarning(const WChar *format, ...);

void	ReadParsFromFile(WChar *fileName);		/* Used in test programs. */

DSAM_API void	ResetGUIDialogs(void);

BOOLN	ResizeFloatArray_Common(Float **array, int *oldLength, int length);

void	SetDiagnosticsPrefix(WChar *prefix);

DSAM_API void	SetDiagMode(DiagModeSpecifier mode);

DSAM_API void	SetDPrintFunc(void (* Func)(const WChar *, va_list));

DSAM_API void	SetErrorsFile_Common(const WChar *outputSpecifier, FileAccessSpecifier mode);

DSAM_API void	SetInterruptRequestStatus_Common(BOOLN status);

DSAM_API void	SetNotifyFunc(void (* Func)(const WChar *, CommonDiagSpecifier));

DSAM_API BOOLN	SetParsFile_Common(const WChar *outputSpecifier, FileAccessSpecifier mode);

DSAM_API void	SetParsFilePath_Common(WChar *name);

void	SetSegmentedMode(BOOLN setting);

DSAM_API void	SetUsingExtStatus(BOOLN status);

DSAM_API void	SetUsingGUIStatus(BOOLN status);

void	SetWarningsFile_Common(const WChar *outputSpecifier,
		  FileAccessSpecifier mode);

void	SwitchDiagnostics_Common(CommonDiagSpecifier specifier, BOOLN on);

DSAM_API void	SwitchGUILocking_Common(BOOLN on);

__END_DECLS

#endif
