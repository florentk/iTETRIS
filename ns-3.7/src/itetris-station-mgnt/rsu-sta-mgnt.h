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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#ifndef RSU_STA_MGNT_H
#define RSU_STA_MGNT_H

#include "ns3/simulator.h"
#include "ns3/object.h"

namespace ns3
{

class c2cAddress;
class Node;

class RsuStaMgnt : public Object
{
  public:
    static TypeId GetTypeId (void);
    virtual ~RsuStaMgnt();

    /**
     * Function called from the facility Addressing Support to retrieve
     * the c2cAddress (nodeId+position) of a given node. The function looks up 
     * the nodeId in the rsu's Neighbors table.
     */
    Ptr<c2cAddress> GetC2cAddress (uint32_t nodeId) const;

    uint32_t GetNumberOfNodesInCoverage (void) const;
    double 	 GetCoverageRange (void) const;

    void SetNode (Ptr<Node> node);

    uint32_t GetNodeId ();
  private:
    Ptr<Node> m_node;

};

}

#endif
