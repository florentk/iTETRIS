/****************************************************************************/
/// @file    MSAbstractLaneChangeModel.h
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 29.04.2005
/// @version $Id: MSAbstractLaneChangeModel.cpp 18842 2015-09-17 10:43:51Z behrisch $
///
// Interface for lane-change models
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/options/OptionsCont.h>
#include "MSAbstractLaneChangeModel.h"
#include "MSLCM_DK2008.h"
#include "MSLCM_LC2013.h"
#include "MSLCM_JE2013.h"
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>

/* -------------------------------------------------------------------------
 * static members
 * ----------------------------------------------------------------------- */
bool MSAbstractLaneChangeModel::myAllowOvertakingRight(false);

/* -------------------------------------------------------------------------
 * MSAbstractLaneChangeModel-methods
 * ----------------------------------------------------------------------- */

void
MSAbstractLaneChangeModel::initGlobalOptions(const OptionsCont& oc) {
    myAllowOvertakingRight = oc.getBool("lanechange.overtake-right");
}


MSAbstractLaneChangeModel*
MSAbstractLaneChangeModel::build(LaneChangeModel lcm, MSVehicle& v) {
    switch (lcm) {
        case LCM_DK2008:
            return new MSLCM_DK2008(v);
        case LCM_LC2013:
            return new MSLCM_LC2013(v);
        case LCM_JE2013:
            return new MSLCM_JE2013(v);
        default:
            throw ProcessError("Lane change model '" + toString(lcm) + "' not implemented");
    }
}


MSAbstractLaneChangeModel::MSAbstractLaneChangeModel(MSVehicle& v) :
    myVehicle(v),
    myOwnState(0),
    myLaneChangeCompletion(1.0),
    myLaneChangeDirection(0),
    myLaneChangeMidpointPassed(false),
    myAlreadyMoved(false),
    myShadowLane(0),
    myHaveShadow(false),
    myCarFollowModel(v.getCarFollowModel()),
    myLastLaneChangeOffset(0) {
}


MSAbstractLaneChangeModel::~MSAbstractLaneChangeModel() {
    if (myShadowLane != 0 && myHaveShadow) {
        myShadowLane->removeVehicle(&myVehicle, MSMoveReminder::NOTIFICATION_VAPORIZED, false);
    }
}


bool
MSAbstractLaneChangeModel::congested(const MSVehicle* const neighLeader) {
    if (neighLeader == 0) {
        return false;
    }
    // Congested situation are relevant only on highways (maxSpeed > 70km/h)
    // and congested on German Highways means that the vehicles have speeds
    // below 60km/h. Overtaking on the right is allowed then.
    if ((myVehicle.getLane()->getSpeedLimit() <= 70.0 / 3.6) || (neighLeader->getLane()->getSpeedLimit() <= 70.0 / 3.6)) {

        return false;
    }
    if (myVehicle.congested() && neighLeader->congested()) {
        return true;
    }
    return false;
}


bool
MSAbstractLaneChangeModel::predInteraction(const MSVehicle* const leader) {
    if (leader == 0) {
        return false;
    }
    // let's check it on highways only
    if (leader->getSpeed() < (80.0 / 3.6)) {
        return false;
    }
    SUMOReal gap = leader->getPositionOnLane() - leader->getVehicleType().getLength() - myVehicle.getVehicleType().getMinGap() - myVehicle.getPositionOnLane();
    return gap < myCarFollowModel.interactionGap(&myVehicle, leader->getSpeed());
}


bool
MSAbstractLaneChangeModel::startLaneChangeManeuver(MSLane* source, MSLane* target, int direction) {
    target->enteredByLaneChange(&myVehicle);
    if (MSGlobals::gLaneChangeDuration > DELTA_T) {
        // initialize further lanes for shadow vehicle
        // (must come before myLaneChangeCompletion is initialized)
        const std::vector<MSLane*>& further = myVehicle.getFurtherLanes();
        SUMOReal leftLength = myVehicle.getVehicleType().getLength() - myVehicle.getPositionOnLane();
        MSLane* lane = target;
        for (int i = 0; i < (int)further.size(); i++) {
            lane = lane->getLogicalPredecessorLane(further[i]->getEdge());
            if (lane != 0) {
                leftLength -= lane->setPartialOccupation(&myVehicle, leftLength);
                setShadowPartialOccupator(lane);
            } else {
                break;
            }
        }
        myLaneChangeCompletion = 0;
        myShadowLane = target;
        myHaveShadow = true;
        myLaneChangeMidpointPassed = false;
        myLaneChangeDirection = direction;
        continueLaneChangeManeuver(false);
        myVehicle.switchOffSignal(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT | MSVehicle::VEH_SIGNAL_BLINKER_LEFT);
        myVehicle.switchOnSignal(direction == 1 ? MSVehicle::VEH_SIGNAL_BLINKER_LEFT : MSVehicle::VEH_SIGNAL_BLINKER_RIGHT);
        return true;
    } else {
        myVehicle.leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
        source->leftByLaneChange(&myVehicle);
        myVehicle.enterLaneAtLaneChange(target);
        changed(direction);
        return false;
    }
}


MSLane*
MSAbstractLaneChangeModel::getShadowLane(const MSLane* lane) const {
    if (std::find(myNoPartiallyOccupatedByShadow.begin(), myNoPartiallyOccupatedByShadow.end(), lane) == myNoPartiallyOccupatedByShadow.end()) {
        const int shadowDirection = myLaneChangeMidpointPassed ? -myLaneChangeDirection : myLaneChangeDirection;
        return lane->getParallelLane(shadowDirection);
    } else {
        return 0;
    }
}


void
MSAbstractLaneChangeModel::continueLaneChangeManeuver(bool moved) {
    if (moved && myHaveShadow) {
        // move shadow to next lane
        removeLaneChangeShadow(MSMoveReminder::NOTIFICATION_JUNCTION, false);
        myShadowLane = getShadowLane(myVehicle.getLane());
        if (myShadowLane == 0) {
            // abort lane change
            WRITE_WARNING("Vehicle '" + myVehicle.getID() + "' could not finish continuous lane change (lane disappeared) time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            endLaneChangeManeuver();
            return;
        }
        myHaveShadow = true;
    }
    myLaneChangeCompletion += (SUMOReal)DELTA_T / (SUMOReal)MSGlobals::gLaneChangeDuration;
    if (!myLaneChangeMidpointPassed && myLaneChangeCompletion >=
            myVehicle.getLane()->getWidth() / (myVehicle.getLane()->getWidth() + myShadowLane->getWidth())) {
        // maneuver midpoint reached, swap myLane and myShadowLane
        myLaneChangeMidpointPassed = true;
        MSLane* tmp = myVehicle.getLane();
        // removing partial occupator shadows - will be rebuilt in enterLaneAtLaneChange
        for (std::vector<MSLane*>::const_iterator it = myPartiallyOccupatedByShadow.begin(); it != myPartiallyOccupatedByShadow.end(); ++it) {
            (*it)->resetPartialOccupation(&myVehicle);
        }
        myPartiallyOccupatedByShadow.clear();
        myVehicle.leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
        myVehicle.enterLaneAtLaneChange(myShadowLane);
        myShadowLane = tmp;
        if (myVehicle.fixPosition()) {
            WRITE_WARNING("Vehicle '" + myVehicle.getID() + "' set back by " + toString(myVehicle.getPositionOnLane() - myVehicle.getLane()->getLength()) +
                          "m when changing lanes on lane '" + myVehicle.getLane()->getID() + "' time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
        }
        changed(myLaneChangeDirection);
        myAlreadyMoved = true;
    }
    // remove shadow as soon as the vehicle leaves the original lane geometrically
    if (myLaneChangeMidpointPassed && myHaveShadow) {
        const SUMOReal sourceHalfWidth = myShadowLane->getWidth() / 2.0;
        const SUMOReal targetHalfWidth = myVehicle.getLane()->getWidth() / 2.0;
        if (myLaneChangeCompletion * (sourceHalfWidth + targetHalfWidth) - myVehicle.getVehicleType().getWidth() / 2.0 > sourceHalfWidth) {
            removeLaneChangeShadow(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
        }
    }
    // finish maneuver
    if (!isChangingLanes()) {
        assert(myLaneChangeMidpointPassed);
        endLaneChangeManeuver();
    }
}


void
MSAbstractLaneChangeModel::removeLaneChangeShadow(const MSMoveReminder::Notification reason, bool notify) {
    if (myShadowLane != 0 && myHaveShadow) {
        myShadowLane->removeVehicle(&myVehicle, reason, notify);
        myHaveShadow = false;
        // maintain pointer to myShadowLane for interpolating position
    }
}

void
MSAbstractLaneChangeModel::endLaneChangeManeuver(const MSMoveReminder::Notification reason) {
    removeLaneChangeShadow(reason);
    myLaneChangeCompletion = 1;
    myShadowLane = 0;
    // removing partial occupator shadows
    for (std::vector<MSLane*>::const_iterator it = myPartiallyOccupatedByShadow.begin(); it != myPartiallyOccupatedByShadow.end(); ++it) {
        (*it)->resetPartialOccupation(&myVehicle);
    }
    myPartiallyOccupatedByShadow.clear();
    myNoPartiallyOccupatedByShadow.clear();
}

bool
MSAbstractLaneChangeModel::cancelRequest(int state) {
    int ret = myVehicle.influenceChangeDecision(state);
    return ret != state;
}


void
MSAbstractLaneChangeModel::initLastLaneChangeOffset(int dir) {
    if (dir > 0) {
        myLastLaneChangeOffset = 1;
    } else if (dir < 0) {
        myLastLaneChangeOffset = -1;
    }
}
