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


#ifndef ITETRIS_APPLICATION_H
#define ITETRIS_APPLICATION_H

#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/callback.h" 


namespace ns3 {

class c2cAddress;

class iTETRISApplication : public Application
{
public:
  static TypeId GetTypeId (void);

  iTETRISApplication ();
  virtual ~iTETRISApplication();

  void SetServiceType (std::string servicetype);
  void SetServiceIndex(uint32_t app_index);
  typedef Callback<bool, uint32_t, std::string, uint32_t, uint32_t > ReceiveCallback; 
  void SetReceiveCallback (iTETRISApplication::ReceiveCallback cb); 
  void UnsetReceiveCallback (void); 

  virtual void SetFrequency (double frequency); 
  virtual void SetMessRegenerationTime (double MessRegenerationTime);
  virtual void SetMsgLifeTime (uint8_t MsgLifeTime); 
  virtual void SetPacketSize (uint32_t PacketSize);
  virtual void SetC2CAddress(Ptr<c2cAddress> address);
  virtual void SetIPAddress(Ipv4Address address);
  virtual void SetSockets(void);   
  virtual void SetApplicationId(uint64_t applicationId);

  virtual  void StartTransmitting(Ipv4Address address);
  virtual  void StartTransmitting(Ptr<c2cAddress> address);
  virtual  void StopTransmitting(void);  

private:
  /**
   * \brief Application specific startup code
   *
   * The StartApplication method is called at the start time specifed by Start
   * This method should be overridden by all or most application
   * subclasses.
   */
  virtual void StartApplication (void);

  /**
   * \brief Application specific shutdown code
   *
   * The StopApplication method is called at the stop time specifed by Stop
   * This method should be overridden by all or most application
   * subclasses.
   */
  virtual void StopApplication (void);

protected:
  virtual void DoDispose (void);
  void AddInciPacketTags (Ptr<Packet> p);
  void RetrieveInciPacketTags (Ptr<Packet> p);
  void InitializeINCIvariables(void);

  std::string     m_servicetype;
  std::string 	  m_composedServiceType;
  Ptr<c2cAddress> m_c2cAddress;  
  uint32_t        m_packetSize;
  double          m_frequency;
  double          m_MessRegenerationTime;
  uint8_t         m_MsgLifeTime;
  Ipv4Address     m_IPAddress;

  // inci variables for message tracking in the iCS
  uint32_t        m_currentTimeStep;
  uint32_t        m_previousTimeStep;
  uint32_t        m_stepSequenceNumber;
  uint32_t        m_app_index;
  iTETRISApplication::ReceiveCallback m_forwardIcs;

private:
  std::string m_mwServiceType;

};

} // namespace ns3

#endif   /* ITETRIS_APPLICATION_H  */