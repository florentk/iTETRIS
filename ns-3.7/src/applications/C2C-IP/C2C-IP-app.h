/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
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
 * Author:  Michele Rondinone <mrondinone@umh.es>
 */


#ifndef C2C_IP_APP_H
#define C2C_IP_APP_H

#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"
#include "ns3/random-variable.h" 
#include "ns3/address-utils.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/socket-factory.h"
#include "ns3/iTETRIS-Application.h"  


namespace ns3 {

class Socketc2c;

class C2CIPApp : public iTETRISApplication
{
public:
  static TypeId GetTypeId (void);

  C2CIPApp ();
  virtual ~C2CIPApp();

  void SetSockets(void);      
  void SetFrequency(double frequency);
  void SetMessRegenerationTime (double MessRegenerationTime);
  void SetMsgLifeTime (uint8_t MsgLifeTime); 

  void SetC2CAddress(Ptr<c2cAddress> address);
  void SetIPAddress(Ipv4Address address);

  void StartTransmitting(Ipv4Address address);
  void StartTransmitting(Ptr<c2cAddress> address);

  void StopTransmitting(void);
  void SetPacketSize (uint32_t packetSize); 

protected:
  virtual void DoDispose (void);

private:
  virtual void StartApplication(void);
  virtual void StopApplication(void);

  void DoSendIP(Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);
  void DoSendC2C(Ptr<c2cAddress> address,double messRegenerationTime,double startTime);
  void SendPacketIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);
  void SendPacketC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime);
  void ReceiveC2C (Ptr<Socketc2c> socketc2c);
  void ReceiveIP (Ptr<Socket> socketip);
  void ScheduleTxC2C (Ptr<c2cAddress> address,double messRegenerationTime,double startTime);
  void ScheduleTxIP (Ipv4Address address,Address destinationAddress,double messRegenerationTime,double startTime);

  Ptr<Socketc2c>  m_C2Csocket;  
  Ptr<Socket>     m_IPv4socket; 
  uint16_t        m_portC2C;    
  uint16_t        m_portIP;      
  uint32_t        m_packetSize;
  DataRate        m_dataRate;
  EventId         m_sendEventC2C;    
  EventId         m_sendEventIP;     
  bool            m_runningC2C;   
  bool            m_runningIP;    
  uint32_t        m_packetsSentC2C;  
  uint32_t        m_packetsSentIP;   
  TypeId          m_firstSocketfactory;   
  TypeId          m_secondSocketfactory;  
  uint16_t        m_sendCountC2C;
  uint64_t        m_recvCountC2C;
  uint16_t        m_sendCountIP;
  uint64_t        m_recvCountIP;
  double          m_frequency;
  double          m_MessRegenerationTime;
  uint8_t         m_MsgLifeTime;
  double          m_StartTime;
  Ipv4Address     m_IPAddress;
  Address         m_destinationaddress;
  Ptr<c2cAddress> m_c2cAddress;
  RandomVariable  m_rndOffset;

};

} // namespace ns3

#endif   /* C2C_IP_APP_H  */