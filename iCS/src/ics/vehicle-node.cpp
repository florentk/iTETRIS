/****************************************************************************/
/// @file    vehicle-node.cpp
/// @author  Julen Maneros
/// @date
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

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "vehicle-node.h"
#include "sync-manager.h"
#include "FacilitiesManager.h"
#include "../utils/ics/geometric/Point2D.h"
#include "../utils/ics/log/ics-log.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace ics_types;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
VehicleNode::VehicleNode(const std::string &nodeId) : ITetrisNode()
{
    m_nsId = 0;

    // If the iCS ID already was assigned to a node
    bool validID = false;
    set<stationID_t>::iterator it;
    while (validID == false) {
        it = m_preAssignedIds.find(m_idCounter);
        if (it != m_preAssignedIds.end()) {
            m_icsId = ++m_idCounter;
            validID = false;
        } else {
            m_icsId = m_idCounter;
            validID = true;
        }
    }

    m_tsId = nodeId;
    m_moved = false;
    m_type = staType_CAR;
    m_lastSpeed = -1;
    m_currentSpeed = -1;

    stringstream log;
    log << "VehicleNode() Created vehicle with iCS ID: " << m_icsId;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
}

float
VehicleNode::ChangeSpeed(float speed)
{
    float acceleration;
    m_lastSpeed = m_currentSpeed;
    m_currentSpeed = speed;

    if (m_lastSpeed == -1) {
        acceleration = m_currentSpeed / 1;
    } else {
        acceleration = (m_currentSpeed - m_lastSpeed) / 1;
    }

    return acceleration;
}

bool
VehicleNode::CheckPosition(pair<float, float> position)
{
    Point2D current2D = SyncManager::m_facilitiesManager->getStationPosition(m_icsId);
    pair<float, float> currentPos(current2D.x(), current2D.y());
    if (currentPos == position) {
        m_moved = false;
        return false;
    } else {
        m_moved = true;
        return true;
    }
}

float
VehicleNode::GetSpeed()
{
    return SyncManager::m_facilitiesManager->getMobileStationSpeed(m_icsId);
}

float
VehicleNode::GetHeading()
{
    return SyncManager::m_facilitiesManager->getMobileStationDirection(m_icsId);
}

}
