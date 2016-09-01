/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/simulator-module.h"
#include "ns3/node-module.h"
#include "ns3/helper-module.h"
#include "ns3/inci-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main (int argc, char *argv[])
{

  int port;
  //Check port in input
  if (argc < 1)
  {
    std::cout << "No port for ns-3 server introduced." << std::endl;
    std::cout << "Exiting..." << std::endl;
    return 0;
  }
  port = atoi(argv[0]);

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  ns3::Ns3Server::processCommandsUntilSimStep(port, nodes);

  /*NodeContainer::Iterator it;
  for(it=nodes.Begin(); it<nodes.End(); it++)
  {
    Ptr<Node> n = (*it);
    std::cout << "Created node with ID: " << n->GetId() << std::endl;
  }*/

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));  

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));  

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  MobilityHelper mobility;

  /*mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
    "MinX", DoubleValue (0.0),
    "MinY", DoubleValue (0.0),
    "DeltaX", DoubleValue (5.0),
    "DeltaY", DoubleValue (10.0),
    "GridWidth", UintegerValue (3),
    "LayoutType", StringValue ("RowFirst"));*/

  /*mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
    "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));*/

  //Ptr<ListPositionAllocator> allocator;
  //Vector v(-500,0,0);
  //allocator->Add(v);

  //allocator->Add(Vector(-500,0,0));  
  //mobility.SetPositionAllocator (allocator);    

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");  
  
  mobility.Install (nodes);

  MobilityHelper::EnableAsciiAll(std::cout);

  ns3::Ns3Server::processCommandsUntilSimStep(port, nodes);

  //Simulator::Run ();
  
  Simulator::Destroy ();
  
  return 0;
}
