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
#include "ip-base-sta-mgnt.h"
#include "ns3/itetris-types.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-interface-address.h"

NS_LOG_COMPONENT_DEFINE ("IpBaseStaMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (IpBaseStaMgnt);

TypeId IpBaseStaMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::IpBaseStaMgnt")
    .SetParent<Object> ()                
    ;
  return tid;
}

IpBaseStaMgnt::IpBaseStaMgnt ()
  : m_nodeActive (false)
{}

IpBaseStaMgnt::~IpBaseStaMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void 
IpBaseStaMgnt::SetNetDevice (Ptr<NetDevice> netDevice)
{
  m_netDevice = netDevice;
}

Ptr<NetDevice> 
IpBaseStaMgnt::GetNetDevice (void)
{
  return m_netDevice;
}

void 
IpBaseStaMgnt::SetNode (Ptr<Node> node)
{
  m_node = node;
}

Ipv4Address* 
IpBaseStaMgnt::GetIpBroadcastAddress (void) const
{
  Ipv4Address* address = NULL;
  Ptr<Ipv4> ipStack = m_node->GetObject <Ipv4> (); 
  if (ipStack != NULL && ipStack->GetNInterfaces () > 1)
    {
      Ipv4InterfaceAddress ipInterface = ipStack->GetAddress (1,0);
      address = new Ipv4Address (ipInterface.GetBroadcast ());
      NS_LOG_INFO ("IP broadcast address  = " << *address);
    }
  return address;
}

bool 
IpBaseStaMgnt::IsNodeActive (void)
{
  return (m_nodeActive);
}

void 
IpBaseStaMgnt::ActivateNode (void)
{
  m_nodeActive = true;
}
    
void 
IpBaseStaMgnt::DeactivateNode (void)
{
  m_nodeActive = false;
}

} // namespace ns3
