/****************************************************************************/
/// @file    RONet.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id: RONet.cpp 18774 2015-09-03 14:06:40Z behrisch $
///
// The router's network representation
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

#include <algorithm>
#include "ROEdge.h"
#include "RONode.h"
#include "RONet.h"
#include "RORoute.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include <utils/vehicle/RouteCostCalculator.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
RONet* RONet::myInstance = 0;


// ===========================================================================
// method definitions
// ===========================================================================
RONet*
RONet::getInstance(void) {
    if (myInstance != 0) {
        return myInstance;
    }
    throw ProcessError("A network was not yet constructed.");
}


RONet::RONet()
    : myVehicleTypes(), myDefaultVTypeMayBeDeleted(true),
      myRoutesOutput(0), myRouteAlternativesOutput(0), myTypesOutput(0),
      myReadRouteNo(0), myDiscardedRouteNo(0), myWrittenRouteNo(0),
      myHavePermissions(false),
      myNumInternalEdges(0),
      myErrorHandler(OptionsCont::getOptions().exists("ignore-errors")
                     && OptionsCont::getOptions().getBool("ignore-errors") ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance()) {
    if (myInstance != 0) {
        throw ProcessError("A network was already constructed.");
    }
    SUMOVTypeParameter* type = new SUMOVTypeParameter(DEFAULT_VTYPE_ID, SVC_IGNORING);
    type->onlyReferenced = true;
    myVehicleTypes.add(type->id, type);
    myInstance = this;
}


RONet::~RONet() {
    myNodes.clear();
    myEdges.clear();
    myVehicleTypes.clear();
    myRoutes.clear();
    myVehicles.clear();
}


void
RONet::addRestriction(const std::string& id, const SUMOVehicleClass svc, const SUMOReal speed) {
    myRestrictions[id][svc] = speed;
}


const std::map<SUMOVehicleClass, SUMOReal>*
RONet::getRestrictions(const std::string& id) const {
    std::map<std::string, std::map<SUMOVehicleClass, SUMOReal> >::const_iterator i = myRestrictions.find(id);
    if (i == myRestrictions.end()) {
        return 0;
    }
    return &i->second;
}


bool
RONet::addEdge(ROEdge* edge) {
    if (!myEdges.add(edge->getID(), edge)) {
        WRITE_ERROR("The edge '" + edge->getID() + "' occurs at least twice.");
        delete edge;
        return false;
    }
    if (edge->getFunc() == ROEdge::ET_INTERNAL) {
        myNumInternalEdges += 1;
    }
    return true;
}


bool
RONet::addDistrict(const std::string id, ROEdge* source, ROEdge* sink) {
    if (myDistricts.count(id) > 0) {
        WRITE_ERROR("The TAZ '" + id + "' occurs at least twice.");
        delete source;
        delete sink;
        return false;
    }
    sink->setFunc(ROEdge::ET_DISTRICT);
    addEdge(sink);
    source->setFunc(ROEdge::ET_DISTRICT);
    addEdge(source);
    myDistricts[id] = std::make_pair(std::vector<std::string>(), std::vector<std::string>());
    return true;
}


bool
RONet::addDistrictEdge(const std::string tazID, const std::string edgeID, const bool isSource) {
    if (myDistricts.count(tazID) == 0) {
        WRITE_ERROR("The TAZ '" + tazID + "' is unknown.");
        return false;
    }
    ROEdge* edge = getEdge(edgeID);
    if (edge == 0) {
        WRITE_ERROR("The edge '" + edgeID + "' for TAZ '" + tazID + "' is unknown.");
        return false;
    }
    if (isSource) {
        getEdge(tazID + "-source")->addSuccessor(edge);
        myDistricts[tazID].first.push_back(edgeID);
    } else {
        edge->addSuccessor(getEdge(tazID + "-sink"));
        myDistricts[tazID].second.push_back(edgeID);
    }
    return true;
}


void
RONet::addNode(RONode* node) {
    if (!myNodes.add(node->getID(), node)) {
        WRITE_ERROR("The node '" + node->getID() + "' occurs at least twice.");
        delete node;
    }
}


void
RONet::addBusStop(const std::string& id, SUMOVehicleParameter::Stop* stop) {
    std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator it = myBusStops.find(id);
    if (it != myBusStops.end()) {
        WRITE_ERROR("The bus stop '" + id + "' occurs at least twice.");
        delete stop;
    }
    myBusStops[id] = stop;
}


void
RONet::addContainerStop(const std::string& id, SUMOVehicleParameter::Stop* stop) {
    std::map<std::string, SUMOVehicleParameter::Stop*>::const_iterator it = myContainerStops.find(id);
    if (it != myContainerStops.end()) {
        WRITE_ERROR("The container stop '" + id + "' occurs at least twice.");
        delete stop;
    }
    myContainerStops[id] = stop;
}


bool
RONet::addRouteDef(RORouteDef* def) {
    return myRoutes.add(def->getID(), def);
}


void
RONet::openOutput(const std::string& filename, const std::string altFilename, const std::string typeFilename) {
    if (filename != "") {
        myRoutesOutput = &OutputDevice::getDevice(filename);
        myRoutesOutput->writeHeader<ROEdge>(SUMO_TAG_ROUTES);
        myRoutesOutput->writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/routes_file.xsd");
    }
    if (altFilename != "") {
        myRouteAlternativesOutput = &OutputDevice::getDevice(altFilename);
        myRouteAlternativesOutput->writeHeader<ROEdge>(SUMO_TAG_ROUTES);
        myRouteAlternativesOutput->writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/routes_file.xsd");
    }
    if (typeFilename != "") {
        myTypesOutput = &OutputDevice::getDevice(typeFilename);
        myTypesOutput->writeXMLHeader("routes", "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/routes_file.xsd\"");
    }
}


void
RONet::cleanup(SUMOAbstractRouter<ROEdge, ROVehicle>* router) {
    // end writing
    if (myRoutesOutput != 0) {
        myRoutesOutput->close();
    }
    // only if opened
    if (myRouteAlternativesOutput != 0) {
        myRouteAlternativesOutput->close();
    }
    // only if opened
    if (myTypesOutput != 0) {
        myTypesOutput->close();
    }
    RouteCostCalculator<RORoute, ROEdge, ROVehicle>::cleanup();
#ifdef HAVE_FOX
    if (myThreadPool.size() > 0) {
        myThreadPool.clear();
        return;
    }
#endif
    delete router;
}



SUMOVTypeParameter*
RONet::getVehicleTypeSecure(const std::string& id) {
    // check whether the type was already known
    SUMOVTypeParameter* type = myVehicleTypes.get(id);
    if (id == DEFAULT_VTYPE_ID) {
        myDefaultVTypeMayBeDeleted = false;
    }
    if (type != 0) {
        return type;
    }
    VTypeDistDictType::iterator it2 = myVTypeDistDict.find(id);
    if (it2 != myVTypeDistDict.end()) {
        return it2->second->get();
    }
    if (id == "") {
        // ok, no vehicle type or an unknown type was given within the user input
        //  return the default type
        myDefaultVTypeMayBeDeleted = false;
        return myVehicleTypes.get(DEFAULT_VTYPE_ID);
    }
    return type;
}


bool
RONet::checkVType(const std::string& id) {
    if (id == DEFAULT_VTYPE_ID) {
        if (myDefaultVTypeMayBeDeleted) {
            myVehicleTypes.remove(id);
            myDefaultVTypeMayBeDeleted = false;
        } else {
            return false;
        }
    } else {
        if (myVehicleTypes.get(id) != 0 || myVTypeDistDict.find(id) != myVTypeDistDict.end()) {
            return false;
        }
    }
    return true;
}


bool
RONet::addVehicleType(SUMOVTypeParameter* type) {
    if (checkVType(type->id)) {
        myVehicleTypes.add(type->id, type);
    } else {
        WRITE_ERROR("The vehicle type '" + type->id + "' occurs at least twice.");
        delete type;
        return false;
    }
    return true;
}


bool
RONet::addVTypeDistribution(const std::string& id, RandomDistributor<SUMOVTypeParameter*>* vehTypeDistribution) {
    if (checkVType(id)) {
        myVTypeDistDict[id] = vehTypeDistribution;
        return true;
    }
    return false;
}


bool
RONet::addVehicle(const std::string& id, ROVehicle* veh) {
    if (myVehIDs.find(id) == myVehIDs.end() && myVehicles.add(id, veh)) {
        myVehIDs.insert(id);
        myReadRouteNo++;
        return true;
    }
    WRITE_ERROR("Another vehicle with the id '" + id + "' exists.");
    return false;
}


bool
RONet::addFlow(SUMOVehicleParameter* flow, const bool randomize) {
    if (randomize) {
        myDepartures[flow->id].reserve(flow->repetitionNumber);
        for (int i = 0; i < flow->repetitionNumber; ++i) {
            myDepartures[flow->id].push_back(flow->depart + RandHelper::rand(flow->repetitionNumber * flow->repetitionOffset));
        }
        std::sort(myDepartures[flow->id].begin(), myDepartures[flow->id].end());
        std::reverse(myDepartures[flow->id].begin(), myDepartures[flow->id].end());
    }
    return myFlows.add(flow->id, flow);
}


void
RONet::addPerson(const SUMOTime depart, const std::string desc) {
    myPersons.insert(std::pair<const SUMOTime, const std::string>(depart, desc));
}

void
RONet::addContainer(const SUMOTime depart, const std::string desc) {
    myContainers.insert(std::pair<const SUMOTime, const std::string>(depart, desc));
}


bool
RONet::computeRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                    const ROVehicle* const veh, const bool removeLoops,
                    MsgHandler* errorHandler) {
    std::string noRouteMsg = "The vehicle '" + veh->getID() + "' has no valid route.";
    RORouteDef* const routeDef = veh->getRouteDefinition();
    // check if the route definition is valid
    if (routeDef == 0) {
        errorHandler->inform(noRouteMsg);
        return false;
    }
    RORoute* current = routeDef->buildCurrentRoute(router, veh->getDepartureTime(), *veh);
    if (current == 0 || current->size() == 0) {
        delete current;
        errorHandler->inform(noRouteMsg);
        return false;
    }
    // check whether we have to evaluate the route for not containing loops
    if (removeLoops) {
        current->recheckForLoops();
        // check whether the route is still valid
        if (current->size() == 0) {
            delete current;
            errorHandler->inform(noRouteMsg + " (after removing loops)");
            return false;
        }
    }
    // add built route
    routeDef->addAlternative(router, veh, current, veh->getDepartureTime());
    return true;
}


void
RONet::checkFlows(SUMOTime time) {
    std::vector<std::string> toRemove;
    for (NamedObjectCont<SUMOVehicleParameter*>::IDMap::const_iterator i = myFlows.getMyMap().begin(); i != myFlows.getMyMap().end(); ++i) {
        SUMOVehicleParameter* pars = i->second;
        if (pars->repetitionProbability > 0) {
            while (pars->depart < time) {
                if (pars->repetitionEnd <= pars->depart) {
                    toRemove.push_back(i->first);
                    break;
                }
                // only call rand if all other conditions are met
                if (RandHelper::rand() < (pars->repetitionProbability * TS)) {
                    SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
                    newPars->id = pars->id + "." + toString(pars->repetitionsDone);
                    newPars->depart = pars->depart;
                    pars->repetitionsDone++;
                    // try to build the vehicle
                    SUMOVTypeParameter* type = getVehicleTypeSecure(pars->vtypeid);
                    if (type == 0) {
                        type = getVehicleTypeSecure(DEFAULT_VTYPE_ID);
                    } else {
                        // fix the type id in case we used a distribution
                        newPars->vtypeid = type->id;
                    }
                    RORouteDef* route = getRouteDef(pars->routeid)->copy("!" + newPars->id);
                    ROVehicle* veh = new ROVehicle(*newPars, route, type, this);
                    addVehicle(newPars->id, veh);
                    delete newPars;
                }
                pars->depart += DELTA_T;
            }
        } else {
            while (pars->repetitionsDone < pars->repetitionNumber) {
                SUMOTime depart = static_cast<SUMOTime>(pars->depart + pars->repetitionsDone * pars->repetitionOffset);
                if (myDepartures.find(pars->id) != myDepartures.end()) {
                    depart = myDepartures[pars->id].back();
                }
                if (depart >= time + DELTA_T) {
                    break;
                }
                if (myDepartures.find(pars->id) != myDepartures.end()) {
                    myDepartures[pars->id].pop_back();
                }
                SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
                newPars->id = pars->id + "." + toString(pars->repetitionsDone);
                newPars->depart = depart;
                pars->repetitionsDone++;
                // try to build the vehicle
                SUMOVTypeParameter* type = getVehicleTypeSecure(pars->vtypeid);
                if (type == 0) {
                    type = getVehicleTypeSecure(DEFAULT_VTYPE_ID);
                } else {
                    // fix the type id in case we used a distribution
                    newPars->vtypeid = type->id;
                }
                RORouteDef* route = getRouteDef(pars->routeid)->copy("!" + newPars->id);
                ROVehicle* veh = new ROVehicle(*newPars, route, type, this);
                addVehicle(newPars->id, veh);
                delete newPars;
            }
            if (pars->repetitionsDone == pars->repetitionNumber) {
                toRemove.push_back(i->first);
            }
        }
    }
    for (std::vector<std::string>::const_iterator i = toRemove.begin(); i != toRemove.end(); ++i) {
        myFlows.erase(*i);
    }
}


void
RONet::createBulkRouteRequests(SUMOAbstractRouter<ROEdge, ROVehicle>& router, const SUMOTime time, const bool removeLoops, const std::map<std::string, ROVehicle*>& mmap) {
    std::map<const unsigned int, std::vector<ROVehicle*> > bulkVehs;
    for (std::map<std::string, ROVehicle*>::const_iterator i = mmap.begin(); i != mmap.end(); ++i) {
        ROVehicle* const vehicle = i->second;
        if (vehicle->getDepart() < time) {
            const RORoute* const stub = vehicle->getRouteDefinition()->getFirstRoute();
            bulkVehs[stub->getFirst()->getNumericalID()].push_back(vehicle);
            ROVehicle* const first = bulkVehs[stub->getFirst()->getNumericalID()].front();
            if (first->getMaxSpeed() != vehicle->getMaxSpeed()) {
                WRITE_WARNING("Bulking different maximum speeds ('" + first->getID() + "' and '" + vehicle->getID() + "') may lead to suboptimal routes.");
            }
            if (first->getVClass() != vehicle->getVClass()) {
                WRITE_WARNING("Bulking different vehicle classes ('" + first->getID() + "' and '" + vehicle->getID() + "') may lead to invalid routes.");
            }
        }
    }
    int workerIndex = 0;
    for (std::map<const unsigned int, std::vector<ROVehicle*> >::const_iterator i = bulkVehs.begin(); i != bulkVehs.end(); ++i) {
#ifdef HAVE_FOX
        if (myThreadPool.size() > 0) {
            ROVehicle* const first = i->second.front();
            myThreadPool.add(new RoutingTask(first, removeLoops, myErrorHandler), workerIndex);
            myThreadPool.add(new BulkmodeTask(true), workerIndex);
            for (std::vector<ROVehicle*>::const_iterator j = i->second.begin() + 1; j != i->second.end(); ++j) {
                myThreadPool.add(new RoutingTask(*j, removeLoops, myErrorHandler), workerIndex);
            }
            myThreadPool.add(new BulkmodeTask(false), workerIndex);
            workerIndex++;
            if (workerIndex == (int)myThreadPool.size()) {
                workerIndex = 0;
            }
            continue;
        }
#endif
        for (std::vector<ROVehicle*>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
            (*j)->setRoutingSuccess(computeRoute(router, *j, removeLoops, myErrorHandler));
            router.setBulkMode(true);
        }
        router.setBulkMode(false);
    }
}


SUMOTime
RONet::saveAndRemoveRoutesUntil(OptionsCont& options, SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                                SUMOTime time) {
    checkFlows(time);
    SUMOTime lastTime = -1;
    const bool removeLoops = options.getBool("remove-loops");
#ifdef HAVE_FOX
    const int maxNumThreads = options.getInt("routing-threads");
#endif
    if (myVehicles.size() != 0) {
        const std::map<std::string, ROVehicle*>& mmap = myVehicles.getMyMap();
        if (options.getBool("bulk-routing")) {
#ifdef HAVE_FOX
            while ((int)myThreadPool.size() < maxNumThreads) {
                new WorkerThread(myThreadPool, myThreadPool.size() == 0 ? &router : router.clone());
            }
#endif
            createBulkRouteRequests(router, time, removeLoops, mmap);
        } else {
            for (std::map<std::string, ROVehicle*>::const_iterator i = mmap.begin(); i != mmap.end(); ++i) {
                ROVehicle* const vehicle = i->second;
                if (vehicle->getDepart() >= time) {
                    // we cannot go through a sorted list here, because the priority queue in the myVehicles container is not fully sorted
                    continue;
                }
#ifdef HAVE_FOX
                // add task
                if (maxNumThreads > 0) {
                    vehicle->setRoutingSuccess(false);
                    // add thread if necessary
                    const int numThreads = (int)myThreadPool.size();
                    if (numThreads < maxNumThreads && myThreadPool.isFull()) {
                        new WorkerThread(myThreadPool, numThreads == 0 ? &router : router.clone());
                    }
                    myThreadPool.add(new RoutingTask(vehicle, removeLoops, myErrorHandler));
                    continue;
                }
#endif
                vehicle->setRoutingSuccess(computeRoute(router, vehicle, removeLoops, myErrorHandler));
            }
        }
#ifdef HAVE_FOX
        myThreadPool.waitAll();
#endif
    }
    // write all vehicles (and additional structures)
    while (myVehicles.size() != 0 || myPersons.size() != 0 || myContainers.size() != 0) {
        // get the next vehicle, person or container
        const ROVehicle* const veh = myVehicles.getTopVehicle();
        const SUMOTime vehicleTime = veh == 0 ? SUMOTime_MAX : veh->getDepart();
        PersonMap::iterator person = myPersons.begin();
        const SUMOTime personTime = person == myPersons.end() ? SUMOTime_MAX : person->first;
        ContainerMap::iterator container = myContainers.begin();
        const SUMOTime containerTime = container == myContainers.end() ? SUMOTime_MAX : container->first;
        // check whether it shall not yet be computed
        if (vehicleTime >= time && personTime >= time && containerTime >= time) {
            lastTime = MIN3(vehicleTime, personTime, containerTime);
            break;
        }
        SUMOTime minTime = MIN3(vehicleTime, personTime, containerTime);
        if (vehicleTime == minTime) {
            // check whether to print the output
            if (lastTime != vehicleTime && lastTime != -1) {
                // report writing progress
                if (options.getInt("stats-period") >= 0 && ((int) vehicleTime % options.getInt("stats-period")) == 0) {
                    WRITE_MESSAGE("Read: " + toString(myReadRouteNo) + ",  Discarded: " + toString(myDiscardedRouteNo) + ",  Written: " + toString(myWrittenRouteNo));
                }
            }
            lastTime = vehicleTime;

            // ok, compute the route (try it)
            if (veh->getRoutingSuccess()) {
                // write the route
                veh->saveTypeAsXML(*myRoutesOutput, myRouteAlternativesOutput, myTypesOutput);
                veh->saveAllAsXML(*myRoutesOutput, false, options.getBool("exit-times"));
                if (myRouteAlternativesOutput != 0) {
                    veh->saveAllAsXML(*myRouteAlternativesOutput, true, options.getBool("exit-times"));
                }
                myWrittenRouteNo++;
            } else {
                myDiscardedRouteNo++;
            }
            // delete routes and the vehicle
            if (veh->getRouteDefinition()->getID()[0] == '!') {
                if (!myRoutes.erase(veh->getRouteDefinition()->getID())) {
                    delete veh->getRouteDefinition();
                }
            }
            myVehicles.erase(veh->getID());
        }
        if (personTime == minTime) {
            myRoutesOutput->writePreformattedTag(person->second);
            if (myRouteAlternativesOutput != 0) {
                myRouteAlternativesOutput->writePreformattedTag(person->second);
            }
            myPersons.erase(person);
        }
        if (containerTime == minTime) {
            myRoutesOutput->writePreformattedTag(container->second);
            if (myRouteAlternativesOutput != 0) {
                myRouteAlternativesOutput->writePreformattedTag(container->second);
            }
            myContainers.erase(container);
        }
    }
    return lastTime;
}


bool
RONet::furtherStored() {
    return myVehicles.size() > 0 || myFlows.size() > 0 || myPersons.size() > 0 || myContainers.size() > 0;
}


size_t
RONet::getEdgeNo() const {
    return myEdges.size();
}


int
RONet::getInternalEdgeNumber() const {
    return myNumInternalEdges;
}


const std::map<std::string, ROEdge*>&
RONet::getEdgeMap() const {
    return myEdges.getMyMap();
}


bool
RONet::hasPermissions() const {
    return myHavePermissions;
}


void
RONet::setPermissionsFound() {
    myHavePermissions = true;
}


#ifdef HAVE_FOX
// ---------------------------------------------------------------------------
// RONet::RoutingTask-methods
// ---------------------------------------------------------------------------
void
RONet::RoutingTask::run(FXWorkerThread* context) {
    myVehicle->setRoutingSuccess(RONet::computeRoute(static_cast<WorkerThread*>(context)->getRouter(), myVehicle, myRemoveLoops, myErrorHandler));
}
#endif


/****************************************************************************/

