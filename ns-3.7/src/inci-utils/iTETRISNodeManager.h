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

#ifndef ITETRISNODEMANAGER_H
#define ITETRISNODEMANAGER_H

#include "ns3/mobility-model.h"
#include "comm-module-installer.h"
#include <map>

namespace ns3
{

class NodeContainer;
class Node;

/**
 * @class iTETRISNodeManager 
 * @brief The class iTETRISNodeManager manages the creation, the initial placement, and the position updates of ns3 nodes. The additions of communication modules (technologies, protocols stacks, etc.) is perfomed through installers.
 */
class iTETRISNodeManager
{
  public:
    iTETRISNodeManager (void);

    /**
     * @brief Create a single node
     */
    void CreateItetrisNode (void); 
    uint32_t CreateItetrisNode (Vector position); 
    uint32_t CreateItetrisNode (const Vector &position, const float &speed, const float & heading, const std::string &laneId); 
    void CreateItetrisTMC (void);

    /**
     * @brief Get all the iTETRIS nodes
     */
    NodeContainer GetItetrisNodes (void);

    /**
     * @brief Get a NodeContainer pointer of the nodes with the corresponding communication module (typeOfModule)
     */
    NodeContainer* GetItetrisTechNodes (std::string typeOfModule);

    /**
     * @brief Attach a new communication module Installer to the NodeManager
     */
    void AttachInstaller (std::string typeOfModule, Ptr<CommModuleInstaller> installer); 

    /**
     * @brief Install a new communication module ('typeOfModule') in the last node that has been created
     */
    NodeContainer* InstallCommunicationModule (std::string typeOfModule); 

    /**
     * @brief Get the installer for the communication module ('typeOfModule') 
     */
    Ptr<CommModuleInstaller> GetInstaller (std::string typeOfModule);

    /**
     * @brief Set the default modules that will be installed in each new node that is created
     */
    void SetDefaultModule (std::string typeOfModule);

    /**
     * @brief Update node's position
     */
    void UpdateNodePosition(uint32_t nodeId, Vector position); 
    void UpdateNodePosition (uint32_t nodeId, const Vector &position, const float &speed, const float & heading, const std::string &laneId); 

    Ptr<Node> GetItetrisNode (uint32_t nodeId);

    bool ActivateNode (uint32_t nodeId);
    bool DeactivateNode (uint32_t nodeId);
    bool IsNodeActive (uint32_t nodeId);

  private:

    std::string GetEdgeId (std::string laneId);

    /**
     * @brief Node container with all the iTETRIS nodes
     */
    NodeContainer m_iTETRISNodes; 

    typedef std::map<std::string, Ptr<CommModuleInstaller> > InstallerContainerList; 
    typedef std::map<std::string, NodeContainer*> NodeContainerList; 

    /**
     * @brief List of NodeContainers with a NodeContainer per communication module (e.g. WAVE, DVB-H, etc.)
     */
    NodeContainerList m_itetrisTechNodes; 

    /**
     * @brief List of communication module installers. 
     */
    InstallerContainerList m_itetrisInstallers;  

    std::vector<Ptr<CommModuleInstaller> > m_defaultModules;
 
};

}

#endif
