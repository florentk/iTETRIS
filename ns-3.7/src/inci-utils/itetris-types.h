/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ramon Bauza <rbauza@umh.es>, Michele Rondinone <mrondinone@umh.es>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
 */

#include "ns3/node.h"

#ifndef ITETRIS_TYPES_H
#define ITETRIS_TYPES_H
#include <list>
#include "itetris-technologies.h"

namespace ns3
{
  const uint32_t ID_BROADCAST =  900000000;
  
  const uint32_t TOPO_BROADCAST =  900000001;
  
  const uint32_t GEO_UNICAST =  900000002;

  const uint32_t ID_MULTICAST =  900000003;

  const uint32_t TMC_CONSTANT =  900000004;   

  enum STACK { IPv4, IPv6, C2C };

  enum TransmissionMode { IP_BROADCAST, IP_MULTICAST, C2C_GEOBROADCAST, C2C_GEOANYCAST, C2C_TOPOBROADCAST };

  enum ServiceProfile {CAM, DEMN, APP, APP_ITSG5};

  typedef std::vector< std::string > TechnologyList;
 
  typedef struct CircularGeoAddress {
    uint32_t lat;
    uint32_t  lon;
    uint32_t areaSize;
  } CircularGeoAddress;

  typedef struct DissProfile { STACK stack; NetDeviceType tech; Ptr<Node> disseminator; } disseminationProfile;

  typedef struct StackToDestination { STACK stack; uint32_t destination; char* tech; } stacktodestination;

}

#endif
