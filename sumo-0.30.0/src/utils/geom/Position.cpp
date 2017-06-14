/****************************************************************************/
/// @file    Position.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id: Position.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A position in the 2D- or 3D-world
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

#include <limits>
#include "Position.h"


// ===========================================================================
// static member definitions
// ===========================================================================

// Position 1Mio km below the surface should suffice for signaling invalidity inside the solar system
const Position Position::INVALID(
    - 1024 * 1024 * 1024,
    - 1024 * 1024 * 1024,
    - 1024 * 1024 * 1024);
