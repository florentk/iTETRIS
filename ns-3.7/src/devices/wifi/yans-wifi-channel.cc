/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA,
 *            2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *            University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Mathieu Lacage, <mathieu.lacage@sophia.inria.fr>, Ramon Bauza <rbauza@umh.es>
 */
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/object-factory.h"
#include "yans-wifi-channel.h"
#include "yans-wifi-phy.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"

NS_LOG_COMPONENT_DEFINE ("YansWifiChannel");

namespace ns3 {

TypeId 
YansWifiChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::YansWifiChannel")
    .SetParent<WifiChannel> ()
    .AddConstructor<YansWifiChannel> ()
    .AddAttribute ("PropagationLossModel", "A pointer to the propagation loss model attached to this channel.",
                   PointerValue (),
                   MakePointerAccessor (&YansWifiChannel::m_loss),
                   MakePointerChecker<PropagationLossModel> ())
    .AddAttribute ("PropagationDelayModel", "A pointer to the propagation delay model attached to this channel.",
                   PointerValue (),
                   MakePointerAccessor (&YansWifiChannel::m_delay),
                   MakePointerChecker<PropagationDelayModel> ())
    ;
  return tid;
}

YansWifiChannel::YansWifiChannel ()
  : m_interferenceRangeVehicle (3000), 
    m_interferenceRangeCiu (5000)
{}

YansWifiChannel::~YansWifiChannel ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_phyList.clear ();
}

void 
YansWifiChannel::SetPropagationLossModel (Ptr<PropagationLossModel> loss)
{
  m_loss = loss;
}
void 
YansWifiChannel::SetPropagationDelayModel (Ptr<PropagationDelayModel> delay)
{
  m_delay = delay;
}

void 
YansWifiChannel::Send (Ptr<YansWifiPhy> sender, Ptr<const Packet> packet, double txPowerDbm,
                       WifiMode wifiMode, WifiPreamble preamble) const
{
  Ptr<MobilityModel> senderMobility = sender->GetMobility ()->GetObject<MobilityModel> ();
  NS_ASSERT (senderMobility != 0);
  uint32_t j = 0;
  for (PhyList::const_iterator i = m_phyList.begin (); i != m_phyList.end (); i++, j++)
    { 
      if (sender != (*i))
        {
          Ptr<MobilityModel> receiverMobility = (*i)->GetMobility ()->GetObject<MobilityModel> ();
	  
          NS_LOG_DEBUG ("   Does node " << receiverMobility->GetNode()->GetId()<< " process the incoming packet? from node " << senderMobility->GetNode()->GetId());
          // For now don't account for inter channel interference
          if ((*i)->GetChannelNumber() != sender->GetChannelNumber())
	    continue;

	  if (!(*i)->IsNodeActivated()) 
	  {
	    NS_LOG_DEBUG ("   no, it is off");
	    continue;
	  }

	  if (senderMobility->GetNode()->IsMobileNode() && receiverMobility->GetNode()->IsMobileNode())
	    {
	      // Both nodes are vehicles
	      if (senderMobility->GetDistanceFrom(receiverMobility) > m_interferenceRangeVehicle)
	      {
		NS_LOG_DEBUG ("   no, it is out of the interference range");
		continue;
	      }
	    }
	  else
	    {
	      // One of the nodes is a CIU
	      if (senderMobility->GetDistanceFrom(receiverMobility) > m_interferenceRangeCiu)
	      {
		NS_LOG_DEBUG ("   no, it is out of the interference range");
		continue;
	      }
	    }

          Time delay = m_delay->GetDelay (senderMobility, receiverMobility);
          double rxPowerDbm = m_loss->CalcRxPower (txPowerDbm, senderMobility, receiverMobility);
          NS_LOG_DEBUG (" yes it does. propagation: txPower="<<txPowerDbm<<"dbm, rxPower="<<rxPowerDbm<<"dbm, "<<
                        "distance="<<senderMobility->GetDistanceFrom (receiverMobility)<<"m, delay="<<delay);
          Ptr<Packet> copy = packet->Copy ();
          Ptr<Object> dstNetDevice = m_phyList[j]->GetDevice ();
          uint32_t dstNode;
          if (dstNetDevice == 0)
            {
              dstNode = 0xffffffff;
            }
          else
            {
              dstNode = dstNetDevice->GetObject<NetDevice> ()->GetNode ()->GetId ();
            }
          Simulator::ScheduleWithContext (dstNode,
                                          delay, &YansWifiChannel::Receive, this, 
                                          j, copy, rxPowerDbm, wifiMode, preamble);
        }
    }
}

void
YansWifiChannel::Receive (uint32_t i, Ptr<Packet> packet, double rxPowerDbm,
                          WifiMode txMode, WifiPreamble preamble) const
{
  m_phyList[i]->StartReceivePacket (packet, rxPowerDbm, txMode, preamble);
}

uint32_t 
YansWifiChannel::GetNDevices (void) const
{
  return m_phyList.size ();
}
Ptr<NetDevice> 
YansWifiChannel::GetDevice (uint32_t i) const
{
  return m_phyList[i]->GetDevice ()->GetObject<NetDevice> ();
}

void 
YansWifiChannel::Add (Ptr<YansWifiPhy> phy)
{
  m_phyList.push_back (phy);
}

void 
YansWifiChannel::SetInterferenceRangeVehicle (float range)
{
  m_interferenceRangeVehicle = range;
}

void 
YansWifiChannel::SetInterferenceRangeCiu (float range)
{
  m_interferenceRangeCiu = range;
}

} // namespace ns3
