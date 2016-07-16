/****************************************************************************/
/// @file    traci-client.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef TRACI_CLIENT_H
#define TRACI_CLIENT_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <vector>

#include <foreign/tcpip/socket.h>
#include <foreign/tcpip/storage.h>
#include "traci-comm-constants.h"
#include "traffic-simulator-communicator.h"
#include "../../utils/ics/iCStypes.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class SyncManager;
class ITetrisNode;

// ===========================================================================
// struct definitions
// ===========================================================================
/**
* @struct Position2D
* @brief Stores the coordinates of the node
*/
struct Position2D {
    float x;
    float y;
};

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class TraCIClient
* @brief Manages the connection with SUMO
*/
class TraCIClient : public TrafficSimulatorCommunicator
{
public:
    /// @brief Constructor.
    TraCIClient();

    /**
    * @brief Constructor.
    * @param[in] port Port used in the connection with SUMO
    * @param[in] host Host in which SUMO is running
    */
    TraCIClient(int port, std::string host = "localhost");

    /// @brief Destructor.
    ~TraCIClient();

    /**
    * @brief Creates a socket and establishes the connection with SUMO.
    * @return True: If the connection establishes successfully.
    * @return False: If it is impossible to establish the connection.
    */
    bool Connect();

    /**
    * @brief Closes the connection with SUMO.
    * @return EXIT_SUCCESS if the socket is closed successfully, EXIT_FAILURE otherwise.
    */
    int Close();

    /**
    * @brief Sends a message to SUMO in order to start its simulation.
    * @param[in] time Timestep in which the simulation of SUMO should start.
    * @param[in,out] &departed Collection of vehicles that are in a certain area.
    * @param[in,out] &arrived Collection of vehicles that left a certain area.
    * @return EXIT_SUCCESS if SUMO simulated correctly, EXIT_FAILURE otherwise.
    */
    int CommandSimulationStep(double time, std::vector<std::string> &departed, std::vector<std::string> &arrived);

    /**
    * @brief Sends a command close message
    * @return EXIT_SUCCESS if SUMO exited successfully, EXIT_FAILURE otherwise
    */
    int CommandClose();

    /**
    * @brief Sends a message to SUMO in order to establish the value of the maximum speed for a certain vehicle. 
    * @param[in,out] &node The node to establish its maximum speed value.
    * @param[in] maxSeep The value of the maximum speed to assign to the node.
    * @return EXIT_SUCCESS if the speed was successfuly updated, EXIT_FAILURE otherwise.
    */
    int CommandSetMaximumSpeed(const ITetrisNode &node, float maxSeep);
    
    
    /**
    * @brief Sends a message to SUMO in order to changes the speed smoothly to 
    *   the given value over the given amount of time in ms. 
    *   See http://sumo.dlr.de/wiki/TraCI/Change_Vehicle_State#slow_down_.280x14.29
    * @param[in,out] &node The node to slow down.
    * @param[in] speed The value of slow down speed.
    * @param[in] duration The value of duration of slow down speed.    
    * @return EXIT_SUCCESS if the slowdown was successfuly, EXIT_FAILURE otherwise.
    */    
    int CommandSlowDown(const ITetrisNode &node, float speed, int duration);

    /**
    * @brief Gets the speed of certain station from SUMO.
    * @param[in,out] &node The node to get information from.
    * @return The value of the speed.
    * @todo To implement..
    */
    float GetSpeed(const ITetrisNode &node);

    /**
    * @brief Gets the direction of certain station.
    * @param[in,out] &node The node to get information from.
    * @return The value of the direction.
    * @todo To implement..
    */
    float GetDirection(const ITetrisNode &node);

    /**
    * @brief Gets the vehicle length of certain station.
    * @param[in,out] &node The node to get information from.
    * @return The value of the vehicle length
    */
    float GetVehicleLength(const ITetrisNode &node);

    /**
    * @brief Gets the vehicle width of certain station.
    * @param[in,out] &node The node to get information from.
    * @return The value of the vehicle width.
    */
    float GetVehicleWidth(const ITetrisNode &node);

    /**
    * @brief Gets the vehicle's position.
    * @param[in,out] &node The node to get information from.
    * @return The coordinates correspondig to the node's position
    */
    std::pair<float,float> GetPosition(const ITetrisNode &node);

    /**
    * @brief Gets the status of the exterior lights.
    * @bug LOW PRIORITY.
    * @todo To implement.
    */
    bool GetExteriorLights(const ITetrisNode &node);

    /**
    * @brief Gets the lane identifier the node is at.
    * @param[in,out] &node The node to get information from.
    * @return The lane identifier
    */
    std::string GetLane(const ITetrisNode &node);

    /**
    * @brief Gets the type of the vehicle.
    * @param[in,out] &node The node to get information from.
    * @return The type of the vehicle
    */
    std::string GetVehicleType(const ITetrisNode &node);

    /**
    * @brief S3 Bus Lane Management (DLR)
    * Lets one station run in a lane that is "bus-only"
    * @todo to be commented
    */
    int SetVehicleToRunInLane(const ITetrisNode &node, std::string laneId);

    /**
    * @brief S3 Bus Lane Management (DLR)
    * Lets every station to run in a lane that is "bus-only"
    * @todo to be commented
    */
    int SetVehicleToRunInLane(std::string laneId);

    /**
    * @brief S5: Request Based Personalized Navigation (Eurecom)
    * Reroute a vehicle.
    * @todo to be commented
    */
    bool ReRoute(const ITetrisNode &node, std::vector<std::string> route);

    /**
    * @brief S5: Request Based Personalized Navigation (Eurecom)
    * Reroute a vehicle.
    * @todo to be commented
    */
    bool ReRoute(const ITetrisNode &node);

    /**
    * @brief Involved in S7 Changes the states of certain traffic light.
    * @todo to be commented
    */
    bool ChangeTrafficLightStatus(std::string trafficLightId, std::string lightStates);

    /// @brief Set the TravelTime weight for an edge attached to the road followed by a node
    bool ChangeEdgeWeight(const ITetrisNode &node, std::string edgeId, float weight);

    /// @brief Set the TravelTime weight for an edge
    bool SetEdgeWeight(std::string edgeId, float weight);

    /// @brief Get the TravelTime weight for an edge
    float GetEdgeWeight(std::string edgeId);

    /// @brief Get the vector of edges belonging to the current route followed by a node
    std::vector<std::string> GetRouteEdges(const ITetrisNode &node);

    /// @brief Get the vector of edges belonging to a route
    std::vector<std::string> GetRouteEdges(std::string routeID);

    /// @todo to be commented
    int GetTrafficLights(std::vector<ics_types::trafficLightID_t>& trafficLigthIds);

    /// @todo to be commented
    int GetTrafficLightStatus(ics_types::trafficLightID_t trafficLightId, std::string &state);

    /// @brief Port used for the connection with SUMO.
    int m_port;

    /// @brief Host in which ns-3 is running.
    std::string m_host;

private:
    /**
    * @brief
    * @param[in,out] &inMsg
    * @param[in,out] &departed
    * @param[in,out] &arrived
    * @return True:
    * @return False:
    * @todo to be commented
    */
    bool processSubscriptions(tcpip::Storage &inMsg, std::vector<std::string> &departed, std::vector<std::string> &arrived);

    /**
    * @brief
    * @param[in,out] &objID
    * @param[in] varID
    * @param[in,out] &inMsg
    * @param[in] domID
    * @todo to be commented
    */
    void beginValueRetrieval(const std::string &objID, int varID, tcpip::Storage &inMsg, int domID=CMD_GET_VEHICLE_VARIABLE);

    /**
    * @brief Validates the received command responses
    * @param[in,out] &inMsg Message received
    * @param[in] command Command received
    * @return True: If the command messages match
    * @return False: If the command messages don't match
    */
    bool ReportResultState(tcpip::Storage& inMsg, int command);

    /// @brief Socket for the connection.
    tcpip::Socket* m_socket;
};

}

#endif
