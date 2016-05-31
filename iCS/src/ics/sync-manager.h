/****************************************************************************/
/// @file    sync-manager.h
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <vector>

#include "utilities.h"
#include "itetris-simulation-config.h"
#include "traffic_sim_communicator/traci-client.h"
#include "wirelesscom_sim_communicator/ns3-client.h"
#include "utils/ics/iCStypes.h"
#include "wirelesscom_sim_message_tracker/V2X-message-manager.h"
#include "FacilitiesManager.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class ITetrisNode;
class VehicleNode;
class ApplicationHandler;
class TrafficSimulatorCommunicator;
class V2xMessageManager;
class Subscription;
class FacilitiesManager;

// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @struct MessageInfo
 * @brief Struct to share the information between the iCS core functions and
 / facilities.
 * The members in this structure is filled with the information from the
 * simulation of the V2X messages in ns-3. It is used by the Facilities Manager
 * to make the information available to the facilities block in the iCS.
*/
struct MessageInfo {
    int nodeId;
    int nodeType;
    int infoType;
    float posX;
    float posY;
    float speed;
};

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SyncManager
 * @brief Controls the synchronization of the simulation.
*/
class SyncManager
{
public:

    /**
    * @brief Constructor.
    * @param[in] ns3Port ns-3 listening socket port number.
    * @param[in] sumoPort SUMO listening socket port number.
    * @param[in] sumoHost host where SUMO is running.
    * @param[in] ns3Host host where ns-3 is running.
    * @param[in] beginTime Timestep in which the iCS starts monitoring the simulation.
    * @param[in] endTime Timestep in which the whole simulation ends.
    */
    SyncManager(int ns3Port, int sumoPort, std::string sumoHost, std::string ns3Host, int beginTime, int endTime);

    /// @brief Destructor.
    ~SyncManager();

    /**
    * @brief Abstract connector to the wireless communication simulator.
    * This member is used as a generic link. The real implementation
    * is in the ns-3 connector member.
    */
    static WirelessComSimulatorCommunicator* m_wirelessComSimCommunicator;

    /// @brief Connector to the the ns-3 simulator.
    Ns3Client m_ns3Client;

    /**
    * @brief Abstract connector to the traffic road simulator.
    * This member is used as a generic link. The real implementation
    * is in the TraCI connector member.
    */
    static TrafficSimulatorCommunicator* m_trafficSimCommunicator;

    /// @brief Interface to the SUMO.
    TraCIClient m_traci;

    /// @brief Entity to access the information of the messages scheduled in ns-3.
    static V2xMessageManager* m_v2xMessageTracker;

    /**
     * @brief Global simulation timestep counter.
     * It is initialized with -1 value.
     */
    static ics_types::icstime_t m_simStep;

    /**
     * @brief The collection of ITS Stations involved in the simulation.
     * @todo Consider to move this to static.
     */
    std::vector<ITetrisNode*>* m_iTetrisNodeCollection;

    /// @brief Table that stores the information related to the CAM scheduled messages.
    std::vector<ScheduledCamMessageData> ScheduledCamMessageTable;

    /// @brief Table that stores the information related to the UNICAST scheduled messages.
    std::vector<ScheduledUnicastMessageData> ScheduledUnicastMessageTable;

    /// @brief Table that stores the information related to the GEOBROADCAST scheduled messages.
    std::vector<ScheduledGeobroadcastMessageData> ScheduledGeobroadcastMessageTable;

    /// @brief Table that stores the information related to the TOPOBROADCAST scheduled messages.
    std::vector<ScheduledTopobroadcastMessageData> ScheduledTopobroadcastMessageTable;

    /// @brief Table that stores the identifiers related to the received CAM messages.
    std::vector<IdentifiersStorageStruct> IdentifiersStorageTable;

    /**
    * @brief Member function launching the run-time phase.
    * @param[in] interactive Whether the loop shall run in interactive mode
    * @return 0: If there isn't any error
    */
    int Run(bool interactive);

    /**
    * @brief Stops the execution of the simulation
    * @return 0: If an error occurs
    * @return 1: If all connections closes successfully
    */
    int Stop();

    /**
    * @brief Establishes the connection with ns-3
    * @return EXIT_SUCCESS if the connection with ns-3 establishes successfully EXIT_FAILURE otherwise
    */
    int ConnectNs3();

    /**
    * @brief Closes the connection with ns-3
    * @return 0: If an error occurs
    * @return 1: If the connection with ns-3 closes successfully
    */
    int CloseNs3();

    /**
    * @brief Establishes the connection with SUMO
    * @return EXIT_SUCCESS if the connection with SUMO establishes successfully EXIT_FAILURE otherwise
    */
    int ConnectSumo();

    /**
    * @brief Closes the connection with SUMO
    * @return 0: If an error occurs
    * @return 1: If the connection with SUMO closes successfully
    */
    int CloseSumo();

    /**
    * @brief Establishes the connection with the applications
    * @return EXIT_SUCCESS if the connection with the application was established successfully EXIT_FAILURE otherwise
    */
    int ConnectToApps();

    /// @todo Not implemented.
    int CloseApps();

    /**
    * @brief Orders ns-3 to create a CIU in its collection
    * @return EXIT_SUCCESS if the all the stations were correctly set up in ns-3, EXIT_FAILURE otherwise
    */
    int SetFixedStationInNs3();

    /**
    * @brief Updates the position of the node in the ns-3 module
    * @return EXIT_SUCCESS if the all the stations were correctly updated in ns-3, EXIT_FAILURE otherwise
    */
    int UpdatePositionsInNs3();

    /**
    * @brief Installs an application on a node
    * @param[in] node A node object
    * @return True: If the installation takes place successfully
    * @return False: If an error occurs
    */
    bool AssignApplication(ITetrisNode *node);

    /**
    * @brief Adds an element in the application handler
    * @param[in] appHandler Element to insert
    */
    void RecognizeNewApplication(ApplicationHandler* appHandler);

    /**
    * @brief Schedules the transmission of messages
    * @return EXIT_SUCCESS if the messages where scheduled correctly, EXIT_FAILURE otherwise
    */
    int ScheduleV2xMessages();

    /**
    * @brief Obtains the received messages from ns-3
    * @return EXIT_SUCCESS if the values where obtained correctly, EXIT_FAILURE otherwise
    */
    int GetDataFromNs3();

    /**
    * @brief Executes an application
    * @return 0: If the execution finishes successfully
    * @return 1: If an error occurs
    */
    int RunApplicationLogic();

    /**
    * @brief Looks for a node using its SUMO identifier
    * @param[in,out] &nodeId SUMO node identifier
    * @return The corresponding node object
    */
    ITetrisNode* GetNodeBySumoId(const std::string &nodeId);

    /**
    * @brief Looks for a node using iCS identifier
    * @param[in] nodeId iCS node identifier
    * @return The corresponding node object
    */
    ITetrisNode* GetNodeByIcsId(ics_types::stationID_t nodeId);

    /**
    * @brief Set up facilities
    * @param[in,out] &file File with the facilities data.
    * @return EXIT_SUCCESS if correct setup.
    */
    int InitializeFacilities(const std::string& file);

    /**
     * @brief Member to access iCS Facilities layer functionalities.
     * @todo Change the member type.
     */
    static FacilitiesManager* m_facilitiesManager;

    ///@brief Get the pointer to the SyncManager object.
    SyncManager* const GetAddress() {
        return this;
    };

    /**
    * @brief Schedules the transmission of UNICAST messages.
    * @param[in] sender Identifier of the station that sends the message.
    * @param[in] appHandlerId Identifier of the application that generated the message.
    * @param[in] destination Identifier of the destination station of the message.
    * @param[in] appMessageId Identifier of the message at the application layer (i.e., the sequence number).
    * @param[in] appMessageType Type of message according to the application logic (e.g., request, response, data, etc.).
    * @param[in] frequency Frequency of generation of the message.
    * @param[in] payloadLength Length of the payload of the message.
    * @param[in] msgRegenerationTime
    * @param[in] commProfile Profile for the technology selector (0xFF means any profile and it is the default value).
    * @param[in] preferredRATs Preferred radio access technologies to send the message.
    * @param[in] msgLifetime
    * @return EXIT_SUCCESS if the messages where scheduled correctly, EXIT_FAILURE otherwise
    * @todo To be commented.
    */
    int ScheduleV2xUnicastMessages(ics_types::stationID_t sender, int appHandlerId, ITetrisNode* destination, int appMessageId, unsigned char appMessageType, int frequency, int payloadLength, float msgRegenerationTime, unsigned char commProfile, unsigned char preferredRATs, unsigned int msgLifetime);

    /**
       * @brief Schedules the transmission of APP messages, which technology and type (broadcast, unicast) will be selected by the TechnoSelector.
       * @param[in] sender Identifier of the station that sends the message.
       * @param[in] appHandlerId Identifier of the application that generated the message.
       * @param[in] destination Identifier of the destination station of the message, if any.
       * @param[in] appMessageId Identifier of the message at the application layer (i.e., the sequence number).
       * @param[in] appMessageType Type of message according to the application logic (e.g., request, response, data, etc.).
       * @param[in] frequency Frequency of generation of the message.
       * @param[in] payloadLength Length of the payload of the message.
       * @param[in] msgRegenerationTime
       * @param[in] commProfile Profile for the technology selector (0xFF means any profile and it is the default value).
       * @param[in] preferredRATs Preferred radio access technologies to send the message.
       * @param[in] areas Vector containing the dissemination areas from the application point of view, if any.
       * @param[in]  numHops number of hops expected to be followed by the message transmission (if supported by the message type)
       * @param[in] msgLifetime
       * @return EXIT_SUCCESS if the messages where scheduled correctly, EXIT_FAILURE otherwise
       */
    int ScheduleV2XApplicationMessages(stationID_t sender, int appHandlerId, ITetrisNode* destination, int appMessageId, unsigned char appMessageType, int frequency, int payloadLength, float msgRegenerationTime, unsigned char commProfile, unsigned char preferredRATs, std::vector<Area2D*> areas, short numHops, unsigned int msgLifetime);

    /**
    * @brief Schedules the transmission of TOPOBROADCAST messages
    * @param[in] sender Identifier of the station that sends the message.
    * @param[in] appHandlerId Identifier of the application that generated the message.
    * @param[in] appMessageId Identifier of the message at the application layer (i.e., the sequence number).
    * @param[in] frequency Frequency of generation of the message.
    * @param[in] payloadLength Length of the payload of the message.
    * @param[in] msgRegenerationTime
    * @param[in] appMessageType Type of message according to the application logic (e.g., request, response, data, etc.).
    * @param[in] commProfile Profile for the technology selector (0xFF means any profile and it is the default value).
    * @param[in] preferredRATs Preferred radio access technologies to send the message.
    * @param[in] numHops Number of hops from the source station.
    * @param[in] msgLifetime
    * @return EXIT_SUCCESS if the messages where scheduled correctly, EXIT_FAILURE otherwise
    */
    int ScheduleV2xTopobroadcastMessages(ics_types::stationID_t sender, int appHandlerId, int appMessageId, int frequency, int payloadLength, float msgRegenerationTime, unsigned char appMessageType, unsigned char commProfile, unsigned char preferredRATs, short numHops, unsigned int msgLifetime);

    /**
    * @brief Schedules the transmission of GEOBROADCAST messages
    * @param[in] sender Identifier of the station that sends the message.
    * @param[in] appHandlerId Identifier of the application that generated the message.
    * @param[in] appMessageId Identifier of the message at the application layer (i.e., the sequence number).
    * @param[in] frequency Frequency of generation of the message.
    * @param[in] payloadLength Length of the payload of the message.
    * @param[in] msgRegenerationTime
    * @param[in] appMessageType Type of message according to the application logic (e.g., request, response, data, etc.).
    * @param[in] commProfile Profile for the technology selector (0xFF means any profile and it is the default value).
    * @param[in] preferredRATs Preferred radio access technologies to send the message.
    * @param[in] areas Vector containing the dissemination areas from the application point of view.
    * @param[in] msgLifetime
    * @return EXIT_SUCCESS if the messages where scheduled correctly, EXIT_FAILURE otherwise
    */
    int ScheduleV2xGeobroadcastMessages(ics_types::stationID_t sender, int appHandlerId, int appMessageId, int frequency, int payloadLength, float msgRegenerationTime, unsigned char appMessageType, unsigned char commProfile, unsigned char preferredRATs, std::vector<Area2D*> areas, unsigned int msgLifetime);

    /**
    * @brief Removes a node from the CAM Areas when the correspondent node has reached his destination
    * @param[in] node
    */
    void RemoveNodeInTheArea(ITetrisNode* node);

private:

    /// @brief The timestep in which the simulation will end.
    int m_lastTimeStep;

    /// @brief The timestep in which the applications will enter the simulation.
    int m_firstTimeStep;

    /// @brief The collection of applications running in the simulation.
    std::vector<ApplicationHandler*>* m_applicationHandlerCollection;

    /**
     * @brief Executes the logic of SUMO for the current timestep.
     * @return 0 if timestep was correctly executed.
     * @return 1 If timestep didn't execute correctly.
     * @todo Remove the vehicles that left the simulation.
     * @todo Update the facilities with new information.
     * @todo Inform applications???
     */
    int RunOneSumoTimeStep();

    /**
     * @brief Executes the logic of ns-3 for the next timestep.
     * @return 0 if timestep was correctly executed.
     * @return 1 If timestep didn't execute correctly.
     */
    int RunOneNs3TimeStep();

    /**
     * @brief Loops the results of the applications and updates the
     / brief facilities accordingly.
     * @return EXIT_SUCCESS if the connection with ns-3 establishes successfully EXIT_FAILURE otherwise.
     * @todo RESULTS ARE LOOPED BUT NOT APPLIED.
     */
    int ProcessApplicationResults();

    /**
     * @brief Sends to the application the subscribed data of a node.
     * @param[in] node The node the applications belongs to.
     * @return EXIT_SUCCESS if the connection with ns-3 establishes successfully EXIT_FAILURE otherwise.
     */
    int ForwardSubscribedDataToApplication(ITetrisNode* node);

    /**
    * @brief Questions the applications if it would like to subscribe to data.
    * @param[in] node The node the subscription belong to.
    * @return EXIT_SUCCESS if the subscription were created correctly, EXIT_FAILURE otherwise.
    */
    int NewSubscriptions(ITetrisNode* node);

    /**
    * @brief Asks the application if it would like to unsubscribe.
    * @param[in] node The node related with the subscriptions to drop.
    * @return EXIT_SUCCESS if the subscription were removed correctly, EXIT_FAILURE otherwise.
    */
    int DropSubscriptions(ITetrisNode* node);

    /**
    * @brief Sends to the Application the IDs and receiver stations IDs of the
    * correctly communicated messages.
    * @param[in] node The node running the Application that generated the result in the first place.
    * @return EXIT_SUCCESS if the subscription were removed correctly, EXIT_FAILURE otherwise.
    */
    int DeliverMessageStatus(ITetrisNode* node);

    /**
    * @brief Tells the application it can execute its main algorithm.
    * @param[in] node The node storing the result of the application.
    * @return EXIT_SUCCESS if the subscription were removed correctly, EXIT_FAILURE otherwise.
    */
    int ExecuteApplicationMainFunction(ITetrisNode* node);

    /**
    * @brief Returns the node corresponding to the ns-3 ID
    * @param[in] nodeID The ID of the node in ns-3 simulator
    * @return The pointer to the station.
    */
    ITetrisNode* GetNodeByNs3Id(int nodeId);

    /// @brief Collection of all current subscriptions.
    std::vector<Subscription*>* m_subscriptionCollection;

    /**
    * @brief Schedules Geobroadcast messages
    * @return EXIT_SUCCESS if the messages where scheduled correctly, EXIT_FAILURE otherwise
    */
    int ScheduleV2xGeobroadcastMessages();

    /**
    * @brief Schedules CAM messages
    * @return EXIT_SUCCESS if the messages where scheduled correctly, EXIT_FAILURE otherwise
    */
    int ScheduleV2xCamAreaMessages();

    /**
    * @brief Refresh the table that stores the scheduled CAM messages
    * @return EXIT_SUCCESS if the refresh finishes successfully, EXIT_FAILURE otherwise
    */
    int RefreshScheduledCamMessageTable();

    /**
    * @brief Refresh the table that stores the scheduled UNICAST messages
    * @return EXIT_SUCCESS if the refresh finishes successfully, EXIT_FAILURE otherwise
    */
    int RefreshScheduledUnicastMessageTable();

    /**
    * @brief Refresh the table that stores the scheduled GEOBROADCAST messages
    * @return EXIT_SUCCESS if the refresh finishes successfully, EXIT_FAILURE otherwise
    */
    int RefreshScheduledGeobroadcastMessageTable();

    /**
    * @brief Refresh the table that stores the scheduled TOPOBROADCAST messages
    * @return EXIT_SUCCESS if the refresh finishes successfully, EXIT_FAILURE otherwise
    */
    int RefreshScheduledTopobroadcastMessageTable();

    /// @todo To be implemented + commented
    int ProcessGeobroadcastMessages(ScheduledGeobroadcastMessageData message, stationID_t receiverID);

    /// @todo To be commented
    int ProcessTopobroadcastMessages(ScheduledTopobroadcastMessageData message, stationID_t receiverID);

    /**
     * @brief Process Message to find attached subscriptions for APP messages
     * @return EXIT_SUCCESS if APP messages found and processed, EXIT_FAILURE otherwise
    */
    int ProcessAppMessages(ScheduledAPPMessageData appMessage);

    /// @brief Vehicles that left the simulation that are going to tell wireless simulator to deactivate.
    std::vector<std::string> m_vehiclesToBeDeactivated;

    /// @brief Testing purposes only.
    void TestInciPrimitives();
};

}

#endif
