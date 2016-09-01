/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, UMH, EU FP7 iTETRIS project
 *            2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *            University Miguel Hernandez, EU FP7 iTETRIS project
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
ReceivePackets (PacketManager* packetManager, uint32_t nodeId)
{
  struct InciPacket::ReceivedInciPacket inciPacket;
  std::cout << "*** iCS collecting received packets from ns-3 ***" << std::endl;
  while ( packetManager->GetReceivedPacket(nodeId,inciPacket) )
    {
      std::cout << "Node=" << nodeId << " at time=" << inciPacket.ts << " received a msgType="<< inciPacket.msgType 
      << " fromNode="<< inciPacket.senderId << std::endl;
    }
}

//The next Function sends a Unicast packet of size bytes from the sender to the destination, being the sender a Fixed Station. The static 1 values should not be changed

void InitiateIpCiuTxon (PacketManager* packetManager, uint32_t size, uint32_t sender, uint32_t destination)
{
   if (destination != ID_BROADCAST)
   {
      packetManager->InitiateIPCIUTxon (sender, "serviceIdUnicast", 1, size, destination, 1);
   }
   else
   {
      packetManager->InitiateIPCIUTxon (sender, "serviceIdBroadcast", 1, size, destination, 1);
   }
}
   

//The next Function sends a Unicast packet of size bytes from the sender to the destination,being the sender a Mobile Node. The static 1 values should not be changed
void ActivateIdBasedTxon (PacketManager* packetManager,uint32_t size, uint32_t sender, uint32_t destination)
{
   TechnologyList techList;
   techList.push_back("UmtsVehicle");
   packetManager->InitiateIdBasedTxon (sender, "serviceIdUnicast", 0, techList, 1, size, destination, 2, 1);
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

  LogComponentEnable ("PacketManager",LOG_LEVEL_LOGIC);
  LogComponentEnable ("VehicleStaMgnt",LOG_LEVEL_LOGIC);
//   LogComponentEnable ("ConfigurationManagerXml",LOG_LEVEL_DEBUG);
//   LogComponentEnable ("WaveInstaller",LOG_LEVEL_DEBUG);


  // Logging for UMTS
  LogComponentEnable ("UmtsApp",LOG_LEVEL_LOGIC);
  LogComponentEnable ("UmtsBsInstaller",LOG_LEVEL_LOGIC);
  LogComponentEnable ("UmtsVehicleInstaller",LOG_LEVEL_LOGIC);
  LogComponentEnable ("UmtsInstaller",LOG_LEVEL_LOGIC);
  LogComponentEnable ("iTETRISApplication",LOG_LEVEL_LOGIC);
  LogComponentEnable ("UmtsBaseStationManager",LOG_LEVEL_LOGIC);
  LogComponentEnable ("UmtsUserEquipmentManager",LOG_LEVEL_LOGIC);

 
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
  
  //Creation of Fixed Nodes indicating their position (X,Y,Z)
  
  nodeManager->CreateItetrisNode (Vector(200,50,0));
  nodeManager->InstallCommunicationModule ("UmtsBs");		//Id=0

  //Creation of Mobile nodes indicating their position (X,Y,Z)
  
  nodeManager->CreateItetrisNode (Vector(100,0,0));
  nodeManager->InstallCommunicationModule ("UmtsVehicle");	//Id=1
  nodeManager->ActivateNode (1);	//Id=1

  nodeManager->CreateItetrisNode (Vector(200,0,0));
  nodeManager->InstallCommunicationModule ("UmtsVehicle");	//Id=2
  nodeManager->ActivateNode (2);	//Id=2
  
//   nodeManager->CreateItetrisNode (Vector(500,100,0));
//   nodeManager->InstallCommunicationModule ("UmtsVehicle");	//Id=3
//   nodeManager->ActivateNode (3);	//Id=3
//   
//   nodeManager->CreateItetrisNode (Vector(250,50,0));
//   nodeManager->InstallCommunicationModule ("UmtsVehicle");	//Id=4
//   nodeManager->ActivateNode (4);	//Id=4

  
  NodeContainer mobileNodes = nodeManager->GetItetrisNodes ();

  PacketManager* packetManager = new PacketManager ();
  packetManager->SetNodeManager(nodeManager);
  
  //Start the data transmission from 2 nodes
  
    //Initiate Data Transmission from the Mobile Node 1 to the Fixed Station 0
  Simulator::Schedule (Seconds (5.0), &ActivateIdBasedTxon,packetManager,350,1,0);
   
  //Initiate Data Transmission from the Fixed Station 0 to the Mobile Node 1
  Simulator::Schedule (Seconds (1.0), &InitiateIpCiuTxon,packetManager,500,0,1);	
  
  //Initiate a broadcast Data Transmission from the Fixed Station 0 
  Simulator::Schedule (Seconds (8.0), &InitiateIpCiuTxon,packetManager,500,0, ID_BROADCAST);


  //Check the received data packets of the nodes with ID 0,1 and 2
  Simulator::Schedule (Seconds (10.0), &ReceivePackets,packetManager,2);
  Simulator::Schedule (Seconds (10.0), &ReceivePackets,packetManager,1);
  Simulator::Schedule (Seconds (10.0), &ReceivePackets,packetManager,0);

  // Update position of Node 1 to (X,Y,Z)=(550,0,0)
  nodeManager->UpdateNodePosition (1,Vector(550,0,0));

  Simulator::Stop (Seconds (sim_time));
  
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
