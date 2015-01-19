/**********************
 *
 * File:		GrBrushes.h
 * Purpose: 	Routines for the globally used brushes.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		26 Nov 1996
 * Updated:		
 * Copyright:	(c) 1996, 2010 Lowel P. O'Mard
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

#ifndef _GRBRUSHES_H
#define _GRBRUSHES_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	MAX_BRUSHES						20
#define	BRUSHES_GREYSCALE_MAX_RED		255
#define	BRUSHES_GREYSCALE_MAX_BLUE		255
#define	BRUSHES_GREYSCALE_MAX_GREEN		255

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

class GrBrushes
{
	int		numBrushes;
	wxBrush	*brush[MAX_BRUSHES];

  public:
	GrBrushes(void);
	~GrBrushes(void);
	
	void	SetGreyScales(int theNumGreyScales);
	wxBrush	*GetBrush(int theBrush);

};


/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern GrBrushes	*greyBrushes;

#endif

