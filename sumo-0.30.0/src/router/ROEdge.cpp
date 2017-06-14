/****************************************************************************/
/// @file    ROEdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @author  Melanie Knocke
/// @author  Yun-Pang Floetteroed
/// @date    Sept 2002
/// @version $Id: ROEdge.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A basic edge for routing applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include "ROLane.h"
#include "RONet.h"
#include "ROVehicle.h"
#include "ROEdge.h"


// ===========================================================================
// static member definitions
// ===========================================================================
bool ROEdge::myInterpolate = false;
bool ROEdge::myHaveTTWarned = false;
bool ROEdge::myHaveEWarned = false;
ROEdgeVector ROEdge::myEdges;


// ===========================================================================
// method definitions
// ===========================================================================
ROEdge::ROEdge(const std::string& id, RONode* from, RONode* to, int index, const int priority) :
    Named(id),
    myFromJunction(from),
    myToJunction(to),
    myIndex(index),
    myPriority(priority),
    mySpeed(-1),
    myLength(0),
    myUsingTTTimeLine(false),
    myUsingETimeLine(false),
    myCombinedPermissions(0) {
    while ((int)myEdges.size() <= index) {
        myEdges.push_back(0);
    }
    myEdges[index] = this;
    if (from == 0 && to == 0) {
        // TAZ edge, no lanes
        myCombinedPermissions = SVCAll;
    }
}


ROEdge::~ROEdge() {
    for (std::vector<ROLane*>::iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        delete(*i);
    }
}


void
ROEdge::addLane(ROLane* lane) {
    assert(myLanes.empty() || lane->getLength() == myLength);
    myLength = lane->getLength();
    const double speed = lane->getSpeed();
    mySpeed = speed > mySpeed ? speed : mySpeed;
    myLanes.push_back(lane);

    // integrate new allowed classes
    myCombinedPermissions |= lane->getPermissions();
}


void
ROEdge::addSuccessor(ROEdge* s, std::string) {
    if (find(myFollowingEdges.begin(), myFollowingEdges.end(), s) == myFollowingEdges.end()) {
        myFollowingEdges.push_back(s);
        s->myApproachingEdges.push_back(this);
    }
}


void
ROEdge::addEffort(double value, double timeBegin, double timeEnd) {
    myEfforts.add(timeBegin, timeEnd, value);
    myUsingETimeLine = true;
}


void
ROEdge::addTravelTime(double value, double timeBegin, double timeEnd) {
    myTravelTimes.add(timeBegin, timeEnd, value);
    myUsingTTTimeLine = true;
}


double
ROEdge::getEffort(const ROVehicle* const veh, double time) const {
    double ret = 0;
    if (!getStoredEffort(time, ret)) {
        return myLength / MIN2(veh->getType()->maxSpeed, mySpeed);
    }
    return ret;
}


double
ROEdge::getDistanceTo(const ROEdge* other) const {
    if (getToJunction() != 0 && other->getFromJunction() != 0) {
        return getToJunction()->getPosition().distanceTo2D(other->getFromJunction()->getPosition());
    } else {
        return 0; // optimism is just right for astar
    }

}


bool
ROEdge::hasLoadedTravelTime(double time) const {
    return myUsingTTTimeLine && myTravelTimes.describesTime(time);
}


double
ROEdge::getTravelTime(const ROVehicle* const veh, double time) const {
    if (myUsingTTTimeLine) {
        if (myTravelTimes.describesTime(time)) {
            double lineTT = myTravelTimes.getValue(time);
            if (myInterpolate) {
                const double inTT = lineTT;
                const double split = (double)(myTravelTimes.getSplitTime(time, time + inTT) - time);
                if (split >= 0) {
                    lineTT = myTravelTimes.getValue(time + inTT) * ((double)1. - split / inTT) + split;
                }
            }
            return MAX2(getMinimumTravelTime(veh), lineTT);
        } else {
            if (!myHaveTTWarned) {
                WRITE_WARNING("No interval matches passed time " + toString(time)  + " in edge '" + myID + "'.\n Using edge's length / max speed.");
                myHaveTTWarned = true;
            }
        }
    }
    return myLength / MIN2(veh->getType()->maxSpeed, veh->getType()->speedFactor.getParameter()[0] * mySpeed);
}


double
ROEdge::getNoiseEffort(const ROEdge* const edge, const ROVehicle* const veh, double time) {
    double ret = 0;
    if (!edge->getStoredEffort(time, ret)) {
        const double v = MIN2(veh->getType()->maxSpeed, edge->mySpeed);
        ret = HelpersHarmonoise::computeNoise(veh->getType()->emissionClass, v, 0);
    }
    return ret;
}


bool
ROEdge::getStoredEffort(double time, double& ret) const {
    if (myUsingETimeLine) {
        if (!myEfforts.describesTime(time)) {
            if (!myHaveEWarned) {
                WRITE_WARNING("No interval matches passed time " + toString(time)  + " in edge '" + myID + "'.\n Using edge's length / edge's speed.");
                myHaveEWarned = true;
            }
            return false;
        }
        if (myInterpolate) {
            double inTT = myTravelTimes.getValue(time);
            double ratio = (double)(myEfforts.getSplitTime(time, time + (SUMOTime)inTT) - time) / inTT;
            if (ratio >= 0) {
                ret = ratio * myEfforts.getValue(time) + (1 - ratio) * myEfforts.getValue(time + (SUMOTime)inTT);
                return true;
            }
        }
        ret = myEfforts.getValue(time);
        return true;
    }
    return false;
}


int
ROEdge::getNumSuccessors() const {
    if (getFunc() == ET_SINK) {
        return 0;
    }
    return (int) myFollowingEdges.size();
}


int
ROEdge::getNumPredecessors() const {
    if (getFunc() == ET_SOURCE) {
        return 0;
    }
    return (int) myApproachingEdges.size();
}


void
ROEdge::buildTimeLines(const std::string& measure, const bool boundariesOverride) {
    if (myUsingETimeLine) {
        double value = myLength / mySpeed;
        const SUMOEmissionClass c = PollutantsInterface::getClassByName("unknown");
        if (measure == "CO") {
            value = PollutantsInterface::compute(c, PollutantsInterface::CO, mySpeed, 0, 0) * value; // @todo: give correct slope
        }
        if (measure == "CO2") {
            value = PollutantsInterface::compute(c, PollutantsInterface::CO2, mySpeed, 0, 0) * value; // @todo: give correct slope
        }
        if (measure == "HC") {
            value = PollutantsInterface::compute(c, PollutantsInterface::HC, mySpeed, 0, 0) * value; // @todo: give correct slope
        }
        if (measure == "PMx") {
            value = PollutantsInterface::compute(c, PollutantsInterface::PM_X, mySpeed, 0, 0) * value; // @todo: give correct slope
        }
        if (measure == "NOx") {
            value = PollutantsInterface::compute(c, PollutantsInterface::NO_X, mySpeed, 0, 0) * value; // @todo: give correct slope
        }
        if (measure == "fuel") {
            value = PollutantsInterface::compute(c, PollutantsInterface::FUEL, mySpeed, 0, 0) * value; // @todo: give correct slope
        }
        if (measure == "electricity") {
            value = PollutantsInterface::compute(c, PollutantsInterface::ELEC, mySpeed, 0, 0) * value; // @todo: give correct slope
        }
        myEfforts.fillGaps(value, boundariesOverride);
    }
    if (myUsingTTTimeLine) {
        myTravelTimes.fillGaps(myLength / mySpeed, boundariesOverride);
    }
}


bool
ROEdge::allFollowersProhibit(const ROVehicle* const vehicle) const {
    for (ROEdgeVector::const_iterator i = myFollowingEdges.begin(); i != myFollowingEdges.end(); ++i) {
        if (!(*i)->prohibits(vehicle)) {
            return false;
        }
    }
    return true;
}


const ROEdgeVector&
ROEdge::getAllEdges() {
    return myEdges;
}


const ROEdgeVector&
ROEdge::getSuccessors(SUMOVehicleClass vClass) const {
    if (vClass == SVC_IGNORING || !RONet::getInstance()->hasPermissions() || myFunc == ET_DISTRICT) {
        return myFollowingEdges;
    }
#ifdef HAVE_FOX
    FXMutexLock locker(myLock);
#endif
    std::map<SUMOVehicleClass, ROEdgeVector>::const_iterator i = myClassesSuccessorMap.find(vClass);
    if (i != myClassesSuccessorMap.end()) {
        // can use cached value
        return i->second;
    } else {
        // this vClass is requested for the first time. rebuild all successors
        std::set<ROEdge*> followers;
        for (std::vector<ROLane*>::const_iterator it = myLanes.begin(); it != myLanes.end(); ++it) {
            ROLane* lane = *it;
            if ((lane->getPermissions() & vClass) != 0) {
                const std::vector<const ROLane*>& outgoing = lane->getOutgoingLanes();
                for (std::vector<const ROLane*>::const_iterator it2 = outgoing.begin(); it2 != outgoing.end(); ++it2) {
                    const ROLane* next = *it2;
                    if ((next->getPermissions() & vClass) != 0) {
                        followers.insert(&next->getEdge());
                    }
                }
            }
        }
        // also add district edges (they are not connected at the lane level
        for (ROEdgeVector::const_iterator it = myFollowingEdges.begin(); it != myFollowingEdges.end(); ++it) {
            if ((*it)->getFunc() == ET_DISTRICT) {
                followers.insert(*it);
            }
        }
        myClassesSuccessorMap[vClass].insert(myClassesSuccessorMap[vClass].begin(),
                                             followers.begin(), followers.end());
        return myClassesSuccessorMap[vClass];
    }

}


bool
ROEdge::isConnectedTo(const ROEdge* const e, const ROVehicle* const vehicle) const {
    const SUMOVehicleClass vClass = (vehicle == 0 ? SVC_IGNORING : vehicle->getVClass());
    const ROEdgeVector& followers = getSuccessors(vClass);
    return std::find(followers.begin(), followers.end(), e) != followers.end();
}

/****************************************************************************/

