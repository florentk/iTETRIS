/****************************************************************************/
/// @file    application-handler.cpp
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
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

#include "application-handler.h"
#include "subscription.h"
#include "subs-return-cars-zone.h"
#include "subs-set-cam-area.h"
#include "subs-start-travel-time-calculation.h"
#include "subs-stop-travel-time-calculation.h"
#include "subs-calculate-travel-time.h"
#include "subs-get-received-cam-info.h"
#include "subs-get-facilities-info.h"
#include "subs-app-message-send.h"
#include "subs-app-message-receive.h"
#include "subs-app-result-traff-sim.h"
#include "subs-app-cmd-traff-sim.h"
#include "subs-x-application-data.h"
#include "app-message-manager.h"
#include "app-result-maximum-speed.h"
#include "app-result-travel-time.h"
#include "app-result-void.h"
#include "app-result-generic.h"
#include "app-result-traffic-jam-detection.h"
#include "app-result-travel-time.h"
#include "subs-x-application-data.h"
#include "../itetris-node.h"
#include "../vehicle-node.h"
#include "../wirelesscom_sim_message_tracker/V2X-message-manager.h"
#include "../ics.h"
#include "../../utils/ics/log/ics-log.h"

using namespace std;

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
int ApplicationHandler::m_idCounter = 0;

// ===========================================================================
// member method definitions
// ===========================================================================
ApplicationHandler::ApplicationHandler(SyncManager* syncManager, string name, string host, string executable, int port, long seed, double rate, int resultType, ServiceId serviceIds)
{
    m_id = ++m_idCounter;
    m_syncManager = syncManager;
    m_name = name;
    m_executable = executable;
    m_host = host;
    m_port = port;
    m_seed = seed;
    m_rate = rate;
    m_resultType = resultType;
    m_serviceId = serviceIds;

    m_appMessageManager = new AppMessageManager(m_syncManager);
}

ApplicationHandler::ApplicationHandler(const ApplicationHandler& appHandler)
{
    m_id = appHandler.m_id;
    m_name = appHandler.m_name;
    m_host = appHandler.m_host;
    m_port = appHandler.m_port;
    m_rate = appHandler.m_rate;

    m_appMessageManager = new AppMessageManager(appHandler.m_syncManager);
}

ApplicationHandler::~ApplicationHandler()
{
    delete m_appMessageManager;
}

bool
ApplicationHandler::AskForNewSubscriptions(int nodeId, vector<Subscription*> *subscriptions)
{
    if (subscriptions == NULL)
        return false;

    bool noMoreSubscriptions = false;

    // Ask for subscriptions until the applications requests to stop
    while (!noMoreSubscriptions) {
        if (!m_appMessageManager->CommandGetNewSubscriptions(nodeId, m_id, subscriptions, noMoreSubscriptions)) {
            return false;
        }
    }

    return true;
}

bool
ApplicationHandler::AskForUnsubscriptions(int nodeId, vector<Subscription*>* subscriptions)
{
    if (subscriptions == NULL)
        return false;


    for (vector<Subscription*>::iterator it = subscriptions->begin() ; it != subscriptions->end() ; it++) {
        int status = m_appMessageManager->CommandUnsubscribe(nodeId, (*it));

        switch (status) {
        case 0: {
            stringstream log;
            log << "AskForUnsubscriptions() subscription in node [iCD-ID] [" << nodeId << "] is alive.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
            break;
        }
        case 1: {
            subscriptions->erase(it); // Performs the removal of the subscription from the collection
            it--;
            stringstream log;
            log << "AskForUnsubscriptions() unsubscribing in node [iCD-ID] [" << nodeId << "]";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
            break;
        }
        case -1: {
            stringstream log;
            log << "AskForUnsubscriptions() unsubscribing in node [iCD-ID] [" << nodeId << "]";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
            cerr << "iCS --> [ERROR] AskForUnsubscriptions() unsubscribing node [iCD-ID] [" << nodeId << "]" << endl;
            return false;
        }
        default:
            IcsLog::LogLevel("AskForUnsubscriptions() Unknown error code.", kLogLevelWarning);
            break;
        }
    }

    return true;
}

int
ApplicationHandler::SendSubscribedData(int nodeId, Subscription* subscription, vector<ITetrisNode*>* nodes)
{
    if (subscription == NULL || nodes == NULL) {
        cout << "iCS --> [ERROR] SendSubscribedData() Subscription or nodes are NULL." << endl;
        IcsLog::LogLevel("SendSubscribedData() Subscription or nodes are NULL.", kLogLevelError);
        return EXIT_FAILURE;
    }

    Subscription& sub = *subscription;
    const std::type_info& typeinfo = typeid(sub);

    if (typeinfo == typeid(SubsReturnsCarInZone)) { // Subscription to return car IDs in certain zone
#ifdef LOG_ON
        stringstream log;
        log << "iCS --> [ApplicationHanlder] - SendSubscribedData() - forward subscribed cars in zone to node "<< nodeId <<" ";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
        // From all the stations in the simulation just get the ones that are vehicles
        vector<VehicleNode*>* vehicles = new vector<VehicleNode*>();
        for (vector<ITetrisNode*>::iterator nodesIt = nodes->begin() ; nodesIt < nodes->end() ; nodesIt++) {
            ITetrisNode& node = *(*nodesIt);
            const std::type_info& typeinfo2 = typeid(node);
            if (typeinfo2 == typeid(VehicleNode)) {
                VehicleNode* vehicle = static_cast<VehicleNode*>((*nodesIt));
                vehicles->push_back(vehicle);
            }
        }

        // Check out from the vehicles which ones are in the zone
        SubsReturnsCarInZone* subCarsInZone = static_cast<SubsReturnsCarInZone*>(subscription);
        vector<VehicleNode*>* carsInZone = subCarsInZone->GetCarsInZone(vehicles);
        if (carsInZone == NULL) {
            cout << "iCS --> [ERROR] SendSubscribedData() Cars in zone are NULL." << endl;
            IcsLog::LogLevel("SendSubscribedData() SendSubscribedData() Cars in zone are NULL.", kLogLevelError);
            return EXIT_FAILURE;
        }
        if (carsInZone->size() > 0) {
	  
            if (!m_appMessageManager->CommandSendSubscriptionCarsInZone(carsInZone, nodeId)) {
                cout << "iCS --> [ERROR] SendSubscribedData() Error sending data of cars in zone." << endl;
                IcsLog::LogLevel("SendSubscribedData() SendSubscribedData() Error sending data of cars in zone.", kLogLevelError);
                delete carsInZone;
                return EXIT_FAILURE;
            } else {
	        cout << "iCS --> AppHandler SendSubscribedData() Cars in subscribed zones are "<< carsInZone->size() << endl;
                delete carsInZone;
                return EXIT_SUCCESS;
            }
        } else {
            if (carsInZone->size() == 0) {
                IcsLog::LogLevel("SendSubscribedData() Cars in zone are 0.", kLogLevelInfo);
                delete carsInZone;
                return EXIT_SUCCESS;
            } else {
                cout << "[ERROR] SendSubscribedData() Returned cars in zone value is negative." << endl;
                IcsLog::LogLevel("SendSubscribedData() Returned cars in zone value is negative.", kLogLevelError);
                return EXIT_FAILURE;
            }
        }
    }

    if (typeinfo == typeid(SubsSetCamArea)) {  // Subscription of setting a CAM beaconing area
        IcsLog::LogLevel("SendSubscribedData() SubsSetCamArea does not inform about anything.", kLogLevelInfo);
        return EXIT_SUCCESS;
    }

    if (typeinfo == typeid(SubsStartTravelTimeCalculation)) { // Subscription of starting the calculation
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsStartTravelTimeCalculation does not inform about anything.", kLogLevelInfo);
        return EXIT_SUCCESS;
    }

    if (typeinfo == typeid(SubsStopTravelTimeCalculation)) { // Subscription of stopping the calculation
        IcsLog::LogLevel("SendSubscribedData() Subscription SubsStopTravelTimeCalculation does not inform about anything.", kLogLevelInfo);
        return EXIT_SUCCESS;
    }

    // Subscription to calculate travel time
    if (typeinfo == typeid(SubsCalculateTravelTime)) {
        SubsCalculateTravelTime* subCalculateTT = static_cast<SubsCalculateTravelTime*>(subscription);
        if (subCalculateTT->InformApp(m_appMessageManager) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    // Subscription to return information about the received CAMs
    if (typeinfo == typeid(SubsGetReceivedCamInfo)) {

        SubsGetReceivedCamInfo* subsGetReceivedCamInfo = static_cast<SubsGetReceivedCamInfo*>(subscription);
        vector<TCamInformation>* camInfo = NULL;
        camInfo = subsGetReceivedCamInfo->getInformationFromLastReceivedCAMs();
        if (camInfo == NULL) {
            cout << "iCS --> [ERROR] SendSubscribedData() received CAMs are NULL" << endl;
            IcsLog::LogLevel("SendSubscribedData() received CAMs are NULL.", kLogLevelError);
            return EXIT_FAILURE;
        }
        if (camInfo->size() > 0) {

            stringstream log;
            log << "SendSubscribedData() Node " << nodeId << " received " << camInfo->size() << " CAM messages in the last time step.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            if (!m_appMessageManager->CommandSendSubscriptionReceivedCamInfo(camInfo, nodeId)) {
                cout << "iCS --> [ERROR] SendSubscribedData() Error sending information about received CAMs." << endl;
                IcsLog::LogLevel("SendSubscribedData() Error sending information about received CAMs.", kLogLevelError);
                delete camInfo;
                return EXIT_FAILURE;
            }
            delete camInfo;
            return EXIT_SUCCESS;
        } else {
            if (camInfo->size() == 0) {

                stringstream log;
                log << "[INFO] SendSubscribedData() Node " << nodeId << " did not receive CAM messages in the last time step.";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

                delete camInfo;
                return EXIT_SUCCESS;
            } else {
                cout << "iCS --> [ERROR] SendSubscribedData() Number of received CAMs is negative." << endl;

                stringstream log;
                log << "[ERROR] SendSubscribedData() Number of received CAMs is negative.";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);

                delete camInfo;
                return EXIT_FAILURE;
            }
        }
    }

    // Subscription to return information about facilities information about a node
    if (typeinfo == typeid(SubsGetFacilitiesInfo)) {

        SubsGetFacilitiesInfo* subsGetFacilitiesInfo = static_cast<SubsGetFacilitiesInfo*>(subscription);
        vector<unsigned char> facInfo = subsGetFacilitiesInfo->getFacilitiesInformation();

        stringstream log;
        log << "SendSubscribedData() Node " << nodeId << " will be updated about " << subsGetFacilitiesInfo->getNumberOfSubscribedFields() << " location related fields.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        if (!m_appMessageManager->CommandSendSubscriptionFacilitiesInfo(facInfo, nodeId)) {
            cout << "iCS --> [ERROR] SendSubscribedData() Error sending facilities information." << endl;
            IcsLog::LogLevel("SendSubscribedData() Error sending facilities information.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    // Subscription to send an Application-level message
    if (typeinfo == typeid(SubsAppMessageSend)) {

        SubsAppMessageSend* subsAppMessageSend = static_cast<SubsAppMessageSend*>(subscription);
        bool schedulingStatus = subsAppMessageSend->returnStatus();

#ifdef LOG_ON
        stringstream log;
        log << "SubsAppMessageSend() Node " << nodeId << " will be updated about the scheduling of the message.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (!m_appMessageManager->CommandSendSubscriptionAppMessageSend(schedulingStatus, nodeId)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsAppMessageReceive)) {

    	SubsAppMessageReceive* subsAppMessageReceive = static_cast<SubsAppMessageReceive*>(subscription);

#ifdef LOG_ON
        stringstream log;
        log << "SubsAppMessageReceive() Node " << nodeId << " will Pull the Communication Simulator for received data.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (subsAppMessageReceive->InformApp(m_appMessageManager) == EXIT_FAILURE) {
        	IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
        	return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsAppCmdTraffSim)) {

        SubsAppCmdTraffSim* subsAppCmdTraffSim = static_cast<SubsAppCmdTraffSim*>(subscription);
        bool resultStatus = subsAppCmdTraffSim->returnStatus();

#ifdef LOG_ON
        stringstream log;
        log << "SubsAppCmdTraffSim() Node " << nodeId << " will be updated about the status of the command to Traffic Simulator.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (!m_appMessageManager->CommandSendSubscriptionAppCmdTraffSim(resultStatus, nodeId)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsAppResultTraffSim)) {

        SubsAppResultTraffSim* subsAppResultTraffSim = static_cast<SubsAppResultTraffSim*>(subscription);
        vector<unsigned char> tsInfo = subsAppResultTraffSim->pull(m_syncManager);

#ifdef LOG_ON
        stringstream log;
        log << "SubsAppResultTraffSim() Node " << nodeId << " will pull the Traffic Simulator for data.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (!m_appMessageManager->CommandSendSubscriptionAppResultTraffSim(tsInfo, nodeId)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(SubsXApplicationData)) {


        SubsXApplicationData* subsXApplicationData = static_cast<SubsXApplicationData*>(subscription);
       // vector<unsigned char> xAppData = subsXApplicationData->pull(m_syncManager);
        vector<unsigned char> xAppData = subsXApplicationData->returnStatus();

#ifdef LOG_ON
        stringstream log;
        log << "SubsXApplicationData() Node " << nodeId << " will pull cross-application data.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        if (!m_appMessageManager->CommandSendSubscriptionXApplicationData(xAppData, nodeId)) {
            IcsLog::LogLevel("SendSubscribedData() Error sending scheduling status report.", kLogLevelError);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }


    return EXIT_FAILURE;
}

bool
ApplicationHandler::ExecuteApplication(int nodeId, ResultContainer* resultContainer)
{
    if (resultContainer == NULL)
        return false;

    if (!m_appMessageManager->CommandApplicationToExecute(nodeId, resultContainer))
        return false;

    return true;
}

int
ApplicationHandler::SendMessageStatus(int nodeId, ResultContainer* result)
{
    if (result == NULL) {
        cerr << "iCS --> [ERROR] SendMessageStatus() The result is NULL for node " << nodeId << endl;
        return EXIT_FAILURE;
    }

    const std::type_info& typeinfo = typeid(*result);
    if (typeinfo == typeid(ResultSetMaximumSpeed)) {
        ResultSetMaximumSpeed* resultMaxSpeed = static_cast<ResultSetMaximumSpeed*>(result);
        vector< pair<int,stationID_t> > messages;
        messages = resultMaxSpeed->GetReceivedMessages();
        if (m_appMessageManager->NotifyMessageStatus(nodeId, messages) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (typeinfo == typeid(ResultTravelTime)) {
        ResultTravelTime* resultTravelTime= static_cast<ResultTravelTime*>(result);
        vector< pair<int,stationID_t> > messages;
        messages = resultTravelTime->GetReceivedMessages();
        if (m_appMessageManager->NotifyMessageStatus(nodeId, messages) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (typeinfo == typeid(ResultTrafficJamDetection)) {
        ResultTrafficJamDetection* resultTrafficJam = static_cast<ResultTrafficJamDetection*>(result);
        vector< pair<int,stationID_t> > messages;
        messages = resultTrafficJam->GetReceivedMessages();
        if (m_appMessageManager->NotifyMessageStatus(nodeId, messages) == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (typeinfo == typeid(ResultTravelTime)) {

        stringstream log;
        log << "SendMessageStatus() The application does not send message status.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        return EXIT_SUCCESS;
    }

    if (typeinfo == typeid(ResultVoid)) {

        stringstream log;
        log << "SendMessageStatus() The application does not send message status.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        return EXIT_SUCCESS;
    }
    if (typeinfo == typeid(ResultGeneric)) {

        stringstream log;
        log << "SendMessageStatus() The application does not send message status.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        return EXIT_SUCCESS;
    }

    cerr << "iCS --> [ERROR] SendMessageStatus() node " << nodeId << " The result type is not implemented in iTETRIS " << typeinfo.name() << endl;
    return EXIT_FAILURE;
}


}
