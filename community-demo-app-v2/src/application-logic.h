/****************************************************************************/
/// @file    application-logic.h
/// @author  Julen Maneros
/// @date    10.10.2010
///
// iTETRIS Cooperative ITS demo application related logic
/****************************************************************************/
// iTETRIS; see http://www.ict-itetris.eu/
// Copyright 2008-2010 iTETRIS consortium
/****************************************************************************/
#ifndef APPLICATIONLOGIC_H
#define APPLICATIONLOGIC_H

#define SENDER_ID 15

#include "foreign/tcpip/storage.h"
#include <vector>
#include <map>
#include <algorithm>

// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum TrafficApplicationResultMessageState
 * @brief The status of the message in the simulation.
 */
enum TrafficApplicationResultMessageState {
    kToBeScheduled = 0,
    kScheduled,
    kArrived,
    kToBeApplied,
    kApplied,
    kToBeDiscarded,
    kMissed,
};

// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @struct AppMessage
 * @brief Information of a message carrying a result of the App
 */
struct AppMessage {
    /// @brief Unique message identifier
    int messageId;

    /// @brief Sender iCS station ID
    int senderId;

    /// @brief Destination iCS station ID
    int destinationId;

    /// @brief Message creation time step
    int createdTimeStep;

    /// @brief The speed to be updated with, i.e. the result
    float payloadValue;
    
    /// @brief station IDs which received this message
    std::vector<int> receivedIds;

    /// @brief Message status
    TrafficApplicationResultMessageState status;
};

/**
 * @struct Area
 * @brief Definition of an 2D area
 */
struct Area {
    float x;
    float y;
    float radius;
};

/**
 * @struct Vehicle
 * @brief Definition of a vehicle
 */
struct Vehicle {
    int id;
    /// @brief x axis value of the position of the vehicle
    float x;
    /// @brief y axis value of the position of the vehicle
    float y;
    /// @brief current vehicle speed
    float speed;
};

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ApplicationLogic
 * @brief Encapsulates the logic of the ITS Application
 */
class ApplicationLogic
{
public:

    /// @brief The collection of vehicles in the CAM area
    static std::vector<Vehicle> m_vehiclesInArea;

    /// @brief The collection of messages the results travel in
    static std::vector<AppMessage> m_messages;

    /// @brief The collection of vehicles, which subscribed to a RX subscription
    static std::map<int, bool> m_carRxSubs;

    /// @brief Counter to create different message IDs
    static int m_messageCounter;

    /// @brief Flag to indicate whether the CAM area subscription is set or not
    static bool m_camAreaIsSet;

    /// @brief Flag to indicate whether the returning car subscription is set or not
    static bool m_returnCarAreaIsSet;

    /// @brief Intializes CAM area values
    static int SetCamArea(float x, float y, float radius);

    /// @brief Intializes returning car area values
    static int SetCarArea(float x, float y, float radius);

    /// @brief Returns a CAM area
    static Area GetCamArea();

    /// @brief Returns the car area
    static Area GetReturningCarArea();

    /// @brief Assigns a time step for the app to start returning values
    static int SetApplicationStartTimeStep(int timestep);

    /// @brief Skeleton to unsubcription. Demo app never requests it
    static bool DropSubscription(int subscriptionType);

    /// @brief Updates the current knowledge of vehicles in the area
    static bool ProcessSubscriptionCarsInZone(std::vector<Vehicle>& vehicles);

    /// @brief Sets sucessufuly received messages to be apply the payload in the next time step
    static bool ProcessMessageNotifications(std::vector<AppMessage>& messages);

    /// @brief For every vehicle in the area assigns a result value
    static std::vector<AppMessage> SendBackExecutionResults(int senderId, int timestep);

    /// @brief Configure an APP_MSG_RECEIVE Subscription
    static tcpip::Storage RequestReceiveSubscription(int nodeId, int timestep);
    
    /// @brief Checks if within the application life_cycle, a specific subscription became required
    static tcpip::Storage CheckForRequiredSubscriptions (int nodeId, int timestep);
    
    /// @brief Checks if within the application life_cycle, a specific APP_MSG_SEND subscription became required
    //static tcpip::Storage CheckForAppMsgSendSubscriptions(int nodeId, int timestep);
    
    /// @brief Checks if within the application life_cycle, a specific APP_CMD_TRAFFIC_SIM subscription became required
   // static tcpip::Storage  CheckForAppTraffSimSubscriptions(int nodeId, int timestep);

    /// @brief Specific method to handle the reception of APP level message from the APP_MSG_RECEIVE subscription 
    static bool ProcessReceptionOfApplicationMessage(std::vector<AppMessage>& messages);


private:

    /// @brief CAM area definition
    static Area m_camArea;

    /// @brief Returning car area definition
    static Area m_carArea;

    /// @brief The time step in which the app will send back results
    static int m_appStartTimeStep;
};

#endif
