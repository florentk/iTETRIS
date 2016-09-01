/****************************************************************************/
/// @file    NIVissimEdgePosMap.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: NIVissimEdgePosMap.h 18095 2015-03-17 09:39:00Z behrisch $
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimEdgePosMap_h
#define NIVissimEdgePosMap_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <map>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimEdgePosMap {
public:
    NIVissimEdgePosMap();
    ~NIVissimEdgePosMap();
    void add(int edgeid, SUMOReal pos);
    void add(int edgeid, SUMOReal from, SUMOReal to);
    void join(NIVissimEdgePosMap& with);
private:
    typedef std::pair<SUMOReal, SUMOReal> Range;
    typedef std::map<int, Range> ContType;
    ContType myCont;
};


#endif

/****************************************************************************/

