/****************************************************************************/
/// @file    app-message-manager.h
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef APP_MESSAGE_MANAGER_H
#define APP_MESSAGE_MANAGER_H

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

#include <vector>

#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "app-commands-subscriptions-constants.h"
#include "../../utils/ics/iCStypes.h"
#include "../sync-manager.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class Subscription;
class SubsReturnsCarInZone;
class VehicleNode;
class ResultContainer;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AppMessageManager
 * @brief Manages the data exchange with the applications through a predefined brief set of commands.
 */
class AppMessageManager
{

public:

    AppMessageManager(SyncManager* syncManager);

    /// @brief Socket to send messages.
    tcpip::Socket *m_socket;

    SyncManager* m_syncManager;

    /**
    * @brief Creates a socket and establishes the connection with the application thread.
    * @param[in] host Machine in which the application is running.
    * @param[in] port Port number to use in the connection with the application module.
    * @return True: If the connection establishes successfully.
    * @return False: If it is impossible to establish the connection.
    */
    bool Connect(std::string host, int port);

    /**
    * @brief Closes the connection with the application module.
    * @return True: If the connection closes successfully.
    * @return False: If an error occurs trying to close the connection.
    */
    bool Close();

    /// @brief Command to close
    int CommandClose();

    /**
    * @brief Asks the application if it wants to subscribe to a new data.
    * @param[in] nodeId The identifier of the node the application is running on top of.
    * @param[in] appId The identifier of the application.
    * @param[in,out] subscriptions Collection of subscriptions the application will be
    * subscribed for the node.
    * @param[in,out] &noMoreSubs True if application stopped asking for subscriptions,
    * false otherwise.
    * @return True: If the function executes successfully
    * @return False: If an error occurs
    */
    bool CommandGetNewSubscriptions(int nodeId, int appId, std::vector<Subscription*>* subscriptions, bool &noMoreSubs);

    /**
    * @brief Asks the application whether it wants to unsubscribe for current subscription.
    * @param[in] nodeId iCS identifier of the node
    * @param[in] subscriptionsToDrop Collection of subscriptions to drop
    * @param[in,out] &noMoreSubs True if application stopped asking for unsubscriptions,
    * false otherwise.
    * @return True: If the function executes successfully
    * @return False: If an error occurs
    */
    bool CommandUnsubscribe(int nodeId, std::vector<Subscription*>* subscriptionsToDrop, bool &noMoreUnSubs);

    /**
    * @brief Asks the application whether it wants to unsubscribe for current subscription. (V 2.O)
    * @param[in] nodeId iCS identifier of the node.
    * @param[in] subscription The subscription candidate to unbsubscription.
    * @return 0 Drop the subscription.
    * @return 1 Keep the subscription.
    * @return -1 Error occurred.
    */
    int CommandUnsubscribe(int nodeId, Subscription* subscription);

    /**
    * @brief Sends to the application the corresponding data of the subscripiton to Return Cars In Zone.
    * @param[in] carsInZone The vehicles in the current zone.
    * @param[in] nodeID The identifier of the node the subscription belongs to.
    * @return True: If the operation finishes successfully
    * @return False: If an error occurs
    */
    bool CommandSendSubscriptionCarsInZone(std::vector<VehicleNode*>* carsInZone, int nodeId);

    /**
    * @brief
    * @param[in] carsInZone
    * @param[in] nodeID
    * @param[in] nodeID
    * @return
    * @todo To be commented.
    */
    int CommandSendSubcriptionCalculateTravelTimeFlags(int nodeId, int startStation, int endStation);

    /**
    * @brief Sends to the application the corresponding data of the subscription to Get Received CAM Information.
    * @param[in] camInfo The information contained in the CAM.
    * @param[in] nodeID The identifier of the node the subscription belongs to.
    * @return True: If the operation finishes successfully
    * @return False: If an error occurs
    */
    bool CommandSendSubscriptionReceivedCamInfo(std::vector<ics_types::TCamInformation>* camInfo, int nodeId);

    /**
    * @brief Sends to the application the corresponding data of the subscription to Get FAcilities Information.
    * @param[in] facInfo The information about the subscribed node.
    * @param[in] nodeID The identifier of the node the subscription belongs to.
    * @return True: If the operation finishes successfully
    * @return False: If an error occurs
    */
    bool CommandSendSubscriptionFacilitiesInfo(std::vector<unsigned char>& facInfo, int nodeId);

    /**
    * @brief Sends to the application the scheduling status report.
    * @param[in] status of the sent message for the subscribed node.
    * @param[in] nodeID The identifier of the node the subscription belongs to.
    * @return True: If the operation finishes successfully
    * @return False: If an error occurs
    */
    bool CommandSendSubscriptionAppMessageSend(bool status, int nodeId);

    /**
     * @brief Subscribes to receive APP messages.
     * @param[in] msgInfo The received messge information about the subscribed node.
     * @param[in] nodeID The identifier of the node the subscription belongs to.
     * @return True: If the operation finishes successfully
     * @return False: If an error occurs
     */
    bool CommandSendSubscriptionAppMessageReceive(std::vector<std::pair<ScheduledAPPMessageData,stationID_t> >& msgInfo);
    /**
    * @brief Sends to the application the corresponding data of the subscription to Get FAcilities Information.
    * @param[in] tsInfo The information about the subscribed node.
    * @param[in] nodeID The identifier of the node the subscription belongs to.
    * @return True: If the operation finishes successfully
    * @return False: If an error occurs
    */
    bool CommandSendSubscriptionAppResultTraffSim(std::vector<unsigned char>& tsInfo, int nodeId);

    /**
    * @brief Sends to the application the scheduling status report.
    * @param[in] status of the CMD for the subscribed node.
    * @param[in] nodeID The identifier of the node the subscription belongs to.
    * @return True: If the operation finishes successfully
    * @return False: If an error occurs
    */
    bool CommandSendSubscriptionAppCmdTraffSim(bool status, int nodeId);

    /**
    * @brief Sends to the application cross application data.
    * @param[in] status of the CMD for the subscribed node.
    * @param[in] nodeID The identifier of the node the subscription belongs to.
    * @return True: If the operation finishes successfully
    * @return False: If an error occurs
     */
    bool CommandSendSubscriptionXApplicationData(vector<unsigned char>& xAppData , int nodeId);


    /**
    * @brief Command Application to execute its main functionality/algorithm.
    * @param[in] nodeID The identifier of the node the application is running on top of.
    * @param[in,out] resultContainer The object in which the result will be stored in.
    * @return True: If the execution finishes successfully.
    * @return False: If an error occurs.
    */
    bool CommandApplicationToExecute(int nodeId, ResultContainer* resultContainer);

    /**
    * @brief Notifies the status of the scheduled messages.
    * @param[in] nodeId The identifier of the node the application is running on top of.
    * @param[in] receivedMessages The IDs and receiver ID of the messages received.
    * @return EXIT_SUCCESS if the information was delivered correctly, EXIT_FAILURE otherwise
    */
    int NotifyMessageStatus(int nodeId, std::vector<std::pair<int,ics_types::stationID_t> >& receivedMessages);

private:

    /**
    * @brief Reports the result of a command execution.
    * @param[in,out] &inMsg Object to stored the reply from the application.
    * @param[in] command The ID of the command the application is replying to.
    * @return True: In the operation finishes successfully
    * @return False: If an error occurs
    */
    bool ReportResultState(tcpip::Storage& inMsg, int command);

    /**
    * @brief Checks the message looking for subscriptions to drop.
    * @param[in,out] &inMsg Object to stored the reply from the application.
    * @param[in] command The ID of the command the application is replying to.
    * @param[in,out] &noMoreUnSubs stores if the application requested has finished its unsubscription phase
    * @return True: In the operation finishes successfully
    * @return False: If an error occursION
    */
    bool ValidateUnsubscriptions(tcpip::Storage& inMsg, std::vector<Subscription*>* subscriptions, bool &noMoreUnSubs);

    /**
    * @brief Checks the message looking for subscriptions to drop.
    * @param[in,out] &inMsg Object to stored the reply from the application.
    * @return 1 To unsubscribe.
    * @return 0 To keep the subscription on.
    * @return -1 Error.
    */
    int ValidateUnsubscriptions(tcpip::Storage& inMsg);
};

}

#endif
