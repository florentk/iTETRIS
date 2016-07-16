/****************************************************************************/
/// @file    subs-app-cmd-traff-sim.h
/// @author  Jerome Haerri (EURECOM)
/// @date    December 15th, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

#ifndef SUBS_APP_CMD_TRAFF_SIM_H_
#define SUBS_APP_CMD_TRAFF_SIM_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif
#include "foreign/tcpip/storage.h"

#include "subscription.h"

namespace ics
{

// ===========================================================================
// class declarations
// ===========================================================================
class SubscriptionsHelper;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class SubsAppCmdTraffSim
* @brief Manages the transmission of commands to the traffic simulator
*/
class SubsAppCmdTraffSim: public ics::Subscription
{
public:

    /**
    * @brief Constructor
    * @param[in] appId ID of the instantiated subscription.
    * @param[in] synchManager used to get a pointer to the Traffic Simulator.
    * @param[in] stationId Station that owns the subscription.
    * @param[in] fields Command fields to be sent to the traffic simulator.
    */
    SubsAppCmdTraffSim(int subId, ics_types::stationID_t stationId, SyncManager* syncManager, unsigned char* msg, int msgSize);

    /**
    * @brief Destructor
    */
    virtual ~SubsAppCmdTraffSim();

    /**
    * @brief Send a set of commands to the traffic simulator at the current timeStep.
    *
    * The required command and related informations are coded according to the Type-Length-Value method.
    *
    * @return True if the command could be completed. False otherwise.
    */
    bool push(SyncManager* syncManager);

    /**
    * @brief Returns if the command completed successfully.
    * @return TRUE if the command completed, FALSE otherwise.
    */
    bool returnStatus();

    /**
    * @brief Deletes the subscription according to the input parameters.
    * @param[in] subscriptions Collection of subscriptions to delete
    * @return EXIT_SUCCESS if the operation result applied successfully EXIT_FAILURE
    */
    static int Delete(ics_types::stationID_t stationID, std::vector<Subscription*>* subscriptions);

private:

#define VALUE_SET_EDGE_TRAVELTIME       0x01
#define VALUE_GET_EDGE_TRAVELTIME       0x02
#define VALUE_RE_ROUTE   	    		0x03
#define VALUE_GET_ROUTE_VARIABLE	 	0x04
#define VALUE_SET_SPEED					0x05
#define VALUE_SLOW_DOWN				    0x06

    //LGstd::vector<unsigned char> m_msg;
    tcpip::Storage             m_msg;
    /// @brief Status of the push method (success or failure)
    bool m_resultStatus;

    /* std::vector<unsigned char> reinterpretFloat(float value);
     std::vector<unsigned char> reinterpretDouble(double value);
     std::vector<unsigned char> reinterpretInt(int value);
     std::vector<unsigned char> reinterpreteShort(short value);
     std::vector<unsigned char> reinterpreteString(const string s);

     std::vector<unsigned char> writeByEndianess(const unsigned char * begin, unsigned int size);*/

};

}

#endif /* SUBS_APP_CMD_TRAFF_SIM_H_ */
