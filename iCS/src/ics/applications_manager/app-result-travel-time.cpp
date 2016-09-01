/****************************************************************************/
/// @file    app-result-travel-time.cpp
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

#include <iostream>

#include "foreign/tcpip/storage.h"
#include "app-result-travel-time.h"
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
ResultTravelTime::ResultTravelTime(stationID_t owner, int appHandlerId)
{
    m_ownerStation = owner;
    m_applicationHandlerId = appHandlerId;
    m_containerIsEmpty = false;
    m_travelTime.m_travelTime = -1;
    m_travelTime.m_destinationStationId = 0;
    m_travelTime.m_messageStatus = kMissed;
    m_travelTime.m_messageId = -1;
    m_travelTime.m_frequency = -1;
    m_travelTime.m_payloadLength = -1;
    m_travelTime.m_msgRegenerationTime = -1;
}

bool
ResultTravelTime::ProcessResult(Storage& storage)
{
    if (storage.size() == 0) {
        cout << "[ERROR] ProcessResult() There is no data from the application to be processed." << endl;
        return false;
    }

    try {
        cout << "ResultTravelTime::ProcessResult() @0" << endl;
        // read the command length
        storage.readUnsignedByte();
        cout << "ResultTravelTime::ProcessResult() @1" << endl;
        // read the command id
        storage.readUnsignedByte();
        cout << "ResultTravelTime::ProcessResult() @02" << endl;
        // reads if the application execution status
        int hasResult = storage.readUnsignedByte();
        cout << "ResultTravelTime::ProcessResult() @3" << endl;
        if (hasResult == APP_RESULT_ON) {
            cout << "ResultTravelTime::ProcessResult() @4" << endl;
            // read the result
            m_containerIsEmpty = false;
            m_travelTime.m_destinationStationId = storage.readInt();
            cout << "ResultTravelTime::ProcessResult() @41" << endl;

            cout << "ResultTravelTime::ProcessResult() Destination ID is: " << m_travelTime.m_destinationStationId<< endl;
            stringstream log;
            log << "ResultTravelTime::ProcessResult() Destination ID is: " << m_travelTime.m_destinationStationId;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            m_travelTime.m_travelTime = storage.readFloat();
            m_travelTime.m_messageId = storage.readInt();
            m_travelTime.m_frequency = storage.readInt();
            m_travelTime.m_payloadLength = storage.readInt();
            m_travelTime.m_msgRegenerationTime = storage.readFloat();
            m_travelTime.m_messageStatus = kToBeScheduled; // Result from app is always to be scheduled

            //stringstream log;
            log.str("");
            log << "ResultTravelTime::ProcessResult() The application executed in the station: " << m_ownerStation;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        } else if (hasResult == APP_RESULT_OFF) {
            m_containerIsEmpty = true;

            stringstream log;
            log << "ResultTravelTime::ProcessResult() The application did NOT execute in the station: " << m_ownerStation;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        } else {
            IcsLog::LogLevel("ProcessResult() The status of the result is unknown", kLogLevelWarning);
        }
    } catch (std::invalid_argument e) {
        cout << "[ERROR] ProcessResult() an exception was thrown while reading result state message" << endl;
        return false;
    }

    return true;
}

int
ResultTravelTime::ApplyResult(SyncManager* syncManager, ApplicationHandler* appHandler)
{
    switch (m_travelTime.m_messageStatus) {
    case kToBeScheduled: {
        ITetrisNode* destinationNode =  syncManager->GetNodeByIcsId(m_travelTime.m_destinationStationId);
        if (destinationNode == NULL) {  // check the possibility that the vehicle left the simulation
            stringstream log;
            log << "ResultTravelTime::ApplyResult() The iCS ID node does not longer exist: " << m_travelTime.m_destinationStationId;
            IcsLog::LogLevel((log.str()).c_str(),kLogLevelInfo);
            return EXIT_SUCCESS;
        }
        unsigned char appMsgType = 0x01;        // TODO: Application message type hardcoded.
        unsigned char commProfile = 0xFF;       // TODO: Communication profile for the technology selection hardcoded.
        unsigned char preferredRATs = 0xFF;     // TODO: Preferred RATs for the transmission of the message hardcoded.
        unsigned int msgLifetime = 2;           // TODO: Message lifetime hardcoded.
        if (syncManager->ScheduleV2xUnicastMessages(
                    m_ownerStation,
                    m_applicationHandlerId,
                    destinationNode,
                    m_travelTime.m_messageId,
                    appMsgType,
                    m_travelTime.m_frequency,
                    m_travelTime.m_payloadLength,
                    m_travelTime.m_msgRegenerationTime,
                    commProfile,
                    preferredRATs,
                    msgLifetime
                ) == EXIT_FAILURE) {
            m_travelTime.m_messageStatus = kScheduled;
            return EXIT_FAILURE;
        }
        break;
    }
    case kScheduled: {
        IcsLog::LogLevel("ApplyResult() Message is already scheduled", kLogLevelInfo);
        return EXIT_SUCCESS;
        break;
    }
    case kArrived: {
        IcsLog::LogLevel("ApplyResult()", kLogLevelInfo);
        return EXIT_SUCCESS;
        break;
    }
    default: {
        IcsLog::LogLevel("ApplyResult() Status is not defined for the message", kLogLevelWarning);
        return EXIT_SUCCESS;
    }
    }
}

int
ResultTravelTime::CheckMessage(int appMessageId)
{
    if (m_travelTime.m_messageId == appMessageId) {
        m_travelTime.m_messageStatus = kArrived;
        return EXIT_SUCCESS;
    }

#ifdef LOG_ON
    IcsLog::LogLevel("CheckMessage() Message was not schedule by this app result", kLogLevelInfo);
#endif
    return EXIT_SUCCESS;
}

vector<pair<int,stationID_t> >
ResultTravelTime::GetReceivedMessages()
{
    vector<pair<int,stationID_t> > messages;

    if (m_travelTime.m_messageStatus == kArrived) {
        pair<int,stationID_t> message = make_pair(m_travelTime.m_messageId, m_travelTime.m_destinationStationId);
        messages.push_back(message);

        stringstream log;
        log << "[INFO] GetReceivedMessage() Message Id: " << m_travelTime.m_messageId << " checked as arrived.";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);


    }
    return messages;
}

/*bool ResultTravelTime::push(unsigned char CMD_TYPE, std::string TAG, std::vector<unsigned char> result){
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

std::vector<unsigned char> ResultTravelTime::pull(unsigned char CMD_TYPE, std::string TAG){
	containerMap::iterator it = m_resultMap.find(CMD_TYPE);
	inMap resultsMap = it->second;

	inMap::iterator it2 = resultsMap.find(TAG);
	return it2->second;
}*/

/*template <typename genericType> bool ResultTravelTime::push(unsigned char CMD_TYPE, genericType TAG, genericType result){
	template <typename genericType> map<genericType,genericType> >* resultsMap = m_resultMap.get(CMD_TYPE);
	resultsMap->add(TAG,result);
	//m_resultMap.add(CMD_TYPE, resultsMap);
	return true;
}

template <typename genericType> genericType ResultTravelTime::pull(unsigned char CMD_TYPE, genericType TAG){
	template <typename genericType> map<genericType,genericType> > resultsMap = m_resultMap.get(CMD_TYPE);
	return resultsMap.aget(TAG);
}*/

}
