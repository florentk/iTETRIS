/****************************************************************************/
/// @file    MSLane.cpp
/// @author  Christian Roessel
/// @author  Jakob Erdmann
/// @author  Daniel Krajzewicz
/// @author  Tino Morenz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Christoph Sommer
/// @author  Mario Krumnow
/// @date    Mon, 05 Mar 2001
/// @version $Id: MSLane.cpp 19535 2015-12-05 13:47:18Z behrisch $
///
// Representation of a lane in the micro simulation
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

#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "MSVehicle.h"
#include <microsim/pedestrians/MSPModel.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include "MSNet.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSEdgeControl.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSLink.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include "MSVehicleControl.h"
#include "MSInsertionControl.h"
#include "MSVehicleControl.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iterator>
#include <exception>
#include <climits>
#include <set>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <utils/geom/GeomHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
MSLane::DictType MSLane::myDict;


// ===========================================================================
// member method definitions
// ===========================================================================
MSLane::MSLane(const std::string& id, SUMOReal maxSpeed, SUMOReal length, MSEdge* const edge,
               unsigned int numericalID, const PositionVector& shape, SUMOReal width,
               SVCPermissions permissions) :
    Named(id),
    myShape(shape), myNumericalID(numericalID),
    myVehicles(), myLength(length), myWidth(width), myEdge(edge), myMaxSpeed(maxSpeed),
    myPermissions(permissions),
    myLogicalPredecessorLane(0),
    myBruttoVehicleLengthSum(0), myNettoVehicleLengthSum(0), myInlappingVehicleEnd(10000), myInlappingVehicle(0),
    myLengthGeometryFactor(MAX2(POSITION_EPS, myShape.length()) / myLength) { // factor should not be 0
    myRestrictions = MSNet::getInstance()->getRestrictions(edge->getEdgeType());
}


MSLane::~MSLane() {
    for (MSLinkCont::iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        delete *i;
    }
}


void
MSLane::addLink(MSLink* link) {
    myLinks.push_back(link);
}


// ------ interaction with MSMoveReminder ------
void
MSLane::addMoveReminder(MSMoveReminder* rem) {
    myMoveReminders.push_back(rem);
    for (VehCont::iterator veh = myVehicles.begin(); veh != myVehicles.end(); ++veh) {
        (*veh)->addReminder(rem);
    }
}



// ------ Vehicle emission ------
void
MSLane::incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed, const MSLane::VehCont::iterator& at, MSMoveReminder::Notification notification) {
    assert(pos <= myLength);
    bool wasInactive = myVehicles.size() == 0;
    veh->enterLaneAtInsertion(this, pos, speed, notification);
    if (at == myVehicles.end()) {
        // vehicle will be the first on the lane
        myVehicles.push_back(veh);
    } else {
        myVehicles.insert(at, veh);
    }
    myBruttoVehicleLengthSum += veh->getVehicleType().getLengthWithGap();
    myNettoVehicleLengthSum += veh->getVehicleType().getLength();
    myEdge->markDelayed();
    if (wasInactive) {
        MSNet::getInstance()->getEdgeControl().gotActive(this);
    }
}


bool
MSLane::pWagGenericInsertion(MSVehicle& veh, SUMOReal mspeed, SUMOReal maxPos, SUMOReal minPos) {
    veh.setTentativeLaneAndPosition(this, maxPos);
    veh.updateBestLanes(false, this);
    SUMOReal xIn = maxPos;
    SUMOReal vIn = mspeed;
    SUMOReal leaderDecel;
    if (myVehicles.size() != 0) {
        MSVehicle* leader = myVehicles.front();
        xIn = leader->getPositionOnLane() - leader->getVehicleType().getLength() - veh.getVehicleType().getMinGap();
        vIn = leader->getSpeed();
        leaderDecel = leader->getCarFollowModel().getMaxDecel();
    } else {
        SUMOReal brakeGap = veh.getCarFollowModel().brakeGap(mspeed);
        std::pair<MSVehicle* const, SUMOReal> leader = getLeaderOnConsecutive(brakeGap, 0, mspeed, veh, veh.getBestLanesContinuation(this));
        if (leader.first != 0) {
            xIn = getLength() + leader.second;
            vIn = leader.first->getSpeed();
            leaderDecel = leader.first->getCarFollowModel().getMaxDecel();
        } else {
            incorporateVehicle(&veh, maxPos, mspeed, myVehicles.end());
            return true;
        }
    }
    const SUMOReal vHlp = 0.5 * (vIn + mspeed);
    SUMOReal x2 = xIn;// have seen leader length already - skCar::lCar;
    SUMOReal x1 = x2 - 100.0;
    SUMOReal x = 0;
    for (int i = 0; i <= 10; i++) {
        x = 0.5 * (x1 + x2);
        veh.setTentativeLaneAndPosition(this, x);
        SUMOReal vSafe = veh.getCarFollowModel().insertionFollowSpeed(&veh, vHlp, xIn - x, vIn, leaderDecel);
        if (vSafe < vHlp) {
            x2 = x;
        } else {
            x1 = x;
        }
    }
    if (x < minPos) {
        return false;
    } else if (x > maxPos) {
        x = maxPos;
    }
    incorporateVehicle(&veh, x, vHlp, myVehicles.begin());
    return true;
}


bool
MSLane::pWagSimpleInsertion(MSVehicle& veh, SUMOReal mspeed, SUMOReal maxPos, SUMOReal minPos) {
    veh.setTentativeLaneAndPosition(this, maxPos);
    veh.updateBestLanes(false, this);
    SUMOReal xIn = maxPos;
    SUMOReal vIn = mspeed;
    if (myVehicles.size() != 0) {
        MSVehicle* leader = myVehicles.front();
        xIn = leader->getPositionOnLane() - leader->getVehicleType().getLength() - veh.getVehicleType().getMinGap();
        vIn = leader->getSpeed();
    } else {
        SUMOReal brakeGap = veh.getCarFollowModel().brakeGap(mspeed);
        std::pair<MSVehicle* const, SUMOReal> leader = getLeaderOnConsecutive(brakeGap, 0, mspeed, veh, veh.getBestLanesContinuation(this));
        if (leader.first != 0) {
            xIn = getLength() + leader.second;
            vIn = leader.first->getSpeed();
        } else {
            incorporateVehicle(&veh, maxPos, mspeed, myVehicles.end());
            return true;
        }
    }
    const SUMOReal vHlp = 0.5 * (mspeed + vIn);
    xIn = xIn - vHlp * veh.getCarFollowModel().getHeadwayTime() - veh.getVehicleType().getMinGap();
    if (xIn < minPos) {
        return false;
    } else if (xIn > maxPos) {
        xIn = maxPos;
    }
    incorporateVehicle(&veh, xIn, vHlp, myVehicles.begin());
    return true;
}


bool
MSLane::maxSpeedGapInsertion(MSVehicle& veh, SUMOReal mspeed) {
    if (myVehicles.size() == 0) {
        return isInsertionSuccess(&veh, mspeed, myLength / 2, true, MSMoveReminder::NOTIFICATION_DEPARTED);
    }
    // go through the lane, look for free positions (starting after the last vehicle)
    MSLane::VehCont::iterator predIt = myVehicles.begin();
    SUMOReal maxSpeed = 0;
    SUMOReal maxPos = 0;
    MSLane::VehCont::iterator maxIt = myVehicles.begin();
    while (predIt != myVehicles.end()) {
        // get leader (may be zero) and follower
        const MSVehicle* leader = predIt != myVehicles.end() - 1 ? *(predIt + 1) : getPartialOccupator();
        const MSVehicle* follower = *predIt;
        SUMOReal leaderRearPos = getLength();
        SUMOReal leaderSpeed = mspeed;
        if (leader != 0) {
            leaderRearPos = leader->getPositionOnLane() - leader->getVehicleType().getLength();
            if (leader == getPartialOccupator()) {
                leaderRearPos = getPartialOccupatorEnd();
            }
            leaderSpeed = leader->getSpeed();
        }
        const SUMOReal nettoGap = leaderRearPos - follower->getPositionOnLane() - veh.getVehicleType().getLengthWithGap();
        if (nettoGap > 0) {
            const SUMOReal tau = veh.getCarFollowModel().getHeadwayTime();
            const SUMOReal tauDecel = tau * veh.getCarFollowModel().getMaxDecel();
            const SUMOReal fSpeed = follower->getSpeed();
            const SUMOReal lhs = nettoGap / tau + tauDecel - fSpeed - fSpeed * fSpeed / (2 * tauDecel) + leaderSpeed * leaderSpeed / (2 * tauDecel);
            if (lhs >= sqrt(tauDecel * tauDecel + leaderSpeed * leaderSpeed)) {
                const SUMOReal frontGap = (lhs * lhs - tauDecel * tauDecel - leaderSpeed * leaderSpeed) / (2 * veh.getCarFollowModel().getMaxDecel());
                const SUMOReal currentMaxSpeed = lhs - tauDecel;
                if (MIN2(currentMaxSpeed, mspeed) > maxSpeed) {
                    maxSpeed = currentMaxSpeed;
                    maxPos = MIN2(leaderRearPos + frontGap, myLength);
                    maxIt = predIt + 1;
                }
            }
        }
        ++predIt;
    }
    if (maxSpeed > 0) {
        incorporateVehicle(&veh, maxPos, maxSpeed, maxIt);
        return true;
    }
    return false;
}


bool
MSLane::freeInsertion(MSVehicle& veh, SUMOReal mspeed,
                      MSMoveReminder::Notification notification) {
    bool adaptableSpeed = true;
    // try to insert teleporting vehicles fully on this lane
    const SUMOReal minPos = (notification == MSMoveReminder::NOTIFICATION_TELEPORT ?
                             MIN2(myLength, veh.getVehicleType().getLength()) : 0);

    if (myVehicles.size() == 0) {
        // ensure sufficient gap to followers on predecessor lanes
        const SUMOReal backOffset = minPos - veh.getVehicleType().getLength();
        const SUMOReal missingRearGap = getMissingRearGap(backOffset, mspeed, veh.getCarFollowModel().getMaxDecel());
        if (missingRearGap > 0) {
            if (minPos + missingRearGap <= myLength) {
                // @note. The rear gap is tailored to mspeed. If it changes due
                // to a leader vehicle (on subsequent lanes) insertion will
                // still fail. Under the right combination of acceleration and
                // deceleration values there might be another insertion
                // positions that would be successful be we do not look for it.
                return isInsertionSuccess(&veh, mspeed, minPos + missingRearGap, adaptableSpeed, notification);
            } else {
                return false;
            }
        } else {
            return isInsertionSuccess(&veh, mspeed, minPos, adaptableSpeed, notification);
        }

    } else {
        // check whether the vehicle can be put behind the last one if there is such
        MSVehicle* leader = myVehicles.back();
        const SUMOReal leaderPos = leader->getPositionOnLane() - leader->getVehicleType().getLength();
        const SUMOReal speed = adaptableSpeed ? leader->getSpeed() : mspeed;
        const SUMOReal frontGapNeeded = veh.getCarFollowModel().getSecureGap(speed, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()) + veh.getVehicleType().getMinGap();
        if (leaderPos >= frontGapNeeded) {
            const SUMOReal tspeed = MIN2(veh.getCarFollowModel().insertionFollowSpeed(&veh, mspeed, frontGapNeeded, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()), mspeed);
            // check whether we can insert our vehicle behind the last vehicle on the lane
            if (isInsertionSuccess(&veh, tspeed, minPos, adaptableSpeed, notification)) {
                return true;
            }
        }
    }
    // go through the lane, look for free positions (starting after the last vehicle)
    MSLane::VehCont::iterator predIt = myVehicles.begin();
    while (predIt != myVehicles.end()) {
        // get leader (may be zero) and follower
        const MSVehicle* leader = predIt != myVehicles.end() - 1 ? *(predIt + 1) : getPartialOccupator();
        const MSVehicle* follower = *predIt;

        // patch speed if allowed
        SUMOReal speed = mspeed;
        if (adaptableSpeed && leader != 0) {
            speed = MIN2(leader->getSpeed(), mspeed);
        }

        // compute the space needed to not collide with leader
        SUMOReal frontMax = getLength();
        if (leader != 0) {
            SUMOReal leaderRearPos = leader->getPositionOnLane() - leader->getVehicleType().getLength();
            if (leader == getPartialOccupator()) {
                leaderRearPos = getPartialOccupatorEnd();
            }
            SUMOReal frontGapNeeded = veh.getCarFollowModel().getSecureGap(speed, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()) + veh.getVehicleType().getMinGap();
            frontMax = leaderRearPos - frontGapNeeded;
        }
        // compute the space needed to not let the follower collide
        const SUMOReal followPos = follower->getPositionOnLane() + follower->getVehicleType().getMinGap();
        const SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), veh.getSpeed(), veh.getCarFollowModel().getMaxDecel());
        const SUMOReal backMin = followPos + backGapNeeded + veh.getVehicleType().getLength();

        // check whether there is enough room (given some extra space for rounding errors)
        if (frontMax > minPos && backMin + POSITION_EPS < frontMax) {
            // try to insert vehicle (should be always ok)
            if (isInsertionSuccess(&veh, speed, backMin + POSITION_EPS, adaptableSpeed, notification)) {
                return true;
            }
        }
        ++predIt;
    }
    // first check at lane's begin
    return false;
}


SUMOReal
MSLane::getDepartSpeed(const MSVehicle& veh, bool& patchSpeed) {
    SUMOReal speed = 0;
    const SUMOVehicleParameter& pars = veh.getParameter();
    switch (pars.departSpeedProcedure) {
        case DEPART_SPEED_GIVEN:
            speed = pars.departSpeed;
            patchSpeed = false;
            break;
        case DEPART_SPEED_RANDOM:
            speed = RandHelper::rand(getVehicleMaxSpeed(&veh));
            patchSpeed = true; // @todo check
            break;
        case DEPART_SPEED_MAX:
            speed = getVehicleMaxSpeed(&veh);
            patchSpeed = true; // @todo check
            break;
        case DEPART_SPEED_DEFAULT:
        default:
            // speed = 0 was set before
            patchSpeed = false; // @todo check
            break;
    }
    return speed;
}


bool
MSLane::insertVehicle(MSVehicle& veh) {
    SUMOReal pos = 0;
    bool patchSpeed = true; // whether the speed shall be adapted to infrastructure/traffic in front
    const SUMOVehicleParameter& pars = veh.getParameter();
    SUMOReal speed = getDepartSpeed(veh, patchSpeed);

    // determine the position
    switch (pars.departPosProcedure) {
        case DEPART_POS_GIVEN:
            pos = pars.departPos;
            if (pos < 0.) {
                pos += myLength;
            }
            break;
        case DEPART_POS_RANDOM:
            pos = RandHelper::rand(getLength());
            break;
        case DEPART_POS_RANDOM_FREE: {
            for (unsigned int i = 0; i < 10; i++) {
                // we will try some random positions ...
                pos = RandHelper::rand(getLength());
                if (isInsertionSuccess(&veh, speed, pos, patchSpeed, MSMoveReminder::NOTIFICATION_DEPARTED)) {
                    return true;
                }
            }
            // ... and if that doesn't work, we put the vehicle to the free position
            return freeInsertion(veh, speed);
        }
        break;
        case DEPART_POS_FREE:
            return freeInsertion(veh, speed);
        case DEPART_POS_PWAG_SIMPLE:
            return pWagSimpleInsertion(veh, speed, getLength(), 0.0);
        case DEPART_POS_PWAG_GENERIC:
            return pWagGenericInsertion(veh, speed, getLength(), 0.0);
        case DEPART_POS_MAX_SPEED_GAP:
            return maxSpeedGapInsertion(veh, speed);
        case DEPART_POS_BASE:
        case DEPART_POS_DEFAULT:
        default:
            pos = MIN2(static_cast<SUMOReal>(veh.getVehicleType().getLength() + POSITION_EPS), myLength);
            break;
    }
    // try to insert
    return isInsertionSuccess(&veh, speed, pos, patchSpeed, MSMoveReminder::NOTIFICATION_DEPARTED);
}


bool
MSLane::checkFailure(MSVehicle* aVehicle, SUMOReal& speed, SUMOReal& dist, const SUMOReal nspeed, const bool patchSpeed, const std::string errorMsg) const {
    if (nspeed < speed) {
        if (patchSpeed) {
            speed = MIN2(nspeed, speed);
            dist = aVehicle->getCarFollowModel().brakeGap(speed) + aVehicle->getVehicleType().getMinGap();
        } else {
            if (errorMsg != "") {
                WRITE_ERROR("Vehicle '" + aVehicle->getID() + "' will not be able to depart using the given velocity (" + errorMsg + ")!");
                MSNet::getInstance()->getInsertionControl().descheduleDeparture(aVehicle);
            }
            return true;
        }
    }
    return false;
}


bool
MSLane::isInsertionSuccess(MSVehicle* aVehicle,
                           SUMOReal speed, SUMOReal pos, bool patchSpeed,
                           MSMoveReminder::Notification notification) {
    if (pos < 0 || pos > myLength) {
        // we may not start there
        WRITE_WARNING("Invalid departPos " + toString(pos) + " given for vehicle '" +
                      aVehicle->getID() + "'. Inserting at lane end instead.");
        pos = myLength;
    }
    aVehicle->setTentativeLaneAndPosition(this, pos);
    aVehicle->updateBestLanes(false, this);
    const MSCFModel& cfModel = aVehicle->getCarFollowModel();
    const std::vector<MSLane*>& bestLaneConts = aVehicle->getBestLanesContinuation(this);
    std::vector<MSLane*>::const_iterator ri = bestLaneConts.begin();
    SUMOReal seen = getLength() - pos;
    SUMOReal dist = cfModel.brakeGap(speed) + aVehicle->getVehicleType().getMinGap();
    const MSRoute& r = aVehicle->getRoute();
    MSRouteIterator ce = r.begin();
    unsigned int nRouteSuccs = 1;
    MSLane* currentLane = this;
    MSLane* nextLane = this;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / MAX2(speed, SUMO_const_haltingSpeed));
    while (seen < dist && ri != bestLaneConts.end()) {
        // get the next link used...
        MSLinkCont::const_iterator link = succLinkSec(*aVehicle, nRouteSuccs, *currentLane, bestLaneConts);
        if (currentLane->isLinkEnd(link)) {
            if (&currentLane->getEdge() == r.getLastEdge()) {
                // reached the end of the route
                if (aVehicle->getParameter().arrivalSpeedProcedure == ARRIVAL_SPEED_GIVEN) {
                    if (checkFailure(aVehicle, speed, dist, cfModel.freeSpeed(aVehicle, speed, seen, aVehicle->getParameter().arrivalSpeed, true),
                                     patchSpeed, "arrival speed too low")) {
                        // we may not drive with the given velocity - we cannot match the specified arrival speed
                        return false;
                    }
                }
            } else {
                // lane does not continue
                if (checkFailure(aVehicle, speed, dist, cfModel.stopSpeed(aVehicle, speed, seen),
                                 patchSpeed, "junction too close")) {
                    // we may not drive with the given velocity - we cannot stop at the junction
                    return false;
                }
            }
            break;
        }
        if (!(*link)->opened(arrivalTime, speed, speed, aVehicle->getVehicleType().getLength(), aVehicle->getImpatience(), cfModel.getMaxDecel(), 0)
                || !(*link)->havePriority()) {
            // have to stop at junction
            std::string errorMsg = "";
            const LinkState state = (*link)->getState();
            if (state == LINKSTATE_MINOR
                    || state == LINKSTATE_EQUAL
                    || state == LINKSTATE_STOP
                    || state == LINKSTATE_ALLWAY_STOP) {
                // no sense in trying later
                errorMsg = "unpriorised junction too close";
            }
            if (checkFailure(aVehicle, speed, dist, cfModel.stopSpeed(aVehicle, speed, seen),
                             patchSpeed, errorMsg)) {
                // we may not drive with the given velocity - we cannot stop at the junction in time
                return false;
            }
            break;
        }
        // get the next used lane (including internal)
        nextLane = (*link)->getViaLaneOrLane();
        // check how next lane effects the journey
        if (nextLane != 0) {
            // check leader on next lane
            SUMOReal gap = 0;
            MSVehicle* leader = nextLane->getLastVehicle();
            if (leader != 0) {
                gap = seen + leader->getPositionOnLane() - leader->getVehicleType().getLength() -  aVehicle->getVehicleType().getMinGap();
            } else {
                leader = nextLane->getPartialOccupator();
                if (leader != 0) {
                    gap = seen + nextLane->getPartialOccupatorEnd() - aVehicle->getVehicleType().getMinGap();
                }
            }
            if (leader != 0) {
                if (gap < 0) {
                    return false;
                }
                const SUMOReal nspeed = cfModel.insertionFollowSpeed(aVehicle, speed, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
                if (checkFailure(aVehicle, speed, dist, nspeed, patchSpeed, "")) {
                    // we may not drive with the given velocity - we crash into the leader
                    return false;
                }
            }
            // check next lane's maximum velocity
            const SUMOReal nspeed = cfModel.freeSpeed(aVehicle, speed, seen, nextLane->getVehicleMaxSpeed(aVehicle), true);
            if (nspeed < speed) {
                if (patchSpeed) {
                    speed = nspeed;
                    dist = cfModel.brakeGap(speed) + aVehicle->getVehicleType().getMinGap();
                } else {
                    // we may not drive with the given velocity - we would be too fast on the next lane
                    WRITE_ERROR("Vehicle '" + aVehicle->getID() + "' will not be able to depart using the given velocity (slow lane ahead)!");
                    MSNet::getInstance()->getInsertionControl().descheduleDeparture(aVehicle);
                    return false;
                }
            }
            // check traffic on next junction
            // we cannot use (*link)->opened because a vehicle without priority
            // may already be comitted to blocking the link and unable to stop
            const SUMOTime leaveTime = (*link)->getLeaveTime(arrivalTime, speed, speed, aVehicle->getVehicleType().getLength());
            if ((*link)->hasApproachingFoe(arrivalTime, leaveTime, speed, cfModel.getMaxDecel())) {
                if (checkFailure(aVehicle, speed, dist, cfModel.stopSpeed(aVehicle, speed, seen), patchSpeed, "")) {
                    // we may not drive with the given velocity - we crash at the junction
                    return false;
                }
            }
            arrivalTime += TIME2STEPS(nextLane->getLength() / MAX2(speed, NUMERICAL_EPS));
            seen += nextLane->getLength();
            currentLane = nextLane;
#ifdef HAVE_INTERNAL_LANES
            if ((*link)->getViaLane() == 0) {
#else
            if (true) {
#endif
                nRouteSuccs++;
                ++ce;
                ++ri;
            }
        }
    }

    // get the pointer to the vehicle next in front of the given position
    MSVehicle* leader = 0;
    SUMOReal gap = 0;
    MSLane::VehCont::iterator predIt = find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), pos));
    if (predIt != myVehicles.end()) {
        leader = *predIt;
        gap = MSVehicle::gap(leader->getPositionOnLane(), leader->getVehicleType().getLength(), pos + aVehicle->getVehicleType().getMinGap());
    }
    if (leader == 0 && getPartialOccupator() != 0) {
        leader = getPartialOccupator();
        gap = getPartialOccupatorEnd() - pos - aVehicle->getVehicleType().getMinGap();
    }
    if (leader != 0) {
        if (gap < 0) {
            return false;
        }
        const SUMOReal nspeed = cfModel.insertionFollowSpeed(aVehicle, speed, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
        if (checkFailure(aVehicle, speed, dist, nspeed, patchSpeed, "")) {
            // we may not drive with the given velocity - we crash into the leader
            return false;
        }
    }

    // check back vehicle
    if (predIt != myVehicles.begin()) {
        // there is direct follower on this lane
        MSVehicle* follower = *(predIt - 1);
        SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), speed, cfModel.getMaxDecel());
        SUMOReal gap = MSVehicle::gap(pos, aVehicle->getVehicleType().getLength(), follower->getPositionOnLane() + follower->getVehicleType().getMinGap());
        if (gap < backGapNeeded) {
            // too close to the follower on this lane
            return false;
        }
    } else {
        // check approaching vehicles to prevent rear-end collisions
        const SUMOReal backOffset = pos - aVehicle->getVehicleType().getLength();
        const SUMOReal missingRearGap = getMissingRearGap(backOffset, speed, aVehicle->getCarFollowModel().getMaxDecel());
        if (missingRearGap > 0) {
            // too close to a follower
            return false;
        }
    }
    // may got negative while adaptation
    if (speed < 0) {
        return false;
    }
    // enter
    incorporateVehicle(aVehicle, pos, speed, predIt, notification);
    return true;
}


void
MSLane::forceVehicleInsertion(MSVehicle* veh, SUMOReal pos) {
    veh->updateBestLanes(true, this);
    bool dummy;
    const SUMOReal speed = veh->hasDeparted() ? veh->getSpeed() : getDepartSpeed(*veh, dummy);
    incorporateVehicle(veh, pos, speed, find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), pos)));
}


// ------ Handling vehicles lapping into lanes ------
SUMOReal
MSLane::setPartialOccupation(MSVehicle* v, SUMOReal leftVehicleLength) {
    myInlappingVehicle = v;
    myInlappingVehicleEnd = myLength - leftVehicleLength;
    if (v->getLaneChangeModel().isChangingLanes()) {
        MSLane* shadowLane = v->getLaneChangeModel().getShadowLane(this);
        if (shadowLane != 0) {
            v->getLaneChangeModel().setShadowPartialOccupator(shadowLane);
            shadowLane->myInlappingVehicle = v;
            shadowLane->myInlappingVehicleEnd = myLength - leftVehicleLength;
        }
    }
    return myLength;
}


void
MSLane::resetPartialOccupation(MSVehicle* v) {
    if (v == myInlappingVehicle) {
        myInlappingVehicleEnd = 10000;
        myInlappingVehicle = 0;
        if (v->getLaneChangeModel().isChangingLanes()) {
            MSLane* shadowLane = v->getLaneChangeModel().getShadowLane(this);
            if (shadowLane != 0 && v == shadowLane->myInlappingVehicle) {
                shadowLane->myInlappingVehicle = 0;
                shadowLane->myInlappingVehicleEnd = 10000;
            }
        }
    }
}


std::pair<MSVehicle*, SUMOReal>
MSLane::getLastVehicleInformation() const {
    if (myVehicles.size() != 0) {
        // the last vehicle is the one in scheduled by this lane
        MSVehicle* last = *myVehicles.begin();
        const SUMOReal pos = last->getPositionOnLane() - last->getVehicleType().getLength();
        return std::make_pair(last, pos);
    }
    if (myInlappingVehicle != 0) {
        // the last one is a vehicle extending into this lane
        return std::make_pair(myInlappingVehicle, myInlappingVehicleEnd);
    }
    return std::make_pair<MSVehicle*, SUMOReal>(0, 0);
}


// ------  ------
void
MSLane::planMovements(SUMOTime t) {
    assert(myVehicles.size() != 0);
    SUMOReal cumulatedVehLength = 0.;
    const MSVehicle* pred = getPartialOccupator();
    for (VehCont::reverse_iterator veh = myVehicles.rbegin(); veh != myVehicles.rend(); ++veh) {
        if ((*veh)->getLane() == this) {
            (*veh)->planMove(t, pred, cumulatedVehLength);
        }
        pred = *veh;
        cumulatedVehLength += pred->getVehicleType().getLengthWithGap();
    }
}


void
MSLane::detectCollisions(SUMOTime timestep, const std::string& stage) {
    if (myVehicles.size() == 0) {
        return;
    }

    VehCont::iterator lastVeh = myVehicles.end() - 1;
    for (VehCont::iterator veh = myVehicles.begin(); veh != lastVeh;) {
        VehCont::iterator pred = veh + 1;
        if ((*veh)->hasInfluencer() && (*veh)->getInfluencer().isVTDAffected(timestep)) {
            ++veh;
            continue;
        }
        if ((*pred)->hasInfluencer() && (*pred)->getInfluencer().isVTDAffected(timestep)) {
            ++veh;
            continue;
        }
        if (handleCollision(timestep, stage, *veh, *pred, (*pred)->getPositionOnLane() - (*pred)->getVehicleType().getLength())) {
            veh = myVehicles.erase(veh); // remove current vehicle
            lastVeh = myVehicles.end() - 1;
            if (veh == myVehicles.end()) {
                break;
            }
        } else {
            ++veh;
        }
    }
    MSVehicle* predV = getPartialOccupator();
    if (predV != 0) {
        if (handleCollision(timestep, stage, *lastVeh, predV, getPartialOccupatorEnd())) {
            myVehicles.erase(lastVeh);
        }
    }
}


bool
MSLane::handleCollision(SUMOTime timestep, const std::string& stage, MSVehicle* collider, MSVehicle* victim, const SUMOReal victimRear) {
    const SUMOReal gap = victimRear - collider->getPositionOnLane() - collider->getVehicleType().getMinGap();
    if (gap < -NUMERICAL_EPS) {
        if (collider->getLane() == this) {
            if (MSGlobals::gLaneChangeDuration > DELTA_T
                    && collider->getLaneChangeModel().isChangingLanes()
                    && victim->getLaneChangeModel().isChangingLanes()
                    && victim->getLane() != this) {
                // synchroneous lane change maneuver
                return false;
            }
            WRITE_WARNING("Teleporting vehicle '" + collider->getID() + "'; collision with '"
                          + victim->getID() + "', lane='" + getID() + "', gap=" + toString(gap)
                          + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + " stage=" + stage + ".");
            MSNet::getInstance()->getVehicleControl().registerCollision();
            myBruttoVehicleLengthSum -= collider->getVehicleType().getLengthWithGap();
            myNettoVehicleLengthSum -= collider->getVehicleType().getLength();
            MSVehicleTransfer::getInstance()->add(timestep, collider);
            return true;
        } else {
            //WRITE_WARNING("Shadow of vehicle '" + collider->getID() + "'; collision with '"
            //        + victim->getID() + "', lane='" + getID() + "', gap=" + toString(gap)
            //        + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + " stage=" + stage + ".");
            //collider->getLaneChangeModel().endLaneChangeManeuver(MSMoveReminder::NOTIFICATION_TELEPORT);
        }
    }
    return false;
}


bool
MSLane::executeMovements(SUMOTime t, std::vector<MSLane*>& into) {
    // iteratate over vehicles in reverse so that move reminders will be called in the correct order
    for (VehCont::reverse_iterator i = myVehicles.rbegin(); i != myVehicles.rend();) {
        MSVehicle* veh = *i;
        if (veh->getLane() != this || veh->getLaneChangeModel().alreadyMoved()) {
            // this is the shadow during a continuous lane change
            ++i;
            continue;
        }
        // length is needed later when the vehicle may not exist anymore
        const SUMOReal length = veh->getVehicleType().getLengthWithGap();
        const SUMOReal nettoLength = veh->getVehicleType().getLength();
        bool moved = veh->executeMove();
        MSLane* target = veh->getLane();
#ifndef NO_TRACI
        bool vtdControlled = veh->hasInfluencer() && veh->getInfluencer().isVTDControlled();
        if (veh->hasArrived() && !vtdControlled) {
#else
        if (veh->hasArrived()) {
#endif
            // vehicle has reached its arrival position
            veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_ARRIVED);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        } else if (target != 0 && moved) {
            if (target->getEdge().isVaporizing()) {
                // vehicle has reached a vaporizing edge
                veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_VAPORIZED);
                MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
            } else {
                // vehicle has entered a new lane (leaveLane was already called in MSVehicle::executeMove)
                target->myVehBuffer.push_back(veh);
                SUMOReal pspeed = veh->getSpeed();
                SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
                veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
                into.push_back(target);
                if (veh->getLaneChangeModel().hasShadowVehicle()) {
                    MSLane* shadowLane = veh->getLaneChangeModel().getShadowLane();
                    if (shadowLane != 0) {
                        into.push_back(shadowLane);
                        shadowLane->myVehBuffer.push_back(veh);
                    }
                }
            }
        } else if (veh->isParking()) {
            // vehicle started to park
            MSVehicleTransfer::getInstance()->add(t, veh);
        } else if (veh->getPositionOnLane() > getLength()) {
            // for any reasons the vehicle is beyond its lane...
            // this should never happen because it is handled in MSVehicle::executeMove
            assert(false);
            WRITE_WARNING("Teleporting vehicle '" + veh->getID() + "'; beyond end of lane, target lane='" + getID() + "', time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            MSNet::getInstance()->getVehicleControl().registerCollision();
            MSVehicleTransfer::getInstance()->add(t, veh);
        } else {
            ++i;
            continue;
        }
        myBruttoVehicleLengthSum -= length;
        myNettoVehicleLengthSum -= nettoLength;
        ++i;
        i = VehCont::reverse_iterator(myVehicles.erase(i.base()));
    }
    if (myVehicles.size() > 0) {
        if (MSGlobals::gTimeToGridlock > 0 || MSGlobals::gTimeToGridlockHighways > 0) {
            MSVehicle* veh = myVehicles.back(); // the vehice at the front of the queue
            if (!veh->isStopped() && veh->getLane() == this) {
                const bool wrongLane = !veh->getLane()->appropriate(veh);
                const bool r1 = MSGlobals::gTimeToGridlock > 0 && veh->getWaitingTime() > MSGlobals::gTimeToGridlock;
                const bool r2 = MSGlobals::gTimeToGridlockHighways > 0 && veh->getWaitingTime() > MSGlobals::gTimeToGridlockHighways && veh->getLane()->getSpeedLimit() > 69. / 3.6 && wrongLane;
                if (r1 || r2) {
                    const MSLinkCont::const_iterator link = succLinkSec(*veh, 1, *this, veh->getBestLanesContinuation());
                    const bool minorLink = !wrongLane && (link != myLinks.end()) && !((*link)->havePriority());
                    const std::string reason = (wrongLane ? " (wrong lane)" : (minorLink ? " (yield)" : " (jam)"));
                    MSVehicle* veh = *(myVehicles.end() - 1);
                    myBruttoVehicleLengthSum -= veh->getVehicleType().getLengthWithGap();
                    myNettoVehicleLengthSum -= veh->getVehicleType().getLength();
                    myVehicles.erase(myVehicles.end() - 1);
                    WRITE_WARNING("Teleporting vehicle '" + veh->getID() + "'; waited too long"
                                  + reason
                                  + (r2 ? " (highway)" : "")
                                  + ", lane='" + getID() + "', time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                    if (wrongLane) {
                        MSNet::getInstance()->getVehicleControl().registerTeleportWrongLane();
                    } else if (minorLink) {
                        MSNet::getInstance()->getVehicleControl().registerTeleportYield();
                    } else {
                        MSNet::getInstance()->getVehicleControl().registerTeleportJam();
                    }
                    MSVehicleTransfer::getInstance()->add(t, veh);
                }
            } // else look for a vehicle that isn't stopped?
        }
    }
    return myVehicles.size() == 0;
}


const MSEdge*
MSLane::getInternalFollower() const {
    const MSEdge* e = myEdge;
    while (e->getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        e = e->getSuccessors()[0];
    }
    return e;
}


// ------ Static (sic!) container methods  ------
bool
MSLane::dictionary(const std::string& id, MSLane* ptr) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSLane*
MSLane::dictionary(const std::string& id) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSLane::clear() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        delete(*i).second;
    }
    myDict.clear();
}


void
MSLane::insertIDs(std::vector<std::string>& into) {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


template<class RTREE> void
MSLane::fill(RTREE& into) {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        MSLane* l = (*i).second;
        Boundary b = l->getShape().getBoxBoundary();
        b.grow(3.);
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        into.Insert(cmin, cmax, l);
    }
}

template void MSLane::fill<NamedRTree>(NamedRTree& into);
#ifndef NO_TRACI
template void MSLane::fill<LANE_RTREE_QUAL>(LANE_RTREE_QUAL& into);
#endif

// ------   ------
bool
MSLane::appropriate(const MSVehicle* veh) {
    if (myEdge->getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if (veh->succEdge(1) == 0) {
        assert(veh->getBestLanes().size() > veh->getLaneIndex());
        if (veh->getBestLanes()[veh->getLaneIndex()].bestLaneOffset == 0) {
            return true;
        } else {
            return false;
        }
    }
    MSLinkCont::const_iterator link = succLinkSec(*veh, 1, *this, veh->getBestLanesContinuation());
    return (link != myLinks.end());
}


bool
MSLane::integrateNewVehicle(SUMOTime) {
    bool wasInactive = myVehicles.size() == 0;
    sort(myVehBuffer.begin(), myVehBuffer.end(), vehicle_position_sorter());
    for (std::vector<MSVehicle*>::const_iterator i = myVehBuffer.begin(); i != myVehBuffer.end(); ++i) {
        MSVehicle* veh = *i;
        myVehicles.insert(myVehicles.begin(), veh);
        myBruttoVehicleLengthSum += veh->getVehicleType().getLengthWithGap();
        myNettoVehicleLengthSum += veh->getVehicleType().getLength();
        myEdge->markDelayed();
    }
    myVehBuffer.clear();
    return wasInactive && myVehicles.size() != 0;
}


bool
MSLane::isLinkEnd(MSLinkCont::const_iterator& i) const {
    return i == myLinks.end();
}


bool
MSLane::isLinkEnd(MSLinkCont::iterator& i) {
    return i == myLinks.end();
}

bool
MSLane::isEmpty() const {
    return (myVehicles.empty() && myInlappingVehicle == 0);
}

MSVehicle*
MSLane::getLastVehicle() const {
    if (myVehicles.size() == 0) {
        return 0;
    }
    return *myVehicles.begin();
}


MSVehicle*
MSLane::getFirstVehicle() const {
    if (myVehicles.size() == 0) {
        return 0;
    }
    return *(myVehicles.end() - 1);
}


MSLinkCont::const_iterator
MSLane::succLinkSec(const SUMOVehicle& veh, unsigned int nRouteSuccs,
                    const MSLane& succLinkSource, const std::vector<MSLane*>& conts) {
    const MSEdge* nRouteEdge = veh.succEdge(nRouteSuccs);
    // check whether the vehicle tried to look beyond its route
    if (nRouteEdge == 0) {
        // return end (no succeeding link) if so
        return succLinkSource.myLinks.end();
    }
    // if we are on an internal lane there should only be one link and it must be allowed
    if (succLinkSource.getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        assert(succLinkSource.myLinks.size() == 1);
        // could have been disallowed dynamically with a rerouter or via TraCI
        // assert(succLinkSource.myLinks[0]->getLane()->allowsVehicleClass(veh.getVehicleType().getVehicleClass()));
        return succLinkSource.myLinks.begin();
    }
    // a link may be used if
    //  1) there is a destination lane ((*link)->getLane()!=0)
    //  2) the destination lane belongs to the next edge in route ((*link)->getLane()->myEdge == nRouteEdge)
    //  3) the destination lane allows the vehicle's class ((*link)->getLane()->allowsVehicleClass(veh.getVehicleClass()))

    // there should be a link which leads to the next desired lane our route in "conts" (built in "getBestLanes")
    // "conts" stores the best continuations of our current lane
    // we should never return an arbitrary link since this may cause collisions
    MSLinkCont::const_iterator link;
    if (nRouteSuccs < conts.size()) {
        // we go through the links in our list and return the matching one
        for (link = succLinkSource.myLinks.begin(); link != succLinkSource.myLinks.end(); ++link) {
            if ((*link)->getLane() != 0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleType().getVehicleClass())) {
                // we should use the link if it connects us to the best lane
                if ((*link)->getLane() == conts[nRouteSuccs]) {
                    return link;
                }
            }
        }
    } else {
        // the source lane is a dead end (no continuations exist)
        return succLinkSource.myLinks.end();
    }
    // the only case where this should happen is for a disconnected route (deliberately ignored)
#ifdef _DEBUG
    WRITE_WARNING("Could not find connection between lane '" + succLinkSource.getID() + "' and lane '" + conts[nRouteSuccs]->getID() +
                  "' for vehicle '" + veh.getID() + "' time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
#endif
    return succLinkSource.myLinks.end();
}



const MSLinkCont&
MSLane::getLinkCont() const {
    return myLinks;
}


void
MSLane::setMaxSpeed(SUMOReal val) {
    myMaxSpeed = val;
    myEdge->recalcCache();
}


void
MSLane::setLength(SUMOReal val) {
    myLength = val;
    myEdge->recalcCache();
}


void
MSLane::swapAfterLaneChange(SUMOTime) {
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
}


MSVehicle*
MSLane::removeVehicle(MSVehicle* remVehicle, MSMoveReminder::Notification notification, bool notify) {
    for (MSLane::VehCont::iterator it = myVehicles.begin(); it < myVehicles.end(); it++) {
        if (remVehicle == *it) {
            if (notify) {
                remVehicle->leaveLane(notification);
            }
            myVehicles.erase(it);
            myBruttoVehicleLengthSum -= remVehicle->getVehicleType().getLengthWithGap();
            myNettoVehicleLengthSum -= remVehicle->getVehicleType().getLength();
            break;
        }
    }
    return remVehicle;
}


MSLane*
MSLane::getParallelLane(int offset) const {
    return myEdge->parallelLane(this, offset);
}


void
MSLane::addIncomingLane(MSLane* lane, MSLink* viaLink) {
    IncomingLaneInfo ili;
    ili.lane = lane;
    ili.viaLink = viaLink;
    ili.length = lane->getLength();
    myIncomingLanes.push_back(ili);
}


void
MSLane::addApproachingLane(MSLane* lane, bool warnMultiCon) {
    MSEdge* approachingEdge = &lane->getEdge();
    if (myApproachingLanes.find(approachingEdge) == myApproachingLanes.end()) {
        myApproachingLanes[approachingEdge] = std::vector<MSLane*>();
    } else if (approachingEdge->getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL && warnMultiCon) {
        // whenever a normal edge connects twice, there is a corresponding
        // internal edge wich connects twice, one warning is sufficient
        WRITE_WARNING("Lane '" + getID() + "' is approached multiple times from edge '" + approachingEdge->getID() + "'. This may cause collisions.");
    }
    myApproachingLanes[approachingEdge].push_back(lane);
}


bool
MSLane::isApproachedFrom(MSEdge* const edge) {
    return myApproachingLanes.find(edge) != myApproachingLanes.end();
}


bool
MSLane::isApproachedFrom(MSEdge* const edge, MSLane* const lane) {
    std::map<MSEdge*, std::vector<MSLane*> >::const_iterator i = myApproachingLanes.find(edge);
    if (i == myApproachingLanes.end()) {
        return false;
    }
    const std::vector<MSLane*>& lanes = (*i).second;
    return find(lanes.begin(), lanes.end(), lane) != lanes.end();
}


class by_second_sorter {
public:
    inline int operator()(const std::pair<const MSVehicle* , SUMOReal>& p1, const std::pair<const MSVehicle* , SUMOReal>& p2) const {
        return p1.second < p2.second;
    }
};


SUMOReal MSLane::getMissingRearGap(
    SUMOReal backOffset, SUMOReal leaderSpeed, SUMOReal leaderMaxDecel) const {
    // this follows the same logic as getFollowerOnConsecutive. we do a tree
    // search and check for the vehicle with the largest missing rear gap within
    // relevant range
    SUMOReal result = 0;
    std::pair<MSVehicle* const, SUMOReal> followerInfo = getFollowerOnConsecutive(backOffset, leaderSpeed, leaderMaxDecel);
    MSVehicle* v = followerInfo.first;
    if (v != 0) {
        result = v->getCarFollowModel().getSecureGap(v->getSpeed(), leaderSpeed, leaderMaxDecel) - followerInfo.second;
    }
    return result;
}


SUMOReal
MSLane::getMaximumBrakeDist() const {
    const MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    const SUMOReal maxSpeed = getSpeedLimit() * vc.getMaxSpeedFactor();
    // this is an upper bound on the actual braking distance (see ticket #860)
    return maxSpeed * maxSpeed * 0.5 / vc.getMinDeceleration();
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getFollowerOnConsecutive(
    SUMOReal backOffset, SUMOReal leaderSpeed, SUMOReal leaderMaxDecel) const {
    // do a tree search among all follower lanes and check for the most
    // important vehicle (the one requiring the largest reargap)
    // to get a safe bound on the necessary search depth, we need to consider the maximum speed and minimum
    // deceleration of potential follower vehicles
    SUMOReal dist = getMaximumBrakeDist() - backOffset;

    std::pair<MSVehicle*, SUMOReal> result(static_cast<MSVehicle*>(0), -1);
    SUMOReal missingRearGapMax = -std::numeric_limits<SUMOReal>::max();
    std::set<MSLane*> visited;
    std::vector<MSLane::IncomingLaneInfo> newFound;
    std::vector<MSLane::IncomingLaneInfo> toExamine = myIncomingLanes;
    while (toExamine.size() != 0) {
        for (std::vector<MSLane::IncomingLaneInfo>::iterator i = toExamine.begin(); i != toExamine.end(); ++i) {
            MSLane* next = (*i).lane;
            dist = MAX2(dist, next->getMaximumBrakeDist() - backOffset);
            MSVehicle* v = 0;
            SUMOReal agap = 0;
            if (next->getPartialOccupator() != 0) {
                // the front of v is already on divergent trajectory from the ego vehicle
                // for which this method is called (in the context of MSLaneChanger).
                // Therefore, technically v is not a follower but only an obstruction and
                // the gap is not between the front of v and the back of ego
                // but rather between the flank of v and the back of ego.
                agap = (*i).length - next->getLength() + backOffset - next->getPartialOccupator()->getVehicleType().getMinGap();
                if (agap < 0) {
                    // Only if ego overlaps we treat v as if it were a real follower
                    v = next->getPartialOccupator();
                }
            }
            if (v == 0 && next->getFirstVehicle() != 0) {
                v = next->getFirstVehicle();
                agap = (*i).length - v->getPositionOnLane() + backOffset - v->getVehicleType().getMinGap();
            }
            if (v != 0) {
                const SUMOReal missingRearGap = v->getCarFollowModel().getSecureGap(v->getSpeed(), leaderSpeed, leaderMaxDecel) - agap;
                if (missingRearGap > missingRearGapMax) {
                    missingRearGapMax = missingRearGap;
                    result.first = v;
                    result.second = agap;
                }
            } else {
                if ((*i).length < dist) {
                    const std::vector<MSLane::IncomingLaneInfo>& followers = next->getIncomingLanes();
                    for (std::vector<MSLane::IncomingLaneInfo>::const_iterator j = followers.begin(); j != followers.end(); ++j) {
                        if (visited.find((*j).lane) == visited.end()) {
                            visited.insert((*j).lane);
                            MSLane::IncomingLaneInfo ili;
                            ili.lane = (*j).lane;
                            ili.length = (*j).length + (*i).length;
                            ili.viaLink = (*j).viaLink;
                            newFound.push_back(ili);
                        }
                    }
                }
            }
        }
        toExamine.clear();
        swap(newFound, toExamine);
    }
    return result;
}

std::pair<MSVehicle* const, SUMOReal>
MSLane::getLeader(const MSVehicle* veh, const SUMOReal vehPos, bool checkNext) const {
    // get the leading vehicle for (shadow) veh
    // XXX this only works as long as all lanes of an edge have equal length
    for (VehCont::const_iterator i = myVehicles.begin(); i != myVehicles.end(); ++i) {
        if ((*i)->getPositionOnLane() > vehPos + NUMERICAL_EPS) {
            // XXX refactor leaderInfo to use a const vehicle all the way through the call hierarchy
            MSVehicle* pred = (MSVehicle*)*i;
            return std::pair<MSVehicle* const, SUMOReal>(pred, pred->getPositionOnLane() - pred->getVehicleType().getLength() - veh->getVehicleType().getMinGap() - vehPos);
        }
    }
    // XXX from here on the code mirrors MSLaneChanger::getRealLeader
    MSVehicle* pred = getPartialOccupator();
    if (pred != 0) {
        return std::pair<MSVehicle*, SUMOReal>(pred, getPartialOccupatorEnd() - veh->getVehicleType().getMinGap() - vehPos);
    }
    if (checkNext) {
        SUMOReal seen = getLength() - vehPos;
        SUMOReal speed = veh->getSpeed();
        SUMOReal dist = veh->getCarFollowModel().brakeGap(speed) + veh->getVehicleType().getMinGap();
        if (seen > dist) {
            return std::pair<MSVehicle* const, SUMOReal>(static_cast<MSVehicle*>(0), -1);
        }
        const std::vector<MSLane*>& bestLaneConts = veh->getBestLanesContinuation(this);
        return getLeaderOnConsecutive(dist, seen, speed, *veh, bestLaneConts);
    } else {
        return std::make_pair(static_cast<MSVehicle*>(0), -1);
    }
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getLeaderOnConsecutive(SUMOReal dist, SUMOReal seen, SUMOReal speed, const MSVehicle& veh,
                               const std::vector<MSLane*>& bestLaneConts) const {
    if (seen > dist) {
        return std::make_pair(static_cast<MSVehicle*>(0), -1);
    }
    unsigned int view = 1;
    // loop over following lanes
    if (getPartialOccupator() != 0) {
        return std::make_pair(getPartialOccupator(), seen - (getLength() - getPartialOccupatorEnd()) - veh.getVehicleType().getMinGap());
    }
    const MSLane* nextLane = this;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / MAX2(speed, NUMERICAL_EPS));
    do {
        // get the next link used
        MSLinkCont::const_iterator link = succLinkSec(veh, view, *nextLane, bestLaneConts);
        if (nextLane->isLinkEnd(link) || !(*link)->opened(arrivalTime, speed, speed, veh.getVehicleType().getLength(),
                veh.getImpatience(), veh.getCarFollowModel().getMaxDecel(), 0) || (*link)->haveRed()) {
            break;
        }
#ifdef HAVE_INTERNAL_LANES
        // check for link leaders
        const MSLink::LinkLeaders linkLeaders = (*link)->getLeaderInfo(seen, veh.getVehicleType().getMinGap());
        if (linkLeaders.size() > 0) {
            // XXX if there is more than one link leader we should return the most important
            // one (gap, decel) but this is hard to know at this point
            return linkLeaders[0].vehAndGap;
        }
        bool nextInternal = (*link)->getViaLane() != 0;
#endif
        nextLane = (*link)->getViaLaneOrLane();
        if (nextLane == 0) {
            break;
        }
        MSVehicle* leader = nextLane->getLastVehicle();
        if (leader != 0) {
            return std::make_pair(leader, seen + leader->getPositionOnLane() - leader->getVehicleType().getLength() - veh.getVehicleType().getMinGap());
        } else {
            leader = nextLane->getPartialOccupator();
            if (leader != 0) {
                return std::make_pair(leader, seen + nextLane->getPartialOccupatorEnd() - veh.getVehicleType().getMinGap());
            }
        }
        if (nextLane->getVehicleMaxSpeed(&veh) < speed) {
            dist = veh.getCarFollowModel().brakeGap(nextLane->getVehicleMaxSpeed(&veh));
        }
        seen += nextLane->getLength();
        if (seen <= dist) {
            // delaying the update of arrivalTime and making it conditional to avoid possible integer overflows
            arrivalTime += TIME2STEPS(nextLane->getLength() / MAX2(speed, NUMERICAL_EPS));
        }
#ifdef HAVE_INTERNAL_LANES
        if (!nextInternal) {
            view++;
        }
#else
        view++;
#endif
    } while (seen <= dist);
    return std::make_pair(static_cast<MSVehicle*>(0), -1);
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getCriticalLeader(SUMOReal dist, SUMOReal seen, SUMOReal speed, const MSVehicle& veh) const {
    const std::vector<MSLane*>& bestLaneConts = veh.getBestLanesContinuation(this);
    std::pair<MSVehicle*, SUMOReal> result = std::make_pair(static_cast<MSVehicle*>(0), -1);
    SUMOReal safeSpeed = std::numeric_limits<SUMOReal>::max();
    unsigned int view = 1;
    // loop over following lanes
    // @note: we don't check the partial occupator for this lane since it was
    // already checked in MSLaneChanger::getRealLeader()
    const MSLane* nextLane = this;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / MAX2(speed, NUMERICAL_EPS));
    do {
        // get the next link used
        MSLinkCont::const_iterator link = succLinkSec(veh, view, *nextLane, bestLaneConts);
        if (nextLane->isLinkEnd(link) || !(*link)->opened(arrivalTime, speed, speed, veh.getVehicleType().getLength(),
                veh.getImpatience(), veh.getCarFollowModel().getMaxDecel(), 0) || (*link)->haveRed()) {
            return result;
        }
#ifdef HAVE_INTERNAL_LANES
        // check for link leaders
        const MSLink::LinkLeaders linkLeaders = (*link)->getLeaderInfo(seen, veh.getVehicleType().getMinGap());
        for (MSLink::LinkLeaders::const_iterator it = linkLeaders.begin(); it != linkLeaders.end(); ++it) {
            const MSVehicle* leader = (*it).vehAndGap.first;
            if (leader != 0 && leader != result.first) {
                // XXX ignoring pedestrians here!
                // XXX ignoring the fact that the link leader may alread by following us
                // XXX ignoring the fact that we may drive up to the crossing point
                const SUMOReal tmpSpeed = veh.getSafeFollowSpeed((*it).vehAndGap, seen, nextLane, (*it).distToCrossing);
                if (tmpSpeed < safeSpeed) {
                    safeSpeed = tmpSpeed;
                    result = (*it).vehAndGap;
                }
            }
        }
        bool nextInternal = (*link)->getViaLane() != 0;
#endif
        nextLane = (*link)->getViaLaneOrLane();
        if (nextLane == 0) {
            break;
        }
        MSVehicle* leader = nextLane->getLastVehicle();
        if (leader != 0 && leader != result.first) {
            const SUMOReal gap = seen + leader->getPositionOnLane() - leader->getVehicleType().getLength() - veh.getVehicleType().getMinGap();
            const SUMOReal tmpSpeed = veh.getCarFollowModel().insertionFollowSpeed(leader, speed, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
            if (tmpSpeed < safeSpeed) {
                safeSpeed = tmpSpeed;
                result = std::make_pair(leader, gap);
            }
        } else {
            leader = nextLane->getPartialOccupator();
            if (leader != 0 && leader != result.first) {
                const SUMOReal gap = seen + nextLane->getPartialOccupatorEnd() - veh.getVehicleType().getMinGap();
                const SUMOReal tmpSpeed = veh.getCarFollowModel().insertionFollowSpeed(leader, speed, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
                if (tmpSpeed < safeSpeed) {
                    safeSpeed = tmpSpeed;
                    result = std::make_pair(leader, gap);
                }
            }
        }
        if (nextLane->getVehicleMaxSpeed(&veh) < speed) {
            dist = veh.getCarFollowModel().brakeGap(nextLane->getVehicleMaxSpeed(&veh));
        }
        seen += nextLane->getLength();
        if (seen <= dist) {
            // delaying the update of arrivalTime and making it conditional to avoid possible integer overflows
            arrivalTime += TIME2STEPS(nextLane->getLength() / MAX2(speed, NUMERICAL_EPS));
        }
#ifdef HAVE_INTERNAL_LANES
        if (!nextInternal) {
            view++;
        }
#else
        view++;
#endif
    } while (seen <= dist);
    return result;
}


MSLane*
MSLane::getLogicalPredecessorLane() const {
    if (myLogicalPredecessorLane != 0) {
        return myLogicalPredecessorLane;
    }
    if (myLogicalPredecessorLane == 0) {
        MSEdgeVector pred = myEdge->getIncomingEdges();
        // get only those edges which connect to this lane
        for (MSEdgeVector::iterator i = pred.begin(); i != pred.end();) {
            std::vector<IncomingLaneInfo>::const_iterator j = find_if(myIncomingLanes.begin(), myIncomingLanes.end(), edge_finder(*i));
            if (j == myIncomingLanes.end()) {
                i = pred.erase(i);
            } else {
                ++i;
            }
        }
        // get the lane with the "straightest" connection
        if (pred.size() != 0) {
            std::sort(pred.begin(), pred.end(), by_connections_to_sorter(&getEdge()));
            MSEdge* best = *pred.begin();
            std::vector<IncomingLaneInfo>::const_iterator j = find_if(myIncomingLanes.begin(), myIncomingLanes.end(), edge_finder(best));
            myLogicalPredecessorLane = (*j).lane;
        }
    }
    return myLogicalPredecessorLane;
}


MSLane*
MSLane::getLogicalPredecessorLane(const MSEdge& fromEdge) const {
    for (std::vector<IncomingLaneInfo>::const_iterator i = myIncomingLanes.begin(); i != myIncomingLanes.end(); ++i) {
        MSLane* cand = (*i).lane;
        if (&(cand->getEdge()) == &fromEdge) {
            return (*i).lane;
        }
    }
    return 0;
}


LinkState
MSLane::getIncomingLinkState() const {
    MSLane* pred = getLogicalPredecessorLane();
    if (pred == 0) {
        return LINKSTATE_DEADEND;
    } else {
        return MSLinkContHelper::getConnectingLink(*pred, *this)->getState();
    }
}


std::vector<const MSLane*>
MSLane::getOutgoingLanes() const {
    std::vector<const MSLane*> result;
    for (MSLinkCont::const_iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        assert((*i)->getLane() != 0);
        result.push_back((*i)->getLane());
    }
    return result;
}


void
MSLane::leftByLaneChange(MSVehicle* v) {
    myBruttoVehicleLengthSum -= v->getVehicleType().getLengthWithGap();
    myNettoVehicleLengthSum -= v->getVehicleType().getLength();
}


void
MSLane::enteredByLaneChange(MSVehicle* v) {
    myBruttoVehicleLengthSum += v->getVehicleType().getLengthWithGap();
    myNettoVehicleLengthSum += v->getVehicleType().getLength();
}


int
MSLane::getCrossingIndex() const {
    for (MSLinkCont::const_iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        if ((*i)->getLane()->getEdge().isCrossing()) {
            return (int)(i - myLinks.begin());
        }
    }
    return -1;
}

// ------------ Current state retrieval
SUMOReal
MSLane::getBruttoOccupancy() const {
    SUMOReal fractions = myInlappingVehicle != 0 ? myLength - myInlappingVehicleEnd : 0;
    getVehiclesSecure();
    if (myVehicles.size() != 0) {
        MSVehicle* lastVeh = myVehicles.front();
        if (lastVeh->getPositionOnLane() < lastVeh->getVehicleType().getLength()) {
            fractions -= (lastVeh->getVehicleType().getLength() - lastVeh->getPositionOnLane());
        }
    }
    releaseVehicles();
    return (myBruttoVehicleLengthSum + fractions) / myLength;
}


SUMOReal
MSLane::getNettoOccupancy() const {
    SUMOReal fractions = myInlappingVehicle != 0 ? myLength - myInlappingVehicleEnd : 0;
    getVehiclesSecure();
    if (myVehicles.size() != 0) {
        MSVehicle* lastVeh = myVehicles.front();
        if (lastVeh->getPositionOnLane() < lastVeh->getVehicleType().getLength()) {
            fractions -= (lastVeh->getVehicleType().getLength() - lastVeh->getPositionOnLane());
        }
    }
    releaseVehicles();
    return (myNettoVehicleLengthSum + fractions) / myLength;
}


SUMOReal
MSLane::getBruttoVehLenSum() const {
    return myBruttoVehicleLengthSum;
}


SUMOReal
MSLane::getWaitingSeconds() const {
    if (myVehicles.size() == 0) {
        return 0;
    }
    SUMOReal wtime = 0;
    for (VehCont::const_iterator i = myVehicles.begin(); i != myVehicles.end(); ++i) {
        wtime += (*i)->getWaitingSeconds();
    }
    return wtime;
}


SUMOReal
MSLane::getMeanSpeed() const {
    if (myVehicles.size() == 0) {
        return myMaxSpeed;
    }
    SUMOReal v = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        v += (*i)->getSpeed();
    }
    SUMOReal ret = v / (SUMOReal) myVehicles.size();
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getCO2Emissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getCO2Emissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getCOEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getCOEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getPMxEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getPMxEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getNOxEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getNOxEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHCEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getHCEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getFuelConsumption() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getFuelConsumption();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHarmonoise_NoiseEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    if (vehs.size() == 0) {
        releaseVehicles();
        return 0;
    }
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        SUMOReal sv = (*i)->getHarmonoise_NoiseEmissions();
        ret += (SUMOReal) pow(10., (sv / 10.));
    }
    releaseVehicles();
    return HelpersHarmonoise::sum(ret);
}


bool
MSLane::VehPosition::operator()(const MSVehicle* cmp, SUMOReal pos) const {
    return cmp->getPositionOnLane() >= pos;
}


int
MSLane::vehicle_position_sorter::operator()(MSVehicle* v1, MSVehicle* v2) const {
    return v1->getPositionOnLane() > v2->getPositionOnLane();
}

MSLane::by_connections_to_sorter::by_connections_to_sorter(const MSEdge* const e) :
    myEdge(e),
    myLaneDir(e->getLanes()[0]->getShape().angleAt2D(0))
{ }


int
MSLane::by_connections_to_sorter::operator()(const MSEdge* const e1, const MSEdge* const e2) const {
    const std::vector<MSLane*>* ae1 = e1->allowedLanes(*myEdge);
    const std::vector<MSLane*>* ae2 = e2->allowedLanes(*myEdge);
    SUMOReal s1 = 0;
    if (ae1 != 0 && ae1->size() != 0) {
        s1 = (SUMOReal) ae1->size() + fabs(GeomHelper::angleDiff((*ae1)[0]->getShape().angleAt2D(0), myLaneDir)) / M_PI / 2.;
    }
    SUMOReal s2 = 0;
    if (ae2 != 0 && ae2->size() != 0) {
        s2 = (SUMOReal) ae2->size() + fabs(GeomHelper::angleDiff((*ae2)[0]->getShape().angleAt2D(0), myLaneDir)) / M_PI / 2.;
    }
    return s1 < s2;
}


void
MSLane::saveState(OutputDevice& out) {
    out.openTag(SUMO_TAG_LANE);
    out.openTag(SUMO_TAG_VIEWSETTINGS_VEHICLES);
    out.writeAttr(SUMO_ATTR_VALUE, myVehicles);
    out.closeTag();
    out.closeTag();
}


void
MSLane::loadState(std::vector<std::string>& vehIds, MSVehicleControl& vc) {
    for (std::vector<std::string>::const_iterator it = vehIds.begin(); it != vehIds.end(); ++it) {
        MSVehicle* v = dynamic_cast<MSVehicle*>(vc.getVehicle(*it));
        assert(v != 0);
        v->updateBestLanes(false, this);
        incorporateVehicle(v, v->getPositionOnLane(), v->getSpeed(), myVehicles.end(),
                           MSMoveReminder::NOTIFICATION_JUNCTION);
    }
}



/****************************************************************************/

