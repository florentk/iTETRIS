/****************************************************************************/
/// @file    MSPersonControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id: MSPersonControl.cpp 18865 2015-09-18 21:44:49Z namdre $
///
// Stores all persons in the net and handles their waiting for cars.
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

#include <vector>
#include <algorithm>
#include "MSNet.h"
#include "MSEdge.h"
#include <microsim/pedestrians/MSPerson.h>
#include "MSVehicle.h"
#include "MSPersonControl.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSPersonControl::MSPersonControl():
    myLoadedPersonNumber(0),
    myRunningPersonNumber(0),
    myJammedPersonNumber(0)
{}


MSPersonControl::~MSPersonControl() {
    for (std::map<std::string, MSTransportable*>::iterator i = myPersons.begin(); i != myPersons.end(); ++i) {
        delete(*i).second;
    }
    myPersons.clear();
    myWaiting4Vehicle.clear();
}


bool
MSPersonControl::add(const std::string& id, MSPerson* person) {
    if (myPersons.find(id) == myPersons.end()) {
        myPersons[id] = person;
        myLoadedPersonNumber++;
        myRunningPersonNumber++;
        return true;
    }
    return false;
}


MSTransportable*
MSPersonControl::get(const std::string& id) const {
    std::map<std::string, MSTransportable*>::const_iterator i = myPersons.find(id);
    if (i == myPersons.end()) {
        return 0;
    }
    return (*i).second;
}


void
MSPersonControl::erase(MSTransportable* person) {
    const std::string& id = person->getID();
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("tripinfo-output");
        od.openTag("personinfo").writeAttr("id", id).writeAttr("depart", time2string(person->getDesiredDepart()));
        person->tripInfoOutput(od);
        od.closeTag();
    }
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("vehroute-output");
        od.openTag("person").writeAttr("id", id).writeAttr("depart", time2string(person->getDesiredDepart())).writeAttr("arrival", time2string(MSNet::getInstance()->getCurrentTimeStep()));
        person->routeOutput(od);
        od.closeTag();
        od << "\n";
    }
    const std::map<std::string, MSTransportable*>::iterator i = myPersons.find(id);
    if (i != myPersons.end()) {
        myRunningPersonNumber--;
        delete i->second;
        myPersons.erase(i);
    }
}


void
MSPersonControl::setDeparture(const SUMOTime time, MSPerson* person) {
    const SUMOTime step = time % DELTA_T == 0 ? time : (time / DELTA_T + 1) * DELTA_T;
    if (myWaiting4Departure.find(step) == myWaiting4Departure.end()) {
        myWaiting4Departure[step] = PersonVector();
    }
    myWaiting4Departure[step].push_back(person);
}


void
MSPersonControl::setWaitEnd(const SUMOTime time, MSTransportable* person) {
    const SUMOTime step = time % DELTA_T == 0 ? time : (time / DELTA_T + 1) * DELTA_T;
    if (myWaitingUntil.find(step) == myWaitingUntil.end()) {
        myWaitingUntil[step] = PersonVector();
    }
    myWaitingUntil[step].push_back(person);
}


void
MSPersonControl::checkWaitingPersons(MSNet* net, const SUMOTime time) {
    while (myWaiting4Departure.find(time) != myWaiting4Departure.end()) {
        const PersonVector& persons = myWaiting4Departure[time];
        // we cannot use an iterator here because there might be additions to the vector while proceeding
        for (size_t i = 0; i < persons.size(); ++i) {
            if (!persons[i]->proceed(net, time)) {
                erase(persons[i]);
            }
        }
        myWaiting4Departure.erase(time);
    }
    while (myWaitingUntil.find(time) != myWaitingUntil.end()) {
        const PersonVector& persons = myWaitingUntil[time];
        // we cannot use an iterator here because there might be additions to the vector while proceeding
        for (size_t i = 0; i < persons.size(); ++i) {
            if (!persons[i]->proceed(net, time)) {
                erase(persons[i]);
            }
        }
        myWaitingUntil.erase(time);
    }
}


void
MSPersonControl::addWaiting(const MSEdge* const edge, MSTransportable* person) {
    if (myWaiting4Vehicle.find(edge) == myWaiting4Vehicle.end()) {
        myWaiting4Vehicle[edge] = std::vector<MSTransportable*>();
    }
    myWaiting4Vehicle[edge].push_back(person);
}


bool
MSPersonControl::isWaiting4Vehicle(const MSEdge* const edge, MSPerson* /* p */) const {
    return myWaiting4Vehicle.find(edge) != myWaiting4Vehicle.end();
}


bool
MSPersonControl::boardAnyWaiting(MSEdge* edge, MSVehicle* vehicle, MSVehicle::Stop* stop) {
    bool ret = false;
    if (myWaiting4Vehicle.find(edge) != myWaiting4Vehicle.end()) {
        PersonVector& waitPersons = myWaiting4Vehicle[edge];
        const std::string& line = vehicle->getParameter().line == "" ? vehicle->getParameter().id : vehicle->getParameter().line;
        SUMOTime currentTime =  MSNet::getInstance()->getCurrentTimeStep();
        for (PersonVector::iterator i = waitPersons.begin(); i != waitPersons.end();) {
            if ((*i)->isWaitingFor(line) && vehicle->getVehicleType().getPersonCapacity() > vehicle->getPersonNumber() && stop->timeToBoardNextPerson <= currentTime && stop->startPos <= (*i)->getEdgePos() && (*i)->getEdgePos() <= stop->endPos) {
                edge->removePerson(*i);
                vehicle->addPerson(*i);
                //if the time a person needs to enter the vehicle extends the duration of the stop of the vehicle extend
                //the duration by setting it to the boarding duration of the person
                const SUMOTime boardingDuration = vehicle->getVehicleType().getBoardingDuration();
                if (boardingDuration >= stop->duration) {
                    stop->duration = boardingDuration;
                }
                //update the time point at which the next person can board the vehicle
                if (stop->timeToBoardNextPerson > currentTime - DELTA_T) {
                    stop->timeToBoardNextPerson += boardingDuration;
                } else {
                    stop->timeToBoardNextPerson = currentTime + boardingDuration;
                }

                static_cast<MSPerson::MSPersonStage_Driving*>((*i)->getCurrentStage())->setVehicle(vehicle);
                i = waitPersons.erase(i);
                ret = true;
            } else {
                ++i;
            }
        }
        if (waitPersons.size() == 0) {
            myWaiting4Vehicle.erase(myWaiting4Vehicle.find(edge));
        }
    }
    return ret;
}

bool
MSPersonControl::hasPersons() const {
    return !myPersons.empty();
}


bool
MSPersonControl::hasNonWaiting() const {
    return !myWaiting4Departure.empty() || !myWaitingUntil.empty() || !myWalking.empty();
}


void
MSPersonControl::setWalking(MSTransportable* p) {
    myWalking[p->getID()] = p;
}


void
MSPersonControl::unsetWalking(MSTransportable* p) {
    std::map<std::string, MSTransportable*>::iterator i = myWalking.find(p->getID());
    if (i != myWalking.end()) {
        myWalking.erase(i);
    }
}


void
MSPersonControl::abortWaiting() {
    for (std::map<const MSEdge*, PersonVector>::const_iterator i = myWaiting4Vehicle.begin(); i != myWaiting4Vehicle.end(); ++i) {
        const MSEdge* edge = (*i).first;
        const PersonVector& pv = (*i).second;
        for (PersonVector::const_iterator j = pv.begin(); j != pv.end(); ++j) {
            MSTransportable* p = (*j);
            edge->removePerson(p);
            WRITE_WARNING("Person '" + p->getID() + "' aborted waiting for a ride that will never come.");
            erase(p);
        }
    }
}


MSPerson*
MSPersonControl::buildPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) const {
    return new MSPerson(pars, vtype, plan);
}

/****************************************************************************/
