/**********************
 *
 * File:		PaComUtils.h
 * Purpose:		This module contains the set command routines for the MPI
 *				parallel modules.
 * Comments:	.
 * Author:		
 * Created:		Jan 31 1995
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

#ifndef _PACOMUTILS_H
#define _PACOMUTILS_H 1

#include "GeCommon.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

typedef enum {

	PRINT_PARS,
	SET_AMPLITUDE,
	SET_DATA_FILE_NAME,
	SET_DELAY,
	SET_INDIVIDUAL_DEPTH,
	SET_INDIVIDUAL_FREQ,
	SET_INTENSITY,
	SET_FREQUENCY,
	SET_KDECAYTCONST,
	SET_MAXCONDUCTANCE,
	SET_MEMBRANETCONST,
	SET_PERCENTAM,
	SET_PERIOD,
	SET_PULSERATE,
	SET_RANSEED,
	WRITE_OUT_SIGNAL

} CommandSpecifier;

typedef enum {

	PA_INT,
	PA_DOUBLE,
	PA_LONG,
	PA_STRING,
	PA_VOID

} TypeSpecifier;

typedef enum {

	ALL_WORKERS,
	WORKER_CYCLIC

} ScopeSpecifier;

typedef	struct SetCom {

	WChar	*label;
	int		parCount;
	ScopeSpecifier		scope;
	CommandSpecifier	command;
	TypeSpecifier	type;
	union {
		WChar	*string;
		int		*intVal;
		double	*doubleVal;
		long	*longVal;
	} u;
	struct SetCom *next;

} SetCom, *SetComPtr;

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	AddToQueue_SetComUtil(SetComPtr *start, void *parameter, int ParCount,
		  TypeSpecifier type, WChar *label, CommandSpecifier command,
		  ScopeSpecifier scope);

void	PrintQueue_SetComUtil(SetComPtr p);

void	RemoveFromQueue_SetComUtil(SetComPtr *head);

void	RemoveQueue_SetComUtil(SetComPtr *head);

__END_DECLS

#endif
