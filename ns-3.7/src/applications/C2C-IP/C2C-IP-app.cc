/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
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
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable.h"
#include "ns3/socketc2c.h"
#include "ns3/simulator.h"
#include "ns3/socket-factoryc2c.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/c2cl4T-socket-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/socket-factory.h"
#include "ns3/iTETRIS-Application.h"  

#include "C2C-IP-app.h"


NS_LOG_COMPONENT_DEFINE ("C2CIPApp");

using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (C2CIPApp);

TypeId
C2CIPApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::C2CIPApp")
    .SetParent<Application> ()
    .AddConstructor<C2CIPApp> ()
    .AddAttribute ("Frequency", "The frequency in on state.",
    		   DoubleValue (0),
    		   MakeDoubleAccessor  (&C2CIPApp::m_frequency),
    		   MakeDoubleChecker<double> ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
               UintegerValue (0),
               MakeUintegerAccessor (&C2CIPApp::m_packetSize),
               MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("PortC2C", "The port for c2c messages",     
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&C2CIPApp::m_portC2C),     
    		   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PortIP", "The port of the ip messages",    
    		   UintegerValue (0),
    		   MakeUintegerAccessor (&C2CIPApp::m_portIP),     
    		   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("SocketFactorytype1", "The type of the socket factory 1 to use.",   
               TypeIdValue (),
               MakeTypeIdAccessor (&C2CIPApp::m_firstSocketfactory),
               MakeTypeIdChecker ())
    .AddAttribute ("SocketFactorytype2", "The type of the socket factory 2 to use.",
               TypeIdValue (),
               MakeTypeIdAccessor (&C2CIPApp::m_secondSocketfactory),
               MakeTypeIdChecker ())

    ;
  return tid;
}

C2CIPApp::C2CIPApp()
{
    m_C2Csocket = 0;
    m_IPv4socket = 0;
    m_portC2C = 0;
    m_portIP = 0;
    m_packetSize = 0;
    m_dataRate = 0;
    m_frequency = 0;
    m_runningC2C = false;
    m_packetsSentC2C = 0;
    m_sendCountC2C = 0;
    m_recvCountC2C = 0;
    m_runningIP = false;
    m_packetsSentIP = 0;
    m_sendCountIP = 0;
    m_recvCountIP = 0;
    m_MessRegenerationTime = -1;
    m_StartTime =0;
    m_rndOffset = UniformVariable(0.0,0.999); 
}

C2CIPApp::~C2CIPApp()
{
}

void
C2CIPApp::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_C2Csocket = 0;
  m_IPv4socket = 0;
  Application::DoDispose ();
}

void
C2CIPApp::StartApplication (void)
{
}

void
C2CIPApp::SetSockets (void)
{
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][C2CIPApp]******************************* set sockets on node ****************************\n");

  if (!m_C2Csocket)
    {
      NS_LOG_INFO("\n");
      NS_LOG_INFO("[ns3][C2CIPApp] Creating c2c Socket of Type = "<<m_firstSocketfactory<<" | Binds at PORT number = "<<m_portC2C<<"\n");
      m_C2Csocket = Socketc2c::CreateSocket (GetNode(), m_firstSocketfactory);
      m_C2Csocket->Bind (m_portC2C);
      m_C2Csocket->SetRecvCallback(MakeCallback (&C2CIPApp::ReceiveC2C, this));
      NS_LOG_INFO("[ns3][C2CIPApp]*********** C2C socket created and binded ************\n");
    }
    else
    {
      NS_FATAL_ERROR ("[ns3][C2CIPApp] creation attempt of a c2c socket for C2CIPApp that has already a c2c socket active");
      return;
    }

    if (!m_IPv4socket)
      {
        NS_LOG_INFO("\n");
        NS_LOG_INFO("[ns3][C2CIPApp] Creating IP Socket of Type = "<<m_secondSocketfactory<<" | Binds at PORT number = "<<m_portIP<<"\n");

        m_IPv4socket = Socket::CreateSocket (GetNode(), m_secondSocketfactory);
        InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_portIP);
        m_IPv4socket->Bind (local);
        m_IPv4socket->SetRecvCallback (MakeCallback(&C2CIPApp::ReceiveIP, this));
        NS_LOG_INFO("[ns3][C2CIPApp]********* IP socket created and binded ***********\n");    
      }
    else
      {
        NS_FATAL_ERROR ("[ns3][C2CIPApp] creation attempt of an ip socket for C2CIPApp that has already an ip socket active");
        return;
      }
}


void 
C2CIPApp::StopApplication(void)
{
}

void 
C2CIPApp::StopTransmitting(void)
{
  NS_LOG_INFO("[ns3][C2CIPApp] ********STOP C2C-IP APP on node " << GetNode()->GetId() <<" ******\n");
  m_runningC2C = false;
  m_runningIP = false;

  if (m_sendEventC2C.IsRunning ())
    {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][C2CIPApp] ********STOP C2C transmission on node " << GetNode()->GetId() <<"  while running ******\n");
    }
  if (m_C2Csocket)
    {
      m_C2Csocket->Close ();
    }

  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][C2CIPApp] *******  STOP IP transmission on node " << GetNode()->GetId() <<"  while running *********\n");;
    }
  if (m_IPv4socket)
    {
      m_IPv4socket->Close ();
    }
}

void 
C2CIPApp::StartTransmitting(Ipv4Address address )
{
  NS_LOG_FUNCTION_NOARGS ();
  m_runningIP = true;
    
  m_IPAddress = address;
  Address destinationaddress = InetSocketAddress(m_IPAddress, m_portIP);
  m_destinationaddress = destinationaddress;
  
  m_StartTime = (Simulator::Now()).GetSeconds ();
  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][C2CIPApp]********* STOP current IP periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" *********\n");
    }
  InitializeINCIvariables();
  DoSendIP(address,m_destinationaddress,m_MessRegenerationTime,m_StartTime);
}

void 
C2CIPApp::StartTransmitting(Ptr<c2cAddress> address)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_runningC2C = true;
  m_c2cAddress = address;
  m_StartTime = (Simulator::Now()).GetSeconds ();
  if (m_sendEventC2C.IsRunning ())
    {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][C2CIPApp]****** STOP current c2c periodic transmission before transmitting a new message on node " << GetNode()->GetId() <<" **********\n");
    }
  InitializeINCIvariables();
  DoSendC2C(address,m_MessRegenerationTime,m_StartTime);
}

void
C2CIPApp::DoSendIP(Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][C2CIPApp] ======NODE " << GetNode()->GetId() <<": connection attempt to address " << address<<"   =====\n");

  m_IPv4socket->Connect (destinationAddress);
  AddInciPacketTags(packet);
  m_sendCountIP ++;
  
  NS_LOG_INFO("\n");  
  NS_LOG_INFO("[ns3][C2CIPApp] SENDING IP packet no. "<<m_sendCountIP<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<"\n");

  m_IPv4socket->Send (packet);
  ScheduleTxIP (address,destinationAddress,messRegenerationTime,startTime);
}

void
C2CIPApp::DoSendC2C(Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

  AddInciPacketTags(packet);
  m_sendCountC2C ++;
  
  NS_LOG_INFO("\n");
  NS_LOG_INFO("[ns3][C2CIPApp] SENDING C2C packet no. "<<m_sendCountC2C<<" at "<<Simulator::Now ().GetSeconds ()<<" seconds | packet size = "<<packet->GetSize()<<"| address = "<<address<<"\n");

  m_C2Csocket->DoSendTo(packet, address, m_portC2C,m_MsgLifeTime,1,m_sendCountC2C);
  ScheduleTxC2C (address,messRegenerationTime,startTime);
}


void
C2CIPApp::ReceiveC2C (Ptr<Socketc2c> socketc2c)
{
  Ptr<Packet> packet;
  NS_LOG_INFO("[ns3][C2CIPApp] Start Receiving - Call Socketc2c -> Recv()\n");
  packet=socketc2c->Recv();

  RetrieveInciPacketTags (packet);
  m_recvCountC2C++;

  NS_LOG_INFO("[ns3][C2CIPApp] SUCCESS: Receiving c2c message no. "<<m_recvCountC2C<<" at "<<Simulator::Now ().GetSeconds ()<< " seconds | c2c message size  = "<<packet->GetSize()<<" Bytes\n");
  NS_LOG_INFO("[ns3][C2CIPApp]========= SUCCESS : C2C Reception on node " << GetNode()->GetId() <<"  ==========\n");;
}


void 
C2CIPApp::ReceiveIP (Ptr<Socket> socketip)
{
  Ptr<Packet> packet;
  Address from;
  while (packet = socketip->RecvFrom (from))
    {
      NS_LOG_INFO("[ns3][C2CIPApp] Start Receiving - Call SocketIP -> RecvFrom()\n");;
      if (InetSocketAddress::IsMatchingType (from))
        {
          InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
          RetrieveInciPacketTags (packet);
          m_recvCountIP++;

          NS_LOG_INFO("[ns3][C2CIPApp] SUCCESS: Receiving IP packet no. "<<m_recvCountIP<<" from " <<
          address.GetIpv4()<< " at "<<Simulator::Now ().GetSeconds ()<<" seconds | IP packet size  = "<<packet->GetSize()<<" Bytes\n");
          NS_LOG_INFO("[ns3][C2CIPApp]========= SUCCESS : IP reception on node " << GetNode()->GetId() <<"  ==============\n");
        }
    }
}

void 
C2CIPApp::SendPacketIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_sendEventIP.IsRunning ())
    {
      Simulator::Cancel (m_sendEventIP);
      NS_LOG_INFO("[ns3][C2CIPApp]********* STOP current IP periodic transmission " << GetNode()->GetId() <<" *********\n");
    }
  if(messRegenerationTime< 0)// if messRegenerationTime is set to a value <0 , then always keep transmitting
    {
      DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
    }
  else // otherwise only transmit if the time passed from the first transmission is less than the messageRegenerationTime fixed by the app
    {
      double now = (Simulator::Now()).GetSeconds();
      double lifetime =  now - startTime;
      if( lifetime <= messRegenerationTime)
        {
          DoSendIP(address,destinationAddress,messRegenerationTime,startTime);
        }
      else
        {
          messRegenerationTime= -1;  // set to default value
        } 
    } 
}

void 
C2CIPApp::SendPacketC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{
  if (m_sendEventC2C.IsRunning ())
    {
      Simulator::Cancel (m_sendEventC2C);
      NS_LOG_INFO("[ns3][C2CIPApp]********* STOP current C2C periodic transmission " << GetNode()->GetId() <<" *********\n");;
    }
  if(messRegenerationTime < 0)// if messRegenerationTime is set to a value <0 , then always keep transmitting
    {
      DoSendC2C(address,messRegenerationTime,startTime);
    }
  else // otherwise only transmit if the time passed from the first transmission is less than the messageRegenerationTime fixed by the app
    {
      double now = (Simulator::Now()).GetSeconds();
      double lifetime =  now - startTime;
      if( lifetime <= messRegenerationTime)
      {
        DoSendC2C(address,messRegenerationTime,startTime);
      }
      else
        {
          messRegenerationTime= -1;  // set to default value
        } 
    } 
}

void 
C2CIPApp::ScheduleTxC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime)
{
  if (m_runningC2C)
    {
       m_dataRate = (m_packetSize * 8) * m_frequency;
       Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
       m_sendEventC2C = Simulator::Schedule (tNext, &C2CIPApp::SendPacketC2C, this,address,messRegenerationTime,startTime);
    }
  else
    {
      NS_LOG_INFO("\n");
      NS_LOG_INFO("[ns3][C2CIPApp]***************  STOP C2C transmission ON NODE  " << GetNode()->GetId() <<"**************\n");
   }
}

void 
C2CIPApp::ScheduleTxIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime)
{
  if (m_runningIP)
    {
      m_dataRate = (m_packetSize * 8) * m_frequency;
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEventIP = Simulator::Schedule (tNext, &C2CIPApp::SendPacketIP, this,address,destinationAddress,messRegenerationTime,startTime);
    }
  else
    {
      NS_LOG_INFO("\n");
      NS_LOG_INFO("[ns3][C2CIPApp]*************  STOP IP transmission ON NODE  " << GetNode()->GetId() <<"***********\n");
    }
}


void 
C2CIPApp::SetMessRegenerationTime (double MessRegenerationTime)
{
  m_MessRegenerationTime=MessRegenerationTime;
}

void 
C2CIPApp::SetFrequency(double frequency)
{
  m_frequency = frequency;
}

void 
C2CIPApp::SetPacketSize (uint32_t packetSize)
{
  m_packetSize = packetSize;
}

void 
C2CIPApp::SetMsgLifeTime (uint8_t MsgLifeTime)
{
  m_MsgLifeTime = MsgLifeTime;
}

void 
C2CIPApp::SetIPAddress(Ipv4Address address)
{
  m_IPAddress = address;
  Address destinationaddress = InetSocketAddress(m_IPAddress, m_portIP);
  m_destinationaddress = destinationaddress;
}

void 
C2CIPApp::SetC2CAddress(Ptr<c2cAddress> address)
{
  m_c2cAddress = address;
}

} // Namespace ns3
