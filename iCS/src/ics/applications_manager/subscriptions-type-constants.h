/****************************************************************************/
/// @file    subscriptions-type-contants.h
/// @author  Jerome Haerri, Pasquale Cataldi (EURECOM)
/// @date    December 15th, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/


#ifndef SUBSCRIPTIONS_TYPE_CONSTANTS_H_
#define SUBSCRIPTIONS_TYPE_CONSTANTS_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

// ****************************************
// APPLICATION SUBSCRIPTION TYPES
// ****************************************

#define TYPE_ERROR			       			  0x00

#define EXT_HEADER_TYPE_TOPOBROADCAST       0x00
#define EXT_HEADER_TYPE_UNICAST             0x01
#define EXT_HEADER_TYPE_MULTICAST           0x02
#define EXT_HEADER_TYPE_GEOBROADCAST        0x03
#define EXT_HEADER_TYPE_GEOUNICAST          0x04
#define EXT_HEADER_TYPE_GEOANYCAST          0x05

#define EXT_HEADER__VALUE_BLOCK_IDs         0x00
#define EXT_HEADER__VALUE_BLOCK_AREAs       0x01
#define EXT_HEADER__VALUE_BLOCK_HOPS_No     0x02

#define EXT_HEADER__CIRCLE                  0x00
#define EXT_HEADER__ELLIPSE                 0x01
#define EXT_HEADER__CONV_POLYGON            0x02
#define EXT_HEADER__LANE_ID                 0x03
#define EXT_HEADER__EDGE_ID                 0x04
#define EXT_HEADER__JUNCTION_ID             0x05

// ****************************************
// CROSS_APPLICATION SUBSCRIPTION TYPE
// ****************************************
#define TTE									0x01
#define JAM									0x02
#define EDGE_ID								0x03
#define ROUTE_ID							0x04
#define SPEED								0x05

#define VALUE_GET_DATA_BY_RESULT_CONTAINER_ID      0x01

#endif /* SUBSCRIPTIONS_TYPE_CONSTANTS_H_ */
