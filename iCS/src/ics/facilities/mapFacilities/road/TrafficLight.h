/****************************************************************************/
/// @file    TrafficLight.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 12, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../../../../utils/ics/iCStypes.h"
using namespace ics_types;

namespace ics_facilities {

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TrafficLight
 * @brief Represents a traffic light.
*/
class TrafficLight {
public:

    /**
     * @brief Constructor.
     */
    TrafficLight();

    /**
    * @brief Constructor.
    * @param[in] trafficLightID ID of the traffic light to be created.
    * @param[in] linkIndexArg Index of the link.
    * */
    TrafficLight(trafficLightID_t trafficLightID, short linkIndexArg);

    /**
    * @brief Destructor.
    */
    virtual ~TrafficLight();

    /**
    * @brief Returns the ID of the traffic light.
    */
    trafficLightID_t getID() const;

    /**
    * @brief Returns the Index of the link associated to the traffic light.
    */
    short getLinkIndex() const;

    /**
    * @brief Returns the position of the traffic light.
    */
    const Point2D& getPosition() const;

    /**
    * @brief Returns the ID of the lane controlled by the traffic light.
    */
    roadElementID_t getControlledLaneID() const;

    /**
    * @brief Returns the ID of the lane that follows the controlled lane.
    */
    roadElementID_t getFollowingLaneID() const;

    /**
    * @brief Returns the status of the traffic light.
    */
    tlStatus getStatus() const;

    /**
    * @brief Check if the traffic light is active.
    * @return TRUE The traffic light is active.
    * @return FALSE The traffic light is not active.
    */
    bool isActive() const;

    /**
    * @brief Sets the position of the traffic light.
    * @param[in] pos Position of the traffic light.
    */
    void setPosition(Point2D pos);

    /**
    * @brief Sets the status of the traffic light.
    * @param[in] newStatus
    */
    void setStatus(tlStatus newStatus);

    /**
    * @brief Sets the ID of the lane controlled by the traffic light.
    * @param[in] contrLaneID ID of the lane controlled by the traffic light.
    */
    void setControlledLaneID(roadElementID_t contrLaneID);

    /**
    * @brief Sets the ID of the lane that follows the controlled lane of the traffic light.
    * @param[in] followLaneID ID of the lane that follows the lane after the controlled one.
    */
    void setFollowingLaneID(roadElementID_t followLaneID);

    /**
    * @brief Switches on or off the traffic light.
    * @param[in] active Indicates whether the traffic light is active or not (FALSE by default).
    */
    void setActive(bool active);

private:

    /// @brief ID of the traffic light.
    trafficLightID_t ID;

    /// @brief Index of the link associated to the traffic light.
    short linkIndex;

    /// @brief Position of the traffic light.
    Point2D position;

    /// @brief ID of the lane controlled by the traffic light.
    roadElementID_t controlledLaneID;

    /// @brief ID of the lane that follows the lane after the controlled one.
    roadElementID_t followingLaneID;

    /// @brief Status of the traffic light (UNKNOWN by default).
    tlStatus status;

    /// @brief Indicates whether the traffic light is active or not (FALSE by default).
    bool active;
};

} //namespace

#endif /* TRAFFICLIGHT_H_ */

