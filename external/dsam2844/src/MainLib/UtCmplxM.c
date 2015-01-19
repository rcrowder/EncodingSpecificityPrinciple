/**********************
 *
 * File:		UtCmplxm.c
 * Purpose:		routines to handle complex numbers in C
 * Comments:	23-02-93 L.P.O'Mard: Incorporated into the DSAM core routines
 *				  library.
 * Author:		M.A.Stone revised by L. P. O'Mard
 * Created:		?? Sep 1992
 * Updated:		31 Jul 1997
 * Copyright:	(c) 1997, 2010 Lowel P. O'Mard
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
#include "UtCmplxM.h"

/*----------------------------------------------------------------------------*/

/*
 * Complex multiply c = a * b
 */

void
Mult_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c)
{
	Float SumRe,SumIm;

	SumRe = (a->re * b->re) - (a->im * b->im);
	SumIm = (a->re * b->im) + (a->im * b->re);
	(*c).re = SumRe;
	(*c).im = SumIm;
}

/*----------------------------------------------------------------------------*/

/* Complex divide c = a/b */

BOOLN
Div_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c)
{
	Complex Num;
	Float Denom;
	int div_succs;

	Denom  = (b->re * b->re) + (b->im * b->im);
	Num.re = (a->re * b->re) + (a->im * b->im);
	Num.im = (a->im * b->re) - (a->re * b->im);
	if(Denom != 0.0) {
		(*c).re = Num.re/Denom;
		(*c).im = Num.im/Denom;
		div_succs = TRUE;
	}
	else {
		(*c).re = 0.0;
		(*c).im = 0.0;
		div_succs = FALSE;
	}
	return(div_succs); /* return whether divide happened or not */
}

/*----------------------------------------------------------------------------*/

/* Complex addition c = a + b */

void
Add_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c)
{
	Float SumRe,SumIm;

	SumRe = (a->re + b->re);
	SumIm = (a->im + b->im);
	(*c).re = SumRe;
	(*c).im = SumIm;
}

/*----------------------------------------------------------------------------*/

/* Complex subtraction c = a - b */

void Subt_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c)
{
	Float SumRe,SumIm;

	SumRe = (a->re - b->re);
	SumIm = (a->im - b->im);
	(*c).re = SumRe;
	(*c).im = SumIm;
}

/*----------------------------------------------------------------*/

void Convert_CmplxM(ComplexPtr a,ComplexPtr b) /* Complex magnitude b = |a| */
{
	Float SumRe, SumIm;

	SumRe = (a->re * a->re) + (a->im * a->im);;
	SumRe = sqrt(SumRe);
	if(a->re != 0.0)
		SumIm = a->im / a->re;
	else
		SumIm = 0.0;
	b->re = SumRe;
	b->im = SumIm; /* puts tan(theta) in .im */
}

/*--------------------------------------------------------------*/

void Copy_CmplxM(ComplexPtr a,ComplexPtr b) /* copy contents of a to b */
{
	b->re = a->re;
	b->im = a->im;
}

/*-------------------------------------------------*/

void Power_CmplxM(ComplexPtr a, Float n) /* calculate */
/*-------------------(*a) to power n, n fractional */
{
	Float r, theta, real, imag;

	real = a->re; imag = a->im;
	r = sqrt( (real*real) + (imag*imag) );
	if(r != 0.0)
		theta = acos(real/r);
	else
		return;
	if((real > 0.0) && (imag > 0.0))
		 ; /* get quadrant correct */
	else
		if((real > 0.0) && (imag < 0.0))
			theta = TWOPI - theta;
	else
		if((real < 0.0) && (imag < 0.0))
			theta += M_PI;
	else
		if((real < 0.0) && (imag > 0.0))
			theta = M_PI - theta;

	theta *= n;	r = pow(r,n); /* perform power on r*exp(i*theta) */
	real = r*cos(theta); imag = r*sin(theta);
	a->re = real; a->im = imag;
}

/*************************** Modulus ******************************************/

/*
 * This function returns with the modulus of a complex number.
 */

Float
Modulus_CmplxM(ComplexPtr z)
{
	return (sqrt(z->re * z->re + z->im * z->im));

}

/*************************** Phase ********************************************/

/*
 * This function returns with the phase (radians) of a complex number.
 * This needs to be changed.
 */

Float
Phase_CmplxM(ComplexPtr z)
{
	return(atan2(z->im, z->re));

}

/*************************** RThetaSet ****************************************/

/*
 * This function sets a complex number using the z = R * exp(I * theta) format,
 * where R and Theta are passed as arguments.
 */

void
RThetaSet_CmplxM(Float r, Float theta, ComplexPtr z)
{
	z->re = r * cos(theta);
	z->im = r * sin(theta);

}

/*************************** ScalerMult ***************************************/

/*
 * This routine multiplies a complex number by a scaler value
 */

void
ScalerMult_CmplxM(ComplexPtr z, Float scaler)
{
	z->re *= scaler;
	z->im *= scaler;

}


