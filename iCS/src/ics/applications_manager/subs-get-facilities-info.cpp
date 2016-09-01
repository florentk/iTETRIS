/****************************************************************************/
/// @file    subs-get-facilities-info.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    December 3rd, 2010
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

#include "subs-get-facilities-info.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "subscriptions-helper.h"

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
int
SubsGetFacilitiesInfo::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
{
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it=subscriptions->begin() ; it<subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsGetFacilitiesInfo*)) {
            SubsGetFacilitiesInfo* subsGetFacilitiesInfo = static_cast<SubsGetFacilitiesInfo*>(sub);
            if (subsGetFacilitiesInfo->m_nodeId == stationID) {
                delete subsGetFacilitiesInfo;
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
SubsGetFacilitiesInfo::SubsGetFacilitiesInfo(int appId, ics_types::stationID_t stationId, std::vector<unsigned char> fields) : Subscription(stationId)
{
    m_id = ++m_subscriptionCounter;

    m_name = "RETURN INFORMATION ABOUT THE POSITION OF A NODE";

    m_appId = appId;

    m_subscribedInformation = fields;
}

SubsGetFacilitiesInfo::~SubsGetFacilitiesInfo() { }

std::vector<unsigned char>
SubsGetFacilitiesInfo::getFacilitiesInformation()
{
    std::vector<unsigned char> info;
    unsigned int index=0;

    unsigned int numBlocks = m_subscribedInformation.at(index++);
    for (unsigned int i = 0; i < numBlocks; i++) {
        unsigned char type = m_subscribedInformation.at(index++);
        unsigned short numFields = (short) m_subscribedInformation.at(index++);
        switch (type) {
        case TYPE_TOPO: {
            getTopologicalInformation(index, numFields, &info);
            index += numFields;
            break;
        }
        case TYPE_RECEIVED_CAM: {
            getReceivedCamInformation(index, numFields, &info);
            index += numFields;
            break;
        }
        default:
            cout << "[ICS][INFO]SubsGetFacilitiesInfo::getFacilitiesInformation() - unknown type: " << type << endl;
        }
    }
    return info;
}


void SubsGetFacilitiesInfo::getTopologicalInformation(unsigned int startPos, unsigned short numFields, std::vector<unsigned char>* info)
{
    float x, y;
    speed_t speed;
    direction_t direction;
    acceleration_t acceleration;
    roadElementID_t laneID;
    roadElementID_t edgeID;
    roadElementID_t junctionID;
//    cout << "[ICS][INFO]SubsGetFacilitiesInfo::getTopologicalInformation - NodeID: " << m_nodeId << " numFields: " << numFields << endl;

#ifdef LOG_ON
    unsigned int initialInfoLen = info->size();
#endif

    for (int i=0; i<numFields; i++) {
        unsigned char field = m_subscribedInformation.at(i+startPos);
        switch (field) {
        case VALUE__POS_X: {
            info->push_back(VALUE__POS_X);
            x = SyncManager::m_facilitiesManager->getStationPositionX(m_nodeId);
            std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpretFloat(x);
            info->insert(info->end(), tmp.begin(), tmp.end());
            break;
        }
        case VALUE__POS_Y: {
            info->push_back(VALUE__POS_Y);
            y = SyncManager::m_facilitiesManager->getStationPositionY(m_nodeId);
            std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpretFloat(y);
            info->insert(info->end(), tmp.begin(), tmp.end());
            break;
        }
        case VALUE__SPEED: {
            info->push_back(VALUE__SPEED);
            speed = 0.0;
            if (SyncManager::m_facilitiesManager->getStationType(m_nodeId) == STATION_MOBILE)
                speed = SyncManager::m_facilitiesManager->getMobileStationSpeed(m_nodeId);
            std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpretFloat(speed);
            info->insert(info->end(), tmp.begin(), tmp.end());
            break;
        }
        case VALUE__DIRECTION: {
            if (SyncManager::m_facilitiesManager->getStationType(m_nodeId) == STATION_MOBILE) {
                info->push_back(VALUE__DIRECTION);
                speed = SyncManager::m_facilitiesManager->getMobileStationDirection(m_nodeId);
                std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpretFloat(speed);
                info->insert(info->end(), tmp.begin(), tmp.end());
            }
            break;
        }
        case VALUE__ACCELERATION: {
            if (SyncManager::m_facilitiesManager->getStationType(m_nodeId) == STATION_MOBILE) {
                info->push_back(VALUE__ACCELERATION);
                acceleration = SyncManager::m_facilitiesManager->getMobileStationAcceleration(m_nodeId);
                std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpretFloat(acceleration);
                info->insert(info->end(), tmp.begin(), tmp.end());
            }
            break;
        }
        case VALUE__LANE_ID:
        case VALUE__EDGE_ID:
        case VALUE__JUNCTION_ID: {
            Point2D pos = SyncManager::m_facilitiesManager->getStationPosition(m_nodeId);
            laneID = SyncManager::m_facilitiesManager->convertPoint2LaneID(pos);
            edgeID = SyncManager::m_facilitiesManager->getEdgeIDFromLane(laneID);
            junctionID = SyncManager::m_facilitiesManager->getJunctionIDFromLane(laneID);
            if (field == VALUE__LANE_ID) {
                info->push_back(VALUE__LANE_ID);
                std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpreteString(laneID);
                info->insert(info->end(), tmp.begin(), tmp.end());
            }
            if (field == VALUE__EDGE_ID) {
                info->push_back(VALUE__EDGE_ID);
                std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpreteString(edgeID);
                info->insert(info->end(), tmp.begin(), tmp.end());
            }
            if (field == VALUE__JUNCTION_ID) {
                info->push_back(VALUE__JUNCTION_ID);
                std::vector<unsigned char> tmp = SubscriptionsHelper::reinterpreteString(junctionID);
                info->insert(info->end(), tmp.begin(), tmp.end());
            }
            break;
        }
        default: {
            std::cout << "[ICS][getTopologicalInformation] - Impossible to retrieve topological information with code: " << field << endl;
            break;
        }
        }
    }

#ifdef LOG_ON
    stringstream log;
    log << "[INFO] getTopologicalInformation() - NodeID: " << m_nodeId << " -> #WrittenBytes: " << info->size()-initialInfoLen << " |";
//    cout << "[ICS][INFO] getTopologicalInformation() - NodeID: " << m_nodeId << " -> #WrittenBytes: " << info->size()-initialInfoLen << " |" << endl;
    for (int i=0; i<numFields; i++) {
        unsigned char field = m_subscribedInformation.at(i+startPos);
        switch (field) {
        case VALUE__POS_X: {
            log << " X: " << x << " |";
            break;
        }
        case VALUE__POS_Y: {
            log << " Y: " << y << " |";
            break;
        }
        case VALUE__SPEED: {
            log << " SPEED: " << speed << " |";
            break;
        }
        case VALUE__DIRECTION: {
            log << " DIRECTION: " << direction << " |";
            break;
        }
        case VALUE__ACCELERATION: {
            log << " ACCELERATION: " << acceleration << " |";
            break;
        }
        case VALUE__LANE_ID: {
            log << " LANEID: " << laneID << " |";
            break;
        }
        case VALUE__EDGE_ID: {
            log << " EDGEID: " << edgeID << " |";
            break;
        }
        case VALUE__JUNCTION_ID: {
            log << " JUNCTIONID: " << junctionID << " |";
            break;
        }
        }
    }
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
}

void SubsGetFacilitiesInfo::getReceivedCamInformation(unsigned int startPos, unsigned short numFields, std::vector<unsigned char>* info)
{
    //TODO: to be implemented
    std::cout << "[ICS] Subscription SubsGetFacilitiesInfo cannot return information about the received CAM messages yet." << std::endl;
}


short int SubsGetFacilitiesInfo::getNumberOfSubscribedFields()
{
    return (short int) m_subscribedInformation.size();
}

}
