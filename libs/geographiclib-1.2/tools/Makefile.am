#
# Makefile.am
#
# Copyright (C) 2009, Francesco P. Lovergine <frankie@debian.org>
# $Id: Makefile.am 6827 2010-05-20 19:56:18Z karney $

AM_CPPFLAGS = -Wall -funroll-loops -finline-functions -fomit-frame-pointer
INCLUDES = -I$(top_srcdir)/include 

LDADD = $(top_builddir)/src/libGeographic.la
DEPS = $(top_builddir)/src/libGeographic.la

bin_PROGRAMS = GeoConvert TransverseMercatorTest CartConvert Geod EquidistantTest GeoidEval

GeoConvert_SOURCES = GeoConvert.cpp \
	../include/GeographicLib/Constants.hpp \
	../include/GeographicLib/DMS.hpp \
	../include/GeographicLib/GeoCoords.hpp \
	../include/GeographicLib/UTMUPS.hpp

TransverseMercatorTest_SOURCES = TransverseMercatorTest.cpp \
	../include/GeographicLib/Constants.hpp \
	../include/GeographicLib/DMS.hpp \
	../include/GeographicLib/EllipticFunction.hpp \
	../include/GeographicLib/TransverseMercator.hpp \
	../include/GeographicLib/TransverseMercatorExact.hpp

CartConvert_SOURCES = CartConvert.cpp \
	../include/GeographicLib/Constants.hpp \
	../include/GeographicLib/DMS.hpp \
	../include/GeographicLib/Geocentric.hpp \
	../include/GeographicLib/LocalCartesian.hpp

Geod_SOURCES = Geod.cpp \
	../include/GeographicLib/Constants.hpp \
	../include/GeographicLib/DMS.hpp \
	../include/GeographicLib/Geodesic.hpp

EquidistantTest_SOURCES = EquidistantTest.cpp \
	../include/GeographicLib/AzimuthalEquidistant.hpp \
	../include/GeographicLib/CassiniSoldner.hpp \
	../include/GeographicLib/Constants.hpp \
	../include/GeographicLib/DMS.hpp \
	../include/GeographicLib/Geodesic.hpp

GeoidEval_SOURCES = GeoidEval.cpp \
	../include/GeographicLib/Constants.hpp \
	../include/GeographicLib/DMS.hpp \
	../include/GeographicLib/GeoCoords.hpp \
	../include/GeographicLib/Geoid.hpp

EXTRA_DIST = Makefile.mk
