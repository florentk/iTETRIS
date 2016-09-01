/****************************************************************************/
/// @file    PositionVector.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id: PositionVector.h 19535 2015-12-05 13:47:18Z behrisch $
///
// A list of positions
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
#ifndef PositionVector_h
#define PositionVector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include "AbstractPoly.h"


// ===========================================================================
// class declarations
// ===========================================================================
class Boundary;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PositionVector
 * @brief A list of positions
 */
class PositionVector : public AbstractPoly, private std::vector<Position> {

private:
    typedef std::vector<Position> vp;

public:
    /** @brief Constructor
     *
     * Creates an empty position vector
     */
    PositionVector();


    /** @brief Constructor
     * @param[in] v The vector to copy
     */
    PositionVector(const std::vector<Position>& v);


    /** @brief Constructor
     * @param[in] beg The begin iterator for copy
     * @param[in] end The end iterator to copy
     */
    PositionVector(const std::vector<Position>::const_iterator beg, const std::vector<Position>::const_iterator end);


    /** @brief Constructor for lines
     * @param[in] p1 the first position
     * @param[in] p2 the second position
     */
    PositionVector(const Position& p1, const Position& p2);


    /// @brief Destructor
    ~PositionVector();

    using vp::iterator;
    using vp::const_iterator;
    using vp::const_reference;
    using vp::value_type;
    using vp::begin;
    using vp::end;
    using vp::push_back;
    using vp::pop_back;
    using vp::clear;
    using vp::size;
    using vp::front;
    using vp::back;
    using vp::reference;
    using vp::erase;
    using vp::insert;


    /** @brief Returns the information whether the position vector describes a polygon lying around the given point
        The optional offset is added to the polygon's boundaries */
    bool around(const Position& p, SUMOReal offset = 0) const;

    /** @brief Returns the information whether the given polygon overlaps with this
        Again a boundary may be specified */
    bool overlapsWith(const AbstractPoly& poly, SUMOReal offset = 0) const;

    /** Returns the information whether this list of points interesects the given line */
    bool intersects(const Position& p1, const Position& p2) const;

    /** Returns the information whether this list of points interesects one the given lines */
    bool intersects(const PositionVector& v1) const;

    /** Returns the position of the intersection */
    Position intersectionPosition2D(const Position& p1,
                                    const Position& p2,
                                    const SUMOReal withinDist = 0.) const;

    /** @brief For all intersections between this vector and other,
     * return the 2D-length of the subvector from this vectors start to the intersection */
    std::vector<SUMOReal> intersectsAtLengths2D(const PositionVector& other) const;

    /** @brief For all intersections between this vector and line,
     * return the 2D-length of the subvector from this vectors start to the intersection */
    std::vector<SUMOReal> intersectsAtLengths2D(const Position& lp1, const Position& lp2) const;

    /** Returns the position of the intersection */
    Position intersectionPosition2D(const PositionVector& v1) const;

    /// @brief ensures that the last position equals the first
    void closePolygon();

    /** @brief returns the position at the given index
        !!! exceptions?*/
    const Position& operator[](int index) const;
    Position& operator[](int index);

    /// Returns the position at the given length
    Position positionAtOffset(SUMOReal pos, SUMOReal lateralOffset = 0) const;

    /// Returns the position at the given length
    Position positionAtOffset2D(SUMOReal pos, SUMOReal lateralOffset = 0) const;

    /// Returns the rotation at the given length
    SUMOReal rotationAtOffset(SUMOReal pos) const;

    /// Returns the rotation at the given length
    SUMOReal rotationDegreeAtOffset(SUMOReal pos) const;

    /// Returns the slope at the given length
    SUMOReal slopeDegreeAtOffset(SUMOReal pos) const;

    /// Returns the position between the two given point at the specified position */
    static Position positionAtOffset(const Position& p1,
                                     const Position& p2, SUMOReal pos, SUMOReal lateralOffset = 0.);

    /// Returns the position between the two given point at the specified position */
    static Position positionAtOffset2D(const Position& p1,
                                       const Position& p2, SUMOReal pos, SUMOReal lateralOffset = 0.);

    /// Returns a boundary enclosing this list of lines
    Boundary getBoxBoundary() const;

    /** @brief Returns the arithmetic of all corner points
     * @note: this is different from the centroid! */
    Position getPolygonCenter() const;

    /** @brief Returns the centroid (closes the polygon if unclosed) */
    Position getCentroid() const;

    /** @brief enlarges/shrinks the polygon by a factor based at the centroid */
    void scaleRelative(SUMOReal factor);

    /** @brief enlarges/shrinks the polygon by an absolute offset based at the centroid */
    void scaleAbsolute(SUMOReal offset);

    Position getLineCenter() const;

    /// Returns the length
    SUMOReal length() const;
    /// Returns the length
    SUMOReal length2D() const;


    /// Returns the area (0 for non-closed)
    SUMOReal area() const;

    /// Returns the information whether this polygon lies partially within the given polygon
    bool partialWithin(const AbstractPoly& poly, SUMOReal offset = 0) const;

    /// Returns the two lists made when this list vector is splitted at the given point
    std::pair<PositionVector, PositionVector> splitAt(SUMOReal where) const;

    /// Output operator
    friend std::ostream& operator<<(std::ostream& os, const PositionVector& geom);

    bool crosses(const Position& p1, const Position& p2) const;

    void add(SUMOReal xoff, SUMOReal yoff, SUMOReal zoff);

    /// @brief mirror coordinates along the x-axis
    void mirrorX();

    PositionVector convexHull() const;

    // @brief append the given vector to this one
    void append(const PositionVector& v, SUMOReal sameThreshold = 2.0);

    PositionVector getSubpart(SUMOReal beginOffset, SUMOReal endOffset) const;

    PositionVector getSubpart2D(SUMOReal beginOffset, SUMOReal endOffset) const;

    PositionVector getSubpartByIndex(int beginIndex, int count) const;

    void sortAsPolyCWByAngle();

    void sortByIncreasingXY();

    void extrapolate(const SUMOReal val, const bool onlyFirst = false);

    void extrapolate2D(const SUMOReal val, const bool onlyFirst = false);

    PositionVector reverse() const;

    static Position sideOffset(const Position& beg, const Position& end, const SUMOReal amount);

    void move2side(SUMOReal amount);

    SUMOReal angleAt2D(int pos) const;


    // @brief inserts p between the two closest positions and returns the insertion index
    int insertAtClosest(const Position& p);

    /// comparing operation
    bool operator==(const PositionVector& v2) const;

    class as_poly_cw_sorter {
    public:
        /// constructor
        as_poly_cw_sorter() {};

    public:
        /// comparing operation
        int operator()(const Position& p1, const Position& p2) const;

    };

    class increasing_x_y_sorter {
    public:
        /// constructor
        explicit increasing_x_y_sorter();

    public:
        /// comparing operation
        int operator()(const Position& p1, const Position& p2) const;

    };

    // !!!
    SUMOReal isLeft(const Position& P0, const Position& P1, const Position& P2) const;

    /// returns the angle in radians of the line connecting the first and the last position
    SUMOReal beginEndAngle() const;

    SUMOReal nearest_offset_to_point2D(const Position& p, bool perpendicular = true) const;

    /** @brief return position p within the length-wise coordinate system
     * defined by this position vector. The x value is the same as that returned
     * by nearest_offset_to_point2D(p) and the y value is the perpendicular distance to this
     * vector with the sign indicating the side (right is postive).
     * if extend is true, the vector is extended on both sides and the
     * x-coordinate of the result may be below 0 or above the length of the original vector
     */
    Position transformToVectorCoordinates(const Position& p, bool extend = false) const;

    /* @brief index of the closest position to p
     * @note: may only be called for a non-empty vector */
    int indexOfClosest(const Position& p) const;

    /* @brief distances of all my points to s and all of s points to myself
     * if perpenciualr is set to true, only the perpendicular distances are
     * returned
     */
    std::vector<SUMOReal> distances(const PositionVector& s, bool perpendicular = false) const;

    /* @brief closest distance to point p
     * (or -1 if perpendicular is true and the point is beyond this vector)
     */
    SUMOReal distance(const Position& p, bool perpendicular = false) const;

    void push_back_noDoublePos(const Position& p);
    void push_front_noDoublePos(const Position& p);

    bool isClosed() const;

    /** @brief Removes positions if too near
     * @param[in] minDist The minimum accepted distance; default: POSITION_EPS
     * @param[in] assertLength Whether the result must at least contain two points (be a line); default: false, to ensure original behaviour
     */
    void removeDoublePoints(SUMOReal minDist = POSITION_EPS, bool assertLength = false);

    /// @brief return whether two positions differ in z-coordinate
    bool hasElevation() const;

private:
    /** @brief return whether the line segments defined by Line p11,p12
     * and Line p21,p22 intersect
     */
    static bool intersects(const Position& p11, const Position& p12,
                           const Position& p21, const Position& p22,
                           const SUMOReal withinDist = 0.,
                           SUMOReal* x = 0, SUMOReal* y = 0, SUMOReal* mu = 0);

};


#endif

/****************************************************************************/

