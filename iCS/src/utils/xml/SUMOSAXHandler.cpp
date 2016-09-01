/****************************************************************************/
/// @file    SUMOSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: SUMOSAXHandler.cpp 10974 2011-07-12 21:21:54Z behrisch $
///
// SAX-handler base for SUMO-files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <iostream>
#include <utils/xml/GenericSAXHandler.h>
#include "SUMOXMLDefinitions.h"
#include "SUMOSAXHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXHandler::SUMOSAXHandler(const std::string &file)
        : GenericSAXHandler(SUMOXMLDefinitions::tags, SUMO_TAG_NOTHING,
                            SUMOXMLDefinitions::attrs, SUMO_ATTR_NOTHING, file) {}


SUMOSAXHandler::~SUMOSAXHandler() {}


/****************************************************************************/

