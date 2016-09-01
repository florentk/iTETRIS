/****************************************************************************/
/// @file    GNEEvent_NetworkLoaded.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id: GNEEvent_NetworkLoaded.h 18845 2015-09-17 12:50:55Z behrisch $
///
// Event to send when the network has been loaded by GNELoadThread
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
#ifndef GNEEvent_NetworkLoaded_h
#define GNEEvent_NetworkLoaded_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/gui/events/GUIEvent.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class  GNEEvent_NetworkLoaded
 *
 * Throw to GUIApplicationWindow from GUILoadThread after a simulation has
 * been loaded or the loading process failed
 */
class GNEEvent_NetworkLoaded : public GUIEvent {
public:
    /// constructor
    GNEEvent_NetworkLoaded(GNENet* net,
                           const std::string& file,
                           const std::string& settingsFile,
                           const bool viewportFromRegistry)
        : GUIEvent(EVENT_SIMULATION_LOADED),
          myNet(net), myFile(file), mySettingsFile(settingsFile), myViewportFromRegistry(viewportFromRegistry) { }

    /// destructor
    ~GNEEvent_NetworkLoaded() { }

public:
    /// the loaded net
    GNENet*          myNet;

    /// the name of the loaded file
    std::string     myFile;

    /// the name of the settings file to load
    std::string     mySettingsFile;

    /// whether loading viewport from registry
    bool myViewportFromRegistry;
};


#endif

/****************************************************************************/

