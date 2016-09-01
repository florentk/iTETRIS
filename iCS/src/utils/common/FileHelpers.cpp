/****************************************************************************/
/// @file    FileHelpers.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id: FileHelpers.cpp 9525 2011-01-04 21:22:52Z behrisch $
///
// Functions for an easier usage of files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include "FileHelpers.h"
#include "StringTokenizer.h"
#include "MsgHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// file access functions
// ---------------------------------------------------------------------------
bool
FileHelpers::exists(std::string path) {
    if (path.length()==0) {
        return false;
    }
    while (path[path.length()-1]=='/'||path[path.length()-1]=='\\') {
        path.erase(path.end()-1);
    }
    if (path.length()==0) {
        return false;
    }
    struct stat st;
    bool ret = (stat(path.c_str(), &st) == 0);
    return ret;
}


// ---------------------------------------------------------------------------
// file path evaluating functions
// ---------------------------------------------------------------------------
std::string
FileHelpers::getFilePath(const std::string &path) {
    size_t beg = path.find_last_of("\\/");
    if (beg==std::string::npos||beg==0) {
        return "";
    }
    return path.substr(0, beg+1);
}


std::string
FileHelpers::getConfigurationRelative(const std::string &configPath,
                                      const std::string &path) {
    std::string retPath = getFilePath(configPath);
    return retPath + path;
}


bool
FileHelpers::isSocket(const std::string &name) {
    size_t colonPos = name.find(":");
    return (colonPos != std::string::npos) && (colonPos > 1);
}


bool
FileHelpers::isAbsolute(const std::string &path) {
    if (isSocket(path)) {
        return true;
    }
    // check UNIX - absolute paths
    if (path.length()>0&&path[0]=='/') {
        return true;
    }
    // check Windows - absolute paths
    if (path.length()>0&&path[0]=='\\') {
        return true;
    }
    if (path.length()>1&&path[1]==':') {
        return true;
    }
    if (path=="nul"||path=="NUL") {
        return true;
    }
    return false;
}


std::string
FileHelpers::checkForRelativity(std::string filename,
                                const std::string &basePath) {
    if (!isAbsolute(filename)) {
        filename = getConfigurationRelative(basePath, filename);
    }
    return filename;
}


// ---------------------------------------------------------------------------
// binary reading/writing functions
// ---------------------------------------------------------------------------
std::ostream &
FileHelpers::writeInt(std::ostream &strm, int value) {
    strm.write((char*) &value, sizeof(int));
    return strm;
}


std::ostream &
FileHelpers::writeUInt(std::ostream &strm, unsigned int value) {
    strm.write((char*) &value, sizeof(unsigned int));
    return strm;
}


std::ostream &
FileHelpers::writeFloat(std::ostream &strm, SUMOReal value) {
    strm.write((char*) &value, sizeof(SUMOReal));
    return strm;
}


std::ostream &
FileHelpers::writeByte(std::ostream &strm, unsigned char value) {
    strm.write((char*) &value, sizeof(char));
    return strm;
}


std::ostream &
FileHelpers::writeString(std::ostream &strm, const std::string &value) {
    size_t size = value.length();
    const char *cstr = value.c_str();
    writeUInt(strm, (unsigned int) size);
    strm.write((char*) cstr, (std::streamsize)(sizeof(char)*size));
    return strm;
}


std::ostream &
FileHelpers::writeTime(std::ostream &strm, SUMOTime value) {
    strm.write((char*) &value, sizeof(SUMOTime));
    return strm;
}


/****************************************************************************/

