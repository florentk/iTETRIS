/****************************************************************************/
/// @file    GUIGlobals.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id: GUIGlobals.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// }
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

#include "GUIGlobals.h"


// ===========================================================================
// static member variables definitions
// ===========================================================================
bool GUIGlobals::gRunAfterLoad = false;


bool GUIGlobals::gQuitOnEnd = false;


bool GUIGlobals::gDemoAutoReload = false;


/****************************************************************************/
