/****************************************************************************/
/// @file    MSBatteryExport.cpp
/// @author  Mario Krumnow
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
/// @version $Id: MSBatteryExport.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// Realises dumping Battery Data
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

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSBatteryExport.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_Battery.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif


// ===========================================================================
// method definitions
// ===========================================================================
void
MSBatteryExport::write(OutputDevice& of, SUMOTime timestep, int precision) {
    of.openTag(SUMO_TAG_TIMESTEP).writeAttr(SUMO_ATTR_TIME, time2string(timestep));
    of.setPrecision(precision);

    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();
    for (; it != end; ++it) {
        const MSVehicle* veh = static_cast<const MSVehicle*>((*it).second);

        if (!veh->isOnRoad()) {
            continue;
        }

        std::string fclass = veh->getVehicleType().getID();
        fclass = fclass.substr(0, fclass.find_first_of("@"));

        Position pos = veh->getLane()->getShape().positionAtOffset(veh->getPositionOnLane());

        if (static_cast<MSDevice_Battery*>(veh->getDevice(typeid(MSDevice_Battery))) != 0) {
            MSDevice_Battery* batteryToExport = dynamic_cast<MSDevice_Battery*>(veh->getDevice(typeid(MSDevice_Battery)));
            if (batteryToExport->getMaximumBatteryCapacity() > 0) {
                // Open Row
                of.openTag(SUMO_TAG_VEHICLE);
                // Write ID
                of.writeAttr(SUMO_ATTR_ID, veh->getID());
                // Write consum
                of.writeAttr(SUMO_ATTR_ENERGYCONSUMED, batteryToExport->getConsum());
                // Write Actual battery capacity
                of.writeAttr(SUMO_ATTR_ACTUALBATTERYCAPACITY, batteryToExport->getActualBatteryCapacity());
                // Write Maximum battery capacity
                of.writeAttr(SUMO_ATTR_MAXIMUMBATTERYCAPACITY, batteryToExport->getMaximumBatteryCapacity());
                // Write Charging Station ID
                of.writeAttr(SUMO_ATTR_CHARGINGSTATIONID, batteryToExport->getChargingStationID());
                // Write Charge charged in the Battery
                of.writeAttr(SUMO_ATTR_ENERGYCHARGED, batteryToExport->getEnergyCharged());
                // Write ChargeInTransit
                if (batteryToExport->isChargingInTransit()) {
                    of.writeAttr(SUMO_ATTR_ENERGYCHARGEDINTRANSIT, batteryToExport->getEnergyCharged());
                } else {
                    of.writeAttr(SUMO_ATTR_ENERGYCHARGEDINTRANSIT, 0.00);
                }
                // Write ChargingStopped
                if (batteryToExport->isChargingStopped()) {
                    of.writeAttr(SUMO_ATTR_ENERGYCHARGEDSTOPPED, batteryToExport->getEnergyCharged());
                } else {
                    of.writeAttr(SUMO_ATTR_ENERGYCHARGEDSTOPPED, 0.00);
                }
                // Write Speed
                of.writeAttr(SUMO_ATTR_SPEED, veh->getSpeed());
                // Write Acceleration
                of.writeAttr(SUMO_ATTR_ACCELERATION, veh->getAcceleration());
                // Write pos x
                of.writeAttr(SUMO_ATTR_X, veh->getPosition().x());
                // Write pos y
                of.writeAttr(SUMO_ATTR_Y, veh->getPosition().y());
                // Write Lane ID
                of.writeAttr(SUMO_ATTR_LANE, veh->getLane()->getID());
                // Write vehicle position in the lane
                of.writeAttr(SUMO_ATTR_POSONLANE, veh->getPositionOnLane());
                // Write Time stopped (In all cases)
                of.writeAttr(SUMO_ATTR_TIMESTOPPED, batteryToExport->getVehicleStopped());
                // Close Row
                of.closeTag();
            }
        }
    }
    of.closeTag();
}
