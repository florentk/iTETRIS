/****************************************************************************/
/// @file    MSLaneChanger.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 01 Feb 2002
/// @version $Id: MSLaneChanger.h 19336 2015-11-12 08:42:09Z namdre $
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
#ifndef MSLaneChanger_h
#define MSLaneChanger_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLane.h"
#include "MSEdge.h"
#include "MSVehicle.h"
#include <vector>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLaneChanger
 * @brief Performs lane changing of vehicles
 */
class MSLaneChanger {
public:
    /// Constructor
    MSLaneChanger(const std::vector<MSLane*>* lanes, bool allowSwap);

    /// Destructor.
    ~MSLaneChanger();

    /// Start lane-change-process for all vehicles on the edge'e lanes.
    void laneChange(SUMOTime t);

public:
    /** Structure used for lane-change. For every lane you have to
        know four vehicles, the change-candidate veh and it's follower
        and leader. Further, information about the last vehicle that changed
        into this lane is needed */
    struct ChangeElem {
        /// the vehicle following the current vehicle
        MSVehicle*                follow;
        /// the vehicle in front of the current vehicle
        MSVehicle*                lead;
        /// the lane the vehicle is on
        MSLane*                   lane;
        /// the regarded vehicle
        MSLane::VehCont::reverse_iterator veh;
        /// last vehicle that changed into this lane
        MSVehicle*                hoppedVeh;
        /// the vehicle that really wants to change to this lane
        MSVehicle*                lastBlocked;
        /// the first vehicle on this edge that wants to change to this lane
        MSVehicle*                firstBlocked;

        SUMOReal dens;

    };

public:
    /** @brief The list of changers;
        For each lane, a ChangeElem is being build */
    typedef std::vector< ChangeElem > Changer;

    /// the iterator moving over the ChangeElems
    typedef Changer::iterator ChangerIt;

    /// the iterator moving over the ChangeElems
    typedef Changer::const_iterator ConstChangerIt;

protected:
    /// Initialize the changer before looping over all vehicles.
    void initChanger();

    /** @brief Check if there is a single change-candidate in the changer.
        Returns true if there is one. */
    bool vehInChanger() const {
        // If there is at least one valid vehicle under the veh's in myChanger
        // return true.
        for (ConstChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
            if (veh(ce) != 0) {
                return true;
            }
        }
        return false;
    }

    /** Returns a pointer to the changer-element-iterator vehicle, or 0 if
        there is none. */
    MSVehicle* veh(ConstChangerIt ce) const {
        // If ce has a valid vehicle, return it. Otherwise return 0.
        if (ce->veh != ce->lane->myVehicles.rend()) {
            return *(ce->veh);
        }
        return 0;
    }


    /** Find a new candidate and try to change it. */
    bool change();

    /** Update changer for vehicles that did not change */
    void registerUnchanged(MSVehicle* vehicle);

    /** After the possible change, update the changer. */
    void updateChanger(bool vehHasChanged);

    /** During lane-change a temporary vehicle container is filled within
        the lanes (bad pratice to modify foreign members, I know). Swap
        this container with the real one. */
    void updateLanes(SUMOTime t);

    /** @brief Find current candidate.
        If there is none, myChanger.end() is returned. */
    ChangerIt findCandidate();

    /* @brief check whether lane changing in the given direction is desirable
     * and possible */
    int checkChange(
        int laneOffset,
        const std::pair<MSVehicle* const, SUMOReal>& leader,
        const std::vector<MSVehicle::LaneQ>& preb) const;

    ///  @brief start the lane change maneuver (and finish it instantly if gLaneChangeDuration == 0)
    void startChange(MSVehicle* vehicle, ChangerIt& from, int direction);

    std::pair<MSVehicle* const, SUMOReal> getRealFollower(const ChangerIt& target) const;

    std::pair<MSVehicle* const, SUMOReal> getRealLeader(const ChangerIt& target) const;

protected:
    /// Container for ChangeElemements, one for every lane in the edge.
    Changer   myChanger;

    /** Change-candidate. Last of the vehicles in changer. Only this one
        will try to change. Every vehicle on the edge will be a candidate
        once in the change-process. */
    ChangerIt myCandi;

    /// @brief Whether blocking vehicles may be swapped
    bool myAllowsSwap;

private:
    /// Default constructor.
    MSLaneChanger();

    /// Copy constructor.
    MSLaneChanger(const MSLaneChanger&);

    /// Assignment operator.
    MSLaneChanger& operator=(const MSLaneChanger&);
};


#endif

/****************************************************************************/

