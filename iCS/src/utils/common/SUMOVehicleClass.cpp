/****************************************************************************/
/// @file    SUMOVehicleClass.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-24
/// @version $Id: SUMOVehicleClass.cpp 11284 2011-09-23 10:37:35Z namdre $
///
// Definitions of SUMO vehicle classes and helper functions
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
#include <map>
#include "SUMOVehicleClass.h"
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
StringBijection<SUMOVehicleClass>::Entry sumoVehicleClassStringInitializer[] = {
    {"unknown",           SVC_UNKNOWN},
    {"private",           SVC_PRIVATE},
    {"public_transport",  SVC_PUBLIC_TRANSPORT},
    {"public_emergency",  SVC_PUBLIC_EMERGENCY},
    {"public_authority",  SVC_PUBLIC_AUTHORITY},
    {"public_army",       SVC_PUBLIC_ARMY},
    {"vip",               SVC_VIP},
    {"ignoring",          SVC_IGNORING},
    {"passenger",         SVC_PASSENGER},
    {"hov",               SVC_HOV},
    {"taxi",              SVC_TAXI},
    {"bus",               SVC_BUS},
    {"delivery",          SVC_DELIVERY},
    {"transport",         SVC_TRANSPORT},
    {"lightrail",         SVC_LIGHTRAIL},
    {"cityrail",          SVC_CITYRAIL},
    {"rail_slow",         SVC_RAIL_SLOW},
    {"rail_fast",         SVC_RAIL_FAST},
    {"motorcycle",        SVC_MOTORCYCLE},
    {"bicycle",           SVC_BICYCLE},
    {"pedestrian",        SVC_PEDESTRIAN}
};

StringBijection<SUMOVehicleClass> SumoVehicleClassStrings(
    sumoVehicleClassStringInitializer, SVC_PEDESTRIAN);


StringBijection<SUMOVehicleShape>::Entry sumoVehicleShapeStringInitializer[] = {
    {"pedestrian",            SVS_PEDESTRIAN},
    {"bicycle",               SVS_BICYCLE},
    {"motorcycle",            SVS_MOTORCYCLE},
    {"passenger",             SVS_PASSENGER},
    {"passenger/sedan",       SVS_PASSENGER_SEDAN},
    {"passenger/hatchback",   SVS_PASSENGER_HATCHBACK},
    {"passenger/wagon",       SVS_PASSENGER_WAGON},
    {"passenger/van",         SVS_PASSENGER_VAN},
    {"delivery",              SVS_DELIVERY},
    {"transport",             SVS_TRANSPORT},
    {"transport/semitrailer", SVS_TRANSPORT_SEMITRAILER},
    {"transport/trailer",     SVS_TRANSPORT_1TRAILER},
    {"bus",                   SVS_BUS},
    {"bus/city",              SVS_BUS_CITY},
    {"bus/flexible",          SVS_BUS_CITY_FLEXIBLE},
    {"bus/overland",          SVS_BUS_OVERLAND},
    {"bus/trolley",           SVS_BUS_TROLLEY},
    {"rail",                  SVS_RAIL},
    {"rail/light",            SVS_RAIL_LIGHT},
    {"rail/city",             SVS_RAIL_CITY},
    {"rail/slow",             SVS_RAIL_SLOW},
    {"rail/fast",             SVS_RAIL_FAST},
    {"rail/cargo",            SVS_RAIL_CARGO},
    {"evehicle",              SVS_E_VEHICLE},
    {"ant",                   SVS_ANT},
    {"",                      SVS_UNKNOWN}
};


StringBijection<SUMOVehicleShape> SumoVehicleShapeStrings(
    sumoVehicleShapeStringInitializer, SVS_UNKNOWN);


// ===========================================================================
// method definitions
// ===========================================================================
// ------------ Conversion of SUMOVehicleClass

std::string
getVehicleClassCompoundName(int id) {
    std::string ret;
    const std::vector<std::string> names = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        if ((id & SumoVehicleClassStrings.get(*it))) {
            ret += ("|" + *it);
        }
    }
    if (ret.length()>0) {
        return ret.substr(1);
    } else {
        return ret;
    }
}


std::string
getVehicleClassNames(const SUMOVehicleClasses &ids) {
    std::ostringstream oss;
    bool hadOne = false;
    for (SUMOVehicleClasses::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
        if (hadOne) {
            oss << ' ';
        }
        oss << toString(*i);
        hadOne = true;
    }
    return oss.str();
}


SUMOVehicleClass
getVehicleClassID(const std::string &name) {
    if (SumoVehicleClassStrings.hasString(name)) {
        return SumoVehicleClassStrings.get(name);
    }
    throw ProcessError("Unknown vehicle class '" + name + "'.");
}


int
getVehicleClassCompoundID(const std::string &name) {
    int ret = SVC_UNKNOWN;
    const std::vector<std::string> names = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        if (name.find(*it) != std::string::npos) {
            ret = ret | (int) SumoVehicleClassStrings.get(*it);
        }
    }
    return ret;
}


void
parseVehicleClasses(
    const std::string &classNames,
    SUMOVehicleClasses &container) {
    StringTokenizer sta(classNames, " ");
    while (sta.hasNext()) {
        container.insert(getVehicleClassID(sta.next()));
    }
}


void
parseVehicleClasses(const std::string &allowedS,
                    const std::string &disallowedS,
                    SUMOVehicleClasses &allowed,
                    SUMOVehicleClasses &disallowed) {
    parseVehicleClasses(allowedS, allowed);
    parseVehicleClasses(disallowedS, disallowed);
}


void
parseVehicleClasses(const std::vector<std::string> &classesS,
                    SUMOVehicleClasses &classes) {
    for (std::vector<std::string>::const_iterator i=classesS.begin(); i!=classesS.end(); ++i) {
        classes.insert(getVehicleClassID(*i));
    }
}


SUMOVehicleShape
getVehicleShapeID(const std::string &name) {
    if (SumoVehicleShapeStrings.hasString(name)) {
        return SumoVehicleShapeStrings.get(name);
    } else {
        throw ProcessError("Unknown vehicle shape '" + name + "'.");
    }
}


std::string 
getVehicleShapeName(SUMOVehicleShape id) {
    return SumoVehicleShapeStrings.getString(id);
}


// ------------ Conversion of SUMOEmissionClass
SUMOEmissionClass
getVehicleEmissionTypeID(const std::string &name) throw(ProcessError) {
    try {
        if (name=="") {
            return SVE_UNKNOWN;
        } else if (name=="zero") {
            return SVE_ZERO_EMISSIONS;
        } else if (name.find("HDV_3_")==0) {
            return (SUMOEmissionClass)(SVE_HDV_3_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        } else if (name.find("HDV_6_")==0) {
            return (SUMOEmissionClass)(SVE_HDV_6_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        } else if (name.find("HDV_12_")==0) {
            return (SUMOEmissionClass)(SVE_HDV_12_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        } else if (name.find("P_7_")==0) {
            return (SUMOEmissionClass)(SVE_P_LDV_7_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        } else if (name.find("P_14_")==0) {
            return (SUMOEmissionClass)(SVE_P_LDV_14_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        }
    } catch (NumberFormatException &) {
    }
    throw ProcessError("Unknown emission type '" + name + "'.");
}


std::string
getVehicleEmissionTypeName(SUMOEmissionClass id) {
    if (id==SVE_ZERO_EMISSIONS) {
        return "zero";
    }
    if (id<0) {
        return "";
    } else if (id<3) {
        return "HDV_3_" + toString(int(id));
    } else if (id<3+6) {
        return "HDV_6_" + toString(int(id-3));
    } else if (id<3+6+12) {
        return "HDV_12_" + toString(int(id-3-6));
    } else if (id<3+6+12+7) {
        return "P_7_" + toString(int(id-3-6-12));
    } else if (id<3+6+12+7+14) {
        return "P_14_" + toString(int(id-3-6-12-7));
    }
    return "";
}

const std::string DEFAULT_VTYPE_ID("DEFAULT_VEHTYPE");
const SUMOReal DEFAULT_VEH_MAXSPEED(70.0);
const SUMOReal DEFAULT_VEH_ACCEL(2.6);
const SUMOReal DEFAULT_VEH_DECEL(4.5);
const SUMOReal DEFAULT_VEH_SIGMA(0.5);
const SUMOReal DEFAULT_VEH_LENGTH(5.);
const SUMOReal DEFAULT_VEH_MINGAP(2.5);
const SUMOReal DEFAULT_VEH_TAU(1.);
const SUMOVehicleClass DEFAULT_VEH_CLASS(SVC_UNKNOWN);
const SUMOReal DEFAULT_VEH_PROB(1.);
const SUMOReal DEFAULT_VEH_SPEEDFACTOR(1.);
const SUMOReal DEFAULT_VEH_SPEEDDEV(0.);
const SUMOReal DEFAULT_VEH_GUIWIDTH(2.);
const SumoXMLTag DEFAULT_VEH_FOLLOW_MODEL(SUMO_TAG_CF_KRAUSS);
const std::string DEFAULT_VEH_LANE_CHANGE_MODEL("dkrajzew2008");
const SUMOVehicleShape DEFAULT_VEH_SHAPE(SVS_UNKNOWN);

/****************************************************************************/

