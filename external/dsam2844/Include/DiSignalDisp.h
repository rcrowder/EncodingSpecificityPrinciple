/******************
 *
 * File:		DiSignalDisp.h
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
 *				26-11-97 LPO: Using new GeNSpecLists module for BOOLN
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
 * Author:		L.P.O'Mard
 * Created:		06 Jan 1995
 * Updated:		08 Oct 1998
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

#ifndef	_DiSignalDisp_H
#define _DiSignalDisp_H	1

#include "UtRedceChans.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define DISPLAY_NUM_PARS			33

#define	DEFAULT_SIGNAL_Y_SCALE			1.0
#define	DEFAULT_X_RESOLUTION			0.01
#define	DEFAULT_WIDTH					-1.0
#define	DEFAULT_CHANNEL_STEP			1
#define	DEFAULT_X_NUMBER_FORMAT			wxT("xe-3")
#define	DEFAULT_Y_NUMBER_FORMAT			wxT("y")
#define	DEFAULT_X_TICKS					6
#define	DEFAULT_Y_TICKS					15
#define DEFAULT_X_DEC_PLACES			0
#define DEFAULT_Y_DEC_PLACES			0

#define	DISPLAY_PROCESS_NAME				wxT("DSAM Display")
#define	DISPLAY_DEFAULT_FRAME_WIDTH			440
#define	DISPLAY_DEFAULT_FRAME_HEIGHT		500
#define	DISPLAY_DEFAULT_FRAME_DELAY			0.0

#define	GRAPH_TOP_MARGIN_PERCENT			5.0
#define	GRAPH_NUM_GREY_SCALES				10

/******************************************************************************/
/*************************** Pre-reference definitions ************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef struct {

	void	(* FreeCriticalSection)(void);
	void 	(* FreeDisplay)(void *);
	void 	(* GetWindowPosition)(int *, int *);
	void	(* InitCriticalSection)(void);
	BOOLN	(* ShowSignal)(EarObjectPtr);

} SignalDispDefFuncs;

typedef	enum {

	/* Signal Controls */
	DISPLAY_MAGNIFICATION,
	DISPLAY_Y_NORMALISATION_MODE,
	DISPLAY_CHANNEL_STEP,
	DISPLAY_NUMGREYSCALES,
	DISPLAY_X_RESOLUTION,
	DISPLAY_WIDTH,
	/* Y-Axis controls */
	DISPLAY_Y_AXIS_TITLE,
	DISPLAY_Y_AXIS_MODE,
	DISPLAY_AUTOMATIC_SCALING,
	DISPLAY_AUTO_Y_SCALE,
	DISPLAY_MAX_Y,
	DISPLAY_MIN_Y,
	DISPLAY_Y_NUMBER_FORMAT,
	DISPLAY_Y_DEC_PLACES,
	DISPLAY_Y_TICKS,
	DISPLAY_Y_INSET_SCALE,
	/* X-Axis controls */
	DISPLAY_X_AXIS_TITLE,
	DISPLAY_AUTO_X_SCALE,
	DISPLAY_X_NUMBER_FORMAT,
	DISPLAY_X_DEC_PLACES,
	DISPLAY_X_TICKS,
	DISPLAY_X_OFFSET,
	DISPLAY_X_EXTENT,
	/* General Controls */
	DISPLAY_WINDOW_TITLE,
	DISPLAY_MODE,
	DISPLAY_SUMMARY_DISPLAY,
	DISPLAY_CHAN_ACTIVITY_DISPLAY,
	DISPLAY_FRAME_DELAY,
	DISPLAY_TOP_MARGIN,
	DISPLAY_WINDOW_HEIGHT,
	DISPLAY_WINDOW_WIDTH,
	DISPLAY_WINDOW_X_POS,
	DISPLAY_WINDOW_Y_POS,
	DISPLAY_NULL

} SignalDispParSpecifier;

typedef	enum {

	GRAPH_MODE_OFF,
	GRAPH_MODE_LINE,
	GRAPH_MODE_GREY_SCALE,
	GRAPH_MODE_NULL

} SignalDispModeSpecifier;

typedef	enum {

	GRAPH_Y_AXIS_MODE_AUTO,
	GRAPH_Y_AXIS_MODE_CHANNEL,
	GRAPH_Y_AXIS_MODE_LINEAR_SCALE,
	GRAPH_Y_AXIS_MODE_NULL

} SignalDispyAxisModeSpecifier;

typedef enum {

	GRAPH_LINE_YNORM_MIDDLE_MODE,
	GRAPH_LINE_YNORM_BOTTOM_MODE,
	GRAPH_LINE_YNORM_MODE_NULL

} SignalDispLineModeSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;
	BOOLN	guiFlag;
	BOOLN	autoYScale;
	int		channelStep;
	Float	magnification;
	Float	maxY;
	Float	minY;
	int		numGreyScales;
	Float	width;
	Float	xResolution;
	int		yNormalisationMode;
	WChar	yAxisTitle[MAXLINE];
	int		yAxisMode;
	WChar	yNumberFormat[MAXLINE];
	int		yDecPlaces;
	int		yTicks;
	int		yInsetScale;
	WChar	xAxisTitle[MAXLINE];
	WChar	xNumberFormat[MAXLINE];
	int		xDecPlaces;
	int		xTicks;
	BOOLN	autoXScale;
	Float	xOffset;
	Float	xExtent;
	WChar	title[MAXLINE];
	int		mode;
	Float	frameDelay;
	Float	topMargin;
	int		frameHeight;
	int		frameWidth;
	int		frameXPos;
	int		frameYPos;
	int		summaryDisplay;
	int		chanActivityDisplay;

	/* Private variables */
	NameSpecifier	*modeList;
	NameSpecifier	*yAxisModeList;
	NameSpecifier	*yNormModeList;
	UniParListPtr	parList;
	BOOLN			inLineProcess;
	BOOLN			redrawGraphFlag;
	BOOLN			redrawSubDisplaysFlag;
	BOOLN			drawCompletedFlag;
	BOOLN			initialisationFlag;
	ChanLen			bufferCount;
	EarObjectPtr	buffer;
	EarObjectPtr	data;
	EarObjectPtr	summary;
	EarObjectPtr	chanActivity;
	void			*display;
	void			*critSect;

} SignalDisp, *SignalDispPtr;

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

__BEGIN_DECLS

DSAM_API extern SignalDisp	signalDisp, *signalDispPtr;

__END_DECLS

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DSAM_API BOOLN	CheckData_SignalDisp(EarObjectPtr data);

void	CheckForDisplay_SignalDisp(long handle);

BOOLN	Free_SignalDisp(void);

void	FreeCriticalSection_SignalDisp(void);

void	FreeDisplay_SignalDisp(void *display);

BOOLN	FreeProcessVariables_SignalDisp(void);

NameSpecifier *	GetPanelList_SignalDisp(int index);

UniParListPtr	GetUniParListPtr_SignalDisp(void);

void	GetWindowPosition_SignalDisp(int *x, int *y);

BOOLN	Init_SignalDisp(ParameterSpecifier parSpec);

BOOLN	InitBOOLNeanList_SignalDisp(void);

void	InitCriticalSection_SignalDisp(void);

BOOLN	InitYNormModeList_SignalDisp(void);

BOOLN	InitModeList_SignalDisp(void);

BOOLN	InitModule_SignalDisp(ModulePtr theModule);

BOOLN	InitParNameList_SignalDisp(void);

DSAM_API BOOLN	InitProcessVariables_SignalDisp(EarObjectPtr data);

BOOLN	InitYAxisModeList_SignalDisp(void);

void	PostDisplayEvent_SignalDisp(void);

BOOLN	PrintPars_SignalDisp(void);

DSAM_API void	ProcessBuffer_SignalDisp(SignalDataPtr signal,
				  EarObjectPtr bufferEarObj, int windowFrame);

void	SetAutoXScaleParsState_SignalDisp(BOOLN state);

BOOLN	SetAutoYScale_SignalDisp(WChar *theAutoYScale);

void	SetAutoYScaleParsState_SignalDisp(BOOLN state);

BOOLN	SetChanActivityDisplay_SignalDisp(WChar *summaryDisplay_SignalDis);

BOOLN	SetChannelStep_SignalDisp(int theChannelStep);

BOOLN	SetDefaulEnabledPars_SignalDisp(void);

BOOLN	SetFrameDelay_SignalDisp(Float theFrameDelay);

BOOLN	SetFrameHeight_SignalDisp(int theFrameHeight);

BOOLN	SetFrameXPos_SignalDisp(int theFrameXPos);

BOOLN	SetFrameYPos_SignalDisp(int theFrameYPos);

BOOLN	SetFrameWidth_SignalDisp(int theFrameWidth);

DSAM_API void	SetFuncFreeCriticalSection_SignalDisp(void (* Func)(void));

DSAM_API void	SetFuncFreeDisplay_SignalDisp(void (* Func)(void *));

DSAM_API void	SetFuncGetWindowPosition_SignalDisp(void (* Func)(int *, int *));

DSAM_API void	SetFuncInitCriticalSection_SignalDisp(void (* Func)(void));

DSAM_API void	SetFuncShowSignal_SignalDisp(BOOLN (* Func)(EarObjectPtr));

BOOLN	SetMaxY_SignalDisp(Float maxY);

BOOLN	SetMinY_SignalDisp(Float minY);

BOOLN	SetMode_SignalDisp(WChar *theMode);

DSAM_API void	SetSubDisplays_SignalDisp(void);

BOOLN	SetNumGreyScales_SignalDisp(int theNumGreyScales);

BOOLN	SetParsPointer_SignalDisp(ModulePtr theModule);

DSAM_API BOOLN	SetProcessMode_SignalDisp(EarObjectPtr data);

BOOLN	SetMagnification_SignalDisp(Float magnification);

BOOLN	SetSummaryDisplay_SignalDisp(WChar *summaryDisplay_SignalDis);

BOOLN	SetTitle_SignalDisp(WChar *title);

BOOLN	SetTopMargin_SignalDisp(Float topMargin);

BOOLN	SetUniParList_SignalDisp(void);

BOOLN	SetWidth_SignalDisp(Float width);

BOOLN	SetXAxisTitle_SignalDisp(WChar *xAxisTitle);

BOOLN	SetXDecPlaces_SignalDisp(int xDecPlaces);

BOOLN	SetXExtent_SignalDisp(Float theMaxX);

BOOLN	SetXResolution_SignalDisp(Float xResolution);

BOOLN	SetXNumberFormat_SignalDisp(WChar *xNumberFormat);

BOOLN	SetXOffset_SignalDisp(Float theXOffset);

BOOLN	SetXTicks_SignalDisp(int xTicks);

BOOLN	SetAutoXScale_SignalDisp(WChar *autoXScale);

BOOLN	SetYAxisMode_SignalDisp(WChar *theYAxisMode);

BOOLN	SetYAxisTitle_SignalDisp(WChar *yAxisTitle);

BOOLN	SetYDecPlaces_SignalDisp(int yDecPlaces);

BOOLN	SetYNumberFormat_SignalDisp(WChar *yNumberFormat);

BOOLN	SetYInsetScale_SignalDisp(WChar *yInsetScale);

BOOLN	SetYNormalisationMode_SignalDisp(WChar *theYNormalisationMode);

BOOLN	SetYTicks_SignalDisp(int yTicks);

BOOLN	ShowSignal_SignalDisp(EarObjectPtr data);

__END_DECLS

#endif
