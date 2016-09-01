/****************************************************************************/
/// @file    SUMOSAXHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: SUMOSAXHandler.h 10198 2011-05-02 05:53:47Z namdre $
///
// SAX-handler base for SUMO-files
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
#ifndef SUMOSAXHandler_h
#define SUMOSAXHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/GenericSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXHandler
 * @brief SAX-handler base for SUMO-files
 *
 * This class is a GenericSAXHandler which knows all tags SUMO uses, so all
 *  SUMO-XML - loading classes should be derived from it.
 *
 * Besides the functionality of GenericSAXHandler it
 *  implements methods for dealing with SAX-parser errors.
 */
class SUMOSAXHandler : public GenericSAXHandler {
public:
    /**
     * @brief Constructor
     *
     * @param[in] file The name of the processed file
     */
    SUMOSAXHandler(const std::string &file="");


    /// Destructor
    virtual ~SUMOSAXHandler();



private:

private:
    /// @brief invalidated copy constructor
    SUMOSAXHandler(const SUMOSAXHandler &s);

    /// @brief invalidated assignment operator
    const SUMOSAXHandler &operator=(const SUMOSAXHandler &s);

};


#endif

/****************************************************************************/

