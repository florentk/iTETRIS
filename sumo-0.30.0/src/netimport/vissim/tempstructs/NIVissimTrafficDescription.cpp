/****************************************************************************/
/// @file    NIVissimTrafficDescription.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: NIVissimTrafficDescription.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// -------------------
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

#include <string>
#include <map>
#include <cassert>
#include "NIVissimVehicleClassVector.h"
#include "NIVissimTrafficDescription.h"


// ===========================================================================
// member function definitions
// ===========================================================================
NIVissimTrafficDescription::DictType NIVissimTrafficDescription::myDict;


// ===========================================================================
// member method definitions
// ===========================================================================
NIVissimTrafficDescription::NIVissimTrafficDescription(
    const std::string& name,
    const NIVissimVehicleClassVector& vehicleTypes)
    : myName(name), myVehicleTypes(vehicleTypes) {}


NIVissimTrafficDescription::~NIVissimTrafficDescription() {
    for (NIVissimVehicleClassVector::iterator i = myVehicleTypes.begin(); i != myVehicleTypes.end(); i++) {
        delete *i;
    }
    myVehicleTypes.clear();
}


bool
NIVissimTrafficDescription::dictionary(int id,
                                       const std::string& name,
                                       const NIVissimVehicleClassVector& vehicleTypes) {
    NIVissimTrafficDescription* o = new NIVissimTrafficDescription(name, vehicleTypes);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimTrafficDescription::dictionary(int id, NIVissimTrafficDescription* o) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimTrafficDescription*
NIVissimTrafficDescription::dictionary(int id) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimTrafficDescription::clearDict() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}




double
NIVissimTrafficDescription::meanSpeed(int id) {
    NIVissimTrafficDescription* i = dictionary(id);
    assert(i != 0);
    return i->meanSpeed();
}


double
NIVissimTrafficDescription::meanSpeed() const {
    double speed = 0;
    for (NIVissimVehicleClassVector::const_iterator i = myVehicleTypes.begin(); i != myVehicleTypes.end(); i++) {
        speed += (*i)->getSpeed();
    }
    return speed / (double) myVehicleTypes.size();
}



/****************************************************************************/

