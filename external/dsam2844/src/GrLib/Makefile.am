#######################
##
## File:		Makefile.am
## Purpose:		src/GrLib directory makefile.
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

if DSAM_GRAPHICS
SUBDIRS		= Bitmaps

include $(top_srcdir)/src/makeunix.env

lib_LTLIBRARIES = libdsam_g.la
EXTRA_DIST		= makefile.b32

libdsam_g_la_CPPFLAGS	= -DLIBRARY_COMPILE $(MAIN_INC) $(WXINC) \
					-I$(top_srcdir)/src/MainLib -I../MainLib

libdsam_g_la_SOURCES= GrBrushes.cpp GrFonts.cpp GrAxisScale.cpp \
				GrLines.cpp GrCanvas.cpp GrDisplayS.cpp \
				GrSimMgr.cpp GrUtils.cpp GrPrintDisp.cpp GrModParDialog.cpp \
				GrParListInfo.cpp GrDialogInfo.cpp GrParListInfoList.cpp \
				GrParControl.cpp GrSDIDoc.cpp GrIPCServer.cpp GrMainApp.cpp \
				GrSDIPalette.cpp GrSDIView.cpp GrSDIFrame.cpp GrSDICanvas.cpp \
				GrSDIDiagram.cpp GrSDIEvtHandler.cpp GrSDICommand.cpp \
				GrSDIShapes.cpp GrDiagFrame.cpp GrSDIDocManager.cpp \
				GrSDIXMLDoc.cpp GrSDIBaseShapes.cpp

libdsam_g_la_LDFLAGS= $(GenLaLDFLAGS)

else
endif
