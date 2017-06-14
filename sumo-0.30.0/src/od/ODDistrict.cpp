/****************************************************************************/
/// @file    ODDistrict.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Sept 2002
/// @version $Id: ODDistrict.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A district (origin/destination)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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
#include <string>
#include <utility>
#include <utils/common/UtilExceptions.h>
#include <utils/common/Named.h>
#include <utils/common/MsgHandler.h>
#include "ODDistrict.h"



// ===========================================================================
// method definitions
// ===========================================================================
ODDistrict::ODDistrict(const std::string& id)
    : Named(id) {}


ODDistrict::~ODDistrict() {}


void
ODDistrict::addSource(const std::string& id, double weight) {
    mySources.add(id, weight);
}


void
ODDistrict::addSink(const std::string& id, double weight) {
    mySinks.add(id, weight);
}


std::string
ODDistrict::getRandomSource() const {
    return mySources.get();
}


std::string
ODDistrict::getRandomSink() const {
    return mySinks.get();
}


int
ODDistrict::sinkNumber() const {
    return (int) mySinks.getVals().size();
}


int
ODDistrict::sourceNumber() const {
    return (int) mySources.getVals().size();
}



/****************************************************************************/

