/****************************************************************************/
/// @file    GUIDialog_AboutSUMO.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id: GUIDialog_AboutSUMO.h 22743 2017-01-29 11:07:55Z palcraft $
///
// The application's "About" - dialog
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
#ifndef GUIDialog_AboutSUMO_h
#define GUIDialog_AboutSUMO_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_AboutSUMO
 * @brief The application's "About" - dialog
 */
class GUIDialog_AboutSUMO : public FXDialogBox {
public:
    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GUIDialog_AboutSUMO(FXWindow* parent);

    /// @brief Destructor
    ~GUIDialog_AboutSUMO();

    /// @brief Creates the widget
    void create();

private:
    /// @brief Font for the widget
    FXFont* myHeadlineFont;
};


#endif

/****************************************************************************/

