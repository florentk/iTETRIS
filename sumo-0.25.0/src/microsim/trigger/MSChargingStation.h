/****************************************************************************/
/// @file    MSChargingStation_h.h
/// @author  Daniel Krajzewicz
/// @author  Tamas Kurzveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
/// @version $Id: MSChargingStation.h 19535 2015-12-05 13:47:18Z behrisch $
///
// Chargin Station for Electric vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSChargingStation_h
#define MSChargingStation_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <microsim/MSStoppingPlace.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSBusStop;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ChargingStation
 * @brief Definition of charging stations
 */
class MSChargingStation : public MSStoppingPlace {
public:

    // CONSTRUCTORES

    MSChargingStation(const std::string& new_ChargingStationID, const std::vector<std::string>& new_lines, MSLane& new_Lane,
                      SUMOReal new_StartPos, SUMOReal new_EndPos, SUMOReal new_ChrgPower, SUMOReal new_Efficency, SUMOReal new_ChargeInTransit, SUMOReal new_ChargeDelay);

    virtual ~MSChargingStation();

    /// @brief Get parameter 01, charging station's charging power
    SUMOReal getChrgPower() const;

    /// @brief Get parameter 02, efficiency of the charging station
    SUMOReal getEfficency() const;

    /// @brief Get parameter 03, get chargeInTransit
    SUMOReal getChargeInTransit() const;

    /// @brief Get parameter 03, get Charge Delay
    SUMOReal getChargeDelay() const;

    /// @brief Set parameter 05, charging station's charging power
    inline void setChrgPower(SUMOReal new_ChrgPower);

    /// @brief Set parameter 06, efficiency of the charging station
    inline void setEfficency(SUMOReal new_Efficency);


    // OTHER FUNCTIONS

    /** @brief Check if a vehicle is inside in  the Charge Station
     * @param[in] position Position of vehicle in the LANE
     * @return TRUE if is between StartPostion and EndPostion
     */
    bool vehicleIsInside(const SUMOReal position);


private:

    /// @brief Parameter 01, charging station's charging power
    SUMOReal ChrgPower;

    /// @brief Parameter 02, efficiency of the charging station
    SUMOReal Efficency;

    /// @brief Parameter 03, Allow charge in transit
    SUMOReal ChargeInTransit;

    /// @brief Parameter 03, Charge Delay
    SUMOReal ChargeDelay;


    /** @brief convert from SUMOReal to String
     * @param[in] var Variable in SUMOReal format
     * @return Variable var in String format
     */
    std::string SUMOReal_str(const SUMOReal& var);
};

#endif

