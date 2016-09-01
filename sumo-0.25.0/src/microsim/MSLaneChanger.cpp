/****************************************************************************/
/// @file    MSLaneChanger.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Friedemann Wesner
/// @author  Jakob Erdmann
/// @date    Fri, 01 Feb 2002
/// @version $Id: MSLaneChanger.cpp 19329 2015-11-11 11:44:42Z namdre $
///
// Performs lane changing of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2015 DLR (http://www.dlr.de/) and contributors
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

#include "MSLaneChanger.h"
#include "MSNet.h"
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include <cassert>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/MsgHandler.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSLaneChanger::MSLaneChanger(const std::vector<MSLane*>* lanes, bool allowSwap)
    : myAllowsSwap(allowSwap) {
    assert(lanes->size() > 1);

    // Fill the changer with the lane-data.
    myChanger.reserve(lanes->size());
    for (std::vector<MSLane*>::const_iterator lane = lanes->begin(); lane != lanes->end(); ++lane) {
        ChangeElem ce;
        ce.follow    = 0;
        ce.lead      = 0;
        ce.lane      = *lane;
        ce.veh       = (*lane)->myVehicles.rbegin();
        ce.hoppedVeh = 0;
        ce.lastBlocked = 0;
        ce.firstBlocked = 0;
        myChanger.push_back(ce);
    }
}


MSLaneChanger::~MSLaneChanger() {}


void
MSLaneChanger::laneChange(SUMOTime t) {
    // This is what happens in one timestep. After initialization of the
    // changer, each vehicle will try to change. After that the changer
    // nedds an update to prevent multiple changes of one vehicle.
    // Finally, the change-result has to be given back to the lanes.
    initChanger();
    while (vehInChanger()) {
        bool haveChanged = change();
        updateChanger(haveChanged);
    }
    updateLanes(t);
}


void
MSLaneChanger::initChanger() {
    // Prepare myChanger with a safe state.
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        ce->lead = 0;
        ce->hoppedVeh = 0;
        ce->lastBlocked = 0;
        ce->firstBlocked = 0;
        ce->dens = 0;

        MSLane::VehCont& vehicles = ce->lane->myVehicles;
        if (vehicles.empty()) {
            ce->veh  = vehicles.rend();
            ce->follow = 0;
            continue;
        }
        ce->veh  = vehicles.rbegin();
        if (vehicles.size() == 1) {
            ce->follow = 0;
            continue;
        }
        ce->follow = *(vehicles.rbegin() + 1);
    }
}


bool
MSLaneChanger::change() {
    // Find change-candidate. If it is on an allowed lane, try to change
    // to the right (there is a rule in Germany that you have to change
    // to the right, unless you are overtaking). If change to the right
    // isn't possible, check if there is a possibility to overtake (on the
    // left.
    // If candidate isn't on an allowed lane, changing to an allowed has
    // priority.
    myCandi = findCandidate();
    MSVehicle* vehicle = veh(myCandi);
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gDebugSelectedVehicle == vehicle->getID()) {
        int bla = 0;
    }
#endif
    if (vehicle->getLane() != (*myCandi).lane || vehicle->getLaneChangeModel().isChangingLanes()) {
        // vehicles shadows and changing vehicles are not eligible
        registerUnchanged(vehicle);
        return false;
    }
#ifndef NO_TRACI
    if (vehicle->hasInfluencer() && vehicle->getInfluencer().isVTDControlled()) {
        return false; // !!! temporary; just because it broke, here
    }
#endif
    vehicle->updateBestLanes(); // needed?
    for (int i = 0; i < (int) myChanger.size(); ++i) {
        vehicle->adaptBestLanesOccupation(i, myChanger[i].dens);
    }
    const std::vector<MSVehicle::LaneQ>& preb = vehicle->getBestLanes();
    std::pair<MSVehicle* const, SUMOReal> leader = getRealLeader(myCandi);
    // check whether the vehicle wants and is able to change to right lane
    int state1 = 0;
    if (myCandi != myChanger.begin() && (myCandi - 1)->lane->allowsVehicleClass(veh(myCandi)->getVehicleType().getVehicleClass())) {
        state1 = checkChange(-1, leader, preb);
        bool changingAllowed1 = (state1 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state1 & LCA_RIGHT) != 0 && changingAllowed1) {
            startChange(vehicle, myCandi, -1);
            return true;
        }
        if ((state1 & LCA_RIGHT) != 0 && (state1 & LCA_URGENT) != 0) {
            (myCandi - 1)->lastBlocked = vehicle;
            if ((myCandi - 1)->firstBlocked == 0) {
                (myCandi - 1)->firstBlocked = vehicle;
            }
        }
    }



    // check whether the vehicle wants and is able to change to left lane
    int state2 = 0;
    if ((myCandi + 1) != myChanger.end() && (myCandi + 1)->lane->allowsVehicleClass(veh(myCandi)->getVehicleType().getVehicleClass())) {
        state2 = checkChange(1, leader, preb);
        bool changingAllowed2 = (state2 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state2 & LCA_LEFT) != 0 && changingAllowed2) {
            startChange(vehicle, myCandi, 1);
            return true;
        }
        if ((state2 & LCA_LEFT) != 0 && (state2 & LCA_URGENT) != 0) {
            (myCandi + 1)->lastBlocked = vehicle;
            if ((myCandi + 1)->firstBlocked == 0) {
                (myCandi + 1)->firstBlocked = vehicle;
            }
        }
    }

    if ((state1 & (LCA_URGENT)) != 0 && (state2 & (LCA_URGENT)) != 0) {
        // ... wants to go to the left AND to the right
        // just let them go to the right lane...
        state2 = 0;
    }
    vehicle->getLaneChangeModel().setOwnState(state2 | state1);

    // check whether the vehicles should be swapped
    if (myAllowsSwap && ((state1 & (LCA_URGENT)) != 0 || (state2 & (LCA_URGENT)) != 0)) {
        // get the direction ...
        ChangerIt target;
        int direction = 0;
        if ((state1 & (LCA_URGENT)) != 0) {
            // ... wants to go right
            target = myCandi - 1;
            direction = -1;
        }
        if ((state2 & (LCA_URGENT)) != 0) {
            // ... wants to go left
            target = myCandi + 1;
            direction = 1;
        }
        MSVehicle* prohibitor = target->lead;
        if (target->hoppedVeh != 0) {
            SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
            if (prohibitor == 0 || (hoppedPos > vehicle->getPositionOnLane() && prohibitor->getPositionOnLane() > hoppedPos)) {
                prohibitor = 0;// !!! vehicles should not jump over more than one lanetarget->hoppedVeh;
            }
        }
        if (prohibitor != 0
                &&
                ((prohibitor->getLaneChangeModel().getOwnState() & (LCA_URGENT/*|LCA_SPEEDGAIN*/)) != 0
                 &&
                 (prohibitor->getLaneChangeModel().getOwnState() & (LCA_LEFT | LCA_RIGHT))
                 !=
                 (vehicle->getLaneChangeModel().getOwnState() & (LCA_LEFT | LCA_RIGHT))
                )
           ) {

            // check for position and speed
            if (prohibitor->getVehicleType().getLengthWithGap() - vehicle->getVehicleType().getLengthWithGap() == 0) {
                // ok, may be swapped
                // remove vehicle to swap with
                MSLane::VehCont::iterator i = find(target->lane->myTmpVehicles.begin(), target->lane->myTmpVehicles.end(), prohibitor);
                if (i != target->lane->myTmpVehicles.end()) {
                    assert(*i == prohibitor);
                    target->lane->myTmpVehicles.erase(i);
                    // set this vehicle
                    target->hoppedVeh = vehicle;
                    target->lane->myTmpVehicles.insert(target->lane->myTmpVehicles.begin(), vehicle);
                    myCandi->hoppedVeh = prohibitor;
                    myCandi->lane->myTmpVehicles.insert(myCandi->lane->myTmpVehicles.begin(), prohibitor);

                    // leave lane and detectors
                    vehicle->leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
                    prohibitor->leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
                    // patch position and speed
                    SUMOReal p1 = vehicle->getPositionOnLane();
                    vehicle->myState.myPos = prohibitor->myState.myPos;
                    prohibitor->myState.myPos = p1;
                    p1 = vehicle->getSpeed();
                    vehicle->myState.mySpeed = prohibitor->myState.mySpeed;
                    prohibitor->myState.mySpeed = p1;
                    // enter lane and detectors
                    vehicle->enterLaneAtLaneChange(target->lane);
                    prohibitor->enterLaneAtLaneChange(myCandi->lane);
                    // mark lane change
                    vehicle->getLaneChangeModel().changed(direction);
                    prohibitor->getLaneChangeModel().changed(-direction);
                    (myCandi)->dens += prohibitor->getVehicleType().getLengthWithGap();
                    (target)->dens += vehicle->getVehicleType().getLengthWithGap();
                    return true;
                }
            }
        }
    }
    registerUnchanged(vehicle);
    return false;
}


void
MSLaneChanger::registerUnchanged(MSVehicle* vehicle) {
    myCandi->lane->myTmpVehicles.insert(myCandi->lane->myTmpVehicles.begin(), veh(myCandi));
    vehicle->getLaneChangeModel().unchanged();
    (myCandi)->dens += vehicle->getVehicleType().getLengthWithGap();
}


void
MSLaneChanger::startChange(MSVehicle* vehicle, ChangerIt& from, int direction) {
    ChangerIt to = from + direction;
    to->hoppedVeh = vehicle;
    // @todo delay entering the target lane until the vehicle intersects it
    //       physically (considering lane width and vehicle width)
    to->lane->myTmpVehicles.insert(to->lane->myTmpVehicles.begin(), vehicle);
    const bool continuous = vehicle->getLaneChangeModel().startLaneChangeManeuver(from->lane, to->lane, direction);
    if (continuous) {
        from->lane->myTmpVehicles.insert(from->lane->myTmpVehicles.begin(), vehicle);
        from->dens += vehicle->getVehicleType().getLengthWithGap();
    }
    to->dens += to->hoppedVeh->getVehicleType().getLengthWithGap();
}


std::pair<MSVehicle* const, SUMOReal>
MSLaneChanger::getRealLeader(const ChangerIt& target) const {
    // get the leading vehicle on the lane to change to
    MSVehicle* neighLead = target->lead;
    // check whether the hopped vehicle became the leader
    if (target->hoppedVeh != 0) {
        SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
        if (hoppedPos > veh(myCandi)->getPositionOnLane() && (neighLead == 0 || neighLead->getPositionOnLane() > hoppedPos)) {
            neighLead = target->hoppedVeh;
        }
    }
    if (neighLead == 0) {
        MSLane* targetLane = target->lane;
        MSVehicle* predP = targetLane->getPartialOccupator();
        if (predP != 0) {
            return std::pair<MSVehicle*, SUMOReal>(predP, targetLane->getPartialOccupatorEnd() - veh(myCandi)->getPositionOnLane() - veh(myCandi)->getVehicleType().getMinGap());
        }
        SUMOReal seen = myCandi->lane->getLength() - veh(myCandi)->getPositionOnLane();
        SUMOReal speed = veh(myCandi)->getSpeed();
        SUMOReal dist = veh(myCandi)->getCarFollowModel().brakeGap(speed) + veh(myCandi)->getVehicleType().getMinGap();
        if (seen > dist) {
            return std::pair<MSVehicle* const, SUMOReal>(static_cast<MSVehicle*>(0), -1);
        }
        const std::vector<MSLane*>& bestLaneConts = veh(myCandi)->getBestLanesContinuation(targetLane);
        return target->lane->getLeaderOnConsecutive(dist, seen, speed, *veh(myCandi), bestLaneConts);
    } else {
        MSVehicle* candi = veh(myCandi);
        return std::pair<MSVehicle* const, SUMOReal>(neighLead, neighLead->getPositionOnLane() - neighLead->getVehicleType().getLength() - candi->getPositionOnLane() - candi->getVehicleType().getMinGap());
    }
}


std::pair<MSVehicle* const, SUMOReal>
MSLaneChanger::getRealFollower(const ChangerIt& target) const {
    MSVehicle* neighFollow = veh(target);
    // check whether the hopped vehicle became the follower
    if (target->hoppedVeh != 0) {
        SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
        if (hoppedPos <= veh(myCandi)->getPositionOnLane() && (neighFollow == 0 || neighFollow->getPositionOnLane() < hoppedPos)) {
            neighFollow = target->hoppedVeh;
        }
    }
    if (neighFollow == 0) {
        MSVehicle* candi = veh(myCandi);
        return target->lane->getFollowerOnConsecutive(
                   candi->getPositionOnLane() - candi->getVehicleType().getLength(),
                   candi->getSpeed(), candi->getCarFollowModel().getMaxDecel());
    } else {
        MSVehicle* candi = veh(myCandi);
        return std::pair<MSVehicle* const, SUMOReal>(neighFollow,
                candi->getPositionOnLane() - candi->getVehicleType().getLength() - neighFollow->getPositionOnLane() - neighFollow->getVehicleType().getMinGap());
    }
}




void
MSLaneChanger::updateChanger(bool vehHasChanged) {
    assert(myCandi->veh != myCandi->lane->myVehicles.rend());

    // "Push" the vehicles to the back, i.e. follower becomes vehicle,
    // vehicle becomes leader, and leader becomes predecessor of vehicle,
    // if it exists.
    if (!vehHasChanged || MSGlobals::gLaneChangeDuration > DELTA_T) {
        myCandi->lead = veh(myCandi);
    }
    myCandi->veh    = myCandi->veh + 1;

    if (veh(myCandi) == 0) {
        assert(myCandi->follow == 0);
        // leader already 0.
        return;
    }
    if (myCandi->veh + 1 == myCandi->lane->myVehicles.rend()) {
        myCandi->follow = 0;
    } else {
        myCandi->follow = *(myCandi->veh + 1);
    }
    return;
}


void
MSLaneChanger::updateLanes(SUMOTime t) {

    // Update the lane's vehicle-container.
    // First: it is bad style to change other classes members, but for
    // this release, other attempts were too time-consuming. In a next
    // release we will change from this lane-centered design to a vehicle-
    // centered. This will solve many problems.
    // Second: this swap would be faster if vehicle-containers would have
    // been pointers, but then I had to change too much of the MSLane code.
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {

        ce->lane->swapAfterLaneChange(t);
    }
}


MSLaneChanger::ChangerIt
MSLaneChanger::findCandidate() {
    // Find the vehicle in myChanger with the smallest position. If there
    // is no vehicle in myChanger (shouldn't happen) , return
    // myChanger.end().
    ChangerIt max = myChanger.end();
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        if (veh(ce) == 0) {
            continue;
        }
        if (max == myChanger.end()) {
            max = ce;
            continue;
        }
        assert(veh(ce)  != 0);
        assert(veh(max) != 0);
        if (veh(max)->getPositionOnLane() < veh(ce)->getPositionOnLane()) {
            max = ce;
        }
    }
    assert(max != myChanger.end());
    assert(veh(max) != 0);
    return max;
}

int
MSLaneChanger::checkChange(
    int laneOffset,
    const std::pair<MSVehicle* const, SUMOReal>& leader,
    const std::vector<MSVehicle::LaneQ>& preb) const {
    std::pair<MSVehicle* const, SUMOReal> neighLead = getRealLeader(myCandi + laneOffset);
    std::pair<MSVehicle* const, SUMOReal> neighFollow = getRealFollower(myCandi + laneOffset);
    MSVehicle* vehicle = veh(myCandi);
    ChangerIt target = myCandi + laneOffset;
    int blocked = 0;
    int blockedByLeader = (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_LEADER : LCA_BLOCKED_BY_LEFT_LEADER);
    int blockedByFollower = (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_FOLLOWER : LCA_BLOCKED_BY_LEFT_FOLLOWER);
    // overlap
    if (neighFollow.first != 0 && neighFollow.second < 0) {
        blocked |= (blockedByFollower | LCA_OVERLAPPING);
    }
    if (neighLead.first != 0 && neighLead.second < 0) {
        blocked |= (blockedByLeader | LCA_OVERLAPPING);
    }
    // safe back gap
    if (neighFollow.first != 0) {
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (neighFollow.second < neighFollow.first->getCarFollowModel().getSecureGap(neighFollow.first->getSpeed(), vehicle->getSpeed(), vehicle->getCarFollowModel().getMaxDecel())) {
            blocked |= blockedByFollower;
        }
    }

    // safe front gap
    if (neighLead.first != 0) {
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (neighLead.second < vehicle->getCarFollowModel().getSecureGap(vehicle->getSpeed(), neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel())) {
            blocked |= blockedByLeader;
        }
    }

    MSAbstractLaneChangeModel::MSLCMessager msg(leader.first, neighLead.first, neighFollow.first);
    int state = blocked | vehicle->getLaneChangeModel().wantsChange(
                    laneOffset, msg, blocked, leader, neighLead, neighFollow, *(target->lane), preb, &(myCandi->lastBlocked), &(myCandi->firstBlocked));

    if (blocked == 0 && (state & LCA_WANTS_LANECHANGE) != 0 && neighLead.first != 0) {
        // do are more carefull (but expensive) check to ensure that a
        // safety-critical leader is not being overloocked
        const SUMOReal seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
        const SUMOReal speed = vehicle->getSpeed();
        const SUMOReal dist = vehicle->getCarFollowModel().brakeGap(speed) + vehicle->getVehicleType().getMinGap();
        const MSLane* targetLane = (myCandi + laneOffset)->lane;
        if (seen < dist) {
            std::pair<MSVehicle* const, SUMOReal> neighLead2 = targetLane->getCriticalLeader(dist, seen, speed, *vehicle);
            if (neighLead2.first != 0 && neighLead2.first != neighLead.first
                    && (neighLead2.second < vehicle->getCarFollowModel().getSecureGap(
                            vehicle->getSpeed(), neighLead2.first->getSpeed(), neighLead2.first->getCarFollowModel().getMaxDecel()))) {
                state |= blockedByLeader;
            }
        }
    }
    if ((state & LCA_BLOCKED) == 0 && (state & LCA_WANTS_LANECHANGE) != 0 && MSGlobals::gLaneChangeDuration > DELTA_T) {
        // ensure that a continuous lane change manoeuvre can be completed
        // before the next turning movement
        SUMOReal seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
        const SUMOReal decel = vehicle->getCarFollowModel().getMaxDecel() * STEPS2TIME(MSGlobals::gLaneChangeDuration);
        const SUMOReal avgSpeed = 0.5 * (vehicle->getSpeed() + MAX2((SUMOReal)0, vehicle->getSpeed() - decel));
        const SUMOReal space2change = avgSpeed * STEPS2TIME(MSGlobals::gLaneChangeDuration);
        // for finding turns it doesn't matter whether we look along the current lane or the target lane
        const std::vector<MSLane*>& bestLaneConts = vehicle->getBestLanesContinuation();
        unsigned int view = 1;
        MSLane* nextLane = vehicle->getLane();
        MSLinkCont::const_iterator link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
        while (!nextLane->isLinkEnd(link) && seen <= space2change) {
            if ((*link)->getDirection() == LINKDIR_LEFT || (*link)->getDirection() == LINKDIR_RIGHT
                    // the lanes after an internal junction are on different
                    // edges and do not allow lane-changing
                    || (nextLane->getEdge().isInternal() && (*link)->getViaLaneOrLane()->getEdge().isInternal())
               ) {
                state |= LCA_INSUFFICIENT_SPACE;
                break;
            }
#ifdef HAVE_INTERNAL_LANES
            if ((*link)->getViaLane() == 0) {
                view++;
            }
#else
            view++;
#endif
            nextLane = (*link)->getViaLaneOrLane();
            seen += nextLane->getLength();
            // get the next link used
            link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
        }

        if ((state & LCA_BLOCKED) == 0) {
            // check for dangerous leaders in case the target lane changes laterally between
            // now and the lane-changing midpoint
            const SUMOReal speed = vehicle->getSpeed();
            seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
            nextLane = vehicle->getLane();
            view = 1;
            const SUMOReal dist = vehicle->getCarFollowModel().brakeGap(speed) + vehicle->getVehicleType().getMinGap();
            MSLinkCont::const_iterator link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
            while (!nextLane->isLinkEnd(link) && seen <= space2change && seen <= dist) {
                nextLane = (*link)->getViaLaneOrLane();
                MSLane* targetLane = nextLane->getParallelLane(laneOffset);
                if (targetLane == 0) {
                    state |= LCA_INSUFFICIENT_SPACE;
                    break;
                } else {
                    std::pair<MSVehicle* const, SUMOReal> neighLead2 = targetLane->getLeader(vehicle, -seen, true);
                    if (neighLead2.first != 0 && neighLead2.first != neighLead.first
                            && (neighLead2.second < vehicle->getCarFollowModel().getSecureGap(
                                    vehicle->getSpeed(), neighLead2.first->getSpeed(), neighLead2.first->getCarFollowModel().getMaxDecel()))) {
                        state |= blockedByLeader;
                        break;
                    }
                }
#ifdef HAVE_INTERNAL_LANES
                if ((*link)->getViaLane() == 0) {
                    view++;
                }
#else
                view++;
#endif
                seen += nextLane->getLength();
                // get the next link used
                link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
            }
        }
    }
#ifndef NO_TRACI
    // let TraCI influence the wish to change lanes and the security to take
    //const int oldstate = state;
    state = vehicle->influenceChangeDecision(state);
    //if (vehicle->getID() == "150_2_36000000") {
    //    std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) << " veh=" << vehicle->getID() << " oldstate=" << oldstate << " newstate=" << state << "\n";
    //}
#endif
    return state;
}

/****************************************************************************/

