/****************************************************************************/
/// @file    ValueSource.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: ValueSource.h 9525 2011-01-04 21:22:52Z behrisch $
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
#ifndef ValueSource_h
#define ValueSource_h


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
class ValueSource {
public:
    ValueSource() { }
    virtual ~ValueSource() { }
    virtual T getValue() const = 0;
    virtual ValueSource *copy() const = 0;
    virtual ValueSource<SUMOReal> *makeSUMORealReturningCopy() const = 0;

};


#endif

/****************************************************************************/

