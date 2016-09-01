/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, UMH, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "ns3/core-module.h"
#include "ns3/simulator-module.h"
#include "ns3/node-module.h"
#include "ns3/contrib-module.h"
#include "ns3/helper-module.h"
#include "ns3/global-routing-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/c2c-stack-module.h"
#include "ns3/CAMmanagement.h"
#include "ns3/inci-utils-module.h"
#include "ns3/itetris-station-mgnt-module.h"

#include "stdio.h"
#include <iostream>
#include <fstream>

using namespace ns3;
using namespace std;

uint32_t numNodes = 3;
double sim_time = 20.0;
iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();

void
PhyTxTrace (std::string context, Ptr<const Packet> packet, WifiMode mode, WifiPreamble preamble, uint8_t txPower)
{
  WifiMacHeader hdr;
  packet->PeekHeader (hdr);
  std::cout <<  Simulator::Now () << " " << context << " Packet Uid " << packet->GetUid() 
  << " PHYTX mode=" << mode << " to=" << hdr.GetAddr1 () << " from=" << hdr.GetAddr2 () 
  << std::endl;
}

void
PhyRxOkTrace (std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, enum WifiPreamble preamble)
{
  WifiMacHeader hdr;
  packet->PeekHeader (hdr);
  std::cout <<  Simulator::Now () << " " << context << " Packet Uid " << packet->GetUid() 
  << " PHYRXOK mode=" << mode << " to=" << hdr.GetAddr1 () << " from=" << hdr.GetAddr2 () 
  << std::endl;
}

void
ReceivePackets (PacketManager* packetManager, uint32_t nodeId)
{
  struct InciPacket::ReceivedInciPacket inciPacket;
  std::cout << "*** iCS collecting received CAMs ***" << std::endl;
  while ( packetManager->GetReceivedPacket(nodeId,inciPacket) )
    {
      std::cout << "Node=" << nodeId << " at time=" << inciPacket.ts << " received a msgType="<< inciPacket.msgType 
      << " fromNode="<< inciPacket.senderId << std::endl;
    }
}

NS_LOG_COMPONENT_DEFINE ("ns3_iTETRIS");

int 
main (int argc, char *argv[])
{
  /**
   * Two different configuration files can be used: a file (fileGeneralParameters) with the default attribute values to be used 
   * in the simulation and a file (fileConfTechnologies) with the configuration of the iTETRIS scenario. The file paths can be passed 
   * from the command line.
   */
  std::string fileGeneralParameters = "";
  std::string fileConfTechnologies = "";
  CommandLine cmd;
  cmd.AddValue("fileGeneralParameters", "Path to the configuration file", fileGeneralParameters);
  cmd.AddValue("fileConfTechnologies", "Path to the configuration file", fileConfTechnologies);
  cmd.Parse (argc, argv);

  if (fileGeneralParameters != "")
    {
      Config::SetDefault ("ns3::ConfigStore::Filename", StringValue (fileGeneralParameters));
      Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
      ConfigStore config;
      config.ConfigureDefaults ();
    }

  if (fileConfTechnologies == "")
    {
      fileConfTechnologies = "scratch/configTechnologies-ics.xml";
    }

  ConfigurationManagerXml confManager (fileConfTechnologies);
  iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();
  confManager.ReadFile(nodeManager);

  nodeManager->CreateItetrisNode (Vector(100,0,0));
  nodeManager->InstallCommunicationModule ("WaveVehicle");

  nodeManager->CreateItetrisNode (Vector(200,0,0));
  nodeManager->InstallCommunicationModule ("WaveVehicle");

  nodeManager->CreateItetrisNode (Vector(250,0,0));
  nodeManager->InstallCommunicationModule ("WaveRsu");

  NodeContainer mobileNodes = nodeManager->GetItetrisNodes ();

  PacketManager* packetManager = new PacketManager ();
  packetManager->SetNodeManager(nodeManager);
  nodeManager->ActivateNode(0);
  nodeManager->ActivateNode(1);

  Simulator::Schedule (Seconds (1.0), &PacketManager::ActivateCamTxon,packetManager,0,2,300);
  Simulator::Schedule (Seconds (1.0), &PacketManager::ActivateCamTxon,packetManager,1,2,300);
  Simulator::Schedule (Seconds (1.0), &PacketManager::ActivateCamTxon,packetManager,2,2,300);

  Simulator::Schedule (Seconds (4.0), &PacketManager::DeactivateCamTxon,packetManager,0);
  Simulator::Schedule (Seconds (4.0), &PacketManager::DeactivateCamTxon,packetManager,1);
  Simulator::Schedule (Seconds (4.0), &PacketManager::DeactivateCamTxon,packetManager,2);

  Simulator::Schedule (Seconds (8.0), &ReceivePackets,packetManager,0);
  Simulator::Schedule (Seconds (8.0), &ReceivePackets,packetManager,1);
  Simulator::Schedule (Seconds (8.0), &ReceivePackets,packetManager,2);

  Simulator::Stop (Seconds (sim_time));
  Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/RxOk", MakeCallback (&PhyRxOkTrace));
  Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/Tx", MakeCallback (&PhyTxTrace));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
