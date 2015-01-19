/**********************
 *
 * File:		UtRemez.h
 * Purpose:		Parks-McClellan algorithm for FIR filter design (C version)
 * Comments:
 * Author:		Jake Janovetz, additions/revisions L. P. O'Mard
 * Created:
 * Updated:		04 Dec 2000
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

#ifndef __REMEZ_H__
#define __REMEZ_H__

#define BANDPASS       1
#define DIFFERENTIATOR 2
#define HILBERT        3

#define NEGATIVE       0
#define POSITIVE       1

#ifndef Pi
#	define Pi             3.1415926535897932
#endif
#define Pi2            6.2831853071795865

#define GRIDDENSITY    16
#define MAXITERATIONS  40

/* Function prototype for remez() - the only function that should need be
 * called from external code
 */
void remez(Float h[], int numtaps,
           int numFreqs, Float bands[], Float des[], Float weight[],
           int type);

#endif /* __REMEZ_H__ */

