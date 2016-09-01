/****************************************************************************/
/// @file    app-result-void.cpp
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
#include <cstdlib>

#include "foreign/tcpip/storage.h"
#include "app-result-void.h"
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
ResultVoid::ResultVoid(stationID_t owner, int appHandlerId)
{
    m_ownerStation = owner;
    m_applicationHandlerId = appHandlerId;
}

bool
ResultVoid::ProcessResult(Storage& storage)
{
#ifdef LOG_ON
    IcsLog::LogLevel("ProcessResult() The application does not return any result and no data is proccessed.",kLogLevelInfo);
#endif
    return true;
}

int
ResultVoid::ApplyResult(SyncManager* syncManager, ApplicationHandler* appHandler)
{
#ifdef LOG_ON
    IcsLog::LogLevel("ApplyResult() The application does not return any result and no data is applied.",kLogLevelInfo);
#endif
    return EXIT_SUCCESS;
}

vector<pair<int,stationID_t> >
ResultVoid::GetReceivedMessages()
{
    vector<pair<int,stationID_t> > auxiliar;
#ifdef LOG_ON
    IcsLog::LogLevel("GetReceivedMessages() The application does not return any result and no messages are not received.",kLogLevelInfo);
#endif
    return auxiliar;
}

///////////////////////////////////////////////////////////////////////////////////////

/*bool ResultVoid::push(unsigned char CMD_TYPE, std::string TAG, std::vector<unsigned char> result){
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

std::vector<unsigned char> ResultVoid::pull(unsigned char CMD_TYPE, std::string TAG){
	containerMap::iterator it = m_resultMap.find(CMD_TYPE);
	inMap resultsMap = it->second;

	inMap::iterator it2 = resultsMap.find(TAG);
	return it2->second;
}*/

}

