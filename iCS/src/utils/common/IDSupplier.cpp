/****************************************************************************/
/// @file    IDSupplier.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: IDSupplier.cpp 11180 2011-08-29 06:11:20Z namdre $
///
// A class that generates enumerated and prefixed string-ids
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
#include <sstream>
#include "StdDefs.h"
#include "IDSupplier.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
IDSupplier::IDSupplier(const std::string &prefix, long begin)
        : myCurrent(begin), myPrefix(prefix) {}



IDSupplier::IDSupplier(const std::string &prefix, const std::vector<std::string> &knownIDs)
        : myCurrent(0), myPrefix(prefix) {
    for (std::vector<std::string>::const_iterator id_it = knownIDs.begin(); id_it != knownIDs.end(); ++id_it) {
        avoid(*id_it);
    }
}


IDSupplier::~IDSupplier() {}


std::string
IDSupplier::getNext() {
    std::ostringstream strm;
    strm << myPrefix << myCurrent++;
    return strm.str();
}


void
IDSupplier::avoid(const std::string& id) {
    // does it start with prefix?
    if (id.find(myPrefix) == 0) {
        long number;
        std::istringstream buf(id.substr(myPrefix.size(), std::string::npos));
        buf >> number;
        // does it continue with a number?
        if (!buf.fail()) {
            myCurrent = MAX2(myCurrent, number+1);
        }
    }
}


/****************************************************************************/

