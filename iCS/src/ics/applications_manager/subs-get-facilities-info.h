/****************************************************************************/
/// @file    subs-get-facilities-info.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    December 3rd, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef SUBS_GET_TOPOLOGICAL_INFO_H_
#define SUBS_GET_TOPOLOGICAL_INFO_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "subscription.h"

namespace ics
{

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class SubsGetFacilitiesInfo
* @brief Manages the subscriptions to receive the information from the facilities.
*/
class SubsGetFacilitiesInfo: public ics::Subscription
{
public:

    /**
    * @brief Constructor
    * @param[in] appId ID of the instantiated subscription.
    * @param[in] stationId Station that owns the subscription.
    * @param[in] fields Requested fields to return to the application.
    */
    SubsGetFacilitiesInfo(int subId, ics_types::stationID_t stationId, std::vector<unsigned char> fields);

    /**
    * @brief Destructor
    */
    virtual ~SubsGetFacilitiesInfo();

    /**
    * @brief Get facilities-related information of the subscribed station in the last timeStep.
    *
    * The information is coded according to the Type-Length-Value method.
    *
    * @return vector of unsigned char containing the information.
    */
    std::vector<unsigned char> getFacilitiesInformation();

    /**
    * @brief Deletes the subscription according to the input parameters.
    * @param[in] subscriptions Collection of subscriptions to delete
    * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE
    */
    static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);

    /**
    * @brief Returns the number of fields that the node wants to be updated about.
    * @return number of subscribed fields.
    */
    short int getNumberOfSubscribedFields();

private:

#define TYPE_TOPO            0x00
#define TYPE_RECEIVED_CAM    0x01 // TODO: future extension to allow this subscription to get info about received CAMs

#define VALUE__POS_X         0x01
#define VALUE__POS_Y         0x02
#define VALUE__SPEED         0x03
#define VALUE__DIRECTION     0x04
#define VALUE__ACCELERATION  0x05
#define VALUE__LANE_ID       0x06
#define VALUE__EDGE_ID       0x07
#define VALUE__JUNCTION_ID   0x08

    std::vector<unsigned char> m_subscribedInformation;

    void getTopologicalInformation(unsigned int startPos, unsigned short numFields, std::vector<unsigned char>* info);
    void getReceivedCamInformation(unsigned int startPos, unsigned short numFields, std::vector<unsigned char>* info);

};

}

#endif /* SUBS_GET_TOPOLOGICAL_INFO_H_ */
