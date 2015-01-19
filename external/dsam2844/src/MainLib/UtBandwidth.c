/**************
 *
 * File:		UtBandwidth.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "UtBandwidth.h"

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/********************************* ModeList ***********************************/

/*
 * This function returns a CF mode list entry.
 */

NameSpecifier *
ModeList_Bandwidth(int index)
{
	static NameSpecifier	modeList[] = {

			{wxT("ERB"),				BANDWIDTH_ERB },
			{wxT("CAT"),				BANDWIDTH_CAT },
			{wxT("CUSTOM_ERB"),			BANDWIDTH_CUSTOM_ERB },
			{wxT("GUINEA_PIG"),			BANDWIDTH_GUINEA_PIG },
			{wxT("GUINEA_PIG_SCALED"),	BANDWIDTH_GUINEA_PIG_SCALED },
			{wxT("USER"),				BANDWIDTH_USER },
			{wxT("NONLINEAR"),			BANDWIDTH_NONLINEAR },
			{wxT("INTERNAL_DYNAMIC"),	BANDWIDTH_INTERNAL_DYNAMIC },
			{wxT("INTERNAL_STATIC"),	BANDWIDTH_INTERNAL_STATIC },
			{wxT("DISABLED"),			BANDWIDTH_DISABLED },
			{NULL,						BANDWIDTH_NULL }

		};
	return (&modeList[index]);

}

/******************************** GetMode *************************************/

/*
 * This function sets the "Bandwidth" mode to be used by the
 * gamma tone filter routines, or whatever wishes to use it.
 * The band width mode defines the function for calculating the gamma tone
 * filter band width 3 dB down.
 * If the 'INTERNAL_STATIC' mode is used, then the bandwidth function will
 * already have been set, so the value needs to be preserved.
 * It returns FALSE if it fails.
 */

BOOLN
SetMode_Bandwidth(BandwidthModePtr modePtr, WChar *modeName)
{
	static const WChar *funcName = wxT("SetMode_Bandwidth");
	int		specifier;
	Float (* Func)(BandwidthModePtr, Float);
	BandwidthMode modeList[] = {

		{ BANDWIDTH_ERB, 		K1_ERB, K2_ERB, BW_DEF_SCALER, BandwidthFromF_Bandwith},
		{ BANDWIDTH_CAT, 		K1_ERB, K2_ERB, BW_DEF_SCALER, BandwidthFromF_Bandwith},
		{ BANDWIDTH_CUSTOM_ERB, K1_ERB, K2_ERB, BW_DEF_SCALER, BandwidthFromF_Bandwith},
		{ BANDWIDTH_GUINEA_PIG, K1_ERB, K2_ERB, BW_DEF_SCALER, BandwidthFromF_Bandwith},
		{ BANDWIDTH_GUINEA_PIG_SCALED, K1_ERB, K2_ERB, BW_DEF_SCALER, BandwidthFromF_Bandwith},
		{ BANDWIDTH_USER, 		K1_ERB, K2_ERB, BW_DEF_SCALER, NULL},
		{ BANDWIDTH_NONLINEAR,	K1_ERB, K2_ERB, BW_DEF_SCALER, BandwidthFromF_Bandwith},
		{ BANDWIDTH_INTERNAL_DYNAMIC,	K1_ERB, K2_ERB, BW_DEF_SCALER, NULL},
		{ BANDWIDTH_INTERNAL_STATIC,	K1_ERB, K2_ERB, BW_DEF_SCALER, NULL},
		{ BANDWIDTH_DISABLED,	K1_ERB, K2_ERB, BW_DEF_SCALER, NULL},
		{ BANDWIDTH_NULL,		0.0, 0.0, 0.0, NULL}

	};

	if (modePtr == NULL) {
		NotifyError(wxT("%s: Mode pointer is not set ('%s')."), funcName,
		  modeName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(modeName, ModeList_Bandwidth(0))) ==
	  BANDWIDTH_NULL) {
		NotifyError(wxT("%s: Unknown bandwidth  mode (%s)."), funcName,
		  modeName);
		return(FALSE);
	}
	if (specifier == BANDWIDTH_INTERNAL_STATIC)
		Func = modePtr->Func;
	*modePtr = modeList[specifier];
	if (specifier == BANDWIDTH_INTERNAL_STATIC)
		modePtr->Func = Func;
	return(TRUE);

}

/******************************** BandwidthFromF ******************************/

/*
 * This a general routine for calculating the bandwiths.  It calls all the other
 * routines.  Using the appropriate parameters.
 * It assumes that the 'BandwidthMode' structure has been correctly initialised.
 */

Float
BandwidthFromF_Bandwith(BandwidthModePtr p, Float theFrequency)
{
	static const WChar *funcName = wxT("BandwidthFromF_Bandwith");
	if (p == NULL) {
		NotifyError(wxT("%s: BandwidthMode structure not initialised."),
		  funcName);
		return(-1.0);
	}
	switch (p->specifier) {
	case BANDWIDTH_ERB:
		return(ERBFromF_Bandwidth(theFrequency));
	case BANDWIDTH_CAT:
		return(CatFromF_Bandwidth(theFrequency));
	case BANDWIDTH_CUSTOM_ERB:
		return(CustomERBFromF_Bandwidth(theFrequency, p->bwMin, p->quality));
	case BANDWIDTH_GUINEA_PIG:
		return(GuineaPigFromF_Bandwidth(theFrequency));
	case BANDWIDTH_GUINEA_PIG_SCALED:
		return(GuineaPigFromF_Bandwidth(theFrequency) * p->scaler);
	case BANDWIDTH_NONLINEAR:
		return(NonLinearFromF_Bandwidth(theFrequency));
	default:
		NotifyError(wxT("%s: Bandwidth mode not implemented (%d)."), funcName,
		  p->specifier);
	}
	return(-1.0);
}

/******************************** NonLinearFromF ******************************/

/*
 * Calculate the band width of a guinea-pig's filter at a characteristic
 * frequency, f.
 * At present it uses a very rough model fit to CFs at 18 kHz, 8 kHz and 300 Hz.
 * It was written to be used with the DRNL filter.
 * This routine has been fixed to ensure that once the maximum frequency for the
 * equation has been reached, the bandwidth remains the same.
 */

#define	BANDWIDTH(F)	(-1709.0 + 279.14 * log((F)))

Float
NonLinearFromF_Bandwidth(Float f)
{
	Float	bandwidth;

	bandwidth = BANDWIDTH(f);
	return((bandwidth > 0.0)? bandwidth: f / 10.0);

}

#undef	BANDWIDTH

/******************************** GuineaPigFromF ******************************/

/*
 * Calculate the band width of a guinea-pig's filter at a characteristic
 * frequency, f.
 * This function uses the equation ERB = 0.29 CF^0.56 from: Evens E. F. (2001)
 * "Latest comparisons between physiological and behavioural frequency
 * selectivity" In: Breebaart, D. Houtsma A., Kohlrausch A. Prijs V.,
 * Schoonhoven R. (Eds), "Proceedings of the 12th Internation Symposium on
 * Hearing, Physiological and Psychological Bases of Audtiory Function."
 * Maastrict: Shaker BV. pp 382-387.
 */

Float
GuineaPigFromF_Bandwidth(Float f)
{
	return(290.0 * pow(f / 1000.0, 0.56));

}

/******************************** CatFromF ************************************/

/*
 * Calculate the band width of a cat's filter at a characteristic
 * frequency, f.
 * At present Kiang's data (Pickles, 2ndEd, figure 4.5) is used.
 * N.B. The parameters have been divided by 2 to convert from 10 dB down
 * bandwidth to 3 dB down bandwidth, as used by the gamma tone filter algorithm.
 */

Float
CatFromF_Bandwidth(Float f)
{
	Float	ff;

	ff = f * f;
	return(112.8 + 0.0598 * f - 4.159e-7 * ff + 3.599e-11 * (ff * f));

}

/******************************** ERBFromF ************************************/

/*
 * Calculate the erb at frequency f (new M&G formula).
 */

Float
ERBFromF_Bandwidth( Float theFrequency )
{
	return ( K1_ERB * ( K2_ERB * theFrequency + 1.0 ) );

}

/******************************** CustomERBFromF ******************************/

/*
 * Calculate the erb at frequency f (new M&G formula), allowing the change of
 * the constants.
 * This routine uses the equation in a different form from ERBFromF.  In this
 * form the orignal values are bwMin = 24.7, 1/quality = 0.108.
 * This routine expects checks for quality = 0.0 to be made eslewhere.
 */

Float
CustomERBFromF_Bandwidth( Float theFrequency, Float bwMin, Float quality )
{
	return ( bwMin + 1.0 / quality * theFrequency );

}

/******************************** FFromERB ************************************/

/*
 * Calculate the frequency at the associated ERB bandwidth (new M&G formula).
 */

Float
FFromERB_Bandwidth( Float theERB)
{
	return ( (theERB / K1_ERB - 1.0) / K2_ERB );

}

/****************************** ERBRateFromF **********************************/

/*
 * Calculate the ERB rate (ERBs per Hz) at the specified frequency.
 * It assumes that no negative centre frequencies are used (They could cause
 * problems.
 */

Float
ERBRateFromF_Bandwidth(Float theFrequency)
{
	return( K3_ERB * log10(K2_ERB * theFrequency + 1.0) );

}

/****************************** FFromERBRate **********************************/

/*
 * Calculate the frequency (ERBs per Hz) at the specified ERB rate.
 */

Float
FFromERBRate_Bandwidth(Float theERBRate)
{
	return( (pow(10.0, theERBRate / K3_ERB) - 1.0) / K2_ERB);

}

