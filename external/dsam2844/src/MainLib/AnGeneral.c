/**********************
 *
 * File:		AnGeneral.c
 * Purpose:		This module contains various general analysis routines.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "AnGeneral.h"

/**************************** LinearRegression ********************************/

/*
 * This subroutine calculates the best fit line parameters for a set of data
 * points, x,y, where x = dx * i.
 * It returns the parameters as arguments.
 */

BOOLN
LinearRegression_GenAnalysis(Float *y0, Float *gradient, Float *y,
  Float dx, ChanLen startIndex, ChanLen length)
{
	static const WChar *funcName = wxT("LinearRegression_GenAnalysis");
	Float  a, b, c, e, f, x;
	ChanLen	i, endIndex;

	if (length < 2) {
		NotifyError(wxT("%s: Illegal calculation length (%lu)."), funcName,
		  length);
		return(FALSE);
	}
	b = c = e = f = 0.0;
	a = (Float) length;
	endIndex = length + startIndex;
	y += startIndex;
	for (i = startIndex; i < endIndex; i++) {
		x = dx * i;
		b += x;
		c += x * x;
		e += *y;
		f += x * *(y++);
	}
	*gradient = (f - b * e / a) / (c - b * b / a);
	*y0 = (e - b * *gradient) / a;
	return(TRUE);

} /* LinearRegression_GenAnalysis */

/****************************** EuclideanDistance *****************************/

/*
 * This function compares two arrays using a Euclidean distance
 * measure: m = sum[(ti - si)^2].
 * The smaller the return value, m, the better the match between the two
 * arrays.
 * This routine assumes that startIndex and endIndex are within the bounds
 * of both arrays.
 */

Float
EuclideanDistance_GenAnalysis(Float *arrayA, Float *arrayB,
  ChanLen startIndex, ChanLen endIndex)
{
	static const WChar *funcName = wxT("Euclidean_GenAnalysis");
	Float		sum, difference;
	ChanLen		i;
	Float		*ptr1, *ptr2;

	if (arrayA == NULL) {
		NotifyError(wxT("%s: Array A not initialised."), funcName);
		exit(1);
	}
	if (arrayB == NULL) {
		NotifyError(wxT("%s: Array B not initialised."), funcName);
		exit(1);
	}
	if (endIndex < startIndex) {
		NotifyError(wxT("%s: Start "), funcName);
		exit(1);
	}
	ptr1 = arrayA + startIndex;
	ptr2 = arrayB + startIndex;
	for (i = startIndex, sum = 0.0; i < endIndex; i++) {
		difference = *ptr1++ - *ptr2++;
		sum += difference * difference;
	}
	return(sum);

}

