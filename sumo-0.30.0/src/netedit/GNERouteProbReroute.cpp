/****************************************************************************/
/// @file    GNERouteProbReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id: GNERouteProbReroute.cpp 23640 2017-03-24 09:17:38Z palcraft $
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include <utils/common/ToString.h>

#include "GNERouteProbReroute.h"
#include "GNEEdge.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbReroute::GNERouteProbReroute(GNERerouterInterval& rerouterIntervalParent, std::string newRouteId, double probability) :
    myRerouterIntervalParent(&rerouterIntervalParent),
    myNewRouteId(newRouteId),
    myProbability(probability),
    myTag(SUMO_TAG_ROUTE_PROB_REROUTE) {
    // set probability manually to avoid non valid values
    setProbability(probability);
}


GNERouteProbReroute::~GNERouteProbReroute() {
}


std::string
GNERouteProbReroute::getNewRouteId() const {
    return myNewRouteId;
}


void
GNERouteProbReroute::setNewRouteId(std::string newRouteId) {
    myNewRouteId = newRouteId;
}


double
GNERouteProbReroute::getProbability() const {
    return myProbability;
}


bool
GNERouteProbReroute::setProbability(double probability) {
    if (probability >= 0 && probability <= 1) {
        myProbability = probability;
        return true;
    } else {
        return false;
    }
}


SumoXMLTag
GNERouteProbReroute::getTag() const {
    return myTag;
}


const GNERerouterInterval&
GNERouteProbReroute::getRerouterIntervalParent() const {
    return *myRerouterIntervalParent;
}


bool
GNERouteProbReroute::operator==(const GNERouteProbReroute& routeProbReroute) const {
    if ((myRerouterIntervalParent == routeProbReroute.myRerouterIntervalParent) &&
            (myNewRouteId == routeProbReroute.myNewRouteId) &&
            (myProbability && routeProbReroute.myProbability)) {
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
