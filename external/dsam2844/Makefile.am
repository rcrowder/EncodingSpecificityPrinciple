## Top directory makefile.
#######################
##
## File:		Makefile.am
## Purpose:		Top directory makefile.
## Comments:	SVN ID $Id
##				Process this file with automake to produce Makefile.in
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

pkginclude_HEADERS	= DSAMSetup.h

bin_SCRIPTS		= dsam-config
BUILT_SOURCES	= dsam.spec dsam-config
man7_MANS		= dsam.7
EXTRA_DIST		= autogen.sh LICENSE DSAM_SDK.nsi
SUBDIRS			= CfgTools Include src ModuleSpecs Examples MSVC NSIS debian

dsam.spec: dsam.spec.in

dsam-config:	dsam-config.in

dsam.7: manpage.xml
		xmlto man manpage.xml

dist-hook: dsam.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in" \
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) ; \
	fi \
	&& cp dsam.spec $(distdir)
