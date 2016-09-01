/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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
// #include "ns3/cid.h"

#include "stdio.h"
#include <iostream>
#include <fstream>

using namespace ns3;
using namespace std;

uint32_t numNodes = 3;
double sim_time = 3000.0;
iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();
bool going = true;

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
  std::cout << "*** iCS collecting received Packets from ns-3 for node " <<nodeId<<" ***" << std::endl;
  while ( packetManager->GetReceivedPacket(nodeId,inciPacket) )
    {
      std::cout << "Node=" << nodeId << " at time=" << inciPacket.ts << " received a msgType="<< inciPacket.msgType 
      << " fromNode="<< inciPacket.senderId << std::endl;
    }
}

// Send a unicast packet of size bytes from the sender to the destination, being the sender a Mobile Node.
void ActivateIdBasedTxon (PacketManager* packetManager,uint32_t size, uint32_t sender, uint32_t destination)
{
   TechnologyList techList;
   techList.push_back("WimaxVehicle");
   packetManager->InitiateIdBasedTxon (sender, "serviceIdUnicast", 0, techList, 1, size, destination, 0, 1);
  Simulator::Schedule (Seconds (100.0), &ActivateIdBasedTxon,packetManager,50, sender, destination);
}

// Send a unicast or a broadcast packet of size bytes from the sender to the destination, being the sender a Fixed Station. 
void InitiateIpCiuTxon (PacketManager* packetManager, uint32_t size, uint32_t sender, uint32_t destination)
{
  if(destination != ID_BROADCAST)
  {
     packetManager->InitiateIPCIUTxon (sender, "serviceIdUnicast", 1, size, destination, 1);
  }
  else
  {
     packetManager->InitiateIPCIUTxon (sender, "serviceIdBroadcast", 1, size, destination, 1);
  }
  Simulator::Schedule (Seconds (100.0), &InitiateIpCiuTxon,packetManager, 50, sender, destination);	
}

void
CheckMobility (Ptr<Node> node)
{
  Ptr<MobilityModel> model = node->GetObject<MobilityModel> ();
  Vector pos = model->GetPosition ();
  if (pos.x >= 0  && pos.x < 10000 && going)
    {
      going = true;
      pos.x = pos.x + 100;
      model->SetPosition(pos);
    }
  else if (pos.x == 10000 && going)
    {
      going = false;
      pos.x = pos.x - 100;
      model->SetPosition(pos);
    }
  else if (pos.x <= 10000 && pos.x > 0 && !going)
    {
      pos.x = pos.x - 100;
      model->SetPosition(pos);
    }
  else if (pos.x == 0 && !going)
    {
      going = true;
      pos.x = pos.x + 100;
      model->SetPosition(pos);
    }
  cout << "   @ T=" << Simulator::Now () << " vehicle "<<node->GetId()<<" Pos="<< model->GetPosition ().x <<endl;
  Simulator::Schedule (Seconds (10.0), &CheckMobility,node);
}

NS_LOG_COMPONENT_DEFINE ("ns3_iTETRIS");

int 
main (int argc, char *argv[])
{
   LogComponentEnable ("ConfigurationManagerXml",LOG_LEVEL_DEBUG);
   LogComponentEnable("WimaxInstaller", LOG_LEVEL_INFO);
   LogComponentEnable("WimaxVehicleInstaller", LOG_LEVEL_INFO);
   LogComponentEnable("WimaxBsInstaller", LOG_LEVEL_INFO);
   LogComponentEnable("WimaxApp", LOG_LEVEL_INFO);
   LogComponentEnable("IPCIUFacilities",LOG_LEVEL_LOGIC);
   LogComponentEnable("iTETRISns3Facilities", LOG_LEVEL_LOGIC);
//   LogComponentEnable("BsCommandManager", LOG_LEVEL_INFO);
//   LogComponentEnable("BaseStationNetDevice", LOG_LEVEL_INFO);
//   LogComponentEnable("SubscriberStationNetDevice", LOG_LEVEL_INFO);
//   LogComponentEnable("iTETRISApplication", LOG_LEVEL_INFO);
//   LogComponentEnable("IpcsClassifier", LOG_LEVEL_INFO);
//   LogComponentEnable("SSScheduler", LOG_LEVEL_INFO);
//   LogComponentEnable("ServiceFlowManager", LOG_LEVEL_INFO);
//   LogComponentEnable("ServiceFlow", LOG_LEVEL_INFO);
//   LogComponentEnable("IpcsClassifierRecord", LOG_LEVEL_INFO);
//   LogComponentEnable("WimaxMacQueue", LOG_LEVEL_INFO);
//   LogComponentEnable ("PacketManager",LOG_LEVEL_DEBUG);


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
      fileConfTechnologies = "scratch/configTechnologies-wimax.xml";
    }

  ConfigurationManagerXml confManager (fileConfTechnologies);
  iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();
  confManager.ReadFile(nodeManager);

  nodeManager->CreateItetrisNode (Vector(0,0,0)); // ID = 0
  nodeManager->InstallCommunicationModule ("WimaxVehicle");
  nodeManager->ActivateNode (0);

  nodeManager->CreateItetrisNode (Vector(0,0,0)); // ID = 1
  nodeManager->InstallCommunicationModule ("WimaxBs");

  nodeManager->CreateItetrisNode (Vector(10000,0,0)); // ID = 2
  nodeManager->InstallCommunicationModule ("WimaxBs");

  NodeContainer mobileNodes = nodeManager->GetItetrisNodes ();
  PacketManager* packetManager = new PacketManager ();
  packetManager->SetNodeManager(nodeManager);

  Simulator::Schedule (Seconds (1.0), &CheckMobility,mobileNodes.Get(0));

  Simulator::Schedule (Seconds (50), &InitiateIpCiuTxon,packetManager,50,1,ID_BROADCAST);	
  Simulator::Schedule (Seconds (150), &InitiateIpCiuTxon,packetManager,50,2,0);	
  Simulator::Schedule (Seconds (250), &ActivateIdBasedTxon,packetManager,50,0,2);
  
  
  //Check the received data packets of the nodes with ID 0,1 and 2
  Simulator::Schedule (Seconds (2999.0), &ReceivePackets,packetManager,2);
  Simulator::Schedule (Seconds (2999.0), &ReceivePackets,packetManager,1);
  Simulator::Schedule (Seconds (2999.0), &ReceivePackets,packetManager,0);
  
  Simulator::Stop (Seconds (sim_time));
 // Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/RxOk", MakeCallback (&PhyRxOkTrace));
 // Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/Tx", MakeCallback (&PhyTxTrace));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
