#######################
##
## File:		Makefile.am
## Purpose:		src/ExtLib directory makefile.
## Comments:	Process this file with automake to produce Makefile.in
## Author:		L. P. O'Mard
## Created:		
## Updated:
## Copyright:	(c) 2010 Lowel P. O'Mard
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

AUTOMAKE_OPTIONS = foreign

if DSAM_EXTENSIONS

include $(top_srcdir)/src/makeunix.env

lib_LTLIBRARIES = libdsam_ext.la

libdsam_ext_la_CPPFLAGS	= -DLIBRARY_COMPILE $(MAIN_INC) $(WX_BASE_CXXFLAGS) \
					-I$(top_srcdir)/src/MainLib -I../MainLib

DSAM_Ext_Sources	= ExtMainApp.cpp ExtIPCServer.cpp ExtSimThread.cpp \
						ExtIPCUtils.cpp ExtIPCClient.cpp ExtSocket.cpp \
						ExtSocketServer.cpp ExtProcThread.cpp \
						ExtRunThreadedProc.cpp ExtXMLDocument.cpp \
						ExtSingleProcThread.cpp ExtProcChainThread.cpp

libdsam_ext_la_SOURCES= $(DSAM_Ext_Sources)

libdsam_ext_la_LDFLAGS= $(GenLaLDFLAGS)

else
endif
