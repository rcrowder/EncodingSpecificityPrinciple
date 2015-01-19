/**********************
 *
 * File:		ExtCommon.h
 * Purpose:		GUI General header
 * Comments:	23-06-98 LPO: Introduced manager window resizing:
 * Author:		L. P. O'Mard
 * Created:		12 Dec 2003
 * Updated:		
 * Copyright:	(c) 2003, 2010 Lowel P. O'Mard
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

#ifndef	_EXTCOMMON_H
#define _EXTCOMMON_H	1

#if defined(LIBRARY_COMPILE) && defined(GRAPHICS_SUPPORT)
#	define USE_GUI = 1
#endif

// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif
// Any files included regardless of precompiled headers

#if wxUSE_UNICODE
#	define DSAM_USE_UNICODE		1
#endif

#endif
