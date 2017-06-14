/****************************************************************************/
/// @file    MSCFModel_Wiedemann.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    June 2011
/// @version $Id: MSCFModel_Wiedemann.h 23699 2017-03-29 07:16:27Z namdre $
///
// The psycho-physical model of Wiedemann
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
#ifndef MSCFModel_Wiedemann_H
#define MSCFModel_Wiedemann_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_Wiedemann
 * @brief The Wiedemann Model car-following model
 * @see MSCFModel
 */
// XXX: which Wiedemann is this? There are several versions... Below it is stated that it is modified it with Krauss vsafe... (Leo)
class MSCFModel_Wiedemann : public MSCFModel {
public:

    /** @brief Constructor
     *
     * @param[in] security The security parameter in [0,1] (dimensionless)
     * @param[in] estimation The estimation capability parameter in [0,1] (dimensionless)
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] emergencyDecel The maximum emergency deceleration
     *
     * @note other parameters of the wiedemann model:
     * - speed: included in MSVehicleType
     */
    MSCFModel_Wiedemann(const MSVehicleType* vtype,
                        double accel, double decel, double emergencyDecel,
                        double security, double estimation);


    /// @brief Destructor
    ~MSCFModel_Wiedemann();


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
     * @param[in] gap The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    double stopSpeed(const MSVehicle* const veh, const double speed, double gap) const;


    /** @brief Returns the maximum gap at which an interaction between both vehicles occurs
     *
     * "interaction" means that the LEADER influences EGO's speed.
     * @param[in] veh The EGO vehicle
     * @param[in] vL LEADER's speed
     * @return The interaction gap
     * @todo evaluate signature
     * @see MSCFModel::interactionGap
     */
    double interactionGap(const MSVehicle* const , double vL) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return SUMO_TAG_CF_WIEDEMANN;
    }


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;


    VehicleVariables* createVehicleVariables() const {
        return new VehicleVariables();
    }
    /// @}


private:
    class VehicleVariables : public MSCFModel::VehicleVariables {
    public:
        VehicleVariables() : accelSign(1) {}
        /// @brief state variable for remembering the drift direction
        double accelSign;
    };


private:
    /* @brief the main enty point for the speed computation
     * @param[in] gap The netto gap (front bumper of ego to back bumper of leader)
     */
    double _v(const MSVehicle* veh, double predSpeed, double gap) const;

    /// @name acceleration based on the 'driving regime'
    /// @{
    double fullspeed(double v, double vpref, double dx, double bx) const; // also 'WUNSCH'
    double following(double sign) const; // also 'FOLGEN'
    double approaching(double dv, double dx, double bx) const;  // also 'BREMSBX'
    double emergency(double dv, double dx) const; // also 'BREMSAX'
    /// @}

private:
    /// @name model parameter
    /// @{

    /// @brief The driver's security parameter // also 'ZF1'
    const double mySecurity;

    /// @brief The driver's estimation parameter // also 'ZF2'
    const double myEstimation;

    /// @brief front-bumper to front-bumper distance
    const double myAX;

    /// @brief perception threshold modifier
    const double myCX;

    /// @brief The vehicle's minimum acceleration [m/s^2]
    const double myMinAccel;

    /// @brief free-flow distance in m
    static const double D_MAX;
    /// @}

    /// @brief vsafe from krauss since Wiedemann is deficient at approaching
    // standing obstacles (see MSCFModel_Krauss::_vsafe)
    double krauss_vsafe(double gap, double predSpeed) const;

private:
    /// @brief Invalidated assignment operator
    MSCFModel_Wiedemann& operator=(const MSCFModel_Wiedemann& s);
};

#endif /* MSCFModel_Wiedemann_H */
