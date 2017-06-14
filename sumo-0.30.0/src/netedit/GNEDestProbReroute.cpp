/****************************************************************************/
/// @file    GNEDestProbReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id: GNEDestProbReroute.cpp 23640 2017-03-24 09:17:38Z palcraft $
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

#include "GNEDestProbReroute.h"
#include "GNEEdge.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDestProbReroute::GNEDestProbReroute(GNERerouterInterval& rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability):
    myRerouterIntervalParent(&rerouterIntervalParent),
    myNewEdgeDestination(newEdgeDestination),
    myProbability(0),
    myTag(SUMO_TAG_DEST_PROB_REROUTE) {
    // set probability manually to avoid non valid values
    setProbability(probability);
}


GNEDestProbReroute::~GNEDestProbReroute() {
}


GNEEdge*
GNEDestProbReroute::getNewDestination() const {
    return myNewEdgeDestination;
}


void
GNEDestProbReroute::setNewDestination(GNEEdge* edge) {
    myNewEdgeDestination = edge;
}


double
GNEDestProbReroute::getProbability() const {
    return myProbability;
}


bool
GNEDestProbReroute::setProbability(double probability) {
    if (probability >= 0 && probability <= 1) {
        myProbability = probability;
        return true;
    } else {
        return false;
    }
}


SumoXMLTag
GNEDestProbReroute::getTag() const {
    return myTag;
}


const GNERerouterInterval&
GNEDestProbReroute::getRerouterIntervalParent() const {
    return *myRerouterIntervalParent;
}


bool
GNEDestProbReroute::operator==(const GNEDestProbReroute& destProbReroute) const {
    if ((myRerouterIntervalParent == destProbReroute.myRerouterIntervalParent) &&
            (myNewEdgeDestination == destProbReroute.myNewEdgeDestination) &&
            (myProbability == destProbReroute.myProbability)) {
        return true;
    } else {
        return false;
    }
}


/****************************************************************************/
