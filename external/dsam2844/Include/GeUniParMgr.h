/******************
 *
 * File:		GeUniParMgr.h
 * Purpose:		This file contains the universal parameter manager routines
 *				I have got to check this.
 *				08-10-98 LPO: Fixed bug with name specifiers, by introducing
 *				the 'UNIPAR_NAME_SPEC' type: 'LINEAR' mode was being selected
 *				by the system when 'SINE' was asked for - I hadn't noticed that.
 *				12-10-98 LPO: Parameter lists that return structures, such as
 *				the CFList, will only carry out the setting function when all
 *				parameters have been set.
 *				06-11-98 LPO: Implemented the array handling for the likes of
 *				the StMPTone module.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for
 *				modules with no parameters.
 *				11-12-98 LPO: Introduced the second array index for the IC list.
 *				19-01-99 LPO: The 'arrayIndex[0]' always refers to the last
 *				(lowest?) index level, i.e. for the 'ICList' mode
 *				'arrayIndex[1]' refers to the ion ionChannel, and
 *				'arrayIndex[0]' refers to the respective ion channel parameter
 *				array element.
 *				27-01-99 LPO: Corrected problem in 'FindUniPar_' where it was
 *				returning before checking the rest of a parameter list after a
 *				sub module's parameter list.
 *				29-04-99 LPO: The 'FindUniPar_' routine can now find the
 *				'CFLIST' abbreviation so that the 'CFListPtr' pointer can be
 *				accessed.
 *				19-05-99 LPO: I have changed the 'UNIPAR_FILE_NAME' code so that
 *				I can save the 'defaultExtension' to be used with the GUI.
 *				30-05-99 LPO: The strings and file names are now printed
 *				surrounded by speech marks, '"'.
 *				02-06-99 LPO: I have updated the 'FindUniPar_' routine so that
 *				it now will find the parameters within a simulation script.
 *				03-06-99 LPO: Corrected the problem with 'SetParValue_' not
 *				treating the general list parameters correctly.
 *				08-09-99 LPO: In the 'FindUniPar_' routine only the sub-
 *				parameters for a module would be set, and not the module
 *				parameter file name itself.  This has been fixed.
 *				26-10-99 LPO: Introduced the 'enabled' flag for the 'UniPar'
 *				structure.  Printing, setting operations and such like will not
 *				be carried out on 'disabled' parameters.
 * Authors:		L. P. O'Mard
 * Created:		24 Sep 1998
 * Updated:		03 Jun 1999
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

#ifndef	_GEUNIPARMGR_H
#define _GEUNIPARMGR_H	1

#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtDynaList.h"
#include "UtIonChanList.h"
#include "UtParArray.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define UNIPAR_NAME_SEPARATOR		'.'
#define UNIPAR_INDEX_SEPARATOR		':'
#define UNIPAR_SUB_PAR_LIST_MARKER	wxT("  ")
#define UNIPAR_MAX_ARRAY_INDEX		2
#define	UNIPAR_TOP_PARENT_LABEL		wxT(".0")

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	UNIPAR_SEARCH_ABBR,
	UNIPAR_SEARCH_TYPE

} UniParSearchSpecifier;

typedef enum {

	UNIPAR_GENERAL_NULL,
	UNIPAR_GENERAL_ARRAY_INDEX_1,
	UNIPAR_GENERAL_ARRAY_INDEX_2

} UniParGeneralParSpecifier;

typedef enum {

	UNIPAR_UNKNOWN,
	UNIPAR_BOOL,
	UNIPAR_BOOL_ARRAY,
	UNIPAR_CFLIST,
	UNIPAR_FILE_NAME,
	UNIPAR_ICLIST,
	UNIPAR_INT,
	UNIPAR_INT_AL,
	UNIPAR_INT_ARRAY,
	UNIPAR_REAL,
	UNIPAR_REAL_ARRAY,
	UNIPAR_REAL_DYN_ARRAY,
	UNIPAR_STRING,
	UNIPAR_STRING_ARRAY,
	UNIPAR_LONG,
	UNIPAR_LONG_ARRAY,
	UNIPAR_MODULE,
	UNIPAR_NAME_SPEC,
	UNIPAR_NAME_SPEC_ARRAY,
	UNIPAR_NAME_SPEC_WITH_FILE,
	UNIPAR_NAME_SPEC_WITH_FPATH,
	UNIPAR_PARLIST,
	UNIPAR_PARARRAY,
	UNIPAR_SIMSCRIPT

} UniParTypeSpecifier;

typedef enum {

	UNIPAR_SET_CFLIST,
	UNIPAR_SET_GENERAL,
	UNIPAR_SET_IC,
	UNIPAR_SET_ICLIST,
	UNIPAR_SET_PARARRAY,
	UNIPAR_SET_SIMSPEC

} UniParModeSpecifier;

typedef struct UniPar {

	BOOLN	enabled;
	uInt	index;
	const WChar	*abbr;
	const WChar	*altAbbr;
	const WChar	*desc;

	UniParModeSpecifier	mode;
	UniParTypeSpecifier	type;
	union {

		int	*		i;
		long *		l;
		Float *	r;
		WChar *		s;
		CFListPtr	*cFPtr;
		IonChanListPtr	*iCPtr;
		ParArrayPtr		*pAPtr;
		struct {
			UniParListPtr	*list;
			EarObjectPtr	*process;
		} parList;
		struct {
			int	*			specifier;
			NameSpecifier	*list;
		} nameList;
		struct {
			WChar	*name;
			WChar	*defaultExtension;
		} file;
		struct {
			WChar 	*parFile;
			UniParListPtr	parList;
		} module;
		struct {
			WChar 	*fileName;
			DatumPtr	*simulation;
		} simScript;
		struct {
			int	*		numElements;
			int			index;
			union {
				WChar ***	s;
				int	**		i;
				long **		l;
				Float **	r;
				struct {
					int	**			specifier;
					NameSpecifier	*list;
				} nameList;
			} pPtr;
		} array;
	} valuePtr;
	union {

		BOOLN	(* SetInt)(int);
		BOOLN	(* SetIntArrayElement)(int, int);
		BOOLN	(* SetLong)(long);
		BOOLN	(* SetReal)(Float);
		BOOLN	(* SetRealArrayElement)(int, Float);
		BOOLN	(* SetString)(const WChar *);
		BOOLN	(* SetStringArrayElement)(int, WChar *);
		BOOLN	(* SetCFList)(CFListPtr);
		BOOLN	(* SetICList)(IonChanListPtr);
		BOOLN	(* SetDatumPtr)(DatumPtr);

		BOOLN	(* SetCFListInt)(CFListPtr, int);
		BOOLN	(* SetCFListReal)(CFListPtr, Float);
		BOOLN	(* SetCFListRealArrayElement)(CFListPtr, int, Float);
		BOOLN	(* SetCFListString)(CFListPtr, const WChar *);

		BOOLN	(* SetICListInt)(IonChanListPtr, int);
		BOOLN	(* SetICListReal)(IonChanListPtr, Float);
		BOOLN	(* SetICListString)(IonChanListPtr, const WChar *);

		BOOLN	(* SetICInt)(IonChannelPtr, int);
		BOOLN	(* SetICReal)(IonChannelPtr, Float);
		BOOLN	(* SetICRealArrayElement)(IonChannelPtr, int, Float);
		BOOLN	(* SetICString)(IonChannelPtr, const WChar *);

		BOOLN	(* SetParArrayInt)(ParArrayPtr, int);
		BOOLN	(* SetParArrayRealArrayElement)(ParArrayPtr, int, Float);
		BOOLN	(* SetParArrayString)(ParArrayPtr, const WChar *);

	} FuncPtr;

} UniPar, *UniParPtr;

typedef struct UniParListNode {

	UniParModeSpecifier	mode;

	BOOLN	updateFlag;
	int		notebookPanel;
	uInt	numPars;
	UniPar	*pars;
	union {

		CFListPtr		cFs;
		IonChanListPtr	iCs;
		struct {
			ParArrayPtr	ptr;
			BOOLN		(* SetFunc)(ParArrayPtr);
		} parArray;

	} handlePtr;

	NameSpecifier * (* GetPanelList)(int);

} UniParList;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DSAM_API BOOLN	CheckParList_UniParMgr(UniParListPtr list);

int		Cmp_UniParMgr(UniParPtr p, void *item, UniParSearchSpecifier mode);

DSAM_API UniParPtr	FindUniPar_UniParMgr(UniParListPtr *parList,
					  const WChar *parName, UniParSearchSpecifier mode);

WChar *	FormatArrayString_UniParMgr(UniParPtr p, int index, WChar *suffix);

DSAM_API WChar *	FormatPar_UniParMgr(UniParPtr p, WChar *suffix);

DSAM_API void	FreeList_UniParMgr(UniParListPtr *list);

DSAM_API WChar *	GetParString_UniParMgr(UniParPtr p);

DSAM_API UniParListPtr	InitList_UniParMgr(UniParModeSpecifier mode, int numPars,
				  void *handlePtr);

BOOLN	ParseArrayValue_UniParMgr(UniParPtr par, const WChar *parValue,
		  WChar **parValuePtr, int *index);

BOOLN	PrintPar_UniParMgr(UniParPtr p, WChar *prefix, WChar *suffix);

BOOLN	PrintPars_UniParMgr(UniParListPtr list, WChar *prefix, WChar *suffix);

DSAM_API BOOLN	PrintParList_UniParMgr(UniParListPtr parList);

void	PrintSubParList_UniParMgr(UniParListPtr parList);

DSAM_API BOOLN	PrintValue_UniParMgr(UniParPtr p);

BOOLN	ResetCFList_UniParMgr(UniParListPtr parList);

BOOLN	ResizeFloatArray_UniParMgr(Float **array, int *oldLength, int length);

void	SetAltAbbreviation_UniParMgr(UniParPtr p, WChar *altAbbr);

BOOLN	SetCFListParValue_UniParMgr(UniParListPtr *parList, uInt index,
		  const WChar *parValue);

BOOLN	SetCurrentIC_UniParMgr(IonChanListPtr theICs, const WChar *description);

BOOLN	SetGeneralParValue_UniParMgr(UniParListPtr parList, uInt index,
		  const WChar *parValue);

BOOLN	SetICEnabled_IonChanList(IonChannelPtr theIC, WChar *theICEnabled);

BOOLN	SetICListParValue_UniParMgr(UniParListPtr *parList, uInt index,
		  const WChar *parValue);

BOOLN	SetICParValue_UniParMgr(UniParListPtr parList, uInt index,
		  const WChar *parValue);

BOOLN	SetGetPanelListFunc_UniParMgr(UniParListPtr list,
		  NameSpecifier * (* Func)(int));

BOOLN	SetParArrayParValue_UniParMgr(UniParListPtr *parList, uInt index,
		  const WChar *parValue);

DSAM_API BOOLN	SetParValue_UniParMgr(UniParListPtr *parList, uInt index,
				  const WChar *parValue);

DSAM_API void	SetPar_UniParMgr(UniParPtr par, const WChar *abbreviation, const WChar *description,
				  UniParTypeSpecifier type, void *ptr1, void *ptr2, void * (* Func));

BOOLN	SetRealParValue_UniParMgr(UniParListPtr *parList, uInt index,
		  Float parValue);

BOOLN	WriteParFile_UniParMgr(WChar *fileName, UniParListPtr parList);

__END_DECLS

#endif
