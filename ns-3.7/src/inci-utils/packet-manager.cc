/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez
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
 * Author: Ramon Bauza <rbauza@umh.es>, Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

#include <list>
#include "ns3/log.h"

#include "iTETRISNodeManager.h"
#include "ns3/CAMmanagement.h" 
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/node-container.h"
#include "ns3/inci-packet-list.h" 
#include "ns3/itetris-types.h" 
#include "ns3/iTETRISns3Facilities.h" 
#include "ns3/IPCIUFacilities.h" 
#include "ns3/umts-vehicle-scan-mngr.h"
#include "ns3/umts-bs-mgnt.h"
#include "packet-manager.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("PacketManager");

namespace ns3
{
    
PacketManager::PacketManager ()
  : m_nodeManager (0)
{} 

PacketManager::~PacketManager ()
{}

void 
PacketManager::SetNodeManager (iTETRISNodeManager* nodeManager)
{
  m_nodeManager = nodeManager;
}

void
PacketManager::ActivateCamTxon (uint32_t nodeId, float frequency, uint32_t packetSize)
{  
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);
  if (facilities != NULL && IsNodeActive (nodeId))
    {
      facilities->ActivateCamTxon (frequency, packetSize);
    }
}

void
PacketManager::DeactivateCamTxon (uint32_t nodeId)
{
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);
  if (facilities != NULL)
    {      
      facilities->DeactivateCamTxon ();
    }
}

void
PacketManager::ActivateDenmTxon (uint32_t nodeId, CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msgLifetime)
{
  
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);
    if (facilities != NULL && IsNodeActive (nodeId))
    {
      facilities->ActivateDenmTxon (destination, frequency, packetSize, msgRegenerationTime, msgLifetime);
    }
}
 
 void
PacketManager::DeactivateDenmTxon (uint32_t nodeId)
{
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);
  if (facilities != NULL)
    {      
      facilities->DeactivateDenmTxon ();
    }
}

bool 
PacketManager::GetReceivedPacket (uint32_t nodeId, struct InciPacket::ReceivedInciPacket &inciPacketData)
{
  bool morePackets = false;
  Ptr<Node> node = m_nodeManager->GetItetrisNode (nodeId); 
  if (node != NULL)
    {
      Ptr<InciPacketList> packetList = node->GetObject<InciPacketList> ();
      if (packetList != NULL)
        {
          InciPacket packet;
          morePackets = packetList->GetReceivedPacket (packet); 
          inciPacketData = packet.GetData ();
        }
      else
        {
          NS_LOG_DEBUG ("Node with ID " << nodeId << " does not have a PacketList attached");
        }
    }
  else
    {
      NS_LOG_DEBUG ("Node with ID " << nodeId << " has not been found in the ItetrisNodeManager");
    }
   
  return morePackets;
}

Ptr<iTETRISns3Facilities>
PacketManager::GetFacilities (uint32_t nodeId)
{
  Ptr<Node> node = m_nodeManager->GetItetrisNode (nodeId); 
  if (node != NULL)
    {
      return node->GetObject <iTETRISns3Facilities> ();
    }
  else
    {
      return NULL;
    }
}

Ptr<IPCIUFacilities>
PacketManager::GetIPCIUFacilities (uint32_t nodeId)
{
  Ptr<Node> node = m_nodeManager->GetItetrisNode (nodeId); 
  if (node != NULL)
    {
      return node->GetObject <IPCIUFacilities> ();
    }
  else
    {
      return NULL;
    }
}

Ptr<MWFacilities>
PacketManager::GetMWFacilities (uint32_t nodeId)
{
  Ptr<Node> node = m_nodeManager->GetItetrisNode (nodeId); 
  if (node != NULL)
    {
      return node->GetObject <MWFacilities> ();
    }
  else
    {
      return NULL;
    }
}

void 
PacketManager::InitiateIdBasedTxon (uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies, float frequency, uint32_t packetSize, uint32_t destination, double msgRegenerationTime, uint8_t msgLifetime)
{
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);  
  if (facilities != NULL && IsNodeActive (nodeId))
    {
     facilities->InitiateIdBasedTxon(serviceId, commProfile, technologies, destination, frequency, packetSize, msgRegenerationTime, msgLifetime);		               
    }
}

void 
PacketManager::InitiateIPCIUTxon (uint32_t nodeId, std::string serviceId, float frequency, uint32_t packetSize, uint32_t destination, double msgRegenerationTime)
{
  Ptr<IPCIUFacilities> facilities = GetIPCIUFacilities (nodeId);
  if (facilities != NULL && IsNodeActive (nodeId))
    {            	
      facilities->InitiateIPBasedTxon(serviceId, destination, frequency, packetSize, msgRegenerationTime);  
    }
}

void 
PacketManager::InitiateMWTxon (uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies, CircularGeoAddress destination, float frequency, uint32_t packetSize,  double msgRegenerationTime, uint8_t msgLifetime)
{
  Ptr<MWFacilities> facilities = GetMWFacilities (nodeId);
  if (facilities != NULL && IsNodeActive (nodeId))
    {
      facilities->InitiateMWGeoBasedTxon (serviceId, commProfile, technologies, destination, frequency, packetSize, msgRegenerationTime, msgLifetime);
    }
}

void 
PacketManager::InitiateGeoBroadcastTxon (uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msgLifetime)
{
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);
  if (facilities != NULL && IsNodeActive (nodeId))
    {
       facilities->InitiateGeoBroadcastTxon (serviceId, commProfile, technologies, destination, frequency, packetSize, msgRegenerationTime, msgLifetime);
    }
}

void 
PacketManager::InitiateGeoAnycastTxon (uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msgLifetime)
{
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);
  if (facilities != NULL && IsNodeActive (nodeId))
    {
       facilities->InitiateGeoAnycastTxon (serviceId, commProfile, technologies, destination, frequency, packetSize, msgRegenerationTime, msgLifetime);
    }
}

void 
PacketManager::ActivateTopoBroadcastTxon (uint32_t nodeId, std::string serviceId, uint32_t commProfile, TechnologyList technologies, double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msgLifetime, uint32_t numHops)
{
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);
  if (facilities != NULL && IsNodeActive (nodeId))
    {
       facilities->InitiateTopoBroadcastTxon (serviceId, commProfile, technologies, frequency, packetSize, msgRegenerationTime, msgLifetime, numHops);
    }
}

void 
PacketManager::DeactivateServiceTxon (uint32_t nodeId, std::string serviceId)
{
  Ptr<iTETRISns3Facilities> facilities = GetFacilities (nodeId);
  if (facilities != NULL)
    {
        facilities->DeactivateServiceTxon (serviceId);
    }
}

void 
PacketManager::DeactivateIPCIUServiceTxon (uint32_t nodeId, std::string serviceId)
{
  Ptr<IPCIUFacilities> facilities = GetIPCIUFacilities (nodeId);
  if (facilities != NULL)
    {
        facilities->DeactivateServiceTxon (serviceId);
    }
}

void 
PacketManager::DeactivateMWServiceTxon (uint32_t nodeId, std::string serviceId)
{
  Ptr<MWFacilities> facilities = GetMWFacilities (nodeId);
  if (facilities != NULL)
    {
        facilities->DeactivateServiceTxon (serviceId);
    }
}

bool 
PacketManager::IsNodeActive (uint32_t nodeId)
{
  return m_nodeManager->IsNodeActive (nodeId);
}

}
