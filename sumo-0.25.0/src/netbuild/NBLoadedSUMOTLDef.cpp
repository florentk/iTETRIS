/****************************************************************************/
/// @file    NBLoadedSUMOTLDef.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id: NBLoadedSUMOTLDef.cpp 19535 2015-12-05 13:47:18Z behrisch $
///
// A complete traffic light logic loaded from a sumo-net. (opted to reimplement
// since NBLoadedTLDef is quite vissim specific)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2011-2015 DLR (http://www.dlr.de/) and contributors
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
#include <set>
#include <cassert>
#include <iterator>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "NBTrafficLightLogic.h"
#include "NBOwnTLDef.h"
#include "NBTrafficLightDefinition.h"
#include "NBLoadedSUMOTLDef.h"
#include "NBNode.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================

NBLoadedSUMOTLDef::NBLoadedSUMOTLDef(const std::string& id, const std::string& programID,
                                     SUMOTime offset, TrafficLightType type) :
    NBTrafficLightDefinition(id, programID, offset, type),
    myTLLogic(0) {
    myTLLogic = new NBTrafficLightLogic(id, programID, 0, offset, type);
}


NBLoadedSUMOTLDef::NBLoadedSUMOTLDef(NBTrafficLightDefinition* def, NBTrafficLightLogic* logic) :
    // allow for adding a new program for the same def: take the programID from the new logic
    NBTrafficLightDefinition(def->getID(), logic->getProgramID(), def->getOffset(), def->getType()),
    myTLLogic(new NBTrafficLightLogic(logic)),
    myOriginalNodes(def->getNodes().begin(), def->getNodes().end()) {
    assert(def->getOffset() == logic->getOffset());
    assert(def->getType() == logic->getType());
    myControlledLinks = def->getControlledLinks();
    myControlledNodes = def->getNodes();
}


NBLoadedSUMOTLDef::~NBLoadedSUMOTLDef() {
    delete myTLLogic;
}


NBTrafficLightLogic*
NBLoadedSUMOTLDef::myCompute(const NBEdgeCont& ec, unsigned int brakingTimeSeconds) {
    // @todo what to do with those parameters?
    UNUSED_PARAMETER(ec);
    UNUSED_PARAMETER(brakingTimeSeconds);
    myTLLogic->closeBuilding();
    patchIfCrossingsAdded();
    return new NBTrafficLightLogic(myTLLogic);
}


void
NBLoadedSUMOTLDef::addConnection(NBEdge* from, NBEdge* to, int fromLane, int toLane, int linkIndex) {
    assert(myTLLogic->getNumLinks() > 0); // logic should be loaded by now
    if (linkIndex >= (int)myTLLogic->getNumLinks()) {
        throw ProcessError("Invalid linkIndex " + toString(linkIndex) + " for traffic light '" + getID() +
                           "' with " + toString(myTLLogic->getNumLinks()) + " links.");
    }
    NBConnection conn(from, fromLane, to, toLane, linkIndex);
    // avoid duplicates
    remove_if(myControlledLinks.begin(), myControlledLinks.end(), connection_equal(conn));
    myControlledLinks.push_back(conn);
    addNode(from->getToNode());
    addNode(to->getFromNode());
    myOriginalNodes.insert(from->getToNode());
    myOriginalNodes.insert(to->getFromNode());
    // added connections are definitely controlled. make sure none are removed because they lie within the tl
    // myControlledInnerEdges.insert(from->getID()); // @todo recheck: this appears to be obsolete
    // set this information now so that it can be used while loading diffs
    from->setControllingTLInformation(conn, getID());
}


void
NBLoadedSUMOTLDef::setTLControllingInformation(const NBEdgeCont&) const {
    setTLControllingInformation();
}


void
NBLoadedSUMOTLDef::setTLControllingInformation() const {
    // if nodes have been removed our links may have been invalidated as well
    // since no logic will be built anyway there is no reason to inform any edges
    if (amInvalid()) {
        return;
    }
    // set the information about the link's positions within the tl into the
    //  edges the links are starting at, respectively
    for (NBConnectionVector::const_iterator it = myControlledLinks.begin(); it != myControlledLinks.end(); it++) {
        const NBConnection& c = *it;
        if (c.getTLIndex() >= (int)myTLLogic->getNumLinks()) {
            throw ProcessError("Invalid linkIndex " + toString(c.getTLIndex()) + " for traffic light '" + getID() +
                               "' with " + toString(myTLLogic->getNumLinks()) + " links.");
        }
        NBEdge* edge = c.getFrom();
        edge->setControllingTLInformation(c, getID());
    }
}


void
NBLoadedSUMOTLDef::remapRemoved(NBEdge*, const EdgeVector&, const EdgeVector&) {}


void
NBLoadedSUMOTLDef::replaceRemoved(NBEdge* removed, int removedLane, NBEdge* by, int byLane) {
    for (NBConnectionVector::iterator it = myControlledLinks.begin(); it != myControlledLinks.end(); ++it) {
        (*it).replaceFrom(removed, removedLane, by, byLane);
        (*it).replaceTo(removed, removedLane, by, byLane);
    }
}


void
NBLoadedSUMOTLDef::addPhase(SUMOTime duration, const std::string& state) {
    myTLLogic->addStep(duration, state);
}


bool
NBLoadedSUMOTLDef::amInvalid() const {
    if (myControlledLinks.size() == 0) {
        return true;
    }
    // make sure that myControlledNodes are the original nodes
    if (myControlledNodes.size() != myOriginalNodes.size()) {
        return true;
    }
    for (std::vector<NBNode*>::const_iterator i = myControlledNodes.begin(); i != myControlledNodes.end(); i++) {
        if (myOriginalNodes.count(*i) != 1) {
            return true;
        }
    }
    return false;
}


void
NBLoadedSUMOTLDef::removeConnection(const NBConnection& conn, bool reconstruct) {
    NBConnectionVector::iterator it = myControlledLinks.begin();
    // find the connection but ignore its TLIndex since it might have been
    // invalidated by an earlier removal
    for (; it != myControlledLinks.end(); ++it) {
        if (it->getFrom() == conn.getFrom() &&
                it->getTo() == conn.getTo() &&
                it->getFromLane() == conn.getFromLane() &&
                it->getToLane() == conn.getToLane()) {
            break;
        }
    }
    if (it == myControlledLinks.end()) {
        // a traffic light doesn't always controll all connections at a junction
        // especially when using the option --tls.join
        return;
    }
    const int removed = it->getTLIndex();
    // remove the connection
    myControlledLinks.erase(it);
    if (reconstruct) {
        // updating the edge is only needed for immediate use in NETEDIT.
        // It may conflict with loading diffs
        conn.getFrom()->setControllingTLInformation(conn, "");
        // shift link numbers down so there is no gap
        for (NBConnectionVector::iterator it = myControlledLinks.begin(); it != myControlledLinks.end(); it++) {
            NBConnection& c = *it;
            if (c.getTLIndex() > removed) {
                c.setTLIndex(c.getTLIndex() - 1);
            }
        }
        // update controlling information with new link numbers
        setTLControllingInformation();
        // rebuild the logic
        const std::vector<NBTrafficLightLogic::PhaseDefinition> phases = myTLLogic->getPhases();
        NBTrafficLightLogic* newLogic = new NBTrafficLightLogic(getID(), getProgramID(), 0, myOffset, myType);
        for (std::vector<NBTrafficLightLogic::PhaseDefinition>::const_iterator it = phases.begin(); it != phases.end(); it++) {
            std::string newState = it->state;
            newState.erase(newState.begin() + removed);
            newLogic->addStep(it->duration, newState);
        }
        delete myTLLogic;
        myTLLogic = newLogic;
    }
}


void
NBLoadedSUMOTLDef::setOffset(SUMOTime offset) {
    myOffset = offset;
    myTLLogic->setOffset(offset);
}


void
NBLoadedSUMOTLDef::setType(TrafficLightType type) {
    myType = type;
    myTLLogic->setType(type);
}


void
NBLoadedSUMOTLDef::collectEdges() {
    if (myControlledLinks.size() == 0) {
        NBTrafficLightDefinition::collectEdges();
    }
    myIncomingEdges.clear();
    EdgeVector myOutgoing;
    // collect the edges from the participating nodes
    for (std::vector<NBNode*>::iterator i = myControlledNodes.begin(); i != myControlledNodes.end(); i++) {
        const EdgeVector& incoming = (*i)->getIncomingEdges();
        copy(incoming.begin(), incoming.end(), back_inserter(myIncomingEdges));
        const EdgeVector& outgoing = (*i)->getOutgoingEdges();
        copy(outgoing.begin(), outgoing.end(), back_inserter(myOutgoing));
    }
    // check which of the edges are completely within the junction
    // and which are uncontrolled as well (we already know myControlledLinks)
    for (EdgeVector::iterator j = myIncomingEdges.begin(); j != myIncomingEdges.end();) {
        NBEdge* edge = *j;
        // an edge lies within the logic if it is outgoing as well as incoming
        EdgeVector::iterator k = find(myOutgoing.begin(), myOutgoing.end(), edge);
        if (k != myOutgoing.end()) {
            if (myControlledInnerEdges.count(edge->getID()) == 0) {
                bool controlled = false;
                for (NBConnectionVector::iterator it = myControlledLinks.begin(); it != myControlledLinks.end(); it++) {
                    if ((*it).getFrom() == edge) {
                        controlled = true;
                        break;
                    }
                }
                if (controlled) {
                    myControlledInnerEdges.insert(edge->getID());
                } else {
                    myEdgesWithin.push_back(edge);
                    (*j)->setIsInnerEdge();
                    ++j; //j = myIncomingEdges.erase(j);
                    continue;
                }
            }
        }
        ++j;
    }
}


void
NBLoadedSUMOTLDef::collectLinks() {
    if (myControlledLinks.size() == 0) {
        // maybe we only loaded a different program for a default traffic light.
        // Try to build links now.
        myOriginalNodes.insert(myControlledNodes.begin(), myControlledNodes.end());
        collectAllLinks();
    }
}


/// @brief patches signal plans by modifying lane indices
void
NBLoadedSUMOTLDef::shiftTLConnectionLaneIndex(NBEdge* edge, int offset) {
    // avoid shifting twice if the edge is incoming and outgoing to a joined TLS
    if (myShifted.count(edge) == 0) {
        /// XXX what if an edge should really be shifted twice?
        myShifted.insert(edge);
        for (NBConnectionVector::iterator it = myControlledLinks.begin(); it != myControlledLinks.end(); it++) {
            (*it).shiftLaneIndex(edge, offset);
        }
    }
}

void
NBLoadedSUMOTLDef::patchIfCrossingsAdded() {
    // XXX what to do if crossings are removed during network building?
    const unsigned int size = myTLLogic->getNumLinks();
    unsigned int noLinksAll = 0;
    for (NBConnectionVector::const_iterator it = myControlledLinks.begin(); it != myControlledLinks.end(); it++) {
        const NBConnection& c = *it;
        if (c.getTLIndex() != NBConnection::InvalidTlIndex) {
            noLinksAll = MAX2(noLinksAll, (unsigned int)c.getTLIndex() + 1);
        }
    }
    int oldCrossings = 0;
    // collect crossings
    std::vector<NBNode::Crossing> crossings;
    for (std::vector<NBNode*>::iterator i = myControlledNodes.begin(); i != myControlledNodes.end(); i++) {
        const std::vector<NBNode::Crossing>& c = (*i)->getCrossings();
        // set tl indices for crossings
        (*i)->setCrossingTLIndices(noLinksAll);
        copy(c.begin(), c.end(), std::back_inserter(crossings));
        noLinksAll += (unsigned int)c.size();
        oldCrossings += (*i)->numCrossingsFromSumoNet();
    }
    const int newCrossings = (int)crossings.size() - oldCrossings;
    if (newCrossings > 0) {
        const std::vector<NBTrafficLightLogic::PhaseDefinition> phases = myTLLogic->getPhases();
        if (phases.size() > 0) {
            if (phases.front().state.size() == noLinksAll - newCrossings) {
                // patch states for the newly added crossings

                // collect edges
                assert(size > 0);
                EdgeVector fromEdges(size, 0);
                EdgeVector toEdges(size, 0);
                for (NBConnectionVector::const_iterator it = myControlledLinks.begin(); it != myControlledLinks.end(); it++) {
                    const NBConnection& c = *it;
                    if (c.getTLIndex() != NBConnection::InvalidTlIndex) {
                        if (c.getTLIndex() >= (int)size) {
                            throw ProcessError("Invalid linkIndex " + toString(c.getTLIndex()) + " for traffic light '" + getID() +
                                               "' with " + toString(size) + " links.");
                        }


                        fromEdges[c.getTLIndex()] = c.getFrom();
                        toEdges[c.getTLIndex()] = c.getTo();
                    }
                }
                const std::string crossingDefaultState(newCrossings, 'r');

                // rebuild the logic (see NBOwnTLDef.cpp::myCompute)
                const std::vector<NBTrafficLightLogic::PhaseDefinition> phases = myTLLogic->getPhases();
                NBTrafficLightLogic* newLogic = new NBTrafficLightLogic(getID(), getProgramID(), 0, myOffset, myType);
                //std::cout << "patchIfCrossingsAdded for " << getID() << " numPhases=" << phases.size() << "\n";
                for (std::vector<NBTrafficLightLogic::PhaseDefinition>::const_iterator it = phases.begin(); it != phases.end(); it++) {
                    NBOwnTLDef::addPedestrianPhases(newLogic, it->duration, it->state + crossingDefaultState, crossings, fromEdges, toEdges);
                }
                delete myTLLogic;
                myTLLogic = newLogic;
            } else if (phases.front().state.size() != noLinksAll) {
                WRITE_WARNING("Could not patch tlLogic " + getID() + "for new crossings");
            }
        }
    }
}


/****************************************************************************/

