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
#include "ns3/contrib-module.h" // Added by Ramon Bauza
#include "ns3/inci-utils-module.h" // Added by Ramon Bauza
#include <exception>

using namespace ns3;

int main (int argc, char *argv[])
{
  // Added by Ramon Bauza
  
  /**
   * Two different configuration files can be used: a file (fileGeneralParameters) with the default attribute values to be used 
   * in the simulation and a file (fileConfTechnologies) with the configuration of the iTETRIS scenario. The file paths can be passed 
   * from the command line.
   */
  std::string fileGeneralParameters = "";
  std::string fileConfTechnologies = "";
  std::string inciPort = "";
  std::string logFile = "";
  CommandLine cmd;
  cmd.AddValue("fileGeneralParameters", "Path to the configuration file", fileGeneralParameters);
  cmd.AddValue("fileConfTechnologies", "Path to the configuration file", fileConfTechnologies);
  cmd.AddValue("inciPort", "iNCI listening socket port number", inciPort);
  cmd.AddValue("logFile", "Log file path", logFile);
  cmd.Parse (argc, argv);

  LogComponentEnable ("ConfigurationManager",LOG_LEVEL_DEBUG);
 
  if (fileGeneralParameters != "")
    {
      Config::SetDefault ("ns3::ConfigStore::Filename", StringValue (fileGeneralParameters));
      Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
      ConfigStore config;
      config.ConfigureDefaults ();
    }

  if (fileConfTechnologies == "")
    {
      fileConfTechnologies = "scratch/configTechnologies.txt";
    }

  // End addition

#ifdef _DEBUG

#endif
  
  //Check port in input
  if (argc < 5)
  {
    std::cout << "No port or log file for ns-3 server introduced." << std::endl;
    std::cout << "Format is: main-inci3 [port-number] [fileGeneralParameters] [fileConfTechnologies] OPTIONAL [log-path]" << std::endl;
    std::cout << "Exiting..." << std::endl;
    return 0;
  }

  std::cout << inciPort << std::endl;
  int port = atoi(inciPort.c_str());   

  // Added by Ramon Bauza
  ConfigurationManager confManager (fileConfTechnologies);
  iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();
  PacketManager* packetManager = new PacketManager (); // Added by Ramon Bauza
  confManager.ReadFile(nodeManager);
  // End addition

  try {
    ns3::Ns3Server::processCommandsUntilSimStep(port, logFile, nodeManager, packetManager); 
  } catch (int e)
  {
    std::cout << "Exiting... " << std::endl;
    return 0;
  }

  //Simulator::Run ();
  
  Simulator::Destroy ();
  
  return 0;
}
