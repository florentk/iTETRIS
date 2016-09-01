/****************************************************************************/
/// @file    MsgHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 17 Jun 2003
/// @version $Id: MsgHandler.cpp 10297 2011-05-11 12:09:41Z behrisch $
///
// Retrieves messages about the process and gives them further to output
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
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#include "MsgHandler.h"
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include "AbstractMutex.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// global variable definitions
// ===========================================================================
bool gSuppressWarnings = false;
bool gSuppressMessages = false;


// ===========================================================================
// static member variables
// ===========================================================================
MsgHandler *MsgHandler::myErrorInstance = 0;
MsgHandler *MsgHandler::myWarningInstance = 0;
MsgHandler *MsgHandler::myMessageInstance = 0;
bool MsgHandler::myAmProcessingProcess = false;
AbstractMutex *MsgHandler::myLock = 0;


// ===========================================================================
// method definitions
// ===========================================================================
MsgHandler *
MsgHandler::getMessageInstance() {
    if (myMessageInstance==0) {
        myMessageInstance = new MsgHandler(MT_MESSAGE);
    }
    return myMessageInstance;
}


MsgHandler *
MsgHandler::getWarningInstance() {
    if (myWarningInstance==0) {
        myWarningInstance = new MsgHandler(MT_WARNING);
    }
    return myWarningInstance;
}


MsgHandler *
MsgHandler::getErrorInstance() {
    if (myErrorInstance==0) {
        myErrorInstance = new MsgHandler(MT_ERROR);
    }
    return myErrorInstance;
}


void
MsgHandler::inform(std::string msg, bool addType) {
    if (myLock!=0) {
        myLock->lock();
    }
    // beautify progress output
    if (myAmProcessingProcess && (myReport2COUT || myReport2CERR)) {
        std::cout << std::endl;
    }
    msg = build(msg, addType);
    // report to std::cout if wished
    if (myReport2COUT) {
        std::cout << msg << std::endl;
    }
    // report to std::cerr if wished
    if (myReport2CERR) {
        std::cerr << msg << std::endl;
    }
    // inform all other receivers
    for (RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(msg);
    }
    // set the information that something occured
    myWasInformed = true;
    myAmProcessingProcess = false;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::progressMsg(std::string msg, bool addType) {
    if (myLock!=0) {
        myLock->lock();
    }
    // beautify progress output
    if (myAmProcessingProcess && (myReport2COUT || myReport2CERR)) {
        std::cout << std::endl;
    }
    msg = build(msg, addType);
    // report to std::cout if wished
    if (myReport2COUT) {
        std::cout << msg << '\r';
    }
    // report to std::cerr if wished
    if (myReport2CERR) {
        std::cerr << msg << '\r';
    }
    // inform all other receivers
    for (RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(msg);
    }
    // set the information that something occured
    myWasInformed = true;
    myAmProcessingProcess = false;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::beginProcessMsg(std::string msg, bool addType) {
    if (myLock!=0) {
        myLock->lock();
    }
    msg = build(msg, addType);
    // report to std::cout if wished
    if (myReport2COUT) {
        std::cout << msg << ' ';
        std::cout.flush();
        myAmProcessingProcess = true;
    }
    // report to std::cerr if wished
    if (myReport2CERR) {
        std::cerr << msg << ' ';
        std::cerr.flush();
        myAmProcessingProcess = true;
    }
    // inform all other receivers
    for (RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(msg + " ");
        myAmProcessingProcess = true;
    }
    // set the information that something occured
    myWasInformed = true;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::endProcessMsg(std::string msg) {
    if (myLock!=0) {
        myLock->lock();
    }
    // report to std::cout if wished
    if (myReport2COUT) {
        std::cout << msg << std::endl;
    }
    // report to std::cerr if wished
    if (myReport2CERR) {
        std::cerr << msg << std::endl;
    }
    // inform all other receivers
    for (RetrieverVector::iterator i=myRetrievers.begin(); i!=myRetrievers.end(); i++) {
        (*i)->inform(msg);
    }
    // set the information that something occured
    myWasInformed = true;
    myAmProcessingProcess = false;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::clear() {
    if (myLock!=0) {
        myLock->lock();
    }
    myWasInformed = false;
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::addRetriever(OutputDevice *retriever) {
    if (myLock!=0) {
        myLock->lock();
    }
    RetrieverVector::iterator i =
        find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if (i==myRetrievers.end()) {
        myRetrievers.push_back(retriever);
    }
    // check whether the message shall be generated
    if (myType==MT_WARNING) {
        gSuppressWarnings = OptionsCont::getOptions().exists("no-warnings")
                            ? OptionsCont::getOptions().getBool("no-warnings")
                            : false;
    } else if (myType==MT_MESSAGE) {
        gSuppressMessages = false;
    }
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::removeRetriever(OutputDevice *retriever) {
    if (myLock!=0) {
        myLock->lock();
    }
    RetrieverVector::iterator i =
        find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if (i!=myRetrievers.end()) {
        myRetrievers.erase(i);
    }
    // check whether the message shall be generated
    if (myType==MT_WARNING) {
        gSuppressWarnings = OptionsCont::getOptions().exists("no-warnings")
                            ? OptionsCont::getOptions().getBool("no-warnings")
                            : myRetrievers.size()==0;
    } else if (myType==MT_MESSAGE) {
        gSuppressMessages = !(myRetrievers.size()==0||myReport2COUT);
    }
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::report2cout(bool value) {
    if (myLock!=0) {
        myLock->lock();
    }
    myReport2COUT = value;
    if (myType==MT_WARNING) {
        gSuppressWarnings = OptionsCont::getOptions().exists("no-warnings")
                            ? OptionsCont::getOptions().getBool("no-warnings")
                            : !myReport2COUT;
    } else if (myType==MT_MESSAGE) {
        gSuppressMessages = myRetrievers.size()==0&&!myReport2COUT;
    }
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::report2cerr(bool value) {
    if (myLock!=0) {
        myLock->lock();
    }
    myReport2CERR = value;
    if (myType==MT_WARNING) {
        gSuppressWarnings = OptionsCont::getOptions().exists("no-warnings")
                            ? OptionsCont::getOptions().getBool("no-warnings")
                            : !myReport2CERR;
    } else if (myType==MT_MESSAGE) {
        gSuppressMessages = myRetrievers.size()==0&&!myReport2COUT;
    }
    std::cerr.setf(std::ios::fixed, std::ios::floatfield);
    if (myLock!=0) {
        myLock->unlock();
    }
}


void
MsgHandler::initOutputOptions(bool gui) {
    OptionsCont& oc = OptionsCont::getOptions();
    getMessageInstance()->report2cout(!gui && oc.getBool("verbose"));
    getWarningInstance()->report2cerr(!gui && !oc.getBool("no-warnings"));
    // build the logger if possible
    if (oc.isSet("log")) {
        try {
            OutputDevice *logFile = &OutputDevice::getDevice(oc.getString("log"));
            getErrorInstance()->addRetriever(logFile);
            getWarningInstance()->addRetriever(logFile);
            getMessageInstance()->addRetriever(logFile);
        } catch (IOError &) {
            throw ProcessError("Could not build logging file '" + oc.getString("log") + "'");
        }
    }
    if (oc.isSet("message-log")) {
        try {
            OutputDevice *logFile = &OutputDevice::getDevice(oc.getString("message-log"));
            getMessageInstance()->addRetriever(logFile);
        } catch (IOError &) {
            throw ProcessError("Could not build logging file '" + oc.getString("message-log") + "'");
        }
    }
    if (oc.isSet("error-log")) {
        try {
            OutputDevice *logFile = &OutputDevice::getDevice(oc.getString("error-log"));
            getErrorInstance()->addRetriever(logFile);
            getWarningInstance()->addRetriever(logFile);
        } catch (IOError &) {
            throw ProcessError("Could not build logging file '" + oc.getString("error-log") + "'");
        }
    }
}


void
MsgHandler::cleanupOnEnd() {
    if (myLock!=0) {
        myLock->lock();
    }
    delete myMessageInstance;
    myMessageInstance = 0;
    delete myWarningInstance;
    myWarningInstance = 0;
    delete myErrorInstance;
    myErrorInstance = 0;
    if (myLock!=0) {
        myLock->unlock();
    }
}


MsgHandler::MsgHandler(MsgType type)
        : myType(type), myWasInformed(false), myReport2COUT(type==MT_MESSAGE),
        myReport2CERR(type!=MT_MESSAGE) {}


MsgHandler::~MsgHandler() {
}


bool
MsgHandler::wasInformed() const {
    return myWasInformed;
}


void
MsgHandler::assignLock(AbstractMutex *lock) {
    assert(myLock==0);
    myLock = lock ;
}



/****************************************************************************/

