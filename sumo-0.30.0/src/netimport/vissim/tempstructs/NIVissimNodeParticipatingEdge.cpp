/****************************************************************************/
/// @file    NIVissimNodeParticipatingEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: NIVissimNodeParticipatingEdge.cpp 23529 2017-03-18 10:22:36Z behrisch $
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


#include "NIVissimEdge.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeParticipatingEdge.h"



NIVissimNodeParticipatingEdge::NIVissimNodeParticipatingEdge(
    int edgeid, double frompos, double topos)
    : myEdgeID(edgeid), myFromPos(frompos), myToPos(topos) {}

NIVissimNodeParticipatingEdge::~NIVissimNodeParticipatingEdge() {}


int
NIVissimNodeParticipatingEdge::getID() const {
    return myEdgeID;
}


bool
NIVissimNodeParticipatingEdge::positionLiesWithin(double pos) const {
    return
        (myFromPos < myToPos && myFromPos >= pos && myToPos <= pos)
        ||
        (myFromPos > myToPos && myFromPos <= pos && myToPos >= pos);
}

double
NIVissimNodeParticipatingEdge::getFromPos() const {
    return myFromPos;
}


double
NIVissimNodeParticipatingEdge::getToPos() const {
    return myToPos;
}



/****************************************************************************/

