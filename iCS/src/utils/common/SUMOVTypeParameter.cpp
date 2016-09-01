/****************************************************************************/
/// @file    SUMOVTypeParameter.cpp
/// @author  Daniel Krajzewicz
/// @date    10.09.2009
/// @version $Id: SUMOVTypeParameter.cpp 10969 2011-07-12 15:15:19Z namdre $
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

#include <algorithm>
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOXMLDefinitions.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
SUMOVTypeParameter::SUMOVTypeParameter() throw()
        : id(DEFAULT_VTYPE_ID), length(DEFAULT_VEH_LENGTH),
        minGap(DEFAULT_VEH_MINGAP), maxSpeed(DEFAULT_VEH_MAXSPEED),
        defaultProbability(DEFAULT_VEH_PROB),
        speedFactor(DEFAULT_VEH_SPEEDFACTOR), speedDev(DEFAULT_VEH_SPEEDDEV),
        emissionClass(SVE_UNKNOWN), color(RGBColor::DEFAULT_COLOR),
        vehicleClass(SVC_UNKNOWN), width(DEFAULT_VEH_GUIWIDTH),
        shape(DEFAULT_VEH_SHAPE),
        cfModel(DEFAULT_VEH_FOLLOW_MODEL), lcModel(DEFAULT_VEH_LANE_CHANGE_MODEL),
        setParameter(0), saved(false), onlyReferenced(false) {
}


void
SUMOVTypeParameter::write(OutputDevice &dev) const throw(IOError) {
    if (onlyReferenced) {
        return;
    }
    dev.openTag(SUMO_TAG_VTYPE);
    dev.writeAttr(SUMO_ATTR_ID, id);
    if (wasSet(VTYPEPARS_LENGTH_SET)) {
        dev.writeAttr(SUMO_ATTR_LENGTH, length);
    }
    if (wasSet(VTYPEPARS_MINGAP_SET)) {
        dev.writeAttr(SUMO_ATTR_MINGAP, minGap);
    }
    if (wasSet(VTYPEPARS_MAXSPEED_SET)) {
        dev.writeAttr(SUMO_ATTR_MAXSPEED, maxSpeed);
    }
    if (wasSet(VTYPEPARS_PROBABILITY_SET)) {
        dev.writeAttr(SUMO_ATTR_PROB, defaultProbability);
    }
    if (wasSet(VTYPEPARS_SPEEDFACTOR_SET)) {
        dev.writeAttr(SUMO_ATTR_SPEEDFACTOR, speedFactor);
    }
    if (wasSet(VTYPEPARS_SPEEDDEVIATION_SET)) {
        dev.writeAttr(SUMO_ATTR_SPEEDDEV, speedDev);
    }
    if (wasSet(VTYPEPARS_VEHICLECLASS_SET)) {
        dev.writeAttr(SUMO_ATTR_VCLASS, toString(vehicleClass));
    }
    if (wasSet(VTYPEPARS_EMISSIONCLASS_SET)) {
        dev.writeAttr(SUMO_ATTR_EMISSIONCLASS, getVehicleEmissionTypeName(emissionClass));
    }
    if (wasSet(VTYPEPARS_SHAPE_SET)) {
        dev.writeAttr(SUMO_ATTR_GUISHAPE, getVehicleShapeName(shape));
    }
    if (wasSet(VTYPEPARS_WIDTH_SET)) {
        dev.writeAttr(SUMO_ATTR_GUIWIDTH, width);
    }
    if (wasSet(VTYPEPARS_COLOR_SET)) {
        dev.writeAttr(SUMO_ATTR_COLOR, color);
    }

    if (cfParameter.size()!=0) {
        dev << ">\n";
        dev.openTag(cfModel);
        std::vector<SumoXMLAttr> attrs;
        for (CFParams::const_iterator i=cfParameter.begin(); i!=cfParameter.end(); ++i) {
            attrs.push_back(i->first);
        }
        std::sort(attrs.begin(), attrs.end());
        for (std::vector<SumoXMLAttr>::const_iterator i=attrs.begin(); i!=attrs.end(); ++i) {
            dev.writeAttr(*i, cfParameter.find(*i)->second);
        }
        dev.closeTag(true);
        dev.closeTag();
    } else {
        dev.closeTag(true);
    }
}


/****************************************************************************/

