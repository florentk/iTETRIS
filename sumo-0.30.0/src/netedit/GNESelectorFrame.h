/****************************************************************************/
/// @file    GNESelectorFrame.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id: GNESelectorFrame.h 24108 2017-04-27 18:43:30Z behrisch $
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
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
#ifndef GNESelectorFrame_h
#define GNESelectorFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNESelectorFrame
 * The Widget for modifying selections of network-elements
 */
class GNESelectorFrame : public GNEFrame, public GUISelectedStorage::UpdateTarget {
    /// @brief FOX-declaration
    FXDECLARE(GNESelectorFrame)

public:
    /// @brief operations of selector
    enum SetOperation {
        SET_ADD      = 1,
        SET_SUB      = 2,
        SET_RESTRICT = 3,
        SET_REPLACE  = 4,
        SET_DEFAULT  = 5  // use mySetOperation instead of override
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNESelectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNESelectorFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user change type of selction operation
    long onCmdSelectOperation(FXObject*, FXSelector, void*);

    /// @brief Called when the user change the type of element to search (netElement or Additional)
    long onCmdSubset(FXObject*, FXSelector, void*);

    /**@brief Called when the user presses the Load-button
     * @note Opens a file dialog and forces the parent to load the list of selected
     * objects when a file was chosen. Rebuilds the list, then, and redraws itself.
     */
    long onCmdLoad(FXObject*, FXSelector, void*);

    /** @brief Called when the user presses the Save-button
     * @note Opens a file dialog and forces the selection container to save the list
       of selected objects when a file was chosen. If the saveing failed, a message window is shown.
     */
    long onCmdSave(FXObject*, FXSelector, void*);

    /**@brief Called when the user presses the Clear-button
     * @note Clear the internal list and calls GUISelectedStorage::clear and repaints itself
     */
    long onCmdClear(FXObject*, FXSelector, void*);

    /**@brief Called when the user presses the Invert-button
     * @note invert the selection and repaints itself
     */
    long onCmdInvert(FXObject*, FXSelector, void*);

    /**@brief Called when the user selectes a tag in the match box
     * @note updates the attr listbox and repaints itself
     */
    long onCmdSelMBTag(FXObject*, FXSelector, void*);

    /**@brief Called when the user selectes a tag in the match box
     * @note updates the attr listbox and repaints itself
     */
    long onCmdSelMBAttribute(FXObject*, FXSelector, void*);

    /**@brief Called when the user enters a new selection expression
     * @note validates expression and modifies current selection
     */
    long onCmdSelMBString(FXObject*, FXSelector, void*);

    /**@brief Called when the user clicks the help button
     * @note pop up help window
     */
    long onCmdHelp(FXObject*, FXSelector, void*);

    /// @brief Called when the user changes visual scaling
    long onCmdScaleSelection(FXObject*, FXSelector, void*);
    /// @}

    /**@brief apply list of ids to the current selection according to SetOperation,
     * @note if setop==SET_DEFAULT than the currently set mode (mySetOperation) is used
     */
    void handleIDs(std::vector<GUIGlID> ids, bool selectEdgesEnabled, SetOperation setop = SET_DEFAULT);

    /// @brief called if currently registered for updates for changes of global selection
    void selectionUpdated();

protected:
    /// @brief FOX needs this
    GNESelectorFrame() {}

private:
    /// @brief how to modify selection
    SetOperation mySetOperation;

    /// @brief add radio button
    FXRadioButton* myAddRadioButton;

    /// @brief remove radio button
    FXRadioButton* myRemoveRadioButton;

    /// @brief keep button
    FXRadioButton* myKeepRadioButton;

    /// @brief replace radio button
    FXRadioButton* myReplaceRadioButton;

    /// @brief tag of the sets of elements
    FXComboBox* mySetComboBox;

    /// @brief tag of the match box
    FXComboBox* myMatchTagComboBox;

    /// @brief attributes of the match box
    FXComboBox* myMatchAttrComboBox;

    /// @brief string of the match
    FXTextField* myMatchString;

    /// @brief selection scaling
    FXRealSpinDial* mySelectionScaling;

    /// @brief current SumoXMLTag tag
    SumoXMLTag myCurrentTag;

    /// @brief current SumoXMLTag Attribute
    SumoXMLAttr myCurrentAttribute;

private:
    /// @brief get stats
    std::string getStats() const;

    /**@brief return  objects of the given type with matching attrs
     * @param[in] compOp One of {<,>,=} for matching against val or '@' for matching against expr
     */
    std::vector<GUIGlID> getMatches(SumoXMLTag tag, SumoXMLAttr attr, char compOp, double val, const std::string& expr);

    /// @brief the string that should be matched against if attr 'allowed' or 'disallowed' are set to "all"
    const std::string ALL_VCLASS_NAMES_MATCH_STRING;
};


#endif

/****************************************************************************/

