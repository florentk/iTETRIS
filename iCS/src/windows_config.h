/****************************************************************************/
/// @file    windows_config.h
/// @author  Daniel Krajzewicz
/// @date    08.09.2009
/// @version $Id: windows_config.h 7606 2009-08-04 21:37:38Z behrisch $
///
// The general windows configuration file
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef config_h
#define config_h

#ifndef _MSC_VER
#error This file is for MSVC compilation only. GCC should use configure generated config.h.
#endif

/* Disable "identifier truncated in debug info" warnings. */
#pragma warning(disable: 4786)
/* Disable "C++ Exception Specification ignored" warnings */
#pragma warning(disable: 4290)

/* Disable "unsafe" warnings for crt functions in VC++ 2005. */
#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef WIN32
#define WIN32
#endif

#define uint32_t unsigned int
#define int32_t int

#define M_PI 3.1415926535897932384626433832795

/* defines the precision of floats */
#define SUMOReal float

/* defines the epsilon to use on position comparison */
#define POSITION_EPS 0.1

/* defines the number of digits after the comma in output */
#define OUTPUT_ACCURACY 2

/* defines the number of digits after the comma in output of geo-coordinates */
#define GEO_OUTPUT_ACCURACY 6

/* Define if auto-generated version.h should be used. */
#define HAVE_VERSION_H 1

/* Version number of package */
#ifndef HAVE_VERSION_H
#define VERSION_STRING "0.11.0"
#endif

#define HAVE_INTERNAL_LANES 1

/* Definition for the character function of Xerces  */
#define XERCES3_SIZE_t XMLSize_t //Xerces >= 3.0
//#define XERCES3_SIZE_t unsigned int //Xerces < 3.0

#define LOG_ON 1
#define SUMO_ON 1
#define APPLICATIONS_ON 1
//#define NS3_ON 1
//#define USE_CAM2

#endif
