/****************************************************************************/
/// @file    subscriptions-helper.cpp
/// @author  Jerome Haerri, Pasquale Cataldi (EURECOM)
/// @date    December 15th, 2010
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

#include <typeinfo>
#include "subscriptions-helper.h"
#include "subscriptions-type-constants.h"
#include "../facilities/mapFacilities/road/Edge.h"
#include "../facilities/mapFacilities/road/Lane.h"
#include "../facilities/mapFacilities/road/Junction.h"
#include "../../utils/ics/geometric/Shapes.h"
#include <cstring>
#include <vector>
#include <iostream>

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================

short SubscriptionsHelper::readShort(std::vector<unsigned char>* m_msg, unsigned int* index)
{
    short value = 0;
    memcpy(&value, &(*m_msg)[*index], sizeof(short));
    *index += sizeof(short);
    return value;
}

int SubscriptionsHelper::readInt(std::vector<unsigned char>* m_msg, unsigned int* index)
{
    int value = 0;
    memcpy(&value, &(*m_msg)[*index], sizeof(int));
    *index += sizeof(int);
    return value;
}

std::string SubscriptionsHelper::readString(std::vector<unsigned char>* m_msg, unsigned int* index)
{
    int length = readInt(m_msg, index);
    std::string str;
    str.assign((char*) &(*m_msg)[*index], length);
    *index += length;
    std::cout << "[iCS - SubscriptionsHelper::readString()] length " << length << std::endl;
    return str;
}

float SubscriptionsHelper::readFloat(std::vector<unsigned char>* m_msg, unsigned int* index)
{
    float value = 0;
    memcpy(&value, &(*m_msg)[*index], sizeof(float));
    *index += sizeof(float);
    return value;
}

std::vector<ics_types::TArea> SubscriptionsHelper::readBlockAreas(std::vector<unsigned char>* m_msg, unsigned int* index)
{
    std::vector<ics_types::TArea> areas;
    unsigned char numAreas = m_msg->at(*index);
    (*index)++;
    
    for (int i = 0; i < numAreas; i++) {
        unsigned char areaType = m_msg->at(*index);
        (*index)++;
        switch (areaType) {
            case EXT_HEADER__CIRCLE: {
                float x = SubscriptionsHelper::readFloat(m_msg, index);
                float y = SubscriptionsHelper::readFloat(m_msg, index);
                float radius = SubscriptionsHelper::readFloat(m_msg, index);
                Circle* circle = new Circle(Point2D(x, y), radius);
                ics_types::TArea area;
                area.type = GEOMETRICSHAPE;
                area.shape = circle;
                area.shapeType = CIRCLE;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__ELLIPSE: {
                float focusAx = SubscriptionsHelper::readFloat(m_msg, index);
                float focusAy = SubscriptionsHelper::readFloat(m_msg, index);
                float focusBx = SubscriptionsHelper::readFloat(m_msg, index);
                float focusBy = SubscriptionsHelper::readFloat(m_msg, index);
                float eccentricity = SubscriptionsHelper::readFloat(m_msg, index);
                Ellipse* ellipse = new Ellipse(Point2D(focusAx, focusAy), Point2D(focusBx, focusBy), eccentricity);
                ics_types::TArea area;
                area.type = GEOMETRICSHAPE;
                area.shape = ellipse;
                area.shapeType = ELLIPSE;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__CONV_POLYGON: {
                std::vector<Point2D> verts;
                unsigned int numVerts = (unsigned int) m_msg->at(*index);
                (*index)++;
                for (unsigned int j = 0; j < numVerts; j++) {
                    float x = SubscriptionsHelper::readFloat(m_msg, index);
                    float y = SubscriptionsHelper::readFloat(m_msg, index);
                    verts.push_back(Point2D(x, y));
                }
                ConvexPolygon* convexPolygon = new ConvexPolygon(verts);
                ics_types::TArea area;
                area.type = GEOMETRICSHAPE;
                area.shape = convexPolygon;
                area.shapeType = CONVEXPOLIGON;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__LANE_ID: {
                ics_types::roadElementID_t laneId = SubscriptionsHelper::readString(m_msg, index);
                ics_types::TArea area;
                area.type = ROADELEMENT;
                area.roadElementID = laneId;
                area.elementType = LANE;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__EDGE_ID: {
                ics_types::roadElementID_t edgeId = SubscriptionsHelper::readString(m_msg, index);
                ics_types::TArea area;
                area.type = ROADELEMENT;
                area.roadElementID = edgeId;
                area.elementType = EDGE;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__JUNCTION_ID: {
                ics_types::roadElementID_t junctionId = SubscriptionsHelper::readString(m_msg, index);
                ics_types::TArea area;
                area.type = ROADELEMENT;
                area.roadElementID = junctionId;
                area.elementType = JUNCTION;
                areas.push_back(area);
                break;
            }
            default: {
                std::cout << "[iCS - SubscriptionsHelper::readBlockAreas()] - ERROR: The area with code " << (unsigned int) areaType << " was not recognized." << std::endl;
                break;
            }
        }
    }
    return areas;
}


std::vector<ics_types::TArea> SubscriptionsHelper::readBlockAreas(tcpip::Storage* m_msg)
{
    cout << "READ BLOCK AREA ";
    cout << flush;
    
    std::vector<ics_types::TArea> areas;
    unsigned char numAreas = m_msg->readChar();
    
    cout <<  (int)numAreas << " ";
    cout << flush;
        
    
    for (int i = 0; i < numAreas; i++) {
        unsigned char areaType = m_msg->readChar();
        switch (areaType) {
            case EXT_HEADER__CIRCLE: {
                float x = m_msg->readFloat();
                float y =  m_msg->readFloat();
                float radius =  m_msg->readFloat();
                cout <<  x  << " " << y  << " " << radius  << endl;
                cout << flush;
                Circle* circle = new Circle(Point2D(x, y), radius);
                ics_types::TArea area;
                area.type = GEOMETRICSHAPE;
                area.shape = circle;
                area.shapeType = CIRCLE;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__ELLIPSE: {
                float focusAx =  m_msg->readFloat();
                float focusAy =  m_msg->readFloat();
                float focusBx =  m_msg->readFloat();
                float focusBy =  m_msg->readFloat();
                float eccentricity =  m_msg->readFloat();
                Ellipse* ellipse = new Ellipse(Point2D(focusAx, focusAy), Point2D(focusBx, focusBy), eccentricity);
                ics_types::TArea area;
                area.type = GEOMETRICSHAPE;
                area.shape = ellipse;
                area.shapeType = ELLIPSE;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__CONV_POLYGON: {
                std::vector<Point2D> verts;
                unsigned int numVerts =  m_msg->readChar();
                for (unsigned int j = 0; j < numVerts; j++) {
                    float x =  m_msg->readFloat();
                    float y =  m_msg->readFloat();
                    verts.push_back(Point2D(x, y));
                }
                ConvexPolygon* convexPolygon = new ConvexPolygon(verts);
                ics_types::TArea area;
                area.type = GEOMETRICSHAPE;
                area.shape = convexPolygon;
                area.shapeType = CONVEXPOLIGON;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__LANE_ID: {
                ics_types::roadElementID_t laneId =  m_msg->readString();
                ics_types::TArea area;
                area.type = ROADELEMENT;
                area.roadElementID = laneId;
                area.elementType = LANE;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__EDGE_ID: {
                ics_types::roadElementID_t edgeId = m_msg->readString();
                ics_types::TArea area;
                area.type = ROADELEMENT;
                area.roadElementID = edgeId;
                area.elementType = EDGE;
                areas.push_back(area);
                break;
            }
            case EXT_HEADER__JUNCTION_ID: {
                ics_types::roadElementID_t junctionId = m_msg->readString();
                ics_types::TArea area;
                area.type = ROADELEMENT;
                area.roadElementID = junctionId;
                area.elementType = JUNCTION;
                areas.push_back(area);
                break;
            }
            default: {
                std::cout << "[iCS - SubscriptionsHelper::readBlockAreas()] - ERROR: The area with code " << (unsigned int) areaType << " was not recognized." << std::endl;
                break;
            }
        }
    }
    return areas;
}

std::vector<unsigned char> SubscriptionsHelper::reinterpretFloat(float value)
{
    std::vector<unsigned char> ucInfo;
    unsigned char *p_value = NULL;
    // Write the length of the following field
    ucInfo.push_back((unsigned char) 4);
    // Write the field
    p_value = reinterpret_cast<unsigned char*>(&value);
    std::vector<unsigned char> tmp = SubscriptionsHelper::writeByEndianess(p_value, 4);
    ucInfo.insert(ucInfo.end(), tmp.begin(), tmp.end());
    return ucInfo;
}

std::vector<unsigned char> SubscriptionsHelper::reinterpretDouble(double value)
{
    std::vector<unsigned char> ucInfo;
    unsigned char *p_value = NULL;
    // Write the length of the following field
    ucInfo.push_back((unsigned char) 8);
    // Write the field
    p_value = reinterpret_cast<unsigned char*>(&value);
    std::vector<unsigned char> tmp = SubscriptionsHelper::writeByEndianess(p_value, 8);
    ucInfo.insert(ucInfo.end(), tmp.begin(), tmp.end());
    return ucInfo;
}

std::vector<unsigned char> SubscriptionsHelper::reinterpretInt(int value)
{
    std::vector<unsigned char> ucInfo;
    unsigned char *p_value = NULL;
    // Write the length of the following field
    ucInfo.push_back((unsigned char) 4);
    // Write the field
    p_value = reinterpret_cast<unsigned char*>(&value);
    std::vector<unsigned char> tmp = SubscriptionsHelper::writeByEndianess(p_value, 4);
    ucInfo.insert(ucInfo.end(), tmp.begin(), tmp.end());
    return ucInfo;
}

std::vector<unsigned char> SubscriptionsHelper::reinterpreteShort(short value)
{
    std::vector<unsigned char> ucInfo;
    unsigned char *p_value = NULL;
    // Write the length of the following field
    ucInfo.push_back((unsigned char) 2);
    // Write the field
    p_value = reinterpret_cast<unsigned char*>(&value);
    std::vector<unsigned char> tmp = SubscriptionsHelper::writeByEndianess(p_value, 2);
    ucInfo.insert(ucInfo.end(), tmp.begin(), tmp.end());
    return ucInfo;
}

std::vector<unsigned char> SubscriptionsHelper::reinterpreteString(const std::string s)
{
    std::vector<unsigned char> ucInfo;
    // Write the length of the following field
    if (s.length() > 255) {
        std::cout << "[iCS] SubsGetFacilitiesInfo::reinterpreteString() - The length of the string " << s << " is greater than 255 bytes!" << std::endl;
        return ucInfo;
    }
    ucInfo.push_back((unsigned char) s.length());
    // Write the field
    ucInfo.insert(ucInfo.end(), s.begin(), s.end());
    return ucInfo;
}

// ----------------------------------------------------------------------
std::vector<unsigned char> SubscriptionsHelper::writeByEndianess(const unsigned char * begin, unsigned int size)
{
    const unsigned char * end = &(begin[size]);
    std::vector<unsigned char> store;
    store.insert(store.end(), begin, end);
    return store;
}

}
