/****************************************************************************/
/// @file    sync-manager.cpp
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <typeinfo>
#include <sstream>
#include <algorithm>
#include <map>

#include "sync-manager.h"
#include "vehicle-node.h"
#include "fixed-node.h"
#include "tmc-node.h"
#include "wirelesscom_sim_communicator/ns3-client.h"
#include "traffic_sim_communicator/traci-client.h"
#include "wirelesscom_sim_message_tracker/V2X-message-manager.h"
#include "wirelesscom_sim_message_tracker/V2X-cam-area.h"
#include "wirelesscom_sim_message_tracker/V2X-geobroadcast-area.h"
#include "applications_manager/subscription.h"
#include "applications_manager/app-commands-subscriptions-constants.h"
#include "applications_manager/app-result-maximum-speed.h"
#include "applications_manager/app-result-travel-time.h"
#include "applications_manager/app-result-open-buslanes.h"
#include "applications_manager/application-handler.h"
#include "applications_manager/app-message-manager.h"
#include "applications_manager/subs-set-cam-area.h"
#include "applications_manager/subs-start-travel-time-calculation.h"
#include "applications_manager/subs-stop-travel-time-calculation.h"
#include "applications_manager/subs-app-message-send.h"
#include "applications_manager/subs-app-message-receive.h"
#include "applications_manager/app-result-traffic-jam-detection.h"
#include "applications_manager/app-result-void.h"
#include "applications_manager/app-result-generic.h"
#include "FacilitiesManager.h"
#include "ics.h"
#include "../utils/ics/log/ics-log.h"
#include "../utils/ics/iCSGeoUtils.cpp"
#include <cmath>

#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

#ifndef _WIN32
#include "config.h"
#endif

using namespace std;

namespace ics
{

/**
 * @class veh_by_id_finder
 * @brief A class to find the matching lane wrapper
 */
class veh_by_id_finder
{
public:
    /** @brief constructor */
    explicit veh_by_id_finder(const std::string id) :
            myID(id) {
    }

    /** @brief the comparing function */
    bool operator()(ITetrisNode *node) {
        VehicleNode *veh = dynamic_cast<VehicleNode *>(node);
        return veh != 0 && veh->m_tsId == myID;
    }

private:
    /// @brief The id of the searched object
    std::string myID;
};

// ===========================================================================
// class declarations
// ===========================================================================
class ResultContainer;

// ===========================================================================
// static member definitions
// ===========================================================================
icstime_t SyncManager::m_simStep = -1;
FacilitiesManager* SyncManager::m_facilitiesManager;
TrafficSimulatorCommunicator* SyncManager::m_trafficSimCommunicator;
WirelessComSimulatorCommunicator* SyncManager::m_wirelessComSimCommunicator;
V2xMessageManager* SyncManager::m_v2xMessageTracker;

// ===========================================================================
// member method definitions
// ===========================================================================
SyncManager::SyncManager(int ns3Port, int sumoPort, string sumoHost, string ns3Host, int beginTime, int endTime)
        : m_firstTimeStep(beginTime), m_lastTimeStep(endTime)
{
    m_ns3Client.m_port = ns3Port;
    m_ns3Client.m_host = ns3Host;
    m_wirelessComSimCommunicator = &m_ns3Client;

    m_traci.m_port = sumoPort;
    m_traci.m_host = sumoHost;
    m_trafficSimCommunicator = &m_traci;

    m_lastTimeStep = endTime;
    if (m_firstTimeStep >= m_lastTimeStep) {
        cout << "[WARNING] The defined first time step is equal or higher than the ending time step." << endl;
        cout << "[WARNING] The first time step is automatically set to 0." << endl;
        m_firstTimeStep = 0;
    }

    m_iTetrisNodeCollection = new vector<ITetrisNode*> ();
    m_iTetrisNodeCollection->push_back(TmcNode::GetInstance());

    m_applicationHandlerCollection = new vector<ApplicationHandler*> ();
    m_v2xMessageTracker = new V2xMessageManager();
    m_subscriptionCollection = new vector<Subscription*> ();
    m_facilitiesManager = new FacilitiesManager();
}

SyncManager::~SyncManager()
{
    delete m_trafficSimCommunicator;
    delete m_wirelessComSimCommunicator;
    delete m_iTetrisNodeCollection;
    delete m_applicationHandlerCollection;
    delete m_v2xMessageTracker;
    delete m_subscriptionCollection;
    delete m_facilitiesManager;
}

int SyncManager::Run(bool interactive)
{
    m_simStep = 0;

    cout << "\t\tiCS --> Global simulation timestep is: " << m_simStep << "; last step is: " << m_lastTimeStep << endl;
    cout << "\t\t============================================================" << endl;

    while (m_lastTimeStep >= m_simStep) {

#ifdef LOG_ON
        stringstream log;
        log << "Tics = " << m_simStep;
        IcsLog::Log((log.str()).c_str());
        IcsLog::Log("*************************************************************************************");
#endif

        if (interactive) {
            cout << endl;
            utils::Conversion::Wait("Press <Enter> to run the next ns-3 timestep");
            cout << endl;
            cout << "STEP 5 - SIMULATION PHASE - Tics = " << m_simStep << endl;
            cout << "======================================" << endl;
            cout << "[ns-3] EXECUTING EVENTS Tns-3 = Tics + 1" << endl;
            cout << "[APP]  WAITING." << endl;
            cout << "[iCS]  PROCESS THE STATUS OF THE MESSAGES RECEIVED IN ns-3." << endl;
            cout << "[SUMO] WAITING." << endl;
            utils::Conversion::Wait("Press <Enter> to continue...");
            cout << endl;
        } else {
            cout << "STEP 5 - SIMULATION PHASE - Tics = " << m_simStep << endl;
        }

#ifdef NS3_ON
        if (m_simStep >= m_firstTimeStep) {
            if (RunOneNs3TimeStep() == EXIT_FAILURE) {
                utils::Conversion::Wait("iCS --> [ERROR] RunOneNs3TimeStep()");
                return EXIT_FAILURE;
            }

            if (GetDataFromNs3() == EXIT_FAILURE) {
                utils::Conversion::Wait("iCS --> [ERROR] GetDataFromNs3()");
                return EXIT_FAILURE;
            }
        }
#endif

        if (interactive) {
            cout << endl << endl;
            utils::Conversion::Wait("Press <Enter> to run the next SUMO timestep");
            cout << endl;
            cout << "STEP 6 - SIMULATION PHASE - Tics = " << m_simStep << endl;
            cout << "======================================" << endl;
            cout << "[ns-3] CREATE NEW NODES." << endl;
            cout << "[APP]  WAITING." << endl;
            cout << "[iCS]  REGISTER NEW NODES, ASSIGN RAT AND APPLICATIONS. UPDATE POSITIONS."<< endl;
            cout << "[SUMO] EXECUTING EVENTS = Tics" << endl;
            utils::Conversion::Wait("Press <Enter> to continue...");
            cout << endl;
        } else {
            cout << "STEP 6 - SIMULATION PHASE - Tics = " << m_simStep << endl;
        }

#ifdef SUMO_ON
        if (RunOneSumoTimeStep() == EXIT_FAILURE) {
            utils::Conversion::Wait("iCS --> [ERROR] RunOneSumoTimeStep()");
            return EXIT_FAILURE;
        }
#endif

        if (interactive) {
            utils::Conversion::Wait("Press <Enter> to run the next Application round");
            cout << endl;
            cout << "STEP 7 - SIMULATION PHASE - Tics = " << m_simStep << endl;
            cout << "======================================" << endl;
            cout << "[ns-3] WAITING" << endl;
            cout << "[APP]  WAITING." << endl;
            cout << "[iCS]  RUN INTERACTION WITH APPLICATIONS." << endl;
            cout << "[SUMO] WAITING" << endl;
            utils::Conversion::Wait("Press <Enter> to continue...");
            cout << endl;
        } else {
            cout << "STEP 7 - SIMULATION PHASE - Tics = " << m_simStep << endl;
        }

#ifdef APPLICATIONS_ON
        if (m_simStep >= m_firstTimeStep) {
            if (RunApplicationLogic() == EXIT_FAILURE) {
                utils::Conversion::Wait("iCS --> [ERROR] RunApplicationLogic()");
                return EXIT_FAILURE;
            }
        }
#endif

        if (interactive) {
            utils::Conversion::Wait("Press <Enter> to process the result of Applications");
            cout << endl;
            cout << "STEP 8 - SIMULATION PHASE - Tics = " << m_simStep << endl;
            cout << "======================================" << endl;
            cout << "[ns-3] WAITING" << endl;
            cout << "[APP]  WAITING." << endl;
            cout << "[iCS]  PROCESS RESULTS OF THE APPLICATIONS." << endl;
            cout << "[SUMO] WAITING" << endl;
            utils::Conversion::Wait("Press <Enter> to continue...");
            cout << endl;
        } else {
            cout << "STEP 8 - SIMULATION PHASE - Tics = " << m_simStep << endl;
        }
	  if (m_iTetrisNodeCollection !=NULL)
	  cout<<"iCS hhhhhhhhhhhhhhhhhhhhh"<<m_iTetrisNodeCollection->size()<<endl;
	  else
	    cout<<"NULLLLLLLLLLLLL"<<endl;
#ifdef APPLICATIONS_ON
        if (m_simStep >= m_firstTimeStep) {

            if (ProcessApplicationResults() == EXIT_FAILURE) {
//                 utils::Conversion::Wait(
                    cout<< "iCS --> [ERROR] ProcessApplicationResults()"<<endl/*)*/;
                return EXIT_FAILURE;
            }
        }
#endif

        if (interactive) {
            cout << endl;
            utils::Conversion::Wait("Press <Enter> to update positions in ns-3");
            cout << endl;
            cout << "STEP 9 - SIMULATION PHASE - Tics = " << m_simStep << endl;
            cout << "========================================" << endl;
            cout << "[ns-3] UPDATE POSITIONS." << endl;
            cout << "[APP]  WAITING." << endl;
            cout << "[iCS]  WAITING." << endl;
            cout << "[SUMO] WAITING." << endl;
            utils::Conversion::Wait("Press <Enter> to continue...");
            cout << endl;
        } else {
            cout << "STEP 9 - SIMULATION PHASE - Tics = " << m_simStep << endl;
        }

#ifdef NS3_ON
        if (m_simStep >= m_firstTimeStep) {
            if (ScheduleV2xMessages() == EXIT_FAILURE) {
                utils::Conversion::Wait("iCS --> [ERROR] ScheduleV2xMessages()");
                return EXIT_FAILURE;
            }
        }

        if (UpdatePositionsInNs3() == EXIT_FAILURE) {
            utils::Conversion::Wait("iCS --> [ERROR] UpdatePositionsInNs3()");
            return EXIT_FAILURE;
        }

        //TestInciPrimitives ();
#endif

        //Increase global time simulation counter
        m_simStep++;

        //Update facilities timestep
        m_facilitiesManager->updateClock(m_simStep);

        // To not display the last time step header
        if (m_lastTimeStep >= m_simStep) {
            cout << "\t\tiCS --> Global simulation timestep is: " << m_simStep
                 << "; last step is: " << m_lastTimeStep << endl;
            cout
                << "\t\t============================================================"
                << endl;
        }
    }

#ifdef LOG_ON
    stringstream log;
    log << "[Run] The runtime is over. Last time step reached [LastTimeStep] ["
    << m_lastTimeStep << "]";
    IcsLog::Log((log.str()).c_str());
#endif

    return EXIT_SUCCESS;
}

int SyncManager::Stop()
{
    bool success = true;

#ifdef NS3_ON
    if (CloseNs3() == EXIT_FAILURE) {
        success = false;
        Sleep(2000);
    }
#endif

#ifdef SUMO_ON
    if (CloseSumo() == EXIT_FAILURE) {
        success = false;
        Sleep(2000);
    }
#endif

#ifdef APPLICATIONS_ON

    if (CloseApps() == EXIT_FAILURE) {
        success = false;
        Sleep(2000);
    }
#endif

    if (success)
        return EXIT_SUCCESS;

    return EXIT_FAILURE;
}


int
SyncManager::RunOneSumoTimeStep()
{
    std::vector<std::string> departed;
    std::vector<std::string> arrived;
    if (m_trafficSimCommunicator->CommandSimulationStep((double) m_simStep, departed, arrived) == EXIT_FAILURE) {
        IcsLog::LogLevel("RunOneSumoTimeStep() Error trying to command simulation step in traffic simulator.", kLogLevelError);
        return EXIT_FAILURE;
    }

    // remove vehicles that left the simulation
    {
        stringstream log;
        log << "RunOneSumoTimeStep() Number of vehicles that left the simulation: " << arrived.size();
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
    }

    for (vector<string>::const_iterator i = arrived.begin(); i != arrived.end(); ++i) {
        vector<ITetrisNode*>::iterator q1 = find_if(m_iTetrisNodeCollection->begin(),m_iTetrisNodeCollection->end(), veh_by_id_finder(*i));
        if (q1 != m_iTetrisNodeCollection->end()) {

            stringstream log;
            log << "RunOneSumoTimeStep() Left the simulation: " << (*q1)->m_icsId;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            string ns3Id = utils::Conversion::int2String((*q1)->m_nsId);
            m_vehiclesToBeDeactivated.push_back(ns3Id);
            RemoveNodeInTheArea((*q1));
            delete *q1;
            m_iTetrisNodeCollection->erase(q1);
        }
    }

    // assign RATs to new vehicles and create the node in ns-3
    {
        stringstream log;
        log << "RunOneSumoTimeStep() Number of vehicles that entered the simulation: " << departed.size();
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
    }

    // Container of nodes to be activated in wireless communication simulator
    vector<string> nodesToActivateInNs3;
    for (vector<string>::const_iterator i = departed.begin(); i != departed.end(); ++i) {
        if (ITetrisSimulationConfig::HasRat()) {
            VehicleNode* vehicle;

            ics_types::stationID_t iCSId = m_facilitiesManager->getPredefICSidFromTSId(*i);

            //use a specific iCS ID if an association exist with traffic simulator vehicle ID
            if(iCSId == 0)
				vehicle = new VehicleNode(*i);
            else
				vehicle = new VehicleNode(*i,iCSId);

            {
                stringstream log;
                log << "RunOneSumoTimeStep() vehicle " << vehicle->m_icsId << "  entered the simulation.";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
            }

            // Get additional info from SUMO
            std::pair<float, float> pos = m_trafficSimCommunicator->GetPosition(*vehicle);
            float speed                 = m_trafficSimCommunicator->GetSpeed(*vehicle);
            float acceleration          = vehicle->ChangeSpeed(speed);
            float direction             = m_trafficSimCommunicator->GetDirection(*vehicle);
            float length                = m_trafficSimCommunicator->GetVehicleLength(*vehicle);
            bool lightsOn               = m_trafficSimCommunicator->GetExteriorLights(*vehicle);
            float width                 = m_trafficSimCommunicator->GetVehicleWidth(*vehicle);
            string lane                 = m_trafficSimCommunicator->GetLane(*vehicle);

            // Create the new station in the facilities
            TMobileStationDynamicInfo info;
            info.speed = speed;
            info.acceleration = acceleration;
            info.direction = direction;
            info.exteriorLights = lightsOn;
            info.positionX = pos.first;
            info.positionY = pos.second;
            info.length = length;
            info.width = width;
            info.lane = lane;
            info.timeStep = m_simStep;
            m_facilitiesManager->updateMobileStationDynamicInformation(vehicle->m_icsId, info);

            AssignApplication(vehicle);
            vector<RATID>* rats = m_facilitiesManager->getStationActiveRATs(vehicle->m_icsId);
            vector<string> techList;
            if (rats->size() == 0) {
                cout << "[ERROR] getStationActiveRATs returned 0 size" << endl;
                return EXIT_FAILURE;
            } else {
                for (vector<RATID>::iterator it = rats->begin(); it!= rats->end(); it++) {
                    switch (*it) {
                    case WAVE: {
                        techList.push_back("WaveVehicle");
                        vehicle->m_rats.insert("WaveVehicle");
                        break;
                    }
                    case UMTS: {
                        techList.push_back("UmtsVehicle");
                        vehicle->m_rats.insert("UmtsVehicle");
                        break;
                    }
                    case WiMAX: {
                        techList.push_back("WimaxVehicle");
                        vehicle->m_rats.insert("WimaxVehicle");
                      //  utils::Conversion::Wait("iCS --> Wimax not implemented yet.");
                        return EXIT_FAILURE;
                        break;
                    }
                    case DVBH: {
                        techList.push_back("DvbhVehicle");
                        vehicle->m_rats.insert("DvbhVehicle");
                        break;
                    }
                    default: {
                        cout << "[ERROR] RunOneSumoTimeStep() There is no match for the type of RAT" << endl;
                        delete rats;
                        return EXIT_FAILURE;
                    }
                    }
                }
            }
            delete rats;

            // Create the node in ns-3
#ifdef NS3_ON
            int32_t id = m_wirelessComSimCommunicator->CommandCreateNode2(vehicle->GetPositionX(), vehicle->GetPositionY(), vehicle->GetSpeed(), vehicle->GetHeading(), vehicle->GetLane(), techList);
#else
            int32_t id = 1;
#endif
            vehicle->m_nsId = id; // Assign the ns-3 node ID returned by ns-3
            m_iTetrisNodeCollection->push_back(vehicle);
            nodesToActivateInNs3.push_back(utils::Conversion::int2String(vehicle->m_nsId));  // Add vehicle to activate
        }
    }

    // Activate the new nodes in wireless
#ifdef NS3_ON
    m_wirelessComSimCommunicator->CommandActivateNode(nodesToActivateInNs3);
#endif
    // update positions + other data
    for (vector<ITetrisNode*>::const_iterator i = m_iTetrisNodeCollection->begin(); i != m_iTetrisNodeCollection->end(); ++i) {
        VehicleNode* vehicle = dynamic_cast<VehicleNode*>(*i);
        if (vehicle == 0) {
            continue;
        }
        // Get additional info from SUMO
        std::pair<float, float> pos = m_trafficSimCommunicator->GetPosition(*vehicle);
        vehicle->CheckPosition(pos);
        float speed = m_trafficSimCommunicator->GetSpeed(*vehicle);
        float acceleration = vehicle->ChangeSpeed(speed);
        float direction = m_trafficSimCommunicator->GetDirection(*vehicle);
        float length = m_trafficSimCommunicator->GetVehicleLength(*vehicle);
        bool lightsOn = m_trafficSimCommunicator->GetExteriorLights(*vehicle);
        float width = m_trafficSimCommunicator->GetVehicleWidth(*vehicle);
        string lane = m_trafficSimCommunicator->GetLane(*vehicle);

        // Update the station in the facilities
        TMobileStationDynamicInfo info;
        info.speed = speed;
        info.acceleration = acceleration;
        info.direction = direction;
        info.exteriorLights = lightsOn;
        info.positionX = pos.first;
        info.positionY = pos.second;
        info.length = length;
        info.width = width;
        info.lane = lane;
        info.timeStep = m_simStep;
        m_facilitiesManager->updateMobileStationDynamicInformation(vehicle->m_icsId, info);
    }

    // Update traffic lights
    std::vector<ics_types::trafficLightID_t> trafficLigthIds;
    if (m_trafficSimCommunicator->GetTrafficLights(trafficLigthIds) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    bool hadProblems = false;
    for (std::vector<ics_types::trafficLightID_t>::iterator i=trafficLigthIds.begin(); i!=trafficLigthIds.end(); ++i) {
        std::string state;
        if (m_trafficSimCommunicator->GetTrafficLightStatus(*i, state) == EXIT_FAILURE) {
            hadProblems = true;

        } else {
            for (unsigned int j=0; j<state.length(); ++j) {
            	TrafficLightDynamicInfo tti;
                tti.active = true;
                switch (state[j]) {
                case 'r':
                    tti.status = RED;
                    break;
                case 'y':
                case 'Y':
                    tti.status = YELLOW;
                    break;
                case 'g':
                case 'G':
                    tti.status = GREEN;
                    break;
                default:
                    tti.status = UNKNOWN;
                    break;
                }
                m_facilitiesManager->updateTrafficLightDynamicInformation(*i, j, tti);
              //  cout<< " In RUN Simulation STEP 6" << endl;
            }
        }
    }
    if (hadProblems) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}

int SyncManager::RunOneNs3TimeStep()
{
    //ns-3 time step is one step ahead
    int ns3timeStep = m_simStep + 1;

    if (m_wirelessComSimCommunicator->CommandSimulationStep(ns3timeStep)) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

int SyncManager::RunApplicationLogic()
{
    bool success = true;

    vector<ITetrisNode*>::iterator nodeIt;

    // Loop all the nodes
    for (nodeIt = m_iTetrisNodeCollection->begin(); nodeIt < m_iTetrisNodeCollection->end(); nodeIt++) {
        ITetrisNode* currentNode = (*nodeIt);	

        if (currentNode->m_applicationHandlerInstalled->size() != 0) {

            if (NewSubscriptions(currentNode) == EXIT_FAILURE) {
                cout << "iCS --> [ERROR] RunApplicationLogic() in NewSubscriptions." << endl;
                return EXIT_FAILURE;
            }

            if (DropSubscriptions(currentNode) == EXIT_FAILURE) {
                cout << "iCS --> [ERROR] RunApplicationLogic() in DropSubscriptions." << endl;
                return EXIT_FAILURE;
            }

            if (ForwardSubscribedDataToApplication(currentNode) == EXIT_FAILURE) {
            	cout << "iCS --> [ERROR] RunApplicationLogic() in ForwardSubscribedDataToApplication." << endl;
                return EXIT_FAILURE;
            }
            if (DeliverMessageStatus(currentNode) == EXIT_FAILURE) {
                cout << "iCS --> [ERROR] RunApplicationLogic() in DeliverMessageStatus." << endl;
                return EXIT_FAILURE;
            }

            if (ExecuteApplicationMainFunction(currentNode) == EXIT_FAILURE) {
                cout << "iCS --> [ERROR] RunApplicationLogic() in ExecuteApplicationMainFunction." << endl;
                return EXIT_FAILURE;
            }
        }
    }

    if (success) {
        cout << endl;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

int SyncManager::ConnectNs3()
{
    if (m_wirelessComSimCommunicator->Connect())
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}

int SyncManager::CloseNs3()
{
    if (m_wirelessComSimCommunicator->CommandClose() == EXIT_FAILURE) {
        cout << "iCS --> [ERROR] CloseNs3() Finishing NS-3" << endl;
        return EXIT_FAILURE;
    }

    if (m_wirelessComSimCommunicator->Close() == EXIT_FAILURE) {
        cout << "iCS --> [ERROR] CloseNs3() Closing socket." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int SyncManager::ConnectSumo()
{
    if (m_trafficSimCommunicator->Connect())
        return EXIT_SUCCESS;

    return EXIT_FAILURE;
}

int SyncManager::CloseSumo()
{
    if (m_trafficSimCommunicator->CommandClose() == EXIT_FAILURE) {
        cout << "iCS --> [ERROR] CloseSumo() Closing SUMO" << endl;
        return EXIT_FAILURE;
    }

    if (m_trafficSimCommunicator->Close() == EXIT_FAILURE) {
        cout << "iCS --> [ERROR] CloseSumo() Closing socket." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int
SyncManager::ConnectToApps()
{
    bool success = true;

    // Loop Applications
    for (vector<ApplicationHandler*>::iterator appHandlerIt = m_applicationHandlerCollection->begin(); appHandlerIt < m_applicationHandlerCollection->end(); appHandlerIt++) {
        ApplicationHandler* appHandler = (*appHandlerIt);

        if (appHandler->m_appMessageManager->Connect(appHandler->m_host, appHandler->m_port)) {
            cout << "iCS --> Application " << appHandler->m_name  << " connected." << endl;
        } else {
            success = false;
        }
    }

    if (success)
        return EXIT_SUCCESS;

    return EXIT_FAILURE;
}

int SyncManager::CloseApps()
{
    bool success = true;

    // Loop all apps
    for (vector<ApplicationHandler*>::iterator appHandlerIt = m_applicationHandlerCollection->begin(); appHandlerIt < m_applicationHandlerCollection->end(); appHandlerIt++) {
        ApplicationHandler* appHandler = (*appHandlerIt);

        if (appHandler->m_appMessageManager->CommandClose() == EXIT_FAILURE) {
            cout << "iCS --> Application " << appHandler->m_name << " was not closed" << endl;
            success = false;
        }
    }

    if (success)
        return EXIT_SUCCESS;

    return EXIT_FAILURE;
}

int
SyncManager::SetFixedStationInNs3()
{
    if (m_iTetrisNodeCollection->begin() == m_iTetrisNodeCollection->end()) {

        stringstream log;
        log << "SetFixedStationInNs3() There are not fixed stations";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        return EXIT_SUCCESS;
    }

    for (vector<ITetrisNode*>::iterator it = m_iTetrisNodeCollection->begin(); it != m_iTetrisNodeCollection->end(); it++) {
        ITetrisNode *node = (*it);
        ITetrisNode& reference = *node;
        const type_info& typeinfo = typeid(reference);
        if (typeinfo == typeid(FixedNode)) { // Only send message to nodes that are CIUs (fixed stations)
            FixedNode* fixedNode = static_cast<FixedNode*>(node);
            vector<string> techList;
            string tech = m_facilitiesManager->getFixedStationCommunicationProfile(fixedNode->m_icsId);
            techList.push_back(tech);
            int id = m_wirelessComSimCommunicator->CommandCreateNode(fixedNode->GetPositionX(), fixedNode->GetPositionY(), techList);
            if (id == -1) { // Check status
#ifdef LOG_ON
                stringstream log;
                log << "SetFixedStationInNs3() Error in the message to create fixed station in ns-3";
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
                cout << "iCS --> [ERROR] SetFixedStationInNs3() Error in the message to create fixed station in ns-3" << endl;
                return EXIT_FAILURE;
            }

            fixedNode->m_nsId = id; // Assign the ID returned by ns-3
        }
    }

    return EXIT_SUCCESS;
}

int SyncManager::UpdatePositionsInNs3()
{
    // Deactivate nodes that left the scenario
    if (m_vehiclesToBeDeactivated.size() > 0) {
        if (m_wirelessComSimCommunicator->CommandDeactivateNode(m_vehiclesToBeDeactivated)) {
#ifdef LOG_ON
            stringstream log;
            log << "UpdatePositionsInNs3() Deactivated nodes: ";
            for (vector<string>::iterator it = m_vehiclesToBeDeactivated.begin(); it < m_vehiclesToBeDeactivated.end(); it++) {
                log << (*it) << " ";
            }
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            m_vehiclesToBeDeactivated.clear(); // Reset the container
        } else {

            stringstream log;
            log << "UpdatePositionsInNs3() Node deactivation incorrect";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);

        }
    }

    // Update the position of the vehicles.
    for (vector<ITetrisNode*>::iterator it = m_iTetrisNodeCollection->begin(); it
            < m_iTetrisNodeCollection->end(); it++) {
        // Discard the node that are not mobile
        ITetrisNode& reference = *(*it);
        const type_info& typeinfo = typeid(reference);
        if (typeinfo != typeid(FixedNode) && typeinfo != typeid(TmcNode)) {
            VehicleNode* vehicle = (VehicleNode*)(*it);
            if (vehicle->m_moved) {
                if (m_wirelessComSimCommunicator->CommandUpdateNodePosition2(vehicle->m_nsId, vehicle->GetPositionX(), vehicle->GetPositionY(), vehicle->GetSpeed() , vehicle->GetHeading(), vehicle->GetLane()) == EXIT_FAILURE) {
                    return EXIT_FAILURE;
                }
                // Reset the moved info value
                vehicle->m_moved = false;
            }
        }
    }

    return EXIT_SUCCESS;

}

ITetrisNode*
SyncManager::GetNodeByNs3Id(int nodeId)
{
    vector<ITetrisNode*>::iterator it;

    for (it = m_iTetrisNodeCollection->begin(); it
            < m_iTetrisNodeCollection->end(); it++) {
        if ((*it)->m_nsId == nodeId) {
            return *it;
        }
    }
}

ITetrisNode*
SyncManager::GetNodeBySumoId(const std::string &nodeId)
{
    vector<ITetrisNode*>::iterator it;
    for (it = m_iTetrisNodeCollection->begin(); it
            < m_iTetrisNodeCollection->end(); it++) {
        if ((*it)->m_tsId == nodeId) {
            return *it;
        }
    }

    return NULL;
}

ITetrisNode*
SyncManager::GetNodeByIcsId(stationID_t nodeId)
{
    vector<ITetrisNode*>::iterator it;

    for (it = m_iTetrisNodeCollection->begin(); it
            < m_iTetrisNodeCollection->end(); it++) {
        if ((*it)->m_icsId == nodeId) {
            return *it;
        }
    }

    return NULL;
}

int
SyncManager::ForwardSubscribedDataToApplication(ITetrisNode *node)
{
    if (node == NULL) {
        cout << "iCS --> [ERROR] ForwardSubscribedDataToApplication() Node is null." << endl;
        return EXIT_FAILURE;
    }

    if (node->m_subscriptionCollection->size() == 0) {

        stringstream log;
        log << "iCS --> There is 0 subscription (node " << node->m_icsId << ")";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        return EXIT_SUCCESS;
    }

    // Loop the subscriptions of the node
    for (vector<Subscription*>::iterator subIt = node->m_subscriptionCollection->begin(); subIt < node->m_subscriptionCollection->end(); subIt++) {
        Subscription* subscription = (*subIt);
        vector<ApplicationHandler*>* apps = node->m_applicationHandlerInstalled;

        // Loop the applications the node has installled
        for (vector<ApplicationHandler*>::iterator appIt = apps->begin(); appIt < apps->end(); appIt++) {
            ApplicationHandler* app = (*appIt);
            if (app->m_id == subscription->m_appId) { // Check if the subscription was created by the application
                int nodeId = node->m_icsId;

                if (app->SendSubscribedData(nodeId, subscription, m_iTetrisNodeCollection) == EXIT_FAILURE) {
                    cout << "iCS --> [ERROR] ForwardSubscribedDataToApplication() Error sending subscribed data." << endl;
                    return EXIT_FAILURE;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

bool SyncManager::AssignApplication(ITetrisNode *node)
{
    if (node == NULL)
        return false;

    vector<ApplicationHandler*>::iterator it;

    for (it = m_applicationHandlerCollection->begin(); it < m_applicationHandlerCollection->end(); it++) {
        ApplicationHandler* appHand = (*it);

//        cout << "Application rate: " << appHand->m_rate << endl;

        // Decide if the application will be assigned based on the penetration rates
        if (ITetrisSimulationConfig::AssignApplication(appHand->m_rate, appHand->m_seed)) {
            node->m_applicationHandlerInstalled->push_back(appHand);

            switch (appHand->m_resultType) {
            case OUTPUT_SET_SPEED_ADVICE_DEMO:
            case OUTPUT_SET_VEHICLE_MAX_SPEED: {
                ResultSetMaximumSpeed* result = new ResultSetMaximumSpeed(node->m_icsId, appHand->m_id);
                node->m_resultContainerCollection->push_back(result);
                break;
            }
            case OUTPUT_TRAVEL_TIME_ESTIMATION: {
                ResultTravelTime* result = new ResultTravelTime(node->m_icsId, appHand->m_id);
                node->m_resultContainerCollection->push_back(result);
                break;
            }
            case OUTPUT_TRAFFIC_JAM_DETECTION: {
                ResultTrafficJamDetection* result = new ResultTrafficJamDetection(
                    node->m_icsId, appHand->m_id);
                node->m_resultContainerCollection->push_back(result);
                break;
            }
            case OUTPUT_OPEN_BUSLANES: {
                ResultOpenBuslanes* result = new ResultOpenBuslanes(
                    node->m_icsId, appHand->m_id);
                node->m_resultContainerCollection->push_back(result);
                break;
            }
            case OUTPUT_VOID: {
            	ResultVoid* result = new ResultVoid(node->m_icsId, appHand->m_id);
            	node->m_resultContainerCollection->push_back(result);
            	break;
            }
            case OUTPUT_GENERIC: {
            	ResultGeneric* result = new ResultGeneric(node->m_icsId, appHand->m_id);
            	node->m_resultContainerCollection->push_back(result);
            	break;
            }
            default:
                cout << "iCS --> Result type is not registered. Please contact Application scientist. " << endl;
                return false;
            }
        }
    }

    return true;
}

int
SyncManager::NewSubscriptions(ITetrisNode *node)
{
    if (node == NULL)
        return EXIT_FAILURE;

#ifdef LOG_ON
    stringstream log;
    log << " \n \niCS --> Asking new subscriptions to node " << node->m_icsId << " at TS "<< m_simStep<< " ";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
    
    vector<ApplicationHandler*>::iterator appsIt;
    vector<ApplicationHandler*>* apps = node->m_applicationHandlerInstalled;

    vector<Subscription*>* newSubs = new vector<Subscription*> ();

    // Loop the applications installed to ask for subscriptions
    for (appsIt = apps->begin(); appsIt < apps->end(); appsIt++) {
        ApplicationHandler* appHandler = (*appsIt);
        bool success = true;
        success = appHandler->AskForNewSubscriptions(node->m_icsId, newSubs);
        if (!success) {
            cout << "iCS --> Error occurred when asking for new subscriptions (node " << node->m_icsId << ")" << endl;
            return EXIT_FAILURE;
        }
    }

    if (newSubs->size() == 0) {

#ifdef LOG_ON
        stringstream log;
        log << "iCS --> NewSubscriptions() The node " << node->m_icsId << " requested 0 subscriptions";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
        return EXIT_SUCCESS;
    }

    // Loop the new subscription, linked them and process if needed (for example, creating CAM areas)
    vector<Subscription*>::iterator subIt;
    for (subIt = newSubs->begin(); subIt < newSubs->end(); subIt++) {

        Subscription* subscription = *subIt;
        node->m_subscriptionCollection->push_back(subscription);
        m_subscriptionCollection->push_back(subscription);
        
        #ifdef LOG_ON
                stringstream log;
                log << "iCS --> NewSubscriptions() The node " << node->m_icsId << " requested subscriptions " <<  subscription->m_name  << " for " << subscription->m_nodeId ;
                IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
        #endif        

        Subscription& sub = *subscription;
        const std::type_info& typeinfo = typeid(sub);

        if (typeinfo == typeid(SubsSetCamArea)) {
            SubsSetCamArea* subSetCamArea = static_cast<SubsSetCamArea*>(subscription);
            int payloadLength = 20;
            m_v2xMessageTracker->CreateV2xCamArea(subSetCamArea->m_id, subSetCamArea->GetFrequency(), payloadLength);
        }
    }

    return EXIT_SUCCESS;
}

int
SyncManager::DropSubscriptions(ITetrisNode *node)
{
    if (node == NULL)
        return EXIT_FAILURE;

    if (node->m_subscriptionCollection->size() == 0) {

        stringstream log;
        log << "DropSubscriptions() The node " << node->m_icsId << " has 0 subscriptions.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        return EXIT_SUCCESS;
    }

    vector<ApplicationHandler*>::iterator appsIt;
    vector<ApplicationHandler*>* apps = node->m_applicationHandlerInstalled;

    // Loop applications installed in the node
    for (appsIt = apps->begin(); appsIt < apps->end(); appsIt++) {
        ApplicationHandler* appHandler = *appsIt;
        bool success = true;
        success = appHandler->AskForUnsubscriptions(node->m_icsId, node->m_subscriptionCollection);
        if (!success) {
            cout << "iCS --> Error occurred when asking to drop subscriptions (node " << node->m_icsId << ")" << endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int
SyncManager::ExecuteApplicationMainFunction(ITetrisNode *node)
{
    if (node == NULL)
        return EXIT_FAILURE;

    vector<ApplicationHandler*>* apps = node->m_applicationHandlerInstalled;

    // Loop all the application to ask for execution
    for (vector<ApplicationHandler*>::iterator appsIt = apps->begin(); appsIt < apps->end(); appsIt++) {
        ApplicationHandler* appHandler = *appsIt;

        // Look for the appropriated result container of the application
        for (vector<ResultContainer*>::iterator resultIt = node->m_resultContainerCollection->begin(); resultIt < node->m_resultContainerCollection->end(); resultIt++) {
            ResultContainer* resultContainer = *resultIt;
            if ((*appsIt)->m_id == (*resultIt)->m_applicationHandlerId) {   //Find the matching result container of the application
                bool success = EXIT_SUCCESS;
#ifdef LOG_ON
          	    stringstream log;
                log << "iCS --> executing application (node " << node->m_icsId << ")";
                IcsLog::LogLevel((log.str()).c_str(),kLogLevelInfo);
#endif
                success = appHandler->ExecuteApplication(node->m_icsId, resultContainer);
                if (!success) {
                    cout << "iCS --> Error occurred when asking to execute application (node " << node->m_icsId << ")" << endl;
                    return EXIT_FAILURE;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

int
SyncManager::DeliverMessageStatus(ITetrisNode* node)
{
    // Loop applications installed in the node
    for (vector<ApplicationHandler*>::iterator appsIt = node->m_applicationHandlerInstalled->begin(); appsIt < node->m_applicationHandlerInstalled->end(); appsIt++) {
        int appId = (*appsIt)->m_id;

        // Loop results attached to the node
        for (vector<ResultContainer*>::iterator resultIt = node->m_resultContainerCollection->begin(); resultIt < node->m_resultContainerCollection->end(); resultIt++) {
            if (appId == (*resultIt)->m_applicationHandlerId) { //Find the matching result container of the application
                if ((*appsIt)->SendMessageStatus(node->m_icsId, (*resultIt)) == EXIT_FAILURE) {
                    return EXIT_FAILURE;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

int
SyncManager::ProcessApplicationResults()
{
    // Loop all the stations
    for (vector<ITetrisNode*>::iterator nodeIt = m_iTetrisNodeCollection->begin(); nodeIt < m_iTetrisNodeCollection->end(); nodeIt++) {
        ITetrisNode* node = *nodeIt;
        vector<ResultContainer*>::iterator resultIt;

        // Loop all the result of the station
        for (resultIt = node->m_resultContainerCollection->begin(); resultIt < node->m_resultContainerCollection->end(); resultIt++) {
            ResultContainer* result = *resultIt;

            ApplicationHandler* appHandler = NULL;
            // Loop applications installed in the station
            for (vector<ApplicationHandler*>::iterator appsIt = node->m_applicationHandlerInstalled->begin(); appsIt < node->m_applicationHandlerInstalled->end(); appsIt++) {
                if (result->m_applicationHandlerId == (*appsIt)->m_id) {
                    appHandler = (*appsIt);
                }
            }
            
            // Check there is app handler attached.
            if (appHandler == NULL) {
                cout << "iCS --> The result is not attached to any application handler." << endl;
                return EXIT_FAILURE;
            }
            bool noResult = result->ResultContainerIsEmpty(); // Check wether there is a new result
            if (!noResult) {
#ifdef LOG_ON
            	stringstream log;
            	log << "\niCS --> [ProcessApplicationResults] on node "<< node->m_icsId <<" ";
            	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                if (result->ApplyResult(GetAddress(), appHandler) == EXIT_FAILURE) {
                    return EXIT_FAILURE;
                }
            }
            else {
#ifdef LOG_ON
            	stringstream log;
            	log << "\n iCS --> [ProcessApplicationResults] on node "<< node->m_icsId <<" NO RESULTS TO PROCESS";
            	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            }
        }
    }

    return EXIT_SUCCESS;
}

void
SyncManager::RecognizeNewApplication(ApplicationHandler* appHandler)
{
    m_applicationHandlerCollection->push_back(appHandler);
}

void
SyncManager::RemoveNodeInTheArea(ITetrisNode* node)
{
    vector<V2xCamArea*>* camAreas = m_v2xMessageTracker->m_v2xCamAreaCollection;

    // Loop CAM areas
    vector<V2xCamArea*>::iterator camAreasIt;
    for (camAreasIt = camAreas->begin(); camAreasIt < camAreas->end(); camAreasIt++) {
        V2xCamArea* camArea = *camAreasIt;
        int subscriptionId = camArea->m_subscriptionId;
        Subscription* subscription = NULL;
        vector<Subscription*>::iterator subIt;
        for (subIt = m_subscriptionCollection->begin(); subIt < m_subscriptionCollection->end(); subIt++) {
            subscription = *subIt;
            if (subscriptionId == subscription->m_id) {
                break;
            }
        }

        if (subscription != NULL) {
            SubsSetCamArea* subSetCamArea = static_cast<SubsSetCamArea*>(subscription);
            vector<ITetrisNode*>* oldNodesInArea = subSetCamArea->getNodesInArea();
            subSetCamArea->RemoveNodeFromVector(node);
        }
    }
}

int SyncManager::ScheduleV2xMessages()
{
    if (ScheduleV2xCamAreaMessages() == EXIT_FAILURE) {
        cout << "iCS --> [ScheduleV2xMessages] Failure scheduling CAM messages." << endl;
        return EXIT_FAILURE;
    }

    if (ScheduleV2xGeobroadcastMessages() == EXIT_FAILURE) {
        cout << "iCS --> [ScheduleV2xGeobroadcastMessages] Failure scheduling Geobroadcast." << endl;
        return EXIT_FAILURE;
    }

    //Refresh the scheduled messages tables
    if (ScheduledCamMessageTable.size() > 0) {
        RefreshScheduledCamMessageTable();
    }

    if (ScheduledUnicastMessageTable.size() > 0) {
        RefreshScheduledUnicastMessageTable();
    }

    if (ScheduledGeobroadcastMessageTable.size() > 0) {
        RefreshScheduledGeobroadcastMessageTable();
    }

    if (ScheduledTopobroadcastMessageTable.size() > 0) {
        RefreshScheduledTopobroadcastMessageTable();
    }

    return EXIT_SUCCESS;
}

int
SyncManager::GetDataFromNs3()
{
    // STEP 1 GET RECEIVED MESSAGES FROM NS-3

    bool camMessageReceived = false; // Flag to know if any of the received messages is CAM
    bool GeobroadcastMessageReceived = false; // Flag to know if any of the received messages is GEOBROADCAST

    // Loop all the stations
    for (vector<ITetrisNode*>::iterator nodeIterator = m_iTetrisNodeCollection->begin(); nodeIterator != m_iTetrisNodeCollection->end(); nodeIterator++) {
        ITetrisNode* node = *nodeIterator;

        if (node->m_icsId == 0) // If the node is the TMC station skip the steps
            continue;

        vector<ReceivedMessage>* receivedMessages = new vector<ReceivedMessage> ();

        // Ask received messages for the station
        if (!m_wirelessComSimCommunicator->CommandGetReceivedMessages(node->m_nsId, receivedMessages)) {
#ifdef LOG_ON
        	stringstream log;
            log << "iCS --> [GetDataFromNs3] ERROR occurred when trying to obtain received messages";
            IcsLog::LogLevel((log.str()).c_str(),kLogLevelError);
#endif
            return EXIT_FAILURE;
        }
        else {

#ifdef LOG_ON
        	stringstream log;
            log << "GetDataFromNs3() Node " << node->m_icsId << " received " << receivedMessages->size() << " messages";
            IcsLog::LogLevel((log.str()).c_str(),kLogLevelInfo);
#endif

            // Loop received messages
            for (vector<ReceivedMessage>::iterator receivedIterator = receivedMessages->begin(); receivedIterator != receivedMessages->end(); receivedIterator++) {
                ReceivedMessage receivedMessage = *receivedIterator;
                // Check received message type
                switch (receivedMessage.messageType) {

                case CAM: {
                    camMessageReceived = true; // Change flag, at least one message is CAM
                    ScheduledCamMessageData rcvMessage;
                    rcvMessage.senderNs3ID = receivedMessage.senderId;
                    rcvMessage.messageType = receivedMessage.messageType;
                    rcvMessage.timeStep = receivedMessage.timeStep;
                    rcvMessage.sequenceNumber = receivedMessage.sequenceNumber;
                    rcvMessage.received = true;

                    if (ScheduledCamMessageTable.size() > 0) {
                        // Loop the scheduled message table to get the action ID associated
                        for (vector<ScheduledCamMessageData>::iterator
                                messageIterator = ScheduledCamMessageTable.begin(); messageIterator != ScheduledCamMessageTable.end(); messageIterator++) {
                            if (m_v2xMessageTracker->CompareCamRows(rcvMessage, (*messageIterator)) == true) {
                                rcvMessage.actionID = (*messageIterator).actionID;

                                // Groups the receivers that received the same message
                                // Facilities function needs all the receivers associated with the same Action ID
                                m_v2xMessageTracker->UpdateIdentifiersTable(IdentifiersStorageTable, (*messageIterator).actionID, (*messageIterator).senderIcsID, node->m_icsId);

                                // Change the status of the message (marks as received but more nodes could receive it)
                                if ((*messageIterator).received == false) {
                                    (*messageIterator).received = true;
                                }
                            }
                        }
                    } else {
                        IcsLog::LogLevel("GetDataFromNs3() There isn't any scheduled CAM message for the received message",kLogLevelWarning);
                    }
                    break;
                }

                case DENM: {
                    IcsLog::LogLevel("GetDataFromNs3() DENM message type process is to be implemented.",kLogLevelWarning);
                    break;
                }

                case UNICAST: {
                    ScheduledUnicastMessageData rcvMessage;
                    rcvMessage.senderNs3ID = receivedMessage.senderId;
                    rcvMessage.receiverNs3ID = node->m_nsId;
                    rcvMessage.messageType = receivedMessage.messageType;
                    rcvMessage.timeStep = receivedMessage.timeStep;
                    rcvMessage.sequenceNumber = receivedMessage.sequenceNumber;

#ifdef LOG_ON
                    stringstream log;
                    log <<  " iCS --> ProcessUnicastMessages() from NS3 from: "<< receivedMessage.senderId << " messageType:" << receivedMessage.messageType;
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

                    if (ScheduledUnicastMessageTable.size() > 0) {
                        // Loop the scheduled message table to get the action ID associated
                        vector<ScheduledUnicastMessageData>::iterator
                        messageIterator = ScheduledUnicastMessageTable.begin();
                        while (messageIterator != ScheduledUnicastMessageTable.end()) {
                            if (m_v2xMessageTracker->CompareUnicastRows(rcvMessage, (*messageIterator)) == true) {
                                rcvMessage.senderIcsID = (*messageIterator).senderIcsID;
                                rcvMessage.receiverIcsID = (*messageIterator).receiverIcsID;

                                // Call the Facilities
                                vector<stationID_t> vReceiver;
                                vReceiver.push_back((*messageIterator).receiverIcsID);
                                m_facilitiesManager->storeMessage((*messageIterator).actionID,vReceiver);
                                rcvMessage.appMessageId = (*messageIterator).appMessageId;

                                bool messageExistsInApp = false; // Control flag
                                // Loop all nodes to find the exact result attached to the message
                                for (vector<ITetrisNode*>::iterator nodeIt = m_iTetrisNodeCollection->begin(); nodeIt != m_iTetrisNodeCollection->end(); ++nodeIt) {
                                    // Loop the results of the node
                                    for (vector<ResultContainer*>::iterator resultIt = (*nodeIt)->m_resultContainerCollection->begin(); resultIt != (*nodeIt)->m_resultContainerCollection->end(); ++resultIt) {
                                        ResultContainer* result = (*resultIt);
                                        ResultContainer& reference = *result;

                                        const type_info& type = typeid(reference);
                                        if (type == typeid(ResultSetMaximumSpeed)) { // Use case E - SAA
                                            ResultSetMaximumSpeed* resultMaxSpeed;
                                            resultMaxSpeed = static_cast<ResultSetMaximumSpeed*>(result);
                                            if (resultMaxSpeed->CheckMessage(rcvMessage.appMessageId) == EXIT_SUCCESS) {
                                                messageExistsInApp = true;
                                            }
                                        }

                                        if (type == typeid(ResultTravelTime)) { // Use case B - TTE
                                            ResultTravelTime* resultTravelTime;
                                            resultTravelTime = static_cast<ResultTravelTime*>(result);
                                            if (resultTravelTime->CheckMessage(rcvMessage.appMessageId) == EXIT_SUCCESS) {
                                                messageExistsInApp = true;
                                            }
                                        }
                                    }
									// Loop the SUBSCRIPTIONs of the node
                                    // This part is only called if the generic subscriptions are used instead of the result-containers
                                    for (vector<Subscription*>::iterator subsIt = (*nodeIt)->m_subscriptionCollection->begin(); subsIt != (*nodeIt)->m_subscriptionCollection->end(); ++subsIt) {

                                    	Subscription* subscription = (*subsIt);
                                        Subscription& reference = *subscription;
                                        const type_info& typeofSubscription = typeid(reference);
                                        if (typeofSubscription == typeid(SubsAppMessageReceive)) {
                                            SubsAppMessageReceive* appMsgReceive = static_cast<SubsAppMessageReceive*>(subscription);
                                            ScheduledAPPMessageData appMessage;

                                            appMessage.senderNs3ID = receivedMessage.senderId;
                                            appMessage.receiverNs3ID = node->m_nsId;
                                            appMessage.messageType = receivedMessage.messageType;
                                            appMessage.timeStep = receivedMessage.timeStep;
                                            appMessage.sequenceNumber = receivedMessage.sequenceNumber;
                                            appMessage.senderIcsID = (*messageIterator).senderIcsID;
                                            appMessage.receiverIcsID = (*messageIterator).receiverIcsID;
                                            appMessage.actionID = (*messageIterator).actionID;
                                            appMessage.appMessageId  = (*messageIterator).appMessageId;

#ifdef LOG_ON
                                            stringstream log;
                                            log <<  "iCS --> Sync-manager - ProcessUnicastMessages() for APP_MSG_RECEIVE subscriptions:  senderID "<< (*messageIterator).senderIcsID << " receiverID "<< (*messageIterator).receiverIcsID << " appID " << (*messageIterator).appMessageId << " ActionID " <<  (*messageIterator).actionID ;
                                            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

                                            if (appMsgReceive->ProcessReceivedAppMessage(appMessage, GetAddress()) == EXIT_FAILURE) {
                                              IcsLog::LogLevel("iCS --> SynchManager - ProcessUnicastMessages() Error processing App message.", kLogLevelError);
                                              return EXIT_FAILURE;
                                            }
                                        }

                                    }
                                }
                                messageIterator = ScheduledUnicastMessageTable.erase(messageIterator);
                            } else {
                                ++messageIterator;
                            }
                        }
                    } else {
                        IcsLog::LogLevel("iCS --> sync-manager - GetDataFromNs3() There isn't any scheduled UNICAST message.", kLogLevelWarning);
                    }
                    break;
                }
                case GEOBROADCAST: {
                    GeobroadcastMessageReceived = true;
                    ScheduledGeobroadcastMessageData rcvMessage;
                    rcvMessage.senderNs3ID = receivedMessage.senderId;
                    rcvMessage.messageType = receivedMessage.messageType;
                    rcvMessage.timeStep = receivedMessage.timeStep;
                    rcvMessage.sequenceNumber = receivedMessage.sequenceNumber;
                    rcvMessage.received = true;

#ifdef LOG_ON
                    stringstream log;
                    log <<  "iCS --> ProcessGeoBroadcastMessages() from NS3 from: "<< receivedMessage.senderId << " messageType:" << receivedMessage.messageType;
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

                    ITetrisNode* sender = GetNodeByNs3Id(rcvMessage.senderNs3ID);
                    rcvMessage.senderIcsID = sender->m_icsId;

                    rcvMessage.receiverIcsID = node->m_icsId;

                    if (ProcessGeobroadcastMessages(rcvMessage, node->m_nsId) == EXIT_FAILURE) {
                        return EXIT_FAILURE;
                    }
                    break;
                    break;

                }
                case TOPOBROADCAST: {
                    ScheduledTopobroadcastMessageData rcvMessage;
                    rcvMessage.senderNs3ID = receivedMessage.senderId;
                    rcvMessage.messageType = receivedMessage.messageType;
                    rcvMessage.timeStep = receivedMessage.timeStep;
                    rcvMessage.sequenceNumber = receivedMessage.sequenceNumber;

                    if (ProcessTopobroadcastMessages(rcvMessage, node->m_nsId) == EXIT_FAILURE) {
                        return EXIT_FAILURE;
                    }
                    break;
                }
                default: {
#ifdef LOG_ON
                    stringstream log;
                    log << "[ERROR] GetDataFromNs3() The message received has a non defined message type.";
                    IcsLog::Log((log.str()).c_str());
#endif
                    return EXIT_FAILURE;
                    break;
                }
                }
            } // received messages
        }
    }

    // STEP 2: Transfer received Action ID (messages) to the facilities

    // If at least one message is CAM trigger the CAM message process
    if (camMessageReceived) {
        // Loop received Action IDs to get
        if (IdentifiersStorageTable.size() > 0) {
            vector<IdentifiersStorageStruct>::iterator idIterator =
                IdentifiersStorageTable.begin();
            while (idIterator != IdentifiersStorageTable.end()) {
                if ((*idIterator).stored == true) {
                    idIterator = IdentifiersStorageTable.erase(idIterator);
                } else {
                    vector<stationID_t> vReceivers;
                    vReceivers = m_v2xMessageTracker->GroupReceivers(IdentifiersStorageTable, (*idIterator).actionID, (*idIterator).senderID);

                    if (vReceivers.size() != 0) {
                        m_facilitiesManager->storeMessage((*idIterator).actionID, vReceivers);
                    } else {
                        cout << "[GetDataFromNs3] ERROR: There isn't any receiver for this message" << endl;
                        return EXIT_FAILURE;
                    }
                    ++idIterator; // Go to next identifier
                }
            }
        }

        // Optimization
        if (IdentifiersStorageTable.size() > 0) {
            IdentifiersStorageTable.clear();
        }

        // Erase from the CAM scheduling table in case there is no more receivers (received == true)
        if (ScheduledCamMessageTable.size() > 0) {
            vector<ScheduledCamMessageData>::iterator mIterator = ScheduledCamMessageTable.begin();
            while (mIterator != ScheduledCamMessageTable.end()) {
                if ((*mIterator).received == true) {

                    stringstream log;
                    log << "GetDataFromNs3() A received message has been erased from Scheduled CAM Message Table [senderID|time|seqN]: "
                    << "[" << (*mIterator).senderNs3ID << "|"
                    << (*mIterator).timeStep << "|"
                    << (*mIterator).sequenceNumber << "]";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

                    mIterator = ScheduledCamMessageTable.erase(mIterator);
                } else {
                    ++mIterator; // Go to the next schedule message
                }
            }
        } else {
            stringstream log;
            log << "GetDataFromNs3() There isn't any scheduled CAM message to be erased.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
        }
    }

    if (GeobroadcastMessageReceived) {
        // Erase from the GEOBROADCAST scheduling table in case there is no more receivers (received == true)
        if (ScheduledGeobroadcastMessageTable.size() > 0) {
            vector<ScheduledGeobroadcastMessageData>::iterator mIterator = ScheduledGeobroadcastMessageTable.begin();
            while (mIterator != ScheduledGeobroadcastMessageTable.end()) {
                if ((*mIterator).received == true) {

                    stringstream log;
                    log << "GetDataFromNs3() A received message has been erased from Scheduled GEOBROADCAST Message Table [senderID|time|seqN]: "
                    << "[" << (*mIterator).senderNs3ID << "|"
                    << (*mIterator).timeStep << "|"
                    << (*mIterator).sequenceNumber << "]";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

                    mIterator = ScheduledGeobroadcastMessageTable.erase(mIterator);
                } else {
                    ++mIterator; // Go to the next schedule message
                }
            }
        } else {
#ifdef LOG_ON
            stringstream log;
            log << "[GetDataFromNs3] WARNING There isn't any scheduled GEOBROADCAST message to be erased.";
            IcsLog::Log((log.str()).c_str());
#endif
        }
    }

    return EXIT_SUCCESS;
}

int
SyncManager::ProcessTopobroadcastMessages(ScheduledTopobroadcastMessageData rcvMessage, stationID_t receiverID)
{
    actionID_t actionID;

	if (ScheduledTopobroadcastMessageTable.size() > 0) {
        // Loop the scheduled message table to get the action ID associated
        vector<ScheduledTopobroadcastMessageData>::iterator
        messageIterator = ScheduledTopobroadcastMessageTable.begin();
        while (messageIterator != ScheduledTopobroadcastMessageTable.end()) {

            if (m_v2xMessageTracker->CompareTopobroadcastRows(rcvMessage, (*messageIterator)) == true) {
                rcvMessage.senderIcsID = (*messageIterator).senderIcsID;
                rcvMessage.appMessageId = (*messageIterator).appMessageId;
                actionID = (*messageIterator).actionID;
                // Call the Facilities
                vector<stationID_t> vReceiver;
                ////////////////////////////
                // MODIFIED by Florent KAISSER 01/31/2017
                // fix bug : the IDs in vReceiver vector must be a iCS ID and not a NS-3 ID
                //  use GetNodeByNs3Id to retrieve node from NS3 id
                //  and use m_icsId of the node to add it in vReceiver vector 
                ITetrisNode* receiver = GetNodeByNs3Id(receiverID);
                vReceiver.push_back(receiver->m_icsId);
                ////////////////////////////
                m_facilitiesManager->storeMessage((*messageIterator).actionID,vReceiver);
                rcvMessage.appMessageId = (*messageIterator).appMessageId;

                bool messageExistsInApp = false; // Control flag
                // Loop all nodes to find the exact result attached to the message
                for (vector<ITetrisNode*>::iterator nodeIt = m_iTetrisNodeCollection->begin(); nodeIt != m_iTetrisNodeCollection->end(); ++nodeIt) {
                    // Loop the results of the node
                    for (vector<ResultContainer*>::iterator resultIt = (*nodeIt)->m_resultContainerCollection->begin(); resultIt != (*nodeIt)->m_resultContainerCollection->end(); ++resultIt) {
                        ResultContainer* result = (*resultIt);
                        ResultContainer& reference =  *result;

                        const type_info& type = typeid(reference);

                        if (type == typeid(ResultTrafficJamDetection)) {
                            ResultTrafficJamDetection* resultTrafficJam;
                            resultTrafficJam = static_cast<ResultTrafficJamDetection*>(result);
                            if (resultTrafficJam->CheckMessage(rcvMessage.appMessageId, receiverID, GetAddress()) == EXIT_SUCCESS) {
                                messageExistsInApp = true;
                            }
                        }

                        if (type == typeid(ResultOpenBuslanes)) {
                            ResultOpenBuslanes* result = static_cast<ResultOpenBuslanes*>(result);
                            if (result->CheckMessage(rcvMessage.appMessageId, receiverID, GetAddress()) == EXIT_SUCCESS) {
                                messageExistsInApp = true;
                            }
                        }
                    } // the result of the node
                } // the nodes
                
                ////////////////////////////
                // MODIFIED by Florent KAISSER 01/31/2017
                // initialize receiverNs3ID and appMessageType 
                ScheduledAPPMessageData appMessage;
                appMessage.senderNs3ID = rcvMessage.senderNs3ID;
                appMessage.receiverNs3ID = receiverID;
                appMessage.messageType = rcvMessage.messageType;
                appMessage.timeStep = rcvMessage.timeStep;
                appMessage.sequenceNumber = rcvMessage.sequenceNumber;
                ITetrisNode* sender = GetNodeByNs3Id(rcvMessage.senderNs3ID);
                appMessage.senderIcsID = sender->m_icsId;
                appMessage.receiverIcsID = receiver->m_icsId;
                appMessage.actionID = actionID;
                appMessage.appMessageId = rcvMessage.appMessageId;
                appMessage.appMessageType = m_facilitiesManager->getApplicationMessageType(actionID);
                ////////////////////////////                

                if (ProcessAppMessages(appMessage) == EXIT_FAILURE) {
#ifdef LOG_ON
                  stringstream log;
                  log << "[ERROR] GetDataFromNs3() Processing AppMessage failed.";
                  IcsLog::Log((log.str()).c_str());
#endif

                }
            } // if

            ++messageIterator;

        } // while
    } else {
        stringstream log;
        log << "ProcessTopobroadcastMessages() There isn't any scheduled TOPOBROADCAST message.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
    }
    return EXIT_SUCCESS;
}

int
SyncManager::InitializeFacilities(const std::string& file)
{
    if (m_facilitiesManager->configureICSFacilities(file))
        return EXIT_SUCCESS;

    return EXIT_FAILURE;
}

int SyncManager::ScheduleV2xCamAreaMessages()
{
    vector<V2xCamArea*>* camAreas = m_v2xMessageTracker->m_v2xCamAreaCollection;

    vector<string> idNodesToStop;
    vector<string> idNodesToStart;

    // Loop on each station
    for (vector<ITetrisNode*>::iterator nodeIt = m_iTetrisNodeCollection->begin(); nodeIt != m_iTetrisNodeCollection->end(); ++nodeIt) {

        if ((*nodeIt)->m_icsId == 0)  // Skip TMC node
            continue;

        // Flag to know if the current node is inside at least one CAM area.
        bool isNodeInAnyArea = false;
        // Initialize the frequency to send CAM messages, if necessary.
        float maxFrequency = 0.0;
        // Initialize the payload length of the CAM messages to be sent, if necessary.
        unsigned int maxPayloadLength = 0;

        // Loop CAM areas
        for (vector<V2xCamArea*>::iterator camAreasIt = camAreas->begin(); camAreasIt < camAreas->end(); camAreasIt++) {

            // Get the subscriptionId of to the current V2xcamArea
            V2xCamArea* camArea = *camAreasIt;
            int subscriptionId = camArea->m_subscriptionId;
            Subscription* subscription = NULL;

            // Find the subscription that created the CAM area
            for (vector<Subscription*>::iterator subIt = m_subscriptionCollection->begin(); subIt < m_subscriptionCollection->end(); subIt++) {
                subscription = *subIt;
                if (subscriptionId == subscription->m_id) {
                    break;
                }
            }
            if (subscription == NULL) {
                cout << "iCS --> [ScheduleV2xCamAreaMessages] ERROR The CAM area was not created by a subscription." << endl;
                return EXIT_FAILURE;
            } else {

                // Cast subscription to the SubsSetCamArea type
                SubsSetCamArea* subSetCamArea = static_cast<SubsSetCamArea*>(subscription);

                nodeStatusInArea_t nodeStatus = subSetCamArea->checkNodeStatus(*nodeIt);

                switch (nodeStatus) {
                case((int) JustArrivedInArea): {
                    subSetCamArea->AddNodeToVector((*nodeIt));
                    if (maxFrequency < subSetCamArea->GetFrequency())
                        maxFrequency = subSetCamArea->GetFrequency();

                    if (maxPayloadLength < camArea->m_payloadLength)
                        maxPayloadLength = camArea->m_payloadLength;

                    isNodeInAnyArea = true;
                    break;
                }
                case((int) JustLeftArea): {
                    subSetCamArea->RemoveNodeFromVector((*nodeIt));
                    break;
                }
                case((int) StillInsideArea): {
                    isNodeInAnyArea = true;
                    if (maxFrequency < subSetCamArea->GetFrequency())
                        maxFrequency = subSetCamArea->GetFrequency();

                    if (maxPayloadLength < camArea->m_payloadLength)
                        maxPayloadLength = camArea->m_payloadLength;

                    break;
                }
                case((int) StillOutsideArea): {
                    break;
                }
                default: {
                    cout << "iCS --> [ScheduleV2xCamAreaMessages] ERROR It is not possible to determine the status of node " << (*nodeIt)->m_icsId;
                    cout << " within the current V2X-cam-area." << endl;
                    return EXIT_FAILURE;
                }
                }
            }
        }

        if (SubsSetCamArea::isNodeInGeneralCamSubscriptionVector((*nodeIt))) {
            if (isNodeInAnyArea &&
                    (maxFrequency != SubsSetCamArea::getCamFrequencyFromCamSubscriptionVector((*nodeIt)) ||
                     maxPayloadLength != SubsSetCamArea::getCamPayloadLengthFromCamSubscriptionVector((*nodeIt)))
               ) {
                // Stop sending CAM messages (indicate the node by its name known to the wireless simulator)
                stringstream outStop;
                outStop << (*nodeIt)->m_nsId;
                idNodesToStop.push_back(outStop.str());

                // Update new maxFrequency to the generalCamSubscriptionVector
                SubsSetCamArea::setNewCamFrequencyInCamSubscriptionVector((*nodeIt), maxFrequency);

                // Update new maxPayloadLength to the generalCamSubscriptionVector
                SubsSetCamArea::setNewPayloadLengthInCamSubscriptionVector((*nodeIt), maxPayloadLength);

                // Start sending CAM messages with new frequency
                stringstream outStart;
                outStart << (*nodeIt)->m_nsId;
                idNodesToStart.push_back(outStart.str());
            }

            if (!isNodeInAnyArea) {
                // Stop sending messages
                stringstream out;
                out << (*nodeIt)->m_nsId;
                idNodesToStop.push_back(out.str());

                // remove the node from the generalCamSubscriptionVector
                SubsSetCamArea::removeNodeFromCamSubscriptionVector((*nodeIt)->m_icsId);
            }
        } else {
            if (isNodeInAnyArea) {
                // Add the node to the generalCamSubscriptionVector
                SubsSetCamArea::addNodeToCamSubscriptionVector((*nodeIt), maxFrequency, maxPayloadLength);

                // Start sending CAM messages
                stringstream outStart;
                outStart << (*nodeIt)->m_nsId;
                idNodesToStart.push_back(outStart.str());
            }
        }

    }

    // Stop CAM transmission for the nodes in the vector idNodesToStop
    if (idNodesToStop.size() > 0) {
        m_wirelessComSimCommunicator->CommandStopSendingCam(idNodesToStop);
    }


    //TODO - For sending CAM messages we are using the method CommandStartSendingCam(sendersId, payloadLength, frequency)
    //       where the payloadLength and the frequency is common for all the areas. This might NOT happen (in fact, each subscription has
    //       its own payloadLength and frequency).
    //       In order to keep the implementation compliant with the current implementation, we retrieve from the generalCamSubscriptionVector
    //       the max frequency and payloadLeght among all the nodes.
    //       However, this method has to be changed by passing per each station its frequency and payloadLength.

    // Get the max frequency for the nodes (to be removed upon update of the CommandStartSendingCam() method)
    float maxGeneralFrequency = SubsSetCamArea::getMaxFrequencyFromCamSubscriptionVector();
    // Get the max payloadLength for the nodes (to be removed upon update of the CommandStartSendingCam() method)
    unsigned int maxGeneralPayloadLength = SubsSetCamArea::getMaxPayloadLengthFromCamSubscriptionVector();

    // Start CAM transmission for the nodes in the vector idNodesToStart
    if (idNodesToStart.size() > 0) {
        if (m_wirelessComSimCommunicator->CommandStartSendingCam(idNodesToStart, maxGeneralPayloadLength, maxGeneralFrequency) == false) {
            cout << "iCS --> [ScheduleV2xCamAreaMessages] ERROR While commanding to start sending CAM in the network simulator." << endl;
            return EXIT_FAILURE;
        }
    }

    // --- Create Payload and store CAMs
    //TODO: We only consider a unique frequency and payload until the method CommandStartSendingCam() is fixed

    vector<TGeneralCamSubscription>::iterator itGenCamSubsVect;
    for (itGenCamSubsVect = SubsSetCamArea::generalCamSubscriptions.begin(); itGenCamSubsVect != SubsSetCamArea::generalCamSubscriptions.end(); itGenCamSubsVect++) {
        // Loop the nodes to get and store Facilities Action ID

        TGeneralCamSubscription currGCS = *itGenCamSubsVect;

        // If the node does not exist anymore in the simulation, delete it from the subscription
        bool nodeInSimulation = false;
        if (currGCS.node != NULL) {
            for (vector<ITetrisNode*>::iterator nodeIt = m_iTetrisNodeCollection->begin();
                    nodeIt != m_iTetrisNodeCollection->end(); ++nodeIt) {
                if ((*nodeIt)->m_icsId == currGCS.stationId) {
                    nodeInSimulation = true;
                    break;
                }
            }
        }

        if (nodeInSimulation) {
#ifdef LOG_ON
            {
                stringstream log;
                log << "[INFO] ScheduleV2xCamAreaMessages() create and store cams for node: " << itGenCamSubsVect->stationId;
                IcsLog::Log((log.str()).c_str());
            }
#endif
            bool nodeWithWave = false;
            for (set<string>::iterator itRats = currGCS.node->m_rats.begin(); itRats != currGCS.node->m_rats.end(); itRats++) {
                if ((*itRats) == "WaveVehicle" || (*itRats) == "WaveRsu") {
                    nodeWithWave = true;
                    break;
                }
            }
            if (nodeWithWave == true) {
                ScheduledCamMessageData messageData;

//                int numberOfMessages = it->maxFrequency;  // TODO: We must use this when the method CommandStartSendingCam() is fixed
                int numberOfMessages = (unsigned int) maxGeneralFrequency; // TODO: To delete when the method CommandStartSendingCam() is fixed

                // Stores one Action ID per message in the timestep (i.e. frequency)
                for (int i = 0; i < numberOfMessages; i++) {
                    messageData.senderNs3ID = currGCS.node->m_nsId;
                    messageData.senderIcsID = currGCS.stationId;
                    messageData.messageType = CAM; // NOTE, this is dynamic, should change to DNEM or so
                    messageData.timeStep = m_simStep + 1;
                    messageData.sequenceNumber = i;
                    messageData.actionID = m_facilitiesManager->createCAMpayload(currGCS.stationId); // NOTE, this is dynamic
                    messageData.received = false;
                    m_v2xMessageTracker->InsertCamRow(ScheduledCamMessageTable, messageData);
                }
            } else {
#ifdef LOG_ON
                stringstream log;
                log << "[WARNING] ScheduleV2xCamAreaMessages() Wave technology hasn't been installed in node " << currGCS.stationId;
                IcsLog::Log((log.str()).c_str());
#endif
            }
        }
    }
    return EXIT_SUCCESS;
}

int
SyncManager::ScheduleV2xGeobroadcastMessages()
{
//     // 1. Loop Geobroadcast areas
//     for (vector<V2xGeobroadcastArea*>::iterator it = m_v2xMessageTracker->m_v2xGeobroadcastAreaCollection->begin(); it != m_v2xMessageTracker->m_v2xGeobroadcastAreaCollection->end(); it++) {
//         V2xGeobroadcastArea* geobroadcastArea = *it;
// 
//         int subscriptionId = geobroadcastArea->m_subscriptionId;
//         Subscription* subscription = NULL;
// 
//         // Find the subscription that created the GEOBROADCAST area
//         for (vector<Subscription*>::iterator subIt = m_subscriptionCollection->begin(); subIt < m_subscriptionCollection->end(); subIt++) {
//             subscription = *subIt;
//             if (subscriptionId == subscription->m_id) {
//                 break;
//             }
//         }
// 
//         if (subscription == NULL) {
//             stringstream log;
//             log << "ScheduleV2xGeobroadcastMessages() The GEOBROADCAST area was not created by a subscription.";
//             IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
//             return EXIT_FAILURE;
//         } else {
//             // Create the structure to store in the scheduled geobroadcast messages table
//             ScheduledGeobroadcastMessageData messageData;
// 
//             // Identify the type of subscription
//             Subscription& reference = *subscription;
//             const type_info& typeofSubscription = typeid(reference);
//             unsigned int msgLifetime;
//             float msgRegenerationTime;
//             GeometricShape* areaShape;
//             // for the AppMsgSend and TMC
//             std::vector<std::string> techList;
//             unsigned char commProfile;
// 
//             int appId;
//             bool subscriptionExits = false;
// 
//             if (typeofSubscription == typeid(SubsStartTravelTimeCalculation)) { // TRAVEL TIME ESTIMATION START
//                 messageData.subscriptionType = "START";
//                 SubsStartTravelTimeCalculation* subStartTime = static_cast<SubsStartTravelTimeCalculation*>(subscription);
//                 msgLifetime = subStartTime->m_messageLifeTime;
//                 msgRegenerationTime = subStartTime->m_messageRegenerationTime;
//                 areaShape =  &(subStartTime->m_communicationArea);
//                 appId = subStartTime->m_appId;
//                 subscriptionExits = true;
//             }
// 
//             if (typeofSubscription == typeid(SubsStopTravelTimeCalculation)) { // TRAVEL TIME ESTIMATION STOP
//                 messageData.subscriptionType = "STOP";
//                 SubsStopTravelTimeCalculation* subStopTime = static_cast<SubsStopTravelTimeCalculation*>(subscription);
//                 msgLifetime = subStopTime->m_messageLifeTime;
//                 msgRegenerationTime = subStopTime->m_messageRegenerationTime;
//                 areaShape =  &(subStopTime->m_communicationArea);
//                 appId = subStopTime->m_appId;
//                 subscriptionExits = true;
//             }
// 
//             if (typeofSubscription == typeid(SubsAppMessageSend)) { // Generic PUSH Send subscription
//             	SubsAppMessageSend* appMsgSend = static_cast<SubsAppMessageSend*>(subscription);
// 
//             	messageData.subscriptionType = "SubsAppMessageSend"; //APPMSGSEND
//             	msgLifetime = appMsgSend->getMsgLifetime();
//             	msgRegenerationTime = appMsgSend->getMsgRegenerationTime();
//             	//add technolist
//             	if ((appMsgSend->getPrefTechs() & 0x01)==true)
//             	   techList.push_back("WaveVehicle");
// 
//             	if ((appMsgSend->getPrefTechs() & 0x02)==true)
//             	    techList.push_back("UmtsVehicle");
// 
//             	if ((appMsgSend->getPrefTechs() & 0x03)==true)
//             	    techList.push_back("WimaxVehicle");
// 
//             	if ((appMsgSend->getPrefTechs() & 0x04)==true)
//             		techList.push_back("DvbhVehicle");
// 
//             	commProfile = appMsgSend->getCommProfile();
//             	areaShape =  &(appMsgSend->getCircleFromAreas());
//             	appId = appMsgSend->m_appId;
//             	subscriptionExits = true;
//             }
// 
//             if (!subscriptionExits) { // ERROR
//                 stringstream log;
//                 log << "ScheduleV2xGeobroadcastMessages() This type of subscription "<< typeofSubscription.name() <<" cannot create a GEOBROADCAST area.";
//                 IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
//                 cout << (log.str()).c_str() << endl;
//                 return EXIT_FAILURE;
//             }
// 
//             // Store data in the facilities and in the scheduled geobroadcast messages table
//             int numberOfMessages = geobroadcastArea->m_frequency;
//             ITetrisNode* node = GetNodeByIcsId(subscription->m_nodeId);
// 
//             for (int i = 0; i < numberOfMessages; i++) {
// 
//                 // TODO: Add the messages to the m_facilitiesManager->createApplicationMessagePayload()
// 
//                 messageData.senderNs3ID = node->m_nsId;
//                 messageData.senderIcsID = subscription->m_nodeId;
//                 messageData.messageType = GEOBROADCAST;
//                 messageData.timeStep = m_simStep + 1;
//                 messageData.sequenceNumber = i;
//                 messageData.received = false;
//                 messageData.appMessageId  = appId;
//                 m_v2xMessageTracker->InsertGeobroadcastRow(ScheduledGeobroadcastMessageTable, messageData);
//             }
// 
//             // Schedule GEOBROADCAST message in ns-3
//             vector<string> sendersId;
//             int senderNs3Id = node->m_nsId;
//             sendersId.push_back(utils::Conversion::int2String(senderNs3Id));
// 
//             // Get service ID for the geobroadcasting
//             string serviceId = "NULL";
//             for (vector<ApplicationHandler*>::iterator it = m_applicationHandlerCollection->begin(); it != m_applicationHandlerCollection->end(); ++it) {
//                 if ((*it)->m_id == subscription->m_appId) {
//                     serviceId = (*it)->m_serviceId.geoBroadcastServiceId;
//                     break;
//                 }
//             }
//             if (serviceId == "NULL") {
// #ifdef LOG_ON
//                 stringstream log;
//                 log << "ScheduleV2xGeobroadcastMessages() Service ID is not correct.";
//                 IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
// #endif
//                 cout << "iCS --> [ERROR] ScheduleV2xGeobroadcastMessages() Service ID is not correct" << endl;
//                 return EXIT_FAILURE;
//             }
// 
//             CircularGeoAddress destination;
//             Circle circle;
//             switch (areaShape->getShapeType()) {
//             case CIRCLE: {
//                 Circle* tmpArea = dynamic_cast<Circle*>(areaShape);
//                 circle = *tmpArea;
//                 break;
//             }
//             case ELLIPSE: {
//                 ics_types::Ellipse* tmpArea = dynamic_cast<ics_types::Ellipse*>(areaShape);
//                 circle = tmpArea->getCircumscribedCircle();
//                 break;
//             }
//             case RECTANGLE: {
//                 ics_types::Rectangle* tmpArea = dynamic_cast<ics_types::Rectangle*>(areaShape);
//                 circle = tmpArea->getCircumscribedCircle();
//                 break;
//             }
//             case CONVEXPOLIGON: {
//                 ConvexPolygon* tmpArea = dynamic_cast<ConvexPolygon*>(areaShape);
//                 circle = tmpArea->getCircumscribedCircle();
//                 break;
//             }
//             }
// 
//             double x = (double)circle.getCenter().x();
//             double y = (double)circle.getCenter().y();
//             vector <double> degreesCoordinates = LocToGeoConvert(x,y, 0, m_facilitiesManager->getLat0(), m_facilitiesManager->getLon0(), m_facilitiesManager->getAlt0());
//             destination.lat = degreesCoordinates.at(0);
//             destination.lon = degreesCoordinates.at(1);
//             destination.areaSize = (uint32_t)circle.getArea();
// 
//             // JHNote: TMC has a specific send message
//             if (subscription->m_nodeId == 0)
//             	m_wirelessComSimCommunicator->CommandStartMWTxon(sendersId, serviceId, commProfile, techList, destination, geobroadcastArea->m_frequency, geobroadcastArea->m_payloadLength, msgRegenerationTime, msgLifetime);
//             else {
//             	 m_wirelessComSimCommunicator->StartGeobroadcastTxon(sendersId, serviceId, commProfile, techList, destination, geobroadcastArea->m_frequency, geobroadcastArea->m_payloadLength, msgRegenerationTime, msgLifetime);
//             }
//         }
//     }
    return EXIT_SUCCESS;
}


int
SyncManager::ScheduleV2xUnicastMessages(
    stationID_t sender, int appHandlerId, ITetrisNode* destination,
    int appMessageId, unsigned char appMessageType, int frequency, int payloadLength, float msgRegenerationTime,
    unsigned char commProfile, unsigned char preferredRATs, unsigned int msgLifetime)
{
    if (destination == NULL) {
        IcsLog::LogLevel("ScheduleV2xUnicastMessages() Destination node is null pointer.", kLogLevelError);
        return EXIT_FAILURE;
    }

    vector<RATID>* rats = m_facilitiesManager->getStationActiveRATs(sender);
    vector<string> techList;
    if (rats->size() == 0) {
        cout << "[ScheduleResult] getStationActiveRATs returned 0 size" << endl;
        return EXIT_FAILURE;
    } else {
        for (vector<RATID>::iterator it = rats->begin(); it != rats->end(); it++) {
            switch (*it) {
            case WAVE: {
                // Check if the available technology is also among the preferred for the message
                if ((preferredRATs & 0x01) == 0x01) {
                    techList.push_back("WaveVehicle");
                    cout << "[iCS --> ScheduleV2xUnicastMessages] getStationActiveRATs is WaveVehicle " << endl;
                }
                break;
            }
            case UMTS: {
                // Check if the available technology is also among the preferred for the message
                if ((preferredRATs & 0x02) == 0x02) {
                    techList.push_back("UmtsVehicle");
                    cout << "[iCS --> ScheduleV2xUnicastMessages] getStationActiveRATs is UmtsVehicle " << endl;
                }
                break;
            }
            case WiMAX: {
	           techList.push_back("WimaxVehicle");
		       break;
            }
            case DVBH: {
                techList.push_back("DvbhVehicle");
                break;
            }
            default: {
                cout << "[ERROR] ScheduleV2xUnicastMessages() There is no match for the type of RAT" << endl;
                delete rats;
                return EXIT_FAILURE;
            }
            }
        }
    }

    // Convert sender ID to string in a vector
    vector<string> sendersId;
    int senderNs3Id = GetNodeByIcsId(sender)->m_nsId;
    sendersId.push_back(utils::Conversion::int2String(senderNs3Id));
    // Convert receiver
    unsigned int destinationNs3Id = destination->m_nsId;

    string serviceId = "NULL";
    for (vector<ApplicationHandler*>::iterator it = m_applicationHandlerCollection->begin(); it != m_applicationHandlerCollection->end(); ++it) {
        if ((*it)->m_id == appHandlerId) {
            serviceId = (*it)->m_serviceId.unicastServiceId;
            break;
        }
    }
    if (serviceId == "NULL") {
        cout
            << "iCS --> [ERROR] ScheduleV2xUnicastMessages() Service ID is not correct"
            << endl;
        return EXIT_FAILURE;
    }

    // 1. GENERATE PAYLOAD + ACTIONID IN THE FACILITIES
    ScheduledUnicastMessageData messageData;
    bool exists = false;

    // Stores one Action ID per message in the timestep (i.e. frequency)
    for (int i = 0; i < frequency; i++) {
        messageData.senderNs3ID = senderNs3Id;
        messageData.senderIcsID = sender;
        messageData.receiverNs3ID = destination->m_nsId;
        messageData.receiverIcsID = destination->m_icsId;
        messageData.messageType = UNICAST;
        messageData.timeStep = m_simStep + 1;
        messageData.sequenceNumber = i;
        messageData.appMessageId = appMessageId;

#ifdef LOG_ON
       stringstream log;
       log <<  "iCS -->  [ScheduleV2xUnicastMessages] try to schedule UNICAST from "<<messageData.senderIcsID<< " to "<< messageData.receiverIcsID <<" SN "
               << messageData.sequenceNumber << " at " << messageData.timeStep << "";
       IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        // Checks if the message has already been scheduled
        if (ScheduledUnicastMessageTable.size() > 0) {
            for (vector<ScheduledUnicastMessageData>::iterator it = ScheduledUnicastMessageTable.begin(); it != ScheduledUnicastMessageTable.end(); it++) {
                if ((messageData.senderIcsID == (*it).senderIcsID)
                        && (messageData.receiverIcsID == (*it).receiverIcsID)
                        && (messageData.timeStep == (*it).timeStep)
                        && (messageData.sequenceNumber == (*it).sequenceNumber)) {
                    exists = true;
                    break;
                } else {
                    exists = false;
                }
            }
        } else {
            exists = false;
        }

        if (!exists) {
	  
            //Call facilities
            TApplicationMessageDestination appDest;
            appDest.dest_station = messageData.receiverIcsID;
            messageData.actionID = m_facilitiesManager->createApplicationMessagePayload(
                                       messageData.senderIcsID,
                                       messageData.messageType,
                                       appDest,
                                       preferredRATs,
                                       payloadLength,
                                       commProfile,
                                       appHandlerId,
                                       appMessageType,
                                       appMessageId);
				       
            m_v2xMessageTracker->InsertUnicastRow(ScheduledUnicastMessageTable, messageData);

            // 2. SCHEDULE MESSAGE IN ns-3
            ITetrisNode& receiverReference = *destination;
            ITetrisNode* senderNode = GetNodeByIcsId(sender);
            ITetrisNode& senderReference = *senderNode;
            const type_info& typeinfoRcv = typeid(receiverReference);
            const type_info& typeinfoSender = typeid(senderReference);

#ifdef LOG_ON
            stringstream log;
            log <<  "iCS -->  [ScheduleV2xUnicastMessages] serviceId " << serviceId << " commProfle " << commProfile << " frequency " << frequency << " payloadLength " << payloadLength << " destinationNs3Id " << destinationNs3Id << " msgRegenerationTime " << msgRegenerationTime << " msgLifetime " << msgLifetime<<"";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

            // Decide whether the tx is from the a RSU to a vehicle or viceversa
            if ((typeinfoSender == typeid(VehicleNode) || ((typeinfoSender == typeid(FixedNode)) && techList.front() == "WaveVehicle"))) {
                //Sender->vehicle  Receiver->RSU
                if (!(SyncManager::m_wirelessComSimCommunicator->CommandStartIdBasedTxon(
                            sendersId, serviceId, commProfile, techList,frequency, payloadLength,
                            destinationNs3Id, msgRegenerationTime,
                            msgLifetime))) {
#ifdef LOG_ON
                    stringstream log;
                    log <<  "iCS -->  [ScheduleV2xUnicastMessages] [ERROR] ScheduleResult() Error while scheduling id based Txon";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif

                    delete rats;
                    return EXIT_FAILURE;
                }
                else {
#ifdef LOG_ON
                    stringstream log;
                    log <<  "iCS -->  [ScheduleV2xUnicastMessages] message correctly scheduled for transmission in ns-3";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
                }

            } else {
                //Sender->RSU  Receiver->vehicle
                if (!(SyncManager::m_wirelessComSimCommunicator->CommandStartIpCiuTxon(sendersId, serviceId, frequency, payloadLength, destinationNs3Id, msgRegenerationTime))) {
#ifdef LOG_ON
                    stringstream log;
                    log <<  "iCS -->  [ScheduleV2xUnicastMessages] [ERROR] ScheduleResult() Error while scheduling IP based Txon";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
                    delete rats;
                    return EXIT_FAILURE;
                }
            }
        }
        else {
#ifdef LOG_ON
                    stringstream log;
                    log <<  "iCS -->  [ScheduleV2xUnicastMessages] message not scheduled beacuse already scheduled";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
	    }
    }
    delete rats;
    return EXIT_SUCCESS;
}

int
SyncManager::ScheduleV2XApplicationMessages(
    stationID_t sender, int appHandlerId, ITetrisNode* destination,
    int appMessageId, unsigned char appMessageType, int frequency, int payloadLength, float msgRegenerationTime,
    unsigned char commProfile, unsigned char preferredRATs, std::vector<Area2D*> areas, short numHops, unsigned int msgLifetime)
{

    if ((destination == NULL) && (areas.size() == 0)) {
    	IcsLog::LogLevel("ScheduleV2XApplicationMessages() At least Destination node OR Gegraphic Area MUST NOT be null pointer.", kLogLevelError);
        return EXIT_FAILURE;
    }

    if(appMessageType ==  UNICAST) {
    	if (ScheduleV2xUnicastMessages(sender, appHandlerId, destination,
    		    appMessageId, appMessageType, frequency, payloadLength, msgRegenerationTime,
    		    commProfile, preferredRATs, msgLifetime)== EXIT_FAILURE) {

    		cout << "iCS --> [ScheduleV2xUnicastMessages] Failure scheduling Geobroadcast." << endl;
    		return EXIT_FAILURE;
    	}
    }
    else if (appMessageType == GEOBROADCAST) {
    	if (ScheduleV2xGeobroadcastMessages(
    	    sender, appHandlerId, appMessageId,
    	    frequency, payloadLength, msgRegenerationTime,
    	    appMessageType, commProfile, preferredRATs,
    	    areas, msgLifetime)== EXIT_FAILURE) {

    		 cout << "iCS --> [ScheduleV2xGeobroadcastMessages] Failure scheduling Geobroadcast." << endl;
    		 return EXIT_FAILURE;
    	}
    }
    else if (appMessageType == TOPOBROADCAST) {
    	if (ScheduleV2xTopobroadcastMessages(
    	    sender, appHandlerId, appMessageId,
    	    frequency, payloadLength, msgRegenerationTime,
    	    appMessageType, commProfile, preferredRATs, numHops, msgLifetime)== EXIT_FAILURE) // TODO add numHops in the subs
    	{
    	    cout << "iCS --> [ScheduleV2xTopobroadcastMessages] Failure scheduling Geobroadcast." << endl;
    	    return EXIT_FAILURE;
    	}

    }
    else if (appMessageType == CAM) {
        	    cout << "iCS --> [ScheduleV2XApplicationMessages()] CAM scheduling should not be sent using this subscription. Please use ScheduleCAMAreaMessage() instead" << endl;
        	    return EXIT_FAILURE;


    }
    else if (appMessageType == DENM) {
       cout << "iCS --> [ScheduleV2XApplicationMessages()] Dedicated DENM scheduling is not supported yet." << endl;
       return EXIT_FAILURE;

    }
    else {
    	IcsLog::LogLevel("ScheduleV2XApplicationMessages() Unsupported appMessageType.", kLogLevelError);
    	return EXIT_FAILURE;
    }


    //Refresh the scheduled messages tables
    if (ScheduledUnicastMessageTable.size() > 0) {
        RefreshScheduledUnicastMessageTable();
    }

    if (ScheduledGeobroadcastMessageTable.size() > 0) {
        RefreshScheduledGeobroadcastMessageTable();
    }

    if (ScheduledTopobroadcastMessageTable.size() > 0) {
        RefreshScheduledTopobroadcastMessageTable();
    }

    return EXIT_SUCCESS;
}

int
SyncManager::ScheduleV2xTopobroadcastMessages(
    stationID_t sender, int appHandlerId, int appMessageId,
    int frequency, int payloadLength, float msgRegenerationTime,
    unsigned char appMessageType, unsigned char commProfile, unsigned char preferredRATs, short numHops, unsigned int msgLifetime)
{

    vector<RATID>* rats = m_facilitiesManager->getStationActiveRATs(sender);
    vector<string> techList;
    if (rats->size() == 0) {
        cout << "[ScheduleResult] getStationActiveRATs returned 0 size" << endl;
        return EXIT_FAILURE;
    } else {
        bool nodeWithWave = false;
        for (vector<RATID>::iterator it = rats->begin(); it != rats->end(); it++) {
            if ((*it) == WAVE) {
                nodeWithWave = true;
                break;
            }
        }

        if (!nodeWithWave) {
            cout << "[ERROR] ScheduleV2xTopobroadcastMessages() There is no match for the type of RAT" << endl;
            return EXIT_FAILURE;
        }

        if (nodeWithWave && ((preferredRATs & 0x01)!=0x01)) {
            cout << "[ERROR] ScheduleV2xTopobroadcastMessages() Topobroadcast can be performed only with WAVE, "
                 "and this technology was not set in the preferred technologies in the application message header." << endl;
            return EXIT_FAILURE;
        }
    }

    // Convert sender ID to string in a vector
    vector<string> sendersId;
    int senderNs3Id = GetNodeByIcsId(sender)->m_nsId;
    sendersId.push_back(utils::Conversion::int2String(senderNs3Id));

    string serviceId = "NULL";
    for (vector<ApplicationHandler*>::iterator it = m_applicationHandlerCollection->begin(); it != m_applicationHandlerCollection->end(); ++it) {
        if ((*it)->m_id == appHandlerId) {
            serviceId = (*it)->m_serviceId.topoBroadcastServiceId;
            break;
        }
    }

    if (serviceId == "NULL") {
        cout
            << "iCS --> [ERROR] ScheduleV2xTopobroadcastMessages() Service ID is not correct"
            << endl;
        return EXIT_FAILURE;
    }

    // 1. GENERATE PAYLOAD + ACTIONID IN THE FACILITIES

    ScheduledTopobroadcastMessageData messageData;
    bool exists = false;

    // Stores one Action ID per message in the timestep (i.e. frequency)
    for (int i = 0; i < frequency; i++) {
        messageData.senderNs3ID = senderNs3Id;
        messageData.senderIcsID = sender;
        messageData.messageType = TOPOBROADCAST;
        messageData.timeStep = m_simStep;
        messageData.sequenceNumber = i;
        messageData.appMessageId = appMessageId;

        // Checks if the message has already been scheduled
        if (ScheduledTopobroadcastMessageTable.size() > 0) {
            for (vector<ScheduledTopobroadcastMessageData>::iterator it = ScheduledTopobroadcastMessageTable.begin(); it != ScheduledTopobroadcastMessageTable.end(); it++) {
                if ((messageData.senderIcsID == (*it).senderIcsID) && (messageData.timeStep == (*it).timeStep) && (messageData.sequenceNumber == (*it).sequenceNumber)) {
                    exists = true;
                    break;
                } else {
                    exists = false;
                }
            }
        } else {
            exists = false;
        }

        if (!exists) {
            //Call facilities
            TApplicationMessageDestination appDest;
            appDest.dest_numHops = numHops;
            messageData.actionID = m_facilitiesManager->createApplicationMessagePayload(
                                       messageData.senderIcsID,
                                       messageData.messageType,
                                       appDest,
                                       preferredRATs,
                                       payloadLength,
                                       commProfile,
                                       appHandlerId,
                                       appMessageType,
                                       appMessageId);
            m_v2xMessageTracker->InsertTopobroadcastRow(ScheduledTopobroadcastMessageTable, messageData);

            // 2. SCHEDULE MESSAGE IN ns-3
            ITetrisNode* senderNode = GetNodeByIcsId(sender);
            ITetrisNode& senderReference = *senderNode;
            const type_info& typeinfoSender = typeid(senderReference);


            if (sender == 0) {
            	 cout
            	            << "iCS --> [ERROR] ScheduleV2xTopobroadcastMessages() TMC cannot send Topobroadcast"
            	            << endl;
            }
            else  {
               if (!SyncManager::m_wirelessComSimCommunicator->CommandStartTopoTxon(sendersId, "serviceIdTopobroadcast", commProfile, techList, frequency, payloadLength, msgRegenerationTime, 1, (unsigned int) numHops)) {
                  cout << "[ERROR] ScheduleResult() Error while scheduling TopoBroadcast txon" << endl;
			      return EXIT_FAILURE;
		       }
		    }
	    }
   }
   
    return EXIT_SUCCESS;
}


int
SyncManager::ScheduleV2xGeobroadcastMessages(
    ics_types::stationID_t sender, int appHandlerId, int appMessageId,
    int frequency, int payloadLength, float msgRegenerationTime,
    unsigned char appMessageType, unsigned char commProfile, unsigned char preferredRATs,
    std::vector<Area2D*> areas, unsigned int msgLifetime)
{

    vector<RATID>* rats = m_facilitiesManager->getStationActiveRATs(sender);
    std::vector<std::string> techList;
    if (rats->size() == 0) {
        cout << "[ScheduleResult] getStationActiveRATs returned 0 size" << endl;
        return EXIT_FAILURE;
    }
    else if (sender == 0) {
    	// TMC node does not hold any techno interface
    	// it only fills the technoList vector for later processing at the techno selector.
#ifdef LOG_ON
    	stringstream log;
    	log << "[ScheduleResult] TMC node scheduled to send geobroadcast msg";
    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

    	if ((preferredRATs & 0x01)==0x01)
    	    techList.push_back("WaveVehicle");

    	if ((preferredRATs & 0x02)==0x02)
    	    techList.push_back("UmtsVehicle");

    	if ((preferredRATs & 0x04)==0x04)
    	    techList.push_back("WimaxVehicle");

    	if ((preferredRATs & 0x08)==0x08)
    		techList.push_back("DvbhVehicle");

    	if (((preferredRATs & 0x08)!=0x08) && ((preferredRATs & 0x04)!=0x04) && ((preferredRATs & 0x02)!=0x02) && ((preferredRATs & 0x01)!=0x01) ) {
    		// must return error as none of the preferred technologies are supported
#ifdef LOG_ON
    		stringstream log;
    		log << "ScheduleV2xGeobroadcastMessages() Geobroadcast preferred technology not supported";
    		IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
    		return EXIT_FAILURE;
    	}

    }
    else {
        bool nodeWithWave = false;
        for (vector<RATID>::iterator it = rats->begin(); it != rats->end(); it++) {
            if ((*it) == WAVE) {
                nodeWithWave = true;
                //ADD by Florent Kaisser, 05/25/16
                //techList can not be null : add WaveVehicule to the list
                techList.push_back("WaveVehicle");
                ////
                break;
            }
        }

        if (!nodeWithWave) {
            cout << "[ERROR] ScheduleV2xGeobroadcastMessages() There is no match for the type of RAT" << endl;
            return EXIT_FAILURE;
        }

        if (nodeWithWave && ((preferredRATs & 0x01) != 0x01)) {
            cout << "[ERROR] ScheduleV2xGeobroadcastMessages() Geobroadcast can be performed only with WAVE, "
                 "and this technology was not set in the preferred technologies in the application message header." << endl;
            return EXIT_FAILURE;
        }
    }

    // Convert sender ID to string in a vector
    std::vector<std::string> sendersId;
    int senderNs3Id = GetNodeByIcsId(sender)->m_nsId;
    sendersId.push_back(utils::Conversion::int2String(senderNs3Id));

    string serviceId = "NULL";
    for (vector<ApplicationHandler*>::iterator it = m_applicationHandlerCollection->begin(); it != m_applicationHandlerCollection->end(); ++it) {
        if ((*it)->m_id == appHandlerId) {
            serviceId = (*it)->m_serviceId.geoBroadcastServiceId;
            break;
        }
    }

    if (serviceId == "NULL") {
        cout
            << "iCS --> [ERROR] ScheduleV2xGeobroadcastMessages() Service ID is not correct"
            << endl;
        return EXIT_FAILURE;
    }

    // 1. GENERATE PAYLOAD + ACTIONID IN THE FACILITIES

    ScheduledGeobroadcastMessageData messageData;
    bool exists = false;

    // Stores one Action ID per message in the timestep (i.e. frequency)
    for (int i = 0; i < frequency; i++) {
        messageData.senderNs3ID = senderNs3Id;
        messageData.senderIcsID = sender;
        messageData.messageType = GEOBROADCAST;
        messageData.timeStep = m_simStep;
        messageData.sequenceNumber = i;
        messageData.appMessageId = appMessageId;

        // Checks if the message has already been scheduled
        if (ScheduledGeobroadcastMessageTable.size() > 0) {
            for (vector<ScheduledGeobroadcastMessageData>::iterator it = ScheduledGeobroadcastMessageTable.begin(); it != ScheduledGeobroadcastMessageTable.end(); it++) {
                if ((messageData.senderIcsID == (*it).senderIcsID) && (messageData.timeStep == (*it).timeStep) && (messageData.sequenceNumber == (*it).sequenceNumber)) {
                    exists = true;
                    break;
                } else {
                    exists = false;
                }
            }
        }
        else {
            exists = false;
        }

        if (!exists) {
            //Call facilities
            TApplicationMessageDestination appDest;
            appDest.dest_areas = areas;
            messageData.actionID = m_facilitiesManager->createApplicationMessagePayload(
                                       messageData.senderIcsID,
                                       messageData.messageType,
                                       appDest,
                                       preferredRATs,
                                       payloadLength,
                                       commProfile,
                                       appHandlerId,
                                       appMessageType,
                                       appMessageId);
            m_v2xMessageTracker->InsertGeobroadcastRow(ScheduledGeobroadcastMessageTable, messageData);

            // 2. SCHEDULE MESSAGE IN ns-3
            ITetrisNode* senderNode = GetNodeByIcsId(sender);
            ITetrisNode& senderReference = *senderNode;
            const type_info& typeinfoSender = typeid(senderReference);

            /////////////////////////////////////////////////////
            // NOTE by Florent KAISSER 08/21/2016
            //
            // Here, the area is convert to a circle !!
            // Then, the geobroadcast doesn't work with 
            // a other type of shape, all area is a circle.
            //
            /////////////////////////////////////////////////////
            CircularGeoAddress destination;
            Area2D* area2D = m_facilitiesManager->getWholeArea(areas);
            Circle circle = m_facilitiesManager->getCircleFromArea(area2D);
            delete area2D;
            double x = (double)circle.getCenter().x();
            double y = (double)circle.getCenter().y();

            vector <double> degreesCoordinates = LocToGeoConvert(x,y, 0, m_facilitiesManager->getLat0(), m_facilitiesManager->getLon0(), m_facilitiesManager->getAlt0());
            
            /////////////////////////////////////////////////////
            // MODIFICATION by Florent KAISSER 08/24/2016
            //        
            // degreesCoordinates is a vector of double but
            // destination.lat and destination.lon are int32.
            // BIG ISSUE ! The double multiplied by 10000000 
            // like in ns-3 source itetris-pos-helper.cc
            //   
            //destination.lat = degreesCoordinates.at(0);
            //destination.lon = degreesCoordinates.at(1);
            destination.lat = (uint32_t)(degreesCoordinates.at(0) * 10000000.0);
            destination.lon = (uint32_t)(degreesCoordinates.at(1) * 10000000.0);            
            destination.areaSize = (uint32_t)circle.getArea();

            if (sender == 0) {
            	if (!SyncManager::m_wirelessComSimCommunicator->CommandStartMWTxon(sendersId, serviceId, commProfile, techList, destination, (float)frequency, (uint32_t)payloadLength, (double)msgRegenerationTime, (uint8_t)msgLifetime)) {
            		cout << "[ERROR] ScheduleResult() Error while scheduling MWtxon" << endl;
            		return EXIT_FAILURE;
            	}
            }
            else {
            	if (!SyncManager::m_wirelessComSimCommunicator->StartGeobroadcastTxon(sendersId, serviceId, commProfile, techList, destination, frequency, payloadLength, msgRegenerationTime, msgLifetime)) {
            		cout << "[ERROR] ScheduleResult() Error while scheduling GeoBroadcast txon" << endl;
            		return EXIT_FAILURE;
            	}
            }
        }
    }
    
    return EXIT_SUCCESS;
}

int
SyncManager::RefreshScheduledCamMessageTable()
{
    if (ScheduledCamMessageTable.size() > 0) {
        vector<ScheduledCamMessageData>::iterator messageIterator = ScheduledCamMessageTable.begin();
        while (messageIterator != ScheduledCamMessageTable.end()) {
            if (m_simStep >= ((*messageIterator).timeStep + ITetrisSimulationConfig::m_scheduleMessageCleanUp)) {
                messageIterator = ScheduledCamMessageTable.erase(messageIterator);
            } else {
                ++messageIterator;
            }
        }
    } else {
#ifdef LOG_ON
        stringstream log;
        log
        << "[WARNING] RefreshScheduledCamMessageTable() CAM scheduled table is empty";
        IcsLog::Log((log.str()).c_str());
#endif
    }

    return EXIT_SUCCESS;
}

int
SyncManager::RefreshScheduledUnicastMessageTable()
{
    if (ScheduledUnicastMessageTable.size() > 0) {
        vector<ScheduledUnicastMessageData>::iterator messageIterator = ScheduledUnicastMessageTable.begin();
        while (messageIterator != ScheduledUnicastMessageTable.end()) {
            if (m_simStep >= ((*messageIterator).timeStep + ITetrisSimulationConfig::m_scheduleMessageCleanUp)) {
                messageIterator = ScheduledUnicastMessageTable.erase(messageIterator);
            } else {
                ++messageIterator;
            }
        }
    } else {
#ifdef LOG_ON
        stringstream log;
        log
        << "[RefreshScheduledUnicastMessageTable] [WARNING] UNICAST scheduled table is empty";
        IcsLog::Log((log.str()).c_str());
#endif
    }
    return EXIT_SUCCESS;
}

int
SyncManager::RefreshScheduledGeobroadcastMessageTable()
{
    if (ScheduledGeobroadcastMessageTable.size() > 0) {
        vector<ScheduledGeobroadcastMessageData>::iterator messageIterator = ScheduledGeobroadcastMessageTable.begin();
        while (messageIterator != ScheduledGeobroadcastMessageTable.end()) {
            if (m_simStep >= ((*messageIterator).timeStep + ITetrisSimulationConfig::m_scheduleMessageCleanUp)) {
                messageIterator = ScheduledGeobroadcastMessageTable.erase(messageIterator);
            } else {
                ++messageIterator;
            }
        }
    } else {
#ifdef LOG_ON
        stringstream log;
        log << "RefreshScheduledGeobroadcastMessageTable() GEOBROADCAST scheduled table is empty";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
    }
    return EXIT_SUCCESS;
}


int
SyncManager::RefreshScheduledTopobroadcastMessageTable()
{
    if (ScheduledTopobroadcastMessageTable.size() > 0) {
        vector<ScheduledTopobroadcastMessageData>::iterator messageIterator = ScheduledTopobroadcastMessageTable.begin();
        while (messageIterator != ScheduledTopobroadcastMessageTable.end()) {
            if (m_simStep >= ((*messageIterator).timeStep + ITetrisSimulationConfig::m_scheduleMessageCleanUp)) {
                messageIterator = ScheduledTopobroadcastMessageTable.erase(messageIterator);
            } else {
                ++messageIterator;
            }
        }
    } else {
        stringstream log;
        log << "RefreshScheduledTopobroadcastMessageTable() TOPOBROADCAST scheduled table is empty";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    }
    return EXIT_SUCCESS;
}

int
SyncManager::ProcessGeobroadcastMessages(ScheduledGeobroadcastMessageData message, stationID_t receiverID)
{



    // reference to the app that triggered the geobroadcasting
    int appType;
    int messageId;
    actionID_t actionID; // reference of the message for the APP_MSG_RECEIVE
    // Check if the message was scheduled
    if (ScheduledGeobroadcastMessageTable.size() > 0) {
        // Loop the scheduled message table to check if the received message had been scheduled
        for (vector<ScheduledGeobroadcastMessageData>::iterator messageIterator = ScheduledGeobroadcastMessageTable.begin(); messageIterator != ScheduledGeobroadcastMessageTable.end(); messageIterator++) {
            if (m_v2xMessageTracker->CompareGeobroadcastRows(message, (*messageIterator)) == true) {
                //message.actionID = (*messageIterator).actionID;
            	actionID = (*messageIterator).actionID;
            	// Change the status of the message (marks as received but more nodes could receive it)
                if ((*messageIterator).received == false) {
                    (*messageIterator).received = true;
                }
 
 
                ////////////////////////////////////
                // CHANGED by Florent KAISSER, 06/06/2016
                // Fix bug
                // Get also the application type. No confound
                // message Id and application type Id !
                // We must use getReceivedMessagePayload to
                //   retreive the type in stored payload.
                //                
                messageId = (*messageIterator).appMessageId;
                appType = m_facilitiesManager->getApplicationMessageType(actionID);
                ///////////////////////////////////
            }
        }
    } else {
        IcsLog::LogLevel("ProcessGeobroadcastMessages() There isn't any scheduled GEOBROADCAST message for the received message",kLogLevelWarning);
        return EXIT_SUCCESS;
    }
    
    ///////////////////
    // COMMENTED by Florent KAISSER, 06/03/2016
    // Fix bug
    //    use ProcessAppMessages    
    /*
    // 1. Get reference to receiver
    ITetrisNode* receiver = GetNodeByNs3Id(receiverID);

    // 2. Check whether the receiver runs app that has subscription able to process the message

    Subscription* subscription = NULL;
    for (vector<Subscription*>::iterator it = receiver->m_subscriptionCollection->begin(); it != receiver->m_subscriptionCollection->end(); ++it) {
        if ((*it)->m_appId == appId) {
            subscription = *it;
            break;
        }
    }
    if (subscription == NULL) {
        stringstream log;
        log << "ProcessGeobroadcastMessages() The message has no subscription to be processed.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
        return EXIT_SUCCESS;
    }
    

    // 3. Process subscription
    
    
    */

    ScheduledAPPMessageData appMessage;
    ///////////////////
    // ADDED by Florent KAISSER, 05/30/2016
    // Fix bug
    // appMessage.receiverNs3ID must be initialized !
    appMessage.receiverNs3ID = receiverID;
    //////////////////
    appMessage.senderNs3ID = message.senderNs3ID;
    appMessage.messageType = message.messageType;
    appMessage.timeStep = message.timeStep;
    appMessage.sequenceNumber = message.sequenceNumber;
    appMessage.senderIcsID = message.senderIcsID;
    appMessage.receiverIcsID = message.receiverIcsID;
    appMessage.actionID = actionID;
    appMessage.appMessageId = messageId;
    appMessage.appMessageType = appType;


   if (ProcessAppMessages(appMessage) == EXIT_FAILURE) {
#ifdef LOG_ON
      stringstream log;
      log << "[ERROR] GetDataFromNs3() Processing AppMessage in ProcessGeoBroadcastMessages() failed.";
      IcsLog::Log((log.str()).c_str());
#endif
  
      return EXIT_FAILURE;
   }
  
  ///////////////////////////////////////


  return EXIT_SUCCESS;
}

int
SyncManager::ProcessAppMessages(ScheduledAPPMessageData appMessage)
{
  // checking for APP msg subscriptions
  // 1. Get reference to receiver
  ITetrisNode* receiver = GetNodeByNs3Id(appMessage.receiverNs3ID);

  // 2. Check whether the receiver runs app that has subscription able to process the message
  Subscription* subscription = NULL;
  for (vector<Subscription*>::iterator it = receiver->m_subscriptionCollection->begin(); it != receiver->m_subscriptionCollection->end(); ++it) {
    //CHANGED by Florent KAISSER, 06/06/2016, use appType to compare the application type id
    if ((*it)!=NULL && (*it)->m_appId == appMessage.appMessageType) {
      subscription = *it;
      break;
    }
  }
  if (subscription == NULL) {
    stringstream log;
    log << "ProcessAppMessages() The message has no subscription to be processed.";
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
    return EXIT_SUCCESS;
  }

  // 3. Process subscription
  Subscription& reference = *subscription;
  const type_info& typeofSubscription = typeid(reference);
  if (typeofSubscription == typeid(SubsAppMessageReceive)) {
    SubsAppMessageReceive* appMsgReceive = static_cast<SubsAppMessageReceive*>(subscription);
    if (appMsgReceive->ProcessReceivedAppMessage(appMessage, GetAddress()) == EXIT_FAILURE) {
      IcsLog::LogLevel("ProcessGeobroadcastMessages() Error processing App message.", kLogLevelError);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

}//syncmanager
