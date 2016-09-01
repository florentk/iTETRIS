/****************************************************************************/
/// @file    SUMOVehicleParameter.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 31.03.2009
/// @version $Id: SUMOVehicleParameter.cpp 11120 2011-08-19 05:46:19Z behrisch $
///
// Structure representing possible vehicle parameter
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

#include "SUMOVehicleParameter.h"
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
SUMOVehicleParameter::SUMOVehicleParameter() throw()
        : vtypeid(DEFAULT_VTYPE_ID), color(RGBColor::DEFAULT_COLOR),
        depart(-1), departProcedure(DEPART_GIVEN),
        departLane(0), departLaneProcedure(DEPART_LANE_DEFAULT),
        departPos(0), departPosProcedure(DEPART_POS_DEFAULT),
        departSpeed(-1), departSpeedProcedure(DEPART_SPEED_DEFAULT),
        arrivalLane(0), arrivalLaneProcedure(ARRIVAL_LANE_DEFAULT),
        arrivalPos(0), arrivalPosProcedure(ARRIVAL_POS_DEFAULT),
        arrivalSpeed(-1), arrivalSpeedProcedure(ARRIVAL_SPEED_DEFAULT),
        repetitionNumber(-1), repetitionsDone(-1), repetitionOffset(-1),
        line(), fromTaz(), toTaz(), setParameter(0) {
}


bool
SUMOVehicleParameter::defaultOptionOverrides(const OptionsCont &oc, const std::string &optionName) const throw() {
    return oc.isSet(optionName) && oc.getBool("defaults-override");
}


void
SUMOVehicleParameter::writeAs(const std::string &xmlElem, OutputDevice &dev,
                              const OptionsCont &oc) const throw(IOError) {
    dev.openTag(xmlElem) << " id=\"" << id << "\"";
    if (wasSet(VEHPARS_VTYPE_SET)) {
        dev << " type=\"" << vtypeid << "\"";
    }
    dev << " depart=\"" << time2string(depart) << "\"";

    // optional parameter
    //  departlane
    if (wasSet(VEHPARS_DEPARTLANE_SET) && !defaultOptionOverrides(oc, "departlane")) {
        std::string val;
        switch (departLaneProcedure) {
        case DEPART_LANE_GIVEN:
            val = toString(departLane);
            break;
        case DEPART_LANE_RANDOM:
            val = "random";
            break;
        case DEPART_LANE_FREE:
            val = "free";
            break;
        case DEPART_LANE_ALLOWED_FREE:
            val = "allowed";
            break;
        case DEPART_LANE_BEST_FREE:
            val = "best";
            break;
        case DEPART_LANE_DEFAULT:
        default:
            break;
        }
        dev << " departLane=\"" << val << "\"";
    } else if (oc.isSet("departlane")) {
        dev << " departLane=\"" << oc.getString("departlane") << "\"";
    }
    //  departpos
    if (wasSet(VEHPARS_DEPARTPOS_SET) && !defaultOptionOverrides(oc, "departpos")) {
        std::string val;
        switch (departPosProcedure) {
        case DEPART_POS_GIVEN:
            val = toString(departPos);
            break;
        case DEPART_POS_RANDOM:
            val = "random";
            break;
        case DEPART_POS_RANDOM_FREE:
            val = "random_free";
            break;
        case DEPART_POS_FREE:
            val = "free";
            break;
        case DEPART_POS_PWAG_SIMPLE:
            val = "pwagSimple";
            break;
        case DEPART_POS_PWAG_GENERIC:
            val = "pwagGeneric";
            break;
        case DEPART_POS_MAX_SPEED_GAP:
            val = "maxSpeedGap";
            break;
        case DEPART_POS_BASE:
            val = "base";
            break;
        case DEPART_POS_DEFAULT:
        default:
            break;
        }
        dev << " departPos=\"" << val << "\"";
    } else if (oc.isSet("departpos")) {
        dev << " departPos=\"" << oc.getString("departpos") << "\"";
    }
    //  departspeed
    if (wasSet(VEHPARS_DEPARTSPEED_SET) && !defaultOptionOverrides(oc, "departspeed")) {
        std::string val;
        switch (departSpeedProcedure) {
        case DEPART_SPEED_GIVEN:
            val = toString(departSpeed);
            break;
        case DEPART_SPEED_RANDOM:
            val = "random";
            break;
        case DEPART_SPEED_MAX:
            val = "max";
            break;
        case DEPART_SPEED_DEFAULT:
        default:
            break;
        }
        dev << " departSpeed=\"" << val << "\"";
    } else if (oc.isSet("departspeed")) {
        dev << " departSpeed=\"" << oc.getString("departspeed") << "\"";
    }

    //  arrivallane
    if (wasSet(VEHPARS_ARRIVALLANE_SET) && !defaultOptionOverrides(oc, "arrivallane")) {
        std::string val;
        switch (arrivalLaneProcedure) {
        case ARRIVAL_LANE_GIVEN:
            val = toString(arrivalLane);
            break;
        case ARRIVAL_LANE_CURRENT:
            val = "current";
            break;
        case ARRIVAL_LANE_DEFAULT:
        default:
            break;
        }
        dev << " arrivalLane=\"" << val << "\"";
    } else if (oc.isSet("arrivallane")) {
        dev << " arrivalLane=\"" << oc.getString("arrivallane") << "\"";
    }
    //  arrivalpos
    if (wasSet(VEHPARS_ARRIVALPOS_SET) && !defaultOptionOverrides(oc, "arrivalpos")) {
        std::string val;
        switch (arrivalPosProcedure) {
        case ARRIVAL_POS_GIVEN:
            val = toString(arrivalPos);
            break;
        case ARRIVAL_POS_RANDOM:
            val = "random";
            break;
        case ARRIVAL_POS_MAX:
            val = "max";
            break;
        case ARRIVAL_POS_DEFAULT:
        default:
            break;
        }
        dev << " arrivalPos=\"" << val << "\"";
    } else if (oc.isSet("arrivalpos")) {
        dev << " arrivalPos=\"" << oc.getString("arrivalpos") << "\"";
    }
    //  arrivalspeed
    if (wasSet(VEHPARS_ARRIVALSPEED_SET) && !defaultOptionOverrides(oc, "arrivalspeed")) {
        std::string val;
        switch (arrivalSpeedProcedure) {
        case ARRIVAL_SPEED_GIVEN:
            val = toString(arrivalSpeed);
            break;
        case ARRIVAL_SPEED_CURRENT:
            val = "current";
            break;
        case ARRIVAL_SPEED_DEFAULT:
        default:
            break;
        }
        dev << " arrivalSpeed=\"" << val << "\"";
    } else if (oc.isSet("arrivalspeed")) {
        dev << " arrivalSpeed=\"" << oc.getString("arrivalspeed") << "\"";
    }

    // color
    if (wasSet(VEHPARS_COLOR_SET)) {
        dev << " color=\"" << color << "\"";
    }
    // repetition values
    if (wasSet(VEHPARS_PERIODNUM_SET)) {
        dev << " repno=\"" << repetitionNumber << "\"";
    }
    if (wasSet(VEHPARS_PERIODFREQ_SET)) {
#ifdef HAVE_SUBSECOND_TIMESTEPS
        dev << " period=\"" << time2string(repetitionOffset) << "\"";
#else
        dev << " period=\"" << repetitionOffset << "\"";
#endif
    }
    if (wasSet(VEHPARS_LINE_SET)) {
        dev << " line=\"" << line << "\"";
    }
    if (wasSet(VEHPARS_TAZ_SET)) {
        dev << " fromTaz=\"" << fromTaz << "\" toTaz=\"" << toTaz << "\"";
    }
    dev << ">\n";
}


bool
SUMOVehicleParameter::departlaneValidate(const std::string &val) throw() {
    if (val=="random"||val=="free"||val=="departlane"||val=="allowed"||val=="best") {
        return true;
    }
    try {
        TplConvert<char>::_2int(val.c_str());
        return true;
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
    }
    WRITE_ERROR("Invalid departlane definition;\n must be one of (\"departlane\", \"random\", \"free\", \"allowed\", \"best\", or an int>0)");
    return false;
}


bool
SUMOVehicleParameter::departposValidate(const std::string &val) throw() {
    if (val=="random"||val=="free"||val=="random_free"||val=="base"||val=="pwagSimple"||val=="pwagGeneric"||val=="maxSpeedGap") {
        return true;
    }
    try {
        TplConvert<char>::_2SUMOReal(val.c_str());
        return true;
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
    }
    WRITE_ERROR("Invalid departpos definition;\n must be one of (\"random\", \"random_free\", \"free\", \"base\", \"pwagSimple\", \"pwagGeneric\", \"maxSpeedGap\", or a float)");
    return false;
}


bool
SUMOVehicleParameter::departspeedValidate(const std::string &val) throw() {
    if (val=="random"||val=="max") {
        return true;
    }
    try {
        TplConvert<char>::_2SUMOReal(val.c_str());
        return true;
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
    }
    WRITE_ERROR("Invalid departspeed definition;\n must be one of (\"random\", \"max\", or a float>0)");
    return false;
}


bool
SUMOVehicleParameter::arrivallaneValidate(const std::string &val) throw() {
    if (val=="current") {
        return true;
    }
    try {
        TplConvert<char>::_2int(val.c_str());
        return true;
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
    }
    WRITE_ERROR("Invalid arrivallane definition;\n must be one of (\"current\", or int>0)");
    return false;
}


bool
SUMOVehicleParameter::arrivalposValidate(const std::string &val) throw() {
    if (val=="random"||val=="max") {
        return true;
    }
    try {
        TplConvert<char>::_2SUMOReal(val.c_str());
        return true;
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
    }
    WRITE_ERROR("Invalid arrivalpos definition;\n must be one of (\"random\", \"max\", or a float)");
    return false;
}


bool
SUMOVehicleParameter::arrivalspeedValidate(const std::string &val) throw() {
    if (val=="current") {
        return true;
    }
    try {
        TplConvert<char>::_2SUMOReal(val.c_str());
        return true;
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
    }
    WRITE_ERROR("Invalid arrivalspeed definition;\n must be one of (\"current\", or a float>0)");
    return false;
}


/****************************************************************************/

