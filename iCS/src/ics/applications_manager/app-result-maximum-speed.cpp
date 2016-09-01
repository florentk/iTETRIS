/****************************************************************************/
/// @file    app-result-maximum-speed.cpp
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

#include <iostream>

#include "foreign/tcpip/storage.h"
#include "app-result-maximum-speed.h"
#include "../traffic_sim_communicator/traffic-simulator-communicator.h"
#include "app-commands-subscriptions-constants.h"
#include "../itetris-node.h"
#include "../sync-manager.h"
#include "../ics.h"
#include "../../utils/ics/log/ics-log.h"
#include "../utilities.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace tcpip;
using namespace std;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
ResultSetMaximumSpeed::ResultSetMaximumSpeed(stationID_t owner, int appHandlerId)
{
    m_ownerStation = owner;
    m_applicationHandlerId = appHandlerId;
}

bool
ResultSetMaximumSpeed::ProcessResult(Storage& storage)
{
    if (storage.size() == 0) {
        cout << "iCS --> #Error: There is no data from the application to be processed." << endl;
        return false;
    }

    try {
    	cout << "iCS --> in Result Container..." << endl;
        // read the command length
        storage.readInt();        
	//storage.readUnsignedByte();
        // read the command id
        storage.readUnsignedByte();
        // reads if the application execution status
        int hasResult = storage.readUnsignedByte();
        cout << "iCS --> result type..." << hasResult << " " << endl;
        if (hasResult == APP_RESULT_ON) {
            // read the result
        	cout << "iCS --> getting results..." << endl;
            m_containerIsEmpty = false;
            int numOfResults = storage.readInt();
	    cout << "iCS --> [ResultSetMaximumSpeed::ProcessResult] " << numOfResults << " results present" << endl;
            for (int i = 0; i < numOfResults ; i++) {
                TSetMaximumSpeed result;
                result.m_nodeId = storage.readInt();
                result.m_maximumSpeed = storage.readFloat();
              //  cout << "iCS --> getting new Speed..." << result.m_maximumSpeed << " " << endl;
                result.m_messageId = storage.readInt();
                int status = storage.readInt();
                switch (status) {
                case APP_RESULT_TO_SCHEDULE: {
                    result.m_frequency = storage.readInt();
                    result.m_payloadLength = storage.readInt();
                    result.m_msgRegenerationTime = storage.readFloat();
                    result.m_messageStatus = kToBeScheduled;

                    stringstream log;
                    log << "[INFO] ProcessResult()  Message Id: " <<result.m_messageId << " for Destination Node " <<result.m_nodeId << " with advised speed " << result.m_maximumSpeed << " received from App to be SCHEDULED";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
                    cout << "[INFO] ProcessResult()  Message Id: " <<result.m_messageId << " for Destination Node " <<result.m_nodeId << " with advised speed " << result.m_maximumSpeed << " received from App to be SCHEDULED"<< endl;
                   
		    m_maximumSpeeds.push_back(result); // The message is new so it has to be included in the collection
                    break;
                }
                case APP_RESULT_TO_APPLY: {
                    result.m_frequency = storage.readInt();
                    result.m_payloadLength = storage.readInt();
                    result.m_msgRegenerationTime = storage.readFloat();
                    // Look for the message in current collection and change status
                    bool found = false;
                    for (vector<TSetMaximumSpeed>::iterator it=m_maximumSpeeds.begin() ; it<m_maximumSpeeds.end() ; it++) {
                        if ((*it).m_messageId == result.m_messageId) {
                            (*it).m_messageStatus = kToBeApplied;
                            found = true;
                        }
                    }

                    stringstream log;
                    log << "[INFO] ProcessResult()  Message Id: " <<result.m_messageId << " for Destination Node " <<result.m_nodeId << " with advised speed " << result.m_maximumSpeed << " received from App to be APPLIED";
                    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
                    cout << "[INFO] ProcessResult()  Message Id: " <<result.m_messageId << " for Destination Node " <<result.m_nodeId << " with advised speed " << result.m_maximumSpeed << " received from App to be APPLIED"<< endl;

                    if (!found) {

                        stringstream log;
                        log << "[WARNING] ProcessResult() Potential pitfall, message not found.";
                        IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);

                    }
                    break;
                }
                case APP_RESULT_TO_DISCARD: {
                    result.m_frequency = storage.readInt();
                    result.m_payloadLength = storage.readInt();
                    result.m_msgRegenerationTime = storage.readFloat();
                    // Look for the message in current collection and change status
                    bool found = false;
                    for (vector<TSetMaximumSpeed>::iterator it=m_maximumSpeeds.begin() ; it<m_maximumSpeeds.end() ; it++) {
                        TSetMaximumSpeed maxSpeed = *it;
                        if ((*it).m_messageId == result.m_messageId) {
                            (*it).m_messageStatus = kToBeDiscarded;
                            found = true;
                        }
                    }
                    if (!found) {
                        utils::Conversion::Wait("iCS--> [WARNING] ProcessResult. Potential pitfall, message not found.");
                    }
                    break;
                }
                default:
                    cout << "iCS --> [ERROR] ProcessResult() Application returned result status does not exist: " << status << endl;
                    return false;
                }
            }
        } else {
            m_containerIsEmpty = true;
        }

    } catch (std::invalid_argument e) {
        cout << "iCS --> #Error: an exception was thrown while reading result state message" << endl;
        return false;
    }

    return true;
}

float
ResultSetMaximumSpeed::GetMaximumSpeed(int nodeId)
{
    vector<TSetMaximumSpeed>::iterator it;
    for (it=m_maximumSpeeds.begin() ; it<m_maximumSpeeds.end() ; it++) {
        TSetMaximumSpeed s = *it;
        if (s.m_nodeId == nodeId) {
            return s.m_maximumSpeed;
        }
    }
    return -1;
}

int
ResultSetMaximumSpeed::ApplyResult(SyncManager* syncManager, ApplicationHandler* appHandler)
{

    vector<TSetMaximumSpeed>::iterator it=m_maximumSpeeds.begin();
    while (it != m_maximumSpeeds.end()) {
        TSetMaximumSpeed speed = *it;
        ITetrisNode* node = syncManager->GetNodeByIcsId(speed.m_nodeId);
        if (node == NULL) {

            stringstream log;
            log << "[WARNING] ApplyResult() Node was not retrieved with the iCS ID: " << speed.m_nodeId << " Left the simulation?";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);

            ++it;

        } else { // Check the status of the message
            switch (speed.m_messageStatus) {
            case kToBeScheduled: { // Message has to be scheduled in ns-3
                unsigned char appMsgType = 0x01;        // TODO: Application message type hardcoded.
                unsigned char commProfile = 0x00;       // TODO: Communication profile hardcoded.
                unsigned char preferredRATs = 0xFF;     // TODO: Preferred RATs hardcoded.
                unsigned int msgLifetime = 2;           // TODO: Application message life time hardcoded.
                if (syncManager->ScheduleV2xUnicastMessages(
                            m_ownerStation,
                            m_applicationHandlerId ,
                            node,
                            speed.m_messageId,
                            appMsgType,
                            speed.m_frequency,
                            speed.m_payloadLength,
                            speed.m_msgRegenerationTime,
                            commProfile,
                            preferredRATs,
                            msgLifetime
                        ) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                speed.m_messageStatus = kScheduled;
                ++it;
                break;
            }
            case kArrived: {
                cout << "iCS --> Message arrived. Would be notified to the app later on." << endl;
                ++it;
                break;

            }
            case kToBeApplied: { // Value has to be applied because App requested so.
           if (SyncManager::m_trafficSimCommunicator->CommandSetMaximumSpeed(*node, speed.m_maximumSpeed) == EXIT_FAILURE)
		{
		   stringstream log;
            	    log << "iCS -->  [ApplyResult] result speed "<< speed.m_maximumSpeed <<" NOT correctly applied in SUMO for Node Id: " <<node->m_icsId <<" ";
            	    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
		    cout <<   "iCS -->  [ApplyResult] result speed "<< speed.m_maximumSpeed <<" NOT correctly applied in SUMO for Node Id: " <<node->m_icsId <<" "<< endl;
		  
                    return EXIT_FAILURE;
		}
		else
		{
		    stringstream log;
            	    log << "iCS -->  [ApplyResult] result speed "<< speed.m_maximumSpeed <<" correctly applied in SUMO for Node Id: " <<node->m_icsId <<" ";
            	    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
		    cout <<   "iCS -->  [ApplyResult] result speed "<< speed.m_maximumSpeed <<" correctly applied in SUMO for Node Id: " <<node->m_icsId <<" "<< endl;
		}
                it = m_maximumSpeeds.erase(it);
                break;
            }
            case kToBeDiscarded: {
                it = m_maximumSpeeds.erase(it);
                break;
            }
            case kMissed: { // Unexpected behaviours
                ++it;
                break;
            }
            default : {
                cout << "iCS --> [ApplyResult] The message result status is undefined. Node iCS ID: " << speed.m_nodeId << " Status: " << speed.m_messageStatus << endl;
                return EXIT_FAILURE;
            }
            }
        }
    }

    return EXIT_SUCCESS;
}

int
ResultSetMaximumSpeed::CheckMessage(int appMessageId)
{
    // Loop the speeds for the different vehicles
    for (vector<TSetMaximumSpeed>::iterator it=m_maximumSpeeds.begin() ; it<m_maximumSpeeds.end() ; it++) {
        TSetMaximumSpeed speed = *it;
        if (speed.m_messageId == appMessageId) {
            (*it).m_messageStatus = kArrived;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

vector<pair<int,stationID_t> >
ResultSetMaximumSpeed::GetReceivedMessages()
{
    vector<pair<int,stationID_t> > messages;

    for (vector<TSetMaximumSpeed>::iterator it=m_maximumSpeeds.begin() ; it<m_maximumSpeeds.end() ; it++) {
        TSetMaximumSpeed speed = *it;
        if (speed.m_messageStatus == kArrived) {
          pair<int,stationID_t> message = make_pair(speed.m_messageId, speed.m_nodeId);
          messages.push_back(message);
        }

        stringstream log;
        log << "[INFO] GetReceivedMessage() Message Id: " << speed.m_messageId << " checked as arrived.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    }

    return messages;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/*bool ResultSetMaximumSpeed::push(unsigned char CMD_TYPE, std::string TAG, std::vector<unsigned char> result){
	containerMap::iterator it = m_resultMap.find(CMD_TYPE);
	 if( it != m_resultMap.end() ) {
		 inMap resultsMap = it->second;

		 resultsMap.insert(make_pair(TAG,result));
	 }
	 else {
		 inMap resultsMap;
		 resultsMap.insert(make_pair(TAG,result));
		 m_resultMap.insert(make_pair(CMD_TYPE,resultsMap));
	 }
	 return EXIT_SUCCESS;
}

std::vector<unsigned char> ResultSetMaximumSpeed::pull(unsigned char CMD_TYPE, std::string TAG){
	containerMap::iterator it = m_resultMap.find(CMD_TYPE);
	inMap resultsMap = it->second;

	inMap::iterator it2 = resultsMap.find(TAG);
	return it2->second;
}*/

}

