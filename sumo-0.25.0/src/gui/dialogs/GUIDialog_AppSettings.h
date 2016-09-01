/****************************************************************************/
/// @file    GUIDialog_AppSettings.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 08.03.2004
/// @version $Id: GUIDialog_AppSettings.h 18841 2015-09-17 10:43:01Z behrisch $
///
// The application-settings dialog
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIDialog_AppSettings_h
#define GUIDialog_AppSettings_h


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
 * @class GUIDialog_AppSettings
 * @brief The dialog to change the application (gui) settings.
 */
class GUIDialog_AppSettings : public FXDialogBox {
    // is a FOX-object with an own mapping
    FXDECLARE(GUIDialog_AppSettings)

public:
    /** @brief Constructor
     *
     * @param[in] parent The parent window
     */
    GUIDialog_AppSettings(FXMainWindow* parent);

    /// @brief Destructor
    ~GUIDialog_AppSettings();



    /// @name FOX-callbacks
    /// @{

    /// @brief Called on OK-button pressure
    long onCmdOk(FXObject*, FXSelector, void*);

    /// @brief Called on Cancel-button pressure
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief Called on "Quit on end"-button change
    long onCmdQuitOnEnd(FXObject*, FXSelector, void*);

    /// @brief Called on "Allow textures"-button change
    long onCmdAllowTextures(FXObject*, FXSelector, void*);

    /// @brief Called on "Locate links in messages"-button change
    long onCmdLocateLinks(FXObject*, FXSelector, void*);
    /// @}


private:
    /// @brief Information whether the application shall be quit
    bool myAppQuitOnEnd;

    /// @brief Information whether textures may be used
    bool myAllowTextures;

    /// @brief Information whether locate links appear in messages
    bool myLocateLinks;


protected:
    /// default constructor (needed by FOX)
    GUIDialog_AppSettings() { }

};


#endif

/****************************************************************************/

