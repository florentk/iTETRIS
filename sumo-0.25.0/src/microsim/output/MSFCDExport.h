/****************************************************************************/
/// @file    MSFCDExport.h
/// @author  Mario Krumnow
/// @date    2012-04-26
/// @version $Id: MSFCDExport.h 18287 2015-04-23 13:20:21Z namdre $
///
// Realises dumping Floating Car Data (FCD) Data
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSFCDExport_h
#define MSFCDExport_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSFCDExport
 * @brief Realises dumping FCD Data
 *
 *  The class offers a static method, which writes Position of each vehicle of
 *  the network into the given OutputDevice.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSFCDExport {
public:
    /** @brief Writes the posion and the angle of each vehcile into the given device
     *
     *  Opens the current time step and export the values vehicle id, position and angle
     *
     * @param[in] of The output device to use
     * @param[in] timestep The current time step
     * @param[in] elevation Whether elevation data shall be written
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void write(OutputDevice& of, SUMOTime timestep, bool elevation);


private:
    /// @brief Invalidated copy constructor.
    MSFCDExport(const MSFCDExport&);

    /// @brief Invalidated assignment operator.
    MSFCDExport& operator=(const MSFCDExport&);


};


#endif

/****************************************************************************/


