/****************************************************************************/
/// @file    NWWriter_OpenDrive.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id: NWWriter_OpenDrive.h 19443 2015-11-26 14:16:12Z namdre $
///
// Exporter writing networks using the openDRIVE format
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
#ifndef NWWriter_OpenDrive_h
#define NWWriter_OpenDrive_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StringBijection.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class NBEdge;
class OptionsCont;
class PositionVector;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NWWriter_OpenDrive
 * @brief Exporter writing networks using the openDRIVE format
 *
 */
class NWWriter_OpenDrive {
public:
    /** @brief Writes the network into a openDRIVE-file
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void writeNetwork(const OptionsCont& oc, NBNetBuilder& nb);

protected:
    static void writePlanView(const PositionVector& shape, OutputDevice& device);
    static void writeEmptyCenterLane(OutputDevice& device, const std::string& mark, SUMOReal markWidth);
    static int getID(const std::string& origID, StringBijection<int>& map, int& lastID);

    static std::string getLaneType(SVCPermissions permissions);

    /// @brief get the left border of the leftmost lane
    static PositionVector getLeftBorder(const NBEdge* edge);
};


#endif

/****************************************************************************/

