/****************************************************************************/
/// @file    RODUAEdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @date    Tue, 20 Jan 2004
/// @version $Id: RODUAEdgeBuilder.cpp 18394 2015-05-18 13:26:24Z behrisch $
///
// Interface for building instances of duarouter-edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2015 DLR (http://www.dlr.de/) and contributors
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

#include "RODUAEdgeBuilder.h"
#include <router/ROEdge.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RODUAEdgeBuilder::RODUAEdgeBuilder() {
}


RODUAEdgeBuilder::~RODUAEdgeBuilder() {}


ROEdge*
RODUAEdgeBuilder::buildEdge(const std::string& name, RONode* from, RONode* to, const int priority) {
    return new ROEdge(name, from, to, getNextIndex(), priority);
}


/****************************************************************************/

