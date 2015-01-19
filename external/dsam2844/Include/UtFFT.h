/**********************
 *
 * File:		UtFFT.h
 * Purpose:		This module contains the fast fourier transform analysis
 *				routines.
 * Comments:	This was renamed from the old AnFourierT module.
 * Author:		L. P. O'Mard
 * Created:		17-07-00
 * Updated:
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
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

#ifndef	_UTFFT_H
#define _UTFFT_H	1

#if HAVE_FFTW3
#	include <fftw3.h>
#endif

#include "UtCmplxM.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	FORWARD_FT		1			/* Forward FT */
#define	BACKWARD_FT		-1			/* Backward FT */

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#if DSAM_USE_FLOAT
#	define DSAM_FFTW_NAME(NAME)	fftwf_ ## NAME
#else
#	define DSAM_FFTW_NAME(NAME)	fftw_ ## NAME
#endif

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

#if HAVE_FFTW3
	typedef DSAM_FFTW_NAME(complex)	Complx, *ComplxPtr;
	typedef DSAM_FFTW_NAME(plan)	Fftw_plan;

	typedef struct {		/* Used so that logical lengths for the arrays can be set */

		int		numPlans;
		Fftw_plan	*plan;
		Float	*data;
		unsigned long	arrayLen;
		unsigned long	fftLen;
		unsigned long	dataLen;

	} FFTArray, *FFTArrayPtr;
#else
#	define Complx	Complex
#	define ComplxPtr	ComplexPtr
#endif

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	Calc_FFT(Float *data, unsigned long nn, int isign);

void	CalcComplex_FFT(Complex data[], unsigned long nn, int isign);

BOOLN	CalcReal_FFT(SignalDataPtr signal, Float *fT, int direction);

void	CreateNoiseBand_FFT(FFTArrayPtr fTInv, int plan, RandParsPtr randPars,
		  int kLow, int kUpp);

#if HAVE_FFTW3
	FFTArrayPtr	InitArray_FFT(unsigned long dataLen, BOOLN forInPlaceFFT, int numPlans);

	void	FreeArray_FFT(FFTArrayPtr *p);
#endif /* HAVE_FFTW3 */

unsigned long	Length_FFT(unsigned long length);

__END_DECLS

#endif
