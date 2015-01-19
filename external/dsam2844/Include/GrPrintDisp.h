/**********************
 *
 * File:		GrPrintDisp.h
 * Purpose: 	Routines for printing the display window.
 * Comments:	Created to use the WxWin 2 printing routines.
 * Created:		20 Sep 1999
 * Updated:		
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

#ifndef _GRPRINTDISP_H
#define _GRPRINTDISP_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PRINTDISP_PS_X_OFFSET		50.0
#define	PRINTDISP_PS_Y_OFFSET		50.0
#define PRINTDISP_HEADER_OFFSET		20

#if defined(__WXMSW__)
#	define	PRINTDISP_X_SCALE				7.0
#	define	PRINTDISP_Y_SCALE				7.0
#else
#	define	PRINTDISP_X_SCALE				1.0
#	define	PRINTDISP_Y_SCALE				1.0
#endif /* __WXMSW__ */

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** PrintDisp *********************************/

class PrintDisp: public wxPrintout
{
	MyCanvas	*canvas;

  public:
	PrintDisp(MyCanvas *theCanvas, const wxChar *title = wxT("DSAM Print-out"));
	bool OnPrintPage(int page);
	bool HasPage(int page);
	bool OnBeginDocument(int startPage, int endPage);
	void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom,
	  int *selPageTo);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern wxPrintData *printData;

#endif
