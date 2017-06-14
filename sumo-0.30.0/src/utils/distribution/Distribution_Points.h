/****************************************************************************/
/// @file    Distribution_Points.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id: Distribution_Points.h 23362 2017-03-14 09:16:46Z behrisch $
///
// The description of a distribution by a curve
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
#ifndef Distribution_Points_h
#define Distribution_Points_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "RandomDistributor.h"
#include "Distribution.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Distribution_Points
 * A description of a distribution that uses a set of points in a 2d-space
 *  to describe the values (each points x-value) and their possibilities
 *  (each points y-value)
 */
class Distribution_Points :
    public Distribution, public RandomDistributor<double> {
public:
    /// Constructor
    Distribution_Points(const std::string& id);

    /// Destructor
    virtual ~Distribution_Points();

    /** @brief Draw a sample of the distribution.
    *
    * A random sample is drawn according to the assigned probabilities.
    *
    * @param[in] which The random number generator to use; the static one will be used if 0 is passed
    * @return the drawn member
    */
    double sample(MTRand* which = 0) const {
        return get(which);
    }

    /// Returns the maximum value of this distribution
    double getMax() const;

    /// Returns the string representation of this distribution
    std::string toStr(std::streamsize accuracy) const;
};


#endif

/****************************************************************************/

