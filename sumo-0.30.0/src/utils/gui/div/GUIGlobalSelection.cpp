/****************************************************************************/
/// @file    GUIGlobalSelection.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id: GUIGlobalSelection.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A global holder of selected objects
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

#include "GUIGlobalSelection.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUISelectedStorage.h>

#include <algorithm>


// ===========================================================================
// global variables definitions
// ===========================================================================
GUISelectedStorage gSelected;



/****************************************************************************/

