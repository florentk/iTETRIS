/****************************************************************************/
/// @file    GUIPersonControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 13.06.2012
/// @version $Id: GUIPersonControl.cpp 18274 2015-04-22 15:00:44Z behrisch $
///
// GUI-version of the person control for building gui persons
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

#include <vector>
#include <algorithm>
#include "GUINet.h"
#include "GUIPersonControl.h"
#include "GUIPerson.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIPersonControl::GUIPersonControl() {}


GUIPersonControl::~GUIPersonControl() {
}


MSPerson*
GUIPersonControl::buildPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) const {
    return new GUIPerson(pars, vtype, plan);
}


void
GUIPersonControl::insertPersonIDs(std::vector<GUIGlID>& into) {
    into.reserve(myPersons.size());
    for (std::map<std::string, MSTransportable*>::const_iterator it = myPersons.begin(); it != myPersons.end(); ++it) {
        into.push_back(static_cast<const GUIPerson*>((*it).second)->getGlID());
    }
}


/****************************************************************************/
