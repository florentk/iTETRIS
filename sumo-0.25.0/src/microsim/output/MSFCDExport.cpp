/****************************************************************************/
/// @file    MSFCDExport.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    2012-04-26
/// @version $Id: MSFCDExport.cpp 19230 2015-10-30 21:59:38Z behrisch $
///
// Realises dumping Floating Car Data (FCD) Data
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2015 DLR (http://www.dlr.de/) and contributors
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

#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include "MSFCDExport.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/pedestrians/MSPerson.h>
#include <microsim/MSPersonControl.h>
#include <microsim/MSContainer.h>
#include <microsim/MSContainerControl.h>
#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSFCDExport::write(OutputDevice& of, SUMOTime timestep, bool elevation) {
    const bool useGeo = OptionsCont::getOptions().getBool("fcd-output.geo");
    const bool signals = OptionsCont::getOptions().getBool("fcd-output.signals");
    of.openTag("timestep").writeAttr(SUMO_ATTR_TIME, time2string(timestep));
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
        const SUMOVehicle* veh = it->second;
        const MSVehicle* microVeh = dynamic_cast<const MSVehicle*>(veh);
        if (veh->isOnRoad()) {
            Position pos = veh->getPosition();
            if (useGeo) {
                of.setPrecision(GEO_OUTPUT_ACCURACY);
                GeoConvHelper::getFinal().cartesian2geo(pos);
            }
            of.openTag(SUMO_TAG_VEHICLE);
            of.writeAttr(SUMO_ATTR_ID, veh->getID());
            of.writeAttr(SUMO_ATTR_X, pos.x());
            of.writeAttr(SUMO_ATTR_Y, pos.y());
            if (elevation) {
                of.writeAttr(SUMO_ATTR_Z, pos.z());
            }
            of.writeAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(veh->getAngle()));
            of.writeAttr(SUMO_ATTR_TYPE, veh->getVehicleType().getID());
            of.writeAttr(SUMO_ATTR_SPEED, veh->getSpeed());
            of.writeAttr(SUMO_ATTR_POSITION, veh->getPositionOnLane());
            if (microVeh != 0) {
                of.writeAttr(SUMO_ATTR_LANE, microVeh->getLane()->getID());
            }
            of.writeAttr(SUMO_ATTR_SLOPE, veh->getSlope());
            if (microVeh != 0 && signals) {
                of.writeAttr("signals", toString(microVeh->getSignals()));
            }
            of.closeTag();
        }
    }
    if (MSNet::getInstance()->getPersonControl().hasPersons()) {
        // write persons
        MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
        const MSEdgeVector& edges = ec.getEdges();
        for (MSEdgeVector::const_iterator e = edges.begin(); e != edges.end(); ++e) {
            const std::vector<MSTransportable*>& persons = (*e)->getSortedPersons(timestep);
            for (std::vector<MSTransportable*>::const_iterator it_p = persons.begin(); it_p != persons.end(); ++it_p) {
                MSTransportable* p = *it_p;
                Position pos = p->getPosition();
                if (useGeo) {
                    of.setPrecision(GEO_OUTPUT_ACCURACY);
                    GeoConvHelper::getFinal().cartesian2geo(pos);
                }
                of.openTag(SUMO_TAG_PERSON);
                of.writeAttr(SUMO_ATTR_ID, p->getID());
                of.writeAttr(SUMO_ATTR_X, pos.x());
                of.writeAttr(SUMO_ATTR_Y, pos.y());
                if (elevation) {
                    of.writeAttr(SUMO_ATTR_Z, pos.z());
                }
                of.writeAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(p->getAngle()));
                of.writeAttr(SUMO_ATTR_SPEED, p->getSpeed());
                of.writeAttr(SUMO_ATTR_POSITION, p->getEdgePos());
                of.writeAttr(SUMO_ATTR_EDGE, (*e)->getID());
                of.writeAttr(SUMO_ATTR_SLOPE, (*e)->getLanes()[0]->getShape().slopeDegreeAtOffset(p->getEdgePos()));
                of.closeTag();
            }
        }
    }
    if (MSNet::getInstance()->getContainerControl().hasContainers()) {
        // write containers
        MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
        const std::vector<MSEdge*>& edges = ec.getEdges();
        for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
            const std::vector<MSTransportable*>& containers = (*e)->getSortedContainers(timestep);
            for (std::vector<MSTransportable*>::const_iterator it_c = containers.begin(); it_c != containers.end(); ++it_c) {
                MSTransportable* c = *it_c;
                Position pos = c->getPosition();
                if (useGeo) {
                    of.setPrecision(GEO_OUTPUT_ACCURACY);
                    GeoConvHelper::getFinal().cartesian2geo(pos);
                }
                of.openTag(SUMO_TAG_CONTAINER);
                of.writeAttr(SUMO_ATTR_ID, c->getID());
                of.writeAttr(SUMO_ATTR_X, pos.x());
                of.writeAttr(SUMO_ATTR_Y, pos.y());
                if (elevation) {
                    of.writeAttr(SUMO_ATTR_Z, pos.z());
                }
                of.writeAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(c->getAngle()));
                of.writeAttr(SUMO_ATTR_SPEED, c->getSpeed());
                of.writeAttr(SUMO_ATTR_POSITION, c->getEdgePos());
                of.writeAttr(SUMO_ATTR_EDGE, (*e)->getID());
                of.writeAttr(SUMO_ATTR_SLOPE, (*e)->getLanes()[0]->getShape().slopeDegreeAtOffset(c->getEdgePos()));
                of.closeTag();
            }
        }
    }
    of.closeTag();
}

/****************************************************************************/
