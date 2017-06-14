/****************************************************************************/
/// @file    MSParkingArea.h
/// @author  Mirco Sturari
/// @author  Jakob Erdmann
/// @date    Tue, 19.01.2016
/// @version $Id: MSParkingArea.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A area where vehicles can park next to the road
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2015-2017 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <microsim/MSVehicleType.h>
#include <utils/foxtools/MFXMutex.h>
#include "MSLane.h"
#include "MSTransportable.h"
#include "MSParkingArea.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSParkingArea::MSParkingArea(const std::string& id,
                             const std::vector<std::string>& lines,
                             MSLane& lane,
                             double begPos, double endPos,
                             unsigned int capacity,
                             double width, double length, double angle) :
    MSStoppingPlace(id, lines, lane, begPos, endPos),
    myCapacity(capacity),
    myWidth(width),
    myLength(length),
    myAngle(angle) {
    // initialize unspecified defaults
    if (myWidth == 0) {
        myWidth = SUMO_const_laneWidth;
    }
    if (myLength == 0) {
        myLength = getSpaceDim();
    }

    myShape = lane.getShape();
    myShape.move2side(lane.getWidth() / 2. + myWidth / 2.);
    myShape = myShape.getSubpart(begPos, endPos);
    // Initialize space occupancies if there is a road-side capacity
    // The overall number of lots is fixed and each lot accepts one vehicle regardless of size
    if (myCapacity > 0) {
        for (int i = 1; i <= myCapacity; ++i) {
            mySpaceOccupancies[i] = LotSpaceDefinition();
            mySpaceOccupancies[i].index = i;
            mySpaceOccupancies[i].vehicle = 0;
            mySpaceOccupancies[i].myWidth = myWidth;
            mySpaceOccupancies[i].myLength = myLength;
            mySpaceOccupancies[i].myEndPos = myBegPos + getSpaceDim() * i;

            const Position& f = myShape.positionAtOffset(getSpaceDim() * (i - 1));
            const Position& s = myShape.positionAtOffset(getSpaceDim() * (i));
            double lot_angle = ((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double) PI) + myAngle;
            mySpaceOccupancies[i].myRotation = lot_angle;
            if (myAngle == 0) {
                // parking parallel to the road
                mySpaceOccupancies[i].myPosition = s;
            } else {
                // angled parking
                mySpaceOccupancies[i].myPosition = (f + s) * 0.5;
            }

        }
    }
    computeLastFreePos();
}

MSParkingArea::~MSParkingArea() {}

double
MSParkingArea::getLastFreePos(const SUMOVehicle& /* forVehicle */) const {
    return myLastFreePos;
}

Position
MSParkingArea::getVehiclePosition(const SUMOVehicle& forVehicle) {
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == &forVehicle) {
            return (*i).second.myPosition;
        }
    }
    return Position::INVALID;
}

double
MSParkingArea::getVehicleAngle(const SUMOVehicle& forVehicle) {
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == &forVehicle) {
            return (((*i).second.myRotation - 90.) * (double) PI / (double) 180.0);
        }
    }
    return 0.;
}


double
MSParkingArea::getSpaceDim() const {
    return (myEndPos - myBegPos) / myCapacity;
}


void
MSParkingArea::addLotEntry(double x, double y, double z,
                           double width, double length, double angle) {

    const int i = (int)mySpaceOccupancies.size() + 1;

    mySpaceOccupancies[i] = LotSpaceDefinition();
    mySpaceOccupancies[i].index = i;
    mySpaceOccupancies[i].vehicle = 0;
    mySpaceOccupancies[i].myPosition = Position(x, y, z);
    mySpaceOccupancies[i].myWidth = width;
    mySpaceOccupancies[i].myLength = length;
    mySpaceOccupancies[i].myRotation = angle;
    mySpaceOccupancies[i].myEndPos = myEndPos;
    myCapacity = (int)mySpaceOccupancies.size();
    computeLastFreePos();
}


void
MSParkingArea::enter(SUMOVehicle* what, double beg, double end) {
    if (myLastFreeLot >= 1 && myLastFreeLot <= (int)mySpaceOccupancies.size()) {
        mySpaceOccupancies[myLastFreeLot].vehicle = what;
        myEndPositions[what] = std::pair<double, double>(beg, end);
        computeLastFreePos();
    }
}


void
MSParkingArea::leaveFrom(SUMOVehicle* what) {
    assert(myEndPositions.find(what) != myEndPositions.end());
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == what) {
            (*i).second.vehicle = 0;
            break;
        }
    }
    myEndPositions.erase(myEndPositions.find(what));
    computeLastFreePos();
}


void
MSParkingArea::computeLastFreePos() {
    myLastFreeLot = 0;
    myLastFreePos = myBegPos;
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == 0) {
            myLastFreeLot = (*i).first;
            myLastFreePos = (*i).second.myEndPos;
            break;
        }
    }
}


double
MSParkingArea::getWidth() const {
    return myWidth;
}


double
MSParkingArea::getLength() const {
    return myLength;
}


double
MSParkingArea::getAngle() const {
    return myAngle;
}


int
MSParkingArea::getCapacity() const {
    return myCapacity;
}


int
MSParkingArea::getOccupancy() const {
    return (int)myEndPositions.size();
}


/****************************************************************************/
