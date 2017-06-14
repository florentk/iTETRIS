/****************************************************************************/
/// @file    OptionsIO.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
/// @version $Id: OptionsIO.cpp 24108 2017-04-27 18:43:30Z behrisch $
///
// Helper for parsing command line arguments and reading configuration files
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

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include "OptionsIO.h"
#include "OptionsCont.h"
#include "OptionsLoader.h"
#include "OptionsParser.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>

// ===========================================================================
// static member definitions
// ===========================================================================
int OptionsIO::myArgC;
char** OptionsIO::myArgV;


// ===========================================================================
// method definitions
// ===========================================================================
void
OptionsIO::setArgs(int argc, char** argv) {
    myArgC = argc;
    myArgV = argv;
}


void
OptionsIO::setArgs(const std::vector<std::string>& args) {
    myArgC = (int)args.size() + 1;
    char** argv = new char* [myArgC];
    argv[0] = myArgV[0];
    for (int i = 1; i < myArgC; i++) {
        argv[i] = new char[args[i - 1].size() + 1];
        std::strcpy(argv[i], args[i - 1].c_str());
    }
    myArgV = argv;
}


void
OptionsIO::getOptions(const bool commandLineOnly) {
    if (myArgC == 2 && myArgV[1][0] != '-') {
        // special case only one parameter, check who can handle it
        if (OptionsCont::getOptions().setByRootElement(getRoot(myArgV[1]), myArgV[1])) {
            loadConfiguration();
            return;
        }
    }
    // preparse the options
    //  (maybe another configuration file was chosen)
    if (!OptionsParser::parse(myArgC, myArgV)) {
        throw ProcessError("Could not parse commandline options.");
    }
    if (!commandLineOnly) {
        // read the configuration when everything's ok
        OptionsCont::getOptions().resetWritable();
        loadConfiguration();
        // reparse the options
        //  (overwrite the settings from the configuration file)
        OptionsCont::getOptions().resetWritable();
        OptionsParser::parse(myArgC, myArgV);
    }
}


void
OptionsIO::loadConfiguration() {
    OptionsCont& oc = OptionsCont::getOptions();
    if (!oc.exists("configuration-file") || !oc.isSet("configuration-file")) {
        return;
    }
    std::string path = oc.getString("configuration-file");
    if (!FileHelpers::isReadable(path)) {
        throw ProcessError("Could not access configuration '" + oc.getString("configuration-file") + "'.");
    }
    PROGRESS_BEGIN_MESSAGE("Loading configuration");
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Auto);
    parser.setDoNamespaces(false);
    parser.setDoSchema(false);
    // start the parsing
    OptionsLoader handler;
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        parser.parse(path.c_str());
        if (handler.errorOccured()) {
            throw ProcessError("Could not load configuration '" + path + "'.");
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        throw ProcessError("Could not load configuration '" + path + "':\n " + TplConvert::_2str(e.getMessage()));
    }
    oc.relocateFiles(path);
    PROGRESS_DONE_MESSAGE();
}


std::string
OptionsIO::getRoot(const std::string& filename) {
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    // start the parsing
    OptionsLoader handler;
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        XERCES_CPP_NAMESPACE::XMLPScanToken token;
        if (!parser.parseFirst(filename.c_str(), token)) {
            throw ProcessError("Can not read XML-file '" + filename + "'.");
        }
        while (parser.parseNext(token) && handler.getItem() == "");
        if (handler.errorOccured()) {
            throw ProcessError("Could not load '" + filename + "'.");
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        throw ProcessError("Could not load '" + filename + "':\n " + TplConvert::_2str(e.getMessage()));
    }
    return handler.getItem();
}


/****************************************************************************/

