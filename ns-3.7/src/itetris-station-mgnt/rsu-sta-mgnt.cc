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

#include "ns3/log.h"
#include "rsu-sta-mgnt.h"
#include "ns3/node.h"
#include "ns3/c2c-address.h"
#include "ns3/location-table.h"
#include "ns3/itetris-types.h"

NS_LOG_COMPONENT_DEFINE ("RsuStaMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (RsuStaMgnt);

TypeId RsuStaMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RsuStaMgnt")
    .SetParent<Object> ()                  
    ;
  return tid;
}

RsuStaMgnt::~RsuStaMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ptr<c2cAddress> 
RsuStaMgnt::GetC2cAddress (uint32_t nodeId) const
{
  Ptr<c2cAddress> resAddress = NULL; 
  if (nodeId == ID_BROADCAST)
    { 
      resAddress = CreateObject<c2cAddress> (); 
      NS_LOG_INFO ("[RsuStaMgnt::GetC2cAddress] Broadcast C2C address");
      resAddress->Set(c2cAddress::BROAD, 1); // One-hop topo-broadcast
      return (resAddress);
    }
  else
    {
      Ptr<LocationTable> locationTable = m_node->GetObject <LocationTable> ();
      if (locationTable != NULL)
	{
	  LocationTable::Table table = locationTable->GetTable ();
	  NS_LOG_INFO ("[RsuStaMgnt::GetC2cAddress] Looking up c2cAdress of node "<< nodeId << " in neighbor table of node " << m_node->GetId ());
	  for (LocationTable::Table::const_iterator iter = table.begin(); iter < table.end(); iter++)
	    {
	      if ((*iter).gnAddr == nodeId)
		{
		  resAddress = CreateObject<c2cAddress> (); 
		  NS_LOG_INFO ("Node found with Id "<< nodeId );
		  resAddress->Set(nodeId, (*iter).Lat, (*iter).Long);
		  return (resAddress);
		}
	    }
	}
    }
  return (resAddress);
}

void 
RsuStaMgnt::SetNode (Ptr<Node> node)
{
  m_node = node;
}

uint32_t 
RsuStaMgnt::GetNodeId ()
{
  return m_node->GetId();
}

uint32_t 
RsuStaMgnt::GetNumberOfNodesInCoverage (void) const
{
  Ptr<LocationTable> locationTable = m_node->GetObject <LocationTable> ();
  if (locationTable != NULL)
    {
      return (locationTable->GetNbNeighs ());
    }
  return (0);
}

double
RsuStaMgnt::GetCoverageRange (void) const
{
	// TODO complete the CoverageRange of RSUSTAMANAGER
	//return DynamicCast<WifiRemoteStationManager>(DynamicCast<WifiNetDevice>(m_netDevice)->GetManager())->GetCoverageRange();
	return (300.0); // JHNote (24.01.2011) - hardcoded at this time - the method is not available in the netDevice and does not make much sense
}

} // namespace ns3
