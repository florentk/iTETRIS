/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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

#include "mcs-tag-wifi-manager.h"
#include "ns3/mcs-tag.h"
#include "ns3/string.h"
#include "ns3/assert.h"

namespace ns3 {

McsTagWifiRemoteStation::McsTagWifiRemoteStation (Ptr<McsTagWifiManager> manager)
  : m_manager (manager)
{}

McsTagWifiRemoteStation::~McsTagWifiRemoteStation ()
{}

void 
McsTagWifiRemoteStation::DoReportRxOk (double rxSnr, WifiMode txMode)
{}
void 
McsTagWifiRemoteStation::DoReportRtsFailed (void)
{}
void 
McsTagWifiRemoteStation::DoReportDataFailed (void)
{}
void 
McsTagWifiRemoteStation::DoReportRtsOk (double ctsSnr, WifiMode ctsMode, double rtsSnr)
{}
void 
McsTagWifiRemoteStation::DoReportDataOk (double ackSnr, WifiMode ackMode, double dataSnr)
{
  m_packet = 0; 
}
void 
McsTagWifiRemoteStation::DoReportFinalRtsFailed (void)
{
  m_packet = 0; 
}
void 
McsTagWifiRemoteStation::DoReportFinalDataFailed (void)
{
  m_packet = 0; 
}

Ptr<WifiRemoteStationManager>
McsTagWifiRemoteStation::GetManager (void) const
{
  return m_manager;
}

WifiMode 
McsTagWifiRemoteStation::DoGetDataMode (uint32_t size)
{
  NS_ASSERT(GetManager ()->IsLowLatency ());
  McsTag tag;
  bool found;
  WifiMode mode;
  found = m_packet->PeekPacketTag (tag);
  if (found)
    {
      mode = GetWifiMode (tag.Get());
    }
  else
    {
      mode = m_manager->GetNonUnicastMode ();
    }
  return mode;
}

WifiMode 
McsTagWifiRemoteStation::DoGetRtsMode (void)
{
  NS_ASSERT(GetManager ()->IsLowLatency ());
  McsTag tag;
  bool found;
  WifiMode mode;
  found = m_packet->PeekPacketTag (tag);
  if (found)
    {
      mode = GetWifiMode (tag.Get());
    }
  else
    {
      mode = m_manager->GetNonUnicastMode ();
    }
  return mode;
}

WifiMode 
McsTagWifiRemoteStation::GetDataMode (Ptr<const Packet> packet, uint32_t fullPacketSize)
{
  NS_ASSERT(GetManager ()->IsLowLatency ());
  m_packet = packet;
  return DoGetDataMode (fullPacketSize);
}

WifiMode 
McsTagWifiRemoteStation::GetRtsMode (Ptr<const Packet> packet)
{
  NS_ASSERT(GetManager ()->IsLowLatency ());
  m_packet = packet;
  return DoGetRtsMode ();
}

WifiMode 
McsTagWifiRemoteStation::GetWifiMode (uint8_t mcs)
{
  switch (mcs) {
  case 0 :
    {
    WifiMode mode ("wifi-3mbs-10Mhz");
    return mode;
    break;
    }
  case 1 :
    {
    WifiMode mode ("wifi-4.5mbs-10Mhz");
    return mode;
    break;
    }
  case 2 :
    {
    WifiMode mode ("wifi-6mbs-10Mhz");
    return mode;
    break;
    }
  case 3 :
    {
    WifiMode mode ("wifi-9mbs-10Mhz");
    return mode;
    break;
    }
  case 4 :
    {
    WifiMode mode ("wifi-12mbs-10Mhz");
    return mode;
    break;
    }
  case 5 :
    {
    WifiMode mode ("wifi-18mbs-10Mhz");
    return mode;
    break;
    }
  case 6 :
    {
    WifiMode mode ("wifi-24mbs-10Mhz");
    return mode;
    break;
    }
  case 7 : 
    {
    WifiMode mode ("wifi-27mbs-10Mhz");
    return mode;
    break;
    }
  }
  WifiMode mode ("wifi-3mbs-10Mhz");
  return mode;
}


NS_OBJECT_ENSURE_REGISTERED (McsTagWifiManager);

TypeId 
McsTagWifiManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McsTagWifiManager")
    .SetParent<WifiRemoteStationManager> ()
    .AddConstructor<McsTagWifiManager> ();
  return tid;
}

McsTagWifiManager::McsTagWifiManager ()
{
  delete m_nonUnicast;
  m_nonUnicast = CreateStation ();
}

McsTagWifiManager::~McsTagWifiManager ()
{}

WifiRemoteStation *
McsTagWifiManager::CreateStation (void)
{
  return new McsTagWifiRemoteStation (this);
}

} // namespace ns3
