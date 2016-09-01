/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 * University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>,
 */

#ifndef SERVICE_MANAGEMENT_H
#define SERVICE_MANAGEMENT_H

#include <vector>

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/service-list.h"
#include "ns3/ipv4-address.h"

#include "ns3/message-management.h"

namespace ns3 {

class C2CAddress;
class ServiceManagement : public Object
{

public:

  static TypeId GetTypeId (void);
  ServiceManagement ();
  ~ServiceManagement ();
  void DoDispose (void);

void 
SetMessageManagement( Ptr<MessageManagement> messMgmt);

/**
* Set the list of services the ServiceManagement facility has to work with 
*/
  void SetServiceList(ServiceList* services);

/**
* Get the list of services the ServiceManagement facility works with.
*
* this command can be used to add a newly installed service to the retrieved service list
*/
  ServiceList* GetServiceList(void);
  
  /**
* Activate a specific service among those stored in the service list and transmit with the IP stack
*/
  void ActivateIPService(std::string ServiceID, Ipv4Address address, double frequency, double MessRegenerationTime, uint32_t packetSize); 

/**
* Activate a specific service among those stored in the service list and transmit with the C2C stack
*/
  void ActivateC2CService(std::string ServiceID, Ptr<c2cAddress> address, double frequency, double MessRegenerationTime, uint8_t msglifetime, uint32_t packetSize); 

/**
* Activate a specific service among those stored in the service list and transmit with the C2C stack
*/
  void ActivateC2CService(std::string ServiceID, Ptr<c2cAddress> address, double frequency, uint32_t packetSize);

/**
* Dectivate a specific service among those stored in the service list
*/
  void DeactivateService(std::string ServiceID);

  void SetNode (Ptr<Node> node);
  void NotifyNewAggregate ();

private:

  ServiceList*  m_servicelist;
  Ptr<Node> m_node;
  Ptr<MessageManagement> m_MessageManagement;

};

}; //namespace ns3

#endif /* SERVICE_MANAGEMENT_H */