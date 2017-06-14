/****************************************************************************/
/// @file    MSEdgeWeightsStorage.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    02.11.2009
/// @version $Id: MSEdgeWeightsStorage.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A storage for edge travel times and efforts
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

#include "MSEdgeWeightsStorage.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSEdgeWeightsStorage::MSEdgeWeightsStorage() {
}


MSEdgeWeightsStorage::~MSEdgeWeightsStorage() {
}


bool
MSEdgeWeightsStorage::retrieveExistingTravelTime(const MSEdge* const e, const double t, double& value) const {
    std::map<const MSEdge*, ValueTimeLine<double> >::const_iterator i = myTravelTimes.find(e);
    if (i == myTravelTimes.end()) {
        return false;
    }
    const ValueTimeLine<double>& tl = (*i).second;
    if (!tl.describesTime(t)) {
        return false;
    }
    value = tl.getValue(t);
    return true;
}


bool
MSEdgeWeightsStorage::retrieveExistingEffort(const MSEdge* const e, const double t, double& value) const {
    std::map<const MSEdge*, ValueTimeLine<double> >::const_iterator i = myEfforts.find(e);
    if (i == myEfforts.end()) {
        return false;
    }
    const ValueTimeLine<double>& tl = (*i).second;
    if (!tl.describesTime(t)) {
        return false;
    }
    value = tl.getValue(t);
    return true;
}


void
MSEdgeWeightsStorage::addTravelTime(const MSEdge* const e,
                                    double begin, double end,
                                    double value) {
    std::map<const MSEdge*, ValueTimeLine<double> >::iterator i = myTravelTimes.find(e);
    if (i == myTravelTimes.end()) {
        myTravelTimes[e] = ValueTimeLine<double>();
        i = myTravelTimes.find(e);
    }
    (*i).second.add(begin, end, value);
}


void
MSEdgeWeightsStorage::addEffort(const MSEdge* const e,
                                double begin, double end,
                                double value) {
    std::map<const MSEdge*, ValueTimeLine<double> >::iterator i = myEfforts.find(e);
    if (i == myEfforts.end()) {
        myEfforts[e] = ValueTimeLine<double>();
        i = myEfforts.find(e);
    }
    (*i).second.add(begin, end, value);
}


void
MSEdgeWeightsStorage::removeTravelTime(const MSEdge* const e) {
    std::map<const MSEdge*, ValueTimeLine<double> >::iterator i = myTravelTimes.find(e);
    if (i != myTravelTimes.end()) {
        myTravelTimes.erase(i);
    }
}


void
MSEdgeWeightsStorage::removeEffort(const MSEdge* const e) {
    std::map<const MSEdge*, ValueTimeLine<double> >::iterator i = myEfforts.find(e);
    if (i != myEfforts.end()) {
        myEfforts.erase(i);
    }
}


bool
MSEdgeWeightsStorage::knowsTravelTime(const MSEdge* const e) const {
    return myTravelTimes.find(e) != myTravelTimes.end();
}


bool
MSEdgeWeightsStorage::knowsEffort(const MSEdge* const e) const {
    return myEfforts.find(e) != myEfforts.end();
}



/****************************************************************************/

