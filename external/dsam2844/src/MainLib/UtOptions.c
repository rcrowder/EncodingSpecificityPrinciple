/**********************
 *
 * File:		UtOptions.c
 * Purpose:		This module processes unix-type  options.
 * Comments:	18-03-99 LPO: I have introduced the 'optionsPleaseLink' global
 *				variable.  By accessing this from the GeModuleMgr routine, I
 *				ensure that link problems do not occur for the graphics library
 *				linking.
 * Author:		L. P. O'Mard
 * Created:		31 Jul 1998
 * Updated:		18 Mar 1999
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
#include <string.h>

#include "GeCommon.h"
#include "UtOptions.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

int		optionsPleaseLink = 0;

/****************************** Process ***************************************/

/*
 * Decode most reasonable forms of UNIX option syntax. Takes main()-
 * style argument indices (argc/argv) and a string of valid option
 * letters. Letters denoting options with arguments must be followed
 * by colons
 * With valid options, the routine returns the option letter and a pointer to
 * the associated argument (if any).
 * It sets the letter to '?' for bad options and missing arguments.
 * Returns 0 when no options remain, 
 * leaves giving "optind" indexing the first remaining argument.
 */

DSAM_API WChar
Process_Options(int argc, register WChar **argv, int *optind, int *optsub,
  WChar **argument, const WChar *proto)
{
	register WChar	*idx, c;

	*argument = NULL;
	if (*optind >= argc)
		return (0);
	if (*optsub == 0 && (argv[*optind][0] != '-' || argv[*optind][1] == '\0'))
		return (0);
	switch (c = argv[*optind][++(*optsub)]) {
	case '\0':
		++*optind;
		*optsub = 0;
		return (Process_Options(argc, argv, optind, optsub, argument, proto));
	case '-':
		++*optind;
		*optsub = 0;
		return (0);
	case ':':
		return ('?');
	}
	if ((idx = DSAM_strchr (proto, c)) == NULL)
		return ('?');
	if (idx[1] != ':')
		return (c);
	*argument = &argv[(*optind)++][++(*optsub)];
	*optsub = 0;
	if (*argument[0])
		return (c);
	if (*optind >= argc)
		return ('?');
	*argument = argv[(*optind)++];
	return (c);

}

/****************************** MarkIgnore ************************************/

/*
 * This routine marks an with the OPTIONS_IGNORE_OPTION character.
 * It expects the option to exist in the argument list.
 */

DSAM_API void
MarkIgnore_Options(int argc, register WChar **argv, const WChar *optionString,
  OptionsArgSpecifier state)
{
	int	i, length;

	length = DSAM_strlen(optionString);
	for (i = 1; (DSAM_strncmp(argv[i], optionString, length) != 0) && (i <
	  argc); i++)
		;
	argv[i][1] = (state == OPTIONS_NO_ARG)? OPTIONS_IGNORE_OPTION:
	  OPTIONS_IGNORE_ARG_OPTION;

}

