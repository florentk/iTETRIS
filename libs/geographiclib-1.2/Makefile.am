#
# Makefile.am
#
# Copyright (C) 2009, Francesco P. Lovergine <frankie@debian.org>
# $Id: Makefile.am 6829 2010-05-21 15:12:33Z karney $

AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src tools include

EXTRA_DIST = AUTHORS 00README.txt COPYING NEWS INSTALL \
			 Makefile.mk windows maxima doc 

dist-hook:
	find $(distdir)/windows -mindepth 1 -type d | xargs rm -rf ; \
	find $(distdir)/windows -type f \
		\( ! -name '*.sln' -a ! -name '*.vcproj' -a ! -name '*.mk' \)| \
		xargs rm -f ; \
	find $(distdir) \
		\( -name .svn -o -name .git -o -name CVS -o -name Makefile -o -name '*~' \)| \
		xargs rm -rf ; \
	echo include Makefile.mk > $(distdir)/Makefile

