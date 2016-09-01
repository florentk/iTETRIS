/****************************************************************************/
/// @file    SUMOVTypeParameter.h
/// @author  Daniel Krajzewicz
/// @date    10.09.2009
/// @version $Id: SUMOVTypeParameter.h 10651 2011-06-17 21:34:20Z behrisch $
///
// Structure representing possible vehicle parameter
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
#ifndef SUMOVTypeParameter_h
#define SUMOVTypeParameter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include "SUMOVehicleClass.h"
#include "RGBColor.h"
#include "SUMOTime.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class OptionsCont;


// ===========================================================================
// value definitions
// ===========================================================================
const int VTYPEPARS_LENGTH_SET = 1;
const int VTYPEPARS_MINGAP_SET = 2;
const int VTYPEPARS_MAXSPEED_SET = 2 << 2;
const int VTYPEPARS_PROBABILITY_SET = 2 << 3;
const int VTYPEPARS_SPEEDFACTOR_SET = 2 << 4;
const int VTYPEPARS_SPEEDDEVIATION_SET = 2 << 5;
const int VTYPEPARS_EMISSIONCLASS_SET = 2 << 6;
const int VTYPEPARS_COLOR_SET = 2 << 7;
const int VTYPEPARS_VEHICLECLASS_SET = 2 << 8;
const int VTYPEPARS_WIDTH_SET = 2 << 9;
const int VTYPEPARS_SHAPE_SET = 2 << 10;


// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @class SUMOVTypeParameter
 * @brief Structure representing possible vehicle parameter
 */
class SUMOVTypeParameter {
public:
    /** @brief Constructor
     *
     * Initialises the structure with default values
     */
    SUMOVTypeParameter() throw();


    /** @brief Returns whether the given parameter was set
     * @param[in] what The parameter which one asks for
     * @return Whether the given parameter was set
     */
    bool wasSet(int what) const throw() {
        return (setParameter&what)!=0;
    }


    /** @brief Writes the vtype
     *
     * @param[in, out] dev The device to write into
     * @exception IOError not yet implemented
     */
    void write(OutputDevice &dev) const throw(IOError);

    /** @brief Validates stored car-following parameter
     */
    void validateCFParameter() const throw();


    /// @brief The vehicle type's id
    std::string id;

    /// @brief The physical vehicle length + the standing gap in m
    SUMOReal length;
    /// @brief This class' free space in front of the vehicle itself
    SUMOReal minGap;
    /// @brief The vehicle type's maximum speed [m/s]
    SUMOReal maxSpeed;
    /// @brief The probability when being added to a distribution without an explicit probability
    SUMOReal defaultProbability;
    /// @brief The factor by which the maximum speed may deviate from the allowed max speed on the street
    SUMOReal speedFactor;
    /// @brief The standard deviation for speed variations
    SUMOReal speedDev;
    /// @brief The emission class of this vehicle
    SUMOEmissionClass emissionClass;
    /// @brief The color
    RGBColor color;
    /// @brief The vehicle's class
    SUMOVehicleClass vehicleClass;


    /// @name Values for drawing this class' vehicles
    /// @{

    /// @brief This class' width
    SUMOReal width;

    /// @brief This class' shape
    SUMOVehicleShape shape;
    /// @}


    /// @brief The enum-representation of the car-following model to use
    SumoXMLTag cfModel;

    /// @brief Car-following parameter
    typedef std::map<SumoXMLAttr, SUMOReal> CFParams;
    CFParams cfParameter;

    /// @brief The name of the lane-change model to use
    std::string lcModel;

    /// @brief Information for the router which parameter were set
    int setParameter;


    /// @brief Information whether this type was already saved (needed by routers)
    mutable bool saved;

    /// @brief Information whether this is a type-stub, being only referenced but not defined (needed by routers)
    mutable bool onlyReferenced;

};

#endif

/****************************************************************************/

