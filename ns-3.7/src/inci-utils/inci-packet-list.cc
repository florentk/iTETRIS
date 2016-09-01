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

#include "inci-packet-list.h"
#include "inci-packet.h"
#include "ns3/node.h"
#include "ns3/log.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("InciPacketList");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (InciPacketList);

TypeId InciPacketList::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::InciPacketList")
    .SetParent<Object> () 
    .AddConstructor<InciPacketList> ()                 
    ;
  return tid;
}
    
InciPacketList::InciPacketList ()
{} 

bool 
InciPacketList::ReceiveFromApplication (uint32_t senderId, std::string msgType, uint32_t ts, uint32_t tsSeqNo)
{
  NS_LOG_DEBUG ("Packet received in InciPacketList");
  NS_LOG_DEBUG ("SenderId = " << senderId << " msgType = " << msgType << " ts = " << ts << " tsSeqNo = " << tsSeqNo);
  InciPacket* newPacket = new InciPacket (senderId, msgType, ts, tsSeqNo);
  m_packetList.push_back (newPacket);
  return false;
}

bool 
InciPacketList::GetReceivedPacket (InciPacket &packet)
{
  if (m_packetList.size () == 0)
    {
      return false;
    }
  else
    {
      packet = *(m_packetList.back ());
      m_packetList.pop_back ();
      return true;
    }
}

void
InciPacketList::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  Object::DoDispose ();
}

void 
InciPacketList::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;
}

void
InciPacketList::NotifyNewAggregate ()
{
  if (m_node == 0)
    {
      Ptr<Node>node = this->GetObject<Node>();
      // Verify that it's a valid node and that the node has not been set before
      if (node != 0)
        {
          this->SetNode (node);
        }
    }
  Object::NotifyNewAggregate ();
}

}