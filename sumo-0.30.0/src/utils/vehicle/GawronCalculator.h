/****************************************************************************/
/// @file    GawronCalculator.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id: GawronCalculator.h 23150 2017-02-27 12:08:30Z behrisch $
///
// Calculators for route costs and probabilities
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GawronCalculator_h
#define GawronCalculator_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GawronCalculator
 * @brief Cost calculation with Gawron's method.
 */
template<class R, class E, class V>
class GawronCalculator : public RouteCostCalculator<R, E, V> {
public:
    /// Constructor
    GawronCalculator(const double beta, const double a) : myBeta(beta), myA(a) {}

    /// Destructor
    virtual ~GawronCalculator() {}

    void setCosts(R* route, const double costs, const bool isActive = false) const {
        if (isActive) {
            route->setCosts(costs);
        } else {
            route->setCosts(myBeta * costs + ((double) 1.0 - myBeta) * route->getCosts());
        }
    }

    /** @brief calculate the probabilities */
    void calculateProbabilities(std::vector<R*> alternatives, const V* const /* veh */, const SUMOTime /* time */) {
        for (typename std::vector<R*>::iterator i = alternatives.begin(); i != alternatives.end() - 1; i++) {
            R* pR = *i;
            for (typename std::vector<R*>::iterator j = i + 1; j != alternatives.end(); j++) {
                R* pS = *j;
                // see [Gawron, 1998] (4.2)
                const double delta =
                    (pS->getCosts() - pR->getCosts()) /
                    (pS->getCosts() + pR->getCosts());
                // see [Gawron, 1998] (4.3a, 4.3b)
                double newPR = gawronF(pR->getProbability(), pS->getProbability(), delta);
                double newPS = pR->getProbability() + pS->getProbability() - newPR;
                if (ISNAN(newPR) || ISNAN(newPS)) {
                    newPR = pS->getCosts() > pR->getCosts()
                            ? (double) 1. : 0;
                    newPS = pS->getCosts() > pR->getCosts()
                            ? 0 : (double) 1.;
                }
                newPR = MIN2((double) MAX2(newPR, (double) 0), (double) 1);
                newPS = MIN2((double) MAX2(newPS, (double) 0), (double) 1);
                pR->setProbability(newPR);
                pS->setProbability(newPS);
            }
        }
    }

private:
    /** @brief Performs the gawron - f() function
        From "Dynamic User Equilibria..." */
    double gawronF(const double pdr, const double pds, const double x) const {
        if (pdr * gawronG(myA, x) + pds == 0) {
            return std::numeric_limits<double>::max();
        }
        return (pdr * (pdr + pds) * gawronG(myA, x)) /
               (pdr * gawronG(myA, x) + pds);
    }

    /** @brief Performs the gawron - g() function
        From "Dynamic User Equilibria..." */
    double gawronG(const double a, const double x) const {
        if (((1.0 - (x * x)) == 0)) {
            return std::numeric_limits<double>::max();
        }
        return (double) exp((a * x) / (1.0 - (x * x)));
    }

private:
    /// @brief gawron beta - value
    const double myBeta;

    /// @brief gawron a - value
    const double myA;

private:
    /** @brief invalidated assignment operator */
    GawronCalculator& operator=(const GawronCalculator& s);

};
#endif

/****************************************************************************/

