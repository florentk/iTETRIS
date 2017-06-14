/****************************************************************************/
/// @file    windows_config.h
/// @author  Daniel Krajzewicz
/// @author  Tino Morenz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Felix Brack
/// @author  Jakob Erdmann
/// @date    Mon, 17 Dec 2001
/// @version $Id: windows_config.h 24130 2017-05-01 19:45:53Z behrisch $
///
// The general windows configuration file
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef config_h
#define config_h

#ifndef _MSC_VER
#error This file is for MSVC compilation only. GCC should use configure generated config.h.
#endif

/* Disable "decorated name length exceeded, name was truncated" warnings. */
#pragma warning(disable: 4503)
/* Disable "identifier truncated in debug info" warnings. */
#pragma warning(disable: 4786)
/* Disable "C++ Exception Specification ignored" warnings */
#pragma warning(disable: 4290)
/* Disable DLL-Interface warnings */
#pragma warning(disable: 4251)

/* Disable "unsafe" warnings for crt functions in VC++ 2005. */
#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef WIN32
#define WIN32
#endif

/* Define for dynamic Fox linkage */
#define FOXDLL 1

/* defines the epsilon to use on position comparison */
#define POSITION_EPS 0.1

/* defines the epsilon to use on general floating point comparison */
#define NUMERICAL_EPS 0.001

/* Define if auto-generated version.h should be used. */
//#define HAVE_VERSION_H 1

/* Version number of package */
#ifndef HAVE_VERSION_H
#define VERSION_STRING "0.30.0"
#endif

/* Definition for the character function of Xerces  */
#define XERCES3_SIZE_t XMLSize_t //Xerces >= 3.0
//#define XERCES3_SIZE_t unsigned int //Xerces < 3.0

#endif
