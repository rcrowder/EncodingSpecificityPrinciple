/******************
 *		
 * File:		GrDisplayS.h
 * Purpose: 	Display class module.
 * Comments:	It was created by extraction from the original DiSignalDisp
 *				module.
 *				23-06-98 LPO: Introduced display window resizing:
 *				DisplayS::OnSize
 * Author:		L.P.O'Mard
 * Created:		27 Jul 1997
 * Updated:		23 Jun 1998
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

#ifndef	_GRDISPLAYS_H
#define _GRDISPLAYS_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*************************** Misc *********************************************/

#ifndef WXDIR
#	define	WXDIR						"$WXDIR"
#endif

#define	DISPLAY_PANEL_TEXT_LEN			5
#define	DISPLAY_MSW_PRINTER_X_SCALE		2.0
#define	DISPLAY_MSW_PRINTER_Y_SCALE		2.0

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/


/*************************** MyCanvas prefreeference **************************/

class MyCanvas;

/*************************** DisplayS *****************************************/

class DisplayS: public wxFrame
{
	SignalDispPtr	mySignalDispPtr;

 public:
	MyCanvas	*canvas;

	DisplayS(wxFrame *frame, SignalDispPtr signalDispPtr);
	~DisplayS(void);

	void	DeleteDisplay(void);
	SignalDispPtr	GetSignalDispPtr(void) { return(mySignalDispPtr); }
	void	SetDisplayTitle(void);
	void	OnMove(wxMoveEvent& event);

	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

void	InitCriticalSection_DisplayS(void);

void	FreeCriticalSection_DisplayS(void);

void	FreeDisplay_DisplayS(void *display);

void	GetWindowPosition_DisplayS(int *x, int *y);

BOOLN	ShowSignal_DisplayS(EarObjectPtr data);

#endif
