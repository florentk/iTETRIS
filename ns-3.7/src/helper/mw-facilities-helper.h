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

#ifndef MW_FACILITIES_HELPER_H
#define MW_FACILITIES_HELPER_H

#include "ns3/node-container.h"
#include "ns3/CAMmanage-helper.h"
#include "ns3/service-list-helper.h"
#include "ns3/service-list.h"
#include "ns3/net-device-container.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/object-factory.h"

namespace ns3 {

class Node;
               
/**
 * This helper is used to create and aggregate the functionalities of the iTETRIS ns-3 Facilities layer to MW Nodes.
 */
class MWFacilitiesHelper
{
public:
  /**
   * Create a new MWFacilitiesHelper which implements some specific
   * service helpers by default and include them in its private list
   * of service helpers .
   */
  MWFacilitiesHelper(void);

  /**
   * Destroy the FacilitiesHelper
   */
  virtual ~MWFacilitiesHelper(void);

  /**
   * Aggregate the implementations of the facilities classes on this
   *  node.
   * 
   * \param nodeName The node where the facilities have to be installed.
   */
  void Install (std::string nodeName);

  /**
   * Aggregate the implementations of the facilities classes on this
   * node.
   * 
   * \param node The node where the facilities have to be installed.
   */
  void Install (Ptr<Node> node);

  /**
   * Aggregate the implementations of the facilities classes on all the nodes of this
   * node container.
   * 
   * \param c NodeContainer containing the pointer to nodes where the facilities have to be installed
   */
  void Install (NodeContainer c) ;

private:

  static void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId);

};

} // namespace ns3

#endif /* MW_FACILITIES_HELPER_H*/