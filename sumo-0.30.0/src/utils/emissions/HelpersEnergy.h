/****************************************************************************/
/// @file    HelpersEnergy.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id: HelpersEnergy.h 23673 2017-03-27 13:02:44Z behrisch $
///
// Helper methods for HBEFA-based emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef HelpersEnergy_h
#define HelpersEnergy_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <limits>
#include <cmath>
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include "PollutantsInterface.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersEnergy
 * @brief Helper methods for energy-based electricity consumption computation based on the battery device
 */
class HelpersEnergy : public PollutantsInterface::Helper {
public:
    static const int ENERGY_BASE = 3 << 16;

    /** @brief Constructor (initializes myEmissionClassStrings)
     */
    HelpersEnergy();


    /** @brief Computes the emitted pollutant amount using the given speed and acceleration
     *
     * Returns only valid values for electricity all other types give 0.
     *
     * @param[in] c emission class for the function parameters to use
     * @param[in] e the type of emission (CO, CO2, ...), only electricity gives valid results
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [deg]
     * @return The amount emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
     */
    double compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const std::map<int, double>* param) const;

    double getDefaultParam(int paramKey) const {
        return myDefaultParameter.find(paramKey)->second;
    }


private:
    /// @brief The default parameter
    std::map<int, double> myDefaultParameter;

};


#endif

/****************************************************************************/

