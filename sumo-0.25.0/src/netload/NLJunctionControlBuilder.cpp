/****************************************************************************/
/// @file    NLJunctionControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id: NLJunctionControlBuilder.cpp 19535 2015-12-05 13:47:18Z behrisch $
///
// Builder of microsim-junctions and tls
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

#include <map>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <microsim/MSJunctionLogic.h>
#include <microsim/MSNoLogicJunction.h>
#include <microsim/MSRightOfWayJunction.h>
#include <microsim/MSInternalJunction.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/traffic_lights/MSRailSignal.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSOffTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <netbuild/NBNode.h>
#include "NLBuilder.h"
#include "NLJunctionControlBuilder.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
const int NLJunctionControlBuilder::NO_REQUEST_SIZE = -1;

// ===========================================================================
// method definitions
// ===========================================================================
NLJunctionControlBuilder::NLJunctionControlBuilder(MSNet& net, NLDetectorBuilder& db) :
    myNet(net),
    myDetectorBuilder(db),
    myOffset(0),
    myJunctions(0),
    myNetIsLoaded(false) {
    myLogicControl = new MSTLLogicControl();
    myJunctions = new MSJunctionControl();
}


NLJunctionControlBuilder::~NLJunctionControlBuilder() {
    delete myLogicControl;
    delete myJunctions;
}


void
NLJunctionControlBuilder::openJunction(const std::string& id,
                                       const std::string& key,
                                       const SumoXMLNodeType type,
                                       SUMOReal x, SUMOReal y,
                                       const PositionVector& shape,
                                       const std::vector<MSLane*>& incomingLanes,
                                       const std::vector<MSLane*>& internalLanes) {
#ifdef HAVE_INTERNAL_LANES
    myActiveInternalLanes = internalLanes;
#else
    UNUSED_PARAMETER(internalLanes);
#endif
    myActiveIncomingLanes = incomingLanes;
    myActiveID = id;
    myActiveKey = key;
    myType = type;
    myPosition.set(x, y);
    myShape = shape;
}


void
NLJunctionControlBuilder::closeJunction(const std::string& basePath) {
    if (myJunctions == 0) {
        throw ProcessError("Information about the number of nodes was missing.");
    }
    MSJunction* junction = 0;
    switch (myType) {
        case NODETYPE_NOJUNCTION:
        case NODETYPE_DEAD_END:
        case NODETYPE_DEAD_END_DEPRECATED:
        case NODETYPE_DISTRICT:
        case NODETYPE_TRAFFIC_LIGHT_NOJUNCTION:
            junction = buildNoLogicJunction();
            break;
        case NODETYPE_TRAFFIC_LIGHT:
        case NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED:
        case NODETYPE_RIGHT_BEFORE_LEFT:
        case NODETYPE_PRIORITY:
        case NODETYPE_PRIORITY_STOP:
        case NODETYPE_ALLWAY_STOP:
        case NODETYPE_ZIPPER:
            junction = buildLogicJunction();
            break;
        case NODETYPE_INTERNAL:
#ifdef HAVE_INTERNAL_LANES
            if (MSGlobals::gUsingInternalLanes) {
                junction = buildInternalJunction();
            }
#endif
            break;
        case NODETYPE_RAIL_SIGNAL:
            myOffset = 0;
            myActiveKey = myActiveID;
            myActiveProgram = "0";
            myLogicType = TLTYPE_RAIL;
            closeTrafficLightLogic(basePath);
            junction = buildLogicJunction();
            break;
        default:
            throw InvalidArgument("False junction logic type.");
    }
    if (junction != 0) {
        if (!myJunctions->add(myActiveID, junction)) {
            throw InvalidArgument("Another junction with the id '" + myActiveID + "' exists.");
        }
    }
}


MSJunctionControl*
NLJunctionControlBuilder::build() const {
    MSJunctionControl* js = myJunctions;
    myJunctions = 0;
    return js;
}


MSJunction*
NLJunctionControlBuilder::buildNoLogicJunction() {
    return new MSNoLogicJunction(myActiveID, myType, myPosition, myShape, myActiveIncomingLanes
#ifdef HAVE_INTERNAL_LANES
                                 , myActiveInternalLanes
#endif
                                );
}


MSJunction*
NLJunctionControlBuilder::buildLogicJunction() {
    MSJunctionLogic* jtype = getJunctionLogicSecure();
    // build the junction
    return new MSRightOfWayJunction(myActiveID, myType, myPosition, myShape, myActiveIncomingLanes,
#ifdef HAVE_INTERNAL_LANES
                                    myActiveInternalLanes,
#endif
                                    jtype);
}


#ifdef HAVE_INTERNAL_LANES
MSJunction*
NLJunctionControlBuilder::buildInternalJunction() {
    // build the junction
    return new MSInternalJunction(myActiveID, myType, myPosition, myShape, myActiveIncomingLanes,
                                  myActiveInternalLanes);
}
#endif


MSJunctionLogic*
NLJunctionControlBuilder::getJunctionLogicSecure() {
    // get and check the junction logic
    if (myLogics.find(myActiveID) == myLogics.end()) {
        throw InvalidArgument("Missing junction logic '" + myActiveID + "'.");
    }
    return myLogics[myActiveID];
}


MSTLLogicControl::TLSLogicVariants&
NLJunctionControlBuilder::getTLLogic(const std::string& id) const {
    return getTLLogicControlToUse().get(id);
}


void
NLJunctionControlBuilder::closeTrafficLightLogic(const std::string& basePath) {
    if (myActiveProgram == "off") {
        if (myAbsDuration > 0) {
            throw InvalidArgument("The off program for TLS '" + myActiveKey + "' has phases.");
        }
        if (!getTLLogicControlToUse().add(myActiveKey, myActiveProgram,
                                          new MSOffTrafficLightLogic(getTLLogicControlToUse(), myActiveKey))) {
            throw InvalidArgument("Another logic with id '" + myActiveKey + "' and subid '" + myActiveProgram + "' exists.");
        }
        return;
    }
    SUMOTime firstEventOffset = 0;
    unsigned int step = 0;
    MSTrafficLightLogic* existing = 0;
    MSSimpleTrafficLightLogic::Phases::const_iterator i = myActivePhases.begin();
    if (myLogicType != TLTYPE_RAIL) {
        if (myAbsDuration == 0) {
            existing = getTLLogicControlToUse().get(myActiveKey, myActiveProgram);
            if (existing == 0) {
                throw InvalidArgument("TLS program '" + myActiveProgram + "' for TLS '" + myActiveKey + "' has a duration of 0.");
            } else {
                // only modify the offset of an existing logic
                myAbsDuration = existing->getDefaultCycleTime();
                i = existing->getPhases().begin();
            }
        }
        // compute the initial step and first switch time of the tls-logic
        // a positive offset delays all phases by x (advance by absDuration - x) while a negative offset advances all phases by x seconds
        // @note The implementation of % for negative values is implementation defined in ISO1998
        SUMOTime offset; // the time to run the traffic light in advance
        if (myOffset >= 0) {
            offset = (myNet.getCurrentTimeStep() + myAbsDuration - (myOffset % myAbsDuration)) % myAbsDuration;
        } else {
            offset = (myNet.getCurrentTimeStep() + ((-myOffset) % myAbsDuration)) % myAbsDuration;
        }
        while (offset >= (*i)->duration) {
            step++;
            offset -= (*i)->duration;
            ++i;
        }
        firstEventOffset = (*i)->duration - offset + myNet.getCurrentTimeStep();
        if (existing != 0) {
            existing->changeStepAndDuration(getTLLogicControlToUse(),
                                            myNet.getCurrentTimeStep(), step, (*i)->duration - offset);
            return;
        }
    }

    if (myActiveProgram == "") {
        myActiveProgram = "default";
    }
    MSTrafficLightLogic* tlLogic = 0;
    // build the tls-logic in dependance to its type
    switch (myLogicType) {
        case TLTYPE_ACTUATED:
            // @note it is unclear how to apply the given offset in the context
            // of variable-length phases
            tlLogic = new MSActuatedTrafficLightLogic(getTLLogicControlToUse(),
                    myActiveKey, myActiveProgram,
                    myActivePhases, step, (*i)->minDuration + myNet.getCurrentTimeStep(),
                    myAdditionalParameter, basePath);
            break;
        case TLTYPE_STATIC:
            tlLogic =
                new MSSimpleTrafficLightLogic(getTLLogicControlToUse(),
                                              myActiveKey, myActiveProgram,
                                              myActivePhases, step, firstEventOffset,
                                              myAdditionalParameter);
            break;
        case TLTYPE_RAIL:
            tlLogic = new MSRailSignal(getTLLogicControlToUse(),
                                       myActiveKey, myActiveProgram,
                                       myAdditionalParameter);
            break;
    }
    myActivePhases.clear();
    if (tlLogic != 0) {
        if (getTLLogicControlToUse().add(myActiveKey, myActiveProgram, tlLogic)) {
            if (myNetIsLoaded) {
                tlLogic->init(myDetectorBuilder);
            } else {
                myLogics2PostLoadInit.push_back(tlLogic);
            }
        } else {
            WRITE_ERROR("Another logic with id '" + myActiveKey + "' and subid '" + myActiveProgram + "' exists.");
            delete tlLogic;
        }
    }
}


void
NLJunctionControlBuilder::initJunctionLogic(const std::string& id) {
    myActiveKey = id;
    myActiveProgram = "";
    myActiveLogic.clear();
    myActiveFoes.clear();
    myActiveConts.reset();
    myRequestSize = NO_REQUEST_SIZE; // seems not to be used
    myRequestItemNumber = 0;
    myCurrentHasError = false;
}


void
NLJunctionControlBuilder::addLogicItem(int request,
                                       const std::string& response,
                                       const std::string& foes,
                                       bool cont) {
    if (myCurrentHasError) {
        // had an error
        return;
    }
    if (request > 63) {
        // bad request
        myCurrentHasError = true;
        throw InvalidArgument("Junction logic '" + myActiveKey + "' is larger than allowed; recheck the network.");
    }
    if (myRequestSize == NO_REQUEST_SIZE) {
        // initialize
        myRequestSize = (int)response.size();
    }
    if (static_cast<int>(response.size()) != myRequestSize) {
        myCurrentHasError = true;
        throw InvalidArgument("Invalid response size " + toString(response.size()) +
                              " in Junction logic '" + myActiveKey + "' (expected  " + toString(myRequestSize) + ")");
    }
    if (static_cast<int>(foes.size()) != myRequestSize) {
        myCurrentHasError = true;
        throw InvalidArgument("Invalid foes size " + toString(foes.size()) +
                              " in Junction logic '" + myActiveKey + "' (expected  " + toString(myRequestSize) + ")");
    }
    // assert that the logicitems come ordered by their request index
    assert(myActiveLogic.size() == (size_t) request);
    assert(myActiveFoes.size() == (size_t) request);
    // add the read response for the given request index
    myActiveLogic.push_back(std::bitset<SUMO_MAX_CONNECTIONS>(response));
    // add the read junction-internal foes for the given request index
    myActiveFoes.push_back(std::bitset<SUMO_MAX_CONNECTIONS>(foes));
    // add whether the vehicle may drive a little bit further
    myActiveConts.set(request, cont);
    // increse number of set information
    myRequestItemNumber++;
}


void
NLJunctionControlBuilder::initTrafficLightLogic(const std::string& id, const std::string& programID,
        TrafficLightType type, SUMOTime offset) {
    myActiveKey = id;
    myActiveProgram = programID;
    myActivePhases.clear();
    myAbsDuration = 0;
    myRequestSize = NO_REQUEST_SIZE;
    myLogicType = type;
    myOffset = offset;
    myAdditionalParameter.clear();
}


void
NLJunctionControlBuilder::addPhase(SUMOTime duration, const std::string& state,
                                   SUMOTime minDuration, SUMOTime maxDuration) {
    // build and add the phase definition to the list
    myActivePhases.push_back(new MSPhaseDefinition(duration, minDuration, maxDuration, state));
    // add phase duration to the absolute duration
    myAbsDuration += duration;
}


void
NLJunctionControlBuilder::closeJunctionLogic() {
    if (myRequestSize == NO_REQUEST_SIZE) {
        // We have a legacy network. junction element did not contain logicitems; read the logic later
        return;
    }
    if (myCurrentHasError) {
        // had an error before...
        return;
    }
    if (myRequestItemNumber != myRequestSize) {
        throw InvalidArgument("The description for the junction logic '" + myActiveKey + "' is malicious.");
    }
    if (myLogics.count(myActiveKey) > 0) {
        throw InvalidArgument("Junction logic '" + myActiveKey + "' was defined twice.");
    }
    MSJunctionLogic* logic = new MSBitsetLogic(myRequestSize,
            new MSBitsetLogic::Logic(myActiveLogic),
            new MSBitsetLogic::Foes(myActiveFoes),
            myActiveConts);
    myLogics[myActiveKey] = logic;
}


MSTLLogicControl*
NLJunctionControlBuilder::buildTLLogics() const {
    if (!myLogicControl->closeNetworkReading()) {
        throw ProcessError("Traffic lights could not be built.");
    }
    MSTLLogicControl* ret = myLogicControl;
    myLogicControl = 0;
    return ret;
}


void
NLJunctionControlBuilder::addParam(const std::string& key,
                                   const std::string& value) {
    myAdditionalParameter[key] = value;
}


MSTLLogicControl&
NLJunctionControlBuilder::getTLLogicControlToUse() const {
    if (myLogicControl != 0) {
        return *myLogicControl;
    }
    return myNet.getTLSControl();
}


const std::string&
NLJunctionControlBuilder::getActiveKey() const {
    return myActiveKey;
}


const std::string&
NLJunctionControlBuilder::getActiveSubKey() const {
    return myActiveProgram;
}


void
NLJunctionControlBuilder::postLoadInitialization() {
    for (std::vector<MSTrafficLightLogic*>::const_iterator it = myLogics2PostLoadInit.begin();
            it != myLogics2PostLoadInit.end(); ++it) {
        (*it)->init(myDetectorBuilder);
    }
    myNetIsLoaded = true;
}


MSJunction*
NLJunctionControlBuilder::retrieve(const std::string id) {
    if (myJunctions != 0) {
        return myJunctions->get(id);
    } else {
        return 0;
    }
}

/****************************************************************************/
