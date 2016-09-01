/****************************************************************************/
/// @file    MSContainer.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Thu, 12 Jun 2014
/// @version $Id: MSContainer.cpp 19535 2015-12-05 13:47:18Z behrisch $
///
// The class for modelling container-movements
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

#include <string>
#include <vector>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include "MSNet.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSContainer.h"
#include "microsim/MSStoppingPlace.h"
#include "MSContainerControl.h"
#include "MSInsertionControl.h"
#include "MSVehicle.h"
#include "MSCModel_NonInteracting.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

/* -------------------------------------------------------------------------
 * static member definitions
 * ----------------------------------------------------------------------- */
const SUMOReal MSContainer::ROADSIDE_OFFSET(3);

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage_Driving - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage_Driving::MSContainerStage_Driving(const MSEdge& destination,
        MSStoppingPlace* toStop, const SUMOReal arrivalPos, const std::vector<std::string>& lines)
    : MSTransportable::Stage(destination, toStop, arrivalPos, DRIVING), myLines(lines.begin(), lines.end()),
      myVehicle(0) {}


MSContainer::MSContainerStage_Driving::~MSContainerStage_Driving() {}

void
MSContainer::MSContainerStage_Driving::proceed(MSNet* net, MSTransportable* container, SUMOTime now, Stage* previous) {
    if (previous->getDestinationStop() != 0) {
        // the arrival stop may have an access point
        myWaitingEdge = &previous->getDestinationStop()->getLane().getEdge();
    } else {
        myWaitingEdge = previous->getEdge();
    }
    myWaitingPos = previous->getEdgePos(now);
    myWaitingSince = now;
    SUMOVehicle* availableVehicle = net->getVehicleControl().getWaitingVehicle(myWaitingEdge, myLines, myWaitingPos, container->getID());
    if (availableVehicle != 0 && availableVehicle->getParameter().departProcedure == DEPART_CONTAINER_TRIGGERED) {
        myVehicle = availableVehicle;
        myWaitingEdge->removeContainer(container);
        myVehicle->addContainer(container);
        net->getInsertionControl().add(myVehicle);
        net->getVehicleControl().removeWaiting(myWaitingEdge, myVehicle);
        net->getVehicleControl().unregisterOneWaitingForContainer();
    } else {
        net->getContainerControl().addWaiting(myWaitingEdge, container);
        myWaitingEdge->addContainer(container);
    }
}

const MSEdge*
MSContainer::MSContainerStage_Driving::getEdge() const {
    if (myVehicle != 0) {
        return myVehicle->getEdge();
    }
    return myWaitingEdge;
}


const MSEdge*
MSContainer::MSContainerStage_Driving::getFromEdge() const {
    return myWaitingEdge;
}


SUMOReal
MSContainer::MSContainerStage_Driving::getEdgePos(SUMOTime /* now */) const {
    if (myVehicle != 0) {
        // vehicle may already have passed the lane (check whether this is correct)
        return MIN2(myVehicle->getPositionOnLane(), getEdge()->getLength());
    }
    return myWaitingPos;
}

Position
MSContainer::MSContainerStage_Driving::getPosition(SUMOTime /* now */) const {
    if (myVehicle != 0) {
        /// @bug this fails while vehicle is driving across a junction
        return myVehicle->getEdge()->getLanes()[0]->getShape().positionAtOffset(myVehicle->getPositionOnLane());
    }
    return getEdgePosition(myWaitingEdge, myWaitingPos, ROADSIDE_OFFSET);
}

SUMOReal
MSContainer::MSContainerStage_Driving::getAngle(SUMOTime /* now */) const {
    if (myVehicle != 0) {
        MSVehicle* veh = dynamic_cast<MSVehicle*>(myVehicle);
        if (veh != 0) {
            return veh->getAngle();
        } else {
            return 0;
        }
    }
    return getEdgeAngle(myWaitingEdge, myWaitingPos) + M_PI / 2.;
}

bool
MSContainer::MSContainerStage_Driving::isWaitingFor(const std::string& line) const {
    return myLines.count(line) > 0;
}

bool
MSContainer::MSContainerStage_Driving::isWaiting4Vehicle() const {
    return myVehicle == 0;
}

SUMOTime
MSContainer::MSContainerStage_Driving::getWaitingTime(SUMOTime now) const {
    return isWaiting4Vehicle() ? now - myWaitingSince : 0;
}

SUMOReal
MSContainer::MSContainerStage_Driving::getSpeed() const {
    return myVehicle == 0 ? 0 : myVehicle->getSpeed();
}

std::string
MSContainer::MSContainerStage_Driving::getStageDescription() const {
    return isWaiting4Vehicle() ? "waiting for " + joinToString(myLines, ",") : "transport";
}

MSStoppingPlace*
MSContainer::MSContainerStage_Driving::getDepartContainerStop() const {
    return myDepartContainerStop;
}

void
MSContainer::MSContainerStage_Driving::tripInfoOutput(OutputDevice& os) const {
    os.openTag("transport").writeAttr("depart", time2string(myDeparted)).writeAttr("arrival", time2string(myArrived)).closeTag();
}

void
MSContainer::MSContainerStage_Driving::routeOutput(OutputDevice& os) const {
    os.openTag("transport").writeAttr(SUMO_ATTR_FROM, getFromEdge()->getID()).writeAttr(SUMO_ATTR_TO, getDestination().getID());
    os.writeAttr(SUMO_ATTR_LINES, myLines).closeTag();
}

void
MSContainer::MSContainerStage_Driving::beginEventOutput(const MSTransportable& container, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", container.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}

void
MSContainer::MSContainerStage_Driving::endEventOutput(const MSTransportable& container, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", container.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}



/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage_Waiting - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage_Waiting::MSContainerStage_Waiting(const MSEdge& destination,
        SUMOTime duration, SUMOTime until, SUMOReal pos, const std::string& actType) :
    MSTransportable::Stage(destination, 0, SUMOVehicleParameter::interpretEdgePos(
                               pos, destination.getLength(), SUMO_ATTR_DEPARTPOS, "container stopping at " + destination.getID()), WAITING),
    myWaitingDuration(duration),
    myWaitingUntil(until),
    myActType(actType) {
}

MSContainer::MSContainerStage_Waiting::~MSContainerStage_Waiting() {}

const MSEdge*
MSContainer::MSContainerStage_Waiting::getEdge() const {
    return &myDestination;
}

const MSEdge*
MSContainer::MSContainerStage_Waiting::getFromEdge() const {
    return &myDestination;
}

SUMOReal
MSContainer::MSContainerStage_Waiting::getEdgePos(SUMOTime /* now */) const {
    return myArrivalPos;
}

SUMOTime
MSContainer::MSContainerStage_Waiting::getUntil() const {
    return myWaitingUntil;
}

Position
MSContainer::MSContainerStage_Waiting::getPosition(SUMOTime /* now */) const {
    return getEdgePosition(&myDestination, myArrivalPos, ROADSIDE_OFFSET);
}

SUMOReal
MSContainer::MSContainerStage_Waiting::getAngle(SUMOTime /* now */) const {
    return getEdgeAngle(&myDestination, myArrivalPos) + M_PI;
}

SUMOTime
MSContainer::MSContainerStage_Waiting::getWaitingTime(SUMOTime now) const {
    return now - myWaitingStart;
}

SUMOReal
MSContainer::MSContainerStage_Waiting::getSpeed() const {
    return 0;
}

MSStoppingPlace*
MSContainer::MSContainerStage_Waiting::getDepartContainerStop() const {
    return myCurrentContainerStop;
}

void
MSContainer::MSContainerStage_Waiting::proceed(MSNet* net, MSTransportable* container, SUMOTime now, Stage* previous) {
    previous->getEdge()->addContainer(container);
    myWaitingStart = now;
    const SUMOTime until = MAX3(now, now + myWaitingDuration, myWaitingUntil);
    net->getContainerControl().setWaitEnd(until, container);
}

void
MSContainer::MSContainerStage_Waiting::tripInfoOutput(OutputDevice& os) const {
    os.openTag("stop").writeAttr("arrival", time2string(myArrived)).closeTag();
}

void
MSContainer::MSContainerStage_Waiting::routeOutput(OutputDevice& os) const {
    os.openTag("stop").writeAttr(SUMO_ATTR_LANE, getDestination().getID());
    if (myWaitingDuration >= 0) {
        os.writeAttr(SUMO_ATTR_DURATION, time2string(myWaitingDuration));
    }
    if (myWaitingUntil >= 0) {
        os.writeAttr(SUMO_ATTR_UNTIL, time2string(myWaitingUntil));
    }
    os.closeTag();
}

void
MSContainer::MSContainerStage_Waiting::beginEventOutput(const MSTransportable& container, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actstart " + myActType)
    .writeAttr("agent", container.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}

void
MSContainer::MSContainerStage_Waiting::endEventOutput(const MSTransportable& container, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actend " + myActType).writeAttr("agent", container.getID())
    .writeAttr("link", getEdge()->getID()).closeTag();
}

/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage_Tranship - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage_Tranship::MSContainerStage_Tranship(const std::vector<const MSEdge*>& route,
        MSStoppingPlace* toStop,
        SUMOReal speed,
        SUMOReal departPos, SUMOReal arrivalPos) :
    MSTransportable::Stage(*route.back(), toStop, SUMOVehicleParameter::interpretEdgePos(
                               arrivalPos, route.back()->getLength(), SUMO_ATTR_ARRIVALPOS, "container getting transhipped to " + route.back()->getID()), MOVING_WITHOUT_VEHICLE), myRoute(route),
    mySpeed(speed), myContainerState(0), myCurrentInternalEdge(0) {
    myDepartPos = SUMOVehicleParameter::interpretEdgePos(
                      departPos, myRoute.front()->getLength(), SUMO_ATTR_DEPARTPOS, "container getting transhipped from " + myRoute.front()->getID());
}

MSContainer::MSContainerStage_Tranship::~MSContainerStage_Tranship() {
}

void
MSContainer::MSContainerStage_Tranship::proceed(MSNet* /* net */, MSTransportable* container, SUMOTime now, Stage* previous) {
    previous->getEdge()->removeContainer(container);
    myRouteStep = myRoute.end() - 1;   //define that the container is already on its destination edge
    MSNet::getInstance()->getContainerControl().setTranship(container);
    myDepartPos = previous->getEdgePos(now);
    myContainerState = MSCModel_NonInteracting::getModel()->add(container, this, now);
    (*myRouteStep)->addContainer(container);
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getEdge() const {
    if (myCurrentInternalEdge != 0) {
        return myCurrentInternalEdge;
    } else {
        return *myRouteStep;
    }
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getFromEdge() const {
    return myRoute.front();
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getToEdge() const {
    return myRoute.back();
}

SUMOReal
MSContainer::MSContainerStage_Tranship::getEdgePos(SUMOTime now) const {
    return myContainerState->getEdgePos(*this, now);
}

Position
MSContainer::MSContainerStage_Tranship::getPosition(SUMOTime now) const {
    return myContainerState->getPosition(*this, now);
}

SUMOReal
MSContainer::MSContainerStage_Tranship::getAngle(SUMOTime now) const {
    return myContainerState->getAngle(*this, now);
}

SUMOTime
MSContainer::MSContainerStage_Tranship::getWaitingTime(SUMOTime /* now */) const {
    return 0;
}

SUMOReal
MSContainer::MSContainerStage_Tranship::getSpeed() const {
    return myContainerState->getSpeed(*this);
}

MSStoppingPlace*
MSContainer::MSContainerStage_Tranship::getDepartContainerStop() const {
    return myDepartContainerStop;
}

void
MSContainer::MSContainerStage_Tranship::tripInfoOutput(OutputDevice& os) const {
    os.openTag("tranship").writeAttr("arrival", time2string(myArrived)).closeTag();
}


void
MSContainer::MSContainerStage_Tranship::routeOutput(OutputDevice& os) const {
    os.openTag("tranship").writeAttr(SUMO_ATTR_EDGES, myRoute);
    os.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    os.closeTag();
}


void
MSContainer::MSContainerStage_Tranship::beginEventOutput(const MSTransportable& c, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "departure")
    .writeAttr("agent", c.getID()).writeAttr("link", myRoute.front()->getID()).closeTag();
}


void
MSContainer::MSContainerStage_Tranship::endEventOutput(const MSTransportable& c, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival")
    .writeAttr("agent", c.getID()).writeAttr("link", myRoute.back()->getID()).closeTag();
}

bool
MSContainer::MSContainerStage_Tranship::moveToNextEdge(MSTransportable* container, SUMOTime currentTime, MSEdge* nextInternal) {
    ((MSEdge*)getEdge())->removeContainer(container);
    if (myRouteStep == myRoute.end() - 1) {
        MSNet::getInstance()->getContainerControl().unsetTranship(container);
        if (myDestinationStop != 0) {
            myDestinationStop->addTransportable(container);    //jakob
        }
        if (!container->proceed(MSNet::getInstance(), currentTime)) {
            MSNet::getInstance()->getContainerControl().erase(container);
        }
        return true;
    } else {
        if (nextInternal == 0) {
            ++myRouteStep;
            myCurrentInternalEdge = 0;
        } else {
            myCurrentInternalEdge = nextInternal;
        }
        ((MSEdge*) getEdge())->addContainer(container);
        return false;
    }
}

/* -------------------------------------------------------------------------
 * MSContainer - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportablePlan* plan)
    : MSTransportable(pars, vtype, plan) {
}

MSContainer::~MSContainer() {
}

bool
MSContainer::proceed(MSNet* net, SUMOTime time) {
    Stage* prior = *myStep;
    prior->setArrived(time);
    myStep++;
    if (myStep != myPlan->end()) {
        (*myStep)->proceed(net, this, time, prior);
        return true;
    } else {
        prior->getEdge()->removeContainer(this);
        return false;
    }
}


void
MSContainer::routeOutput(OutputDevice& os) const {
    MSTransportablePlan::const_iterator i = myPlan->begin();
    if ((*i)->getStageType() == WAITING && getDesiredDepart() == static_cast<MSContainerStage_Waiting*>(*i)->getUntil()) {
        ++i;
    }
    for (; i != myPlan->end(); ++i) {
        (*i)->routeOutput(os);
    }
}


/****************************************************************************/
