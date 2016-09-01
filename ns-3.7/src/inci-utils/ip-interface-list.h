/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#ifndef IP_INTERFACE_LIST_H
#define IP_INTERFACE_LIST_H

#include "ns3/object.h"
#include "ns3/ipv4-interface-address.h"
#include <map>

namespace ns3
{

/**
 * @class IpInterfaceList
 * @brief The class IpInterfaceList should be attached to every node that has IP stack. It maintains a mapping between the name of the NetDevices and their assigned IP addresses. Thus, it allows retrieving the IP address of a given NetDevice from other modules in ns-3, e.g. GetIpAddress("WiFi")
 */
class IpInterfaceList : public Object
{
  public:
    static TypeId GetTypeId (void);
    IpInterfaceList (void);
    bool AddIpInterface (std::string InterfaceName, Ipv4InterfaceAddress);
    Ipv4InterfaceAddress GetIpInterfaceAddress (std::string InterfaceName);
    Ipv4Address GetIpAddress (std::string interfaceName);

  private:
    typedef std::map<std::string, Ipv4InterfaceAddress> InterfaceList; 
    InterfaceList m_ipInterfaceList;
};

}

#endif
