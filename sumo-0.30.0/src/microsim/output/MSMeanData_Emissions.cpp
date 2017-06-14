/****************************************************************************/
/// @file    MSMeanData_Emissions.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id: MSMeanData_Emissions.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// Emission data collector for edges/lanes that
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

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Emissions.h"
#include <utils/emissions/PollutantsInterface.h>
#include <limits>


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// MSMeanData_Emissions::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_Emissions::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane* const lane,
        const double length, const bool doAdd,
        const MSMeanData_Emissions* parent)
    : MSMeanData::MeanDataValues(lane, length, doAdd, parent),
      myEmissions() {}


MSMeanData_Emissions::MSLaneMeanDataValues::~MSLaneMeanDataValues() {
}


void
MSMeanData_Emissions::MSLaneMeanDataValues::reset(bool) {
    sampleSeconds = 0.;
    travelledDistance = 0.;
    myEmissions = PollutantsInterface::Emissions();
}


void
MSMeanData_Emissions::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues& val) const {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.sampleSeconds += sampleSeconds;
    v.travelledDistance += travelledDistance;
    v.myEmissions.addScaled(myEmissions);
}


void
MSMeanData_Emissions::MSLaneMeanDataValues::notifyMoveInternal(const SUMOVehicle& veh, const double /* frontOnLane */, const double timeOnLane, const double /*meanSpeedFrontOnLane*/, const double meanSpeedVehicleOnLane, const double /*travelledDistanceFrontOnLane*/, const double travelledDistanceVehicleOnLane) {
    sampleSeconds += timeOnLane;
    travelledDistance += travelledDistanceVehicleOnLane;
    const double a = veh.getAcceleration();
    myEmissions.addScaled(PollutantsInterface::computeAll(veh.getVehicleType().getEmissionClass(),
                          // XXX: recheck, which value to use here for the speed. (Leo) Refs. #2579
                          meanSpeedVehicleOnLane, a, veh.getSlope()), timeOnLane);
}


void
MSMeanData_Emissions::MSLaneMeanDataValues::write(OutputDevice& dev, const SUMOTime period,
        const double /*numLanes*/, const double defaultTravelTime, const int /*numVehicles*/) const {
    const double normFactor = double(3600. / STEPS2TIME(period) / myLaneLength);
    dev << " CO_abs=\"" << OutputDevice::realString(myEmissions.CO, 6) <<
        "\" CO2_abs=\"" << OutputDevice::realString(myEmissions.CO2, 6) <<
        "\" HC_abs=\"" << OutputDevice::realString(myEmissions.HC, 6) <<
        "\" PMx_abs=\"" << OutputDevice::realString(myEmissions.PMx, 6) <<
        "\" NOx_abs=\"" << OutputDevice::realString(myEmissions.NOx, 6) <<
        "\" fuel_abs=\"" << OutputDevice::realString(myEmissions.fuel, 6) <<
        "\" electricity_abs=\"" << OutputDevice::realString(myEmissions.electricity, 6) <<
        "\"\n            CO_normed=\"" << OutputDevice::realString(normFactor * myEmissions.CO, 6) <<
        "\" CO2_normed=\"" << OutputDevice::realString(normFactor * myEmissions.CO2, 6) <<
        "\" HC_normed=\"" << OutputDevice::realString(normFactor * myEmissions.HC, 6) <<
        "\" PMx_normed=\"" << OutputDevice::realString(normFactor * myEmissions.PMx, 6) <<
        "\" NOx_normed=\"" << OutputDevice::realString(normFactor * myEmissions.NOx, 6) <<
        "\" fuel_normed=\"" << OutputDevice::realString(normFactor * myEmissions.fuel, 6) <<
        "\" electricity_normed=\"" << OutputDevice::realString(normFactor * myEmissions.electricity, 6);
    if (sampleSeconds > myParent->getMinSamples()) {
        double vehFactor = myParent->getMaxTravelTime() / sampleSeconds;
        double traveltime = myParent->getMaxTravelTime();
        if (travelledDistance > 0.f) {
            vehFactor = MIN2(vehFactor, myLaneLength / travelledDistance);
            traveltime = MIN2(traveltime, myLaneLength * sampleSeconds / travelledDistance);
        }
        dev << "\"\n            traveltime=\"" << OutputDevice::realString(traveltime) <<
            "\" CO_perVeh=\"" << OutputDevice::realString(myEmissions.CO * vehFactor, 6) <<
            "\" CO2_perVeh=\"" << OutputDevice::realString(myEmissions.CO2 * vehFactor, 6) <<
            "\" HC_perVeh=\"" << OutputDevice::realString(myEmissions.HC * vehFactor, 6) <<
            "\" PMx_perVeh=\"" << OutputDevice::realString(myEmissions.PMx * vehFactor, 6) <<
            "\" NOx_perVeh=\"" << OutputDevice::realString(myEmissions.NOx * vehFactor, 6) <<
            "\" fuel_perVeh=\"" << OutputDevice::realString(myEmissions.fuel * vehFactor, 6) <<
            "\" electricity_perVeh=\"" << OutputDevice::realString(myEmissions.electricity * vehFactor, 6);
    } else if (defaultTravelTime >= 0.) {
        const MSVehicleType* t = MSNet::getInstance()->getVehicleControl().getVType();
        const double speed = MIN2(myLaneLength / defaultTravelTime, t->getMaxSpeed());
        dev << "\"\n            traveltime=\"" << OutputDevice::realString(defaultTravelTime) <<
            "\" CO_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::CO, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" CO2_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::CO2, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" HC_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::HC, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" PMx_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::PM_X, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" NOx_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::NO_X, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" fuel_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::FUEL, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6) << // @todo: give correct slope
            "\" electricity_perVeh=\"" << OutputDevice::realString(PollutantsInterface::computeDefault(t->getEmissionClass(), PollutantsInterface::ELEC, speed, t->getCarFollowModel().getMaxAccel(), 0, defaultTravelTime), 6); // @todo: give correct slope
    }
    dev << "\"";
    dev.closeTag();
}



// ---------------------------------------------------------------------------
// MSMeanData_Emissions - methods
// ---------------------------------------------------------------------------
MSMeanData_Emissions::MSMeanData_Emissions(const std::string& id,
        const SUMOTime dumpBegin,
        const SUMOTime dumpEnd,
        const bool useLanes, const bool withEmpty,
        const bool printDefaults,
        const bool withInternal,
        const bool trackVehicles,
        const double maxTravelTime,
        const double minSamples,
        const std::string& vTypes)
    : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, printDefaults,
                 withInternal, trackVehicles, maxTravelTime, minSamples, vTypes) {
}


MSMeanData_Emissions::~MSMeanData_Emissions() {}


MSMeanData::MeanDataValues*
MSMeanData_Emissions::createValues(MSLane* const lane, const double length, const bool doAdd) const {
    return new MSLaneMeanDataValues(lane, length, doAdd, this);
}


/****************************************************************************/
