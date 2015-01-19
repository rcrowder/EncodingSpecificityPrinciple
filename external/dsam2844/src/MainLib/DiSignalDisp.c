/******************
 *
 * File:		DiSignalDisp.c
 * Purpose: 	This graphics module serves as the interface between DSAM
 *				and the WxWin GUI.
 * Comments:	The "ReadPars" routine for this module is different from the
 *				standard used by most of the other modules.
 *				21-10-97 LPO: Added absolute scaling mode.
 *				27-10-97 LPO: Added parameter flags so that only those
 *				parameters which have been set will be changed from their
 *				current/default values.
 *				27-10-97 LPO: Summary EarObject is only calculated when the
 *				summary display mode is on (default: off).
 *				12-11-97 LPO: Moved the SetProcessContinuity_.. call to before
 *				the setting of the display, otherwise the correct time is not
 *				shown in segmented mode (particularly for the second segment).
 *				26-11-97 LPO: Using new GeNSpecLists module for bool
 *				parameters.
 *				27-05-98 LPO: The default value of the summary display is now
 *				"OFF".
 *				28-06-98 LPO: Introduced the 'reduceChansInitialised' otherwise
 *				the Free_Utility_ReduceChannels(...) routine could be called
 *				with an undefined reduceChanPtr structure.
 *				30-06-98 LPO: The width parameter value is overridden when a
 *				signal with multiple windows is displayed.
 *				07-10-98 LPO: Changed some parameter names to correspond with
 *				the old AIM names.
 *				31-12-98 LPO: Problem with the frameDelay code causing a
 *				segmentation fault with egcs-1.1.1  What's the problem?
 * Author:		L.P.O'Mard
 * Created:		06 Jan 1995
 * Updated:		31 Dec 1998
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtString.h"
#include "DiSignalDisp.h"

#ifndef DEBUG
#	define	DEBUG
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

DSAM_API SignalDispPtr	signalDispPtr = NULL;
SignalDispDefFuncs	signalDispDefFuncs = {
						FreeCriticalSection_SignalDisp,
						FreeDisplay_SignalDisp,
						GetWindowPosition_SignalDisp,
						InitCriticalSection_SignalDisp,
						RunModel_ModuleMgr_Null
					};

/******************************************************************************/
/*************************** NonGlobal declarations ***************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/****************************** SetFuncGetWindowPosition **********************/

/*
 * This routine sets the GetWindowPosition function.
 */

DSAM_API void
SetFuncGetWindowPosition_SignalDisp(void (* Func)(int *, int *))
{
	signalDispDefFuncs.GetWindowPosition = Func;
}

/****************************** SetFuncInitCriticalSection ********************/

/*
 * This routine sets the InitCriticalSection function.
 */

DSAM_API void
SetFuncInitCriticalSection_SignalDisp(void (* Func)(void))
{
	signalDispDefFuncs.InitCriticalSection = Func;
}

/****************************** SetFuncFreeCriticalSection ********************/

/*
 * This routine sets the FreeCriticalSection function.
 */

DSAM_API void
SetFuncFreeCriticalSection_SignalDisp(void (* Func)(void))
{
	signalDispDefFuncs.FreeCriticalSection = Func;
}

/****************************** SetFuncFreeDisplay ****************************/

/*
 * This routine sets the FreeDisplay function.
 */

DSAM_API void
SetFuncFreeDisplay_SignalDisp(void (* Func)(void *))
{
	signalDispDefFuncs.FreeDisplay = Func;
}

/****************************** SetFuncShowSignal *****************************/

/*
 * This routine sets the ShowSignal function.
 */

DSAM_API void
SetFuncShowSignal_SignalDisp(BOOLN (* Func)(EarObjectPtr))
{
	signalDispDefFuncs.ShowSignal = Func;
}

/****************************** PanelList *************************************/

/*
 * This routine returns the name specifier for the display panel list.
 */

NameSpecifier *
GetPanelList_SignalDisp(int index)
{
	static NameSpecifier	list[] = {

				{ (WChar *) wxT("Signal"),	DISPLAY_MAGNIFICATION },
				{ (WChar *) wxT("Y-axis"),	DISPLAY_Y_AXIS_TITLE },
				{ (WChar *) wxT("X-axis"),	DISPLAY_X_AXIS_TITLE },
				{ (WChar *) wxT("General"),	DISPLAY_WINDOW_TITLE },
				{ NULL, 			DISPLAY_NULL }
			};
	;
	return(&list[index]);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the mode list array.
 */

BOOLN
InitModeList_SignalDisp(void)
{
	static NameSpecifier	list[] = {

					{ (WChar *) wxT("OFF"),			GRAPH_MODE_OFF },
					{ (WChar *) wxT("LINE"),			GRAPH_MODE_LINE },
					{ (WChar *) wxT("GREY_SCALE"),	GRAPH_MODE_GREY_SCALE },
					{ NULL,					GRAPH_MODE_NULL }
				};
	signalDispPtr->modeList = list;
	return(TRUE);

}

/****************************** InitYAxisModeList *****************************/

/*
 * This routine intialises the y-axis mode list array.
 */

BOOLN
InitYAxisModeList_SignalDisp(void)
{
	static NameSpecifier	list[] = {

					{ (WChar *) wxT("AUTO"),			GRAPH_Y_AXIS_MODE_AUTO },
					{ (WChar *) wxT("CHANNEL"),		GRAPH_Y_AXIS_MODE_CHANNEL },
					{ (WChar *) wxT("LINEAR_SCALE"),	GRAPH_Y_AXIS_MODE_LINEAR_SCALE },
					{ NULL, 				GRAPH_Y_AXIS_MODE_NULL }
				};
	signalDispPtr->yAxisModeList = list;
	return(TRUE);

}

/****************************** InitYNormModeList *****************************/

/*
 * This routine intialises the signal line mode list array.
 */

BOOLN
InitYNormModeList_SignalDisp(void)
{
	static NameSpecifier	list[] = {

					{(WChar *) wxT("MIDDLE"),		GRAPH_LINE_YNORM_MIDDLE_MODE },
					{(WChar *) wxT("BOTTOM"),		GRAPH_LINE_YNORM_BOTTOM_MODE },
					{NULL,				GRAPH_LINE_YNORM_MODE_NULL }
				};
	signalDispPtr->yNormModeList = list;
	return(TRUE);

}

/**************************** Init ********************************************/

/*
 * This routine initialises the SignalDisp module parameters to default values.
 * The GLOBAL option is for hard programming - it sets the module's pointer to
 * the global parameter structure and initialises the parameters.
 * The LOCAL option is for generic programming - it initialises the parameter
 * structure currently pointed to by the module's parameter pointer.
 */

BOOLN
Init_SignalDisp(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_SignalDisp");

	if (parSpec == GLOBAL) {
		if (signalDispPtr != NULL)
			Free_SignalDisp();
		if ((signalDispPtr = (SignalDispPtr) malloc(sizeof(SignalDisp))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName,
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (signalDispPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName,
			  funcName);
			return(FALSE);
		}
	}

	signalDispPtr->updateProcessVariablesFlag = TRUE;
	signalDispPtr->parSpec = parSpec;
	signalDispPtr->autoYScale = GENERAL_BOOLEAN_ON;
	signalDispPtr->frameDelay = DISPLAY_DEFAULT_FRAME_DELAY;
	signalDispPtr->frameHeight = DISPLAY_DEFAULT_FRAME_HEIGHT;
	signalDispPtr->frameWidth = DISPLAY_DEFAULT_FRAME_WIDTH;
	(signalDispDefFuncs.GetWindowPosition)(&signalDispPtr->frameXPos,
	  &signalDispPtr->frameYPos);
	signalDispPtr->yAxisMode = GRAPH_Y_AXIS_MODE_AUTO;
	signalDispPtr->yNormalisationMode = GRAPH_LINE_YNORM_MIDDLE_MODE;
	signalDispPtr->mode = GRAPH_MODE_LINE;
	signalDispPtr->numGreyScales = GRAPH_NUM_GREY_SCALES;
	signalDispPtr->magnification = DEFAULT_SIGNAL_Y_SCALE;
	signalDispPtr->summaryDisplay = GENERAL_BOOLEAN_OFF;
	signalDispPtr->chanActivityDisplay = GENERAL_BOOLEAN_OFF;
	signalDispPtr->topMargin = GRAPH_TOP_MARGIN_PERCENT;
	signalDispPtr->width = DEFAULT_WIDTH;
	signalDispPtr->channelStep = DEFAULT_CHANNEL_STEP;
	signalDispPtr->xTicks = DEFAULT_X_TICKS;
	DSAM_strcpy(signalDispPtr->xNumberFormat, DEFAULT_X_NUMBER_FORMAT);
	DSAM_strcpy(signalDispPtr->yNumberFormat, DEFAULT_Y_NUMBER_FORMAT);
	signalDispPtr->xDecPlaces = DEFAULT_X_DEC_PLACES;
	signalDispPtr->yDecPlaces = DEFAULT_Y_DEC_PLACES;
	signalDispPtr->yTicks = DEFAULT_Y_TICKS;
	signalDispPtr->yInsetScale = GENERAL_BOOLEAN_ON;
	signalDispPtr->maxY = 0.0;
	signalDispPtr->minY = 0.0;
	signalDispPtr->title[0] = '\0';
	signalDispPtr->xAxisTitle[0] = '\0';
	signalDispPtr->yAxisTitle[0] = '\0';
	signalDispPtr->xResolution = DEFAULT_X_RESOLUTION;
	signalDispPtr->autoXScale = GENERAL_BOOLEAN_ON;
	signalDispPtr->xExtent = -1.0;
	signalDispPtr->xOffset = 0.0;

	InitModeList_SignalDisp();
	InitYAxisModeList_SignalDisp();
	InitYNormModeList_SignalDisp();
	if (!SetUniParList_SignalDisp()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_SignalDisp();
		return(FALSE);
	}
	signalDispPtr->bufferCount = 0;
	signalDispPtr->buffer = NULL;
	signalDispPtr->data = NULL;
	signalDispPtr->summary = NULL;
	signalDispPtr->chanActivity = NULL;
	signalDispPtr->inLineProcess = FALSE;
	signalDispPtr->redrawGraphFlag = FALSE;
	signalDispPtr->redrawSubDisplaysFlag = FALSE;
	signalDispPtr->drawCompletedFlag = TRUE;
	signalDispPtr->initialisationFlag = FALSE;
	signalDispPtr->display = NULL;
	signalDispPtr->critSect = NULL;

	return(TRUE);

}

/****************************** GetWindowPosition ****************************/

/*
 * This is the default routine for setting the window position.
 * This is for the non-gui version and does nothing important.
 */

void
GetWindowPosition_SignalDisp(int *x, int *y)
{
	x = y;

}

/********************************* InitCriticalSection ***********************/

/*
 * This is the default routine for initialising the critical section mutex.
 * This is for the non-gui version and does nothing important.
 */

void
InitCriticalSection_SignalDisp(void)
{

}

/********************************* FreeCriticalSection ***********************/

/*
 * This is the default routine for deleting the critical section mutex.
 * This is for the non-gui version and does nothing important.
 */

void
FreeCriticalSection_SignalDisp(void)
{

}

/********************************* FreeDisplay *******************************/

/*
 * This is the default routine for setting the window position.
 * This is for the non-gui version and does nothing important.
 */

void
FreeDisplay_SignalDisp(void *display)
{

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a bool value because the generic module
 * interface requires that a non-void value be returned.
 * This routine will not free anything if the 'registeredWithDisplayFlag' flag
 * is set.
 */

BOOLN
Free_SignalDisp(void)
{
	if (signalDispPtr == NULL)
		return(TRUE);
	FreeProcessVariables_SignalDisp();
	(signalDispDefFuncs.FreeDisplay)(signalDispPtr->display);
	if (signalDispPtr->parList)
		FreeList_UniParMgr(&signalDispPtr->parList);
	if (signalDispPtr->parSpec == GLOBAL) {
		free(signalDispPtr);
		signalDispPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */

BOOLN
SetUniParList_SignalDisp(void)
{
	static const WChar *funcName = wxT("SetUniParList_SignalDisp");
	UniParPtr	pars;

	if ((signalDispPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  DISPLAY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	SetGetPanelListFunc_UniParMgr(signalDispPtr->parList,
	  GetPanelList_SignalDisp);

	pars = signalDispPtr->parList->pars;
	SetPar_UniParMgr(&pars[DISPLAY_AUTOMATIC_SCALING], wxT("AUTO_SCALING"),
	  wxT("Automatic scaling ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &signalDispPtr->autoYScale, NULL,
	  (void * (*)) SetAutoYScale_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_AUTO_Y_SCALE], wxT("AUTO_Y_SCALE"),
	  wxT("Automatic y-axis scale ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &signalDispPtr->autoYScale, NULL,
	  (void * (*)) SetAutoYScale_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_CHANNEL_STEP], wxT("CHANNEL_STEP"),
	  wxT("Channel stepping mode."),
	  UNIPAR_INT,
	  &signalDispPtr->channelStep, NULL,
	  (void * (*)) SetChannelStep_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_TITLE], wxT("WIN_TITLE"),
	  wxT("Display window title."),
	  UNIPAR_STRING,
	  &signalDispPtr->title, NULL,
	  (void * (*)) SetTitle_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_FRAME_DELAY], wxT("FRAMEDELAY"),
	  wxT("Delay between display frames (s)"),
	  UNIPAR_REAL,
	  &signalDispPtr->frameDelay, NULL,
	  (void * (*)) SetFrameDelay_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MAGNIFICATION], wxT("MAGNIFICATION"),
	  wxT("Signal magnification."),
	  UNIPAR_REAL,
	  &signalDispPtr->magnification, NULL,
	  (void * (*)) SetMagnification_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MAX_Y], wxT("MAXY"),
	  wxT("Maximum Y value (for manual scaling)."),
	  UNIPAR_REAL,
	  &signalDispPtr->maxY, NULL,
	  (void * (*)) SetMaxY_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MIN_Y],wxT("MINY"),
	  wxT("Minimum Y Value (for manual scaling)."),
	  UNIPAR_REAL,
	  &signalDispPtr->minY, NULL,
	  (void * (*)) SetMinY_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_MODE], wxT("MODE"),
	  wxT("Display mode ('off', 'line' or 'gray_scale')."),
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->mode, signalDispPtr->modeList,
	  (void * (*)) SetMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_NUMGREYSCALES], wxT("NUMGREYSCALES"),
	  wxT("Number of grey scales."),
	  UNIPAR_INT,
	  &signalDispPtr->numGreyScales, NULL,
	  (void * (*)) SetNumGreyScales_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_SUMMARY_DISPLAY], wxT("SUMMARYDISPLAY"),
	  wxT("Summary display mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &signalDispPtr->summaryDisplay, NULL,
	  (void * (*)) SetSummaryDisplay_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_CHAN_ACTIVITY_DISPLAY], wxT(
	  "CHAN_ACTIVITY_DISPLAY"),
	  wxT("Channel activity display mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &signalDispPtr->chanActivityDisplay, NULL,
	  (void * (*)) SetChanActivityDisplay_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_TOP_MARGIN], wxT("TOPMARGIN"),
	  wxT("Top margin for display (percent of display height)."),
	  UNIPAR_REAL,
	  &signalDispPtr->topMargin, NULL,
	  (void * (*)) SetTopMargin_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WIDTH], wxT("WIDTH"),
	  wxT("Displayed signal width (seconds or x units)."),
	  UNIPAR_REAL,
	  &signalDispPtr->width, NULL,
	  (void * (*)) SetWidth_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_HEIGHT], wxT("WIN_HEIGHT"),
	  wxT("Display frame height (pixel units)."),
	  UNIPAR_INT,
	  &signalDispPtr->frameHeight, NULL,
	  (void * (*)) SetFrameHeight_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_WIDTH], wxT("WIN_WIDTH"),
	  wxT("Display frame width (pixel units)."),
	  UNIPAR_INT,
	  &signalDispPtr->frameWidth, NULL,
	  (void * (*)) SetFrameWidth_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_X_POS], wxT("WIN_X_POS"),
	  wxT("Display frame X position (pixel units)."),
	  UNIPAR_INT,
	  &signalDispPtr->frameXPos, NULL,
	  (void * (*)) SetFrameXPos_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_WINDOW_Y_POS], wxT("WIN_Y_POS"),
	  wxT("Display frame Y position (pixel units)."),
	  UNIPAR_INT,
	  &signalDispPtr->frameYPos, NULL,
	  (void * (*)) SetFrameYPos_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_RESOLUTION], wxT("X_RESOLUTION"),
	  wxT("Resolution of X scale (1 - low, fractions are higher)."),
	  UNIPAR_REAL,
	  &signalDispPtr->xResolution, NULL,
	  (void * (*)) SetXResolution_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_NORMALISATION_MODE], wxT("NORMALISATION"),
	  wxT("Y normalisation mode ('bottom' or 'middle')."),
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->yNormalisationMode, signalDispPtr->yNormModeList,
	  (void * (*)) SetYNormalisationMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_AXIS_TITLE], wxT("Y_AXIS_TITLE"),
	  wxT("Y-axis title."),
	  UNIPAR_STRING,
	  &signalDispPtr->yAxisTitle, NULL,
	  (void * (*)) SetYAxisTitle_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_AXIS_MODE], wxT("Y_AXIS_MODE"),
	  wxT("Y-axis mode ('channel' (No.) or 'scale')."),
	  UNIPAR_NAME_SPEC,
	  &signalDispPtr->yAxisMode, signalDispPtr->yAxisModeList,
	  (void * (*)) SetYAxisMode_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_NUMBER_FORMAT], wxT("Y_NUMBER_FORMAT"),
	  wxT("Y axis scale number format, (e.g. y.yye-3)."),
	  UNIPAR_STRING,
	  &signalDispPtr->yNumberFormat, NULL,
	  (void * (*)) SetYNumberFormat_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_DEC_PLACES], wxT("Y_DEC_PLACES"),
	  wxT("Y axis scale decimal places."),
	  UNIPAR_INT,
	  &signalDispPtr->yDecPlaces, NULL,
	  (void * (*)) SetYDecPlaces_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_TICKS], wxT("Y_TICKS"),
	  wxT("Y axis tick marks."),
	  UNIPAR_INT,
	  &signalDispPtr->yTicks, NULL,
	  (void * (*)) SetYTicks_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_Y_INSET_SCALE], wxT("Y_INSET_SCALE"),
	  wxT("Y inset scale mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &signalDispPtr->yInsetScale, NULL,
	  (void * (*)) SetYInsetScale_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_AXIS_TITLE], wxT("X_AXIS_TITLE"),
	  wxT("X axis title."),
	  UNIPAR_STRING,
	  &signalDispPtr->xAxisTitle, NULL,
	  (void * (*)) SetXAxisTitle_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_NUMBER_FORMAT], wxT("X_NUMBER_FORMAT"),
	  wxT("X axis scale number format, (e.g. x.xxe-3)."),
	  UNIPAR_STRING,
	  &signalDispPtr->xNumberFormat, NULL,
	  (void * (*)) SetXNumberFormat_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_DEC_PLACES], wxT("X_DEC_PLACES"),
	  wxT("X axis scale decimal places."),
	  UNIPAR_INT,
	  &signalDispPtr->xDecPlaces, NULL,
	  (void * (*)) SetXDecPlaces_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_TICKS], wxT("X_TICKS"),
	  wxT("X axis tick marks."),
	  UNIPAR_INT,
	  &signalDispPtr->xTicks, NULL,
	  (void * (*)) SetXTicks_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_AUTO_X_SCALE], wxT("AUTO_X_SCALE"),
	  wxT("Autoscale option for x-axis ('on' or 'off')"),
	  UNIPAR_BOOL,
	  &signalDispPtr->autoXScale, NULL,
	  (void * (*)) SetAutoXScale_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_OFFSET], wxT("X_OFFSET"),
	  wxT("X offset for display in zoom mode (x units)."),
	  UNIPAR_REAL,
	  &signalDispPtr->xOffset, NULL,
	  (void * (*)) SetXOffset_SignalDisp);
	SetPar_UniParMgr(&pars[DISPLAY_X_EXTENT], wxT("X_EXTENT"),
	  wxT("X extent for display in zoom mode (x units or -1 for end of ")
	    wxT("signal)."),
	  UNIPAR_REAL,
	  &signalDispPtr->xExtent, NULL,
	  (void * (*)) SetXExtent_SignalDisp);

	SetDefaulEnabledPars_SignalDisp();

	return(TRUE);

}

/********************************* SetDefaulEnabledPars ***********************/

/*
 * This routine sets the parameter list so that the correct default parameters
 * are enabled/disabled.
 */

BOOLN
SetDefaulEnabledPars_SignalDisp(void)
{
	static const WChar *funcName = wxT("SetDefaulEnabledPars_SignalDisp");
	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	signalDispPtr->parList->pars[DISPLAY_X_DEC_PLACES].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_Y_DEC_PLACES].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_WIDTH].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_AUTOMATIC_SCALING].enabled = FALSE;

	SetAutoYScale_SignalDisp((WChar *) BooleanList_NSpecLists(signalDispPtr->
	  autoYScale)->name);
	SetAutoXScale_SignalDisp((WChar *) BooleanList_NSpecLists(signalDispPtr->
	  autoXScale)->name);
	/* The following should not be set from the GUI */
	/*signalDispPtr->parList->pars[DISPLAY_WINDOW_HEIGHT].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_WINDOW_WIDTH].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_WINDOW_X_POS].enabled = FALSE;
	signalDispPtr->parList->pars[DISPLAY_WINDOW_Y_POS].enabled = FALSE;*/
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_SignalDisp(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(NULL);
	}
	if (signalDispPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(signalDispPtr->parList);

}
/****************************** SetAutoYScaleParsState ************************/

/*
 * This function sets enables or disables the auto Y scale relatated parameter.
 */

void
SetAutoYScaleParsState_SignalDisp(BOOLN state)
{
	signalDispPtr->parList->pars[DISPLAY_MAX_Y].enabled = state;
	signalDispPtr->parList->pars[DISPLAY_MIN_Y].enabled = state;
	if (signalDispPtr->yAxisMode == GRAPH_Y_AXIS_MODE_CHANNEL) {
		signalDispPtr->parList->pars[DISPLAY_Y_NUMBER_FORMAT].enabled = FALSE;
		signalDispPtr->parList->pars[DISPLAY_Y_TICKS].enabled = FALSE;
	} else {
		signalDispPtr->parList->pars[DISPLAY_Y_NUMBER_FORMAT].enabled = state;
		signalDispPtr->parList->pars[DISPLAY_Y_TICKS].enabled = state;
	}
	signalDispPtr->parList->updateFlag = TRUE;

}

/**************************** SetAutoYScale ***********************************/

/*
 * This routine sets the automatic scaling mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetAutoYScale_SignalDisp(WChar *theAutoYScale)
{
	static const WChar *funcName = wxT("SetAutoYScale_SignalDisp");
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAutoYScale,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  theAutoYScale);
		return(FALSE);
	}
	signalDispPtr->autoYScale = specifier;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	SetAutoYScaleParsState_SignalDisp(!signalDispPtr->autoYScale);
	return(TRUE);

}

/**************************** SetFrameDelay ***********************************/

/*
 * This routine sets the frame delay for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameDelay_SignalDisp(Float frameDelay)
{
	static const WChar *funcName = wxT("SetFrameDelay_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	signalDispPtr->frameDelay = frameDelay;
	return(TRUE);

}

/**************************** SetChannelStep **********************************/

/*
 * This routine sets the chennl step for the display.
 * This is a boolean response, hence only non-zero values are TRUE.
 */

BOOLN
SetChannelStep_SignalDisp(int channelStep)
{
	static const WChar *funcName = wxT("SetChannelStep_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	signalDispPtr->channelStep = channelStep;
	return(TRUE);

}

/**************************** SetFrameHeight **********************************/

/*
 * This routine sets the frame height for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameHeight_SignalDisp(int frameHeight)
{
	static const WChar *funcName = wxT("SetFrameHeight_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (frameHeight <= 0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g)"), funcName,
		  frameHeight);
		return(FALSE);
	}
	signalDispPtr->frameHeight = frameHeight;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetFrameWidth ***********************************/

/*
 * This routine sets the frame width for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameWidth_SignalDisp(int frameWidth)
{
	static const WChar *funcName = wxT("SetFrameWidth_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (frameWidth <= 0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g)"), funcName,
		  frameWidth);
		return(FALSE);
	}
	signalDispPtr->frameWidth = frameWidth;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetFrameXPos ************************************/

/*
 * This routine sets the frame x position for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameXPos_SignalDisp(int frameXPos)
{
	static const WChar *funcName = wxT("SetFrameXPos_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	signalDispPtr->frameXPos = frameXPos;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetFrameYPos ************************************/

/*
 * This routine sets the frame x position for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetFrameYPos_SignalDisp(int frameYPos)
{
	static const WChar *funcName = wxT("SetFrameYPos_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	signalDispPtr->frameYPos = frameYPos;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetYAxisTitle ***********************************/

/*
 * This routine sets the 'y_axis_title' field of the 'signalDisp' structure.
 */

BOOLN
SetYAxisTitle_SignalDisp(WChar *yAxisTitle)
{
	static const WChar *funcName = wxT("SetYAxisTitle_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(signalDispPtr->yAxisTitle, yAxisTitle, MAXLINE);
	return(TRUE);

}

/**************************** SetYAxisModeParsState ***************************/

/*
 * This routine sets the module's y-axis mode related parameters state.
 */

void
SetYAxisModeParsState_SignalDisp(BOOLN state)
{
	SetAutoXScaleParsState_SignalDisp(!state);
	signalDispPtr->parList->pars[DISPLAY_Y_INSET_SCALE].enabled = state;
	signalDispPtr->parList->updateFlag = TRUE;

}

/**************************** SetYAxisMode ************************************/

/*
 * This routine sets the module's y-axis mode.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetYAxisMode_SignalDisp(WChar *theYAxisMode)
{
	static const WChar *funcName = wxT("SetYAxisMode_SignalDisp");
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theYAxisMode,
	  signalDispPtr->yAxisModeList)) == GRAPH_MODE_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theYAxisMode);
		return(FALSE);
	}
	signalDispPtr->yAxisMode = specifier;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	SetYAxisModeParsState_SignalDisp(signalDispPtr->yAxisMode ==
	  GRAPH_Y_AXIS_MODE_CHANNEL);
	return(TRUE);

}

/**************************** SetYNormalisationMode ***************************/

/*
 * This routine sets the automatic y offset mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetYNormalisationMode_SignalDisp(WChar *theYNormalisationMode)
{
	static const WChar *funcName = wxT("SetYNormalisationMode_SignalDisp");
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theYNormalisationMode,
	  signalDispPtr->yNormModeList)) == GRAPH_LINE_YNORM_MODE_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  theYNormalisationMode);
		return(FALSE);
	}
	signalDispPtr->yNormalisationMode = specifier;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetXResolution **********************************/

/*
 * This routine sets the x resolution for the display.
 * The x resolution is used in the algorithm for optimising printing x values.
 * It prevents inefficient wxT("over-plotting") of points.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetXResolution_SignalDisp(Float xResolution)
{
	static const WChar *funcName = wxT("SetXResolution_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((xResolution < FLT_EPSILON) || (xResolution > 1.0)) {
		NotifyError(wxT("%s: Value must be in range %.2g - 1.0 (%g)"), funcName,
		  FLT_EPSILON, xResolution);
		return(FALSE);
	}
	signalDispPtr->xResolution = xResolution;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetMagnification ********************************/

/*
 * This routine sets the signal y scale for printing.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetMagnification_SignalDisp(Float magnification)
{
	static const WChar *funcName = wxT("SetMagnification_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (magnification < FLT_EPSILON) {
		NotifyError(wxT("%s: Illegal value (%g)"), funcName, magnification);
		return(FALSE);
	}
	signalDispPtr->magnification = magnification;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetMaxY *****************************************/

/*
 * This routine sets the maximum Y scale for use when that automatic scaling is
 * disabled.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetMaxY_SignalDisp(Float maxY)
{
	static const WChar *funcName = wxT("SetMaxY_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	signalDispPtr->maxY = maxY;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetMinY *****************************************/

/*
 * This routine sets the minimum Y scale for use when that automatic scaling is
 * disabled.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetMinY_SignalDisp(Float minY)
{
	static const WChar *funcName = wxT("SetMinY_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	signalDispPtr->minY = minY;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetTopMargin ************************************/

/*
 * This routine sets the percentage of the display height to use as the top
 * margin.
 */

BOOLN
SetTopMargin_SignalDisp(Float topMargin)
{
	static const WChar *funcName = wxT("SetTopMargin_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	signalDispPtr->topMargin = topMargin;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetWidth ****************************************/

/*
 * This routine sets the width for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetWidth_SignalDisp(Float width)
{
	static const WChar *funcName = wxT("SetWidth_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/* signalDispPtr->width = width; parameter to be removed. */
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetMode *****************************************/

/*
 * This routine sets the module's display mode.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetMode_SignalDisp(WChar *theMode)
{
	static const WChar *funcName = wxT("SetMode_SignalDisp");
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
	  signalDispPtr->modeList)) == GRAPH_MODE_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, theMode);
		return(FALSE);
	}
	if ((specifier == GRAPH_MODE_GREY_SCALE) && (signalDispPtr->autoYScale ==
	  GENERAL_BOOLEAN_OFF)) {
		NotifyError(wxT("%s: Automatic Y scaling must be on to use the grey-")
		  wxT("scale mode."), funcName);
		return(FALSE);
	}
	signalDispPtr->mode = specifier;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetNumGreyScales ********************************/

/*
 * This routine sets the number of grey scales in the 'grey_scale' display mode.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetNumGreyScales_SignalDisp(int theNumGreyScales)
{
	static const WChar *funcName = wxT("SetNumGreyScales_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumGreyScales < 1) {
		NotifyError(wxT("%s: Illegal value (%d)"), funcName, theNumGreyScales);
		return(FALSE);
	}
	signalDispPtr->numGreyScales = theNumGreyScales;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetSummaryDisplay *******************************/

/*
 * This routine sets the summary display mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetSummaryDisplay_SignalDisp(WChar *theSummaryDisplay)
{
	static const WChar *funcName = wxT("SetSummaryDisplay_SignalDisp");
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSummaryDisplay,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  theSummaryDisplay);
		return(FALSE);
	}
	signalDispPtr->summaryDisplay = specifier;
	signalDispPtr->redrawSubDisplaysFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetChanActivityDisplay **************************/

/*
 * This routine sets the summary display mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetChanActivityDisplay_SignalDisp(WChar *theChanActivityDisplay)
{
	static const WChar *funcName = wxT("SetChanActivityDisplay_SignalDisp");
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theChanActivityDisplay,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  theChanActivityDisplay);
		return(FALSE);
	}
	signalDispPtr->chanActivityDisplay = specifier;
	signalDispPtr->redrawSubDisplaysFlag = TRUE;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetTitle ****************************************/

/*
 * This routine sets the 'title' field of the 'signalDisp' structure.
 */

BOOLN
SetTitle_SignalDisp(WChar *title)
{
	static const WChar *funcName = wxT("SetTitle_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(signalDispPtr->title, title, MAXLINE);
	signalDispPtr->redrawGraphFlag = TRUE;
	return(TRUE);

}

/**************************** SetXTicks ***************************************/

/*
 * This routine sets the X axis ticks for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetXTicks_SignalDisp(int xTicks)
{
	static const WChar *funcName = wxT("SetXTicks_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (xTicks <= 0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g)"), funcName,
		  xTicks);
		return(FALSE);
	}
	signalDispPtr->xTicks = xTicks;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetXAxisTitle ***********************************/

/*
 * This routine sets the 'x_axis_title' field of the 'signalDisp' structure.
 */

BOOLN
SetXAxisTitle_SignalDisp(WChar *xAxisTitle)
{
	static const WChar *funcName = wxT("SetXAxisTitle_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(signalDispPtr->xAxisTitle, xAxisTitle, MAXLINE);
	return(TRUE);

}

/**************************** SetXNumberFormat ********************************/

/*
 * This routine sets the displayed number format, i.e. the number of decimal
 * places displayed and the exponent for the display x-axis.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetXNumberFormat_SignalDisp(WChar *xNumberFormat)
{
	static const WChar *funcName = wxT("SetXNumberFormat_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!xNumberFormat || (xNumberFormat[0] == '\0')) {
		NotifyError(wxT("%s: This setting must be in the form 'xx.e-3' (%s)"),
		  funcName, xNumberFormat);
		return(FALSE);
	}
	DSAM_strncpy(signalDispPtr->xNumberFormat, xNumberFormat, MAXLINE);
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetAutoXScaleParsState ************************/

/*
 * This function sets enables or disables the auto scale relatated parameter.
 */

void
SetAutoXScaleParsState_SignalDisp(BOOLN state)
{
	signalDispPtr->parList->pars[DISPLAY_X_NUMBER_FORMAT].enabled = state;
	signalDispPtr->parList->pars[DISPLAY_X_TICKS].enabled = state;
	signalDispPtr->parList->pars[DISPLAY_X_OFFSET].enabled = state;
	signalDispPtr->parList->pars[DISPLAY_X_EXTENT].enabled = state;
	signalDispPtr->parList->updateFlag = TRUE;

}

/****************************** SetAutoXScale *********************************/

/*
 * This function sets the module's autoXScale parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAutoXScale_SignalDisp(WChar *theAutoXScale)
{
	static const WChar	*funcName = wxT("SetAutoXScale_SignalDisp");
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAutoXScale,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  theAutoXScale);
		return(FALSE);
	}
	signalDispPtr->autoXScale = specifier;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	SetAutoXScaleParsState_SignalDisp(!signalDispPtr->autoXScale);
	return(TRUE);

}

/****************************** SetXOffset ************************************/

/*
 * This function sets the module's xOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetXOffset_SignalDisp(Float theXOffset)
{
	static const WChar	*funcName = wxT("SetXOffset_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theXOffset < 0.0) {
		NotifyError(wxT("%s: Cannot set the X offset less than zero (%g ")
		  wxT("units)."), funcName, theXOffset);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	signalDispPtr->xOffset = theXOffset;
	return(TRUE);

}

/****************************** SetXExtent ************************************/

/*
 * This function sets the module's xExtent parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetXExtent_SignalDisp(Float theXExtent)
{
	static const WChar	*funcName = wxT("SetXExtent_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	signalDispPtr->xExtent = theXExtent;
	return(TRUE);

}

/**************************** SetYNumberFormat ********************************/

/*
 * This routine sets the decimal places displayed for the display x-axis.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetYNumberFormat_SignalDisp(WChar *yNumberFormat)
{
	static const WChar *funcName = wxT("SetYNumberFormat_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!yNumberFormat || (yNumberFormat[0] == '\0')) {
		NotifyError(wxT("%s: This setting must be in the form 'xx.e-3' (%s)"),
		  funcName, yNumberFormat);
		return(FALSE);
	}
	DSAM_strncpy(signalDispPtr->yNumberFormat, yNumberFormat, MAXLINE);
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetYTicks ***************************************/

/*
 * This routine sets the Y axis ticks for the display.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetYTicks_SignalDisp(int yTicks)
{
	static const WChar *funcName = wxT("SetYTicks_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (yTicks <= 0) {
		NotifyError(wxT("%s: Value must be greater than zero (%g)"), funcName,
		  yTicks);
		return(FALSE);
	}
	signalDispPtr->yTicks = yTicks;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetYInsetScale **********************************/

/*
 * This routine sets the y inset scale mode for the display.
 * If set to true, the scaling will be direct and related only to the
 * signal yScale parameter.
 */

BOOLN
SetYInsetScale_SignalDisp(WChar *theYInsetScale)
{
	static const WChar *funcName = wxT("SetYInsetScale_SignalDisp");
	int		specifier;

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theYInsetScale,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName,
		  theYInsetScale);
		return(FALSE);
	}
	signalDispPtr->yInsetScale = specifier;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetXDecPlaces ***********************************/

/*
 * This routine sets the decimal places displayed for the display x-axis.
 * This is a bool response, hence only non-zero values are TRUE.
 * This parameter has been disabled in the parlist.  It is obsolete.
 */

BOOLN
SetXDecPlaces_SignalDisp(int xDecPlaces)
{
	static const WChar *funcName = wxT("SetXDecPlaces_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (xDecPlaces < 0) {
		NotifyError(wxT("%s: Value cannot be negative (%g)"), funcName,
		  xDecPlaces);
		return(FALSE);
	}
	signalDispPtr->xDecPlaces = xDecPlaces;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetYDecPlaces ***********************************/

/*
 * This routine sets the decimal places displayed for the display x-axis.
 * This parameter has been disabled in the parlist.  It is obsolete.
 * This is a bool response, hence only non-zero values are TRUE.
 */

BOOLN
SetYDecPlaces_SignalDisp(int yDecPlaces)
{
	static const WChar *funcName = wxT("SetYDecPlaces_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (yDecPlaces < 0) {
		NotifyError(wxT("%s: Value cannot be negative (%g)"), funcName,
		  yDecPlaces);
		return(FALSE);
	}
	signalDispPtr->yDecPlaces = yDecPlaces;
	signalDispPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_SignalDisp(void)
{
	static const WChar *funcName = wxT("PrintPars_SignalDisp");

	if (signalDispPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	DPrint(wxT("SignalDisp Module Parameters:-\n"));
	DPrint(wxT("\tWindow title = %s,\n"), signalDispPtr->title);
	DPrint(wxT("\tMode = %s,\n"),
	  signalDispPtr->modeList[signalDispPtr->mode].name);
	if (signalDispPtr->mode == GRAPH_MODE_GREY_SCALE)
		DPrint(wxT("\tNo. grey scales = %d.\n"), signalDispPtr->numGreyScales);
	DPrint(wxT("\tAutomatic scaling mode: %s\n"),
	  BooleanList_NSpecLists(signalDispPtr->autoYScale)->name);
	DPrint(wxT("\tChannel step for display: %d\n"), signalDispPtr->channelStep);
	if (!signalDispPtr->autoYScale)
		DPrint(wxT("\tMinimum/Maximum Y values: %g/%g units.\n"),
		  signalDispPtr->minY, signalDispPtr->maxY);
	DPrint(wxT("\tY normalisation mode: %s\n"),
	  signalDispPtr->yNormModeList[signalDispPtr->yNormalisationMode].name);
	DPrint(wxT("\tSummary display mode: %s\n"),
	  BooleanList_NSpecLists(signalDispPtr->summaryDisplay)->name);
	DPrint(wxT("\tChannel Activity display mode: %s\n"),
	  BooleanList_NSpecLists(signalDispPtr->chanActivityDisplay)->name);
	DPrint(wxT("\tSignal Y scale = %g.\n"), signalDispPtr->magnification);
	DPrint(wxT("\tY axis title = %s\n"), signalDispPtr->yAxisTitle);
	DPrint(wxT("\tY-axis mode = %s,\n"),
	  signalDispPtr->yAxisModeList[signalDispPtr->yAxisMode].name);
	DPrint(wxT("\tY axis scale number format = %s\n"), signalDispPtr->
	  yNumberFormat);
	DPrint(wxT("\tY axis ticks = %d\n"), signalDispPtr->yTicks);
	DPrint(wxT("\tY inset scale mode: %s\n"),
	  BooleanList_NSpecLists(signalDispPtr->yInsetScale)->name);
	DPrint(wxT("\tX resolution = %g (<= 1.0)\n"), signalDispPtr->xResolution);
	DPrint(wxT("\tX axis title = %s\n"), signalDispPtr->xAxisTitle);
	DPrint(wxT("\tX axis ticks = %d\n"), signalDispPtr->xTicks);
	DPrint(wxT("\tX axis scale number format = %s\n"), signalDispPtr->
	  xNumberFormat);
	if (!signalDispPtr->autoXScale) {
		DPrint(wxT("\tOffset/extent X values: %g"), signalDispPtr->xOffset);
		if (signalDispPtr->xExtent < DSAM_EPSILON)
			DPrint(wxT("units / <end of signal>).\n"));
		else
			DPrint(wxT("/%g units.\n"), signalDispPtr->xExtent);
	}
	DPrint(wxT("\tTop margin percentage = %g %%\n"), signalDispPtr->topMargin);
	DPrint(wxT("\tFrame: dimensions %d x %d\n"), signalDispPtr->frameWidth,
	  signalDispPtr->frameHeight);
	DPrint(wxT("\tFrame: position %d x %d\n"), signalDispPtr->frameXPos,
	  signalDispPtr->frameYPos);
	DPrint(wxT("\tDelay %g ms\n"), MILLI(signalDispPtr->frameDelay));
	return(TRUE);

}

/**************************** SetProcessMode **********************************/

/*
 * This routine checks whether this module is being used in-line within an
 * EarObject pipline.
 * This is checked by ensuring that this is the first
 * module to use the EarObject, or that it was previously used by this module,
 * i.e. the process name has not been set, or it is set to this module's name.
 * This routine assumes that 'data' has been correctly initialised.
 */

DSAM_API BOOLN
SetProcessMode_SignalDisp(EarObjectPtr data)
{
	static const WChar *funcName = wxT("SetProcessMode_SignalDisp");

	if (InLineProcess_ModuleMgr(data, ShowSignal_SignalDisp)) {
		SetProcessName_EarObject(data, wxT("%s (%d)"), DISPLAY_PROCESS_NAME,
		  (int) data->handle);
		data->outSignalPtr = data->inSignal[0];
		data->outSignal = _OutSig_EarObject(data);
		signalDispPtr->inLineProcess = TRUE;
	} else {
		if (!CheckPars_SignalData(data->outSignal)) {
			NotifyError(wxT("%s: Output signal not correctly set for 'sample' ")
			  wxT("use."), funcName);
			return(FALSE);
		}
		signalDispPtr->inLineProcess = FALSE;
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_SignalDisp(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_SignalDisp");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	signalDispPtr = (SignalDispPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_SignalDisp(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_SignalDisp");

	if (!SetParsPointer_SignalDisp(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_SignalDisp(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = signalDispPtr;
	theModule->threadMode = MODULE_THREAD_MODE_TRANSFER;
	theModule->Free = Free_SignalDisp;
	theModule->GetUniParListPtr = GetUniParListPtr_SignalDisp;
	theModule->PrintPars = PrintPars_SignalDisp;
	theModule->RunProcess = ShowSignal_SignalDisp;
	theModule->SetParsPointer = SetParsPointer_SignalDisp;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

DSAM_API BOOLN
CheckData_SignalDisp(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_SignalDisp");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the summary EarObject used in the display module.
 * It initialises the channel activity EarObject used in the display module.
 * It assumes that the centre frequency list is set up correctly.
 * It also assumes that the output signal has already been initialised.
 */

DSAM_API BOOLN
InitProcessVariables_SignalDisp(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_SignalDisp");
	Float	definedDuration;
	SignalDataPtr	signal = _OutSig_EarObject(data), buffer;
	SignalDispPtr	p = signalDispPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->autoXScale)
			definedDuration = -1.0;
		else
			definedDuration = (p->xExtent > DSAM_EPSILON)? p->xOffset + p->
			  xExtent: _GetDuration_SignalData(signal) - p->xOffset;
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_SignalDisp();
			(signalDispDefFuncs.InitCriticalSection)();
			if ((p->summary = Init_EarObject(wxT("Util_ReduceChannels"))) ==
			  NULL) {
				NotifyError(wxT("%s: Out of memory for summary EarObject."),
				  funcName);
				return(FALSE);
			}
			if ((p->chanActivity = Init_EarObject(wxT("Ana_Intensity"))) ==
			  NULL) {
				NotifyError(wxT("%s: Out of memory for channel activity ")
				  wxT("EarObject."), funcName);
				return(FALSE);
			}
			ConnectOutSignalToIn_EarObject(data, p->summary);
			ConnectOutSignalToIn_EarObject(data, p->chanActivity);
			if ((definedDuration > _GetDuration_SignalData(signal)) ||
			  (!signal->staticTimeFlag && (signal->
			  numWindowFrames != SIGNALDATA_DEFAULT_NUM_WINDOW_FRAMES))) {
				if ((p->buffer = Init_EarObject(wxT("NULL"))) == NULL) {
					NotifyError(wxT("%s: Out of memory for buffer EarObject."),
					  funcName);
					return(FALSE);
				}
				if (signal->numWindowFrames !=
				  SIGNALDATA_DEFAULT_NUM_WINDOW_FRAMES) {
					p->xExtent = _GetDuration_SignalData(signal) / signal->
					  numWindowFrames;
					p->xOffset = 0.0;
				}
			}
			if (p->xAxisTitle[0] == '\0')
				SetXAxisTitle_SignalDisp(signal->info.sampleTitle);
			if (p->yAxisTitle[0] == '\0')
				SetYAxisTitle_SignalDisp(signal->info.channelTitle);
			if (p->yAxisMode == GRAPH_Y_AXIS_MODE_AUTO)
				p->yAxisMode = (signal->numChannels > 1)?
				  GRAPH_Y_AXIS_MODE_CHANNEL: GRAPH_Y_AXIS_MODE_LINEAR_SCALE;
			p->parList->updateFlag = TRUE;
			p->updateProcessVariablesFlag = FALSE;
			data->updateProcessFlag = FALSE;
		}
		p->bufferCount = 0;
		if (p->buffer) {
			if (!InitOutSignal_EarObject(p->buffer, signal->numChannels,
			  (ChanLen) floor(definedDuration / signal->dt + 0.5), signal->
			  dt)) {
				NotifyError(wxT("%s: Could not initialise buffer signal."),
				  funcName);
				return(FALSE);
			}
			buffer = _OutSig_EarObject(p->buffer);
			SetInterleaveLevel_SignalData(buffer, signal->interleaveLevel);
			SetNumWindowFrames_SignalData(buffer, signal->numWindowFrames);
			SetOutputTimeOffset_SignalData(buffer, signal->outputTimeOffset);
			SetStaticTimeFlag_SignalData(buffer, signal->staticTimeFlag);
			CopyInfo_SignalData(buffer, signal);
		}
		/**** Put things that need to be initialised at start time here. ***/
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 * It just checks the first set of coefficients, and assumes that the others
 * are in the same state.
 */

BOOLN
FreeProcessVariables_SignalDisp(void)
{

	Free_EarObject(&signalDispPtr->buffer);
	Free_EarObject(&signalDispPtr->summary);
	Free_EarObject(&signalDispPtr->chanActivity);
	(signalDispDefFuncs.FreeCriticalSection)();

	signalDispPtr->updateProcessVariablesFlag = TRUE;
 	return(TRUE);

}

/**************************** ProcessBuffer ***********************************/

/*
 * This routine processes the display buffer.
 * It reads in data into the buffer, shifting existing data along if necessary.
 */

DSAM_API void
ProcessBuffer_SignalDisp(SignalDataPtr signal, EarObjectPtr bufferEarObj,
  int windowFrame)
{
	/* static const WChar *funcName = wxT("ProcessBuffer_SignalDisp"); */
	register	ChanData	*inPtr, *outPtr;
	int		chan;
	ChanLen	j, shift, signalLength;
	SignalDataPtr buffer;

	buffer = _OutSig_EarObject(bufferEarObj);
	signalLength = signal->length / signal->numWindowFrames;
	if (signalDispPtr->bufferCount + signalLength > buffer->length) {
		shift = signalLength - (buffer->length - signalDispPtr->bufferCount);
		if (shift < buffer->length)
			for (chan = 0; chan < signal->numChannels; chan++) {
				inPtr = buffer->channel[chan] + shift;
				outPtr = buffer->channel[chan];
				for (j = 0; j < signalDispPtr->bufferCount; j++)
					*outPtr++ = *inPtr++;
			}
		signalDispPtr->bufferCount -= shift;
		if (!signal->staticTimeFlag)
			buffer->outputTimeOffset += shift * buffer->dt;
	}
	for (chan = 0; chan < signal->numChannels; chan++) {
		inPtr = signal->channel[chan] + windowFrame * signalLength;
		outPtr = buffer->channel[chan] + signalDispPtr->bufferCount;
		for (j = 0; j < signalLength; j++)
			*outPtr++ = *inPtr++;
	}
	if (signalDispPtr->bufferCount < buffer->length) {
		signalDispPtr->bufferCount += signalLength;
	}

}

/**************************** SetSubDisplays **********************************/

/*
 * This routine sets up the canvas sub-display control parameters, i.e. for the
 * summary and channel activity displays.
 */

DSAM_API void
SetSubDisplays_SignalDisp(void)
{
	/* static const WChar *funcName = wxT("SetSubDisplays_SignalDisp"); */

	if (signalDispPtr->summaryDisplay) {
		SetPar_ModuleMgr(signalDispPtr->summary, wxT("mode"), wxT("average"));
		SetPar_ModuleMgr(signalDispPtr->summary, wxT("num_Channels"), wxT("1"));
		RunProcess_ModuleMgr(signalDispPtr->summary);
	}
	if (signalDispPtr->chanActivityDisplay) {
		SetRealPar_ModuleMgr(signalDispPtr->chanActivity, wxT("offset"), 0.0);
		SetRealPar_ModuleMgr(signalDispPtr->chanActivity, wxT("extent"), -1.0);
		RunProcess_ModuleMgr(signalDispPtr->chanActivity);
	}
	signalDispPtr->redrawSubDisplaysFlag = FALSE;

}

/**************************** ShowSignal **************************************/

/*
 * This this routine is for the non-gui mode.
 */

BOOLN
ShowSignal_SignalDisp(EarObjectPtr data)
{

	return((signalDispDefFuncs.ShowSignal)(data));

}
