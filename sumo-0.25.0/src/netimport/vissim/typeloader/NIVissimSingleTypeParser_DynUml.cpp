/****************************************************************************/
/// @file    NIVissimSingleTypeParser_DynUml.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id: NIVissimSingleTypeParser_DynUml.cpp 18095 2015-03-17 09:39:00Z behrisch $
///
//
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

#include <iostream>
#include <vector>
#include <utils/common/TplConvert.h>
#include "../NIImporter_Vissim.h"
#include "NIVissimSingleTypeParser_DynUml.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_DynUml::NIVissimSingleTypeParser_DynUml(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_DynUml::~NIVissimSingleTypeParser_DynUml() {}


bool
NIVissimSingleTypeParser_DynUml::parse(std::istream& from) {
    std::vector<std::string> tmp;
    tmp.push_back("reisezeit");
    tmp.push_back("kante");
    readUntil(from, "kirchhoffexponent");
    std::string tag = readEndSecure(from, tmp);
    while (tag != "DATAEND") {
        tag = readEndSecure(from, tmp);
    }
    return true;
}



/****************************************************************************/

