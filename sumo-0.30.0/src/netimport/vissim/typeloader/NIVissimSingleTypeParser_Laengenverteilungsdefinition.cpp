/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Laengenverteilungsdefinition.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 18 Dec 2002
/// @version $Id: NIVissimSingleTypeParser_Laengenverteilungsdefinition.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
//
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

#include <iostream>
#include <utils/common/TplConvert.h>
#include <utils/geom/PositionVector.h>
#include "../NIImporter_Vissim.h"
#include <utils/distribution/Distribution_Points.h>
#include <utils/distribution/DistributionCont.h>
#include "NIVissimSingleTypeParser_Laengenverteilungsdefinition.h"


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimSingleTypeParser_Laengenverteilungsdefinition::NIVissimSingleTypeParser_Laengenverteilungsdefinition(NIImporter_Vissim& parent)
    : NIImporter_Vissim::VissimSingleTypeParser(parent) {}


NIVissimSingleTypeParser_Laengenverteilungsdefinition::~NIVissimSingleTypeParser_Laengenverteilungsdefinition() {}


bool
NIVissimSingleTypeParser_Laengenverteilungsdefinition::parse(std::istream& from) {
    // id
    std::string id;
    from >> id;
    // list of points
    Distribution_Points* points = new Distribution_Points(id);
    std::string tag;
    do {
        tag = readEndSecure(from);
        if (tag != "DATAEND") {
            double p1 = TplConvert::_2double(tag.c_str());
            from >> tag;
            double p2 = TplConvert::_2double(tag.c_str());
            points->add(p1, p2);
        }
    } while (tag != "DATAEND");
    DistributionCont::dictionary("length", id, points);
    return true;
}



/****************************************************************************/

