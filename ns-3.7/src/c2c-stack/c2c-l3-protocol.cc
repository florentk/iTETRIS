/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, HITACHI EUROPE SAS, EURECOM, EU FP7 iTETRIS project
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
 * Author: Vineet Kumar <Vineet.Kumar@hitachi-eu.com>, Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/callback.h"
#include "ns3/c2c-route.h"
#include "ns3/socketc2c.h"
#include "ns3/net-device.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/object-vector.h"
#include "ns3/boolean.h"
//#include "ns3/mobility-model.h"
#include "ns3/itetris-mobility-model.h"

#include "c2c-l3-protocol.h"
#include "c2c-l4-protocol.h"
#include "c2c-interface.h"

#include "channel-tag.h"
#include "node-id-tag.h" // Added by Ramon Bauza
#include "time-stamp-tag.h" // Added by Ramon Bauza

NS_LOG_COMPONENT_DEFINE ("c2cL3Protocol");

namespace ns3 {

const uint16_t c2cL3Protocol::PROT_NUMBER = 0x0707;
NS_OBJECT_ENSURE_REGISTERED (c2cL3Protocol);

TypeId 
c2cL3Protocol::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cL3Protocol")
    .SetParent<c2c> ()
    .AddConstructor<c2cL3Protocol> ()
    .AddTraceSource ("Drop",
                     "GeoNetwork Packet dropped.",
                     MakeTraceSourceAccessor (&c2cL3Protocol::m_dropTrace))
    .AddTraceSource ("Tx", "Send GeoNetwork packet to outgoing interface.",
                   MakeTraceSourceAccessor (&c2cL3Protocol::m_txTrace))
    .AddTraceSource ("Rx", "Receive GeoNetwork packet from incoming interface.",
                     MakeTraceSourceAccessor (&c2cL3Protocol::m_rxTrace))
    .AddTraceSource ("UnicastForward", "A unicast c2c packet was received by this node and is being forwarded to another node",
                     MakeTraceSourceAccessor (&c2cL3Protocol::m_unicastForwardTrace))
    .AddTraceSource ("LocalDeliver", "A c2c packet was received by/for this node, and it is being forward up the stack",
                     MakeTraceSourceAccessor (&c2cL3Protocol::m_localDeliverTrace))
    ;
  return tid;
}

c2cL3Protocol::c2cL3Protocol ()
: m_nInterfaces (0)
{
  NS_LOG_FUNCTION (this);
}

c2cL3Protocol::~c2cL3Protocol ()
{
  NS_LOG_FUNCTION (this);
}

void
c2cL3Protocol::Insert(Ptr<c2cL4Protocol> protocol)
{
	m_protocols.push_back (protocol);
}

Ptr<c2cL4Protocol>
c2cL3Protocol::GetProtocol(int protocolNumber) const
{
  for (L4List_t::const_iterator i = m_protocols.begin(); i != m_protocols.end(); ++i)
    {
      if ((*i)->GetProtocolNumber () == protocolNumber)
	{
	  return *i;
	}
    }
  return 0;
}

void
c2cL3Protocol::Remove (Ptr<c2cL4Protocol> protocol)
{
  m_protocols.remove (protocol);
}

void
c2cL3Protocol::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;

  //SetupLoopback ();
}

/*
 * This method is called by AddAgregate and completes the aggregation
 * by setting the node in the c2c stack
 */
void
c2cL3Protocol::NotifyNewAggregate ()
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
c2cL3Protocol::SetRoutingProtocol (Ptr<c2cRoutingProtocol> routingProtocol)
{
  NS_LOG_FUNCTION (this);
  m_routingProtocol = routingProtocol;
  m_routingProtocol->Setc2c (this);
}

Ptr<c2cRoutingProtocol>
c2cL3Protocol::GetRoutingProtocol (void) const
{
  return m_routingProtocol;
}

void
c2cL3Protocol::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  for (L4List_t::iterator i = m_protocols.begin(); i != m_protocols.end(); ++i)
    {
      *i = 0;
    }
  m_protocols.clear ();

  for (c2cInterfaceList::iterator i = m_interfaces.begin (); i != m_interfaces.end (); ++i)
    {
      *i = 0;
    }
  m_interfaces.clear ();
  m_node = 0;
  m_routingProtocol = 0;
  Object::DoDispose ();
}

/*
void
c2cL3Protocol::SetupLoopback (void)
{
  NS_LOG_FUNCTION_NOARGS ();

}
*/

uint32_t
c2cL3Protocol::AddInterface (NetDeviceType deviceType, Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << &device);

  //---------------- check iTETRIS ----------------------------
  NS_LOG_INFO ("c2cL3Protocol: Add Interface (NetDeviceType, NetDevice)");
  //---------------- check iTETRIS ----------------------------

  Ptr<Node> node = GetObject<Node> ();
  node->RegisterProtocolHandler (MakeCallback (&c2cL3Protocol::Receive, this), c2cL3Protocol::PROT_NUMBER, device, true);

  Ptr<c2cInterface> interface = CreateObject<c2cInterface> ();
  interface->SetNode (m_node);
  interface->SetDevice (device);
  interface->SetDeviceType(deviceType);

  m_devInterfaces.insert(std::make_pair(deviceType,device));

  return Addc2cInterface (interface);
}

uint32_t
c2cL3Protocol::AddInterface (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << &device);

  //---------------- check iTETRIS ----------------------------
  NS_LOG_INFO ("c2cL3Protocol: Add Interface (NetDevice)");
  //---------------- check iTETRIS ----------------------------

  Ptr<Node> node = GetObject<Node> ();
  node->RegisterProtocolHandler (MakeCallback (&c2cL3Protocol::Receive, this), c2cL3Protocol::PROT_NUMBER, device, true);

  Ptr<c2cInterface> interface = CreateObject<c2cInterface> ();
  interface->SetNode (m_node);
  interface->SetDevice (device);

  return Addc2cInterface (interface);
}

uint32_t
c2cL3Protocol::Addc2cInterface(Ptr<c2cInterface>interface)
{
  NS_LOG_FUNCTION (this << interface);
  uint32_t index = m_nInterfaces;
  m_interfaces.push_back (interface);
  m_nInterfaces++;
  return index;
}

Ptr<c2cInterface>
c2cL3Protocol::GetInterface (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  uint32_t tmp = 0;
  for (c2cInterfaceList::const_iterator i = m_interfaces.begin (); i != m_interfaces.end (); i++)
    {
      if (index == tmp)
	{
	  return *i;
	}
      tmp++;
    }
  return 0;
}

uint32_t
c2cL3Protocol::GetNInterfaces (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_nInterfaces;
}

int32_t
c2cL3Protocol::GetInterfaceForDevice (
  Ptr<const NetDevice> device) const
{
  NS_LOG_FUNCTION (this << device->GetIfIndex());

  int32_t interface = 0;
  for (c2cInterfaceList::const_iterator i = m_interfaces.begin ();
       i != m_interfaces.end ();
       i++, interface++)
    {
      if ((*i)->GetDevice () == device)
        {
          return interface;
        }
    }

  return -1;
}

// Added by Ramon Bauza
void 
c2cL3Protocol::InsertNodeIdAddressMap (uint32_t nodeId, Address address)
{
  if (m_nodeIdAddressMap.size() != 0)
    {
      for (NodeIdAddressMapList::iterator i = m_nodeIdAddressMap.begin (); i != m_nodeIdAddressMap.end (); i++)
        {
          if (i->nodeId == nodeId)
            {
              return;
            }
        }
    }
  struct NodeIdAddressMap idAddressMap;
  idAddressMap.nodeId = nodeId;
  idAddressMap.address = address;
  m_nodeIdAddressMap.push_back (idAddressMap);
  NS_LOG_LOGIC ("c2cL3Protocol: Inserting new entry in NodeIdAddressMapList. NodeId=" << nodeId << " Address=" <<   address);
}

// Added by Ramon Bauza
bool c2cL3Protocol::GetAddressByNodeId (uint32_t nodeId, Address &address)
{
  Address destAddress;
  if (m_nodeIdAddressMap.size() != 0)
    {
      for (NodeIdAddressMapList::iterator i = m_nodeIdAddressMap.begin (); i != m_nodeIdAddressMap.end (); i++)
        {
          if (i->nodeId == nodeId)
            {
              address = i->address;
              return (true);
            }
        }
    }
  return false;
}

void
c2cL3Protocol::Receive (Ptr<NetDevice> device, Ptr<const Packet> p, 
                        uint16_t protocol, const Address &from,
                        const Address &to, NetDevice::PacketType packetType)
{
  NS_LOG_FUNCTION (this << &device << p << protocol <<  from);
  NS_LOG_LOGIC ("Packet from " << from << " received on node " <<
  m_node->GetId ());

  uint32_t interface = 0;
  Ptr<Packet> packet = p->Copy ();

  //---------------- check iTETRIS ----------------------------
  NS_LOG_INFO ("");
  NS_LOG_INFO ("c2cL3Protocol: Receiving Local  NodeID = "<<device->GetNode()->GetId());
  NS_LOG_INFO ("c2cL3Protocol: Receiving From   Device = "<<from);
  //---------------- check iTETRIS ----------------------------

  Ptr<c2cInterface> c2cInterface;
  for (c2cInterfaceList::const_iterator i = m_interfaces.begin ();
       i != m_interfaces.end ();
       i++, interface++)
    {
      c2cInterface= *i;
      if (c2cInterface->GetDevice () == device)
        {
          //---------------- check iTETRIS ----------------------------
          NS_LOG_INFO ("c2cL3Protocol: Receiving Local  Device = "<<c2cInterface->GetDevice()->GetAddress());
          //---------------- check iTETRIS ----------------------------

          if (c2cInterface->IsUp ())
            {
              m_rxTrace (packet, interface);
              break;
            }
          else
            {
              NS_LOG_LOGIC ("Dropping received packet-- interface is down");
              c2cCommonHeader commonHeader;
              packet->RemoveHeader (commonHeader);
              m_dropTrace (commonHeader, packet, DROP_INTERFACE_DOWN, interface);
              return;
            }
        }
    }

  packet->Print(std::cout);
  NS_LOG_INFO ("");

  c2cCommonHeader commonHeader;
  packet->RemoveHeader (commonHeader);

  InsertNodeIdAddressMap (commonHeader.GetSourPosVector ().gnAddr, from); // Added by Ramon Bauza

  Ptr <LocationTable> ltable = m_node->GetObject<LocationTable> ();
  if (ltable != 0)
  ltable ->AddPosEntry (commonHeader);

   // Trim any residual frame padding from underlying devices
  if (commonHeader.GetLength() < packet->GetSize ())
   {
     packet->RemoveAtEnd (packet->GetSize () - commonHeader.GetLength());
   }


  ///////////////////////////////////////////////
  // Modification by Florent KAISSER 08/25/2016
  //
  // Here, the positions must be in degree,
  // then GetPosition of MobilityModel does not must be used. We use
  // GetLatitude and GetLongitude of ItetrisMobilityModel.
  // See itetris-mobility-model.h
  //
  c2cCommonHeader::ShortPositionVector m_posvector;

  Ptr<ItetrisMobilityModel> model = m_node->GetObject<ItetrisMobilityModel> ();
  m_posvector.gnAddr = m_node->GetId ();
  //m_posvector.Lat = (uint32_t) model->GetPosition().x;
  //m_posvector.Long = (uint32_t) model->GetPosition().y;
  m_posvector.Lat = model->GetLatitude();
  m_posvector.Long = model->GetLongitude();
  ///////////////////////////////////////////////  
  
  NS_ASSERT_MSG (m_routingProtocol != 0, "Need a routing protocol object to process packets");
    NS_LOG_INFO ("     L3: searching for a routing protocol to process the packet             ");
  m_routingProtocol->RouteInput (packet, commonHeader, device,
     MakeCallback (&c2cL3Protocol::L3Forward, this),
     MakeCallback (&c2cL3Protocol::LocalDeliver, this),
     MakeCallback (&c2cL3Protocol::RouteInputError, this)
   );
}

void
c2cL3Protocol::Send (struct c2cRoutingProtocol::output routeresult,
                    uint16_t protocol, uint16_t htype,
                    uint8_t hstype, uint8_t tc)
{
  NS_LOG_FUNCTION (this << routeresult.packet << uint32_t(protocol));
  ChannelTag channel;
  if (routeresult.packet->PeekPacketTag (channel))
	    {
	      //---------------- check iTETRIS ----------------------------
		  NS_LOG_INFO ("c2cL3Protocol: Channel Type (180-CCH/ 176-SCH1 / 178-SCH2) = "<<channel.Get());
		  //---------------- check iTETRIS ----------------------------
	    }
  c2cCommonHeader commonHeader;
  Ptr<c2cAddress> addr;
  addr = routeresult.route->GetGateway();
  commonHeader = BuildCommonHeader (addr, protocol, routeresult.packet->GetSize (), htype, hstype, tc);

  uint32_t ifaceIndex = 0;

//    if (addr->IsBroadcast())
//    {
     // 1) packet is destined to broadcast address
     for (c2cInterfaceList::iterator ifaceIter = m_interfaces.begin (); ifaceIter != m_interfaces.end (); ifaceIter++, ifaceIndex++)
        {
          Ptr<c2cInterface> outInterface = *ifaceIter;
          Ptr<Packet> packetCopy = routeresult.packet->Copy ();

          NS_ASSERT (packetCopy->GetSize () <= outInterface->GetDevice()->GetMtu ());
          packetCopy->AddHeader (commonHeader);

          m_txTrace (packetCopy, ifaceIndex);

          //---------------- check iTETRIS ----------------------------
          NS_LOG_INFO ("c2cL3Protocol: Broadcast via c2cInterface");
          //---------------- check iTETRIS ----------------------------

          packetCopy->Print(std::cout);
          NS_LOG_INFO ("");
//           outInterface->Send (packetCopy); // Modifed by Ramon Bauza
          // Added by Ramon Bauza
          if (addr->IsBroadcast ())
            {
              NS_LOG_LOGIC ("c2cL3Protocol: Broadcast via c2cInterface");
	      outInterface->Send (packetCopy);
            }
          else 
            {   
              Address destAddress;
              if (GetAddressByNodeId(addr->GetId(), destAddress))
                {
                  NS_LOG_LOGIC ("c2cL3Protocol: Unicast via c2cInterface");
	          outInterface->Send (packetCopy, destAddress); 
                }
              else
                {
                  NS_LOG_LOGIC ("c2cL3Protocol: MAC address unknown for nodeId="<<addr->GetId());
                }               
            }
        }

      return;
}

void
c2cL3Protocol::SendTo (struct c2cRoutingProtocol::output routeresult,
                    const c2cCommonHeader &header, uint8_t lt)
{
  ChannelTag channel;
  if (routeresult.packet->PeekPacketTag (channel))
	    {
	      //---------------- check iTETRIS ----------------------------
		  NS_LOG_INFO ("c2cL3Protocol: Channel Type (180-CCH/ 176-SCH1 / 178-SCH2) = "<<channel.Get());
		  //---------------- check iTETRIS ----------------------------
	    }
  c2cCommonHeader commonHeader;
  Ptr<c2cAddress> addr;
  addr = routeresult.route->GetGateway();
  commonHeader = BuildCommonHeader (addr, (uint16_t) header.GetNextHeader(), routeresult.packet->GetSize (), header.GetHtype (), header.GetHSubtype (), header.GetTrafClass ());
  commonHeader.SetHopLimit (lt);

  uint32_t ifaceIndex = 0;

     for (c2cInterfaceList::iterator ifaceIter = m_interfaces.begin (); ifaceIter != m_interfaces.end (); ifaceIter++, ifaceIndex++)
        {
          Ptr<c2cInterface> outInterface = *ifaceIter;
          Ptr<Packet> packetCopy = routeresult.packet->Copy ();

          NS_ASSERT (packetCopy->GetSize () <= outInterface->GetDevice()->GetMtu ());
          packetCopy->AddHeader (commonHeader);

          if (addr->IsBroadcast ())  // if gateway address is broadcast
            {
              NS_LOG_DEBUG ("c2cL3Protocol: forward BROADCAST via c2cInterface");
	      outInterface->Send (packetCopy);
            }
            else // else if gateway address is not broadcast (unicast)
            {   
              Address gatewayAddress;
              if (GetAddressByNodeId(addr->GetId(), gatewayAddress))
                {
                  NS_LOG_DEBUG("c2cL3Protocol: forward UNICAST via c2cInterface");
	          outInterface->Send (packetCopy, gatewayAddress); 
                }
              else
                {
                  NS_LOG_DEBUG ("c2cL3Protocol: Forward Unicast: MAC address unknown for gateway nodeId= "<<addr->GetId());
                }               
            }
        }

      return;

}


c2cCommonHeader
c2cL3Protocol::BuildCommonHeader (Ptr<c2cAddress> addr, uint16_t protocol,
                                  uint16_t payloadSize, uint16_t htype,
                                  uint8_t hstype, uint8_t tc)
{
  NS_LOG_FUNCTION_NOARGS ();

  c2cCommonHeader commonHeader;
  commonHeader.SetHtype (static_cast <uint8_t> (htype));
  commonHeader.SetHSubtype (hstype);
  commonHeader.SetNextHeader (protocol);
  commonHeader.SetLength (payloadSize); //Length of the C2C-CC common network header + Payload
  commonHeader.SetTrafClass (tc);

  if (htype == 1)
  {
  commonHeader.SetHopLimit (1);    // One-hop
  }
  else if (htype == 5)
  {
  commonHeader.SetHopLimit (addr->GetAreaSize ()); // In case of topo-broadcast mechanism the ttl is retrieved from the areasize field of the destination address
  }
  else
  {
  /// To simulate CAM transmission, geo-broadcast is used and TTL is set to 1.
  //To enable multihop: reset to 255
  commonHeader.SetHopLimit (255);  // Multi-hop
  }

  ///////////////////////////////////////////////
  // Modification by Florent KAISSER 08/24/2016
  //
  // Here, the positions must be in degree,
  // then GetPosition of MobilityModel does not must be used. We use
  // GetLatitude and GetLongitude of ItetrisMobilityModel.
  // See itetris-mobility-model.h
  //
  // Retrieving position information (latitude, longitude, altitude), heading and speed.
  struct c2cCommonHeader::LongPositionVector vector;
  Ptr<ItetrisMobilityModel> model = m_node->GetObject<ItetrisMobilityModel> ();
  vector.gnAddr = m_node->GetId ();
  //vector.Lat = (uint32_t) model->GetPosition().x;
  //vector.Long = (uint32_t) model->GetPosition().y;
  vector.Lat = model->GetLatitude();
  vector.Long = model->GetLongitude();
  vector.Alt = 0;
  vector.Speed = (uint16_t) model->GetVelocity().x;
  vector.Ts = (static_cast<uint32_t>(Simulator::Now ().GetSeconds()));
  ///////////////////////////////////////////////      

  commonHeader.SetSourPosVector(vector);
  return commonHeader;
}

void
c2cL3Protocol::L3Forward (struct c2cRoutingProtocol::output rtentry, const c2cCommonHeader &header)
{
  NS_LOG_FUNCTION (rtentry.packet << header);
  NS_LOG_LOGIC ("Forwarding logic for node: " << m_node->GetId ());

  // Forwarding
  c2cCommonHeader commonHeader = header;

  Ptr<Packet> packet = rtentry.packet->Copy ();
  int32_t interface = GetInterfaceForDevice (rtentry.route->GetOutputDevice ());
  commonHeader.SetHopLimit(commonHeader.GetHopLimit() - 1);
  if (commonHeader.GetHopLimit() == 0)
    {
      NS_LOG_WARN ("TTL exceeded.  Drop.");
      m_dropTrace (header, packet, DROP_TTL_EXPIRED, interface);
      return;
    }
  m_unicastForwardTrace (commonHeader, packet, interface);
  SendTo(rtentry, header, commonHeader.GetHopLimit());
}

void
c2cL3Protocol::LocalDeliver (Ptr<const Packet> packet, c2cCommonHeader const&header, Ptr<const c2cAddress> saddr, Ptr<const c2cAddress> daddr, uint32_t iif)
{
  NS_LOG_FUNCTION (this << packet);
  Ptr<Packet> p = packet->Copy (); // need to pass a non-const packet up

  m_localDeliverTrace (header, packet, iif);

  Ptr<c2cL4Protocol> protocol = GetProtocol (header.GetNextHeader());
  
  //Ptr<c2cL4Protocol> protocol = GetProtocol (0x0808); // Modified by Ramon Bauza
  NS_LOG_INFO ("     L3: local deliver: searching for l4 protocol by analyzing the next header field             ");
  if (protocol != 0)
    {
      // we need to make a copy in the unlikely event we hit the
      // RX_ENDPOINT_UNREACH codepath
      Ptr<Packet> copy = p->Copy ();
      //AddPacketTags (p, header); // Added by Ramon Bauza
      enum c2cL4Protocol::RxStatus status =
        protocol->Receive (p, saddr, daddr, GetInterface (iif));
      switch (status) {
      case c2cL4Protocol::RX_OK:
        // fall through
      case c2cL4Protocol::RX_ENDPOINT_CLOSED:
        // fall through
      case c2cL4Protocol::RX_CSUM_FAILED:
        break;
      case c2cL4Protocol::RX_ENDPOINT_UNREACH:
    	break;
      }
    }
}



// Added by Ramon Bauza
void
c2cL3Protocol::AddPacketTags (Ptr<const Packet> p, c2cCommonHeader const&header)
{
  NodeIdTag nodeTag;
  nodeTag.Set(header.GetSourPosVector().gnAddr);
  p->AddPacketTag (nodeTag);

  TimeStampTag timeTag;
  timeTag.Set(header.GetSourPosVector().Ts);
  p->AddPacketTag (timeTag);
}

void
c2cL3Protocol::RouteInputError (Ptr<const Packet> p, const c2cCommonHeader & header, Socketc2c::SocketErrno sockErrno)
{
  NS_LOG_FUNCTION (this << p << header << sockErrno);
  NS_LOG_LOGIC ("Route input failure-- dropping packet to " << header << " with errno " << sockErrno);
  m_dropTrace (header, p, DROP_ROUTE_ERROR, 0);
}

uint16_t
c2cL3Protocol::GetMtu (uint32_t i) const
{
  NS_LOG_FUNCTION (this << i);
  Ptr<c2cInterface> interface = GetInterface (i);
  return interface->GetDevice ()->GetMtu ();
}

Ptr<NetDevice>
c2cL3Protocol::GetNetDevice (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
  return GetInterface (i)-> GetDevice ();
}

} //namespace ns3
