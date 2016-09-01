/****************************************************************************/
/// @file    MFXMenuHeader.h
/// @author  Daniel Krajzewicz
/// @date    2004-07-02
/// @version $Id: MFXMenuHeader.h 18095 2015-03-17 09:39:00Z behrisch $
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MFXMenuHeader_h
#define MFXMenuHeader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>

class MFXMenuHeader : public FXMenuCommand {
public:
    MFXMenuHeader(FXComposite* p, FXFont* fnt, const FXString& text, FXIcon* ic = NULL,
                  FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = 0);
    ~MFXMenuHeader();
};


#endif

/****************************************************************************/

