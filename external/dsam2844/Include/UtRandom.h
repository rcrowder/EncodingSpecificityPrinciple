/**********************
 *
 * File:		UtRandom.h
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

#ifndef	_UTRANDOM_H
#define _UTRANDOM_H	1

/******************************************************************************/
/************************** Constant definitions ******************************/
/******************************************************************************/

#define RANDOM_INITIAL_SEED		3141L

#define RANDOM_IA 16807
#define RANDOM_IM 2147483647L
#define RANDOM_AM (1.0 / RANDOM_IM)
#define RANDOM_IQ 127773L
#define RANDOM_IR 2836
#define RANDOM_NDIV (1 + (RANDOM_IM - 1) / RANDOM_NTAB)
#define RANDOM_EPS 1.2e-7
#define RANDOM_RNMX (1.0 - RANDOM_EPS)
#define RANDOM_NTAB				32

/******************************************************************************/
/************************** Macro definitions *********************************/
/******************************************************************************/

/************************** _Ran01 *********************************************/

/*
 * This routine is a uniformally distributed random number generator.
 * It has been revised from the "Numerical Recipes 2nd ed." Ran1 (p.280).
 * Set "seed" to any negative value to initialise or re-intialise the sequence.
 */

#define	_Ran01_Random(P, RES) \
	{ \
		int		j; \
		long	k;	\
		Float	temp; \
		if ((P)->idum <= 0 || !(P)->iy) { \
			if ((P)->idum == 0) \
				(P)->idum = (long) clock(); \
			else \
				(P)->idum = (-((P)->idum) < 1)? 1: -((P)->idum); \
			(P)->idum += (P)->offset; \
			for (j = RANDOM_NTAB + 7; j >= 0; j--) { \
				k = ((P)->idum) / RANDOM_IQ; \
				(P)->idum = RANDOM_IA * ((P)->idum - k * RANDOM_IQ) - RANDOM_IR * k; \
				if ((P)->idum < 0) \
					(P)->idum += RANDOM_IM; \
				if (j < RANDOM_NTAB)  \
					(P)->iv[j] = (P)->idum; \
			} \
			(P)->iy = (P)->iv[0]; \
		} \
		k = ((P)->idum) / RANDOM_IQ; \
		(P)->idum = RANDOM_IA * ((P)->idum - k * RANDOM_IQ) - RANDOM_IR * k; \
		if ((P)->idum < 0) \
			(P)->idum += RANDOM_IM; \
		j = (P)->iy / RANDOM_NDIV; \
		(P)->iy = (P)->iv[j]; \
		(P)->iv[j] = (P)->idum; \
		(RES) = ((temp = RANDOM_AM * (P)->iy) > RANDOM_RNMX)? RANDOM_RNMX: temp; \
	}

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/
typedef struct {

	long	idum;
	long	offset;
	long	iy;
	long	iv[RANDOM_NTAB];

} RandPars, *RandParsPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern long	randomNumSeed;			/* Seed for the random number generator */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	FreePars_Random(RandParsPtr *p);

int		GeomDist_Random(Float probability, int numTrials, RandParsPtr p);

Float	GaussRan01_Random(RandParsPtr p);

RandParsPtr	InitPars_Random(long idum, long offset);

Float	Ran_Random(RandParsPtr p, Float minValue, Float maxValue);

Float	Ran01_Random(RandParsPtr p);

BOOLN	SetSeed_Random(RandParsPtr p, long ranSeed, long offset);

__END_DECLS

#endif
