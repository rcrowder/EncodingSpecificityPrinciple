/**********************
 *
 * File:		UtRandom.c
 * Purpose:		This is the random number routines module.
 * Comments:	22-03-98 LPO Added support for seeds set the time for
 *				completely random numbers.
 *				17-11-05 LPO Added the offset for setting seeds in thread mode.
 * Author:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		22 Mar 1998
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
#include <math.h>
#include <float.h>
#include <time.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "UtRandom.h"

/******************************************************************************/
/************************** Global Variables **********************************/
/******************************************************************************/

/******************************************************************************/
/************************** Subroutines and functions *************************/
/******************************************************************************/

/************************** InitPars ******************************************/

/*
 * This routine initialises the a RandPars structure.
 * It returns NULL if it fails in any way.
 */

RandParsPtr
InitPars_Random(long idum, long offset)
{
	static const WChar *funcName = wxT("InitPars_Random");
	RandParsPtr	p;

	if ((p = (RandParsPtr) malloc(sizeof(RandPars))) == NULL) {
		NotifyError(wxT("%s: Could not initialises structure."), funcName);
		return(NULL);
	}
	p->idum = idum;
	p->offset = offset;
	p->iy = 0;
	return(p);

}

/************************** FreePars ******************************************/

/*
 * This routine frees the memory allocated for a RandPars structure.
 */

void
FreePars_Random(RandParsPtr *p)
{
	if (!*p)
		return;
	free(*p);
	*p = NULL;

}

/************************** SetSeed *******************************************/

/*
 * This function sets the random number seed.
 * The offset is used for setting different seeds in thread processing mode.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetSeed_Random(RandParsPtr p, long ranSeed, long offset)
{
	static const WChar *funcName = wxT("SetSeed_Random");
	if (!p) {
		NotifyError(wxT("%s: Structure is not initialised."), funcName);
		return(FALSE);
	}
	p->idum = ranSeed;
	p->offset = offset;
	return(TRUE);

}

/************************** Ran01 *********************************************/

/*
 * This routine is a uniformally distributed random number generator.
 * It has been revised from the "Numerical Recipes 2nd ed." Ran1 (p.280).
 * Set "seed" to any negative value to initialise or re-intialise the sequence.
 */

Float
Ran01_Random(RandParsPtr p)
{
	int j;
	long k;
	Float temp;

	if (p->idum <= 0 || !p->iy) {
		if (p->idum == 0)
			p->idum = (long) time(NULL);
		else
			p->idum = (-(p->idum) < 1)? 1: -(p->idum);
		p->idum += p->offset;
		for (j = RANDOM_NTAB + 7; j >= 0; j--) {
			k = (p->idum) / RANDOM_IQ;
			p->idum = RANDOM_IA * (p->idum - k * RANDOM_IQ) - RANDOM_IR * k;
			if (p->idum < 0)
				p->idum += RANDOM_IM;
			if (j < RANDOM_NTAB)
				p->iv[j] = p->idum;
		}
		p->iy = p->iv[0];
	}
	k = (p->idum) / RANDOM_IQ;
	p->idum = RANDOM_IA * (p->idum - k * RANDOM_IQ) - RANDOM_IR * k;
	if (p->idum < 0)
		p->idum += RANDOM_IM;
	j = p->iy / RANDOM_NDIV;
	p->iy = p->iv[j];
	p->iv[j] = p->idum;
	return( ((temp = RANDOM_AM * p->iy) > RANDOM_RNMX)? RANDOM_RNMX: temp);

}

/************************** GeomDist ******************************************/

/*
 * This routine calculates the number of times that an event occurs with
 * a set probability and number of trials.
 * It uses the geometric distribution method, but the probability must be
 * small.
 * The zero numTrials check speeds up processing when many such conditions
 * are processed.
 */

int
GeomDist_Random(Float probability, int numTrials, RandParsPtr p)
{
	int		i;
	Float	sum, logNotProbable;

	#ifdef DEBUG
	static const WChar *funcName = wxT("GeomDist_Random");
		if (probability >= 1.0) {
			NotifyError(wxT("%s: probability = %g: greater than 1."), funcName,
			  probability);
			exit(1);
		}
	#endif
	if ((numTrials <= 0) || (probability < DSAM_EPSILON))
		return(0);
	logNotProbable = log(1.0 - probability);
	for (i = 0, sum = 0.0; ((sum += log(Ran01_Random(p)) / logNotProbable) <
	  numTrials); i++)
		;
	if (i > numTrials)
		i = numTrials;
	return (i);

}

/************************** GaussRan01 ****************************************/

/*
 * This routine is a Gaussian (normally) distributed random number generator.
 * It has been revised from the "Numerical Recipes 2nd ed." Ran1 (p.289).
 */

Float
GaussRan01_Random(RandParsPtr p)
{
	static BOOLN	isSet = FALSE;
	static Float	gSet;
	Float	fac, rSquared, v1, v2;

	if (!isSet) {
		do {
			v1 = 2.0 * Ran01_Random(p) - 1.0;
			v2 = 2.0 * Ran01_Random(p) - 1.0;
			rSquared = SQR(v1) + SQR(v2);
		} while ((rSquared >= 1.0) || (rSquared == 0.0));
		fac = sqrt(-2.0 * log(rSquared) / rSquared);
		gSet = v1 * fac;
		isSet = TRUE;
		return (v2 * fac);
	} else {
		isSet = FALSE;
		return (gSet);
	}
}

/************************** Ran ***********************************************/

/*
 * This routine calculates a random number between a set of limits.
 * Set "seed" to any negative value to initialise or re-intialise the sequence.
 */

Float
Ran_Random(RandParsPtr p, Float minValue, Float maxValue)
{
	return(Ran01_Random(p) * (maxValue - minValue) + minValue);

}
