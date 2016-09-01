/****************************************************************************/
/// @file    ics-log.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <time.h>
#include <sstream>
#include <cstdlib>

#include "ics-log.h"
#include "../../../ics/sync-manager.h"
#include "../../common/TplConvert.h"

using namespace std;

namespace ics
{

IcsLog* IcsLog::instance_ = 0;
long IcsLog::lineCounter_ = 0;
ics::LogLevel IcsLog::logLevel_;
long IcsLog::timeStepThreshold_;
long IcsLog::nextTimeStepThreshold_;
int IcsLog::currentNumberLogFiles_ = 0;

IcsLog::IcsLog(string path, string timeThreshold)
{
    path_ = path;
    myfile_.open(path.c_str());
    logLevel_ = kLogLevelInfo;
    IcsLog::SetLogTimeThreshold(timeThreshold.c_str());
    nextTimeStepThreshold_ = timeStepThreshold_ - 1;
}

int
IcsLog::StartLog(string path, string timeThreshold)
{
    if (instance_ == 0) {
	instance_ = new IcsLog(path, timeThreshold);
	return EXIT_SUCCESS;
    }

    return -1;
}

void
IcsLog::SetLogLevel(ics::LogLevel logLevel)
{
    logLevel_ = logLevel;
}

void
IcsLog::SetLogTimeThreshold(char* logThreshold)
{
   try {
      timeStepThreshold_ = TplConvert<char>::_2long(logThreshold);   
   } catch (EmptyData e) {
      cout << "[INFO] Log time files maximum timestep set to 200."<< endl;
      timeStepThreshold_ = 200;
   }
}

void
IcsLog::SetLogTimeThreshold(string logThreshold)
{
   try {
      timeStepThreshold_ = TplConvert<char>::_2long(logThreshold.c_str());
   } catch (EmptyData e) {
      cout << "[INFO] Log time files maximum timestep set to 200."<< endl;
      timeStepThreshold_ = 200;
   }
}

void
IcsLog::Close()
{
    instance_->myfile_.close();
    delete instance_;
}

bool
IcsLog::Log(const char* message)
{
    // check the time step and create a new file if necessary
    if (timeStepThreshold_ != 0 && SyncManager::m_simStep == nextTimeStepThreshold_ ) {
	nextTimeStepThreshold_ += timeStepThreshold_;
	instance_->StartNewFile();
    }

    if (!instance_->myfile_.good()) {
        return false;
    }
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo == NULL) {
        cout << "[WARNING] Impossible to get time from system." << endl;
        return false;
    }

    // Removes \0 character
    stringstream auxi;
    auxi.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
    auxi << asctime(timeinfo);
    string mytime;
    try {
        getline(auxi, mytime);
    } catch (stringstream::failure e) {
        cout << "[WARNING] Impossible to format system time." << endl;
        return false;
    }

    instance_->myfile_ << "[" << mytime << "] " << "[" << SyncManager::m_simStep << "] " << message << endl;

    lineCounter_++;

    return true;
}

bool
IcsLog::LogLevel(const char* message, ics::LogLevel messageLogLevel)
{
    // check the time step and create a new file if necessary
    if (timeStepThreshold_ != 0 && SyncManager::m_simStep == nextTimeStepThreshold_ ) {
	nextTimeStepThreshold_ += timeStepThreshold_;
	instance_->StartNewFile();
    }

    // decides if the message will be written or not
    bool write = false;
    string levelName;

    // If level set to INFO write ALL messages
    if (logLevel_ == kLogLevelInfo) {
	write = true;
	levelName = "[INFO] ";
    }

    // If level set to WARNING write except if message leve is is INFO
    if (logLevel_ == kLogLevelWarning && messageLogLevel != kLogLevelInfo) {
	write = true;
	levelName = "[WARNING] ";
    }

    // If level set to ERROR and message IS ERROR write
    if ( (logLevel_ == kLogLevelError) && (messageLogLevel == kLogLevelError))  {
	write = true;	
	levelName = "[ERROR] ";
    }

    if (!write)
	return true; // Writing is canceled

    if (!instance_->myfile_.good()) {
        return false;
    }
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo == NULL) {
        cout << "[WARNING] Impossible to get time from system." << endl;
        return false;
    }

    // Removes \0 character
    stringstream auxi;
    auxi.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
    auxi << asctime(timeinfo);
    string mytime;
    try {
        getline(auxi, mytime);
    } catch (stringstream::failure e) {
        cout << "[WARNING] Impossible to format system time." << endl;
        return false;
    }

#ifdef LOG_ON
    instance_->myfile_ << "[" << mytime << "] " << "[" << SyncManager::m_simStep << "] " << levelName << message << endl;
#endif

    lineCounter_++;

    return true;
}

string
IcsLog::GetPath()
{
    return instance_->path_;
}

int
IcsLog::StartNewFile()
{
    instance_->myfile_.close();
    currentNumberLogFiles_++;
    string counter = utils::Conversion::int2String(currentNumberLogFiles_);
    string auxiPath = path_ + "-" + counter;    
    instance_->myfile_.open(auxiPath.c_str());    
    return EXIT_SUCCESS;
}


}
