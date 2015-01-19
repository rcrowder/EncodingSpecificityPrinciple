/**********************
 *
 * File:		UtSSParser.y
 * Purpose: 	Module parser.
 * Comments:	28-08-97 LPO: Added the UTSSPARSER_H symbol so that it would
 *				appear in the header file and allow the use of pre-compiled
 *				headers.
 * Author:		L.P.O'Mard
 * Created:		06 Nov 1995
 * Updated:		28 Aug 1997
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
 
/******************************************************************************/
/****************************** Grammar ***************************************/
/******************************************************************************/

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtString.h"
#include "UtSSSymbols.h"
#include "UtDynaList.h"
#include "UtSimScript.h"

int		yyerror(char *);
int		IgnoreToEndOfLine(void);
#define	yylex	yylex
int		yylex(void);

#if defined(CRL_mac) && !defined(alloca) 
#	define	alloca	malloc
#endif
#if defined(BORLANDC) && !defined(alloca) 
#	define	alloca	malloc
#endif

%}
%union {				/* stack type */
	Symbol	*sym;
	int		num;
	Datum	*inst;
	DynaListPtr	node;
}
%token	<sym>	_UTSSPARSER_H REPEAT BEGIN STOP STRING QUOTED_STRING RESET
%token	<num>	NUMBER PROCESS
%type	<inst>	process_specifier process reset repeat
%type	<inst>	simulation simulation_name labelled_process
%type	<node>	connection_list
%%
list:		/* nothing */
		|	list '\n'
		|	list simulation_list '\n'
		|	list par_statement_list simulation_list '\n'
		|	list error '\n'			{ yyerrok; }
	;
par_statement_list:
			par_statement
		|	par_statement_list par_statement
	;
par_statement:
			STRING STRING
			{	if (!SetUniParValue_Utility_SimScript($1->name, $2->name))
					return 1;
				IgnoreToEndOfLine();
			}
		|	STRING QUOTED_STRING
			{	if (!SetUniParValue_Utility_SimScript($1->name, $2->name))
					return 1;
				IgnoreToEndOfLine();
			}
	;
simulation_list:
			simulation	{ ;}
		|	simulation_list simulation
	;
simulation:
			BEGIN '{' statement_list '}'
			{ if (!InitialiseEarObjects_Utility_SimScript()) {
				NotifyError_Utility_SimScript(wxT("parser: error for ")
				  wxT("simulation."));
				return 1;
			  }
			  if (!simScriptPtr->subSimList)
				return 0;
			}
		|	BEGIN simulation_name '{' statement_list '}'
			{ $$ = $2;
			  if (!InitialiseEarObjects_Utility_SimScript()) {
				NotifyError_Utility_SimScript(wxT("parser: error for ")
				  wxT("simulation '%s'."), $2->u.proc.parFile);
				return 1;
			  }
			  if (!simScriptPtr->subSimList)
				return 0;
			}
	;
simulation_name:
			STRING
			{ if (!simScriptPtr->subSimList) {
			    $$ = *simScriptPtr->simPtr;
				DSAM_strcpy(simScriptPtr->simFileName, $1->name);
				
			  } else {
			  $$ = (DatumPtr) Pull_Utility_DynaList(&simScriptPtr->subSimList);
			  if (DSAM_strcmp($$->u.proc.parFile, $1->name) != 0) {
			    NotifyError_Utility_SimScript(wxT("parser: '%s' simulation ")
				  wxT("script does not correspond with '%s' ."), $$->u.proc.
				  parFile, $1->name);
			    return 1;
			  }
			  simScriptPtr->simPtr = GetSimScriptSimulation_Utility_SimScript(
			   $$);
			}
			}
		;
statement_list:
			statement
		|	statement_list statement
	;
statement:
			process_specifier
			{ $1->u.proc.parFile = InitString_Utility_String(NO_FILE); }
		|	process_specifier '<' STRING
			{ $1->u.proc.parFile = InitString_Utility_String($3->name); }
		|	process_specifier '<' '(' STRING ')'
			{ $1->u.proc.parFile = InitString_Utility_String($4->name);
			  Append_Utility_DynaList(&simScriptPtr->subSimList, $1);
			}
		|	process_specifier '<' STRING QUOTED_STRING
			{ $1->u.proc.parFile = InitString_Utility_String($3->name); }
			/* Ignore the QUOTED_STRING at present. */
		|	repeat NUMBER statement
			{	$1->u.loop.count = $2;
				$1->u.loop.stopPC = InstallInst_Utility_Datum(simScriptPtr->
				  simPtr, STOP);
			}
		|	reset STRING
			{ $1->u.ref.string = InitString_Utility_String($2->name); }
		|	'{' statement_list '}'
	;
process:
			STRING
			{	if (($$ = InstallProcessInst_Utility_SimScripts($1->name)) ==
				  NULL)
			       return 1;
			}
		| '@' STRING
			{	if (($$ = InstallProcessInst_Utility_SimScripts($2->name)) ==
				  NULL)
			       return 1;
				$$->onFlag = FALSE;
			}
	;
labelled_process:
			STRING '%' process
			{ $$ = $3;
			  $3->label = InitString_Utility_String($1->name);
			}
	;	
connection_list:
			STRING
			{	$$ = Append_Utility_DynaList(NULL, $1->name); }
		|	connection_list ',' STRING
			{	$$ = $1;
				Append_Utility_DynaList(&$1, $3->name);
			}
	;
process_specifier:
			process
		|	labelled_process
		|	labelled_process '(' connection_list '-' '>' ')'
				{ $$ = $1;
				  $1->u.proc.inputList = $3;
				}
		|	labelled_process '(' '-' '>' connection_list ')'
				{ $$ = $1;
				  $1->u.proc.outputList = $5;
				}
		|	labelled_process '(' connection_list '-' '>' connection_list ')'
				{ $$ = $1;
				  $1->u.proc.inputList = $3;
				  $1->u.proc.outputList = $6;
				}
	;
repeat:	
			REPEAT
			{	$$ = InstallInst_Utility_Datum(simScriptPtr->simPtr, REPEAT);
			}
		|	STRING '%' REPEAT
			{	$$ = InstallInst_Utility_Datum(simScriptPtr->simPtr, REPEAT);
				$$->label = InitString_Utility_String($1->name);
			}
	;
reset:	
			RESET
			{	$$ = InstallInst_Utility_Datum(simScriptPtr->simPtr, RESET);
			}
		|	STRING '%' RESET
			{	$$ = InstallInst_Utility_Datum(simScriptPtr->simPtr, RESET);
				$$->label = InitString_Utility_String($1->name);
			}
	;
%%

/******************************************************************************/
/****************************** Includes **************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Macro Definitions *****************************/
/******************************************************************************/

#define	IS_FILE_PATH_CHAR(C)	((isalnum ((C)) ||(C) == '.') || ((C) == \
								'_') || ((C) == '/') || ((C) == ':'))

/******************************************************************************/
/****************************** Gobal variables *******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** yyerror ***************************************/

/*
 * This function is called for yacc syntax error.
 */

int
yyerror(char *s)
{
	WChar	*string;
#	if DSAM_USE_UNICODE
	WChar	newString[LONG_STRING];
	DSAM_strcpy(newString, MBSToWCS_Utility_String(s));
	string = newString;
#	else
	string = s;
#	endif
	NotifyError_Utility_SimScript(wxT("%s"), string);
	return 1;
}

/****************************** IgnoreToEndOfLine *****************************/

/*
 * This function ignores characters until the end of the line.
 */

int
IgnoreToEndOfLine(void)
{
	int		c;

	while (((c = fgetc(simScriptPtr->fp)) != '\n') && (c != EOF))
			;
	return(c);

}

/****************************** yylex *****************************************/

/*
 * This routine is the lexical analyser.
 * It will install multiple symbols if the first instance of the symbol
 * is a custom reserved word.
 */

int
yylex(void)
{
	static const WChar *funcName = wxT("yylex");
	int		c;
	
	while (isspace((c = fgetc(simScriptPtr->fp))))
		if (c == '\n')
			simScriptPtr->lineNumber++;
	while (c == '#') {				 /* Check for comments. */
		c = IgnoreToEndOfLine();
		for ( ; isspace(c); c = fgetc(simScriptPtr->fp))
			if (c == '\n')
				simScriptPtr->lineNumber++;
	}
	if (c == EOF)
		return 0;
	if (isdigit(c)) {	/* number */
		int		d;
		ungetc(c, simScriptPtr->fp);
		fscanf(simScriptPtr->fp, "%d", &d);
		yylval.num = d;
		return NUMBER;
	}
	if (c == '"') {
		Symbol *s;
		char	sbuf[LONG_STRING], *p = sbuf;
		while ((c = fgetc(simScriptPtr->fp)) != '"' && (c != EOF)) {
			if (p >= sbuf + LONG_STRING - 1) {
				*p = '\0';
				NotifyError_Utility_SimScript(wxT("%s: String in quotes is ")
				  wxT("too long (%s)"), funcName, MBSToWCS_Utility_String(
				  sbuf));
				exit(1);
			}
			*p++ = c;
		}
		*p = '\0';
		if (c == EOF) {
			NotifyError_Utility_SimScript(wxT("%s: File ends before ")
			  wxT("terminating quotes."), funcName);
			exit(1);
		}
		s = InstallSymbol_Utility_SSSymbols(&simScriptPtr->symList,
		  MBSToWCS_Utility_String(sbuf),QUOTED_STRING);
		yylval.sym = s;
		return QUOTED_STRING;
	}		
	if (IS_FILE_PATH_CHAR(c)) {
		Symbol *s;
		char	sbuf[LONG_STRING], *p = sbuf;
		WChar	*wSbuf;
		do {
			if (p >= sbuf + LONG_STRING - 1) {
				*p = '\0';
				NotifyError_Utility_SimScript(wxT("%s: Name too long (%s)"),
				  funcName, MBSToWCS_Utility_String(sbuf));
				exit(1);
			}
			*p++ = c;
		} while ((c = fgetc(simScriptPtr->fp)) != EOF && IS_FILE_PATH_CHAR(c) );
		ungetc(c, simScriptPtr->fp);
		*p = '\0';
		wSbuf = MBSToWCS_Utility_String(sbuf);
		if ((s = LookUpSymbol_Utility_SSSymbols(simScriptPtr->symList,
		  wSbuf)) == 0)
			s = InstallSymbol_Utility_SSSymbols(&simScriptPtr->symList, wSbuf,
			  STRING);
		else
			if ((s->type == STRING) || (s->type == QUOTED_STRING))
				s = InstallSymbol_Utility_SSSymbols(&simScriptPtr->symList,
				  wSbuf, STRING);
		yylval.sym = s;
		return s->type;
	}
	return c;

}
