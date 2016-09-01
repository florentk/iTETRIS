/****************************************************************************/
/// @file    subs-app-result-traff-sim.cpp
/// @author  Jerome Haerri (EURECOM)
/// @date    December 15th, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>
#include <cstring>

#include "subs-app-result-traff-sim.h"
#include "subscriptions-helper.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"

namespace ics
{

// ===========================================================================
// Constants
// ===========================================================================

#define TYPE_ERROR			       			  0x00
#define TYPE_EDGE_TRAVELTIME       			  0x01
#define TYPE_EDGES_ID_FROM_ROAD_ID      	  0x02

// ===========================================================================
// static member definitions
// ===========================================================================
int
SubsAppResultTraffSim::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
{
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it=subscriptions->begin() ; it<subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsAppResultTraffSim*)) {
            SubsAppResultTraffSim* subsAppResultTraffSim = static_cast<SubsAppResultTraffSim*>(sub);
            if (subsAppResultTraffSim->m_nodeId == stationID) {
                delete subsAppResultTraffSim;
                delete sub;
                return EXIT_SUCCESS;
            }
        }
    }
    return EXIT_SUCCESS;
}

// ===========================================================================
// member method definitions
// ===========================================================================

SubsAppResultTraffSim::SubsAppResultTraffSim(int appId, ics_types::stationID_t stationId, unsigned char* msg, int msgSize) : Subscription(stationId), m_msg(msg, msgSize)
{
    // Read parameters
    m_id = ++m_subscriptionCounter;

    m_name = "SEND A Command To the Traffic Simulator";

    m_appId = appId;

    /* EURECOM-LG m_msg = msg*/
}

SubsAppResultTraffSim::~SubsAppResultTraffSim() {}

std::vector<unsigned char>
SubsAppResultTraffSim::pull(SyncManager* syncManager)
{
    std::vector<unsigned char> info;
    unsigned int index = 0;

    unsigned char appMsgType = m_msg.readChar();       // APP_MSG_TYPE
    unsigned char cmdMode = m_msg.readChar();         // Defines the command mode and thus the way the rest of the msg will be read.

    switch (cmdMode) {
    case VALUE_GET_EDGE_TRAVELTIME: {
        std::cout << "Command: GET_EDGE_TRAVELTIME" << std::endl;

        ics_types::roadElementID_t edgeID = "";

        ics_types::stationID_t m_iCS_ID = (ics_types::stationID_t) m_msg.readInt();
        unsigned int numWeights = (unsigned int) m_msg.readChar();

        for (unsigned int i = 0; i < numWeights; i++) {
            unsigned char typeEdgeID = m_msg.readChar();
            if (typeEdgeID == TYPE_EDGE_TRAVELTIME) {
                info.push_back(TYPE_EDGE_TRAVELTIME);
                edgeID = m_msg.readString();
                /*tcpip::Storage outMsg;
                ITetrisNode *node = syncManager->GetNodeByIcsId(m_iCS_ID);
                syncManager->m_trafficSimCommunicator->beginValueRetrieval(edgeID, VAR_EDGE_TRAVELTIME, outMsg, CMD_GET_EDGE_VARIABLE);
                float edgeTravelTime = outMsg.readFloat();*/
                float edgeTravelTime = syncManager->m_trafficSimCommunicator->GetEdgeWeight(edgeID);
                std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpretFloat(edgeTravelTime);
                info.insert(info.end(), tmp.begin(), tmp.end());
            } else {
                std::cout << "To send a GET_EDGE_TRAVELTIME command a list of EdgeIDs must be  specified." << std::endl;
                info.push_back(TYPE_ERROR);
                return info;
            }
        }
        break;
    }
    case VALUE_GET_ROUTE_VARIABLE: {
        std::cout << "Command: GET ROUTE VARIABLE" << std::endl;

        ics_types::roadElementID_t routeID = "";

        ics_types::stationID_t m_iCS_ID = (ics_types::stationID_t) m_msg.readInt();
        unsigned int numRoute = (unsigned int) m_msg.readChar();

        for (unsigned int i = 0; i < numRoute; i++) {
            unsigned char typeRouteID = m_msg.readChar();
            if (typeRouteID == TYPE_EDGES_ID_FROM_ROAD_ID) {
                info.push_back(TYPE_EDGES_ID_FROM_ROAD_ID);
                routeID = m_msg.readString();
                std::vector<std::string> routeEdges = syncManager->m_trafficSimCommunicator->GetRouteEdges(routeID);
                /*	tcpip::Storage outMsg;
                	ITetrisNode *node = syncManager->GetNodeByIcsId(m_iCS_ID);

                	syncManager->m_trafficSimCommunicator->beginValueRetrieval(node.m_tsId, VAR_EDGES, outMsg);
                 //SyncManager::m_facilitiesManager->beginValueRetrieval(*node, outMsg, routeID, CMD_GET_ROUTE_VARIABLE);

                 std::vector<std::string> routeEdges = outMsg.readStringList();*/
                for (unsigned int i = 0; i < routeEdges.size()-1; i++) {
                    string tmpString = routeEdges.at(i);
                    std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpreteString(tmpString);
                    info.insert(info.end(), tmp.begin(), tmp.end());
                }
                //std::vector<unsigned char> tmp = reinterpretString(edgeID);
                //info->insert(info->end(), tmp.begin(), tmp.end());
            } else {
                std::cout << "To send a GET_ROUTE_VARIABLE command a list of routeIDs must be  specified." << std::endl;
                info.push_back(TYPE_ERROR);
                return info;
            }
        }

        break;
    }
    default: {
        std::cout << "Impossible to send the command. Command not recognized." << std::endl;
        info.push_back(TYPE_ERROR);
        return info;
    }

    }
    return info;

    std::cout << "[DEB] - The message was correctly sent!!!" << std::endl;
}

} // end namespace ics
