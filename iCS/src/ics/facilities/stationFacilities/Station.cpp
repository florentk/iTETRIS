/****************************************************************************/
/// @file    Station.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Apr 15, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Station.h"

namespace ics_facilities {

Station::Station() {
    position.set(-1, -1);
}

Station::~Station() {
    if (!RATs.empty())
        RATs.clear();
}

stationID_t Station::getID() {
    return ID;
}

icsstationtype_t Station::getType() const {
    return type;
}

const Point2D& Station::getPosition() const {
    return position;
}

vector<RATID>* Station::getRATs() {
    vector<RATID> *vrats = new vector<RATID>;
    vector< pair<RATID, bool> >::iterator it;
    for (it = RATs.begin(); it != RATs.end(); it++)
        vrats->push_back(it->first);
    return vrats;
}

vector<RATID>* Station::getActiveRATs() {
    vector<RATID> *vrats = new vector<RATID>;
    vector< pair<RATID, bool> >::iterator it;
    for (it = RATs.begin(); it != RATs.end(); it++)
        if (it->second)
            vrats->push_back(it->first);
    return vrats;
}

void Station::setPosition(Point2D position) {
    this->position = position;
}

void Station::setRATs(vector< pair<RATID, bool> > RATs) {
    if (!this->RATs.empty())
        this->RATs.clear();
    this->RATs = RATs;
    return;
}

bool Station::enableRAT(RATID toEnable) {
    vector< pair<RATID, bool> >::iterator it;
    for (it = RATs.begin(); it != RATs.end(); it++)
        if (it->first == toEnable) {
            it->second = true;
            return true;
        }
    return false;
}

bool Station::disableRAT(RATID toDisable) {
    vector< pair<RATID, bool> >::iterator it;
    for (it = RATs.begin(); it != RATs.end(); it++)
        if (it->first == toDisable) {
            it->second = false;
            return true;
        }
    return false;
}

}
