/****************************************************************************/
/// @file    TraCIServerAPI_MultiEntryExit.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_MultiEntryExit.h 24028 2017-04-24 05:10:18Z behrisch $
///
// APIs for getting/setting multi-entry/multi-exit detector values via TraCI
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
#ifndef TraCIServerAPI_MultiEntryExit_h
#define TraCIServerAPI_MultiEntryExit_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include "TraCIDefs.h"
#include "TraCIServer.h"
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_MultiEntryExit
 * @brief APIs for getting/setting multi-entry/multi-exit detector values via TraCI
 */
class TraCIServerAPI_MultiEntryExit {
public:
    /** @brief Processes a get value command (Command 0xa1: Get MeMeDetector Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_MultiEntryExit(const TraCIServerAPI_MultiEntryExit& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_MultiEntryExit& operator=(const TraCIServerAPI_MultiEntryExit& s);


};


#endif

#endif

/****************************************************************************/

