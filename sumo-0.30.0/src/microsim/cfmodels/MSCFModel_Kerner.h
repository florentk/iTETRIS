/****************************************************************************/
/// @file    MSCFModel_Kerner.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    03.04.2010
/// @version $Id: MSCFModel_Kerner.h 23699 2017-03-29 07:16:27Z namdre $
///
// car-following model by B. Kerner
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
#ifndef MSCFModel_Kerner_h
#define MSCFModel_Kerner_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_Kerner
 * @brief car-following model by B. Kerner
 * @see MSCFModel
 */
class MSCFModel_Kerner : public MSCFModel {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] emergencyDecel The maximum emergency deceleration
     * @param[in] dawdle The driver imperfection
     * @param[in] tau The driver's reaction time
     */
    MSCFModel_Kerner(const MSVehicleType* vtype, double accel, double decel, double emergencyDecel, double headwayTime, double k, double phi);


    /// @brief Destructor
    ~MSCFModel_Kerner();


    /// @name Implementations of the MSCFModel interface
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    double moveHelper(MSVehicle* const veh, double vPos) const;

    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @see MSCFModel::ffeV
     */
    double followSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double predMaxDecel) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    double stopSpeed(const MSVehicle* const veh, const double speed, double gap2pred) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return SUMO_TAG_CF_BKERNER;
    }
    /// @}



    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;


    MSCFModel::VehicleVariables* createVehicleVariables() const;


private:
    class VehicleVariables : public MSCFModel::VehicleVariables {
    public:
        double rand;
    };

    /** @brief Returns the "safe" velocity
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The LEADER's speed
     * @return the safe velocity
     */
    double _v(const MSVehicle* const veh, double speed, double vfree, double gap, double predSpeed) const;



private:
    /// @name model parameter
    /// @{
    /// @brief Kerner's k
    double myK;

    /// @brief Kerner's phi
    double myPhi;

    /// @brief The precomputed value for myDecel*myTau
    double myTauDecel;
    /// @}

};

#endif /* MSCFModel_Kerner_H */

