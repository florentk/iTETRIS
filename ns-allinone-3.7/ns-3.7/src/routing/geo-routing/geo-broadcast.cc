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

#include <stdlib.h>

//#include "ns3/mobility-model.h"
#include "ns3/itetris-mobility-model.h"

#include "geo-broadcast.h"
#include "geoBroadAnycast-header.h"
#include "utils.h"

#include <math.h>

#include "ns3/app-index-tag.h"

NS_LOG_COMPONENT_DEFINE ("geoBroadcast");
namespace ns3
{

TypeId
geoBroadcast::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::geo-routing::geoBroadcast")
      .SetParent<c2cRoutingProtocol> ()
      .AddConstructor<geoBroadcast> ()
  ;
  return tid;
}

geoBroadcast::geoBroadcast ()
{
}

geoBroadcast::~geoBroadcast ()
{
}

void
geoBroadcast::Setc2c (Ptr<c2c> c2c)
{
  NS_ASSERT (c2c != 0);
  NS_ASSERT (m_c2c == 0);

  m_c2c = c2c;
}

void
geoBroadcast::DoDispose ()
{
  m_c2c = 0;
  c2cRoutingProtocol::DoDispose ();
}

struct c2cRoutingProtocol::output
geoBroadcast::RouteOutput (Ptr<Packet> p, c2cCommonHeader::LongPositionVector sourcePosVector,
                           Ptr<c2cAddress> daddr, uint8_t lt, uint16_t sn,
                           Ptr<NetDevice> oif, Socketc2c::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << p->GetUid());

  sockerr = Socketc2c::ERROR_NOTERROR;
  struct c2cRoutingProtocol::output result;
  result.route = CreateObject<c2cRoute> ();

  //---------------- check iTETRIS ----------------------------
//  std::cout<<"GeoBroadcast: RouteOutput at Node "<<(m_c2c->GetObject<Node> ())->GetId()<<std::endl;
  //---------------- check iTETRIS ----------------------------

  struct c2cCommonHeader::geoAreaPos areapos = *(daddr->GetGeoAreaPos1 ());
  


  //Geobroadcast extended header
  GeoABcastHeader bheader;
  bheader.SetLifeTime (lt);
  bheader.SetSeqNb(sn);
  bheader.SetSourPosVector (sourcePosVector);
  bheader.SetGeoAreaPos1 (areapos);
  if ((daddr->GetGeoAreaPos2 ()) != NULL)
  bheader.SetGeoAreaPos2 (*(daddr->GetGeoAreaPos2 ()));
  bheader.SetAreaSize (daddr->GetAreaSize ());

  p->AddHeader (bheader);
  result.packet = p;

  //C2C route: source and destination address
  Ptr<c2cAddress> saddr = CreateObject<c2cAddress> ();
  saddr->Set (sourcePosVector.gnAddr, sourcePosVector.Lat, sourcePosVector.Long);
  result.route->SetSource (saddr);
  result.route->SetDestination (daddr);
//   result.route->SetOutputDevice ();

  // the distance is computed from geographic coordinate in degree, then we use DistanceInMeters instead CartesianDistance
  //if (CartesianDistance (sourcePosVector.Lat, sourcePosVector.Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon) < daddr->GetAreaSize())
  //{ 
  
  double distance = DistanceInMeters (sourcePosVector.Lat, sourcePosVector.Long, areapos.lat, areapos.lon);

  // Area is in square meter, to compare with 
  // a distance, we compute the radius
  double radius = sqrt(((double)daddr->GetAreaSize() / M_PI));
  if (distance < radius)
  {
    //---------------- check iTETRIS ----------------------------
 //   std::cout<<"GeoBroadcast RouteOutput at Node "<< m_c2c->GetObject<Node> ()->GetId ()<<" is in the destination area "<<std::endl;
    //---------------- check iTETRIS ----------------------------
    //construct the route:gateway 
    result.route->SetGateway (daddr);
    result.packet->Print(std::cout);
    std::cout<<""<<std::endl;
  }
  else
  {
     c2cCommonHeader::ShortPositionVector m_posvector;
     m_posvector.gnAddr = m_c2c->GetObject<Node>()->GetId ();
     m_posvector.Lat = sourcePosVector.Lat;
     m_posvector.Long = sourcePosVector.Long;
    //---------------- check iTETRIS ----------------------------
//    std::cout<<"GeoBroadcast RouteOutput at Node "<< m_c2c->GetObject<Node> ()->GetId ()<<" is NOT in the destination area "<<std::endl;
    //---------------- check iTETRIS ----------------------------

    if ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> () != 0)
    {
    struct c2cCommonHeader::LongPositionVector vector = getMinDistToDest ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> (), daddr);
    if (vector.gnAddr != m_c2c->GetObject<Node> ()->GetId ())
    {
      //route:gateway
      Ptr<c2cAddress> gw = CreateObject<c2cAddress> ();
      gw->Set (vector.gnAddr, vector.Lat, vector.Long);
      result.route->SetGateway (gw);
    }
//     else
//     {
//        store (p);
//     }
    }
//     else
//     {
//        store (p);
//     }
  }
  return result;
}

bool
geoBroadcast::RouteInput (Ptr<const Packet> p, const c2cCommonHeader &header,
                          Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                          LocalDeliverCallback lcb, ErrorCallback ecb)
{
  NS_LOG_FUNCTION (this << p->GetUid());
  NS_ASSERT (m_c2c != 0);

  // Check if input device supports IP
  NS_ASSERT (m_c2c->GetInterfaceForDevice (idev) >= 0);
  int32_t iif = m_c2c->GetInterfaceForDevice (idev);
  struct c2cRoutingProtocol::output result;
  result.route = CreateObject<c2cRoute> ();

  //---------------- check iTETRIS ----------------------------
//  std::cout<<"GeoBroadcast: RouteInput at Node "<<(m_c2c->GetObject<Node> ())->GetId()<<std::endl;
  //---------------- check iTETRIS ----------------------------

  if (checkReception (p, header) == false)
  {
    Ptr<Packet> packetlcb = p->Copy ();
    GeoABcastHeader bheader;
    packetlcb->RemoveHeader (bheader);

    struct c2cCommonHeader::geoAreaPos areapos1 = bheader.GetGeoAreaPos1 ();
    struct c2cCommonHeader::geoAreaPos areapos2 = bheader.GetGeoAreaPos2 ();    
    
    struct c2cCommonHeader::geoAreaPos* pareapos1 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
    struct c2cCommonHeader::geoAreaPos* pareapos2 = (struct c2cCommonHeader::geoAreaPos*) malloc (sizeof (struct c2cCommonHeader::geoAreaPos));
    if (pareapos1 != NULL)
    *(pareapos1) =  areapos1;
    if (pareapos2 != NULL)
    *(pareapos2) =  areapos2;


     Ptr<c2cAddress> saddr = CreateObject<c2cAddress> ();
     saddr->Set (bheader.GetSourPosVector().gnAddr, bheader.GetSourPosVector().Lat, bheader.GetSourPosVector().Long);

     Ptr<c2cAddress> daddr = CreateObject<c2cAddress> ();
     daddr->Set (c2cAddress::BROAD, pareapos1, pareapos2, bheader.GetAreaSize());

     
     free (pareapos1);
     free (pareapos2);

     result.packet = p->Copy ();
     result.route->SetSource (saddr);
     result.route->SetDestination (daddr);
//      result.route->SetOutputDevice ();

     //Update location table
     Ptr <LocationTable> ltable = (m_c2c->GetObject<Node> ())->GetObject<LocationTable> ();
     if (ltable != 0)
     ltable ->AddPosEntry (bheader.GetSourPosVector());

     ///////////////////////////////////////////////
     // Modification by Florent KAISSER 08/21/2016
     //
     // Here, to compute the distance, positions must be in degree,
     // then GetPosition of MobilityModel does not must be used. We use
     // GetLatitude and GetLongitude of ItetrisMobilityModel.
     // See itetris-mobility-model.h
     //
     //Position is retrieved from the mobility model
     c2cCommonHeader::ShortPositionVector m_posvector;
     Ptr<ItetrisMobilityModel> model = m_c2c->GetObject<Node>()->GetObject<ItetrisMobilityModel> ();
     
     uint32_t mylat = model->GetLatitude();
     uint32_t mylon = model->GetLongitude(); 

     /*double mylatO = model->GetLatitudeO();
     double mylonO = model->GetLongitudeO(); */
     
     struct c2cCommonHeader::geoAreaPos areapos = areapos1;
     /*areapos.lat *= 10000000;
     areapos.lon *= 10000000;*/     
     
     m_posvector.gnAddr = m_c2c->GetObject<Node>()->GetId ();
     m_posvector.Lat = mylat;
     m_posvector.Long = mylon;     
     //m_posvector.Lat = (uint32_t) model->GetPosition().x;
     //m_posvector.Long = (uint32_t) model->GetPosition().y;

     // the distance is computed from geographic coordinate in degree, then we use DistanceInMeters instead CartesianDistance
     double distanceReceiver = DistanceInMeters (mylat, mylon, areapos.lat, areapos.lon);
     //double distanceReceiver = CartesianDistance(m_posvector.Lat, m_posvector.Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon); 
     //
     // Area is in square meter, to compare with 
     // a distance, we compute the radius
     double radius = sqrt(((double)daddr->GetAreaSize() / M_PI));
     //
     ///////////////////////////////////////////////


   //  std::cout<<"distance= "<<distanceReceiver<<std::endl;
     if (distanceReceiver <= radius)
     {
      //---------------- check iTETRIS ----------------------------
 //     std::cout<<"GeoBroadcast RouteInput: Node "<< m_c2c->GetObject<Node> ()->GetId ()<<" is in the destination area "<<std::endl;
      //---------------- check iTETRIS ----------------------------
      //Local deliver
      lcb (packetlcb, header, saddr, daddr, iif);
      result.route->SetGateway (daddr);
      ucb (result, header);
      return true;
     }

     else
     {
      //---------------- check iTETRIS ----------------------------
//      std::cout<<"GeoBroadcast RouteInput: Node "<< m_c2c->GetObject<Node> ()->GetId ()<<" is NOT in the destination area "<<std::endl;
      //---------------- check iTETRIS ----------------------------

     //double distanceLastF = CartesianDistance(header.GetSourPosVector().Lat, header.GetSourPosVector().Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon);
     double distanceLastF = DistanceInMeters (header.GetSourPosVector().Lat, header.GetSourPosVector().Long, areapos.lat, areapos.lon);

     if (distanceLastF <= radius)
     {
        //---------------- check iTETRIS ----------------------------
//        std::cout<<"GeoBroadcast RouteInput: Discard message "<<std::endl;
        //---------------- check iTETRIS ----------------------------
        //drop packet coming from a forwarder located inside the destination area
        return false;
     }
     else
     {
       struct c2cCommonHeader::LongPositionVector vector = getMinDistToDest ((m_c2c->GetObject<Node> ())->GetObject<LocationTable> (), daddr);
       //double distanceM = CartesianDistance (vector.Lat, vector.Long, daddr->GetGeoAreaPos1 ()->lat, daddr->GetGeoAreaPos1 ()->lon);
       double distanceM = DistanceInMeters (vector.Lat, vector.Long, areapos.lat, areapos.lon);
       if (distanceM < distanceLastF)
       {
         if (vector.gnAddr != m_c2c->GetObject<Node> ()->GetId())
         {
            Ptr<c2cAddress> gw = CreateObject<c2cAddress> ();
            gw->Set  (vector.gnAddr, vector.Lat, vector.Long);
            result.route->SetGateway (gw);

             //---------------- check iTETRIS ----------------------------
//             std::cout<<"geoBroadcast RouteInput: forward to "<<gw->GetId()<<std::endl;
            //---------------- check iTETRIS ----------------------------

            ucb (result, header);
            return true;
         }
         else
         {
//            store (p);
           return false;
         }
       }
       else
       {
//          store (p);
         return false;
       }
     }
    }
  }
  else
	//  std::cout<<"Packet Processed Before"<<std::endl; //vineet
  return false; //Packet processed before
}

/*
bool
geoBroadcast::checkReception (Ptr<const Packet> p)
{
  bool result = false;
  Ptr<Packet> packet = p->Copy ();
  GeoABcastHeader bheader;
  packet->RemoveHeader (bheader);

  c2cCommonHeader commonHeader;
  packet->RemoveHeader (commonHeader);

  if (m_c2c->GetObject<Node> ()->GetId() != bheader.GetSourPosVector().gnAddr)
  {
  if ((!packetReceived.empty()))
  {
  std::map<uint64_t,T>::iterator iter = packetReceived.find (bheader.GetSourPosVector().gnAddr);
  if (iter != packetReceived.end())
  {
    for (T::iterator i = iter->second.begin ();
       i != iter->second.end (); i++)
    {
      if (*(i) == commonHeader.GetSourPosVector().Ts)
      result = true;
    }
    if (result == false)
    {
     iter->second.push_back (commonHeader.GetSourPosVector().Ts);
    }
  }
  else
  {
  result = false;
  std::vector <uint32_t> v;
  v.push_back (commonHeader.GetSourPosVector().Ts);
  packetReceived [bheader.GetSourPosVector().gnAddr] = v;
  }
  }
  else
  {
    std::vector <uint32_t> v;
    v.push_back (commonHeader.GetSourPosVector().Ts);
    packetReceived [bheader.GetSourPosVector().gnAddr] = v;
    result = false;
  }
  }
  else
  result = true;
  return result;
}

*/

bool // modified by MR - Vineet
geoBroadcast::checkReception (Ptr<const Packet> p, const c2cCommonHeader &commonHeader)
{
  bool result = false;
  Ptr<Packet> packet = p->Copy ();
  GeoABcastHeader bheader;
  packet->RemoveHeader (bheader);

   AppIndexTag appindexTag;
   bool found;
   found = packet->PeekPacketTag (appindexTag);
   NS_ASSERT (found);
   uint32_t appindex = appindexTag.Get ();

   NS_LOG_INFO ("TopoBroadcast: checkreception: appindex tag= "<< appindex );

  if (m_c2c->GetObject<Node> ()->GetId() != bheader.GetSourPosVector().gnAddr)
  {
     if ((!m_packetReceived.empty()))  // local node did receive packets before
     {
         packetReceived::iterator iter = m_packetReceived.find (bheader.GetSourPosVector().gnAddr);
         if (iter != m_packetReceived.end())  // local node did receive packets from this source before
         {
              AppPackets::iterator it =  iter->second.find (appindex);
              if (it != iter->second.end())  // local node did receive packets belonging to this application from this node before
              {
                  for (PacketSequenceNumber::iterator i = it->second.begin (); i != it->second.end (); i++) // scan all the packets belonging to this application received from this node
                  {
                       if (*(i) == bheader.GetSeqNb()) //the local node has received a packet belonging to this application from this sender with the same SN as this one (the local node has not to retransmit this packet)
                       {
                          result = true;
                          break;
                       }
                  }
                  if (result == false) //the local node has received a packet belonging to this application from this sender but not with the same SN as this one
                  {
                      it->second.push_back (bheader.GetSeqNb());
                  }
              }
              else //local node did not receive packets belonging to this application from this node before
              {
                  result = false;
                  PacketSequenceNumber v;
                  v.push_back (bheader.GetSeqNb());
                  AppPackets AppPkts;
                  AppPkts [appindex] = v;
                  m_packetReceived [bheader.GetSourPosVector().gnAddr] = AppPkts;
              }
           }
           else  // local node did not receive a packet from this source before
             {
                 result = false;
                 PacketSequenceNumber v;
                 v.push_back (bheader.GetSeqNb());
                 AppPackets AppPkts;
                 AppPkts [appindex] = v;
                 m_packetReceived [bheader.GetSourPosVector().gnAddr] = AppPkts;
             }
     }
     else  // local node did not receive any packet before
     {
        PacketSequenceNumber v;
        v.push_back (bheader.GetSeqNb());
        AppPackets AppPkts;
        AppPkts [appindex] = v;
        m_packetReceived [bheader.GetSourPosVector().gnAddr] = AppPkts;
        result = false;
     }
  }
  else // local node is the packet source (the local node has not to retransmit this packet)
  result = true;
  return result;
}


}
