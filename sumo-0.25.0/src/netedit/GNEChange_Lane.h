/****************************************************************************/
/// @file    GNEChange_Lane.h
/// @author  Jakob Erdmann
/// @date    April 2011
/// @version $Id: GNEChange_Lane.h 18830 2015-09-11 08:57:34Z behrisch $
///
// A network change in which a single lane is created or deleted
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
#ifndef GNEChange_Lane_h
#define GNEChange_Lane_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <netbuild/NBEdge.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEEdge;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Lane
 * A network change in which a single lane is created or deleted
 */
class GNEChange_Lane : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Lane)

public:
    /** @brief Constructor for creating/deleting an edge
     * @param[in] edge The edge on which to apply changes
     * @param[in] lane The lane to be deleted or 0 if a lane should be created
     * @param[in] laneAttrs The attributes of the lane to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Lane(GNEEdge* edge, GNELane* lane, const NBEdge::Lane& laneAttrs, bool forward);

    /// @brief Destructor
    ~GNEChange_Lane();

    FXString undoName() const;
    FXString redoName() const;
    void undo();
    void redo();


private:
    /** @brief full information regarding the lane that is to be created/deleted
     * we assume shared responsibility for the pointers (via reference counting)
     */

    // we need the edge because it is the target of our change commands
    GNEEdge* myEdge;

    // we need to preserve the lane because it maybe the target of
    // GNEChange_Attribute commands
    GNELane* myLane;

    // we need to preserve the attributes explicitly because they are not
    // contained withing GNELane itself
    const NBEdge::Lane myLaneAttrs;
};

#endif
/****************************************************************************/
