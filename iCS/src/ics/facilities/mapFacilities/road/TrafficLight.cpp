/****************************************************************************/
/// @file    TrafficLight.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "TrafficLight.h"

namespace ics_facilities {

TrafficLight::TrafficLight() {
    ID          = trafficLightID_t(0);//"";
    status      = UNKNOWN;
    active      = false;
}

TrafficLight::TrafficLight(trafficLightID_t trafficLightID, short linkIndexArg) {
    linkIndex = linkIndexArg;
    ID          = trafficLightID;
    status      = UNKNOWN;
    active      = true;
}

TrafficLight::~TrafficLight() { }

trafficLightID_t TrafficLight::getID() const {
    return ID;
}

short TrafficLight::getLinkIndex() const {
    return linkIndex;
}

const Point2D&  TrafficLight::getPosition() const {
    return position;
}

roadElementID_t TrafficLight::getControlledLaneID() const {
    return controlledLaneID;
}

roadElementID_t TrafficLight::getFollowingLaneID() const {
    return followingLaneID;
}

tlStatus        TrafficLight::getStatus() const {
    return status;
}

bool            TrafficLight::isActive() const {
    return active;
}

void            TrafficLight::setPosition(Point2D pos) {
    this->position = pos;
}

void            TrafficLight::setStatus(tlStatus newStatus) {
    this->status = newStatus;
}

void            TrafficLight::setControlledLaneID(roadElementID_t contrLaneID) {
    this->controlledLaneID = contrLaneID;
}

void            TrafficLight::setFollowingLaneID(roadElementID_t followLaneID) {
    this->followingLaneID = followLaneID;
}

void            TrafficLight::setActive(bool active) {
    this->active = active;
}

}
