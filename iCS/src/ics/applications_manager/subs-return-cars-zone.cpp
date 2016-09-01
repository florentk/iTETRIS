/****************************************************************************/
/// @file    subs-return-cars-zone.cpp
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

#include <typeinfo>
#include <math.h>

#include "subs-return-cars-zone.h"
#include "../itetris-node.h"
#include "../vehicle-node.h"
#include "../ics.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "../../utils/ics/geometric/Circle.h"

using namespace std;
using namespace ics_types;

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
int
SubsReturnsCarInZone::Delete(float baseX, float baseY, float radius, vector<Subscription*>* subscriptions)
{
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it=subscriptions->begin() ; it<subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsReturnsCarInZone*)) {
            SubsReturnsCarInZone* subsReturnCarInZone = static_cast<SubsReturnsCarInZone*>(sub);
            float x, y, r;
            x = subsReturnCarInZone->m_baseX;
            y = subsReturnCarInZone->m_baseY;
            r = subsReturnCarInZone->m_radius;
            if ((x==baseX) && (y==baseY) && (r==radius)) {
                delete subsReturnCarInZone;
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
SubsReturnsCarInZone::SubsReturnsCarInZone(int appId, stationID_t stationId, float baseX, float baseY, float radius) : Subscription(stationId)
{
    m_id = ++m_subscriptionCounter;
    m_name = "RETURN CARS IN ZONE";
    m_appId = appId;
    m_baseX = baseX;
    m_baseY = baseY;
    m_radius = radius;
}

SubsReturnsCarInZone::~SubsReturnsCarInZone() { }

vector<VehicleNode*>*
SubsReturnsCarInZone::GetCarsInZone(vector<VehicleNode*>* vehicles)
{
    if (vehicles == NULL) {
        cout << "[GetCarsInZone] [ERROR] Input parameter is NULL" << endl;
        return NULL;
    }

    // Container to return the nodes in zone
    vector<VehicleNode*>* nodesInZone = new vector<VehicleNode*>();

    // Get the IDs of the nodes in area from the facilities
    Point2D point(m_baseX, m_baseY);
    Circle area(point, m_radius);
    vector<stationID_t>* nodesInArea = SyncManager::m_facilitiesManager->getMobileStationsIDsInArea(area);
    if (nodesInArea->size() == 0) {

        stringstream log;
        log << "[INFO] GetCarsInZone() There are 0 nodes of total " << vehicles->size()  << " vehicles in the area according to the Facilities";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        delete nodesInArea;
        return nodesInZone;
    }

    else {
        stringstream log;
        log << "[INFO] GetCarsInZone() There are in area nodes " << nodesInArea->size();
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
    }

     stringstream log1;
     log1 << "[INFO] GetCarsInZone() Starting the Lookup ";
     IcsLog::LogLevel((log1.str()).c_str(), kLogLevelInfo);
    // Process to get the pointer to the vehicle
    // Loop all the vehicles in iTETRIS
    for (vector<VehicleNode*>::iterator nodeIt=vehicles->begin() ; nodeIt < vehicles->end() ; nodeIt++) {
        VehicleNode* vehicle = *nodeIt;
        stringstream log1;
	log1 << "[INFO] GetCarsInZone() Full Vehicle List: Vehicle ID " << vehicle->m_icsId;
	IcsLog::LogLevel((log1.str()).c_str(), kLogLevelInfo);
        // Loop the IDs of the vehicles in the area
        for (vector<stationID_t>::iterator areaNodesIt=nodesInArea->begin() ; areaNodesIt < nodesInArea->end() ; areaNodesIt++) {
            stationID_t nodeInAreaID = *areaNodesIt;
            stringstream log2;
	    log2 << "[INFO] GetCarsInZone() Nodes in Area: Vehicle ID " << nodeInAreaID;
	    IcsLog::LogLevel((log2.str()).c_str(), kLogLevelInfo);
            if (nodeInAreaID ==  vehicle->m_icsId) { // If the IDs match, include the pointer in the collection to return
	    stringstream log3;
	    log3 << "[INFO] GetCarsInZone() Logging node " << nodeInAreaID;
	    IcsLog::LogLevel((log3.str()).c_str(), kLogLevelInfo);
                nodesInZone->push_back(vehicle);
            }
        }
    }
    delete nodesInArea;
    return nodesInZone;
}

int
SubsReturnsCarInZone::ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager)
{
    IcsLog::LogLevel("[WARNING] ProcessReceivedGeobroadcastMessage() SubsReturnsCarInZone not based on Geobroadcasting", kLogLevelWarning);

    return EXIT_SUCCESS;
}

int
SubsReturnsCarInZone::ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message)
{
    stringstream log;
    log << "[WARNING] ProcessReceivedUnicastMessage() SubsReturnsCarInZone not based on Unicasting";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    return EXIT_SUCCESS;
}

}
