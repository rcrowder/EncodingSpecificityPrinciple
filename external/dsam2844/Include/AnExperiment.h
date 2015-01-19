/**********************
 *
 * File:		AnExperiment.c
 * Purpose:		This module contains various analysis routines, which have
 *				been used in real experiments.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		19 Mar 1993
 * Updated:		10 Oct 1996
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

#ifndef	_ANEXPERIMENT_H
#define _ANEXPERIMENT_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define ANALYSIS_MAX_INTENSITY_GUESS 50.0	/* Initial threshold intensity. */
#define MAXIMUM_THRESHOLD_LOOP		20	/* - FindThresholdIntensity routine. */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc2CompAdapt_ExpAnalysis(Float Tst[], Float Tr[], EarObjectPtr data,
		  Float shortTermT1, Float shortTermPeriod, Float rapidAdaptPeriod);

Float	CalcQValue_ExpAnalysis( EarObjectPtr (* RunModel)(void),
		  BOOLN (* SetStimulusFreq)(Float), Float centreFreq,
		  Float variationFromCF, Float dBDownDiff,
		  Float initialFreqIncrement, Float accuracy );

Float	FindThresholdIntensity_ExpAnalysis( EarObjectPtr (* RunModel)(void),
		  BOOLN (* SetIntensity)(Float), Float targetThreshold,
		  Float targetAccuracy, BOOLN *firstPass );

Float	FindThresholdIntensity_ExpAnalysis_Slow(EarObjectPtr (* RunModel)(void),
		  BOOLN (* SetIntensity)(Float), Float targetThreshold,
		  Float targetAccuracy, Float initialDeltaIntensity);

__END_DECLS

#endif
