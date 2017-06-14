/****************************************************************************/
/// @file    MSMeanData_Harmonoise.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id: MSMeanData_Harmonoise.cpp 24108 2017-04-27 18:43:30Z behrisch $
///
// Redirector for mean data output (net->edgecontrol)
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

#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Harmonoise.h"
#include <utils/emissions/HelpersHarmonoise.h>
#include <limits>


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane* const lane, const double length, const bool doAdd,
        const MSMeanData_Harmonoise* parent)
    : MSMeanData::MeanDataValues(lane, length, doAdd, parent),
      currentTimeN(0), meanNTemp(0), myParent(parent) {}


MSMeanData_Harmonoise::MSLaneMeanDataValues::~MSLaneMeanDataValues() {
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::reset(bool) {
    sampleSeconds = 0;
    currentTimeN = 0;
    meanNTemp = 0;
    travelledDistance = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::addTo(MSMeanData::MeanDataValues& val) const {
    MSLaneMeanDataValues& v = (MSLaneMeanDataValues&) val;
    v.sampleSeconds += sampleSeconds;
    v.meanNTemp += (double) pow(10., HelpersHarmonoise::sum(meanNTemp) / 10.);
    v.travelledDistance += travelledDistance;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::update() {
    meanNTemp += (double) pow(10., HelpersHarmonoise::sum(currentTimeN) / 10.);
    currentTimeN = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::notifyMoveInternal(const SUMOVehicle& veh, const double /* frontOnLane */, const double timeOnLane, const double /*meanSpeedFrontOnLane*/, const double meanSpeedVehicleOnLane, const double /*travelledDistanceFrontOnLane*/, const double travelledDistanceVehicleOnLane) {
    const double sn = HelpersHarmonoise::computeNoise(veh.getVehicleType().getEmissionClass(),
                      // XXX: recheck, which value to use here for the speed. (Leo) Refs. #2579
                      (double) meanSpeedVehicleOnLane, veh.getAcceleration());
    currentTimeN += (double) pow(10., (sn / 10.));
    sampleSeconds += timeOnLane;
    travelledDistance += travelledDistanceVehicleOnLane;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::write(OutputDevice& dev, const SUMOTime period,
        const double /*numLanes*/, const double defaultTravelTime, const int /*numVehicles*/) const {
    dev.writeAttr("noise", (meanNTemp != 0 ? (double)(10. * log10(meanNTemp * TS / STEPS2TIME(period))) : (double) 0.));
    if (sampleSeconds > myParent->myMinSamples) {
        double traveltime = myParent->myMaxTravelTime;
        if (travelledDistance > 0.f) {
            traveltime = MIN2(traveltime, myLaneLength * sampleSeconds / travelledDistance);
        }
        dev.writeAttr("traveltime", traveltime);
    } else if (defaultTravelTime >= 0.) {
        // @todo default value for noise
        dev.writeAttr("traveltime", defaultTravelTime);
    }
    dev.closeTag();
}



// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSMeanData_Harmonoise(const std::string& id,
        const SUMOTime dumpBegin, const SUMOTime dumpEnd,
        const bool useLanes, const bool withEmpty,
        const bool printDefaults, const bool withInternal,
        const bool trackVehicles,
        const double maxTravelTime, const double minSamples,
        const std::string& vTypes)
    : MSMeanData(id, dumpBegin, dumpEnd, useLanes, withEmpty, printDefaults,
                 withInternal, trackVehicles, maxTravelTime, minSamples, vTypes) {
}


MSMeanData_Harmonoise::~MSMeanData_Harmonoise() {}


MSMeanData::MeanDataValues*
MSMeanData_Harmonoise::createValues(MSLane* const lane, const double length, const bool doAdd) const {
    return new MSLaneMeanDataValues(lane, length, doAdd, this);
}


void
MSMeanData_Harmonoise::detectorUpdate(const SUMOTime step) {
    MSMeanData::detectorUpdate(step);
    for (std::vector<std::vector<MeanDataValues*> >::const_iterator i = myMeasures.begin(); i != myMeasures.end(); ++i) {
        const std::vector<MeanDataValues*>& lm = *i;
        for (std::vector<MeanDataValues*>::const_iterator j = lm.begin(); j != lm.end(); ++j) {
            (*j)->update();
        }
    }
}


/****************************************************************************/

