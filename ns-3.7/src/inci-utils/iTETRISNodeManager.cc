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
 * Author: Ramon Bauza <rbauza@umh.es>, Michele Rondinone <mrondinone@umh.es>
 */

#include "ns3/mobility-helper.h"
#include "iTETRISNodeManager.h"
#include "ns3/log.h"
#include <iostream>
#include "ns3/CAMmanagement.h" 
#include "ns3/node-container.h"
#include "ns3/vehicle-sta-mgnt.h"
#include "ns3/itetris-mobility-model.h" 

using namespace std;

NS_LOG_COMPONENT_DEFINE ("iTETRISNodeManager");

namespace ns3
{
    
iTETRISNodeManager::iTETRISNodeManager(){} 

uint32_t iTETRISNodeManager::CreateItetrisNode (Vector position)
{
  CreateItetrisNode ();
  Ptr<Node> node = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
  Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();
  mobModel->SetPosition(position);
  Vector newPos= mobModel->GetPosition();
  NS_LOG_DEBUG ( "ns-3 server --> node position= ("<< newPos.x << ", " << newPos.y << ")" ); 
  return node->GetId();	
}

uint32_t iTETRISNodeManager::CreateItetrisNode (const Vector &position, const float &speed, const float & heading, const std::string &laneId)
{
  CreateItetrisNode ();
  Ptr<Node> node = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
  Ptr<ItetrisMobilityModel> itetrisMobModel = node->GetObject<ItetrisMobilityModel> ();
  if (itetrisMobModel == NULL)
    {
      Ptr<MobilityModel> basicMobModel = node->GetObject<MobilityModel> (); 
      basicMobModel->SetPosition (position);
      return node->GetId();
    } 
  itetrisMobModel->SetPositionAndSpeed (position, speed, heading, GetEdgeId (laneId), laneId);
  Vector newPos= itetrisMobModel->GetPosition();
  NS_LOG_DEBUG ( "ns-3 server --> node position= ("<< newPos.x << ", " << newPos.y << ")" );
  return node->GetId();	
}

void iTETRISNodeManager::CreateItetrisNode (void)
{
  m_iTETRISNodes.Create (1);
  Ptr<Node> singleNode = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
  NodeContainer singleNodeContainer;
  singleNodeContainer.Add(singleNode);
  vector<Ptr<CommModuleInstaller> >::iterator it;
  for (it = m_defaultModules.begin(); it < m_defaultModules.end(); it++)
  {
    (*it)->Install(singleNodeContainer);
  }
}


void iTETRISNodeManager::CreateItetrisTMC (void)
{
  m_iTETRISNodes.Create (1);
  Ptr<Node> singleNode = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);
  NodeContainer singleNodeContainer;
  singleNodeContainer.Add(singleNode);
  Ptr<CommModuleInstaller> comInstaller = GetInstaller ("TMC");
  comInstaller->Install(singleNodeContainer);
}

NodeContainer*
iTETRISNodeManager::InstallCommunicationModule (std::string typeOfModule)
{
  NodeContainerList::iterator iterCommModule = m_itetrisTechNodes.find(typeOfModule);
  if( iterCommModule != m_itetrisTechNodes.end() ) 
    {
      NodeContainer* container = iterCommModule->second;      
      InstallerContainerList::iterator iterInstaller = m_itetrisInstallers.find(typeOfModule);
      if( iterInstaller != m_itetrisInstallers.end() ) 
        {	  
          Ptr<CommModuleInstaller> installer = iterInstaller->second;	 
          Ptr<Node> singleNode = m_iTETRISNodes.Get(m_iTETRISNodes.GetN() - 1);	  
          NodeContainer singleNodeContainer;
          singleNodeContainer.Add(singleNode);
	  installer->Install(singleNodeContainer);
	  container->Add(singleNode);	  	  
	  return (container);
        }
      NS_FATAL_ERROR ( "Communication module installer not found in iTETRISNodeManager - " << typeOfModule );
      return (NULL);
    }
  NS_FATAL_ERROR ( "Communication module not found in iTETRISNodeManager - " << typeOfModule );
  return (NULL);  
}

void
iTETRISNodeManager::AttachInstaller (std::string typeOfModule, Ptr<CommModuleInstaller> installer)
{
  InstallerContainerList::iterator iter = m_itetrisInstallers.find(typeOfModule);
  if( iter != m_itetrisInstallers.end() ) 
    {
      return;
    }
  m_itetrisInstallers.insert (std::make_pair(typeOfModule, installer));    
  NodeContainer* newContainer = new NodeContainer ();
  m_itetrisTechNodes.insert (std::make_pair(typeOfModule, newContainer));
}

NodeContainer 
iTETRISNodeManager::GetItetrisNodes (void)
{
  return (m_iTETRISNodes);
}

NodeContainer* 
iTETRISNodeManager::GetItetrisTechNodes (std::string typeOfModule)
{
  NodeContainerList::iterator iterCommModule = m_itetrisTechNodes.find(typeOfModule);
  if( iterCommModule != m_itetrisTechNodes.end() ) 
    {
      return(iterCommModule->second);
    }
  else
    {
      return (NULL);
    }
}

void 
iTETRISNodeManager::SetDefaultModule (std::string typeOfModule)
{
  InstallerContainerList::iterator iter = m_itetrisInstallers.find(typeOfModule);
  if( iter != m_itetrisInstallers.end() ) 
    {
      m_defaultModules.push_back(iter->second);
    }
}

void
iTETRISNodeManager::UpdateNodePosition (uint32_t nodeId, Vector position) 
{
  Ptr<Node> node = m_iTETRISNodes.Get(nodeId);  
  Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();  
  mobModel->SetPosition(position);  
}

void
iTETRISNodeManager::UpdateNodePosition (uint32_t nodeId, const Vector &position, const float &speed, const float & heading, const std::string &laneId) 
{
  Ptr<Node> node = m_iTETRISNodes.Get(nodeId);  
  Ptr<ItetrisMobilityModel> itetrisMobModel = node->GetObject<ItetrisMobilityModel> ();  
  if (itetrisMobModel == NULL)
    {
      Ptr<MobilityModel> basicMobModel = node->GetObject<MobilityModel> (); 
      basicMobModel->SetPosition (position);
      return;
    } 
  itetrisMobModel->SetPositionAndSpeed (position,speed, heading, GetEdgeId (laneId), laneId);  
}

Ptr<Node> 
iTETRISNodeManager::GetItetrisNode (uint32_t nodeId)
{
  Ptr<Node> node = NULL;
  if ( (nodeId <= (m_iTETRISNodes.GetN () - 1)) && (m_iTETRISNodes.GetN () > 0) )
    {
      node = m_iTETRISNodes.Get(nodeId); 
    }
  return (node);
}

Ptr<CommModuleInstaller> 
iTETRISNodeManager::GetInstaller (std::string typeOfModule)
{
  Ptr<CommModuleInstaller> installer = NULL;
  InstallerContainerList::iterator iterInstaller = m_itetrisInstallers.find(typeOfModule);
  if( iterInstaller != m_itetrisInstallers.end() ) 
    {	  
      installer = iterInstaller->second;	
    }
  return installer;
}

bool
iTETRISNodeManager::ActivateNode (uint32_t nodeId)
{
  Ptr<Node> node = GetItetrisNode (nodeId);
  if (node)
    {
      if (node->IsMobileNode ())
	{
	  Ptr<VehicleStaMgnt> staMgnt = node->GetObject<VehicleStaMgnt> ();
	  NS_ASSERT_MSG (staMgnt, "VehicleStaMgnt object not found in the vehicle");
	  staMgnt->ActivateNode();
	  return true;
	}
      else
	{
	  cout << "[iTETRISNodeManager::ActivateNode] Only vehicles can be activated. Node with ID=" << nodeId<< " is not a vehicle."<<endl;
	  return false;
	}
    }
  return false;
}

bool 
iTETRISNodeManager::DeactivateNode (uint32_t nodeId)
{
  Ptr<Node> node = GetItetrisNode (nodeId);
  if (node)
    {
      if (node->IsMobileNode ())
	{
	  Ptr<VehicleStaMgnt> staMgnt = node->GetObject<VehicleStaMgnt> ();
	  NS_ASSERT_MSG (staMgnt, "VehicleStaMgnt object not found in the vehicle");
	  staMgnt->DeactivateNode();
	  return true;
	}
    }
  return false;
}

bool iTETRISNodeManager::IsNodeActive (uint32_t nodeId)
{
  Ptr<Node> node = GetItetrisNode (nodeId);
  if (node)
    {
      if (node->IsMobileNode ())
	{
	  Ptr<VehicleStaMgnt> staMgnt = node->GetObject<VehicleStaMgnt> ();
	  NS_ASSERT_MSG (staMgnt, "VehicleStaMgnt object not found in the vehicle");
	  return staMgnt->IsNodeActive();
	}
    }

  return true; // Fixed nodes are considered to be always active
}

std::string 
iTETRISNodeManager::GetEdgeId (std::string laneId)
{
  string edgeId = "";
  size_t found = laneId.find ("_"); 
  size_t pos = string::npos;
  
  while (found != string::npos)
    {
      pos = found;
      found = laneId.find ("_",found+1,1); 
    }
  
  if (pos != string::npos)
    {
      edgeId = laneId.substr (0,pos);
    }
  return edgeId;
}

}
