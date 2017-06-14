/****************************************************************************/
/// @file    RONode.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id: RONode.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A single router's node
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
#include "RONode.h"


// ===========================================================================
// method definitions
// ===========================================================================
RONode::RONode(const std::string& id)
    : Named(id) {}


RONode::~RONode() {}


void
RONode::setPosition(const Position& p) {
    myPosition = p;
}



/****************************************************************************/

