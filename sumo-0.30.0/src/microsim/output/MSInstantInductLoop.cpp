/****************************************************************************/
/// @file    MSInstantInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2011-09.08
/// @version $Id: MSInstantInductLoop.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// An instantaneous induction loop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors
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

#include "MSInstantInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/common/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSInstantInductLoop::MSInstantInductLoop(const std::string& id,
        OutputDevice& od, MSLane* const lane, double positionInMeters,
        const std::string& vTypes) :
    MSMoveReminder(id, lane),
    MSDetectorFileOutput(id, vTypes),
    myOutputDevice(od),
    myPosition(positionInMeters), myLastExitTime(-1) {
    assert(myPosition >= 0 && myPosition <= myLane->getLength());
    writeXMLDetectorProlog(od);
}


MSInstantInductLoop::~MSInstantInductLoop() {
}


bool
MSInstantInductLoop::notifyMove(SUMOVehicle& veh, double oldPos,
                                double newPos, double newSpeed) {
    if (!vehicleApplies(veh)) {
        return false;
    }
    if (newPos < myPosition) {
        // detector not reached yet
        return true;
    }

    const double oldSpeed = veh.getPreviousSpeed();
    double enterSpeed = MSGlobals::gSemiImplicitEulerUpdate ? newSpeed : oldSpeed; // NOTE: For the euler update, the vehicle is assumed to travel at constant speed for the whole time step

    if (newPos >= myPosition && oldPos < myPosition/* && static_cast<MSVehicle&>(veh).getLane() == myLane*/) {
        const double timeBeforeEnter = MSCFModel::passingTime(oldPos, myPosition, newPos, oldSpeed, newSpeed);
        const double entryTime = SIMTIME - TS + timeBeforeEnter;
        enterSpeed = MSCFModel::speedAfterTime(timeBeforeEnter, oldSpeed, newPos - oldPos);
        if (myLastExitTime >= 0) {
            write("enter", entryTime, veh, enterSpeed, "gap", entryTime - myLastExitTime);
        } else {
            write("enter", entryTime, veh, enterSpeed);
        }
        myEntryTimes[&veh] = entryTime;
    }
    const double newBackPos = newPos - veh.getVehicleType().getLength();
    const double oldBackPos = oldPos - veh.getVehicleType().getLength();
    if (newBackPos > myPosition) {
        std::map<SUMOVehicle*, double>::iterator i = myEntryTimes.find(&veh);
        if (i != myEntryTimes.end()) {
            // vehicle passed the detector
            const double timeBeforeLeave = MSCFModel::passingTime(oldBackPos, myPosition, newBackPos, oldSpeed, newSpeed);
            const double leaveTime = SIMTIME - TS + timeBeforeLeave;
            write("leave", leaveTime, veh, newSpeed, "occupancy", leaveTime - (*i).second);
            myEntryTimes.erase(i);
            myLastExitTime = leaveTime;
        }
        return false;
    }
    // vehicle stays on the detector
    write("stay", SIMTIME, veh, newSpeed);
    return true;
}


void
MSInstantInductLoop::write(const char* state, double t, SUMOVehicle& veh, double speed, const char* add, double addValue) {
    myOutputDevice.openTag("instantOut").writeAttr(
        "id", getID()).writeAttr("time", toString(t)).writeAttr("state", state).writeAttr(
            "vehID", veh.getID()).writeAttr("speed", toString(speed)).writeAttr(
                "length", toString(veh.getVehicleType().getLength())).writeAttr(
                    "type", veh.getVehicleType().getID());
    if (add != 0) {
        myOutputDevice.writeAttr(add, toString(addValue));
    }
    myOutputDevice.closeTag();
}


bool
MSInstantInductLoop::notifyLeave(SUMOVehicle& veh, double /* lastPos */, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason == MSMoveReminder::NOTIFICATION_JUNCTION) {
        // vehicle might have jumped over detector at the end of the lane. we need
        // one more notifyMove to register it
        return true;
    }
    std::map<SUMOVehicle*, double>::iterator i = myEntryTimes.find(&veh);
    if (i != myEntryTimes.end()) {
        write("leave", SIMTIME, veh, veh.getSpeed());
        myEntryTimes.erase(i);
    }
    return false;
}


void
MSInstantInductLoop::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("instantE1", "instant_e1_file.xsd");
}


/****************************************************************************/
