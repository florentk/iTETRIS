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
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/c2c-address.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/node-id-tag.h" 
#include "ns3/time-step-tag.h" 
#include "ns3/TStep-sequence-number-tag.h"
#include "ns3/app-index-tag.h"

#include "iTETRIS-Application.h"

NS_LOG_COMPONENT_DEFINE ("iTETRISApplication");

using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (iTETRISApplication);

TypeId
iTETRISApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::iTETRISApplication")
    .SetParent<Application> ()
    .AddConstructor<iTETRISApplication> ()
    ;
  return tid;
}

iTETRISApplication::iTETRISApplication ()
{
}

iTETRISApplication::~iTETRISApplication()
{
}

void
iTETRISApplication::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Application::DoDispose ();
}

void 
iTETRISApplication::SetServiceType (std::string servicetype)
{
   if(servicetype.find("-")!=string::npos)
     {
       m_composedServiceType=servicetype;      
       int index=m_composedServiceType.find("-");      
       m_servicetype=m_composedServiceType.substr(index+1);
     }
   else
     {
       m_composedServiceType=servicetype;     
       m_servicetype=m_composedServiceType;
     }
}

void 
iTETRISApplication::SetServiceIndex(uint32_t app_index)
{
   m_app_index=app_index;
}

void
iTETRISApplication::AddInciPacketTags (Ptr<Packet> p)
{
  // Sender ID tag
  Ptr<Node> local = GetNode();
  uint32_t nodeID = local->GetId();

  NodeIdTag nodeTag;
  nodeTag.Set(nodeID);
  p->AddPacketTag (nodeTag);

  // Application Index tag
  AppIndexTag appindexTag;
  appindexTag.Set(m_app_index);
  p->AddPacketTag (appindexTag);

  // update of timestep and time step sequence number
  m_currentTimeStep = (static_cast<uint32_t>(Simulator::Now().GetSeconds()));
  if(m_currentTimeStep>m_previousTimeStep)
    {
      m_stepSequenceNumber=0;
      m_previousTimeStep=m_currentTimeStep;
    }
  else
    {
      m_stepSequenceNumber++;  
    }
  
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][iTETRISApplication]service =  "<<m_servicetype<<"         app index =  "<<m_app_index<<"          sender nodeID = "<<nodeID<<"      timestep = "<<m_currentTimeStep<<"        timestepSeqNo = "<<m_stepSequenceNumber<<"\n");

  // time step tag
  TimeStepTag timeStepTag;
  timeStepTag.Set(m_currentTimeStep);
  p->AddPacketTag (timeStepTag);

  // time step sequence number tag
  TStepSequenceNumberTag TSSeqNTag;
  TSSeqNTag.Set(m_stepSequenceNumber);
  p->AddPacketTag (TSSeqNTag);
}

void
iTETRISApplication::RetrieveInciPacketTags (Ptr<Packet> packet)
{
  NodeIdTag nodeTag;
  bool found;
  found = packet->PeekPacketTag (nodeTag);
  NS_ASSERT (found);
  uint32_t senderId = nodeTag.Get ();
 
  TimeStepTag timestepTag;
  found = packet->PeekPacketTag (timestepTag);
  NS_ASSERT (found);
  uint32_t timeStep = timestepTag.Get ();

  TStepSequenceNumberTag timestepSNTag;
  found = packet->PeekPacketTag (timestepSNTag);
  NS_ASSERT (found);
  uint32_t timeStepSN = timestepSNTag.Get ();

  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][iTETRISApplication]RECEPTION: service =  "<<m_servicetype<<"     sender nodeID = "<<senderId<<"      timestep = "<<timeStep<<"        timestepSeqNo = "<<timeStepSN <<"\n");
 
  m_forwardIcs (senderId, m_servicetype, timeStep, timeStepSN);
}

void
iTETRISApplication::InitializeINCIvariables(void)
{
  m_previousTimeStep = (static_cast<uint32_t>(Simulator::Now().GetSeconds()));
  m_currentTimeStep =0;
  m_stepSequenceNumber=-1;  
}

void 
iTETRISApplication::SetApplicationId(uint64_t applicationId)
{
}

void
iTETRISApplication::StartApplication (void)
{
}

void 
iTETRISApplication::StopApplication(void)
{
}

void 
iTETRISApplication::StartTransmitting(Ipv4Address address)
{
}

void 
iTETRISApplication::StartTransmitting(Ptr<c2cAddress> address)
{
}

void 
iTETRISApplication::StopTransmitting(void)
{
}

void 
iTETRISApplication::SetFrequency(double frequency)
{
  m_frequency = frequency;
}

void 
iTETRISApplication::SetMessRegenerationTime (double MessRegenerationTime)
{
  m_MessRegenerationTime= MessRegenerationTime;
}

void 
iTETRISApplication::SetPacketSize (uint32_t PacketSize)
{
  m_packetSize = PacketSize;
}

void 
iTETRISApplication::SetMsgLifeTime (uint8_t MsgLifeTime)
{
  m_MsgLifeTime = MsgLifeTime;
}

void 
iTETRISApplication::SetSockets(void)
{
}

void 
iTETRISApplication::SetReceiveCallback (iTETRISApplication::ReceiveCallback cb)
{
  m_forwardIcs = cb;
}

void 
iTETRISApplication::UnsetReceiveCallback (void)
{
  m_forwardIcs.Nullify ();
}

void 
iTETRISApplication::SetC2CAddress(Ptr<c2cAddress> address)
{
}

void 
iTETRISApplication::SetIPAddress(Ipv4Address address)
{
}

} // Namespace ns3
