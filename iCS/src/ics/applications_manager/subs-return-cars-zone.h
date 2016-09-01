/****************************************************************************/
/// @file    subs-return-cars-zone.h
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
#ifndef SUBSCRIPTION_RETURN_CARS_IN_ZONE_H
#define SUBSCRIPTION_RETURN_CARS_IN_ZONE_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>

#include "subscription.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class VehicleNode;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class SubsReturnsCarInZone
* @brief Identifies the subscriptions of the cars in a certain zone.
*/
class SubsReturnsCarInZone : public Subscription
{

public:
    /**
    * @brief Constructor.
    * @param[in] appId The identifier of the subscription..
    * @param[in] stationId Station that owns the subscription
    * @param[in] baseX The X value of the base point defined by the application.
    * @param[in] baseY The Y value of the base point defined by the application.
    * @param[in] radius The radious value of the base point defined by the application.
    */
    SubsReturnsCarInZone(int appId, ics_types::stationID_t stationId, float baseX, float baseY, float radius);

    /// @brief Destructor.
    ~SubsReturnsCarInZone();

    /**
    * @brief Identifies the existing vehicles in a certain zone.
    * @param[in] vehicles Collection of vehicles in a certain zone.
    * @return Collection of vehicles in a certain zone.
    */
    std::vector<VehicleNode*>* GetCarsInZone(std::vector<VehicleNode*>* vehicles);

    /**
    * @brief Deletes the subscription according to the input parameters.
    * @param[in] baseX The X value of the base point defined by the application.
    * @param[in] baseY The Y value of the base point defined by the application.
    * @param[in] radius The radious value of the base point defined by the application.
    * @param[in] subscriptions Collection of subscriptions to delete
    * @return EXIT_SUCCESS if the operation result applied successfuly EXIT_FAILURE
    */
    static int Delete(float baseX, float baseY, float radius, std::vector<Subscription*>* subscriptions);

    /// @todo To be commented
    int ProcessReceivedGeobroadcastMessage(ScheduledGeobroadcastMessageData message, SyncManager* syncManager);

    /// @todo To be commented
    int ProcessReceivedUnicastMessage(ScheduledUnicastMessageData message);

private:

    /// @brief The X value of the base point.
    float m_baseX;

    /// @brief The Y value of the base point.
    float m_baseY;

    /// @brief The radious value of the base point.
    float m_radius;
};

}

#endif