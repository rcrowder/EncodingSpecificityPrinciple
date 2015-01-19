/**********************
 *
 * File:		UtCmplxm.h
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

#ifndef	_UTCMPLXM_H
#define _UTCMPLXM_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef M_PI
#	define M_PI		3.1415926535897932
#endif
#define TWOPI		(2.0 * M_PI)
#define	PI_BY_2		(M_PI / 2.0)
#define	SCALE_RADIANS_TO_DEGREES		(360.0 / TWOPI)

#ifndef TRUE
#	define	TRUE		0xFFFF
#endif
#ifndef FALSE
#	define	FALSE		0
#endif

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#define ADD_CMPLXM(Z, A, B) \
			{(Z).re = (A).re + (B).re; (Z).im = (A).im + (B).im; }

#define EXP_CMPLXM(Z, THETA)	{(Z).re = cos(THETA); (Z).im = sin(THETA); }

#define MULT_CMPLXM(Z, A, B) \
			{(Z).re = (A).re * (B).re - (A).im * (B).im; \
			(Z).im = (A).re * (B).im + (A).im * (B).re; }

#define SCALER_MULT_CMPLXM(Z, A, R) \
			{(Z).re = (A).re * (R); (Z).im = (A).im * (R); }

#define	SET_CMPLXM(Z, R, I)	{(Z).re = (R); (Z).im = (I); }

#define SUBT_CMPLXM(Z, A, B) \
			{(Z).re = (A).re - (B).re; (Z).im = (A).im - (B).im; }

#define CONV2CONJ_CMPLX(Z)	{(Z).im = -(Z).im; }

#define MODULUS_CMPLX(Z)	(sqrt((Z).re * (Z).re + (Z).im * (Z).im))

#define COPY_COMPCONJ_CMPLX(A, B)	{(A).re = (B).re; (A).im = -(B).im; }

#if HAVE_FFTW3
#	define CMPLX_PTR_RE(Z)	(*(Z))[0]
#	define CMPLX_PTR_IM(Z)	(*(Z))[1]
#	define CMPLX_RE(Z)		(Z)[0]
#	define CMPLX_IM(Z)		(Z)[1]
#	define CMPLX_COPY(A, B)	{CMPLX_RE(A) = CMPLX_RE(B); CMPLX_IM(A) = CMPLX_IM(B); }
#	define CMPLX_MULT(Z, A, B) \
			{CMPLX_RE(Z) = CMPLX_RE(A) * CMPLX_RE(B) - CMPLX_IM(A) * CMPLX_IM(B); \
			CMPLX_IM(Z) = CMPLX_RE(A) * CMPLX_IM(B) + CMPLX_IM(A) * CMPLX_RE(B); }
#	define CMPLX_MODULUS(Z)	(sqrt(CMPLX_RE(Z) * CMPLX_RE(Z) + CMPLX_IM(Z) * \
			CMPLX_IM(Z)))
#	define CMPLX_MALLOC		DSAM_FFTW_NAME(malloc)
#	define CMPLX_FREE		DSAM_FFTW_NAME(free)
#else
#	define CMPLX_PTR_RE(Z)	(Z)->re
#	define CMPLX_PTR_IM(Z)	(Z)->im
#	define CMPLX_RE(Z)		(Z).re
#	define CMPLX_IM(Z)		(Z).im
#	define CMPLX_MULT		MULT_CMPLX
#	define CMPLX_MODULUS	MODULUS_CMPLX
#	define CMPLX_MALLOC		malloc
#	define CMPLX_FREE		free
#endif

/******************************************************************************/
/*************************** Type Definitions *********************************/
/******************************************************************************/

#if !defined(BOOLEAN_ALGEBRA)
#	define BOOLEAN_ALGEBRA
	typedef	int	BOOLN;
#endif

typedef struct {

	Float	re, im;

} Complex, *ComplexPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	Add_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c);

void	Convert_CmplxM(ComplexPtr a, ComplexPtr b);	/* .re has magnit.,
												 * .im has tan(theta) */

void	Copy_CmplxM(ComplexPtr a,ComplexPtr b); /*move contents of a to b */

BOOLN	Div_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c);

Float	Modulus_CmplxM(ComplexPtr z);

void	Mult_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c);

Float	Phase_CmplxM(ComplexPtr z);

void	Power_CmplxM(ComplexPtr a, Float n);

void	RThetaSet_CmplxM(Float r, Float theta, ComplexPtr z);

void	ScalerMult_CmplxM(ComplexPtr z, Float scaler);

void	Subt_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c);

__END_DECLS

#endif
