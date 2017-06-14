/****************************************************************************/
/// @file    NIVissimEdgePosMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: NIVissimEdgePosMap.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// -------------------
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <map>
#include "NIVissimEdgePosMap.h"


NIVissimEdgePosMap::NIVissimEdgePosMap() {}


NIVissimEdgePosMap::~NIVissimEdgePosMap() {}


void
NIVissimEdgePosMap::add(int edgeid, double pos) {
    add(edgeid, pos, pos);
}


void
NIVissimEdgePosMap::add(int edgeid, double from, double to) {
    if (from > to) {
        double tmp = from;
        from = to;
        to = tmp;
    }
    ContType::iterator i = myCont.find(edgeid);
    if (i == myCont.end()) {
        myCont[edgeid] = Range(from, to);
    } else {
        double pfrom = (*i).second.first;
        double pto = (*i).second.second;
        if (pfrom < from) {
            from = pfrom;
        }
        if (pto > to) {
            to = pto;
        }
        myCont[edgeid] = Range(from, to);
    }
}


void
NIVissimEdgePosMap::join(NIVissimEdgePosMap& with) {
    for (ContType::iterator i = with.myCont.begin(); i != with.myCont.end(); i++) {
        add((*i).first, (*i).second.first, (*i).second.second);
    }
}



/****************************************************************************/

