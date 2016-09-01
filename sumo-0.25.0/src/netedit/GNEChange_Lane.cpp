/****************************************************************************/
/// @file    GNEChange_Lane.cpp
/// @author  Jakob Erdmann
/// @date    April 2011
/// @version $Id: GNEChange_Lane.cpp 18830 2015-09-11 08:57:34Z behrisch $
///
// A network change in which a single lane is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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
#include "GNEChange_Lane.h"
#include "GNEEdge.h"
#include "GNELane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Lane, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for creating an edge
GNEChange_Lane::GNEChange_Lane(GNEEdge* edge, GNELane* lane, const NBEdge::Lane& laneAttrs, bool forward):
    GNEChange(0, forward),
    myEdge(edge),
    myLane(lane),
    myLaneAttrs(laneAttrs) {
    myEdge->incRef("GNEChange_Lane");
    if (myLane) { // non-zero pointer is only passsed in case of removal
        assert(!forward);
        myLane->incRef("GNEChange_Lane");
    } else {
        assert(forward);
    }
}


GNEChange_Lane::~GNEChange_Lane() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Lane");
    if (myEdge->unreferenced()) {
        delete myEdge;
    }
    if (myLane) {
        myLane->decRef("GNEChange_Lane");
        if (myLane->unreferenced()) {
            delete myLane;
        }
    }
}


void GNEChange_Lane::undo() {
    if (myForward) {
        myEdge->removeLane(myLane);
    } else {
        myEdge->addLane(myLane, myLaneAttrs);
    }
}


void GNEChange_Lane::redo() {
    if (myForward) {
        myEdge->addLane(myLane, myLaneAttrs);
    } else {
        myEdge->removeLane(myLane);
    }
}


FXString GNEChange_Lane::undoName() const {
    if (myForward) {
        return ("Undo create lane");
    } else {
        return ("Undo delete lane");
    }
}


FXString GNEChange_Lane::redoName() const {
    if (myForward) {
        return ("Redo create lane");
    } else {
        return ("Redo delete lane");
    }
}
