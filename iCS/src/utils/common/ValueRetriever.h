/****************************************************************************/
/// @file    ValueRetriever.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: ValueRetriever.h 9525 2011-01-04 21:22:52Z behrisch $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ValueRetriever_h
#define ValueRetriever_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
template<typename T>
class ValueRetriever {
public:
    ValueRetriever() { }
    virtual ~ValueRetriever() { }
    virtual void addValue(T value) = 0;

};


#endif

/****************************************************************************/

