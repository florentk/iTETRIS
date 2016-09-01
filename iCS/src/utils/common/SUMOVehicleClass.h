/****************************************************************************/
/// @file    SUMOVehicleClass.h
/// @author  Daniel Krajzewicz
/// @date    2006-01-24
/// @version $Id: SUMOVehicleClass.h 11284 2011-09-23 10:37:35Z namdre $
///
// Definitions of SUMO vehicle classes and helper functions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOVehicleClass_h
#define SUMOVehicleClass_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <set>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringBijection.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// enum definitions
// ===========================================================================
/**
 * @enum SUMOVehicleShape
 * @brief Definition of vehicle classes to differ between different appearences
 */
enum SUMOVehicleShape {
    /// @brief not defined
    SVS_UNKNOWN,
    /// @brief render as a pedestrian
    SVS_PEDESTRIAN,
    /// @brief render as a bicycle
    SVS_BICYCLE,
    /// @brief render as a motorcycle
    SVS_MOTORCYCLE,
    /// @brief render as a passenger vehicle
    SVS_PASSENGER,
    /// @brief render as a sedan passenger vehicle ("Stufenheck")
    SVS_PASSENGER_SEDAN,
    /// @brief render as a hatchback passenger vehicle ("Fliessheck")
    SVS_PASSENGER_HATCHBACK,
    /// @brief render as a wagon passenger vehicle ("Combi")
    SVS_PASSENGER_WAGON,
    /// @brief render as a van
    SVS_PASSENGER_VAN,
    /// @brief render as a delivery vehicle
    SVS_DELIVERY,
    /// @brief render as a transport vehicle
    SVS_TRANSPORT,
    /// @brief render as a semi-trailer transport vehicle ("Sattelschlepper")
    SVS_TRANSPORT_SEMITRAILER,
    /// @brief render as a transport vehicle with one trailer
    SVS_TRANSPORT_1TRAILER,
    /// @brief render as a bus
    SVS_BUS,
    /// @brief render as a city bus
    SVS_BUS_CITY,
    /// @brief render as a flexible city bus
    SVS_BUS_CITY_FLEXIBLE,
    /// @brief render as a overland bus
    SVS_BUS_OVERLAND,
    /// @brief render as a trolley bus
    SVS_BUS_TROLLEY,
    /// @brief render as a rail
    SVS_RAIL,
    /// @brief render as a light rail
    SVS_RAIL_LIGHT,
    /// @brief render as a city rail
    SVS_RAIL_CITY,
    /// @brief render as a slow (passenger) train
    SVS_RAIL_SLOW,
    /// @brief render as a fast (passenger) train
    SVS_RAIL_FAST,
    /// @brief render as a cargo train
    SVS_RAIL_CARGO,
    /// @brief render as a (futuristic) e-vehicle
    SVS_E_VEHICLE,
    /// @brief render as a giant ant
    SVS_ANT
};



/**
 * @enum SUMOVehicleClass
 * @brief Definition of vehicle classes to differ between different lane usage and authority types
 *
 * Bits:
 * @arg 0-7:  vehicle ownership
 * @arg 8-23: vehicle size
 *
 * From NavTeq:
 * @arg [0] All
 * @arg [1] Passenger cars
 * @arg [2] High Occupancy Vehicle
 * @arg [3] Emergency Vehicle
 * @arg [4] Taxi
 * @arg [5] Public Bus
 * @arg [6] Delivery Truck
 * @arg [7] Transport Truck
 * @arg [8] Bicycle
 * @arg [9] Pedestrian
 */
enum SUMOVehicleClass {
    SVC_UNKNOWN = 0,

    /// @name vehicle ownership
    //@{

    /// @brief private vehicles
    SVC_PRIVATE = 1,
    /// @brief public transport vehicles
    SVC_PUBLIC_TRANSPORT = 2,
    /// @brief public emergency vehicles
    SVC_PUBLIC_EMERGENCY = 4,
    /// @brief authorities vehicles
    SVC_PUBLIC_AUTHORITY = 8,
    /// @brief army vehicles
    SVC_PUBLIC_ARMY = 16,
    /// @brief vip vehicles
    SVC_VIP = 32,
    /// @brief vehicles ignoring classes
    SVC_IGNORING = 64,
    //@}


    /// @name vehicle size
    //@{

    /// @brief vehicle is a passenger car (a "normal" car)
    SVC_PASSENGER = 256,
    /// @brief vehicle is a HOV
    SVC_HOV = 512,
    /// @brief vehicle is a taxi
    SVC_TAXI = 1024,
    /// @brief vehicle is a bus
    SVC_BUS = 2048,
    /// @brief vehicle is a small delivery vehicle
    SVC_DELIVERY = 4096,
    /// @brief vehicle is a large transport vehicle
    SVC_TRANSPORT = 8192,
    /// @brief vehicle is a light rail
    SVC_LIGHTRAIL = 16384,
    /// @brief vehicle is a city rail
    SVC_CITYRAIL = 32768,
    /// @brief vehicle is a slow moving transport rail
    SVC_RAIL_SLOW = 65536,
    /// @brief vehicle is a fast moving rail
    SVC_RAIL_FAST = 131072,

    /// @brief vehicle is a motorcycle
    SVC_MOTORCYCLE = 262144,
    /// @brief vehicle is a bicycle
    SVC_BICYCLE = 524288,
    /// @brief is a pedestrian
    SVC_PEDESTRIAN = 1048576
    //@}

};

extern StringBijection<SUMOVehicleClass> SumoVehicleClassStrings;
extern StringBijection<SUMOVehicleShape> SumoVehicleShapeStrings;

/**
 * @enum SUMOEmissionClass
 * @brief Definition of vehicle emission classes
 *
 * The order is important - HBEFA computation helper use it.
 * @see HelpersHBEFA
 */
enum SUMOEmissionClass {
    SVE_UNKNOWN = -1,
    // heavy duty vehicles; 3 clusters
    SVE_HDV_3_1 = 0,
    SVE_HDV_3_2,
    SVE_HDV_3_3,
    // heavy duty vehicles; 6 clusters
    SVE_HDV_6_1,
    SVE_HDV_6_2,
    SVE_HDV_6_3,
    SVE_HDV_6_4,
    SVE_HDV_6_5,
    SVE_HDV_6_6,
    // heavy duty vehicles; 12 clusters
    SVE_HDV_12_1,
    SVE_HDV_12_2,
    SVE_HDV_12_3,
    SVE_HDV_12_4,
    SVE_HDV_12_5,
    SVE_HDV_12_6,
    SVE_HDV_12_7,
    SVE_HDV_12_8,
    SVE_HDV_12_9,
    SVE_HDV_12_10,
    SVE_HDV_12_11,
    SVE_HDV_12_12,
    // passenger & light duty vehicles; 7 clusters
    SVE_P_LDV_7_1,
    SVE_P_LDV_7_2,
    SVE_P_LDV_7_3,
    SVE_P_LDV_7_4,
    SVE_P_LDV_7_5,
    SVE_P_LDV_7_6,
    SVE_P_LDV_7_7,
    // passenger & light duty vehicles; 14 clusters
    SVE_P_LDV_14_1,
    SVE_P_LDV_14_2,
    SVE_P_LDV_14_3,
    SVE_P_LDV_14_4,
    SVE_P_LDV_14_5,
    SVE_P_LDV_14_6,
    SVE_P_LDV_14_7,
    SVE_P_LDV_14_8,
    SVE_P_LDV_14_9,
    SVE_P_LDV_14_10,
    SVE_P_LDV_14_11,
    SVE_P_LDV_14_12,
    SVE_P_LDV_14_13,
    SVE_P_LDV_14_14,
    // no emissions
    SVE_ZERO_EMISSIONS
};


typedef std::set<SUMOVehicleClass> SUMOVehicleClasses;


// ===========================================================================
// method declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// abstract vehicle class / purpose
// ---------------------------------------------------------------------------
/* @brief SUMOVehicleClass is meant to be OR'ed to combine information about vehicle
 * ownership and vehicle "size" into one int.
 * These OR'ed values cannot be translated directly into strings with toString().
 * The names of all base values are concatenated with '|' as a separator.
 */
extern std::string getVehicleClassCompoundName(int id);


/** @brief Returns the ids of the given classes, divided using a ' '
 * @param[in] ids The ids to encode
 * @return The string representation of these classes
 */
extern std::string getVehicleClassNames(const SUMOVehicleClasses &ids);


/** @brief Returns the class id of the abstract class given by its name
 * @param[in] name The name of the abstract vehicle class
 * @return The internal representation of this class. Name must not be a
 * compound name
 */
extern SUMOVehicleClass getVehicleClassID(const std::string &name);

/** @brief Returns the OR'ed id of the compound class given by its name
 * @param[in] name The name of the abstract vehicle class
 * @return The OR'ed combination of base enum values
 */
extern int getVehicleClassCompoundID(const std::string &name);

/** @brief Parses the given definition of allowed/disallowed vehicle classes into the given containers
 *
 * @param[in] classNames Space separated class names
 * @param[out] container The set of vehicle classes to fill
 */
extern void parseVehicleClasses(const std::string &classNames,
                                SUMOVehicleClasses &container);

/** @brief Parses the given definition of allowed/disallowed vehicle classes into the given containers
 *
 * @param[in] allowedS Definition which classes are allowed
 * @param[in] disallowedS Definition which classes are not allowed
 * @param[out] allowed The set of allowed vehicle classes to fill
 * @param[out] disallowed The set of disallowed vehicle classes to fill
 */
extern void parseVehicleClasses(const std::string &allowedS,
                                const std::string &disallowedS,
                                SUMOVehicleClasses &allowed,
                                SUMOVehicleClasses &disallowed);


/** @brief Parses the given vector of class names into their enum-representation
 * @param[in] classesS The names vector to parse
 * @param[out] classes The parsed classes
 */
extern void parseVehicleClasses(const std::vector<std::string> &classesS,
                                SUMOVehicleClasses &classes);


// ---------------------------------------------------------------------------
// vehicle shape class
// ---------------------------------------------------------------------------
/** @brief Returns the class name of the shape class given by its id
 * @param[in] id The id of the shape class
 * @return The string representation of this class
 */
extern std::string getVehicleShapeName(SUMOVehicleShape id);


/** @brief Returns the class id of the shape class given by its name
 * @param[in] name The name of the shape class
 * @return The internal representation of this class
 */
extern SUMOVehicleShape getVehicleShapeID(const std::string &name);


// ---------------------------------------------------------------------------
// emission class
// ---------------------------------------------------------------------------
/** @brief Returns the class name of the emission class given by its id
 * @param[in] id The id of the emission class
 * @return The string representation of this class
 */
extern std::string getVehicleEmissionTypeName(SUMOEmissionClass id);


/** @brief Returns the class id of the emission class given by its name
 * @param[in] name The name of the emission class
 * @return The internal representation of this class
 */
extern SUMOEmissionClass getVehicleEmissionTypeID(const std::string &name) throw(ProcessError);


// ---------------------------------------------------------------------------
// default vehicle type parameter
// ---------------------------------------------------------------------------
extern const std::string DEFAULT_VTYPE_ID;
extern const SUMOReal DEFAULT_VEH_MAXSPEED;
extern const SUMOReal DEFAULT_VEH_ACCEL;
extern const SUMOReal DEFAULT_VEH_DECEL;
extern const SUMOReal DEFAULT_VEH_SIGMA;
extern const SUMOReal DEFAULT_VEH_LENGTH;
extern const SUMOReal DEFAULT_VEH_MINGAP;
extern const SUMOReal DEFAULT_VEH_TAU;
extern const SUMOVehicleClass DEFAULT_VEH_CLASS;
extern const SUMOReal DEFAULT_VEH_PROB;
extern const SUMOReal DEFAULT_VEH_SPEEDFACTOR;
extern const SUMOReal DEFAULT_VEH_SPEEDDEV;
extern const SUMOReal DEFAULT_VEH_GUIWIDTH;
extern const SumoXMLTag DEFAULT_VEH_FOLLOW_MODEL;
extern const std::string DEFAULT_VEH_LANE_CHANGE_MODEL;
extern const SUMOVehicleShape DEFAULT_VEH_SHAPE;

#endif

/****************************************************************************/

