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

#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/CAMmanagement.h"
#include "ns3/C2C-IP-app.h"
#include "ns3/ipv4-address.h"
#include "ns3/c2c-address.h"
#include "ns3/message-management.h"

#include "service-management.h"

NS_LOG_COMPONENT_DEFINE ("ServiceManagement");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ServiceManagement);

TypeId 
ServiceManagement::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ServiceManagement")
    .SetParent<Object> ()
    .AddConstructor<ServiceManagement> ()
    ;
  return tid;
}

ServiceManagement::ServiceManagement (){
}

ServiceManagement::~ServiceManagement ()
{
}

void
ServiceManagement::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  Object::DoDispose ();
}

void 
ServiceManagement::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;
}

void
ServiceManagement::NotifyNewAggregate ()
{
  if (m_node == 0)
    {
      Ptr<Node>node = this->GetObject<Node>();
      // verify that it's a valid node and that
      // the node has not been set before
      if (node != 0)
        {
          this->SetNode (node);
        }
    }
  Object::NotifyNewAggregate ();
}

void
ServiceManagement::SetServiceList(ServiceList* services)
{
  m_servicelist = services;
}

ServiceList*
ServiceManagement::GetServiceList(void)
{
  return m_servicelist;
}

void
ServiceManagement::ActivateIPService(std::string ServiceID, Ipv4Address address,double frequency, double MessRegenerationTime, uint32_t packetSize)  
{  
  Ptr<Application> service = m_servicelist->GetService (ServiceID);
  Ptr<iTETRISApplication> App = DynamicCast<iTETRISApplication> (service);
  NS_ASSERT_MSG (App,"ServiceManagement::ActivateIPService -> Service with ID="<<ServiceID<<" not found in the service list");
  App->SetMessRegenerationTime (MessRegenerationTime);
  App->SetFrequency (frequency);
  App->SetPacketSize (packetSize);
  m_MessageManagement->StartTransmission(App, address);     
}

void
ServiceManagement::ActivateC2CService(std::string ServiceID, Ptr<c2cAddress> address, double frequency, double MessRegenerationTime, uint8_t msglifetime, uint32_t packetSize)  
{  
  m_servicelist->ShowServices();
  Ptr<Application> service = m_servicelist->GetService (ServiceID);
  Ptr<iTETRISApplication> App = DynamicCast<iTETRISApplication> (service);
  NS_ASSERT_MSG (App,"ServiceManagement::ActivateC2CService -> Service with ID="<<ServiceID<<" not found in the service list");
  App->SetMessRegenerationTime (MessRegenerationTime);
  App->SetFrequency (frequency);
  App->SetPacketSize (packetSize);
  App->SetMsgLifeTime (msglifetime);
   
  m_MessageManagement->StartTransmission(App, address);   
}

void
ServiceManagement::ActivateC2CService(std::string ServiceID, Ptr<c2cAddress> address, double frequency, uint32_t packetSize)
{  
  Ptr<Application> service = m_servicelist->GetService (ServiceID);
  Ptr<iTETRISApplication> App = DynamicCast<iTETRISApplication> (service);
            
  App->SetFrequency (frequency);
  App->SetPacketSize (packetSize);
    
  m_MessageManagement->StartTransmission(App, address);   
}

void 
ServiceManagement::DeactivateService(std::string ServiceID)
{
  Ptr<Application> service = m_servicelist->GetService (ServiceID);
  Ptr<iTETRISApplication> App = DynamicCast<iTETRISApplication> (service);
  App->StopTransmitting();
}

void 
ServiceManagement::SetMessageManagement( Ptr<MessageManagement> messMgmt)
{
 m_MessageManagement=messMgmt;
}

} //namespace ns3
