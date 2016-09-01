/****************************************************************************/
/// @file    MSJunctionLogic.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @date    Wed, 12 Dez 2001
/// @version $Id: MSJunctionLogic.h 18756 2015-08-31 19:16:33Z behrisch $
///
// kinds of logic-implementations.
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
#ifndef MSJunctionLogic_h
#define MSJunctionLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StdDefs.h>
#include "MSLogicJunction.h"
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSJunctionLogic
 */
class MSJunctionLogic {
public:
    /// Destructor.
    virtual ~MSJunctionLogic();

    /// Returns the logic's number of links.
    unsigned int nLinks();

    /// @brief Returns the response for the given link
    virtual const MSLogicJunction::LinkBits& getResponseFor(unsigned int linkIndex) const {
        UNUSED_PARAMETER(linkIndex);
        return myDummyFoes;
    }

    /// @brief Returns the foes for the given link
    virtual const MSLogicJunction::LinkBits& getFoesFor(unsigned int linkIndex) const {
        UNUSED_PARAMETER(linkIndex);
        return myDummyFoes;
    }
    virtual bool getIsCont(unsigned int linkIndex) const {
        UNUSED_PARAMETER(linkIndex);
        return false;
    }


    unsigned int getLogicSize() const {
        return myNLinks;
    }

    virtual bool hasFoes() const {
        return false;
    }




protected:
    /// Constructor.
    MSJunctionLogic(unsigned int nLinks);

    /// The logic's number of links.
    unsigned int myNLinks;

    /// @brief A dummy foe container
    static MSLogicJunction::LinkBits myDummyFoes;

private:
    /// Default constructor.
    MSJunctionLogic();

    /// Copy constructor.
    MSJunctionLogic(const MSJunctionLogic&);

    /// Assignment operator.
    MSJunctionLogic& operator=(const MSJunctionLogic&);

};


#endif

/****************************************************************************/

