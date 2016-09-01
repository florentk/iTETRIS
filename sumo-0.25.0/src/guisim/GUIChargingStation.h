/****************************************************************************/
/// @file    GUIChargingStation.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez L�pez
/// @date    20-12-13
/// @version $Id: GUIChargingStation.h 19535 2015-12-05 13:47:18Z behrisch $
///
// A lane area vehicles can halt at (gui-version)
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
#ifndef GUIChargingStation_h
#define GUIChargingStation_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utils/common/Command.h>
#include <utils/common/VectorHelper.h>
#include <utils/geom/PositionVector.h>
#include <microsim/trigger/MSChargingStation.h>
#include <guisim/GUIBusStop.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/Position.h>
#include <gui/GUIManipulator.h>
#include <utils/foxtools/FXRealSpinDial.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class GUIManipulator;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIChargingStation
 * @brief A lane area vehicles can halt at (gui-version)
 *
 * This gui-version of a bus-stop extends MSChargingStation by methods for displaying
 *  and interaction.
 *
 * @see MSChargingStation
 * @see GUIGlObject_AbstractAdd
 * @see GUIGlObject
 */
class GUIChargingStation : public MSChargingStation, public GUIGlObject_AbstractAdd {
public:
    /** @brief Constructor
     * @param[in] idStorage The gl-id storage for giving this object an gl-id
     * @param[in] id The id of the Charging Station
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @param[in] lane The lane the charging station is placed on
     * @param[in] begPos Begin position of the charging station on the lane
     * @param[in] endPos End position of the charging station on the lane
     */
    GUIChargingStation(const std::string& id,
                       const std::vector<std::string>& lines, MSLane& lane,
                       SUMOReal frompos, SUMOReal topos,
                       SUMOReal new_chrgpower, SUMOReal new_efficiency, SUMOReal new_ChargeInTransit, SUMOReal new_ChargeDelay);


    /// @brief Destructor
    ~GUIChargingStation();


    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent);


    /** @brief Returns an own parameter window
     *
     * Bus stops have no parameter windows (yet).
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window (always 0 in this case)
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent);


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    //@}

private:
    /// @brief chrgpower of the Charging Station
    SUMOReal chrgpower;

    /// @brief efficiency of the Charging Station
    SUMOReal efficiency;

    /// @brief allow charge in transit
    SUMOReal chargeInTransit;

    /// @brief charge delay
    SUMOReal chargeDelay;

private:
    /// @brief The rotations of the shape parts
    std::vector<SUMOReal> myFGShapeRotations;

    /// @brief The lengths of the shape parts
    std::vector<SUMOReal> myFGShapeLengths;

    /// @brief The shape
    PositionVector myFGShape;

    /// @brief The position of the sign
    Position myFGSignPos;

    /// @brief The rotation of the sign
    SUMOReal myFGSignRot;
};


#endif
