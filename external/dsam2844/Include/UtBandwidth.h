/**************
 *
 * File:		UtBandwidth.h
 * Purpose:		This module contains the bandwidth functions, including the
 *				equivalent rectangular bandwidth (ERB) freqency functions.
 * Comments:	13-10-98 LPO: Implemented changes to introduce the
 *				'Custom_ERB' mode.
 * Author:		L. P. O'Mard.
 * Created:		12 Jul 1993
 * Updated:		13 Oct 1998
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

#ifndef	_UTBANDWIDTH_H
#define _UTBANDWIDTH_H	1

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define BANDWIDTH_NUM_PARS				5
#define BANDWIDTH_NUM_CONSTANT_PARS		1
#define	BW_DEF_SCALER					1.0

#define	K1_ERB		24.7		/* k1 constant. */
#define K2_ERB		4.37e-3		/* K2 constant, frequency in Hz. */
#define K3_ERB		21.4		/* K3 constant, for ERB rate calc. */

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/


/* Position of cf unit; from Liberman's map.
 * This has been sited here, rather than in the CFList module, so that it
 * is with with the other experimentally derived BM code.
 */

#define BANDWIDTH_CARNEY_X(CF)	(11.9e-3 * log10(0.80 + (CF) / 456.0))

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BANDWIDTH_ERB,
	BANDWIDTH_CAT,
	BANDWIDTH_CUSTOM_ERB,
	BANDWIDTH_GUINEA_PIG,
	BANDWIDTH_GUINEA_PIG_SCALED,
	BANDWIDTH_USER,
	BANDWIDTH_NONLINEAR,
	BANDWIDTH_INTERNAL_DYNAMIC,
	BANDWIDTH_INTERNAL_STATIC,
	BANDWIDTH_DISABLED,
	BANDWIDTH_NULL

} BandwidthModeSpecifier;

typedef enum {

	BANDWIDTH_PAR_MODE,
	BANDWIDTH_PAR_MIN,
	BANDWIDTH_PAR_QUALITY,
	BANDWIDTH_PAR_SCALER,
	BANDWIDTH_PAR_BANDWIDTH

} BandwidthParSpecifier;

typedef struct BandwidthMode {

	BandwidthModeSpecifier specifier;
	Float	bwMin;
	Float	quality;
	Float	scaler;
	Float	(* Func)(struct BandwidthMode *, Float);

} BandwidthMode, *BandwidthModePtr;

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

/* formulae.c calls: ERB as a function of frequency etc, Glasberg and Moore 1990
 * versions only.
 */

Float	BandwidthFromF_Bandwith(BandwidthModePtr p, Float theFrequency);

Float	CatFromF_Bandwidth(Float f);

Float	CustomERBFromF_Bandwidth(Float x, Float bwMin, Float quality);

Float	ERBFromF_Bandwidth(Float x);

Float	FFromERB_Bandwidth(Float x);

Float	ERBRateFromF_Bandwidth(Float theFrequency);

Float	FFromERBRate_Bandwidth(Float theERBRate);

Float	GuineaPigFromF_Bandwidth(Float f);

Float	NonLinearFromF_Bandwidth(Float f);

NameSpecifier *	ModeList_Bandwidth(int index);

BOOLN	SetMode_Bandwidth(BandwidthModePtr modePtr, WChar *modeName);

__END_DECLS

#endif
