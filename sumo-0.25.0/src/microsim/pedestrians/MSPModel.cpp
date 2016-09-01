/****************************************************************************/
/// @file    MSPModel.h
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
/// @version $Id: MSPModel.cpp 18095 2015-03-17 09:39:00Z behrisch $
///
// The pedestrian following model (prototype)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2015 DLR (http://www.dlr.de/) and contributors
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

#include <math.h>
#include <algorithm>
#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLane.h>
#include "MSPModel_Striping.h"
#include "MSPModel_NonInteracting.h"
#include "MSPModel.h"




// ===========================================================================
// static members
// ===========================================================================
MSPModel* MSPModel::myModel(0);

// named constants
const int MSPModel::FORWARD(1);
const int MSPModel::BACKWARD(-1);
const int MSPModel::UNDEFINED_DIRECTION(0);

// parameters shared by all models
const SUMOReal MSPModel::SAFETY_GAP(1.0);

const SUMOReal MSPModel::SIDEWALK_OFFSET(3);

// ===========================================================================
// MSPModel method definitions
// ===========================================================================


MSPModel*
MSPModel::getModel() {
    if (myModel == 0) {
        const OptionsCont& oc = OptionsCont::getOptions();
        MSNet* net = MSNet::getInstance();
        const std::string model = oc.getString("pedestrian.model");
        if (model == "striping") {
            myModel = new MSPModel_Striping(oc, net);
        } else if (model == "nonInteracting") {
            myModel = new MSPModel_NonInteracting(oc, net);
        } else {
            throw ProcessError("Unknown pedestrian model '" + model + "'");
        }
    }
    return myModel;
}


void
MSPModel::cleanup() {
    if (myModel != 0) {
        myModel->cleanupHelper();
        delete myModel;
        myModel = 0;
    }
}


MSLane*
MSPModel::getSidewalk(const MSEdge* edge) {
    if (edge == 0) {
        return 0;
    }
    assert(edge->getLanes().size() > 0);
    const std::vector<MSLane*>& lanes = edge->getLanes();
    for (std::vector<MSLane*>::const_iterator it = lanes.begin(); it != lanes.end(); ++it) {
        if ((*it)->allowsVehicleClass(SVC_PEDESTRIAN)) {
            return *it;
        }
    }
    return lanes.front();
}


bool
MSPModel::canTraverse(int dir, const ConstMSEdgeVector& route) {
    const MSJunction* junction = 0;
    for (ConstMSEdgeVector::const_iterator it = route.begin(); it != route.end(); ++it) {
        const MSEdge* edge = *it;
        if (junction != 0) {
            //std::cout << " junction=" << junction->getID() << " edge=" << edge->getID() << "\n";
            if (junction == edge->getFromJunction()) {
                dir = FORWARD;
            } else if (junction == edge->getToJunction()) {
                dir = BACKWARD;
            } else {
                return false;
            }
        }
        junction = dir == FORWARD ? edge->getToJunction() : edge->getFromJunction();
    }
    return true;
}

/****************************************************************************/
