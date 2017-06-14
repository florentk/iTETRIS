/****************************************************************************/
/// @file    Bresenham.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 07 Jun 2002
/// @version $Id: Bresenham.cpp 23529 2017-03-18 10:22:36Z behrisch $
///
// A class to realise a uniform n:m - relationship using the
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

#include <iostream>
#include <utils/common/StdDefs.h>
#include "Bresenham.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
Bresenham::compute(BresenhamCallBack* callBack, const int val1, const int val2) {
    const int smaller = MIN2(val1, val2);
    const int greater = MAX2(val1, val2);
    int pos = 0;
    int c = smaller;
    for (int i = 0; i < greater; i++) {
        if (smaller == val1) {
            callBack->execute(pos, i);
        } else {
            callBack->execute(i, pos);
        }
        c += 2 * smaller;
        if (c >= 2 * greater) {
            pos++;
            c -= 2 * greater;
        }
    }
}



/****************************************************************************/

