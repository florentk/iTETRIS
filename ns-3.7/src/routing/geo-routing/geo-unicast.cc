/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EURECOM, EU FP7 iTETRIS project
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
 * Author: Fatma Hrizi <fatma.hrizi@eurecom.fr>
 */

#include "geo-unicast.h"
#include "geoUnicast-header.h"
#include "utils.h"
#include "ns3/app-index-tag.h" // Added by Ramon Bauza

NS_LOG_COMPONENT_DEFINE ("geoUnicast");

namespace ns3
{

geoUnicast::geoUnicast ()
{
}

TypeId
geoUnicast::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::geo-routing::geoUnicast")
      .SetParent<c2cRoutingProtocol> ()
      .AddConstructor<geoUnicast> ()
  ;
  return tid;
}

geoUnicast::~geoUnicast ()
{
}

void
geoUnicast::Setc2c (Ptr<c2c> c2c)
{
  NS_ASSERT (c2c != 0);
  NS_ASSERT (m_c2c == 0);

  m_c2c = c2c;
}

void
geoUnicast::DoDispose ()
{
  m_c2c = 0;
  c2cRoutingProtocol::DoDispose ();
}

struct c2cRoutingProtocol::output
geoUnicast::RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                         Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn, Ptr<NetDevice> oif,
                         Socketc2c::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << p->GetUid());

  sockerr = Socketc2c::ERROR_NOTERROR;
  struct output result;
  Ptr<c2cAddress> gw;

  //---------------- check iTETRIS ----------------------------
   NS_LOG_INFO ("GeoUnicast: RouteOutput at Node "<<(m_c2c->GetObject<Node> ())->GetId());
  //---------------- check iTETRIS ----------------------------

  //Geounicast extended header
  geoUnicastHeader uheader;
  uheader.SetLifeTime(lt);
  uheader.SetSeqNb(sn);
  uheader.SetSourPosVector(sourcePosVector);

  // Added by Ramon Bauza
  c2cCommonHeader::ShortPositionVector destPosVector;
  destPosVector.gnAddr = daddr->GetId ();
  destPosVector.Lat = daddr->GetGeoAreaPos1 ()->lat;
  destPosVector.Long = daddr->GetGeoAreaPos1 ()->lon;
  uheader.SetDestPosVector(destPosVector); 
  // End addition

  p->AddHeader (uheader);
  result.packet = p;

  //construct the route: source and destination address
  Ptr<c2cAddress> saddr = CreateObject<c2cAddress> ();
  saddr->Set (sourcePosVector.gnAddr, sourcePosVector.Lat, sourcePosVector.Long);
  // Added by Ramon Bauza
  Ptr<c2cRoute> route = CreateObject<c2cRoute> ();
  route->SetSource (saddr);
  route->SetDestination (daddr);
  result.route = route;
  // Commented by Ramon Bauza
//   result.route->SetSource (saddr);
//   result.route->SetDestination (daddr);

//   result.route->SetOutputDevice ();

  c2cCommonHeader::ShortPositionVector m_posvector;
  m_posvector.gnAddr = m_c2c->GetObject<Node>()->GetId ();
  m_posvector.Lat = sourcePosVector.Lat;
  m_posvector.Long = sourcePosVector.Long;

  gw = DirectNeighbour ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> (), daddr);
  if (gw != 0)
  {
    //---------------- check iTETRIS ----------------------------
   NS_LOG_INFO ("GeoUnicast RoutOutput: The destination is a direct neighbor ");
    //---------------- check iTETRIS ----------------------------
    result.route->SetGateway (gw);
  }
  else
  {
    //---------------- check iTETRIS ----------------------------
   NS_LOG_INFO ("GeoUnicastRoutOutput: The destination is NOT a direct neighbor");
    //---------------- check iTETRIS ----------------------------
    struct c2cCommonHeader::LongPositionVector vector = getMinDistToDest ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> (), daddr);
    if (vector.gnAddr != m_c2c->GetObject<Node> ()->GetId())
    {
      Ptr<c2cAddress> gw = CreateObject<c2cAddress> ();
      gw->Set (vector.gnAddr, vector.Lat, vector.Long);
      result.route->SetGateway (gw);
    }
//     else
//     {
//       store (p);
//     }
  }
  return result;
}

bool
geoUnicast::RouteInput (Ptr<const Packet> p, const c2cCommonHeader &header,
                        Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                        LocalDeliverCallback lcb, ErrorCallback ecb)
{
  NS_LOG_FUNCTION (this << p->GetUid() << idev->GetAddress());
  NS_ASSERT (m_c2c != 0);

  // Check if input device supports IP
  NS_ASSERT (m_c2c->GetInterfaceForDevice (idev) >= 0);
  int32_t iif = m_c2c->GetInterfaceForDevice (idev);
  struct output result;

  //---------------- check iTETRIS ----------------------------
 NS_LOG_INFO ("GeoUnicast: RouteInput at Node "<<(m_c2c->GetObject<Node> ())->GetId());
  //---------------- check iTETRIS ----------------------------

//   if (checkReception (p) == false) // Modified by Ramon Bauza
  if (checkReception (p,header) == false) // Added by Ramon Bauza
  {
    Ptr<Packet> packetlcb = p->Copy ();
    geoUnicastHeader uheader;
    packetlcb->RemoveHeader (uheader);

    Ptr<c2cAddress> saddr = CreateObject<c2cAddress> ();
    saddr->Set (uheader.GetSourPosVector().gnAddr, uheader.GetSourPosVector().Lat, uheader.GetSourPosVector().Long);
    Ptr<c2cAddress> daddr = CreateObject<c2cAddress> ();
    daddr->Set (uheader.GetDestPosVector().gnAddr, uheader.GetDestPosVector().Lat,  uheader.GetDestPosVector().Long);
    result.packet = p->Copy();
    // Added by Ramon Bauza
    Ptr<c2cRoute> route = CreateObject<c2cRoute> ();
    route->SetSource (saddr);
    route->SetDestination (daddr);
    result.route = route;
    // Commented by Ramon Bauza
//     result.route->SetSource (saddr);
//     result.route->SetDestination (daddr);

//     result.route->SetOutputDevice ();

     //Update location table
     Ptr <LocationTable> ltable = (m_c2c->GetObject<Node> ())->GetObject<LocationTable> ();
     if (ltable != 0)
     ltable ->AddPosEntry (uheader.GetSourPosVector());

     if (uheader.GetDestPosVector().gnAddr == m_c2c->GetObject<Node> ()->GetId())
     {
      //Local deliver
      lcb (packetlcb, header, saddr, daddr, iif);
      return true;
     }
     else
     {

      //Position is retrieved from the mobility model
      c2cCommonHeader::ShortPositionVector m_posvector;
      Ptr<MobilityModel> model = m_c2c->GetObject<Node>()->GetObject<MobilityModel> ();
      m_posvector.gnAddr = m_c2c->GetObject<Node>()->GetId ();
      m_posvector.Lat = (uint32_t) model->GetPosition().x;
      m_posvector.Long = (uint32_t) model->GetPosition().y;

      Ptr<c2cAddress> gw;
      gw = DirectNeighbour ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> (), daddr);
      if (gw != 0)
       {
         //---------------- check iTETRIS ----------------------------
        NS_LOG_INFO ("GeoUnicast RouteInput: The destination is a direct neighbor ");
         //---------------- check iTETRIS ----------------------------
         result.route->SetGateway (gw);
         ucb (result, header);
         return true;
       }
       else
       {
         //---------------- check iTETRIS ----------------------------
        NS_LOG_INFO ("GeoUnicast RouteInput: The destination is NOT a direct neighbor ");
         //---------------- check iTETRIS ----------------------------
         struct c2cCommonHeader::LongPositionVector vector = getMinDistToDest ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> (), daddr);
         if (vector.gnAddr != m_c2c->GetObject<Node> ()->GetId())
          {
            Ptr<c2cAddress> gw = CreateObject<c2cAddress> ();
            gw->Set (vector.gnAddr, vector.Lat, vector.Long);
            result.route->SetGateway (gw);
	    ucb (result, header);
            return true;
          }
         else
          {
//            store (p);
           return false;
          }
       }
      }
  }
  else
  return false; //Packet processed before
}

// bool
// geoUnicast::checkReception (Ptr<const Packet> p)
// {
//   bool result = false;
//   Ptr<Packet> packet = p->Copy ();
//   geoUnicastHeader uheader;
//   packet->RemoveHeader (uheader);
// 
//   c2cCommonHeader commonHeader;
//   packet->RemoveHeader (commonHeader);
// 
//   if (m_c2c->GetObject<Node> ()->GetId() != uheader.GetSourPosVector().gnAddr)
//   {
//   if ((!packetReceived.empty()))
//   {
//   std::map<uint64_t,T>::iterator iter = packetReceived.find (uheader.GetSourPosVector().gnAddr);
//   if (iter != packetReceived.end())
//   {
//     for (T::iterator i = iter->second.begin ();
//        i != iter->second.end (); i++)
//     {
//       if (*(i) == commonHeader.GetSourPosVector().Ts)
//       result = true;
//     }
//     if (result == false)
//     {
//      iter->second.push_back (commonHeader.GetSourPosVector().Ts);
//     }
//   }
//   else
//   {
//   result = false;
//   std::vector <uint32_t> v;
//   v.push_back (commonHeader.GetSourPosVector().Ts);
//   packetReceived [uheader.GetSourPosVector().gnAddr] = v;
//   }
//   }
//   else
//   {
//     std::vector <uint32_t> v;
//     v.push_back (commonHeader.GetSourPosVector().Ts);
//     packetReceived [uheader.GetSourPosVector().gnAddr] = v;
//     result = false;
//   }
//   }
//   else
//   result = true;
//   return result;
// }

bool 
geoUnicast::checkReception (Ptr<const Packet> p, const c2cCommonHeader &commonHeader) 
{
  bool result = false;
  Ptr<Packet> packet = p->Copy ();
  geoUnicastHeader uheader;
  packet->RemoveHeader (uheader);

   AppIndexTag appindexTag;
   bool found;
   found = packet->PeekPacketTag (appindexTag);
   NS_ASSERT (found);
   uint32_t appindex = appindexTag.Get ();

//   std::cout<<"Geounicast: checkreception: appindex tag= "<< appindex <<std::endl;

  if (m_c2c->GetObject<Node> ()->GetId() != uheader.GetSourPosVector().gnAddr)
  {
     if ((!m_packetReceived.empty()))  // local node did receive packets before
     {
         packetReceived::iterator iter = m_packetReceived.find (uheader.GetSourPosVector().gnAddr);
         if (iter != m_packetReceived.end())  // local node did receive packets from this source before
         {
              AppPackets::iterator it =  iter->second.find (appindex);
              if (it != iter->second.end())  // local node did receive packets belonging to this application from this node before
              {
                  for (PacketSequenceNumber::iterator i = it->second.begin (); i != it->second.end (); i++) // scan all the packets belonging to this application received from this node
                  {
                       if (*(i) == uheader.GetSeqNb()) //the local node has received a packet belonging to this application from this sender with the same SN as this one (the local node has not to retransmit this packet)
                       {
                          result = true;
                          break;
                       }
                  }
                  if (result == false) //the local node has received a packet belonging to this application from this sender but not with the same SN as this one
                  {
                      it->second.push_back (uheader.GetSeqNb());
                  }
              }
              else //local node did not receive packets belonging to this application from this node before
              {
                  result = false;
                  PacketSequenceNumber v;
                  v.push_back (uheader.GetSeqNb());
                  AppPackets AppPkts;
                  AppPkts [appindex] = v;
                  m_packetReceived [uheader.GetSourPosVector().gnAddr] = AppPkts;
              }
           }
           else  // local node did not receive a packet from this source before
             {
                 result = false;
                 PacketSequenceNumber v;
                 v.push_back (uheader.GetSeqNb());
                 AppPackets AppPkts;
                 AppPkts [appindex] = v;
                 m_packetReceived [uheader.GetSourPosVector().gnAddr] = AppPkts;
             }
     }
     else  // local node did not receive any packet before
     {
        PacketSequenceNumber v;
        v.push_back (uheader.GetSeqNb());
        AppPackets AppPkts;
        AppPkts [appindex] = v;
        m_packetReceived [uheader.GetSourPosVector().gnAddr] = AppPkts;
        result = false;
     }
  }
  else // local node is the packet source (the local node has not to retransmit this packet)
  result = true;
  return result;
}

}
