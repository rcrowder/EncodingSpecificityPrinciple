/******************
 *		
 * File:		GrDisplayS.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"

#include "GrUtils.h"
#include "GrSimMgr.h"
#include "GrLines.h"
#include "GrModParDialog.h"
#include "DiSignalDisp.h"
#include "GrSDIFrame.h"
#include "GrCanvas.h"
#include "GrDisplayS.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "Bitmaps/dsam.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** DisplayS **************************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

BEGIN_EVENT_TABLE(DisplayS, wxFrame)
	EVT_MOVE(DisplayS::OnMove)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

DisplayS::DisplayS(wxFrame *frame, SignalDispPtr signalDispPtr): wxFrame(frame,
  -1, signalDispPtr->title, wxPoint(signalDispPtr->frameXPos,
  signalDispPtr->frameYPos), wxSize(signalDispPtr->frameWidth, signalDispPtr->
  frameHeight))
{
	wxLayoutConstraints	*c;

	// Load bitmaps and icons
	SetIcon(wxICON(dsam));
	
	mySignalDispPtr = signalDispPtr;

	// Create a new canvas
	canvas = new MyCanvas(this, signalDispPtr);
 	
	c = new wxLayoutConstraints;

	c->left.SameAs(this, wxLeft, 4);
	c->right.SameAs(this, wxRight, 4);
	c->top.SameAs(this, wxTop, 4);
	c->bottom.SameAs(this, wxBottom, 4);

	canvas->SetConstraints(c);

	mySignalDispPtr->critSect = new wxCriticalSection();
	mySignalDispPtr->display = this;
	
	wxGetApp().displays.Add(this);

}

/****************************** Destructor ************************************/

DisplayS::~DisplayS(void)
{
	delete canvas;
	mySignalDispPtr->display = NULL;
	wxGetApp().displays.Remove(this);

}

/****************************** SetDisplayTitle *******************************/

void
DisplayS::SetDisplayTitle(void)
{
	SetTitle(signalDispPtr->title);
	
}

/****************************** OnMove ****************************************/

void
DisplayS::OnMove(wxMoveEvent& event)
{
	wxPoint pos = event.GetPosition();
	GetSignalDispPtr()->frameXPos = pos.x;
	GetSignalDispPtr()->frameYPos = pos.y;
	GetSize(&GetSignalDispPtr()->frameWidth, &GetSignalDispPtr()->frameHeight);

    event.Skip();

}

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

/********************************* InitCriticalSection ***********************/

/*
 * This is the default routine for initialising the critical section mutex.
 */

void
InitCriticalSection_DisplayS(void)
{
	signalDispPtr->critSect = new wxCriticalSection;

}

/********************************* FreeCriticalSection ***********************/

/*
 * This is the default routine for deleting the critical section mutex.
 */

void
FreeCriticalSection_DisplayS(void)
{
	if (signalDispPtr->critSect)
		delete (wxCriticalSection *) signalDispPtr->critSect;

}

/****************************** GetWindowPosition ****************************/

/*
 * This is the default routine for setting the window position.
 */

void
GetWindowPosition_DisplayS(int *x, int *y)
{
	wxGetApp().GetDefaultDisplayPos(x, y);

}

/********************************* FreeDisplayFunc **************************/

/*
 * This is the default routine for setting the window position.
 */

void
FreeDisplay_DisplayS(void *display)
{
	if (!display)
		return;
	DisplayS *disp = (DisplayS *) display;
	delete disp;

}

/****************************** PostDisplayEvent ******************************/

/*
 * This routine is used by a thread to post a display event in a thread-safe
 * way.
 */

void
PostDisplayEvent_DisplayS(void)
{
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED,
	  SDIFRAME_SIM_THREAD_DISPLAY_EVENT);
	event.SetInt(MYAPP_THREAD_DRAW_GRAPH);
	event.SetClientData(signalDispPtr);
	wxPostEvent(wxGetApp().GetFrame(), event);

}

/**************************** ShowSignal **************************************/

/*
 * This this routine creates a canvas for a signal display chart.
 * At present it MUST BE USED IN LINE.
 * After initialisation the canvas' earObject is unregistered from the main
 * list.  This means that the canvas is responsible for deleting the memory
 * when it is finished with it.
 * If this module is in-line, then the output signal is set to the input
 * input signal.
 * The call to SetProcessContinuity must be done before the setting of the
 * display parameters, otherwise the correct time is not set for the display
 * in segmented mode.
 */

BOOLN
ShowSignal_DisplayS(EarObjectPtr data)
{

	static const WChar *funcName = wxT("ShowSignal_DisplayS");
	time_t	startTime;
	BOOLN	notReady;
	int		i, numWindowFrames;
	SignalDataPtr	outSignal;

	SET_PARS_POINTER(data);
	if (!data->threadRunFlag) {
		if (!CheckData_SignalDisp(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		if (!SetProcessMode_SignalDisp(data)) {
			NotifyError(wxT("%s: Could not set process mode."), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_SignalDisp(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	if (signalDispPtr->inLineProcess)
		SetProcessContinuity_EarObject(data);
	if (signalDispPtr->mode == GRAPH_MODE_OFF)
		return(TRUE);
	outSignal = _OutSig_EarObject(data);
	numWindowFrames = (GetDSAMPtr_Common()->segmentedMode)? 1: outSignal->
	  numWindowFrames;
	for (i = 0, startTime = time(NULL); i < numWindowFrames; i++) {
		if (signalDispPtr->buffer)
			ProcessBuffer_SignalDisp(outSignal, signalDispPtr->buffer, i);
#		if !defined(linux)
  	  	if (signalDispPtr->frameDelay > DSAM_EPSILON) {
  			while (difftime(time(NULL), startTime) < signalDispPtr->frameDelay)
  				;
  			startTime = time(NULL);
  		}
#		endif
		notReady = TRUE;
		while (notReady) {
			((wxCriticalSection *) signalDispPtr->critSect)->Enter();
			notReady = !signalDispPtr->drawCompletedFlag;
			((wxCriticalSection *) signalDispPtr->critSect)->Leave();
		}
		signalDispPtr->drawCompletedFlag = FALSE;
		((wxCriticalSection *) signalDispPtr->critSect)->Enter();
		SetSubDisplays_SignalDisp();
		signalDispPtr->data = (signalDispPtr->buffer)? signalDispPtr->buffer:
		  data;
		signalDispPtr->redrawGraphFlag = TRUE;
		((wxCriticalSection *) signalDispPtr->critSect)->Leave();
		PostDisplayEvent_DisplayS();
	}
	return(TRUE);

}
