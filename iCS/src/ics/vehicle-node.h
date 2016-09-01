/****************************************************************************/
/// @file    vehicle-node.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef VEHICLE_NODE_H
#define VEHICLE_NODE_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "itetris-node.h"

namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class VehicleNode
 * @brief Represents a vehicle in the simulation.
 */
class VehicleNode : public ITetrisNode
{
public:

    /**
    * @brief Constructor.
    * @param[in] nodeId Vehicle identifier.
    */
    VehicleNode(const std::string &nodeId);

    /// @brief True if the vehicle moved since the last time-step.
    bool m_moved;

    /**
    * @brief Changes the current speed of the vehicle and returns the acceleration.
    * @param[in] speed The new speed.
    * @return The acceleration of the vehicle since the last speed update.
    * @todo The division by 1 has to be changed if the value is in a subsecond basis.
    */
    float ChangeSpeed(float speed);

    /**
    * @brief Checks if the vehicle changed its position since the last timestep.
    * @param[in] position Position to compare
    * @return True if the vehicle moved.
    */
    bool CheckPosition(std::pair<float, float> position);

    /**
    * @brief Returns the current speed of the vehicle.
    * @return The current speed.
    */
    float GetSpeed();

    /**
    * @brief Returns the current direction of the vehicle.
    * @return The current heading.
    */
    float GetHeading();

private:

    /// @brief Speed value in the last time step.
    float m_lastSpeed;

    /// @brief Speed of the vehicle int he current time step.
    float m_currentSpeed;
};

}

#endif