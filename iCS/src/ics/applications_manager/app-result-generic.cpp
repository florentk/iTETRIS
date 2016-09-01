/****************************************************************************/
/// @file    app-result-generic.cpp
/// @author  Jerome Haerri
/// @date 17 juin 2012
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
#include <cstdlib>
#include <sstream>

#include "foreign/tcpip/storage.h"
#include "app-result-generic.h"
#include "../traffic_sim_communicator/traffic-simulator-communicator.h"
#include "app-commands-subscriptions-constants.h"
#include "../../utils/ics/log/ics-log.h"

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace tcpip;
using namespace std;
using namespace ics_types;

namespace ics
{

// ===========================================================================
// member method definitions
// ===========================================================================
ResultGeneric::ResultGeneric(stationID_t owner, int appHandlerId)
{
    m_ownerStation = owner;
    m_applicationHandlerId = appHandlerId;
}

bool
ResultGeneric::ProcessResult(Storage& storage)
{
#ifdef LOG_ON
    IcsLog::LogLevel("ProcessResult() The application optionally returns DATA to be stored and exchanged between APP.",kLogLevelInfo);
#endif


    if (storage.size() == 0) {
#ifdef LOG_ON
    	IcsLog::LogLevel("iCS --> [ResultGeneric] ProcessResult() There is no data from the application to be processed.",kLogLevelInfo);
#endif
    	return false;
    }

    try {

       // read the command length
       storage.readInt();
       // read the command id
       storage.readUnsignedByte();
       // reads if the application execution status
       int hasResult = storage.readUnsignedByte();
       if (hasResult == APP_RESULT_ON) {
         // read the result
         m_containerIsEmpty = false;
    	 int numOfResults = storage.readInt();
    	 for (int i = 0; i < numOfResults ; i++) {
    		std::string tag = storage.readString();//add number to the stream
    		int local_length = storage.readUnsignedByte();
    		vector<unsigned char> packet;//(local_length);
    		for (unsigned int i = 0; i < local_length; i++) {
    			packet.push_back(storage.readChar());
    		}

#ifdef LOG_ON
    	    stringstream log;
    	    log << "ResultTravelTime::ProcessResult() for TAG: " << tag;
    	    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

    	    push(SPEED,tag,packet);
    	 }


#ifdef LOG_ON
    	 stringstream log;
         log << "ResultGeneric::ProcessResult() The application executed in the station: " << m_ownerStation;
         IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

       }
       else if (hasResult == APP_RESULT_OFF) {
          m_containerIsEmpty = true;

#ifdef LOG_ON
          stringstream log;
          log << "ResultGeneric::ProcessResult() The application did NOT execute in the station: " << m_ownerStation;
          IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

       }
       else {
          IcsLog::LogLevel("ResultGeneric::ProcessResult() The status of the result is unknown", kLogLevelWarning);
       }
    }
    catch (std::invalid_argument e) {
         cout << "[ERROR] ResultGeneric::ProcessResult() an exception was thrown while reading result state message" << endl;
         return false;
    }
    return true;
}

int
ResultGeneric::ApplyResult(SyncManager* syncManager, ApplicationHandler* appHandler)
{
#ifdef LOG_ON
    IcsLog::LogLevel("ApplyResult() The application does not return any result and no data is applied.",kLogLevelInfo);
#endif
    return EXIT_SUCCESS;
}

vector<pair<int,stationID_t> >
ResultGeneric::GetReceivedMessages()
{
    vector<pair<int,stationID_t> > auxiliar;
#ifdef LOG_ON
    IcsLog::LogLevel("GetReceivedMessages() The application does not return any result and no messages are not received.",kLogLevelInfo);
#endif
    return auxiliar;
}

bool ResultGeneric::push(int CMD_TYPE, std::string TAG, vector<unsigned char> result){
	containerMap::iterator it = m_resultMap.find(CMD_TYPE);
	 if( it != m_resultMap.end() ) {
		 inMap resultsMap = it->second;

		 inMap::iterator it2 = resultsMap.find(TAG);
		 if (it2 !=resultsMap.end()) {
			 resultsMap.erase(TAG);
			 resultsMap.insert(make_pair(TAG,result));
		 }
		 else {
			 resultsMap.insert(make_pair(TAG,result));
		 }
	 }
	 else {
		 inMap resultsMap;

		 resultsMap.insert(make_pair(TAG,result));
		 m_resultMap.insert(make_pair(CMD_TYPE,resultsMap));
	 }

	 return EXIT_SUCCESS;
}

std::vector<unsigned char> ResultGeneric::pull(int CMD_TYPE, std::string TAG){

	containerMap::iterator it = m_resultMap.find(CMD_TYPE);
	if(it != m_resultMap.end()) {
	  inMap resultsMap = it->second;

	  inMap::iterator it2 = resultsMap.find(TAG);
	  if(it2 != resultsMap.end()) {
	      return it2->second;
	  }
	  else {
		  std::vector<unsigned char> empty;
		  return empty;
	  }
	}
    else {
    	std::vector<unsigned char> empty;
        return empty;
	}
}

}

