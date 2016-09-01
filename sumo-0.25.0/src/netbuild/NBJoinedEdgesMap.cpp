/****************************************************************************/
/// @file    NBJoinedEdgesMap.cpp
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
/// @version $Id: NBJoinedEdgesMap.cpp 18095 2015-03-17 09:39:00Z behrisch $
///
// A structure storing information about which edges were joined
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

#include <cassert>
#include <iostream>
#include "NBJoinedEdgesMap.h"
#include "NBEdgeCont.h"
#include "NBEdge.h"
#include <algorithm>
#include <iterator>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
NBJoinedEdgesMap::NBJoinedEdgesMap() {}


NBJoinedEdgesMap::~NBJoinedEdgesMap() {}


void
NBJoinedEdgesMap::init(NBEdgeCont& ec) {
    const std::vector<std::string> edgeNames = ec.getAllNames();
    myMap.clear();
    for (std::vector<std::string>::const_iterator i = edgeNames.begin(); i != edgeNames.end(); i++) {
        MappedEdgesVector e;
        e.push_back(*i);
        myMap[*i] = e;
        myLengths[*i] = ec.retrieve(*i)->getLength();
    }
}


void
NBJoinedEdgesMap::appended(const std::string& to, const std::string& what) {
    copy(myMap[what].begin(), myMap[what].end(), back_inserter(myMap[to]));
    JoinedEdgesMap::iterator i = myMap.find(what);
    assert(i != myMap.end());
    myMap.erase(i);
}


std::ostream&
operator<<(std::ostream& os, const NBJoinedEdgesMap& jemap) {
    NBJoinedEdgesMap::JoinedEdgesMap::const_iterator i;
    for (i = jemap.myMap.begin(); i != jemap.myMap.end(); ++i) {
        os << (*i).first << "\t";
        const NBJoinedEdgesMap::MappedEdgesVector& e = (*i).second;
        for (NBJoinedEdgesMap::MappedEdgesVector::const_iterator j = e.begin(); j != e.end(); ++j) {
            os << (*j) << ":" << jemap.myLengths.find(*j)->second << "\t";
        }
        os << std::endl;
    }
    return os;
}



/****************************************************************************/

