/****************************************************************************/
/// @file    subs-set-cam-area.cpp
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

#include <typeinfo>
#include <math.h>

#include "subs-set-cam-area.h"
#include "app-commands-subscriptions-constants.h"
#include "../itetris-node.h"
#include "../ics.h"
#include "../../utils/ics/geometric/Circle.h"
#include "../sync-manager.h"
#include "../../utils/ics/iCStypes.h"
#include "../../utils/ics/log/ics-log.h"

using namespace std;

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
vector<TGeneralCamSubscription> SubsSetCamArea::generalCamSubscriptions;

int
SubsSetCamArea::Delete(float baseX, float baseY, float radius, float frequency, vector<Subscription*>* subscriptions)
{
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it=subscriptions->begin() ; it<subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsSetCamArea*)) {
            SubsSetCamArea* subsSetCamArea = static_cast<SubsSetCamArea*>(sub);
            float x, y, r, f;
            x = subsSetCamArea->m_baseX;
            y = subsSetCamArea->m_baseY;
            r = subsSetCamArea->m_radius;
            f = subsSetCamArea->m_frequency;
            if ((x==baseX) && (y==baseY) && (r==radius) && (f==frequency)) {
                delete subsSetCamArea;
                return EXIT_SUCCESS;
            }
        }
    }

    return EXIT_SUCCESS;
}

bool
SubsSetCamArea::isNodeInGeneralCamSubscriptionVector(ITetrisNode* node)
{
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (node->m_icsId == it->stationId)
            return true;
    }
    return false;
}

float
SubsSetCamArea::getCamFrequencyFromCamSubscriptionVector(ITetrisNode* node)
{
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (node->m_icsId == it->stationId)
            return it->maxFrequency;
    }
    return 0;
}

unsigned int
SubsSetCamArea::getCamPayloadLengthFromCamSubscriptionVector(ITetrisNode* node)
{
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (node->m_icsId == it->stationId)
            return it->maxPayloadLength;
    }
    return 0;
}

void
SubsSetCamArea::setNewCamFrequencyInCamSubscriptionVector(ITetrisNode* node, float newFrequency)
{
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (node->m_icsId == it->stationId) {
            it->maxFrequency = newFrequency;
            return;
        }
    }
    return;
}

void
SubsSetCamArea::setNewPayloadLengthInCamSubscriptionVector(ITetrisNode* node, float newPayloadLength)
{
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (node->m_icsId == it->stationId) {
            it->maxPayloadLength = newPayloadLength;
            return;
        }
    }
    return;
}

bool
SubsSetCamArea::removeNodeFromCamSubscriptionVector(stationID_t nodeId)
{
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (nodeId == it->stationId) {
            generalCamSubscriptions.erase(it);
            return true;
        }
    }
    return false;
}

void
SubsSetCamArea::addNodeToCamSubscriptionVector(ITetrisNode* node, float maxFrequency, unsigned int maxPayloadLength)
{
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (node->m_icsId == it->stationId) {
            it->maxFrequency = maxFrequency;
            it->maxPayloadLength = maxPayloadLength;
            return;
        }
    }
    TGeneralCamSubscription sub;
    sub.stationId = node->m_icsId;
    sub.maxFrequency = maxFrequency;
    sub.maxPayloadLength = maxPayloadLength;
    sub.node = node;
    generalCamSubscriptions.push_back(sub);
    return;
}

float
SubsSetCamArea::getMaxFrequencyFromCamSubscriptionVector()
{
    float maxFreq = 0;
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (maxFreq < it->maxFrequency)
            maxFreq = it->maxFrequency;
    }
    return maxFreq;
}
unsigned int
SubsSetCamArea::getMaxPayloadLengthFromCamSubscriptionVector()
{
    unsigned int maxLength = 0;
    vector<TGeneralCamSubscription>::iterator it;
    for (it = generalCamSubscriptions.begin(); it != generalCamSubscriptions.end(); it++) {
        if (maxLength < it->maxPayloadLength)
            maxLength = it->maxPayloadLength;
    }
    return maxLength;
}


// ===========================================================================
// member method definitions
// ===========================================================================
SubsSetCamArea::SubsSetCamArea(int appId, stationID_t stationId, float baseX, float baseY, float radius, float frequency, int infoType) : Subscription(stationId)
{
    m_id = ++m_subscriptionCounter;

    m_name = "SET CAM AREA";

    m_appId = appId;
    m_baseX = baseX;
    m_baseY = baseY;
    m_radius = radius;
    m_frequency = frequency;
    m_nodesInArea = new vector<ITetrisNode*>();
    m_nodeId = stationId;
}

SubsSetCamArea::~SubsSetCamArea()
{
    m_nodesInArea = NULL;
}

int
SubsSetCamArea::ScanArea(SyncManager* syncManager)
{
    if (syncManager == NULL)  {
        cout << "iCS --> SyncManager is null" << endl;
        return EXIT_FAILURE;
    }

    Circle circle(Point2D(m_baseX, m_baseY), m_radius);

    m_nodesInArea = new vector<ITetrisNode*>();
    vector<ITetrisNode*>* nodes = syncManager->m_iTetrisNodeCollection;
    for (vector<ITetrisNode*>::iterator nodeIt=nodes->begin() ; nodeIt!=nodes->end() ; nodeIt++) {
        Point2D position((*nodeIt)->GetPositionX(), (*nodeIt)-> GetPositionY());

        if (circle.isInternal(position)) {
            AddNodeToVector((*nodeIt));
        }
    }
    return EXIT_SUCCESS;
}

unsigned int
SubsSetCamArea::GetFrequency()
{
    return m_frequency;
}

void
SubsSetCamArea::RemoveNodeFromVector(ITetrisNode* node)
{
    vector<ITetrisNode*>::iterator nodeIt;
    for (nodeIt=m_nodesInArea->begin() ; nodeIt < m_nodesInArea->end() ; nodeIt++) {

        if ((*nodeIt)==node) {
            m_nodesInArea->erase(nodeIt);
            break;
        }
    }
}

void
SubsSetCamArea::AddNodeToVector(ITetrisNode* node)
{
    bool alreadyPresent = false;
    vector<ITetrisNode*>::iterator nodeIt;
    for (nodeIt=m_nodesInArea->begin() ; nodeIt < m_nodesInArea->end() ; nodeIt++) {
        if ((*nodeIt)==node) {
            alreadyPresent = true;
            break;
        }
    }
    if (!alreadyPresent)
        m_nodesInArea->push_back(node);
}

bool
SubsSetCamArea::IsNodeInVector(ITetrisNode* node)
{
    vector<ITetrisNode*>::iterator nodeIt;
    for (nodeIt=m_nodesInArea->begin() ; nodeIt < m_nodesInArea->end() ; nodeIt++) {
        if ((*nodeIt)==node)
            return true;
    }
    return false;
}

int
SubsSetCamArea::ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager)
{
#ifdef LOG_ON
    stringstream log;
    log << "[WARNING] ProcessReceivedGeobroadcastMessage() SubsSetCamArea not based on Geobroadcasting";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif

    return EXIT_SUCCESS;
}

int
SubsSetCamArea::ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message)
{
#ifdef LOG_ON
    stringstream log;
    log << "[WARNING] ProcessReceivedUnicastMessage() SubsSetCamArea not based on Unicasting";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
    return EXIT_SUCCESS;
}

nodeStatusInArea_t
SubsSetCamArea::checkNodeStatus(ITetrisNode* node)
{
    Circle circle(Point2D(m_baseX, m_baseY), m_radius);
    bool nowIsInternal = circle.isInternal(Point2D(node->GetPositionX(), node->GetPositionY()));
    bool wasAlreadyInternal = IsNodeInVector(node);
    if (wasAlreadyInternal && nowIsInternal)
        return StillInsideArea;
    if (!wasAlreadyInternal && nowIsInternal)
        return JustArrivedInArea;
    if (!wasAlreadyInternal && !nowIsInternal)
        return StillOutsideArea;
    if (wasAlreadyInternal && !nowIsInternal)
        return JustLeftArea;

    return UnknownStatus;   // Rises an error!
}

std::vector<ITetrisNode*>*
SubsSetCamArea::getNodesInArea()
{
    return m_nodesInArea;
}

}
