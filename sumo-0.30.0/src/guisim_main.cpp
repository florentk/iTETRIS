/****************************************************************************/
/// @file    guisim_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Felix Brack
/// @date    Tue, 20 Nov 2001
/// @version $Id: guisim_main.cpp 23637 2017-03-23 20:40:16Z behrisch $
///
// Main for GUISIM
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <ctime>
#include <signal.h>
#include <iostream>
#include <microsim/MSFrame.h>
#include <microsim/MSNet.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/xml/XMLSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>

#ifndef NO_TRACI
#include <traci-server/TraCIServer.h>
#endif


// ===========================================================================
// main function
// ===========================================================================
int
main(int argc, char** argv) {
    // make the output aware of threading
    MFXMutex lock;
    MsgHandler::assignLock(&lock);
    // get the options
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("GUI version of the simulation SUMO.");
    oc.setApplicationName("sumo-gui", "SUMO gui Version " VERSION_STRING);
    int ret = 0;
    try {
        // initialise subsystems
        XMLSubSys::init();
        MSFrame::fillOptions();
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions(true);
        if (oc.processMetaOptions(false)) {
            SystemFrame::close();
            return 0;
        }
        // Make application
        FXApp application("SUMO GUISimulation", "DLR");
        // Open display
        application.init(argc, argv);
        int minor, major;
        if (!FXGLVisual::supported(&application, major, minor)) {
            throw ProcessError("This system has no OpenGL support. Exiting.");
        }

        // build the main window
        GUIApplicationWindow* window =
            new GUIApplicationWindow(&application, "*.sumo.cfg,*.sumocfg");
        gSchemeStorage.init(&application);
        window->dependentBuild();
        // Create app
        application.addSignal(SIGINT, window, MID_QUIT);
        application.create();
        // Load configuration given on command line
        if (argc > 1) {
            window->loadOnStartup();
        }
        // Run
        ret = application.run();
    } catch (const ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (const std::exception& e) {
        if (std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
#ifndef NO_TRACI
    TraCIServer::close();
#endif
    SystemFrame::close();
    return ret;
}


/****************************************************************************/
