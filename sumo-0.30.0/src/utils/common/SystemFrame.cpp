/****************************************************************************/
/// @file    SystemFrame.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 23.06.2003
/// @version $Id: SystemFrame.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A set of actions common to all applications
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

#include "SystemFrame.h"
#include <string>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "RandHelper.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
SystemFrame::addConfigurationOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Configuration");

    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("configuration-file", "configuration");
    oc.addDescription("configuration-file", "Configuration", "Loads the named config on startup");
    oc.addXMLDefault("configuration-file");

    oc.doRegister("save-configuration", new Option_FileName());
    oc.addSynonyme("save-config", "save-configuration");
    oc.addDescription("save-configuration", "Configuration", "Saves current configuration into FILE");

    oc.doRegister("save-template", new Option_FileName());
    oc.addDescription("save-template", "Configuration", "Saves a configuration template (empty) into FILE");

    oc.doRegister("save-schema", new Option_FileName());
    oc.addDescription("save-schema", "Configuration", "Saves the configuration schema into FILE");

    oc.doRegister("save-commented", new Option_Bool(false));
    oc.addSynonyme("save-commented", "save-template.commented");
    oc.addDescription("save-commented", "Configuration", "Adds comments to saved template, configuration, or schema");
}


void
SystemFrame::addReportOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Report");

    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.addDescription("verbose", "Report", "Switches to verbose output");

    oc.doRegister("print-options", new Option_Bool(false));
    oc.addDescription("print-options", "Report", "Prints option values before processing");

    oc.doRegister("help", '?', new Option_Bool(false));
    oc.addDescription("help", "Report", "Prints this screen");

    oc.doRegister("version", 'V', new Option_Bool(false));
    oc.addDescription("version", "Report", "Prints the current version");

    oc.doRegister("xml-validation", 'X', new Option_String("auto"));
    oc.addDescription("xml-validation", "Report", "Set schema validation scheme of XML inputs (\"never\", \"auto\" or \"always\")");

    oc.doRegister("xml-validation.net", new Option_String("never"));
    oc.addDescription("xml-validation.net", "Report", "Set schema validation scheme of SUMO network inputs (\"never\", \"auto\" or \"always\")");

    oc.doRegister("no-warnings", 'W', new Option_Bool(false));
    oc.addSynonyme("no-warnings", "suppress-warnings", true);
    oc.addDescription("no-warnings", "Report", "Disables output of warnings");

    oc.doRegister("log", 'l', new Option_FileName());
    oc.addSynonyme("log", "log-file");
    oc.addDescription("log", "Report", "Writes all messages to FILE (implies verbose)");

    oc.doRegister("message-log", new Option_FileName());
    oc.addDescription("message-log", "Report", "Writes all non-error messages to FILE (implies verbose)");

    oc.doRegister("error-log", new Option_FileName());
    oc.addDescription("error-log", "Report", "Writes all warnings and errors to FILE");

    oc.doRegister("output-prefix", new Option_String());
    oc.addDescription("output-prefix", "Output", "Prefix which is applied to all output files. The special string 'TIME' is replaced by the current time.");

    oc.doRegister("precision", new Option_Integer(2));
    oc.addDescription("precision", "Output", "Defines the number of digits after the comma for floating point output");

    oc.doRegister("precision.geo", new Option_Integer(6));
    oc.addDescription("precision.geo", "Output", "Defines the number of digits after the comma for lon,lat output");
}


bool
SystemFrame::checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    gPrecision = oc.getInt("precision");
    gPrecisionGeo = oc.getInt("precision.geo");
    return true;
}


void
SystemFrame::close() {
    // close all output devices
    OutputDevice::closeAll();
    // close the xml-subsystem
    XMLSubSys::close();
    // delete build program options
    OptionsCont::getOptions().clear();
    // delete messages
    MsgHandler::cleanupOnEnd();
}


/****************************************************************************/

