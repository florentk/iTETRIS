/****************************************************************************/
/// @file    GNERerouter.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNERerouter.h 24108 2017-04-27 18:43:30Z behrisch $
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNERerouter_h
#define GNERerouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNERerouterInterval;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERerouter
 * Rerouter changes the route of a vehicle as soon as the vehicle moves onto a specified edge.
 */
class GNERerouter : public GNEAdditional {
public:
    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position (center) of the rerouter in the map
     * @param[in] edges vector with the edges of rerotuer
     * @param[in] filename The path to the definition file
     * @param[in] probability The probability for vehicle rerouting
     * @param[in] off Whether the router should be inactive initially
     */
    GNERerouter(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNEEdge*> edges, const std::string& filename, double probability, bool off);

    /// @brief Destructor
    ~GNERerouter();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    void updateGeometry();

    /// @brief Returns position of Rerouter in view
    Position getPositionInView() const;

    /// @brief open GNERerouterDialog
    void openAdditionalDialog();

    /// @brief change the position of the rerouter geometry
    void moveAdditionalGeometry(double offsetx, double offsety);

    /// @brief updated geometry changes in the attributes of additional
    void commmitAdditionalGeometryMoved(double oldPosx, double oldPosy, GNEUndoList* undoList);

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief add edge child
    void addEdgeChild(GNEEdge* edge);

    /// @brief remove edge child
    void removeEdgeChild(GNEEdge* edge);

    /// @brief get edge chidls
    const std::vector<GNEEdge*>& getEdgeChilds() const;

    /**@brief add rerouter interval
     * @return true if was sucesfully added, false if wasn't added due overlapping
     */
    bool addRerouterInterval(const GNERerouterInterval& rerouterInterval);

    /// @brief get rerouter intervals
    const std::vector<GNERerouterInterval>& getRerouterIntervals() const;

    /**@brief set rerouter intervals
     * @return true if was sucesfully set, false if wasn't set due overlapping
     */
    bool setRerouterIntervals(const std::vector<GNERerouterInterval>& rerouterIntervals);

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    const std::string& getParentName() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

    /// @brief check overlapping of a vector of rerouter intervals
    bool checkOverlapping(std::vector<GNERerouterInterval> rerouterIntervals);

protected:
    /// @brief edges of Rerouter
    std::vector<GNEEdge*> myEdges;

    /// @brief filename of rerouter
    std::string myFilename;

    /// @brief probability of rerouter
    double myProbability;

    /// @brief attribute to enable or disable inactive initially
    bool myOff;

    /// @brief set with the GNERerouterInterval
    std::vector<GNERerouterInterval> myRerouterIntervals;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNERerouter(const GNERerouter&);

    /// @brief Invalidated assignment operator.
    GNERerouter& operator=(const GNERerouter&);
};

#endif

/****************************************************************************/
