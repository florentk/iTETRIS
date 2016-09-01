/****************************************************************************/
/// @file    subs-app-message-receive.cpp
/// @author  Jerome Haerri (EURECOM)
/// @date    April 15th, 2012, revised June 10th, 2012
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
#include <cstring>
#include <algorithm>

#include "subs-app-message-receive.h"

#include "subs-app-message-send.h"
#include "subscriptions-type-constants.h"
#include "subscriptions-helper.h"
#include "../sync-manager.h"
#include "../../utils/ics/log/ics-log.h"
#include "../facilities/mapFacilities/road/Edge.h"
#include "../facilities/mapFacilities/road/Lane.h"
#include "../facilities/mapFacilities/road/Junction.h"
#include "../../utils/ics/geometric/Shapes.h"
#include "../itetris-node.h"

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
int
SubsAppMessageReceive::Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions)
{
    if (subscriptions == NULL) {
        return EXIT_FAILURE;
    }

    vector<Subscription*>::iterator it;
    for (it=subscriptions->begin() ; it<subscriptions->end(); it++) {
        Subscription* sub = *it;
        const type_info& typeinfo = typeid(sub);
        if (typeinfo == typeid(SubsAppMessageReceive*)) {
            SubsAppMessageReceive* subsAppMessageReceive = static_cast<SubsAppMessageReceive*>(sub);
            if (subsAppMessageReceive->m_nodeId == stationID) {
                delete subsAppMessageReceive;
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

SubsAppMessageReceive::SubsAppMessageReceive(
    int appId, ics_types::stationID_t stationId,
                                       SyncManager* syncManager,unsigned char* msg, int msgSize) : Subscription(stationId), m_msg(msg, msgSize)
{
    // Read parameters
    m_id = ++m_subscriptionCounter;

    m_name = "RECEIVE AN APPLICATION MESSAGE";

    m_appId = appId;

    m_appMsgType = 0x01;        // def value
    m_prefTechs = 0xFF;         // def value - means no preferred techno
    m_commProfile = 0x00;       // def value - means no communication profile provided
    m_msgLifetime = 1;          // def value
    m_appMsgLength = 0;         // def value
    m_msgSeqNo = 0;             // def value
    m_frequency = 1;            // def value
    m_msgRegenerationTime = 1;  // def value
    m_numHops = 1;				// def value

    m_appMsgReceived = false;

    // Registers message reception
    m_schedulingStatus = pull(syncManager);
}

SubsAppMessageReceive::~SubsAppMessageReceive() {}

int SubsAppMessageReceive::pull(SyncManager* syncManager)
{

	unsigned int index = 0;

    m_appMsgType = m_msg.readChar();                   // HEADER__APP_MSG_TYPE

    // TODO change the flag for other optional field (list of destination (for instance))
    unsigned char headerFlag = m_msg.readChar();       // HEADER__MSG_FLAG (necessary to know which optional fields of the header are present)

    if ((headerFlag & 0x01)==0x01)
        m_prefTechs = m_msg.readChar();                // HEADER__PREF_TECHS

    if ((headerFlag & 0x02)==0x02)
        m_commProfile = m_msg.readChar();              // HEADER__GEN_PROF

    // if source is not present, it means we listen to packets targeted at a particular node or to all nodes
    // TODO add a source LIST (not a single target)
    if ((headerFlag & 0x04)==0x04)
        m_destId.push_back(m_msg.readInt());                   // HEADER__SOURCE_ID

    if ((headerFlag & 0x08)==0x08)
        m_msgLifetime = m_msg.readInt();                  // HEADER__LIFE_TIME

    m_appMsgLength = m_msg.readShort();                   // HEADER__MSG_PAYLOAD_LENGTH

    unsigned char commMode = m_msg.readChar();        // Defines the communication mode and thus the way the extended header will be read.

    switch (commMode) {
    case EXT_HEADER_TYPE_TOPOBROADCAST: {
    	stringstream log;
    	log << "iCS --> SubAppMsgReceive: Communication mode: TOPOBROADCAST";
    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        m_commMode = TOPOBROADCAST;
        unsigned char flagHops = m_msg.readChar();
        if (flagHops == EXT_HEADER__VALUE_BLOCK_HOPS_No)
            m_numHops = m_msg.readShort();
        else {
        	stringstream log;
        	log << "iCS --> SubAppMsgReceive: Receiving TOPOBROADCAST message of any number of hops.";
        	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
        }

        break;
    }
    case EXT_HEADER_TYPE_UNICAST: {
    	stringstream log;
    	log << "iCS --> SubAppMsgReceive: Communication mode: UNICAST";
    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        m_commMode = UNICAST;
        unsigned char flagDest = m_msg.readChar();
        if (flagDest == EXT_HEADER__VALUE_BLOCK_IDs) {
        	unsigned int numDests = (unsigned int)m_msg.readChar();
        	for (unsigned int i = 0; i < numDests; i++)
        	  m_sourceId.push_back(m_msg.readInt()); // here, source ID either means one particular source node (for unicast) or any source node
        }
        else {
        	// if source ID is not mentioned (m_sourceId.size()==0), then listen to packets from any source
        	stringstream log;
   	    	log << "iCS --> SubAppMsgReceive: Receiving a UNICAST message from any source ID.";
  	    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
        }

        break;
    }
    case EXT_HEADER_TYPE_MULTICAST: {
    	stringstream log;
    	log << "iCS --> SubAppMsgReceive: Communication mode: MULTICAST";
    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        m_commMode = MULTICAST;
        unsigned char flagDest = m_msg.readChar();
        unsigned int numDests = (unsigned int) m_msg.readChar();
        if (flagDest == EXT_HEADER__VALUE_BLOCK_IDs)
            for (unsigned int i = 0; i < numDests; i++)
            	m_sourceId.push_back(m_msg.readInt());
        else {
        	// if source ID is not mentioned (m_sourceId.size()==0), then listen to packets from any source
        	stringstream log;
        	log << "iCS --> SubAppMsgReceive: Receiving a MULTICAST message from any source IDs.";
        	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
        }

        std::cerr << "MULTICAST transmission/reception not implemented in the iCS." << std::endl; // TODO: Implement the multicast transmission.
        return false;
    }
    case EXT_HEADER_TYPE_GEOBROADCAST: {
    	stringstream log;
    	log << "iCS --> SubAppMsgReceive: Communication mode: GEOBROADCAST";
    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        m_commMode = GEOBROADCAST;
        unsigned char flagAreas = m_msg.readChar();
        if (flagAreas == EXT_HEADER__VALUE_BLOCK_AREAs)
            m_areas = SubscriptionsHelper::readBlockAreas(&m_msg);
        else {
        	stringstream log;
        	log << "iCS --> SubAppMsgReceive: Receiving a GEOBROADCAST message from any dissemination area.";
        	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
        }

        break;
    }
    case EXT_HEADER_TYPE_GEOUNICAST: {
    	stringstream log;
    	log << "iCS --> SubAppMsgReceive: Communication mode: GEOUNICAST";
    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        m_commMode = GEOUNICAST;
        bool readArea = false;
        bool readDest = false;
        while (readArea && readDest) {
            unsigned char flag = m_msg.readChar();
            switch (flag) {
            case EXT_HEADER__VALUE_BLOCK_AREAs: {
                readArea = true;
                m_areas = SubscriptionsHelper::readBlockAreas(&m_msg);
                break;
            }
            case EXT_HEADER__VALUE_BLOCK_IDs: {
                readDest = true;

                unsigned int numDests = (unsigned int) m_msg.readChar();
                if (numDests == 1)
                	m_sourceId.push_back(m_msg.readInt());
                else {
                	// if source ID is not mentioned (m_sourceId.size()==0), then listen to packets from any source
                	stringstream log;
                	log << "iCS --> SubAppMsgReceive: The number of destinations must be one for GEOUNICAST transmission/transmission.";
                	IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
                }
                break;
            }
            default: {
            	stringstream log;
            	log << "iCS --> SubAppMsgReceive: GEOUNICAST - Ubiquitous Mode !";
            	IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
            }
            }
        }

        std::cerr << "GEOUNICAST transmission/reception not implemented in the iCS." << std::endl; // TODO: Implement the geounicast transmission.
        return false;

    }
    case EXT_HEADER_TYPE_GEOANYCAST: {
    	stringstream log;
    	log << "iCS --> SubAppMsgReceive: Communication mode: GEOANYCAST";
    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

        m_commMode = GEOANYCAST;
        unsigned char flagAreas = m_msg.readChar();
        if (flagAreas == EXT_HEADER__VALUE_BLOCK_AREAs)
            m_areas = SubscriptionsHelper::readBlockAreas(&m_msg);
        else {
        	stringstream log;
        	log << "iCS --> SubAppMsgReceive: Receiving a GEOANYCAST message from any dissemination area.";
        	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
        }

        std::cerr << "GEOANYCAST transmission/reception not implemented in the iCS." << std::endl; // TODO: Implement the geoanycast transmission.
        return false;

    }
    default: {
    	stringstream log;
    	log << "iCS --> SubAppMsgReceive: Impossible to subscribe to receive the message. Communication mode not recognized.";
    	IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);

        return false;
    }
    }

	stringstream log;
	log << "iCS --> SubAppMsgReceive: [DEB] - The message was correctly scheduled for reception!!!";
	IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    return true;
}

//===================================================================

bool
SubsAppMessageReceive::returnStatus()
{
    return m_schedulingStatus;
}

//===================================================================

int
SubsAppMessageReceive::ProcessReceivedAppMessage(ScheduledAPPMessageData message, SyncManager* syncManager)
{
    if (syncManager == NULL) {
#ifdef LOG_ON
        IcsLog::LogLevel("ProcessReceivedAppMessage() SyncManager is NULL", kLogLevelError);
#endif
        return EXIT_FAILURE;
    }

    int senderId = message.senderIcsID;

    // Identify the subscription type
    ITetrisNode* m_senderNode = NULL;
    m_senderNode = syncManager->GetNodeByIcsId(senderId);
    if (m_senderNode == NULL) {
#ifdef LOG_ON
        stringstream log;
        log << "ProcessReceivedAppMessage() Sender Node does not exist, senderId = " << senderId;
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
        return EXIT_FAILURE;
    }

    if ((m_destId.size() == 0) || (std::find(m_destId.begin(), m_destId.end(), message.receiverIcsID) != m_destId.end())) {
    	// checking on the type of message subscribed by the receiver
    	if (message.messageType == m_commMode) { // same communication mode..looking at the optional flags
    		switch(message.messageType ) {
    		case UNICAST: {
    			if ((m_sourceId.size() == 0) || (std::find(m_sourceId.begin(), m_sourceId.end(), message.senderIcsID) != m_sourceId.end())) {
    				// contact appHandler and forward message to APP
    				receivedData.push_back(make_pair(message, message.receiverIcsID));
#ifdef LOG_ON
    				stringstream log;
    				log << "iCS --> ProcessReceivedAppMessage(): pushed a message with receiver ID " << message.receiverIcsID << " and appID " << message.appMessageId ;
    				IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

    				m_appMsgReceived = true;
    			}
    			else {
#ifdef LOG_ON
    				stringstream log;
    				log << "ProcessReceivedAppMessage() Unmatching Unicast ID Sender Node = " << message.senderIcsID;
    				IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
    			}

    			return EXIT_SUCCESS;
    		}
    		case GEOBROADCAST: {
    			// TODO add a check related to the sender being in the geobroadcast area
    			// contact appHandler and forward message to APP
    			m_appMsgReceived = true;
    			receivedData.push_back(make_pair(message, message.receiverIcsID));

    			return EXIT_SUCCESS;
    		}
    		case TOPOBROADCAST: {
    			// TODO add a check related to the sender being in the geobroadcast area
    			// contact appHandler and forward message to APP
    			m_appMsgReceived = true;
    			receivedData.push_back(make_pair(message, message.receiverIcsID));

    			return EXIT_SUCCESS;
    		}
    		default: { //other communication type not supported in this version of iCS
#ifdef LOG_ON
    			stringstream log;
    			log << "ProcessReceivedAppMessage() Communication Mode unsupported by APP Msg Receive. Blocking Message. ";
    			IcsLog::LogLevel((log.str()).c_str(), kLogLevelError);
#endif
    			return EXIT_FAILURE;
    		}
    		}
    	}
    }
#ifdef LOG_ON
    stringstream log;
    log << "ProcessReceivedAppMessage() Unmatching Destination ID Receiver Node = " << message.receiverIcsID;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelWarning);
#endif
    return EXIT_SUCCESS;  // we do not act, but do not return an error, as it just means that  the message was not meant for us.

}

//===================================================================


Area2D* getWholeArea(vector<Area2D*> areas);

/**
* @brief Given a geometric shape or a road element, returns a circle object that includes the area.
* @param[in] area Geometric shape or road element.
* @return Returns a circle object that includes the area given in input.
*/
Circle getCircleFromArea(Area2D* area);


//===================================================================

int
SubsAppMessageReceive::InformApp(AppMessageManager* messageManager)
{
    if (m_appMsgReceived == false) {
        IcsLog::LogLevel("InformApp() No information about SubsAppMessageReceive will be sent to app.",kLogLevelInfo);
        return EXIT_SUCCESS;
    }

    else {// At least one message has been received for the application
    	if (messageManager->CommandSendSubscriptionAppMessageReceive(receivedData) == EXIT_FAILURE) {
#ifdef LOG_ON
          IcsLog::LogLevel("iCS --> ProcessReceivedAppMessage() InformAPP - could not send the result of the subscription", kLogLevelError);
#endif
    	  return EXIT_FAILURE;
    	}
    	m_appMsgReceived = false;
    	return EXIT_SUCCESS;
    }
 }

}
