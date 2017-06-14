/****************************************************************************/
/// @file    GUIDanielPerspectiveChanger.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id: GUIDanielPerspectiveChanger.h 23150 2017-02-27 12:08:30Z behrisch $
///
// A class that allows to steer the visual output in dependence to
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
#ifndef GUIDanielPerspectiveChanger_h
#define GUIDanielPerspectiveChanger_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/Position.h>
#include "GUIPerspectiveChanger.h"


// ===========================================================================
// class declarations
// ===========================================================================
class Boundary;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDanielPerspectiveChanger
 * This changer has the following behaviour:
 * - zooming by pressing the right mouse button and moving the
 *  mouse vertically
 * - rotation by pressing the right mouse button and moving the
 *  mouse horizontally
 * - network movement by pressing the left mouse button and
 *  moving the mouse
 */
class GUIDanielPerspectiveChanger : public GUIPerspectiveChanger {
public:
    /* Constructor
     * @param[in] callBack The view to be udpated upon changes
     */
    GUIDanielPerspectiveChanger(GUISUMOAbstractView& callBack, const Boundary& viewPort);

    /// Destructor
    ~GUIDanielPerspectiveChanger();

    void onLeftBtnPress(void* data);
    bool onLeftBtnRelease(void* data);
    void onRightBtnPress(void* data);
    bool onRightBtnRelease(void* data);
    void onMouseWheel(void* data);
    void onMouseMove(void* data);
    long onKeyPress(void* data);

    /// Returns the rotation of the canvas stored in this changer
    virtual double getRotation() const;

    /// Returns the x-offset of the field to show stored in this changer
    virtual double getXPos() const;

    /// Returns the y-offset of the field to show stored in this changer
    virtual double getYPos() const;

    /// Returns the zoom factor computed stored in this changer
    virtual double getZoom() const;

    /// @brief Returns the camera height corresponding to the current zoom factor
    virtual double getZPos() const;

    /// @brief Returns the camera height at which the given zoom level is reached
    virtual double zoom2ZPos(double zoom) const;

    /// @brief Returns the zoom level that is achieved at a given camera height
    virtual double zPos2Zoom(double zPos) const;

    /// Centers the view to the given position, setting it to a size that covers the radius
    void centerTo(const Position& pos, double radius, bool applyZoom = true);

    /** @brief Sets the viewport */
    void setViewport(double zoom, double xPos, double yPos);

    /// @brief Alternative method for setting the viewport
    void setViewportFrom(double xPos, double yPos, double zPos);

    /* @brief Adapts the viewport so that a change in canvass size keeps most of the
     * view intact (by showing more / less instead of zooming)
     * The canvass is clipped/enlarged on the left side of the screen
     *
     * @param[in] change The horizontal change in canvas size in pixels
     */
    void changeCanvasSizeLeft(int change);

    /* @brief avoid unwanted flicker
     * @param[in] delay The minimum time delay in nanoseconds after
     *   mouseDown after which mouse-movements should be interpreted as zoom/drag
     */
    void setDragDelay(FXTime delay) {
        myDragDelay = delay;
    }

private:
    /* Performs the view movement
     * @param[in] xdiff the change to myViewCenter in pixel
     * @param[in] ydiff the change to myViewCenter in pixel
     */
    void move(int xdiff, int ydiff);

    /// Performs the zooming of the view
    void zoom(double factor);

    /// Performs the rotation of the view
    void rotate(int diff);

private:
    /// the original viewport dimensions in m which serve as the reference point for 100% zoom
    double myOrigWidth, myOrigHeight;

    /// the current rotation
    double myRotation;

    /// the current mouse state
    int myMouseButtonState;

    /// Information whether the user has moved the cursor while pressing a mouse button
    bool myMoveOnClick;

    /// the network location on which to zoom using right click+drag
    Position myZoomBase;

    /// avoid flicker
    FXTime myDragDelay;
    FXlong myMouseDownTime;


private:
    /// @brief Invalidated copy constructor.
    GUIDanielPerspectiveChanger(const GUIDanielPerspectiveChanger&);

    /// @brief Invalidated assignment operator.
    GUIDanielPerspectiveChanger& operator=(const GUIDanielPerspectiveChanger&);

};


#endif

/****************************************************************************/

