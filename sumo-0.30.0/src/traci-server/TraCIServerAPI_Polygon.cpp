/****************************************************************************/
/// @file    TraCIServerAPI_Polygon.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Gregor L\"ammel
/// @date    Sept 2002
/// @version $Id: TraCIServerAPI_Polygon.cpp 24108 2017-04-27 18:43:30Z behrisch $
///
// APIs for getting/setting polygon values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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

#ifndef NO_TRACI

#include <utils/common/StdDefs.h>
#include <microsim/MSNet.h>
#include <utils/shapes/Polygon.h>
#include <utils/shapes/ShapeContainer.h>
#include <traci-server/lib/TraCI_Polygon.h>
#include <traci-server/lib/TraCI.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Polygon.h"

// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Polygon::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != VAR_TYPE && variable != VAR_COLOR && variable != VAR_SHAPE && variable != VAR_FILL
            && variable != ID_COUNT && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE,
                                          "Get Polygon Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_POLYGON_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable == ID_LIST || variable == ID_COUNT) {
        std::vector<std::string> ids = TraCI_Polygon::getIDList();
        if (variable == ID_LIST) {
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(ids);
        } else {
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt((int) ids.size());
        }
    } else {
        try {
            switch (variable) {
                case VAR_TYPE: {
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(TraCI_Polygon::getType(id));
                }
                break;
                case VAR_COLOR: {
                    TraCIColor tc = TraCI_Polygon::getColor(id);
                    tempMsg.writeUnsignedByte(TYPE_COLOR);
                    tempMsg.writeUnsignedByte(tc.r);
                    tempMsg.writeUnsignedByte(tc.g);
                    tempMsg.writeUnsignedByte(tc.b);
                    tempMsg.writeUnsignedByte(tc.a);
                }
                break;
                case VAR_SHAPE: {
                    tempMsg.writeUnsignedByte(TYPE_POLYGON);
                    TraCIPositionVector tp = TraCI_Polygon::getShape(id);
                    tempMsg.writeUnsignedByte((int) tp.size());
                    for (int iPoint = 0; iPoint < (int)tp.size(); ++iPoint) {
                        tempMsg.writeDouble(tp[iPoint].x);
                        tempMsg.writeDouble(tp[iPoint].y);
                    }
                }
                break;
                case VAR_FILL: {
                    tempMsg.writeUnsignedByte(TYPE_UBYTE);
                    tempMsg.writeUnsignedByte(TraCI_Polygon::getFilled(id) ? 1 : 0);
                }
                break;
                case VAR_PARAMETER: {
                    std::string paramName = "";
                    if (!server.readTypeCheckingString(inputStorage, paramName)) {
                        return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                    }
                    tempMsg.writeUnsignedByte(TYPE_STRING);
                    tempMsg.writeString(TraCI_Polygon::getParameter(id, paramName));
                }
                break;
            }
        } catch (TraCIException& e) {
            return server.writeErrorStatusCmd(CMD_GET_POLYGON_VARIABLE, e.what(), outputStorage);
        }
    }
    server.writeStatusCmd(CMD_GET_POLYGON_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}

bool
TraCIServerAPI_Polygon::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                   tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_TYPE && variable != VAR_COLOR && variable != VAR_SHAPE && variable != VAR_FILL
            && variable != ADD && variable != REMOVE && variable != VAR_PARAMETER) {
        return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE,
                                          "Change Polygon State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    std::string id = inputStorage.readString();
    try {
        // process
        switch (variable) {
            case VAR_TYPE: {
                std::string type;
                if (!server.readTypeCheckingString(inputStorage, type)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The type must be given as a string.", outputStorage);
                }
                TraCI_Polygon::setType(id, type);
            }
            break;
            case VAR_COLOR: {
                TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The color must be given using an according type.", outputStorage);
                }
                TraCI_Polygon::setColor(id, col);
            }
            break;
            case VAR_SHAPE: {
                PositionVector shape;
                if (!server.readTypeCheckingPolygon(inputStorage, shape)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The shape must be given using an accoring type.", outputStorage);
                }
                TraCI_Polygon::setShape(id, TraCI::makeTraCIPositionVector(shape));
            }
            break;
            case VAR_FILL: {
                int value = 0;
                if (!server.readTypeCheckingUnsignedByte(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "'fill' must be defined using an unsigned byte.", outputStorage);
                }
                TraCI_Polygon::setFilled(id, value != 0);
            }
            break;
            case ADD: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "A compound object is needed for setting a new polygon.", outputStorage);
                }
                //readt itemNo
                inputStorage.readInt();
                std::string type;
                if (!server.readTypeCheckingString(inputStorage, type)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The type must be given as a string.", outputStorage);
                }
                TraCIColor col;
                if (!server.readTypeCheckingColor(inputStorage, col)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The second polygon parameter must be the color.", outputStorage);
                }
                int value = 0;
                if (!server.readTypeCheckingUnsignedByte(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The third polygon parameter must be 'fill' encoded as ubyte.", outputStorage);
                }
                bool fill = value != 0;
                int layer = 0;
                if (!server.readTypeCheckingInt(inputStorage, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The fourth polygon parameter must be the layer encoded as int.", outputStorage);
                }
                PositionVector shape;
                if (!server.readTypeCheckingPolygon(inputStorage, shape)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The fifth polygon parameter must be the shape.", outputStorage);
                }
                TraCIPositionVector tp = TraCI::makeTraCIPositionVector(shape);

                TraCI_Polygon::add(id, tp, col, fill, type, layer);

            }
            break;
            case REMOVE: {
                int layer = 0; // !!! layer not used yet (shouldn't the id be enough?)
                if (!server.readTypeCheckingInt(inputStorage, layer)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The layer must be given using an int.", outputStorage);
                }

                TraCI_Polygon::remove(id, layer);

            }
            break;
            case VAR_PARAMETER: {
                if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
                }
                //readt itemNo
                inputStorage.readInt();
                std::string name;
                if (!server.readTypeCheckingString(inputStorage, name)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
                }
                std::string value;
                if (!server.readTypeCheckingString(inputStorage, value)) {
                    return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
                }
                TraCI_Polygon::setParameter(id, name, value);

            }
            break;
            default:
                break;
        }
    } catch (TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_SET_POLYGON_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_SET_POLYGON_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}

bool
TraCIServerAPI_Polygon::getShape(const std::string& id, PositionVector& shape) {
    SUMO::Polygon* poly = getPolygon(id);
    if (poly == 0) {
        return false;
    }
    shape = poly->getShape();
    return true;
}

SUMO::Polygon*
TraCIServerAPI_Polygon::getPolygon(const std::string& id) {
    return MSNet::getInstance()->getShapeContainer().getPolygons().get(id);
}

NamedRTree*
TraCIServerAPI_Polygon::getTree() {
    NamedRTree* t = new NamedRTree();
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    const std::map<std::string, SUMO::Polygon*>& polygons = shapeCont.getPolygons().getMyMap();
    for (std::map<std::string, SUMO::Polygon*>::const_iterator i = polygons.begin(); i != polygons.end(); ++i) {
        Boundary b = (*i).second->getShape().getBoxBoundary();
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        t->Insert(cmin, cmax, (*i).second);
    }
    return t;
}

#endif


/****************************************************************************/

