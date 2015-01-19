#!/bin/sh
#######################
##
## File:		autogen.sh
## Purpose:		This script runs all of the commands necessary for configuring DSAM
##				from the CVS repository.
## Comments:	
## Author:		L. P. O'Mard
## Created:		19 Nov 2006
## Updated:
## Copyright:	(c) 2006, 2010 Lowel P. O'Mard
##
#######################
##
##  This file is part of DSAM.
##
##  DSAM is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  DSAM is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
##
#######################

#gettextize --force --copy
#libtoolize --force --copy
#aclocal
#autoheader
#autoconf
#automake --add-missing --force-missing --gnu

autoreconf -fvis
