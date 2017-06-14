/****************************************************************************/
/// @file    GUIE2Collector.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Okt 2003
/// @version $Id: GUIE2Collector.cpp 24108 2017-04-27 18:43:30Z behrisch $
///
// The gui-version of the MSE2Collector
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

#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/PositionVector.h>
#include "GUIE2Collector.h"
#include <utils/gui/div/GLHelper.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include "GUIEdge.h"
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIE2Collector-methods
 * ----------------------------------------------------------------------- */
GUIE2Collector::GUIE2Collector(const std::string& id, DetectorUsage usage,
                               MSLane* lane, double startPos, double endPos, double detLength,
                               SUMOTime haltingTimeThreshold, double haltingSpeedThreshold,
                               double jamDistThreshold, const std::string& vTypes, bool showDetector)
    : MSE2Collector(id, usage, lane, startPos, endPos, detLength, haltingTimeThreshold,
                    haltingSpeedThreshold, jamDistThreshold, vTypes),
      myShowDetectorInGUI(showDetector) {}

GUIE2Collector::GUIE2Collector(const std::string& id, DetectorUsage usage,
                               std::vector<MSLane*> lanes, double startPos, double endPos,
                               SUMOTime haltingTimeThreshold, double haltingSpeedThreshold,
                               double jamDistThreshold, const std::string& vTypes, bool showDetector)
    : MSE2Collector(id, usage, lanes, startPos, endPos, haltingTimeThreshold,
                    haltingSpeedThreshold, jamDistThreshold, vTypes),
      myShowDetectorInGUI(showDetector) {}

GUIE2Collector::~GUIE2Collector() {}


GUIDetectorWrapper*
GUIE2Collector::buildDetectorGUIRepresentation() {
    return new MyWrapper(*this);
}



/* -------------------------------------------------------------------------
 * GUIE2Collector::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIE2Collector::MyWrapper::MyWrapper(GUIE2Collector& detector)
    : GUIDetectorWrapper("E2 detector", detector.getID()),
      myDetector(detector) {

    // collect detector shape into one vector (v)
    PositionVector v;
    const std::vector<MSLane*> lanes = detector.getLanes();
    for (std::vector<MSLane*>::const_iterator li = lanes.begin(); li != lanes.end(); ++li) {
        const PositionVector& shape = (*li)->getShape();
        v.insert(v.end(), shape.begin(), shape.end());
    }

    // build geometry
    myFullGeometry = v.getSubpart(detector.getStartPos(), detector.getStartPos() + detector.getLength());
    //
    myShapeRotations.reserve(myFullGeometry.size() - 1);
    myShapeLengths.reserve(myFullGeometry.size() - 1);
    int e = (int) myFullGeometry.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myFullGeometry[i];
        const Position& s = myFullGeometry[i + 1];
        myShapeLengths.push_back(f.distanceTo(s));
        myShapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double) PI);
    }
    //
    myBoundary = myFullGeometry.getBoxBoundary();
}


GUIE2Collector::MyWrapper::~MyWrapper() {}


Boundary
GUIE2Collector::MyWrapper::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}


GUIParameterTableWindow*
GUIE2Collector::MyWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 13);
    // add items
    // parameter
    ret->mkItem("length [m]", false, myDetector.getLength());
    ret->mkItem("position [m]", false, myDetector.getStartPos());
    ret->mkItem("lane", false, myDetector.getLane()->getID());
    // values
    ret->mkItem("vehicles [#]", true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentVehicleNumber));
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentOccupancy));
    ret->mkItem("mean speed [m/s]", true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentMeanSpeed));
    ret->mkItem("mean vehicle length [m]", true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentMeanLength));
    ret->mkItem("jam number [#]", true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentJamNumber));
    ret->mkItem("max jam length [veh]", true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentMaxJamLengthInVehicles));
    ret->mkItem("max jam length [m]", true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentMaxJamLengthInMeters));
    ret->mkItem("jam length sum [veh]", true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentJamLengthInVehicles));
    ret->mkItem("jam length sum [m]", true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentJamLengthInMeters));
    ret->mkItem("started halts [#]", true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentStartedHalts));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIE2Collector::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    if (!myDetector.myShowDetectorInGUI) {
        return;
    }
    glPushName(getGlID());
    glPushMatrix();
    glTranslated(0, 0, getType());
    double dwidth = 1;
    const double exaggeration = s.addSize.getExaggeration(s);
    if (exaggeration > 0) {
        if (myDetector.getUsageType() == DU_TL_CONTROL) {
            dwidth = (double) 0.3;
            glColor3d(0, (double) .6, (double) .8);
        } else {
            glColor3d(0, (double) .8, (double) .8);
        }
        double width = (double) 2.0 * s.scale;
        if (width * exaggeration > 1.0) {
            GLHelper::drawBoxLines(myFullGeometry, myShapeRotations, myShapeLengths, dwidth * exaggeration);
        } else {
            int e = (int) myFullGeometry.size() - 1;
            for (int i = 0; i < e; ++i) {
                GLHelper::drawLine(myFullGeometry[i], myShapeRotations[i], myShapeLengths[i]);
            }
        }
    }
    glPopMatrix();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


GUIE2Collector&
GUIE2Collector::MyWrapper::getDetector() {
    return myDetector;
}



/****************************************************************************/

