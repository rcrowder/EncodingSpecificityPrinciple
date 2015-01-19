/**********************
 *
 * File:		GrFonts.h
 * Purpose:		Global font variable and handling routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		27 Oct 1998
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
 
#ifndef _GRFONTS_H
#define _GRFONTS_H 1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern wxFont    *diagnosticsWindowFont;
extern wxFont    *itemFont;
extern wxFont    *labelFont;
extern wxFont    *textWindowFont;

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

void	Init_Fonts(void);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

