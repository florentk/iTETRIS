/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez,
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
 * Author:  Michele Rondinone <mrondinone@umh.es>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
 */

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/service-list.h"
#include "ns3/service-management.h"
#include "ns3/service-list.h"
#include "ns3/itetris-types.h"
#include "ns3/ipv4-address.h"
#include "ns3/addressing-support.h"
#include "ns3/c2c-address.h"
#include "ns3/mw-comm-ch-selector.h"
#include "ns3/node-container.h"

#include "MWFacilities.h"

NS_LOG_COMPONENT_DEFINE ("MWFacilities");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MWFacilities);


TypeId 
MWFacilities::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MWFacilities")
    .SetParent<Object> ()
    .AddConstructor<MWFacilities> ()
    ;
  return tid;
}

MWFacilities::MWFacilities ()
{
}

MWFacilities::~MWFacilities ()
{
}

void
MWFacilities::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  Object::DoDispose ();
}

void 
MWFacilities::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;
}

void
MWFacilities::NotifyNewAggregate ()
{
  if (m_node == 0)
    {
      Ptr<Node>node = this->GetObject<Node>();
      // verify that it's a valid node and that
      // the node has not been set before
      if (node != 0)
        {
          this->SetNode (node);
        }
    }
  Object::NotifyNewAggregate ();
}

void   
MWFacilities::InitiateMWGeoBasedTxon (std::string serviceId, uint32_t commProfile, TechnologyList technologies, CircularGeoAddress destination, double frequency, uint32_t packetSize, double msgRegenerationTime, uint8_t msglifetime)
{
  // retrieve the dissemination profile through the MW communication channel selector
  m_disseminationProfile = m_MWCOMMchSelector -> GetDisseminationProfile(commProfile, destination, technologies);
  Ptr<Node> disseminator = m_disseminationProfile.disseminator;
  if (disseminator != NULL)
  {
    Ptr<ServiceManagement> servMngmt = disseminator->GetObject<ServiceManagement> ();
    Ptr<AddressingSupport> addrsupp = disseminator->GetObject<AddressingSupport> ();
    // if the selected disseminator belongs to the IP-based base stations, then retrieve the adequate IP address for disseminating (broadcast)
    if(m_disseminationProfile.stack == IPv4)
      {
        Ipv4Address* IPaddress = addrsupp->getIPaddress(ID_BROADCAST);
        // finally activate the service on the selected IP basestation
        NS_ASSERT_MSG (IPaddress, "MWFacilities::InitiateMWGeoBasedTxon -> IP address not found.");
        servMngmt-> ActivateIPService(/*m_disseminationProfile.tech+"-"+*/serviceId, *IPaddress, frequency, msgRegenerationTime, packetSize);
      }
      // if the selected disseminator belongs to the C2C-based base stations (an RSU), then retrieve the adequate c2c address  
    if(m_disseminationProfile.stack == C2C)
    {
      Ptr<c2cAddress> c2caddress = addrsupp->getC2CGeoBroadcastAddress(destination);
      // finally activate the service on the selected c2c base station
      NS_ASSERT_MSG (c2caddress, "MWFacilities::InitiateMWGeoBasedTxon -> C2C address not found.");
      servMngmt-> ActivateC2CService(/*m_disseminationProfile.tech+"-"+*/serviceId, c2caddress, frequency, msgRegenerationTime, msglifetime,  packetSize);
    }
  }
}

void   
MWFacilities::DeactivateServiceTxon (std::string serviceId)
{
  Ptr<Node> disseminator = m_disseminationProfile.disseminator;
  Ptr<ServiceManagement> servMngmt = disseminator->GetObject<ServiceManagement> ();
  servMngmt-> DeactivateService(serviceId); 
}

void 
MWFacilities::SetMWCommCHSelector( Ptr<MWCOMMchSelector> MWCHsel)
{
  m_MWCOMMchSelector=MWCHsel;
}

/*void
MWFacilities::SetiTETRISNodeManager( Ptr<iTETRISNodeManager> nodeManager)
{
  m_nodeManager=nodeManager;
}*/

void
MWFacilities::AddInfrastructureTechNode(Ptr<Node> node, std::string typeOfModule) 
{
	NodeContainerList::iterator iterCommModule = m_infranodes.find(typeOfModule);
	if( iterCommModule != m_infranodes.end() )  {
		NodeContainer* container = iterCommModule->second;
		container->Add(node);
	}
	else {
		NodeContainer* newContainer = new NodeContainer (node);
		m_infranodes.insert (std::make_pair(typeOfModule, newContainer));
	}
}

NodeContainer*
MWFacilities::getInfrastructureTechNodes(std::string typeOfModule)
{
	NodeContainerList::iterator iterCommModule = m_infranodes.find(typeOfModule);
	if( iterCommModule != m_infranodes.end() )  {
		return(iterCommModule->second);
	}
	else {
		return (NULL);
	}
}


} //namespace ns3
